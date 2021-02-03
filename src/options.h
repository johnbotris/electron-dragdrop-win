#pragma once

#include <nan.h>
#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>

class Options
{
private:
	HWND hWndParent = nullptr;
    v8::Local<v8::Object> formats;
	v8::Local<v8::Function> completedFunc;
	v8::Local<v8::Function> dragEnterFunc;
	v8::Local<v8::Function> dragLeaveFunc;

public:
	Options(v8::Local<v8::Object> options);
	~Options();


	HWND GetParentHwnd();

	v8::Local<v8::Function> GetCompletedCallbackFunction();
	v8::Local<v8::Function> GetDragEnterCallbackFunction();
	v8::Local<v8::Function> GetDragLeaveCallbackFunction();
	HGLOBAL CopyAsciiToHGlobal(v8::Local<v8::String> value);
	HGLOBAL CopyUnicodeToHGlobal(v8::Local<v8::String> value);
	HGLOBAL CopyCustomToHGlobal(v8::Local<v8::Value> value);

	//static FormatInfo GetFormats(v8::Local<v8::Object> options);

private:
	v8::Local<v8::Function> GetFunction(v8::Local<v8::Object> obj, std::string functionName);
	static LPCSTR Options::GetType(v8::Local<v8::Value> value);
};

