#include "utils.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>

using v8::Context;
using v8::Function;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::ArrayBufferView;

HGLOBAL lockAndDuplicateMemory(HGLOBAL hMem)
{
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

bool compareFormats(FORMATETC &a, FORMATETC &b) {
    return (a.tymed & b.tymed) && a.cfFormat == b.cfFormat && a.dwAspect == b.dwAspect;
}

HGLOBAL CopyAsciiToHGlobal(Local<String> value) {
	Local<Context> context = Nan::GetCurrentContext();
	Isolate *isolate = context->GetIsolate();
	std::string textValue = *String::Utf8Value(isolate, value);
	auto len = value->Length();
	auto hGlobal = (LPSTR)::GlobalAlloc(GPTR, len + 1);
	memcpy(hGlobal, textValue.data(), len);
	hGlobal[len] = '\0';
	return hGlobal;
}

HGLOBAL copyUnicodeToHGlobal(Local<String> value) {
	Local<Context> context = Nan::GetCurrentContext();
	Isolate *isolate = context->GetIsolate();
	auto len = value->Length() * 2;
	std::string asciiTextValue = *String::Utf8Value(isolate, value);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring textValue = converter.from_bytes(asciiTextValue);
	auto hGlobal = (LPSTR)::GlobalAlloc(GPTR, len + 1);
	memcpy(hGlobal, textValue.data(), len);
	hGlobal[len] = '\0';
	return hGlobal;
}

HGLOBAL copyCustomToHGlobal(Local<Value> value) {
	Local<Context> context = Nan::GetCurrentContext();
	if (value->IsString()) {
		return copyUnicodeToHGlobal(value->ToString(context).ToLocalChecked());
	}
	else if (value->IsArrayBufferView()) {
		auto pStgMed = new STGMEDIUM{ TYMED_HGLOBAL,{ nullptr }, nullptr };
		auto buffer = value.As<ArrayBufferView>();
		auto len = buffer->ByteLength();
		auto hGlobal = (LPSTR) ::GlobalAlloc(GPTR, len + 1);
		auto written = buffer->CopyContents(hGlobal, len);
		hGlobal[len] = '\0';
		return hGlobal;
	}
	else if (value->IsObject()) {
		auto format = value.As<Object>();
		auto asciiValue = Nan::Get(format, Nan::New("ascii").ToLocalChecked()).ToLocalChecked();
		auto unicode = TRUE;
		if (asciiValue->IsTrue()) {
			unicode = FALSE;
		}
		auto dataValue = Nan::Get(format, Nan::New("data").ToLocalChecked()).ToLocalChecked();
		Local<String> data;
		if (dataValue->IsString()) {
			data = dataValue->ToString(context).ToLocalChecked();
		}
		else {
			data = Nan::New("").ToLocalChecked();
		}
		return unicode ? copyUnicodeToHGlobal(data) : CopyAsciiToHGlobal(data);
	}
	else {
		return copyUnicodeToHGlobal(value->ToString(context).ToLocalChecked());
	}
}

LPCSTR getTypeAsString(Local<Value> value) {
	if (value->IsArgumentsObject()) return "args";
	if (value->IsArray()) return "array";
	if (value->IsArrayBuffer()) return "array buffer";
	if (value->IsArrayBufferView()) return "array buffer view";
	if (value->IsAsyncFunction()) return "async function";
	if (value->IsBoolean()) return "boolean";
	if (value->IsBooleanObject()) return "boolean object";
	if (value->IsDataView()) return "data view";
	if (value->IsDate()) return "date";
	if (value->IsExternal()) return "external";
	if (value->IsFalse()) return "false";
	if (value->IsFloat32Array()) return "float32 array";
	if (value->IsFloat64Array()) return "float64 array";
	if (value->IsFunction()) return "function";
	if (value->IsGeneratorFunction()) return "generator function";
	if (value->IsGeneratorObject()) return "generator object";
	if (value->IsInt16Array()) return "int16 array";
	if (value->IsInt32()) return "int32";
	if (value->IsInt32Array()) return "int32 array";
	if (value->IsInt8Array()) return "int8 array";
	if (value->IsMap()) return "map";
	if (value->IsMapIterator()) return "map iterator";
	if (value->IsName()) return "name";
	if (value->IsNativeError()) return "native error";
	if (value->IsNull()) return "null";
	if (value->IsNullOrUndefined()) return "null or undefined";
	if (value->IsNumber()) return "number";
	if (value->IsNumberObject()) return "number object";
	if (value->IsObject()) return "object";
	if (value->IsPromise()) return "promise";
	if (value->IsProxy()) return "proxy";
	if (value->IsRegExp()) return "regexp";
	if (value->IsSet()) return "set";
	if (value->IsSetIterator()) return "set iterator";
	if (value->IsSharedArrayBuffer()) return "shared array buffer";
	if (value->IsString()) return "string";
	if (value->IsStringObject()) return "string object";
	if (value->IsSymbol()) return "symbol";
	if (value->IsSymbolObject()) return "symbol object";
	if (value->IsTrue()) return "true";
	if (value->IsTypedArray()) return "typed array";
	if (value->IsUint16Array()) return "uint16 array";
	if (value->IsUint32()) return "uint32";
	if (value->IsUint32Array()) return "uint32 array";
	if (value->IsUint8Array()) return "uint8 array";
	if (value->IsUint8ClampedArray()) return "uint8 clamped array";
	if (value->IsUndefined()) return "undefined";
	if (value->IsWeakMap()) return "weak map";
	if (value->IsWeakSet()) return "weak set";
	return "UNKNOWN";
}

Local<Function> getFunction(Local<Object> obj, std::string functionName) {
    // TODO check if ToLocalChecked will fail before calling it
    Local<Value> func = obj->Get(Nan::GetCurrentContext(), Nan::New(functionName).ToLocalChecked()).ToLocalChecked();
    if (func->IsFunction()) {
        return func.As<Function>();
    }
    return Nan::Undefined().As<Function>();
}