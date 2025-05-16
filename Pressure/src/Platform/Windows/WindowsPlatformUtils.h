#pragma once

#include "Pressure/Utils/PlatformUtils.h"

#include <objbase.h>      // For COM headers
#include <shlwapi.h>
#include <shlobj.h>
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <windows.h>      // For common windows data types and function headers

namespace Pressure
{

	class WindowsPlatformUtils
	{
	public:
		static std::wstring ToWideString(const std::string& str);
		static std::string ToString(const std::wstring& wstr);
		static void AddFilters(IFileDialog* pfd, const std::vector<FileFilter>& filters);
		static std::string OpenInternal(_In_ REFCLSID dialogType, const std::vector<FileFilter>& filters);
	};


	// Allows us to set the default extension to the selected file type when it changes
	class CDialogEventHandler : public IFileDialogEvents
	{
	public:
		// IUnknown methods
		IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
		{
			if (riid == IID_IUnknown || riid == IID_IFileDialogEvents) {
				*ppv = static_cast<IFileDialogEvents*>(this);
				AddRef();
				return S_OK;
			}
			*ppv = nullptr;
			return E_NOINTERFACE;
		}

		IFACEMETHODIMP_(ULONG) AddRef()
		{
			return InterlockedIncrement(&m_cRef);
		}

		IFACEMETHODIMP_(ULONG) Release()
		{
			long cRef = InterlockedDecrement(&m_cRef);
			if (!cRef)
				delete this;
			return cRef;
		}

		// IFileDialogEvents methods
		IFACEMETHODIMP OnTypeChange(IFileDialog* pfd); 
		IFACEMETHODIMP OnFileOk(IFileDialog*) { return S_OK; };
		IFACEMETHODIMP OnFolderChange(IFileDialog*) { return S_OK; };
		IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) { return S_OK; };
		IFACEMETHODIMP OnHelp(IFileDialog*) { return S_OK; };
		IFACEMETHODIMP OnSelectionChange(IFileDialog*) { return S_OK; };
		IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) { return S_OK; };
		IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) { return S_OK; };

		UINT GetSelectedFileTypeIndex() const { return m_SelectedFileTypeIndex; }

		CDialogEventHandler(const std::vector<FileFilter>& filters) : m_cRef(1) {};
	private:
		~CDialogEventHandler() {}
		long m_cRef;
		UINT m_SelectedFileTypeIndex;
	};

}
