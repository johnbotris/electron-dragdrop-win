#pragma once

#include <v8.h>
#include <nan.h>
#include "options.h"
#include "windows.h"

class OleDropSourceNotify: public IDropSourceNotify
{
private:
	Options* m_pOptions = nullptr;
	ULONG m_lRefCount = 0;
	v8::Local<v8::String> GetWindowText(HWND hwnd);

public:
	OleDropSourceNotify(Options* pOptions) {
		m_pOptions = pOptions;
		AddRef();
	}

	~OleDropSourceNotify() {}

	static HRESULT Create(Options* pOptions, IDropSourceNotify** ppDropSourceNotify) {
		if (ppDropSourceNotify == nullptr)
			return E_INVALIDARG;

		*ppDropSourceNotify = new OleDropSourceNotify(pOptions);

		return (*ppDropSourceNotify) ? S_OK : E_OUTOFMEMORY;
	}

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDropSourceNotify methods
	STDMETHOD(DragEnterTarget)(HWND hwndTarget);
	STDMETHOD(DragLeaveTarget)();
};

