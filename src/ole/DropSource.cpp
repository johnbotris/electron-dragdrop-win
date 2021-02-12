#include "DropSource.h"

#include <iostream>

#include "ole.h"

using namespace std;

DropSource::DropSource() : refCount(1) {}

DropSource::~DropSource() {}

/* * * IUnknown methods * * */
STDMETHODIMP DropSource::QueryInterface(REFIID iid, LPVOID* ppvObject) {
    if (ppvObject == 0)
        return E_INVALIDARG;
    else if (iid == IID_IUnknown) {
        cout << "DropSource::QueryInterface: IUnknown" << endl;
        AddRef();
        *ppvObject = reinterpret_cast<LPUNKNOWN>(this);
        return S_OK;
    } else if (iid == IID_IDropSource) {
        cout << "DropSource::QueryInterface: IDropSource" << endl;
        AddRef();
        *ppvObject = reinterpret_cast<LPDROPSOURCE>(this);
        return S_OK;
    } else if (iid == IID_IDropSourceNotify) {
        cout << "DropSource::QueryInterface: IDropSourceNotify" << endl;
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
    wcout << L"entering " << getWindowText(target) << endl;
    return S_OK;
}

STDMETHODIMP DropSource::DragLeaveTarget() {
    wcout << L"leaving" << endl;
    return S_OK;
}
