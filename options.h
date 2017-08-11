#pragma once

#include <nan.h>
#include <string>
#include <list>

class ClipboardFormat {
private:
	std::string m_format;
	LPVOID m_data;

	void SetFormat(v8::Local<v8::Value> value);
	LPCSTR GetType(v8::Local<v8::Value> value);
	void SetData(v8::Local<v8::Value> value);

public:
	ClipboardFormat(v8::Local<v8::Value> item);
	~ClipboardFormat();
	std::string GetFormat();
};

class Options
{
private:
	std::list<ClipboardFormat> m_formatList;
	v8::Local<v8::Function> m_completedFunc;
	v8::Local<v8::Function> m_dragEnterFunc;
	v8::Local<v8::Function> m_dragLeaveFunc;
	void ParseArgs(Nan::NAN_METHOD_ARGS_TYPE args);
	LPCSTR GetType(v8::Local<v8::Value> value);
	v8::Local<v8::Value> GetFunction(v8::Local<v8::Object> obj, LPCSTR szFunctionName);

public:
	Options(Nan::NAN_METHOD_ARGS_TYPE info);
	~Options();
	v8::Local<v8::Function> GetCompletedCallbackFunction();
	v8::Local<v8::Function> GetDragEnterCallbackFunction();
	v8::Local<v8::Function> GetDragLeaveCallbackFunction();
};

