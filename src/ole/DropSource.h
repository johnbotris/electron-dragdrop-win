#pragma once

#include <windows.h>
#include <shellapi.h>
#include <oleidl.h>

class DropSource : public IDropSource, public IDropSourceNotify {
   private:
    ULONG refCount = 0;

   public:
    DropSource();
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