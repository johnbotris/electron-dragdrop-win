#pragma once

#include <memory>

#include <nan.h>

#include <windows.h>
#include <shellapi.h>
#include <oleidl.h>

#include "ole/DataObject.h"
#include "ole/DropSource.h"

class Worker : public Nan::AsyncWorker {
   private:
    HRESULT result;

   public:
    Worker(Nan::Callback* callback, std::shared_ptr<Queue> queue);

    ~Worker();

    // No v8 Objects are allowed inside of this method
    virtual void Execute() override;

   protected:
    virtual void HandleOKCallback() override;
};
