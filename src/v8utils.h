#include <nan.h>

template <typename T>
v8::MaybeLocal<T> MaybeGet(v8::MaybeLocal<v8::Object> object, const char* key) {
    return MaybeGet<T>(object, Nan::New<v8::String>(key));
}

template <typename T>
v8::MaybeLocal<T> MaybeGet(v8::MaybeLocal<v8::Object> object,
                           v8::MaybeLocal<v8::String> key) {
    if (object.IsEmpty() || key.IsEmpty()) {
    }

    return ConvertMaybeLocal<v8::Value, T>(
        Nan::Get(object.ToLocalChecked(), key.ToLocalChecked()));
}

template <typename T>
v8::MaybeLocal<T> MaybeGet(v8::MaybeLocal<v8::Object> array, uint32_t idx) {
    if (array.IsEmpty()) {
        return Nan::MaybeLocal<T>();
    }

    return ConvertMaybeLocal<v8::Value, T>(
        Nan::Get(array.ToLocalChecked(), idx));
}

template <typename FromT, typename ToT>
v8::MaybeLocal<ToT> ConvertMaybeLocal(v8::MaybeLocal<FromT> from) {
    if (from.IsEmpty()) {
        return Nan::MaybeLocal<ToT>();
    }

    v8::Local<FromT> localFrom;
    bool success = from.ToLocal<FromT>(&localFrom);

    if (!success) {
        return Nan::MaybeLocal<ToT>();
    } else {
        return Nan::MaybeLocal<ToT>(localFrom.As<ToT>());
    }
}