#include <nan.h>

#include "dataobject.h"
#include "dropsource.h"
#include "dropsourcenotify.h"
#include "options.h"

#include <stdlib.h>
#include <string>
#include <algorithm>

using v8::Local;
using v8::Value;
using v8::Number;
using v8::Object;
using v8::Context; 

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

	Local<Context> context = Nan::GetCurrentContext();

	// TODO Check if ToLocalChecked will fail before calling it
	Local<Object> options = info[0]->ToObject(context).ToLocalChecked();
	Options opts(options);
	
	IDropSourceNotify* pDropSourceNotify = nullptr;
	LPDATAOBJECT dataObject = new DataObject(opts);
    LPDROPSOURCE dropSource = new DropSource(opts);

	DWORD dwEffect;
    HRESULT result = DoDragDrop(dataObject, dropSource, DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);

    if (opts.GetCompletedCallbackFunction()->IsFunction()) {
		const unsigned argc = 1;
		Local<Value> argv[argc] = { Nan::New<Number>(dwEffect) };
		Nan::MakeCallback(context->Global(), opts.GetCompletedCallbackFunction(), argc, argv);
	}
}

NAN_MODULE_INIT(Initialize)
{
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	::OleInitialize(NULL);
	NAN_EXPORT(target, DragDrop);
}

NODE_MODULE(addon, Initialize)