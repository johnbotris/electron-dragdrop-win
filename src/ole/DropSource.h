#pragma once

// Won't compile without this because we get lots of redefinition errors from
// winsock stuff
#define WIN32_LEAN_AND_MEAN
#include <oleidl.h>
#undef WIN32_LEAN_AND_MEAN

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
