#pragma once

#include <nan.h>
#include <windows.h>
#include <string>
#include <locale>
#include <codecvt>

class Options
{
private:
	LPFORMATETC m_pFmtEtc = nullptr;
	LPSTGMEDIUM m_pStgMed = nullptr;
	uint32_t m_count = 0;
	HWND m_hWndParent = nullptr;
	v8::Local<v8::Function> m_completedFunc;
	v8::Local<v8::Function> m_dragEnterFunc;
	v8::Local<v8::Function> m_dragLeaveFunc;

public:
	Options(v8::Local<v8::Object> options);
	~Options() {
		if (m_pFmtEtc != nullptr) {
			delete[] m_pFmtEtc;
			m_pFmtEtc = nullptr;
		}
		if (m_pStgMed != nullptr) {
			for (uint32_t i = 0; i < m_count; i++) {
				STGMEDIUM stgmed = m_pStgMed[i];
				if (stgmed.tymed == TYMED_HGLOBAL) {
					auto size = ::GlobalSize(stgmed.hGlobal);
					::ZeroMemory(stgmed.hGlobal, size);
					::GlobalFree(stgmed.hGlobal);
				}
			}
			delete[] m_pStgMed;
			m_pStgMed = nullptr;
		}
		if (!m_completedFunc.IsEmpty()) {
			m_completedFunc.Clear();
		}
		if (!m_dragEnterFunc.IsEmpty()) {
			m_dragEnterFunc.Clear();
		}
		if (!m_dragLeaveFunc.IsEmpty()) {
			m_dragLeaveFunc.Clear();
		}
	}

	operator LPFORMATETC() {
		return m_pFmtEtc;
	}
	
	operator LPSTGMEDIUM() {
		return m_pStgMed;
	}


	uint32_t Count() {
		return m_count;
	}

	HWND GetParentHwnd() {
		return m_hWndParent;
	}

	v8::Local<v8::Function> GetCompletedCallbackFunction() {
		return m_completedFunc;
	}

	v8::Local<v8::Function> GetDragEnterCallbackFunction() {
		return m_dragEnterFunc;
	}

	v8::Local<v8::Function> GetDragLeaveCallbackFunction() {
		return m_dragLeaveFunc;
	}

	HGLOBAL CopyAsciiToHGlobal(v8::Local<v8::String> value);
	HGLOBAL CopyUnicodeToHGlobal(v8::Local<v8::String> value);
	HGLOBAL CopyCustomToHGlobal(v8::Local<v8::Value> value);

	//static FormatInfo GetFormats(v8::Local<v8::Object> options);

private:
	void InitFromJS(v8::Local<v8::Object> options);
	v8::Local<v8::Function> GetFunction(v8::Local<v8::Object> obj, std::string functionName) {
		v8::Local<v8::Value> func = obj->Get(Nan::New(functionName).ToLocalChecked());
		if (func->IsFunction()) {
			return func.As<v8::Function>();
		}
		return Nan::Undefined().As<v8::Function>();
	}
	static LPCSTR Options::GetType(v8::Local<v8::Value> value);
	//static LPSTGMEDIUM CreateBitmapFormatStorageMedium(v8::Local<v8::ArrayBufferView> value);
};

