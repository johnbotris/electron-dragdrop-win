#include "Worker.h"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace v8;

Worker::Worker(Nan::Callback* callback, shared_ptr<Queue> queue)
    : Nan::AsyncWorker(callback, "DoDragDrop::DragDropWorker") {
    this->result = 0;
}

Worker::~Worker() {}

void Worker::Execute() {}
void Worker::HandleOKCallback() {
    Nan::HandleScope scope;

    Local<Value> error = Nan::Null();
    Local<Value> data = Nan::Null();

    if (this->result != DRAGDROP_S_DROP && this->result != DRAGDROP_S_CANCEL) {
        cout << "error code " << std::hex << this->result << endl;
        error = Nan::New<String>("An unknown error occurred").ToLocalChecked();
    }

    const int argc = 2;
    Local<Value> args[argc]{error, data};

    this->callback->Call(argc, args, this->async_resource);
}
