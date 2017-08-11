#include <nan.h>

#include "enumformat.h"
#include "dataobject.h"
#include "dropsource.h"
#include "dropsourcenotify.h"
#include "options.h"

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
	Options options(info);
	
	IDropSourceNotify* pDropSourceNotify = nullptr;
	LPDROPSOURCE pDropSource = nullptr;
	LPDATAOBJECT pDataObject = nullptr;
	FORMATETC fmtetc = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed = { TYMED_HGLOBAL,{ 0 }, 0 };

	stgmed.hGlobal = CopyText();

	OleDropSourceNotify::Create(&options, &pDropSourceNotify);
	OleDropSource::Create(pDropSourceNotify, &pDropSource);
	OleDataObject::Create(&fmtetc, &stgmed, 1, &pDataObject);

	//
	//	** ** ** The drag-drop operation starts here! ** ** **
	//
	DWORD		 dwEffect;
	auto dwResult = DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
	auto completedFunc = options.GetCompletedCallbackFunction();
	if (!completedFunc.IsEmpty()) {
		const unsigned argc = 1;
		v8::Local<v8::Value> argv[argc] = { Nan::New(dwResult) };
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), completedFunc, argc, argv);
	}
}

NAN_MODULE_INIT(Initialize)
{
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);
	NAN_EXPORT(target, WhoAmI);
	NAN_EXPORT(target, DragDrop);
}

NODE_MODULE(addon, Initialize)