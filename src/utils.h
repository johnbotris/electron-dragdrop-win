#pragma once

#include <windows.h>
#include <nan.h>

HGLOBAL lockAndDuplicateMemory(HGLOBAL hMem);

bool compareFormats(FORMATETC a, FORMATETC b);

HGLOBAL copyAsciiToHGlobal(v8::Local<v8::String> value);

HGLOBAL copyUnicodeToHGlobal(v8::Local<v8::String> value);

HGLOBAL copyCustomToHGlobal(v8::Local<v8::Value> value);

LPCSTR getTypeAsString(v8::Local<v8::Value> value);

v8::Local<v8::Function> getFunction(v8::Local<v8::Object> obj, std::string functionName);