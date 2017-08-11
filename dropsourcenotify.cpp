#include "dropsourcenotify.h"

#include <iostream>
#include <sstream>

OleDropSourceNotify::OleDropSourceNotify(Options* pOptions)
{
	m_pOptions = pOptions;
	AddRef();
}


OleDropSourceNotify::~OleDropSourceNotify()
{
}

HRESULT OleDropSourceNotify::Create(Options* pOptions, IDropSourceNotify** ppDropSourceNotify)
{
	if (ppDropSourceNotify == nullptr)
		return E_INVALIDARG;

	*ppDropSourceNotify = new OleDropSourceNotify(pOptions);

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

v8::Local<v8::String> OleDropSourceNotify::GetWindowText(HWND hwnd) {
	auto len = ::GetWindowTextLength(hwnd);
	LPSTR szText = (LPSTR)malloc(len + 1);
	::GetWindowText(hwnd, szText, len + 1);
	auto text = Nan::New(szText).ToLocalChecked();
	free(szText);
	return text;
}


STDMETHODIMP OleDropSourceNotify::DragEnterTarget(HWND hwndTarget) {
	auto cb = m_pOptions->GetDragEnterCallbackFunction();
	if (!cb.IsEmpty()) {
		const unsigned argc = 1;
		auto data = Nan::New<v8::Object>();
		data->Set(Nan::New("windowHandle").ToLocalChecked(), Nan::New(hwndTarget));
		auto windowText = GetWindowText(hwndTarget);
		data->Set(Nan::New("windowText").ToLocalChecked(), windowText);
		// Need to iterate through all parent windows and add to an array

		//HWND hwndTopWindow = ::GetTopWindow(hwndTarget);
		//data->Set(Nan::New("topWindowHandle").ToLocalChecked(), Nan::New(hwndTopWindow));
		//auto topWindowText = GetWindowText(hwndTopWindow);
		//data->Set(Nan::New("topWindowText").ToLocalChecked(), topWindowText);
		v8::Local<v8::Value> argv[argc] = { data };
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
	}
	return S_OK;
}

STDMETHODIMP OleDropSourceNotify::DragLeaveTarget() {
	auto cb = m_pOptions->GetDragLeaveCallbackFunction();
	if (!cb.IsEmpty()) {
		const unsigned argc = 0;
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, NULL);
	}
	return S_OK;
}
