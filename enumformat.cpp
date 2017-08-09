#include "stdafx.h"
#include "enumformat.h"


OleEnumFormatEtc::OleEnumFormatEtc(LPFORMATETC pFormatEtc, int nNumFormats)
{
	AddRef();
	m_nNumFormats = nNumFormats;
	m_pFormatEtc = new FORMATETC[nNumFormats];

	// copy the FORMATETC structures
	for (int i = 0; i < nNumFormats; i++)
	{
		DeepCopyFormatEtc(&m_pFormatEtc[i], &pFormatEtc[i]);
	}
}


OleEnumFormatEtc::~OleEnumFormatEtc()
{
	if (m_pFormatEtc) {
		for (ULONG i = 0; i < m_nNumFormats; i++) {
			if (m_pFormatEtc[i].ptd) {
				CoTaskMemFree(m_pFormatEtc[i].ptd);
			}
		}
		delete[] m_pFormatEtc;
	}
}

HRESULT OleEnumFormatEtc::Create(LPFORMATETC pFormatEtc, UINT nNumFormats, LPENUMFORMATETC* ppEnumFormatEtc) {
	if (nNumFormats == 0 || pFormatEtc == 0 || ppEnumFormatEtc == 0)
		return E_INVALIDARG;

	*ppEnumFormatEtc = new OleEnumFormatEtc(pFormatEtc, nNumFormats);

	return (*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
}

void OleEnumFormatEtc::DeepCopyFormatEtc(LPFORMATETC dest, LPFORMATETC source) {
	// copy the source FORMATETC into dest
	*dest = *source;

	if (source->ptd) {
		// allocate memory for the DVTARGETDEVICE if necessary
		dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

		// copy the contents of the source DVTARGETDEVICE into dest->ptd
		*(dest->ptd) = *(source->ptd);
	}
}

// IUnknown methods

STDMETHODIMP OleEnumFormatEtc::QueryInterface(REFIID iid, LPVOID* ppvObject) {
	if (ppvObject == 0) return E_INVALIDARG;
	else if (iid == IID_IUnknown) {
		AddRef();
		*ppvObject = reinterpret_cast<LPUNKNOWN>(this);
		return S_OK;
	}
	else if (iid == IID_IEnumFORMATETC) {
		AddRef();
		*ppvObject = reinterpret_cast<LPENUMFORMATETC>(this);
		return S_OK;
	}

	*ppvObject = nullptr;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) OleEnumFormatEtc::AddRef(void)
{
	// increment object reference count
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) OleEnumFormatEtc::Release(void)
{
	// decrement object reference count
	auto count = InterlockedDecrement(&m_lRefCount);

	if (count == 0) {
		delete this;
	}

	return count;
}

STDMETHODIMP OleEnumFormatEtc::Next(ULONG celt, LPFORMATETC rgelt, ULONG* pceltFetched) {
	ULONG copied = 0;

	// validate arguments
	if (celt == 0 || rgelt == 0) {
		return E_INVALIDARG;
	}

	// copy FORMATETC structures into caller's buffer
	while (m_nIndex < m_nNumFormats && copied < celt) {
		DeepCopyFormatEtc(&rgelt[copied], &m_pFormatEtc[m_nIndex]);
		copied++;
		m_nIndex++;
	}

	// store result
	if (pceltFetched != 0) {
		*pceltFetched = copied;
	}
		
	// did we copy all that was requested?
	return (copied == celt) ? S_OK : S_FALSE;
}

STDMETHODIMP OleEnumFormatEtc::Skip(ULONG celt) {
	m_nIndex += celt;
	return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
}

STDMETHODIMP OleEnumFormatEtc::Reset(void) {
	m_nIndex = 0;
	return S_OK;
}

STDMETHODIMP OleEnumFormatEtc::Clone(LPENUMFORMATETC* ppEnumFormatEtc) {
	// make a duplicate enumerator
	auto hResult = Create(m_pFormatEtc, m_nNumFormats, ppEnumFormatEtc);

	if (hResult == S_OK) {
		// manually set the index state
		((OleEnumFormatEtc *)*ppEnumFormatEtc)->m_nIndex = m_nIndex;
	}

	return hResult;
}
