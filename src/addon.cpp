// Linker can't find shlwapi without this????
#pragma comment(lib, "Shlwapi.lib")

#include <nan.h>

#include <shlwapi.h>
#include <windows.h>
#include <comdef.h>
#include <shlobj_core.h>

#include "ole/wil/resource.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <memory>
#include <sstream>
#include <algorithm>
#include <string>
#include <codecvt>
#include <locale>

#include "v8utils.h"
#include "Worker.h"
#include "ole/DataObject.h"
#include "ole/DropSource.h"
#include "ole/ole.h"
#include "ole/Stream.h"

using namespace std;
using namespace v8;

NAN_METHOD(DoDragDrop) {
    auto argc = info.Length();

    if (argc < 2) {
        Nan::ThrowError("Missing parameter \"dataPromise\".");
        return;
    }

    // if (!info[0]->IsPromise()) {
    //     Nan::ThrowError("Parameter \"options\" is not a Promise");
    //     return;
    // }

    // TODO replace with commented block above
    if (!info[0]->IsArray()) {
        Nan::ThrowError("Parameter \"options\" is not an Array");
        return;
    }

    if (!info[argc - 1]->IsFunction()) {
        Nan::ThrowError(
            "Drag and drop internal error: Expected callback function");
        return;
    }

    Local<Object> v8Files = info[0].As<Object>();
    auto numFiles = v8Files.As<Array>()->Length();
    vector<pair<FILEDESCRIPTOR, STGMEDIUM>> files;

    for (auto fileIdx = 0; fileIdx < numFiles; ++fileIdx) {
        wstring utf16FileName;
        BYTE* fileData;
        size_t fileSize;
        {
            MaybeLocal<Object> fileInfo = MaybeGet<Object>(v8Files, fileIdx);
            Local<String> name;
            Local<Uint8Array> data;
            bool success =
                MaybeGet<String>(fileInfo, "name").ToLocal<String>(&name) &&
                MaybeGet<Uint8Array>(fileInfo, "data")
                    .ToLocal<Uint8Array>(&data);

            if (!success || name->IsUndefined() || data->IsUndefined()) {
                stringstream msg;
                msg << "Invalid Object at index " << fileIdx
                    << " of argument files";
                Nan::ThrowError(msg.str().c_str());
                return;
            }

            string utf8FileName = *Nan::Utf8String(name.As<Value>());
            wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
            utf16FileName = converter.from_bytes(utf8FileName);
            fileSize = data->ByteLength();
            fileData = new BYTE[fileSize];
            data->CopyContents(fileData, fileSize);
        }

        FILEDESCRIPTOR descriptor;
        descriptor.dwFlags = FD_FILESIZE | FD_PROGRESSUI;
        descriptor.nFileSizeHigh = static_cast<DWORD>(fileSize >> 16);
        descriptor.nFileSizeLow = static_cast<DWORD>(fileSize & 0xffff);
        wcscpy_s(descriptor.cFileName, MAX_PATH, utf16FileName.c_str());

        STGMEDIUM storage;
        storage.tymed = TYMED_ISTREAM;
        storage.pUnkForRelease = nullptr;
        // storage.pstm = SHCreateMemStream(fileData, fileSize);
        storage.pstm = new Stream(vector<BYTE>(fileData, fileData + fileSize),
                                  utf16FileName);
        delete[] fileData;
        files.emplace_back(descriptor, storage);
    }

    SIZE_T descriptorSize =
        sizeof(FILEGROUPDESCRIPTOR) + sizeof(FILEDESCRIPTOR) * numFiles;

    STGMEDIUM groupDescriptorStorage;
    groupDescriptorStorage.tymed = TYMED_HGLOBAL;
    groupDescriptorStorage.pUnkForRelease = nullptr;
    groupDescriptorStorage.hGlobal = GlobalAlloc(GHND, descriptorSize);
    {
        wil::unique_hglobal_locked ptr(groupDescriptorStorage.hGlobal);
        FILEGROUPDESCRIPTOR* groupDescriptor =
            static_cast<FILEGROUPDESCRIPTOR*>(ptr.get());

        groupDescriptor->cItems = numFiles;

        for (auto i = 0; i <= numFiles; ++i) {
            FILEDESCRIPTOR descriptor = files[i].first;
            groupDescriptor->fgd[i] = descriptor;
        }
    }

    // Local<Promise> dataPromise = info[0].As<Promise>();
    Local<Function> callback = info[argc - 1].As<Function>();

    initOle();

    DWORD dwEffect;
    auto dataObject = new DataObject(groupDescriptorStorage, files);
    auto dropSource = new DropSource();

    dataObject->SetInDragLoop(true);
    cout << "Drag start" << endl;
    auto result = DoDragDrop(dataObject, dropSource,
                             DROPEFFECT_COPY | DROPEFFECT_MOVE, &dwEffect);

    wcout << L"Drag end. result: " << _com_error(result).ErrorMessage()
          << ". effect: " << dwEffect << endl;

    dataObject->SetInDragLoop(false);

    BOOL isInAsyncOperation;
    dataObject->InOperation(&isInAsyncOperation);

    if (isInAsyncOperation) {
        cout << "in operation" << endl;

    } else {
        cout << "not in operation" << endl;
        // dropSource->Release();
        // dataObject->Release();
        // ::OleUninitialize();
    }

    Local<Value> error = Nan::Null();
    Local<Value> data = Nan::Null();

    if (result != DRAGDROP_S_DROP && result != DRAGDROP_S_CANCEL) {
        cout << "error code " << std::hex << result << endl;
        stringstream message;
        message << "Encountered error: ";
        message << _com_error(result).ErrorMessage();
        message << ". Code: " << result;
        error = Nan::New<String>(message.str()).ToLocalChecked();
    }
    {
        const int argc = 2;
        Local<Value> args[argc]{error, data};
        Nan::Call(Nan::Callback(callback), argc, args);
    }
    // Nan::AsyncQueueWorker(worker);
}

NAN_MODULE_INIT(Init) {
    NAN_EXPORT(target, DoDragDrop);
}

NODE_MODULE(addon, Init)

// shared_ptr<Queue>* queuePtr = new std::shared_ptr<Queue>(new Queue(3));

// auto worker =
//     new Worker(new Nan::Callback(callback),
//     shared_ptr<Queue>(*queuePtr));

// auto dataReadyHandler = [](Nan::NAN_METHOD_ARGS_TYPE info) {
//     Local<External> data = info.Data().As<External>();
//     shared_ptr<Queue>* queue =
//         static_cast<shared_ptr<Queue>*>(data->Value());

//     auto filePaths = vector<string>();

//     // Convert the javascript array of javascript strings into a c++
//     vector
//     // of c++ strings
//     if (info.Length() > 0 && info[0]->IsArray()) {
//         Local<Array> array = info[0].As<Array>();
//         uint32_t count = array->Length();
//         for (uint32_t i = 0; i < count; ++i) {
//             MaybeLocal<Value> value = Nan::Get(array, i);
//             if (!value.IsEmpty() && value.ToLocalChecked()->IsString()) {
//                 string filePath =
//                     *Nan::Utf8String(value.ToLocalChecked().As<String>());
//             }
//         }
//     }

//     // TODO here is where we put whatever data we were waiting for
//     (*queue)->push(filePaths);
//     delete queue;
// };

// auto dataReadyCallback = Nan::GetFunction(Nan::New<FunctionTemplate>(
//     dataReadyHandler, Nan::New<External>(static_cast<void*>(queuePtr))));

// dataPromise->Then(Nan::GetCurrentContext(),
//   dataReadyCallback.ToLocalChecked());
