#include "options.h"

#include <algorithm>
#include <string>
#include <iostream>

using v8::Local;
using v8::Object;
using v8::String;
using v8::Context;
using v8::Isolate;
using v8::Value;

//#include "Magick++.h"

//extern "C" {
//	MagickExport void *ImageToHBITMAP(MagickCore::Image* image, MagickCore::ExceptionInfo* ex);
//}

Options::Options(v8::Local<v8::Object> options) {
	InitFromJS(options);
}

void Options::InitFromJS(v8::Local<v8::Object> options) {
	auto formatsValue = Nan::Get(options, Nan::New("formats").ToLocalChecked()).ToLocalChecked();
	if (formatsValue.IsEmpty()) {
		Nan::ThrowError("formats is empty.");
	}
	if (!formatsValue->IsObject()) {
		Nan::ThrowError("formats must be an object.");
	}
	auto formats = formatsValue.As<v8::Object>();
	auto propertyNames = Nan::GetPropertyNames(formats).ToLocalChecked();
	auto propertyCount = propertyNames->Length();

	Local<Context> context = Nan::GetCurrentContext();
	Isolate *isolate = context->GetIsolate();

	for (uint32_t i = 0; i < propertyCount; i++) {
		Local<Value> propertyName = Nan::Get(propertyNames, i).ToLocalChecked();
		std::string propertyNameString = *String::Utf8Value(isolate, propertyName);
		Local<Value> propertyValue = Nan::Get(formats, propertyName).ToLocalChecked();

		if (!propertyValue.IsEmpty()) {
			m_count++;
			// We create two formats (CF_TEXT and CF_UNICODETEXT) for text property. 
			// Increment format count by one if text property is a string
			if (propertyValue->IsString() && propertyNameString == "text") {
				m_count++;
			}
		}
	}

	this->m_pFmtEtc = new FORMATETC[m_count];
	this->m_pStgMed = new STGMEDIUM[m_count];

	uint32_t idx = 0;
	for (uint32_t i = 0; i < propertyCount; i++, idx++) {
		Local<Value> propertyName = Nan::Get(propertyNames, i).ToLocalChecked();
		std::string propertyNameString  = *String::Utf8Value(isolate, propertyName);
		Local<Value> propertyValue = Nan::Get(formats, propertyName).ToLocalChecked();
		Local<Context> context = Nan::GetCurrentContext();
		if (propertyNameString == "text" &&
			!propertyValue.IsEmpty() &&
			propertyValue->IsString()) {
			this->m_pFmtEtc[idx] = {CF_TEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
			this->m_pStgMed[idx] = { TYMED_HGLOBAL,{ nullptr }, nullptr };
			this->m_pStgMed[idx].hGlobal = CopyAsciiToHGlobal(propertyValue->ToString(context).ToLocalChecked());

			idx++;
			this->m_pFmtEtc[idx] = { CF_UNICODETEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			this->m_pStgMed[idx] = { TYMED_HGLOBAL,{ nullptr }, nullptr };
			this->m_pStgMed[idx].hGlobal = CopyUnicodeToHGlobal(propertyValue->ToString(context).ToLocalChecked());
		}
		else {
			auto clipboardFormat = (CLIPFORMAT)::RegisterClipboardFormatA(propertyNameString.data());
			this->m_pFmtEtc[idx] = {clipboardFormat, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
			this->m_pStgMed[idx] = {TYMED_HGLOBAL, {nullptr}, nullptr};
			this->m_pStgMed[idx].hGlobal = CopyCustomToHGlobal(propertyValue);
		}
	}

	v8::Local<v8::Value> eventsValue = Nan::Get(options, Nan::New("events").ToLocalChecked()).ToLocalChecked();

	if (!eventsValue.IsEmpty()) {
		if (!eventsValue->IsObject()) {
			Nan::ThrowError("events must be an object.");
		}

		if (eventsValue->IsObject()) {
			v8::Local<v8::Object> events = eventsValue->ToObject(context).ToLocalChecked();
			this->m_completedFunc = GetFunction(events, "completed");
			this->m_dragEnterFunc = GetFunction(events, "dragEnter");
			this->m_dragLeaveFunc = GetFunction(events, "dragLeave");
		}
	}

	v8::Local<v8::Value> windowHandleValue = Nan::Get(options, Nan::New("windowHandle").ToLocalChecked()).ToLocalChecked();

	if (!windowHandleValue.IsEmpty() || windowHandleValue->IsArrayBufferView()) {
		auto windowHandleBuffer = windowHandleValue.As<v8::ArrayBufferView>();
		auto len = windowHandleBuffer->ByteLength();
		windowHandleBuffer->CopyContents(&m_hWndParent, len);
		if (!::IsWindow(m_hWndParent)) {
			m_hWndParent = nullptr;
		}
	}
}

HGLOBAL Options::CopyAsciiToHGlobal(v8::Local<v8::String> value) {
	Local<Context> context = Nan::GetCurrentContext();
	Isolate *isolate = context->GetIsolate();
	std::string textValue = *String::Utf8Value(isolate, value);
	auto len = value->Length();
	auto hGlobal = (LPSTR)::GlobalAlloc(GPTR, len + 1);
	memcpy(hGlobal, textValue.data(), len);
	hGlobal[len] = '\0';
	return hGlobal;
}

HGLOBAL Options::CopyUnicodeToHGlobal(v8::Local<v8::String> value) {
	Local<Context> context = Nan::GetCurrentContext();
	Isolate *isolate = context->GetIsolate();
	auto len = value->Length() * 2;
	std::string asciiTextValue = *v8::String::Utf8Value(isolate, value);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring textValue = converter.from_bytes(asciiTextValue);
	auto hGlobal = (LPSTR)::GlobalAlloc(GPTR, len + 1);
	memcpy(hGlobal, textValue.data(), len);
	hGlobal[len] = '\0';
	return hGlobal;
}

HGLOBAL Options::CopyCustomToHGlobal(v8::Local<v8::Value> value) {
	Local<Context> context = Nan::GetCurrentContext();
	if (value->IsString()) {
		return CopyUnicodeToHGlobal(value->ToString(context).ToLocalChecked());
	}
	else if (value->IsArrayBufferView()) {
		auto pStgMed = new STGMEDIUM{ TYMED_HGLOBAL,{ nullptr }, nullptr };
		auto buffer = value.As<v8::ArrayBufferView>();
		auto len = buffer->ByteLength();
		auto hGlobal = (LPSTR) ::GlobalAlloc(GPTR, len + 1);
		auto written = buffer->CopyContents(hGlobal, len);
		hGlobal[len] = '\0';
		return hGlobal;
	}
	else if (value->IsObject()) {
		auto format = value.As<v8::Object>();
		auto asciiValue = Nan::Get(format, Nan::New("ascii").ToLocalChecked()).ToLocalChecked();
		auto unicode = TRUE;
		if (asciiValue->IsTrue()) {
			unicode = FALSE;
		}
		auto dataValue = Nan::Get(format, Nan::New("data").ToLocalChecked()).ToLocalChecked();
		v8::Local<v8::String> data;
		if (dataValue->IsString()) {
			data = dataValue->ToString(context).ToLocalChecked();
		}
		else {
			data = Nan::New("").ToLocalChecked();
		}
		return unicode ? CopyUnicodeToHGlobal(data) : CopyAsciiToHGlobal(data);
	}
	else {
		return CopyUnicodeToHGlobal(value->ToString(context).ToLocalChecked());
	}
}

LPCSTR Options::GetType(v8::Local<v8::Value> value) {
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

//FormatInfo Options::GetFormats(v8::Local<v8::Object> options) {
//	auto formatsValue = options->Get(Nan::New("formats").ToLocalChecked());
//	if (formatsValue.IsEmpty()) {
//		Nan::ThrowError("formats is empty.");
//	}
//	if (!formatsValue->IsObject()) {
//		Nan::ThrowError("formats must be an object.");
//	}
//	auto formatsObj = formatsValue.As<v8::Object>();
//	auto propertyNames = formatsObj->GetPropertyNames();
//	auto propertyCount = propertyNames->Length();
//
//	uint32_t formatCount = 0;
//
//	for (uint32_t i = 0; i < propertyCount; i++) {
//		auto propertyName = propertyNames->Get(i)->ToString();
//		std::string propertyNameString = *v8::String::Utf8Value(propertyName);
//		auto propertyValue = formatsObj->Get(propertyName);
//
//		if (!propertyValue.IsEmpty()) {
//			formatCount++;
//			// We create two formats (CF_TEXT and CF_UNICODETEXT) for text property. 
//			// Increment format count by one if text property is a string
//			if (propertyValue->IsString() && propertyNameString == "text") {
//				formatCount++;
//			}
//		}
//	}
//
//	LPFORMATETC pFmtEtc = new FORMATETC[formatCount];
//	LPSTGMEDIUM pStgMed = new STGMEDIUM[formatCount];
//
//	uint32_t idx = 0;
//	for (uint32_t i = 0; i < propertyCount; i++, idx++) {
//		auto propertyName = propertyNames->Get(i)->ToString();
//		std::string propertyNameString = *v8::String::Utf8Value(propertyName);
//		auto propertyValue = formatsObj->Get(propertyName);
//
//		if (propertyNameString == "text" && 
//			!propertyValue.IsEmpty() &&
//			propertyValue->IsString()) {
//			pFmtEtc[idx] = { CF_TEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
//			pStgMed[idx] = *CreateTextFormatStorageMedium(propertyValue->ToString());
//			
//			idx++;
//			pFmtEtc[idx] = { CF_UNICODETEXT, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
//			pStgMed[idx] = *CreateUnicodeTextFormatStorageMedium(propertyValue->ToString());
//		}
//		else {
//			auto clipboardFormat = (CLIPFORMAT)::RegisterClipboardFormat(propertyNameString.data());
//			pFmtEtc[idx] = { clipboardFormat, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
//			pStgMed[idx] = *CreateCustomFormatStorageMedium(propertyValue);
//		}
//		//if (propertyNameString == "image" &&
//		//	!propertyValue.IsEmpty() &&
//		//	propertyValue->IsArrayBufferView()) {
//		//	pFmtEtc[i] = { CF_DIB, nullptr, DVASPECT_CONTENT, -1, TYMED_GDI };
//		//	pStgMed[i] = *CreateBitmapFormatStorageMedium(propertyValue.As<v8::ArrayBufferView>());
//		//}
//
//	}
//	FormatInfo info{ pFmtEtc, pStgMed, formatCount };
//	return info;
//}

//LPSTGMEDIUM Options::CreateBitmapFormatStorageMedium(v8::Local<v8::ArrayBufferView> value) {
//	LPSTGMEDIUM pStgMed = new STGMEDIUM{ TYMED_GDI,{ nullptr }, nullptr };
//	auto len = value->ByteLength();
//	LPVOID data = ::malloc(len + 1);
//	auto written = value->CopyContents(data, len);
//	Magick::Blob imageBlob(data, len);
//	::free(data);
//	Magick::Image image(imageBlob);
//	MagickCore::ExceptionInfo* pExceptionInfo = MagickCore::AcquireExceptionInfo();
//	MagickCore::Image* pImage = image.image();
//	Magick::ExceptionInfo exInfo;
//	auto hBitmap = (HBITMAP)ImageToHBITMAP(pImage, pExceptionInfo);
//	// this structure is an equivalent of BITMAPINFO except it contains
//	// a full-sized color table
//	struct {
//		BITMAPINFOHEADER bmiHeader;
//		RGBQUAD bmiColors[256];
//	} bmi;
//
//	memset(&bmi, 0, sizeof(bmi));
//	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//
//	HDC hDC = GetDC(NULL);  // any DC will work
//
//	GetDIBits(hDC, hBitmap, 0, 1, NULL, (BITMAPINFO *)&bmi,
//		DIB_RGB_COLORS);
//
//	// now bmi.bmiHeader contains information about bitmap's
//	// color format; check if this is a palettized image and retrieve
//	// the palette if necessary
//
//	if (bmi.bmiHeader.biBitCount <= 8)
//	{
//		GetDIBits(hDC, hBitmap, 0, 1, NULL, (BITMAPINFO *)&bmi,
//			DIB_RGB_COLORS);
//	}
//
//	ReleaseDC(NULL, hDC);
//
//	BITMAP bmp;
//	PBITMAPINFO pbmi;
//
//	GetObject(hBitmap, sizeof(BITMAP), &bmp);
//	auto cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
//	if (cClrBits == 1)
//		cClrBits = 1;
//	else if (cClrBits <= 4)
//		cClrBits = 4;
//	else if (cClrBits <= 8)
//		cClrBits = 8;
//	else if (cClrBits <= 16)
//		cClrBits = 16;
//	else if (cClrBits <= 24)
//		cClrBits = 24;
//	else cClrBits = 32;
//
//	if (cClrBits < 24)
//		pbmi = (PBITMAPINFO)GlobalAlloc(LPTR,
//			sizeof(BITMAPINFOHEADER) +
//			sizeof(RGBQUAD) * (1 << cClrBits));
//	else 
//		pbmi = (PBITMAPINFO)GlobalAlloc(LPTR,
//			sizeof(BITMAPINFOHEADER));
//	auto ptr = (LPVOID)GlobalLock(pbmi);
//
//	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	pbmi->bmiHeader.biWidth = bmp.bmWidth;
//	pbmi->bmiHeader.biHeight = bmp.bmHeight;
//	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
//	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
//	if (cClrBits < 24)
//		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);
//	pbmi->bmiHeader.biCompression = BI_RGB;
//	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
//		* pbmi->bmiHeader.biHeight;
//	pbmi->bmiHeader.biClrImportant = 0;
//	memcpy(ptr, &bmi, len);
//	//ptr[len] = '\0';
//	//pStgMed->hBitmap = hBitmap;
//	pStgMed->hGlobal = ptr;
//	return pStgMed;
//}
