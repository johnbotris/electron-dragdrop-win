#include <nan.h>

#include "enumformat.h"
#include "dataobject.h"
#include "dropsource.h"
#include "dropsourcenotify.h"
#include "options.h"

#include <stdlib.h>
#include <string>
#include <algorithm>
//#include <Magick++.h>

NAN_METHOD(DragDrop)
{
	if (info.Length() == 0) {
		Nan::ThrowError("Invalid parameter. Parameter options missing.");
		return;
	}

	if (!info[0]->IsObject()) {
		Nan::ThrowError("Invalid options property. Must be an object.");
		return;
	}

	v8::Local<v8::Object> options = info[0]->ToObject();
	Options opt(options);
	IDropSourceNotify* pDropSourceNotify = nullptr;
	LPDROPSOURCE pDropSource = nullptr;
	LPDATAOBJECT pDataObject = nullptr;
	OleDropSourceNotify::Create(&opt, &pDropSourceNotify);
	OleDropSource::Create(pDropSourceNotify, &pDropSource);
	OleDataObject::Create(opt, opt, opt.Count(), &pDataObject);

	DWORD		 dwEffect;
	auto dwResult = DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
	if (opt.GetCompletedCallbackFunction()->IsFunction()) {
		const unsigned argc = 1;
		v8::Local<v8::Value> argv[argc] = { Nan::New(dwResult) };
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), opt.GetCompletedCallbackFunction(), argc, argv);
	}
}

NAN_MODULE_INIT(Initialize)
{
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);
	//Magick::InitializeMagick(nullptr);
	NAN_EXPORT(target, DragDrop);
}

NODE_MODULE(addon, Initialize)