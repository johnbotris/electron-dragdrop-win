#include "DataObject.h"

#include <windows.h>

#include <iostream>
#include <iomanip>

#include "wil/resource.h"

#include "EnumFormat.h"
#include "ole.h"

const UINT numFiles = 1;
const WCHAR* textData = L"i have no fucking idea if this will work";
const SIZE_T fileSize = 128;

using namespace std;

DataObject::DataObject(STGMEDIUM groupDescriptorStorage,
                       vector<pair<FILEDESCRIPTOR, STGMEDIUM>> files)
    : refCount(1),
      inDragLoop(false),
      asyncOperationStarted(false),
      groupDescriptorStorage(groupDescriptorStorage),
      files(files) {
    // Create a supported format for each file provided + The
    // fileGroupDescriptor
    // Only do this so it can be passed to EnumFormat in
    // DataObject::EnumFormatEtc
    // Not sure if i need to be provide one per file or just one for the
    // fileGroupDescriptor
    this->numFormats = 1;  // 1 + this->files.size();

    auto formats = new FORMATETC[this->numFormats];

    // FILEGROUPDESCRIPTOR
    formats[0].cfFormat = CF_FILEDESCRIPTOR;
    formats[0].ptd = nullptr;
    formats[0].dwAspect = DVASPECT_CONTENT;
    formats[0].lindex = -1;
    formats[0].tymed = TYMED_HGLOBAL;

    // for (auto i = 1; i < this->numFormats; ++i) {
    //     formats[i].cfFormat = CF_FILECONTENTS;
    //     formats[i].ptd = nullptr;
    //     formats[i].dwAspect = DVASPECT_CONTENT;
    //     formats[i].lindex = i;
    //     formats[i].tymed = files[i].second.tymed;
    // }
    this->supportedFormats = formats;
}

DataObject::~DataObject() {
    for (auto file : this->files) {
        ReleaseStgMedium(&file.second);
    }

    ReleaseStgMedium(&this->groupDescriptorStorage);
    delete[] this->supportedFormats;
}

void DataObject::SetInDragLoop(bool inDragLoop) {
    this->inDragLoop = inDragLoop;
}

STDMETHODIMP DataObject::QueryInterface(REFIID iid, LPVOID* ppvObject) {
    if (ppvObject == nullptr)
        return E_INVALIDARG;
    else if (iid == IID_IUnknown) {
        cout << "DataObject::QueryInterface: IUnknown" << endl;
        AddRef();
        *ppvObject = reinterpret_cast<LPUNKNOWN>(this);
        return S_OK;
    } else if (iid == IID_IDataObject) {
        cout << "DataObject::QueryInterface: IDataObject" << endl;
        AddRef();
        *ppvObject = reinterpret_cast<LPDATAOBJECT>(this);
        return S_OK;
    } else if (iid == IID_IDataObjectAsyncCapability) {
        cout << "DataObject::QueryInterface: IDataObjectAsyncCapability"
             << endl;
        AddRef();
        *ppvObject = reinterpret_cast<IDataObjectAsyncCapability*>(this);
        return S_OK;
    }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DataObject::AddRef(void) {
    auto count = InterlockedIncrement(&this->refCount);
    return count;
}

STDMETHODIMP_(ULONG) DataObject::Release(void) {
    auto count = InterlockedDecrement(&this->refCount);

    if (count == 0) {
        delete this;
    }

    return count;
}

STDMETHODIMP DataObject::SetData(LPFORMATETC pFormatEtc,
                                 LPSTGMEDIUM pMedium,
                                 BOOL fRelease) {
    return E_NOTIMPL;
}

STDMETHODIMP DataObject::GetData(LPFORMATETC queryFormat,
                                 LPSTGMEDIUM outputStorage) {
    wcout << L"DataObject::GetData:" << *queryFormat << endl;

    if (queryFormat->cfFormat == CF_FILEDESCRIPTOR &&
        (queryFormat->tymed & TYMED_HGLOBAL) && queryFormat->lindex <= 0 &&
        queryFormat->dwAspect == DVASPECT_CONTENT) {
        // wcout << L"DataObject::GetData:" << *queryFormat << endl;
        outputStorage->tymed = TYMED_HGLOBAL;
        outputStorage->pUnkForRelease = nullptr;
        outputStorage->hGlobal =
            lockAndDuplicateHGLOBAL(this->groupDescriptorStorage.hGlobal);
        return S_OK;
    } else if (queryFormat->cfFormat == CF_FILECONTENTS &&
               (queryFormat->tymed & TYMED_ISTREAM) &&
               queryFormat->dwAspect == DVASPECT_CONTENT &&
               queryFormat->lindex >= 0 &&
               queryFormat->lindex < this->files.size()) {
        auto file = this->files[queryFormat->lindex].second;
        *outputStorage = file;

        // if (this->asyncOperationStarted) {
        // } else {
        //     outputStorage->pUnkForRelease = nullptr;
        //     return DV_E_FORMATETC;
        // }

        return S_OK;
    }
    return DV_E_FORMATETC;
}

STDMETHODIMP DataObject::QueryGetData(LPFORMATETC queryFormat) {
    if (queryFormat->cfFormat == CF_FILEDESCRIPTOR ||
        (queryFormat->cfFormat == CF_FILECONTENTS &&
         (queryFormat->tymed & TYMED_HGLOBAL))) {
        return S_OK;
    }

    return DATA_E_FORMATETC;
}

STDMETHODIMP DataObject::GetDataHere(LPFORMATETC queryFormat,
                                     LPSTGMEDIUM outputMedium) {
    wcout << L"DataObject::GetData:" << *queryFormat << endl;

    return DATA_E_FORMATETC;
}

STDMETHODIMP DataObject::GetCanonicalFormatEtc(LPFORMATETC pFormatEtcIn,
                                               LPFORMATETC pFormatEtcOut) {
    memcpy(pFormatEtcOut, pFormatEtcIn, sizeof(FORMATETC));
    pFormatEtcOut->ptd = nullptr;
    return DATA_S_SAMEFORMATETC;
}

STDMETHODIMP DataObject::EnumFormatEtc(DWORD dwDirection,
                                       LPENUMFORMATETC* ppEnumFormatEtc) {
    if (dwDirection == DATADIR_GET) {
        return EnumFormat::Create(this->supportedFormats, this->numFormats,
                                  ppEnumFormatEtc);
    }
    return DATA_E_FORMATETC;
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

STDMETHODIMP DataObject::StartOperation(IBindCtx* pbcReserved) {
    cout << "DataObject::StartOperation" << endl;
    this->asyncOperationStarted = true;
    return S_OK;
}

STDMETHODIMP DataObject::InOperation(BOOL* pfInAsyncOp) {
    cout << "DataObject::InOperation" << endl;
    *pfInAsyncOp = this->asyncOperationStarted;
    return S_OK;
}

STDMETHODIMP DataObject::EndOperation(HRESULT hResult,
                                      IBindCtx* pbcReserved,
                                      DWORD dwEffects) {
    cout << "DataObject::EndOperation" << endl;
    // CLEANUP
    return S_OK;
}

STDMETHODIMP DataObject::GetAsyncMode(BOOL* pfIsOpAsync) {
    cout << "DataObject::GetAsyncMode" << endl;
    *pfIsOpAsync = VARIANT_TRUE;
    return S_OK;
}

STDMETHODIMP DataObject::SetAsyncMode(BOOL fDoOpAsync) {
    return E_NOTIMPL;
}
