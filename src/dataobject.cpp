#include "dataobject.h"
#include "options.h"
#include "utils.h"

DataObject::DataObject(Options &opts)
    : refCount(0), formatEtcs(), stgMediums()
{
	AddRef();



    // TODO Fill in formats with opts
    for (uint32_t i = 0; i < propertyCount; i++, idx++)
    {
        Local<Value> propertyName = Nan::Get(propertyNames, i).ToLocalChecked();
        std::string propertyNameString = *String::Utf8Value(isolate, propertyName);
        Local<Value> propertyValue = Nan::Get(formats, propertyName).ToLocalChecked();
        Local<Context> context = Nan::GetCurrentContext();

        if (!propertyValue.isEmpty()) {

            if (propertyValue->IsString() && propertyNameString == "text")
            {
                this->formatEtcs.push({CF_TEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL});
                this->stgMediums.push({TYMED_HGLOBAL, {nullptr}, nullptr});
                this->stgMediums.back().hGlobal = copyAsciiToHGlobal(propertyValue->ToString(context).ToLocalChecked());

                idx++;
                this->formatEtcs.push({CF_UNICODETEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL});
                this->stgMediums.push({TYMED_HGLOBAL, {nullptr}, nullptr});
                this->stgMediums.back().hGlobal = copyUnicodeToHGlobal(propertyValue->ToString(context).ToLocalChecked());
            }
            else 
            {
                auto clipboardFormat = (CLIPFORMAT)::RegisterClipboardFormatA(propertyNameString.data());

                this->formatEtcs.push({clipboardFormat, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL});
                this->stgMediums.push({TYMED_HGLOBAL, {nullptr}, nullptr});
                this->stgMediums.back().hGlobal = copyCustomToHGlobal(propertyValue);
            }
        }
    }
}


DataObject::~DataObject()
{
    // TODO
    // if (m_pFmtEtc != nullptr)
    // {
    //     delete[] m_pFmtEtc;
    //     m_pFmtEtc = nullptr;
    // }
    // if (m_pStgMed != nullptr)
    // {
    //     for (uint32_t i = 0; i < m_count; i++)
    //     {
    //         STGMEDIUM stgmed = m_pStgMed[i];
    //         if (stgmed.tymed == TYMED_HGLOBAL)
    //         {
    //             auto size = ::GlobalSize(stgmed.hGlobal);
    //             ::ZeroMemory(stgmed.hGlobal, size);
    //             ::GlobalFree(stgmed.hGlobal);
    //         }
    //     }
    //     delete[] m_pStgMed;
    //     m_pStgMed = nullptr;
    // }
}

// IUnknown methods

STDMETHODIMP DataObject::QueryInterface(REFIID iid, LPVOID* ppvObject) {
	if (ppvObject == nullptr) return E_INVALIDARG;
	else if (iid == IID_IUnknown) {
		AddRef();
		*ppvObject = reinterpret_cast<LPUNKNOWN>(this);
		return S_OK;
	}
	else if (iid == IID_IDataObject) {
		AddRef();
		*ppvObject = reinterpret_cast<LPDATAOBJECT>(this);
		return S_OK;
	}

	*ppvObject = nullptr;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DataObject::AddRef(void)
{
	// increment object reference count
	return InterlockedIncrement(&this->refCount);
}

STDMETHODIMP_(ULONG) DataObject::Release(void)
{
	// decrement object reference count
    auto count = InterlockedDecrement(&this->refCount);

    if (count == 0) {
		delete this;
	}

	return count;
}

// IDataObject methods
STDMETHODIMP DataObject::GetData(LPFORMATETC queryFormat, LPSTGMEDIUM outputMedium) {
    
    for (size_t i = 0; i < this->formatEtcs.size(); ++i)
    {
        const auto formatEtc = this->formatEtcs[i];
        const auto stgMedium = this->stgMediums[i];

        if (compareFormats(*queryFormat, formatEtc))
        {
            //
            // found a match! transfer the data into the supplied storage-medium
            //
            outputMedium->tymed = formatEtc.tymed;
            outputMedium->pUnkForRelease = nullptr;

            switch (formatEtc.tymed)
            {
            case TYMED_HGLOBAL:
                outputMedium->hGlobal = lockAndDuplicateMemory(stgMedium.hGlobal);
                return S_OK;
            default:
                return DV_E_FORMATETC;
            }
        }
    }
    return DV_E_FORMATETC;
}

STDMETHODIMP DataObject::GetDataHere(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium) {
    // TODO ?
	return DATA_E_FORMATETC;
}

STDMETHODIMP DataObject::QueryGetData(LPFORMATETC queryFormat) {
    for (auto formatEtc : this->formatEtcs) {
        if (compareFormats(*queryFormat, formatEtc)) {
            return S_OK;
        }
    }
    return DV_E_FORMATETC;
}

STDMETHODIMP DataObject::GetCanonicalFormatEtc(LPFORMATETC pFormatEct, LPFORMATETC pFormatEtcOut) {
	pFormatEtcOut->ptd = nullptr;
	return E_NOTIMPL;
}

STDMETHODIMP DataObject::SetData(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium, BOOL fRelease) {
	return E_NOTIMPL;
}

STDMETHODIMP DataObject::EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC* ppEnumFormatEtc) {
	if (dwDirection == DATADIR_GET)	{
		// for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however
		// to support all Windows platforms we need to implement IEnumFormatEtc ourselves.
        return SHCreateStdEnumFmtEtc(this->formats.size(), formatEtcs, ppEnumFormatEtc); // TODO
	}
	else
	{
		// the direction specified is not support for drag+drop
		return E_NOTIMPL;
	}
}

STDMETHODIMP DataObject::DAdvise(LPFORMATETC pFormatEtc, DWORD advf, LPADVISESINK pAdvSink, LPDWORD pdwConnection) {
	return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::DUnadvise(DWORD dwConnection) {
	return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::EnumDAdvise(LPENUMSTATDATA* ppEnumAdvise) {
	return OLE_E_ADVISENOTSUPPORTED;
}