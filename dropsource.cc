//
//	DROPSOURCE.CPP
//
//	Implementation of the IDropSource COM interface
//
//	By J Brown 2004 
//
//	www.catch22.net
//

#include "dropsource.h"

//
//	Constructor
//
OleDropSource::OleDropSource() 
{
	m_lRefCount = 1;
}

//
//	Destructor
//
OleDropSource::~OleDropSource()
{
}

//
//	IUnknown::AddRef
//
ULONG __stdcall OleDropSource::AddRef(void)
{
    // increment object reference count
    return InterlockedIncrement(&m_lRefCount);
}

//
//	IUnknown::Release
//
ULONG __stdcall OleDropSource::Release(void)
{
    // decrement object reference count
	LONG count = InterlockedDecrement(&m_lRefCount);
		
	if(count == 0)
	{
		delete this;
		return 0;
	}
	else
	{
		return count;
	}
}

//
//	IUnknown::QueryInterface
//
HRESULT __stdcall OleDropSource::QueryInterface(REFIID iid, void **ppvObject)
{
    // check to see what interface has been requested
    if(iid == IID_IDropSource || iid == IID_IUnknown)
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
}

//
//	OleDropSource::QueryContinueDrag
//
//	Called by OLE whenever Escape/Control/Shift/Mouse buttons have changed
//
HRESULT __stdcall OleDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	// if the <Escape> key has been pressed since the last call, cancel the drop
	if(fEscapePressed == TRUE)
		return DRAGDROP_S_CANCEL;	

	// if the <LeftMouse> button has been released, then do the drop!
	if((grfKeyState & MK_LBUTTON) == 0)
		return DRAGDROP_S_DROP;

	// continue with the drag-drop
	return S_OK;
}

//
//	OleDropSource::GiveFeedback
//
//	Return either S_OK, or DRAGDROP_S_USEDEFAULTCURSORS to instruct OLE to use the
//  default mouse cursor images
//
HRESULT __stdcall OleDropSource::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

//
//	OleDropSource::DragEnterTarget
//
//	Return S_OK
//
HRESULT __stdcall OleDropSource::DragEnterTarget(HWND hwndTarget)
{
	return S_OK;
}

//
//	OleDropSource::DragLeaveTarget
//
//	Return S_OK
//
HRESULT __stdcall OleDropSource::DragLeaveTarget()
{
	return S_OK;
}

//
//	Helper routine to create an IDropSource object
//	
HRESULT CreateDropSource(IDropSource **ppDropSource)
{
	if(ppDropSource == 0)
		return E_INVALIDARG;

	*ppDropSource = new OleDropSource();

	return (*ppDropSource) ? S_OK : E_OUTOFMEMORY;

}