#include "Stream.h"

#include <iostream>
#include <thread>

using namespace std;

Stream::Stream(vector<uint8_t> data, wstring name)
    : refCount(1), data(data), name(name), cursor(0) {}

Stream::~Stream() {}

STDMETHODIMP Stream::QueryInterface(REFIID iid, LPVOID* ppvObject) {
    if (ppvObject == nullptr) {
        return E_INVALIDARG;
    } else if (iid == IID_IUnknown) {
        cout << "Stream::QueryInterface: IUnknown" << endl;
        AddRef();
        *ppvObject = reinterpret_cast<IUnknown*>(this);
        return S_OK;
    } else if (iid == IID_IStream) {
        cout << "Stream::QueryInterface: IStream" << endl;
        AddRef();
        *ppvObject = reinterpret_cast<IStream*>(this);
        return S_OK;
    } else if (iid == IID_ISequentialStream) {
        cout << "Stream::QueryInterface: ISequentialStream" << endl;
        AddRef();
        *ppvObject = reinterpret_cast<ISequentialStream*>(this);
        return S_OK;
    }
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) Stream::AddRef(void) {
    auto count = InterlockedIncrement(&this->refCount);
    return count;
}

STDMETHODIMP_(ULONG) Stream::Release(void) {
    auto count = InterlockedDecrement(&this->refCount);

    if (count == 0) {
        delete this;
    }

    return count;
}

STDMETHODIMP Stream::Read(void* pv, ULONG cb, ULONG* pcbRead) {
    this_thread::sleep_for(2000ms);
    cout << "Stream::Read " << cb << " bytes" << endl;
    auto remaining = this->data.size() - this->cursor;
    auto amountToCopy = min(remaining, cb);

    if (memcpy_s(pv, cb, static_cast<void*>(&this->data[0]), amountToCopy) !=
        0) {
        return E_FAIL;
    };

    this->cursor += amountToCopy;
    *pcbRead = amountToCopy;

    return S_OK;
}

STDMETHODIMP Stream::Write(const void* pv, ULONG cb, ULONG* pcbWritten) {
    cout << "Stream::Write" << endl;

    return E_NOTIMPL;
}

STDMETHODIMP Stream::Seek(LARGE_INTEGER dlibMove,
                          DWORD dwOrigin,
                          ULARGE_INTEGER* plibNewPosition) {
    LONGLONG amount = dlibMove.QuadPart;
    // MSDN says to interpret dLibMove as unsigned in case of SEEK_SET
    ULONGLONG uamount = static_cast<ULONGLONG>(amount);

    cout << "Stream::Seek: ";
    switch (dwOrigin) {
        case STREAM_SEEK_SET:
            // seek from beginning
            cout << uamount << " from beginning" << endl;
            this->cursor = min(amount, this->data.size() - 1);
            break;
        case STREAM_SEEK_CUR:
            // seek from current location
            cout << amount << " from current position" << endl;
            this->cursor =
                max(0, min(this->data.size() - 1, this->cursor + amount));
            break;
        case STREAM_SEEK_END:
            // seek backwards from the end
            cout << amount << " from the end" << endl;
            // TODO would this generally give a negative number or a positive
            // number?
            // abs just in case
            this->cursor = max(this->data.size() - abs(amount), 0);
            break;
    }

    if (plibNewPosition) {
        plibNewPosition->QuadPart = this->cursor;
    }

    return S_OK;
}

STDMETHODIMP Stream::CopyTo(IStream* pstm,
                            ULARGE_INTEGER cb,
                            ULARGE_INTEGER* pcbRead,
                            ULARGE_INTEGER* pcbWritten) {
    this_thread::sleep_for(2000ms);
    cout << "Stream::CopyTo" << endl;
    ULONG written;
    auto result = pstm->Write(this->data.data() + this->cursor,
                       min(cb.QuadPart, this->data.size() - this->cursor),
                       &written);
    if (pcbRead)
        pcbRead->QuadPart = written;

    if (pcbWritten)
        pcbWritten->QuadPart = written;

    return result;
}

STDMETHODIMP Stream::Stat(STATSTG* pstatsg, DWORD grfStatFlag) {
    cout << "Stream::Stat" << endl;

    if ((grfStatFlag & STATFLAG_NONAME) == 0) {
        pstatsg->pwcsName = static_cast<LPOLESTR>(
            CoTaskMemAlloc(sizeof(WCHAR) * (this->name.size() + 1)));
        wcscpy(pstatsg->pwcsName, this->name.c_str());
    }

    pstatsg->type = STGTY_STREAM;
    pstatsg->cbSize.QuadPart = this->data.size();
    pstatsg->mtime = {0, 0};
    pstatsg->ctime = {0, 0};
    pstatsg->atime = {0, 0};
    pstatsg->grfMode = STGM_READ;
    pstatsg->grfLocksSupported = 0;
    pstatsg->clsid = CLSID_NULL;  // not used
    pstatsg->grfStateBits = 0;    // not used
    pstatsg->reserved;            // not used
    return S_OK;
}
// Garabage VVV

STDMETHODIMP Stream::Clone(IStream** ppstm) {
    cout << "Stream::Clone" << endl;
    return E_NOTIMPL;
}
STDMETHODIMP Stream::Commit(DWORD grfCommitFlags) {
    cout << "Stream::Commit" << endl;
    return E_NOTIMPL;
}
STDMETHODIMP Stream::LockRegion(ULARGE_INTEGER libOffset,
                                ULARGE_INTEGER cb,
                                DWORD dwLockType) {
    cout << "Stream::LockRegion" << endl;
    return E_NOTIMPL;
}
STDMETHODIMP Stream::Revert() {
    cout << "Stream::Revert" << endl;
    return E_NOTIMPL;
}
STDMETHODIMP Stream::SetSize(ULARGE_INTEGER libNewSize) {
    cout << "Stream::SetSize" << endl;
    return E_NOTIMPL;
}

STDMETHODIMP Stream::UnlockRegion(ULARGE_INTEGER libOffset,
                                  ULARGE_INTEGER cb,
                                  DWORD dwLockType) {
    cout << "Stream::UnlockRegion" << endl;
    return E_NOTIMPL;
}
