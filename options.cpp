#include "options.h"

Options::Options(Nan::NAN_METHOD_ARGS_TYPE args) {
    ParseArgs(args);
}

Options::~Options() {
    
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
	if (value->IsWebAssemblyCompiledModule()) return "web assembly compiled module";
	return "UNKNOWN";
}

void Options::ParseArgs(Nan::NAN_METHOD_ARGS_TYPE args) {
	if(args.Length() == 0) {
		Nan::ThrowError("Invalid parameter. Parameter options missing.");
		return;
	}

	if (!args[0]->IsObject()) {
		Nan::ThrowError("Invalid options property. Must be an object.");
		return;
	}

	v8::Local<v8::Object> options = args[0]->ToObject();
	v8::Local<v8::Value> formats = options->Get(Nan::New<v8::String>("formats").ToLocalChecked());

	if (formats.IsEmpty()) {
		Nan::ThrowError("Invalid options property. formats property is missing.");
	}
	else if (!formats->IsArray()) {
		Nan::ThrowError("Invalid options property. formats must be an array.");
	}

	v8::Local<v8::Array> formatsArray = formats.As<v8::Array>();

	auto formatsLength = formatsArray->Length();
	if (formatsLength == 0) {
		Nan::ThrowError("Invalid options property. formats array is empty.");
	}

	for (uint32_t i = 0; i < formatsLength; i++) {
		v8::Local<v8::Value> clipboardFormat = formatsArray->Get(i);
        this->m_formatList.push_back(ClipboardFormat(clipboardFormat));
	}
    
	v8::Local<v8::Value> events = options->Get(Nan::New<v8::String>("events").ToLocalChecked());
	
	if (!events.IsEmpty()) {
		if (!events->IsObject()) {
			Nan::ThrowError("Invalid events property. events must be an object.");
		}

		v8::Local<v8::Object> eventsObj = events->ToObject();
		v8::Local<v8::Value> completed = GetFunction(eventsObj, "completed");
		if (!completed->IsUndefined()) {
			m_completedFunc = completed.As<v8::Function>();
		}

		v8::Local<v8::Value> dragEnter = GetFunction(eventsObj, "dragEnter");
		if (!dragEnter->IsUndefined()) {
			m_dragEnterFunc = dragEnter.As<v8::Function>();
		}

		v8::Local<v8::Value> dragLeave = GetFunction(eventsObj, "dragLeave");
		if (!dragLeave->IsUndefined()) {
			m_dragLeaveFunc = dragLeave.As<v8::Function>();
		}
	}
    return;
}

v8::Local<v8::Value> Options::GetFunction(v8::Local<v8::Object> obj, LPCSTR szFunctionName) {
	v8::Local<v8::Value> func = obj->Get(Nan::New<v8::String>(szFunctionName).ToLocalChecked());
	if (!func.IsEmpty()) {
		if (!func->IsFunction()) {
			return Nan::Undefined();
		}
		return func.As<v8::Function>();
	}
}

v8::Local<v8::Function> Options::GetCompletedCallbackFunction() {
	return m_completedFunc;
}

v8::Local<v8::Function> Options::GetDragEnterCallbackFunction() {
	return m_dragEnterFunc;
}

v8::Local<v8::Function> Options::GetDragLeaveCallbackFunction() {
	return m_dragLeaveFunc;
}


void ClipboardFormat::SetFormat(v8::Local<v8::Value> value) {
	if (value.IsEmpty()) {
		Nan::ThrowError("format missing");
		return;
	}
	if (!value->IsString()) {
		Nan::ThrowError("format must be a string");
		return;
	}
	m_format.assign(*v8::String::Utf8Value(value));
}

LPCSTR ClipboardFormat::GetType(v8::Local<v8::Value> value) {
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
	if (value->IsWebAssemblyCompiledModule()) return "web assembly compiled module";
	return "UNKNOWN";
}

void ClipboardFormat::SetData(v8::Local<v8::Value> value) {
	LPCSTR type = GetType(value);
	if (value.IsEmpty()) {
		Nan::ThrowError("data missing");
		return;
	}
	if (!value->IsArrayBufferView()) {
		Nan::ThrowError("format must be a buffer");
		return;
	}
	auto bufferView = value.As<v8::ArrayBufferView>();
	auto len = bufferView->ByteLength();
	//Need to null terminate payload
	m_data = ::malloc(len + 1);
	auto written = bufferView->CopyContents(m_data, len);
	auto nullPtr = (char*)m_data + len;
	nullPtr[0] = 0;
}

ClipboardFormat::ClipboardFormat(v8::Local<v8::Value> item) {
		if (!item->IsObject()) {
			Nan::ThrowError("Invalid options property. formats array contains invalid data.");
		}

		v8::Local<v8::Object> itemObj = item.As<v8::Object>();
		v8::Local<v8::Value> format = itemObj->Get(Nan::New<v8::String>("format").ToLocalChecked());
		SetFormat(format);
		v8::Local<v8::Value> data = itemObj->Get(Nan::New<v8::String>("data").ToLocalChecked());
		SetData(data);
	}

ClipboardFormat::~ClipboardFormat() {
		if (m_data != nullptr) {
			//::free(m_data);
		}
	}

std::string ClipboardFormat::GetFormat() {
	return m_format;
}
