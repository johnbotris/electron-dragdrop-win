#include "stdafx.h"
#include "dropsourcenotify.h"

#include <iostream>
#include <sstream>

OleDropSourceNotify::OleDropSourceNotify()
{
	AddRef();
}


OleDropSourceNotify::~OleDropSourceNotify()
{
}

HRESULT OleDropSourceNotify::Create(IDropSourceNotify** ppDropSourceNotify)
{
	if (ppDropSourceNotify == nullptr)
		return E_INVALIDARG;

	*ppDropSourceNotify = new OleDropSourceNotify();

	return (*ppDropSourceNotify) ? S_OK : E_OUTOFMEMORY;

}

// IUnknown methods

STDMETHODIMP OleDropSourceNotify::QueryInterface(REFIID iid, LPVOID* ppvObject) {
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
		AddRef();
		*ppvObject = reinterpret_cast<IDropSourceNotify*>(this);
		return S_OK;
	}

	*ppvObject = nullptr;
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) OleDropSourceNotify::AddRef() {
	return InterlockedIncrement(&m_lRefCount);
}

STDMETHODIMP_(ULONG) OleDropSourceNotify::Release() {
	auto count = InterlockedDecrement(&m_lRefCount);

	if (count == 0)
	{
		delete this;
	}

	return count;
}

// IDropSourceNotify methods

STDMETHODIMP OleDropSourceNotify::DragEnterTarget(HWND hwndTarget) {
	::OutputDebugStringW(L"DragEnterTarget\n");
	std::wostringstream w;
	w << hwndTarget;
	std::wcout << L"DragEnterTarget\n" << w.str() << std::endl;
	return S_OK;
}

STDMETHODIMP OleDropSourceNotify::DragLeaveTarget() {
	::OutputDebugStringW(L"DragLeaveTarget\n");
	std::wcout << L"DragLeaveTarget\n" << std::endl;
	return S_OK;
}
