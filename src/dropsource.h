#pragma once

#include "options.h"

class DropSource : public IDropSource, public IDropSourceNotify {
   private:
    Options opts;
    ULONG refCount = 0;

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
