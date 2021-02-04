#pragma once

#include <nan.h>

class Options {
   public:
    Options(v8::Local<v8::Object> options);
    ~Options();

    HWND GetParentHwnd();

    v8::Local<v8::Object>& GetFormats();

    void OnDragEnter(v8::Local<v8::String> windowText);
    void OnDragLeave();
    void OnDragCompleted(v8::Local<v8::Number> returnCode);

   private:
    HWND hWndParent = nullptr;
    v8::Local<v8::Object> formats;
    v8::Local<v8::Function> completedCallback;
    v8::Local<v8::Function> dragEnterCallback;
    v8::Local<v8::Function> dragLeaveCallback;
};
