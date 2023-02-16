#include "Windows.h"
#include <oleidl.h>
#include <iostream>
#include <functional>

class DragAndDropHandler : public IDropTarget
{

public:
	DragAndDropHandler(
		std::function<void(char *, size_t)> fileDropped,
		std::function<void(bool, bool)> fileHovered) : m_fileDropped(fileDropped), m_fileHovered(fileHovered){};
	ULONG AddRef() { return 1; }
	ULONG Release() { return 0; }
	// we handle drop targets, let others know
	HRESULT QueryInterface(REFIID riid, void **ppvObject)
	{
		if (riid == IID_IDropTarget)
		{
			*ppvObject = this; // or static_cast<IUnknown*> if preferred
			// AddRef() if doing things properly
			// but then you should probably handle IID_IUnknown as well;
			return S_OK;
		}

		*ppvObject = NULL;
		return E_NOINTERFACE;
	};

	//--- implement the IDropTarget parts

	// occurs when we drag files into our applications view
	HRESULT DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
	{
		// TODO: check whether we can handle this type of object at all and set *pdwEffect &= DROPEFFECT_NONE if not;

		// do something useful to flag to our application that files have been dragged from the OS into our application
		// ...

		// trigger MouseDown for button 1 within ImGui
		// ...
		m_fileHovered(true, true);
		*pdwEffect &= DROPEFFECT_COPY;
		return S_OK;
	}

	// occurs when we drag files out from our applications view
	HRESULT DragLeave()
	{
		m_fileHovered(false, true);
		return S_OK;
	}

	// occurs when we drag the mouse over our applications view whilst carrying files (post Enter, pre Leave)
	HRESULT DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
	{
		// trigger MouseMove within ImGui, position is within pt.x and pt.y
		// grfKeyState contains flags for control, alt, shift etc
		// ...

		*pdwEffect &= DROPEFFECT_COPY;
		return S_OK;
	}

	// occurs when we release the mouse button to finish the drag-drop operation
	HRESULT Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
	{
		m_fileHovered(false, true);
		// grfKeyState contains flags for control, alt, shift etc
		std::cout << "Drag over..." << std::endl;

		// render the data into stgm using the data description in fmte
		FORMATETC fmte = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
		STGMEDIUM stgm;

		if (SUCCEEDED(pDataObj->GetData(&fmte, &stgm)))
		{
			HDROP hdrop = (HDROP)stgm.hGlobal; // or reinterpret_cast<HDROP> if preferred
			UINT file_count = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);

			// we can drag more than one file at the same time, so we have to loop here
			for (UINT i = 0; i < file_count; i++)
			{
				TCHAR szFile[MAX_PATH];
				UINT cch = DragQueryFile(hdrop, i, szFile, MAX_PATH);
				m_fileDropped(szFile, MAX_PATH);
				if (cch > 0 && cch < MAX_PATH)
				{
					// szFile contains the full path to the file, do something useful with it
					// i.e. add it to a vector or something
					std::cout << szFile << std::endl;
				}
			}

			// we have to release the data when we're done with it
			ReleaseStgMedium(&stgm);

			// notify our application somehow that we've finished dragging the files (provide the data somehow)
			// ...
		}

		// trigger MouseUp for button 1 within ImGui
		// ...

		*pdwEffect &= DROPEFFECT_COPY;
		return S_OK;
	}

private:
	std::function<void(char *, size_t)> m_fileDropped;
	std::function<void(bool, bool)> m_fileHovered;
};