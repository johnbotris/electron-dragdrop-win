#include "options.h"
#include "utils.h"

#include <array>

using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;
using v8::ArrayBufferView;
using v8::Function;
using v8::Number;

Options::Options(Local<Object> options) {
    this->formats =
        Nan::To<Object>(Nan::Get(options, Nan::New("formats").ToLocalChecked())
                            .ToLocalChecked())
            .ToLocalChecked();

    if (this->formats.IsEmpty()) {
        Nan::ThrowError("formats is empty.");
    }
    if (!this->formats->IsObject()) {
        Nan::ThrowError("formats must be an object.");
    }

    Local<Value> eventsValue =
        Nan::Get(options, Nan::New("events").ToLocalChecked()).ToLocalChecked();

    if (!eventsValue.IsEmpty()) {
        Local<Object> events = Nan::To<Object>(eventsValue).ToLocalChecked();
        this->completedCallback = getFunction(events, "completed");
        this->dragEnterCallback = getFunction(events, "dragEnter");
        this->dragLeaveCallback = getFunction(events, "dragLeave");
    }

    Local<Value> windowHandleValue =
        Nan::Get(options, Nan::New("windowHandle").ToLocalChecked())
            .ToLocalChecked();

    if (!windowHandleValue.IsEmpty() ||
        windowHandleValue->IsArrayBufferView()) {
        auto windowHandleBuffer = windowHandleValue.As<ArrayBufferView>();
        auto len = windowHandleBuffer->ByteLength();
        windowHandleBuffer->CopyContents(&this->hWndParent, len);
        if (!::IsWindow(this->hWndParent)) {
            this->hWndParent = nullptr;
        }
    }
}

Options::~Options() {
    this->formats.Clear();
    this->completedCallback.Clear();
    this->dragEnterCallback.Clear();
    this->dragLeaveCallback.Clear();
}

HWND Options::GetParentHwnd() {
    return this->hWndParent;
}

Local<Object>& Options::GetFormats() {
    return this->formats;
}

void Options::OnDragEnter(Local<String> windowText) {
    if (this->dragEnterCallback->IsFunction()) {
        Local<Object> data = Nan::New<Object>();
        data->Set(Nan::GetCurrentContext(),
                  Nan::New("windowText").ToLocalChecked(), windowText);

        std::array<v8::Local<v8::Value>, 1> args = {data};

        Nan::Call(Nan::Callback(this->dragEnterCallback), args.size(),
                  &args[0]);
    }
}

void Options::OnDragLeave() {
    if (this->dragLeaveCallback->IsFunction()) {
        Nan::Call(Nan::Callback(this->dragLeaveCallback), 0, nullptr);
    }
}

void Options::OnDragCompleted(Local<Number> returnCode) {
    if (this->completedCallback->IsFunction()) {
        std::array<Local<Value>, 1> args = {returnCode};

        Nan::Call(Nan::Callback(this->completedCallback), args.size(),
                  &args[0]);
    }
}