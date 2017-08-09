#include "stdafx.h"
#include "dropsource.h"


OleDropSource::OleDropSource(IDropSourceNotify* pDropSourceNotify)
{
	AddRef();
	m_pDropSourceNotify = pDropSourceNotify;
}


OleDropSource::~OleDropSource()
{
}

HRESULT OleDropSource::Create(IDropSourceNotify* pDropSourceNotify, IDropSource **ppDropSource) {
	if (pDropSourceNotify == nullptr || ppDropSource == nullptr)
		return E_INVALIDARG;

	*ppDropSource = new OleDropSource(pDropSourceNotify);

	return (*ppDropSource) ? S_OK : E_OUTOFMEMORY;
}
// IUnknown methods

STDMETHODIMP OleDropSource::QueryInterface(REFIID iid, LPVOID* ppvObject) {
	if (ppvObject == 0) return E_INVALIDARG;
	else if (iid == IID_IUnknown) {
		AddRef();
		*ppvObject = reinterpret_cast<LPUNKNOWN>(this);
		return S_OK;
	}
	else if (iid == IID_IDropSource) {
		AddRef();
		*ppvObject = reinterpret_cast<LPDROPSOURCE>(this);
		return S_OK;
	}
	else if (iid == IID_IDropSourceNotify) {
		return m_pDropSourceNotify->QueryInterface(iid, ppvObject);
		//AddRef();
		//spDropSourceNotify.QueryInterface
		//*ppvObject = reinterpret_cast<IDropSourceNotify*>(this);
		//return S_OK;
	}
	
	*ppvObject = nullptr;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) OleDropSource::AddRef() {
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) OleDropSource::Release() {
	auto count = InterlockedDecrement(&m_lRefCount);

	if (count == 0)
	{
		delete this;
	}

	return count;
}

// IDropSource methods

STDMETHODIMP OleDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState) {
	// Cancel if escape was pressed.
	if (fEscapePressed) {
		return DRAGDROP_S_CANCEL;
	}
	else if (grfKeyState & MK_RBUTTON) {
		// Cancel if right mouse button was pressed.
		return DRAGDROP_S_CANCEL;
	}
	else if (!(grfKeyState & MK_LBUTTON)) {
		// Drop if left mouse button was released.
		return DRAGDROP_S_DROP;
	}
	else {
		// Continue otherwise.
		return S_OK;
	}
}

STDMETHODIMP OleDropSource::GiveFeedback(DWORD dwEffect) {
	// Let OLE show the default cursor.
	return DRAGDROP_S_USEDEFAULTCURSORS;
}
