#pragma once

#include <windows.h>

class OleEnumFormatEtc: public IEnumFORMATETC
{
private:
	ULONG m_lRefCount = 0;
	ULONG m_nIndex = 0; // current enumerator index
	ULONG m_nNumFormats = 0; // number of FORMATETC members
	LPFORMATETC m_pFormatEtc = nullptr; // array of FORMATETC objects

public:
	OleEnumFormatEtc(LPFORMATETC pFormatEtc, int nNumFormats);
	~OleEnumFormatEtc();

	static HRESULT Create(LPFORMATETC pFormatEtc, UINT nNumFormats, LPENUMFORMATETC* ppEnumFormatEtc);
	static void DeepCopyFormatEtc(LPFORMATETC dest, LPFORMATETC source);

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IEnumFORMATETC methods
	STDMETHOD(Next)(ULONG celt, LPFORMATETC rgelt, ULONG* pceltFetched);
	STDMETHOD(Skip)(ULONG celt);
	STDMETHOD(Reset)(void);
	STDMETHOD(Clone)(LPENUMFORMATETC* ppEnumFormatEtc);
};

