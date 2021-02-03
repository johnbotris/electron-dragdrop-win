#include "options.h"

using v8::Local;
using v8::Object;
using v8::String;
using v8::Context;
using v8::Isolate;
using v8::Value;
using v8::Function;

Options::Options(v8::Local<v8::Object> options) {
    this->formats = Nan::Get(options, Nan::New("formats").ToLocalChecked()).ToLocalChecked();
    if (this->formats.IsEmpty())
    {
        Nan::ThrowError("formats is empty.");
    }
    if (!this->formats->IsObject())
    {
        Nan::ThrowError("formats must be an object.");
    }

    // this->formats = formatsValue.As<v8::Object>();
    // this->propertyNames = Nan::GetPropertyNames(formats).ToLocalChecked();

    Local<Context> context = Nan::GetCurrentContext();
    Isolate *isolate = context->GetIsolate();

    v8::Local<v8::Value> eventsValue = Nan::Get(options, Nan::New("events").ToLocalChecked()).ToLocalChecked();

    if (!eventsValue.IsEmpty())
    {
        if (!eventsValue->IsObject())
        {
            Nan::ThrowError("events must be an object.");
        }

        if (eventsValue->IsObject())
        {
            v8::Local<v8::Object> events = eventsValue->ToObject(context).ToLocalChecked();
            this->completedFunc = GetFunction(events, "completed");
            this->dragEnterFunc = GetFunction(events, "dragEnter");
            this->dragLeaveFunc = GetFunction(events, "dragLeave");
        }
    }

    v8::Local<v8::Value> windowHandleValue = Nan::Get(options, Nan::New("windowHandle").ToLocalChecked()).ToLocalChecked();

    if (!windowHandleValue.IsEmpty() || windowHandleValue->IsArrayBufferView())
    {
        auto windowHandleBuffer = windowHandleValue.As<v8::ArrayBufferView>();
        auto len = windowHandleBuffer->ByteLength();
        windowHandleBuffer->CopyContents(&this->hWndParent, len);
        if (!::IsWindow(this->hWndParent))
        {
            this->hWndParent = nullptr;
        }
    }
}

Options::~Options()
{
    
    if (!this->completedFunc.IsEmpty())
    {
        this->completedFunc.Clear();
    }
    if (!this->dragEnterFunc.IsEmpty())
    {
        this->dragEnterFunc.Clear();
    }
    if (!this->dragLeaveFunc.IsEmpty())
    {
        this->dragLeaveFunc.Clear();
    }
}

HWND Options::GetParentHwnd()
{
    return this->hWndParent;
}

v8::Local<v8::Function> Options::GetCompletedCallbackFunction()
{
    return this->completedFunc;
}

v8::Local<v8::Function> Options::GetDragEnterCallbackFunction()
{
    return this->dragEnterFunc;
}

v8::Local<v8::Function> Options::GetDragLeaveCallbackFunction()
{
    return this->dragLeaveFunc;
}

