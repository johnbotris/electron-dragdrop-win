#include "DataObject.h"
#include "oleutils.h"

#include <iostream>
#include <windows.h>
#include <ShlObj_core.h>

using namespace std;

DataObject::DataObject(shared_ptr<Queue> queue)
    : refCount(1), supportedFormats(), storageMediums(), queue(queue) {
    this->supportedFormats.push_back(
        makeFormatEtc(CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL));

    // HGLOBAL data = makeDropFiles(filePaths);
    // this->storageMediums.push_back(makeStorageMedium(data));
}

DataObject::~DataObject() {
    for (auto storage : this->storageMediums) {
        if (storage.tymed == TYMED_HGLOBAL) {
            auto size = ::GlobalSize(storage.hGlobal);
            ZeroMemory(storage.hGlobal, size);
            GlobalFree(storage.hGlobal);
        }
    }
}

// IUnknown methods

STDMETHODIMP DataObject::QueryInterface(REFIID iid, LPVOID* ppvObject) {
    cout << "QueryInterface" << endl;
    if (ppvObject == nullptr)
        return E_INVALIDARG;
    else if (iid == IID_IUnknown) {
        AddRef();
        *ppvObject = reinterpret_cast<LPUNKNOWN>(this);
        return S_OK;
    } else if (iid == IID_IDataObject) {
        AddRef();
        *ppvObject = reinterpret_cast<LPDATAOBJECT>(this);
        return S_OK;
    }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DataObject::AddRef(void) {
    // increment object reference count
    return InterlockedIncrement(&this->refCount);
}

STDMETHODIMP_(ULONG) DataObject::Release(void) {
    // decrement object reference count
    auto count = InterlockedDecrement(&this->refCount);

    if (count == 0) {
        delete this;
    }

    return count;
}

// IDataObject methods
STDMETHODIMP DataObject::GetData(LPFORMATETC queryFormat,
                                 LPSTGMEDIUM outputStorage) {
    cout << "GetData" << endl;
    for (size_t i = 0; i < this->supportedFormats.size(); ++i) {
        const auto supportedFormat = this->supportedFormats[i];
        // const auto storage = this->storageMediums[i];

        if (compareFormats(*queryFormat, supportedFormat)) {
            cout << "Data match!" << endl;
            //
            // found a match! transfer the data into the supplied
            // storage medium
            // outputStorage->tymed = supportedFormat.tymed;
            // outputStorage->pUnkForRelease = nullptr;

            // switch (supportedFormat.tymed) {
            //     case TYMED_HGLOBAL:
            //         outputStorage->hGlobal =
            //             lockAndDuplicateMemory(storage.hGlobal);
            //         return S_OK;
            //     default:
            //         return DV_E_FORMATETC;
            // }
        }
    }

    return DV_E_FORMATETC;
}

STDMETHODIMP DataObject::GetDataHere(LPFORMATETC pFormatEtc,
                                     LPSTGMEDIUM pMedium) {
    // TODO ?
    return DATA_E_FORMATETC;
}

STDMETHODIMP DataObject::QueryGetData(LPFORMATETC queryFormat) {
    cout << "QueryGetData" << endl;
    for (auto format : this->supportedFormats) {
        if (compareFormats(format, *queryFormat)) {
            cout << "Supported" << endl;
            return S_OK;
        }
    }
    cout << "Unsupported" << endl;
    return DV_E_FORMATETC;
}

STDMETHODIMP DataObject::GetCanonicalFormatEtc(LPFORMATETC pFormatEct,
                                               LPFORMATETC pFormatEtcOut) {
    pFormatEtcOut->ptd = nullptr;
    return E_NOTIMPL;
}

STDMETHODIMP DataObject::SetData(LPFORMATETC pFormatEtc,
                                 LPSTGMEDIUM pMedium,
                                 BOOL fRelease) {
    return E_NOTIMPL;
}

STDMETHODIMP DataObject::EnumFormatEtc(DWORD dwDirection,
                                       LPENUMFORMATETC* ppEnumFormatEtc) {
    if (dwDirection == DATADIR_GET) {
        auto count = this->supportedFormats.size();
        return SHCreateStdEnumFmtEtc(
            count, count > 0 ? &this->supportedFormats[0] : nullptr,
            ppEnumFormatEtc);
    }

    // the direction specified is not support for drag+drop
    return E_NOTIMPL;
}

STDMETHODIMP DataObject::DAdvise(LPFORMATETC pFormatEtc,
                                 DWORD advf,
                                 LPADVISESINK pAdvSink,
                                 LPDWORD pdwConnection) {
    return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::DUnadvise(DWORD dwConnection) {
    return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP DataObject::EnumDAdvise(LPENUMSTATDATA* ppEnumAdvise) {
    return OLE_E_ADVISENOTSUPPORTED;
}