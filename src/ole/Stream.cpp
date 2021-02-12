#include "Stream.h"

#include <iostream>

using namespace std;

Stream::Stream(vector<uint8_t> data) : refCount(1), data(data), cursor(0) {}

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
    *pcbRead = 0;
    return E_PENDING;

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
    return E_NOTIMPL;
}

STDMETHODIMP Stream::Seek(LARGE_INTEGER dlibMove,
                          DWORD dwOrigin,
                          ULARGE_INTEGER* plibNewPosition) {
    switch (dwOrigin) {
        case STREAM_SEEK_SET:
            return S_OK;
        case STREAM_SEEK_CUR:
            return S_OK;
        case STREAM_SEEK_END:
            return S_OK;
    }
}

STDMETHODIMP Stream::CopyTo(IStream* pstm,
                            ULARGE_INTEGER cb,
                            ULARGE_INTEGER* pcbRead,
                            ULARGE_INTEGER* pcbWritten) {
    return E_NOTIMPL;
}
