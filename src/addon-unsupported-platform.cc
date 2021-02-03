#include <nan.h>

NAN_METHOD(DragDrop)
{
	Nan::ThrowError("Unsupported operating system.");
}

NAN_MODULE_INIT(Initialize)
{
	NAN_EXPORT(target, DragDrop);
}

NODE_MODULE(addon, Initialize)