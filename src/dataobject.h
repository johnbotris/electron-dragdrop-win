#pragma once

#include <vector>
#include <utility>

#include <windows.h>

class Options;


class DataObject : public IDataObject
{
private:
	LONG refCount = 0;
    std::vector<FORMATETC> formatEtcs;
    std::vector<STGMEDIUM> stgMediums;

public:
	DataObject(Options &opts);
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

	static HRESULT Create(LPFORMATETC pFmtetc, LPSTGMEDIUM pStgmeds, UINT count, LPDATAOBJECT* ppDataObject);
};

