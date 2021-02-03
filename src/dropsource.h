#pragma once

#include <windows.h>

class Options;

class DropSource: public IDropSource, public IDropSourceNotify
{
private:
    Options opts;
	ULONG refCount = 0;
    v8::Local<v8::String> GetWindowText(HWND window);

public:
	DropSource(Options opts);
	~DropSource();

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDropSource methods
	STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState);
	STDMETHOD(GiveFeedback)(DWORD dwEffect);

    // IDropSourceNotify methods
    STDMETHOD(DragEnterTarget)(HWND hwndTarget);
    STDMETHOD(DragLeaveTarget)();
};
