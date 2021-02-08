#pragma once

#include <windows.h>
#include <shellapi.h>
#include <oleidl.h>

#include <vector>
#include <string>

HGLOBAL lockAndDuplicateMemory(HGLOBAL hMem);

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

/**
 * Allocates an creates a global dropfiles structure and it's data.
 */
HDROP makeDropFiles(std::vector<std::string>& filePaths);