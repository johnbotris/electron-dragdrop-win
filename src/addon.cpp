#include <nan.h>

#include "dataobject.h"
#include "dropsource.h"
#include "options.h"

#include <stdlib.h>
#include <string>
#include <algorithm>

using v8::Local;
using v8::Value;
using v8::Number;
using v8::Object;
using v8::Context;

NAN_METHOD(DragDrop) {
    if (info.Length() == 0) {
        Nan::ThrowError("Invalid parameter. Parameter options missing.");
        return;
    }

    if (!info[0]->IsObject()) {
        Nan::ThrowError("Invalid options property. Must be an object.");
        return;
    }

    // TODO Check if ToLocalChecked will fail before calling it
    Options options(Nan::To<Object>(info[0]).ToLocalChecked());

    IDropSourceNotify* pDropSourceNotify = nullptr;
    LPDATAOBJECT dataObject = new DataObject(options);
    LPDROPSOURCE dropSource = new DropSource(options);

    DWORD dwEffect;
    HRESULT result = DoDragDrop(dataObject, dropSource,
                                DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);

    options.OnDragCompleted(Nan::New<Number>(dwEffect));
}

NAN_MODULE_INIT(Initialize) {
    ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    ::OleInitialize(NULL);
    NAN_EXPORT(target, DragDrop);
}

NODE_MODULE(addon, Initialize)