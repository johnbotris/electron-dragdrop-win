#include "oleutils.h"

#include <Shlobj_core.h>

#include <iostream>
#include <cassert>

using namespace std;

HGLOBAL lockAndDuplicateMemory(HGLOBAL hMem) {
    // lock the source memory object
    auto len = GlobalSize(hMem);
    auto source = GlobalLock(hMem);

    // create a fixed "global" block - i.e. just
    // a regular lump of our process heap
    auto dest = GlobalAlloc(GMEM_FIXED, len);

    memcpy(dest, source, len);

    GlobalUnlock(hMem);

    return dest;
}

bool compareFormats(const FORMATETC& a, const FORMATETC& b) {
    return (a.tymed & b.tymed) && a.cfFormat == b.cfFormat &&
           a.dwAspect == b.dwAspect;
}

FORMATETC makeFormatEtc(CLIPFORMAT cfFormat,
                        DVTARGETDEVICE* ptd,
                        DWORD dwAspect,
                        LONG lindex,
                        DWORD tymed) {
    return {cfFormat, ptd, dwAspect, lindex, tymed};
}

STGMEDIUM makeStorageMedium(HGLOBAL data) {
    STGMEDIUM stgMedium = {TYMED_HGLOBAL, {nullptr}, nullptr};
    stgMedium.hGlobal = data;
    return stgMedium;
}

HDROP makeDropFiles(std::vector<std::string>& filePaths) {
    cout << "start make drop files" << endl;
    SIZE_T totalSizeBytes =
        sizeof(DROPFILES) + 1;  // + 1 for extra '\0' at the end
    for (auto path : filePaths) {
        totalSizeBytes += path.size() + 1;  // size in bytes + '\0'
    }

    // https://docs.microsoft.com/en-us/windows/win32/api/shlobj_core/ns-shlobj_core-dropfiles
    DWORD pFiles = sizeof(DROPFILES);
    POINT pt = {0, 0};
    BOOL fNC = FALSE;
    BOOL fWide = TRUE;

    DROPFILES dropFiles = {pFiles, pt, fNC, fWide};

    HGLOBAL handle = GlobalAlloc(GHND, totalSizeBytes);
    LPVOID ptr = GlobalLock(handle);
    memcpy(ptr, &dropFiles, sizeof(dropFiles));

    LPBYTE dataStart = (LPBYTE)ptr + pFiles;

    for (auto path : filePaths) {
        auto size =
            path.size() +
            1;  // TODO is string.size() guaranteed not to include the '\0'?
        auto c_str = path.c_str();
        memcpy(dataStart, c_str, size);
        dataStart += size;
    }

    assert(dataStart == (LPBYTE)ptr + totalSizeBytes - 1);

    *(dataStart + totalSizeBytes) = '\0';

    GlobalUnlock(handle);

    cout << "end make drop fdils" << endl;
    return (HDROP)handle;
}