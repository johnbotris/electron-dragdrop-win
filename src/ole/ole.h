#pragma once

#include <windows.h>

#include <vector>
#include <string>

extern UINT CF_FILECONTENTS;
extern UINT CF_FILEDESCRIPTOR;

// extern UINT CF_INDRAGLOOP;
// extern UINT CF_LOGICALPERFORMEDDROPEFFECT;
// extern UINT CF_PASTESUCCEEDED;
// extern UINT CF_PERFORMEDDROPEFFECT;
// extern UINT CF_PREFERREDDROPEFFECT;
// extern UINT CF_TARGETCLSID;
// extern UINT CF_UNTRUSTEDDRAGDROP;

void initOle();

void registerClipboardFormat(LPWSTR, UINT&);

std::wstring getWindowText(HWND hwnd);

std::wstring getErrorMessage(HRESULT hresult);

std::wstring getClipboardFormatName(const FORMATETC&);

std::wstring getClipboardFormatName(const CLIPFORMAT&);

HGLOBAL lockAndDuplicateHGLOBAL(HGLOBAL hMem);

bool compareFormats(const FORMATETC& a, const FORMATETC& b);

FORMATETC makeFormatEtc(CLIPFORMAT cfFormat,
                        DVTARGETDEVICE* ptd,
                        DWORD dwAspect,
                        LONG lindex,
                        DWORD tymed);

/**
 * Consumes the provided data
 */
STGMEDIUM makeStorageMedium(HGLOBAL data);

void deepCopyFormatEtc(LPFORMATETC dest, LPFORMATETC source);

const TCHAR* getLastErrorMessage();

std::wostream& operator<<(std::wostream& stream, const FORMATETC& fmt);
