#include <nan.h>

#include "enumformat.h"
#include "dataobject.h"
#include "dropsource.h"

HGLOBAL CopyText()
{
	HGLOBAL hMem;
	char   *ptr;

	LPWSTR lpText = L"Ashkan Daie";
	auto len = wcslen(lpText);
	auto tmp = (LPWSTR)malloc(len);

	hMem = GlobalAlloc(GHND, len + 1);
	ptr = (char *)GlobalLock(hMem);

	// copy the selected text and nul-terminate
	memcpy(ptr, tmp, len);
	ptr[len] = '\0';

	GlobalUnlock(hMem);

	free(tmp);

	return hMem;
}

NAN_METHOD(DragDrop)
{
	 //if (!info[0]->IsArrayBuffer()) {
	 //	Nan::ThrowError("Argument 1 must be a buffer");
	 //	return;
	 //}
	 //if (!info[1]->IsObject()) {
		// Nan::ThrowError("Argument 2 must be an object");
		// return;
	 //}

	 //auto data = info[1]->ToObject();
	 //if (data->IsArray()) {
		// v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(data);
		// for (UINT32 i = 0; i < array->Length(); i++) {

		// }
	 //} else {
		// auto formatVal = data->Get(Nan::New("format").ToLocalChecked());
		// if (!formatVal->IsString()) {

		// }
	 //}
	 //HWND hwnd = (HWND)node::Buffer::Data(info[0]->ToObject());
	 //WINDOWINFO windowInfo;
	 //BOOL retVal = ::GetWindowInfo(hwnd, &windowInfo);
	 //if (!retVal) {
		// Nan::ThrowError("Invalid window handle");
	 //}
	LPDROPSOURCE pDropSource;
	LPDATAOBJECT pDataObject;
	FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed = { TYMED_HGLOBAL,{ 0 }, 0 };

	stgmed.hGlobal = CopyText();
	CreateDropSource(&pDropSource);
	CreateDataObject(&fmtetc, &stgmed, 1, &pDataObject);

	//
	//	** ** ** The drag-drop operation starts here! ** ** **
	//
	DWORD		 dwEffect;
	auto dwResult = DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
}

NAN_MODULE_INIT(Initialize)
{
	NAN_EXPORT(target, DragDrop);
}

NODE_MODULE(addon, Initialize)