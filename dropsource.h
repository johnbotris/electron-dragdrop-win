#pragma once
class OleDropSource: public IDropSource
{
private:
	ULONG m_lRefCount = 0;
	IDropSourceNotify* m_pDropSourceNotify = nullptr;

public:
	OleDropSource(IDropSourceNotify* pDropSourceNotify);
	~OleDropSource();

	static HRESULT Create(IDropSourceNotify* pDropSourceNotify, IDropSource **ppDropSource);

	// IUnknown methods
	STDMETHOD(QueryInterface)(REFIID iid, LPVOID* ppvObject);
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();

	// IDropSource methods
	STDMETHOD(QueryContinueDrag)(BOOL fEscapePressed, DWORD grfKeyState);
	STDMETHOD(GiveFeedback)(DWORD dwEffect);
};

