#pragma once
class OleDropSourceNotify: public IDropSourceNotify
{
private:
	ULONG m_lRefCount = 0;

public:
	OleDropSourceNotify();
	~OleDropSourceNotify();

	static HRESULT Create(IDropSourceNotify** ppDropSourceNotify);

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDropSourceNotify methods
	STDMETHOD(DragEnterTarget)(HWND hwndTarget);
	STDMETHOD(DragLeaveTarget)();
};

