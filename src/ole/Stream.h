#pragma once

#include <windows.h>

#include <cstdint>
#include <string>
#include <vector>

class Stream : public IStream {
   private:
    ULONG refCount;
    std::vector<uint8_t> data;
    std::wstring name;
    size_t cursor;

   public:
    Stream(std::vector<uint8_t> data, std::wstring name);
    ~Stream();

    // IUnknown methods
    STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
    STDMETHOD_(ULONG, AddRef)();
    STDMETHOD_(ULONG, Release)();

    STDMETHOD(Read)(void* pv, ULONG cb, ULONG* pcbRead);
    STDMETHOD(Write)(const void* pv, ULONG cb, ULONG* pcbWritten);
    STDMETHOD(Seek)(LARGE_INTEGER dlibMove,
                          DWORD dwOrigin,
                          ULARGE_INTEGER* plibNewPosition);
    STDMETHOD(CopyTo)(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);

    STDMETHOD(Clone)(IStream** ppstm);
    STDMETHOD(Commit)(DWORD grfCommitFlags);
    STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
    STDMETHOD(Revert)();
    STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize);
    STDMETHOD(Stat)(STATSTG* pstatsg, DWORD grfStatFlag);
    STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
};