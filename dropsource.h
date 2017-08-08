//
//	DROPSOURCE.CPP
//
//	Implementation of the IDropSource COM interface
//
//	By J Brown 2004 
//
//	www.catch22.net
//

#include <windows.h>

class OleDropSource : public IDropSource, public IDropSourceNotify
{
public:
	//
    // IUnknown members
	//
    HRESULT __stdcall QueryInterface	(REFIID iid, void ** ppvObject);
    ULONG   __stdcall AddRef			(void);
    ULONG   __stdcall Release			(void);
		
    //
	// IDropSource members
	//
    HRESULT __stdcall QueryContinueDrag	(BOOL fEscapePressed, DWORD grfKeyState);
	HRESULT __stdcall GiveFeedback		(DWORD dwEffect);
	
	//
	// IDropSourceNotify members
	//
	HRESULT __stdcall DragEnterTarget(HWND hwndTarget);
	HRESULT __stdcall DragLeaveTarget();

	//
    // Constructor / Destructor
	//
    OleDropSource();
    ~OleDropSource();
	
private:

    //
	// private members and functions
	//
    LONG	   m_lRefCount;
};

HRESULT CreateDropSource(IDropSource **ppDropSource);
