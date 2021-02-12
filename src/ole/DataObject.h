#pragma once

// for IDataObjectAsyncCapability, also includes windows.h
#include <shldisp.h>
#include <shlobj_core.h>

#include <vector>
#include <string>
#include <utility>

#include "../SPSCQueue.h"

using Queue = rigtorp::SPSCQueue<std::vector<std::string>>;

class DataObject : public IDataObject, public IDataObjectAsyncCapability {
   private:
    LONG refCount;
    bool inDragLoop;
    bool asyncOperationStarted;
    STGMEDIUM groupDescriptorStorage;
    std::vector<std::pair<FILEDESCRIPTOR, STGMEDIUM>> files;
    size_t numFormats;
    LPFORMATETC supportedFormats;

   public:
    DataObject(STGMEDIUM fileGroupDescriptor,
               std::vector<std::pair<FILEDESCRIPTOR, STGMEDIUM>> files);

    virtual ~DataObject();

    void SetInDragLoop(bool inDragLoop);

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    // IDataObject methods
    STDMETHOD(SetData)(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium, BOOL fRelease);
    STDMETHOD(GetData)(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium);
    STDMETHOD(GetDataHere)(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium);
    STDMETHOD(QueryGetData)(LPFORMATETC pFormatEtc);
    STDMETHOD(GetCanonicalFormatEtc)(LPFORMATETC pFormatEct, LPFORMATETC pFormatEtcOut);
    STDMETHOD(EnumFormatEtc)(DWORD dwDirection, LPENUMFORMATETC* ppEnumFormatEtc);
    STDMETHOD(DAdvise)(LPFORMATETC pFormatEtc, DWORD advf, LPADVISESINK pAdvSink, LPDWORD pdwConnection);
    STDMETHOD(DUnadvise)(DWORD dwConnection);
    STDMETHOD(EnumDAdvise)(LPENUMSTATDATA* ppEnumAdvise);

    static HRESULT Create(LPFORMATETC pFmtetc,
                          LPSTGMEDIUM pStgmeds,
                          UINT count,
                          LPDATAOBJECT* ppDataObject);

    // IDataObjectAsyncCapability methods
    STDMETHOD(EndOperation)(HRESULT hResult, IBindCtx *pbcReserved, DWORD dwEffects);
    STDMETHOD(GetAsyncMode)(BOOL* pfIsOpAsync);
    STDMETHOD(InOperation)(BOOL* pfInAsyncOp);
    STDMETHOD(SetAsyncMode)(BOOL fDoOpAsync);
    STDMETHOD(StartOperation)(IBindCtx* pbcReserved);
};