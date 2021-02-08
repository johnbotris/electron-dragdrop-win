#include <nan.h>

#include <windows.h>
#include <shellapi.h>
#include <oleidl.h>

#include <iostream>
#include <vector>
#include <array>
#include <memory>

#include "Worker.h"
#include "ole/DataObject.h"
#include "ole/DropSource.h"

using namespace std;
using namespace v8;

NAN_METHOD(DoDragDrop) {
    auto argc = info.Length();

    if (argc < 2) {
        Nan::ThrowError("Missing parameter \"dataPromise\".");
        return;
    }

    if (!info[0]->IsPromise()) {
        Nan::ThrowError("Parameter \"options\" is not a Promise");
        return;
    }

    if (!info[argc - 1]->IsFunction()) {
        Nan::ThrowError(
            "Drag and drop internal error: Expected callback function");
        return;
    }

    Local<Promise> dataPromise = info[0].As<Promise>();
    Local<Function> callback = info[argc - 1].As<Function>();

    shared_ptr<Queue>* queuePtr = new std::shared_ptr<Queue>(new Queue(3));

    auto worker =
        new Worker(new Nan::Callback(callback), shared_ptr<Queue>(*queuePtr));

    auto dataReadyHandler = [](Nan::NAN_METHOD_ARGS_TYPE info) {
        Local<External> data = info.Data().As<External>();
        shared_ptr<Queue>* queue =
            static_cast<shared_ptr<Queue>*>(data->Value());

        auto filePaths = vector<string>();

        // Convert the javascript array of javascript strings into a c++ vector
        // of c++ strings
        if (info.Length() > 0 && info[0]->IsArray()) {
            Local<Array> array = info[0].As<Array>();
            uint32_t count = array->Length();
            for (uint32_t i = 0; i < count; ++i) {
                MaybeLocal<Value> value = Nan::Get(array, i);
                if (!value.IsEmpty() && value.ToLocalChecked()->IsString()) {
                    string filePath =
                        *Nan::Utf8String(value.ToLocalChecked().As<String>());
                }
            }
        }

        // TODO here is where we put whatever data we were waiting for
        (*queue)->push(filePaths);
        delete queue;
    };

    auto dataReadyCallback = Nan::GetFunction(Nan::New<FunctionTemplate>(
        dataReadyHandler, Nan::New<External>(static_cast<void*>(queuePtr))));

    dataPromise->Then(Nan::GetCurrentContext(),
                      dataReadyCallback.ToLocalChecked());

    Nan::AsyncQueueWorker(worker);
}

NAN_MODULE_INIT(Init) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    OleInitialize(NULL);
    NAN_EXPORT(target, DoDragDrop);
}

NODE_MODULE(addon, Init)
