#pragma once

#include <windows.h>
#include <shellapi.h>
#include <oleidl.h>

#include <vector>
#include <string>

#include "../SPSCQueue.h"

using Queue = rigtorp::SPSCQueue<std::vector<std::string>>;

class DataObject : public IDataObject {
   private:
    LONG refCount;
    std::vector<FORMATETC> supportedFormats;
    std::vector<STGMEDIUM> storageMediums;
    std::shared_ptr<Queue> queue;

   public:
    DataObject(std::shared_ptr<Queue> queue);
    virtual ~DataObject();

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    // IDataObject methods
    STDMETHOD(GetData)(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium);
    STDMETHOD(GetDataHere)(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium);
    STDMETHOD(QueryGetData)(LPFORMATETC pFormatEtc);
    STDMETHOD(GetCanonicalFormatEtc)(LPFORMATETC pFormatEct, LPFORMATETC pFormatEtcOut);
    STDMETHOD(SetData)(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium, BOOL fRelease);
    STDMETHOD(EnumFormatEtc)(DWORD dwDirection, LPENUMFORMATETC* ppEnumFormatEtc);
    STDMETHOD(DAdvise)(LPFORMATETC pFormatEtc, DWORD advf, LPADVISESINK pAdvSink, LPDWORD pdwConnection);
    STDMETHOD(DUnadvise)(DWORD dwConnection);
    STDMETHOD(EnumDAdvise)(LPENUMSTATDATA* ppEnumAdvise);

    static HRESULT Create(LPFORMATETC pFmtetc,
                          LPSTGMEDIUM pStgmeds,
                          UINT count,
                          LPDATAOBJECT* ppDataObject);
};