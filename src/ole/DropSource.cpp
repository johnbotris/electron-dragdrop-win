#include "DropSource.h"

#include <nan.h>
#include <iostream>

using namespace std;

using v8::Local;
using v8::String;
using v8::Object;

DropSource::DropSource() : refCount(1) {}

DropSource::~DropSource() {}

/* * * IUnknown methods * * */
STDMETHODIMP DropSource::QueryInterface(REFIID iid, LPVOID* ppvObject) {
    if (ppvObject == 0)
        return E_INVALIDARG;
    else if (iid == IID_IUnknown) {
        AddRef();
        *ppvObject = reinterpret_cast<LPUNKNOWN>(this);
        return S_OK;
    } else if (iid == IID_IDropSource) {
        AddRef();
        *ppvObject = reinterpret_cast<LPDROPSOURCE>(this);
        return S_OK;
    } else if (iid == IID_IDropSourceNotify) {
        AddRef();
        *ppvObject = reinterpret_cast<IDropSourceNotify*>(this);
        return S_OK;
    }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) DropSource::AddRef() {
    return InterlockedIncrement(&this->refCount);
}

STDMETHODIMP_(ULONG) DropSource::Release() {
    auto count = InterlockedDecrement(&this->refCount);

    if (count == 0) {
        delete this;
    }

    return count;
}

/* * * IDropSource methods * * */
STDMETHODIMP DropSource::QueryContinueDrag(BOOL fEscapePressed,
                                           DWORD grfKeyState) {
    // Cancel if escape was pressed.
    if (fEscapePressed) {
        return DRAGDROP_S_CANCEL;
    } else if (grfKeyState & MK_RBUTTON) {
        // Cancel if right mouse button was pressed.
        return DRAGDROP_S_CANCEL;
    } else if (!(grfKeyState & MK_LBUTTON)) {
        // Drop if left mouse button was released.
        return DRAGDROP_S_DROP;
    } else {
        // Continue otherwise.
        return S_OK;
    }
}

STDMETHODIMP DropSource::GiveFeedback(DWORD dwEffect) {
    // Let OLE show the default cursor.
    return DRAGDROP_S_USEDEFAULTCURSORS;
}

/* * * IDropSourceNotify methods * * */
STDMETHODIMP DropSource::DragEnterTarget(HWND target) {
    cout << "drag enter" << endl;
    return S_OK;
}

STDMETHODIMP DropSource::DragLeaveTarget() {
    cout << "drag leave" << endl;
    return S_OK;
}
