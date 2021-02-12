#include "ole.h"

#include <windows.h>
#include <shlobj_core.h>  // CFSTR_FILECONTENTS, CFSTR_FILEDESCRIPTOR
#include <comdef.h>

#include "wil/resource.h"

#include <iostream>
#include <cassert>
#include <string>

using namespace std;

UINT CF_FILECONTENTS;
UINT CF_FILEDESCRIPTOR;

void initOle() {
    ::OleInitialize(NULL);
    registerClipboardFormat(CFSTR_FILECONTENTS, CF_FILECONTENTS);
    registerClipboardFormat(CFSTR_FILEDESCRIPTOR, CF_FILEDESCRIPTOR);
}

void registerClipboardFormat(LPWSTR, UINT&);

void registerClipboardFormat(LPWSTR format, UINT& formatID) {
    formatID = RegisterClipboardFormat(format);
    wcout << L"Registered " << format << L" with ID " << formatID << endl;
}

wstring getWindowText(HWND hwnd) {
    auto len = GetWindowTextLength(hwnd);
    LPWSTR buffer = new WCHAR[sizeof(WCHAR) * (len + 1)];
    auto result = GetWindowText(hwnd, buffer, len);
    wstring str(buffer, result + 1);
    delete[] buffer;
    return str;
}

std::wstring getClipboardFormatName(const FORMATETC& fmt) {
    return getClipboardFormatName(fmt.cfFormat);
}

std::wstring getClipboardFormatName(const CLIPFORMAT& fmt) {
    WCHAR buffer[64];
    int len = GetClipboardFormatName(fmt, buffer, 64);
    if (len == 0) {
        auto err = HRESULT_FROM_WIN32(GetLastError());
        return _com_error(err).ErrorMessage();
    }
    return wstring(buffer, len + 1);
}

HGLOBAL lockAndDuplicateHGLOBAL(HGLOBAL handle) {
    // lock the source memory object
    auto len = GlobalSize(handle);
    wil::unique_hglobal_locked source(handle);

    // create a fixed "global" block - i.e. just
    // a regular lump of our process heap
    auto dest = GlobalAlloc(GMEM_FIXED, len);

    memcpy(dest, source.get(), len);

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

void deepCopyFormatEtc(LPFORMATETC dest, LPFORMATETC source) {
    // copy the source FORMATETC into dest
    *dest = *source;

    if (source->ptd) {
        // allocate memory for the DVTARGETDEVICE if necessary
        dest->ptd = (DVTARGETDEVICE*)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

        // copy the contents of the source DVTARGETDEVICE into dest->ptd
        *(dest->ptd) = *(source->ptd);
    }
}

const TCHAR* getLastErrorMessage() {
    return _com_error(HRESULT_FROM_WIN32(GetLastError())).ErrorMessage();
}

wostream& operator<<(wostream& stream, const FORMATETC& fmt) {
    stream << L"{ cfFormat: (" << fmt.cfFormat << L", "
           << getClipboardFormatName(fmt) << L"), dwAspect: " << fmt.dwAspect
           << L", lindex: " << fmt.lindex << L", tymed: " << fmt.tymed << L" }";

    return stream;
}