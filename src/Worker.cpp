#include "Worker.h"

#include <iostream>

using namespace std;
using namespace v8;

Worker::Worker(Nan::Callback* callback, shared_ptr<Queue> queue)
    : Nan::AsyncWorker(callback, "DoDragDrop::DragDropWorker") {
    this->dropSource = new DropSource();
    this->dataObject = new DataObject(queue);
    this->result = 0;
}

Worker::~Worker() {
    dropSource->Release();
    dataObject->Release();
}

void Worker::Execute() {
    // cout << "native is waiting for javascript..." << endl;
    // vector<string>* data = nullptr;
    // while ((data = this->queue->front()) == nullptr) {
    // }
    // std::cout << "got some data from javascript: \"" << *data << '"'
    //           << std::endl;
    // this->queue->pop();

    cout << "Drag start" << endl;
    DWORD dwEffect;
    this->result = DoDragDrop(this->dataObject, this->dropSource,
                              DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);
    cout << "Drag end" << endl;
}

void Worker::HandleOKCallback() {
    Nan::HandleScope scope;

    Local<Value> error = Nan::Null();
    Local<Value> data = Nan::Null();

    if (this->result != DRAGDROP_S_DROP && this->result != DRAGDROP_S_CANCEL) {
        error = Nan::New<String>("An unknown error occurred").ToLocalChecked();
    }

    const int argc = 2;
    Local<Value> args[argc]{error, data};

    this->callback->Call(argc, args, this->async_resource);
}
