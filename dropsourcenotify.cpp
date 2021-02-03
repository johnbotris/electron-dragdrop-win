#include "dropsourcenotify.h"

#include <iostream>
#include <sstream>

// IUnknown methods

using v8::Local;
using v8::String;

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

v8::Local<v8::String> OleDropSourceNotify::GetWindowText(HWND hwnd) {
	auto len = ::GetWindowTextLength(hwnd);
	LPSTR szText = (LPSTR)::malloc((len + 1) * sizeof(CHAR));
	::GetWindowTextA(hwnd, szText, len + 1);
	Local<String> text = Nan::New<String>(szText, len).ToLocalChecked();
	::free(szText);
	return text;
}


STDMETHODIMP OleDropSourceNotify::DragEnterTarget(HWND hwndTarget) {
	if (m_pOptions->GetDragEnterCallbackFunction()->IsFunction()) {
		const unsigned argc = 1;
		auto data = Nan::New<v8::Object>();
		auto windowText = GetWindowText(hwndTarget);
		data->Set(Nan::GetCurrentContext(), Nan::New("windowText").ToLocalChecked(), windowText);
		v8::Local<v8::Value> argv[argc] = { data };
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), m_pOptions->GetDragEnterCallbackFunction(), argc, argv);
	}
	return S_OK;
}

STDMETHODIMP OleDropSourceNotify::DragLeaveTarget() {
	if (m_pOptions->GetDragLeaveCallbackFunction()->IsFunction()) {
		const unsigned argc = 0;
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), m_pOptions->GetDragLeaveCallbackFunction(), argc, NULL);
	}
	return S_OK;
}
