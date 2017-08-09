#include "stdafx.h"
#include "dataobject.h"
#include "enumformat.h"

OleDataObject::OleDataObject(FORMATETC *fmtetc, STGMEDIUM *stgmed, int count)
{
	AddRef();
	m_nNumFormats = count;

	m_pFormatEtc = new FORMATETC[count];
	m_pStgMedium = new STGMEDIUM[count];

	for (int i = 0; i < count; i++)
	{
		m_pFormatEtc[i] = fmtetc[i];
		m_pStgMedium[i] = stgmed[i];
	}
}


OleDataObject::~OleDataObject()
{
	if (m_pFormatEtc) delete[] m_pFormatEtc;
	if (m_pStgMedium) delete[] m_pStgMedium;
}

HRESULT OleDataObject::Create(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium, UINT count, LPDATAOBJECT* ppDataObject) {
	if (ppDataObject == 0)
		return E_INVALIDARG;

	*ppDataObject = new OleDataObject(pFormatEtc, pMedium, count);

	return (*ppDataObject) ? S_OK : E_OUTOFMEMORY;
}

int OleDataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
{
	for (int i = 0; i < m_nNumFormats; i++)	{
		if ((pFormatEtc->tymed    &  m_pFormatEtc[i].tymed) &&
			pFormatEtc->cfFormat == m_pFormatEtc[i].cfFormat &&
			pFormatEtc->dwAspect == m_pFormatEtc[i].dwAspect) {
			return i;
		}
	}

	return -1;
}

HGLOBAL OleDataObject::DupMem(HGLOBAL hMem)
{
	// lock the source memory object
	auto len = GlobalSize(hMem);
	auto source = GlobalLock(hMem);

	// create a fixed "global" block - i.e. just
	// a regular lump of our process heap
	auto dest = GlobalAlloc(GMEM_FIXED, len);

	memcpy(dest, source, len);

	GlobalUnlock(hMem);

	return dest;
}

// IUnknown methods

STDMETHODIMP OleDataObject::QueryInterface(REFIID iid, LPVOID* ppvObject) {
	if (ppvObject == 0) return E_INVALIDARG;
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

STDMETHODIMP_(ULONG) OleDataObject::AddRef(void)
{
	// increment object reference count
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) OleDataObject::Release(void)
{
	// decrement object reference count
	auto count = InterlockedDecrement(&m_lRefCount);

	if (count == 0) {
		delete this;
	}

	return count;
}

// IDataObject methods

STDMETHODIMP OleDataObject::GetData(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium) {
	auto idx = LookupFormatEtc(pFormatEtc);

	//
	// try to match the requested FORMATETC with one of our supported formats
	//
	if (idx == -1) {
		return DV_E_FORMATETC;
	}

	//
	// found a match! transfer the data into the supplied storage-medium
	//
	pMedium->tymed = m_pFormatEtc[idx].tymed;
	pMedium->pUnkForRelease = 0;

	switch (m_pFormatEtc[idx].tymed) {
		case TYMED_HGLOBAL:
			pMedium->hGlobal = DupMem(m_pStgMedium[idx].hGlobal);
			return S_OK;

		default:
			return DV_E_FORMATETC;
	}
}

STDMETHODIMP OleDataObject::GetDataHere(LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium) {
	return DATA_E_FORMATETC;
}

STDMETHODIMP OleDataObject::QueryGetData(LPFORMATETC pFormatEtc) {
	return (LookupFormatEtc(pFormatEtc) == -1) ? DV_E_FORMATETC : S_OK;
}

STDMETHODIMP OleDataObject::GetCanonicalFormatEtc(LPFORMATETC pFormatEct, LPFORMATETC pFormatEtcOut) {
	pFormatEtcOut->ptd = nullptr;
	return E_NOTIMPL;
}

STDMETHODIMP OleDataObject::SetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease) {
	return E_NOTIMPL;
}

STDMETHODIMP OleDataObject::EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC* ppEnumFormatEtc) {
	if (dwDirection == DATADIR_GET)	{
		// for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however
		// to support all Windows platforms we need to implement IEnumFormatEtc ourselves.
		return OleEnumFormatEtc::Create(m_pFormatEtc, m_nNumFormats, ppEnumFormatEtc);
	}
	else
	{
		// the direction specified is not support for drag+drop
		return E_NOTIMPL;
	}
}

STDMETHODIMP OleDataObject::DAdvise(LPFORMATETC pFormatEtc, DWORD advf, LPADVISESINK pAdvSink, LPDWORD pdwConnection) {
	return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP OleDataObject::DUnadvise(DWORD dwConnection) {
	return OLE_E_ADVISENOTSUPPORTED;
}

STDMETHODIMP OleDataObject::EnumDAdvise(LPENUMSTATDATA* ppEnumAdvise) {
	return OLE_E_ADVISENOTSUPPORTED;
}
