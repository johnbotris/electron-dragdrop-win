#pragma once

#include <nan.h>

HGLOBAL lockAndDuplicateMemory(HGLOBAL hMem);

bool compareFormats(const FORMATETC& a, const FORMATETC& b);

HGLOBAL copyAsciiToHGlobal(const v8::Local<v8::String>& value);

HGLOBAL copyUnicodeToHGlobal(const v8::Local<v8::String>& value);

HGLOBAL copyCustomToHGlobal(const v8::Local<v8::Value>& value);

LPCSTR getTypeAsString(const v8::Local<v8::Value>& value);

v8::Local<v8::Function> getFunction(const v8::Local<v8::Object>& obj,
                                    const std::string& functionName);

v8::Local<v8::String> getWindowText(HWND hwnd);