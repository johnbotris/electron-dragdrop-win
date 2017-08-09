#include <nan.h>

#include "enumformat.h"
#include "dataobject.h"
#include "dropsource.h"
#include "dropsourcenotify.h"

using namespace std;

HGLOBAL CopyText()
{
	auto lpText = L"Ashkan Daie";
	auto len = wcslen(lpText) * 2;
	auto tmp = (LPWSTR)malloc(len);

	auto hMem = GlobalAlloc(GHND, len + 1);
	auto ptr = (LPWSTR)GlobalLock(hMem);

	// copy the selected text and null-terminate
	memcpy(ptr, lpText, len);
	ptr[len] = '\0';
	len++;
	ptr[len] = '\0';

	GlobalUnlock(hMem);

	free(tmp);

	return hMem;
}

NAN_METHOD(WhoAmI) 
{
	auto message = Nan::New<v8::String>("I'm a Node Hero!").ToLocalChecked();
	info.GetReturnValue().Set(message);
}

NAN_METHOD(DragDrop)
{
	//if (!info[0]->IsArrayBuffer()) {
	//	Nan::ThrowError("Argument 1 must be a buffer");
	//	return;
	//}
	//if (!info[1]->IsObject()) {
	//    Nan::ThrowError("Argument 2 must be an object");
	//    return;
	//}

	//auto data = info[1]->ToObject();
	//if (data->IsArray()) {
	//    v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(data);
	//    for (UINT32 i = 0; i < array->Length(); i++) {

	//    }
	//} else {
	//    auto formatVal = data->Get(Nan::New("format").ToLocalChecked());
	//    if (!formatVal->IsString()) {

	//    }
	//}
	//HWND hwnd = (HWND)node::Buffer::Data(info[0]->ToObject());
	//WINDOWINFO windowInfo;
	//BOOL retVal = ::GetWindowInfo(hwnd, &windowInfo);
	//if (!retVal) {
	//    Nan::ThrowError("Invalid window handle");
	//}
	IDropSourceNotify* pDropSourceNotify = nullptr;
	LPDROPSOURCE pDropSource = nullptr;
	LPDATAOBJECT pDataObject = nullptr;
	FORMATETC fmtetc = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed = { TYMED_HGLOBAL,{ 0 }, 0 };

	stgmed.hGlobal = CopyText();

	OleDropSourceNotify::Create(&pDropSourceNotify);
	OleDropSource::Create(pDropSourceNotify, &pDropSource);
	OleDataObject::Create(&fmtetc, &stgmed, 1, &pDataObject);

	//
	//	** ** ** The drag-drop operation starts here! ** ** **
	//
	DWORD		 dwEffect;
	auto dwResult = DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
}

NAN_MODULE_INIT(Initialize)
{
	//::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	NAN_EXPORT(target, WhoAmI);
	NAN_EXPORT(target, DragDrop);
}

NODE_MODULE(addon, Initialize)