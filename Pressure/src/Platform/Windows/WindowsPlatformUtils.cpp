#include "prspch.h"
#include "WindowsPlatformUtils.h"
#include "Pressure/Utils/PlatformUtils.h"

#include <windows.h>      // For common windows data types and function headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shtypes.h>      // for COMDLG_FILTERSPEC

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "Pressure/Core/Application.h"

namespace Pressure
{

	std::string FileDialogs::OpenFile(const std::vector<FileFilter>& filters)
	{
		return WindowsPlatformUtils::OpenInternal(CLSID_FileOpenDialog, filters);
	}

	std::string FileDialogs::SaveFile(const std::vector<FileFilter>& filters)
	{
		return WindowsPlatformUtils::OpenInternal(CLSID_FileSaveDialog, filters);
	}

	std::wstring WindowsPlatformUtils::ToWideString(const std::string& str)
	{
		if (str.empty()) return std::wstring();

		// Use Windows API for proper UTF-8 to UTF-16 conversion
		int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], sizeNeeded);

		// Remove the null terminator included in the size
		result.resize(wcslen(result.c_str()));

		return result;
	}

	std::string WindowsPlatformUtils::ToString(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();

		// Use Windows API for proper UTF-16 to UTF-8 conversion
		int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], sizeNeeded, nullptr, nullptr);

		// Remove the null terminator included in the size
		result.resize(strlen(result.c_str()));

		return result;
	}

	void WindowsPlatformUtils::AddFilters(IFileDialog* pfd, const std::vector<FileFilter>& filters)
	{
		if (filters.empty()) {
			COMDLG_FILTERSPEC allFilesFilter[] = {
				{ L"All Files", L"*.*" }
			};
			pfd->SetFileTypes(1, allFilesFilter);
			return;
		}

		// Create array of COMDLG_FILTERSPEC
		std::vector<COMDLG_FILTERSPEC> fileTypes(filters.size());

		// Keep these strings alive until SetFileTypes is called
		std::vector<std::wstring> names;
		std::vector<std::wstring> extensions;

		names.reserve(filters.size());
		extensions.reserve(filters.size());

		for (size_t i = 0; i < filters.size(); i++) {
			names.push_back(WindowsPlatformUtils::ToWideString(filters[i].FilterName));
			extensions.push_back(WindowsPlatformUtils::ToWideString(filters[i].FilterExtension));

			fileTypes[i].pszName = names.back().c_str();
			fileTypes[i].pszSpec = extensions.back().c_str();
		}

		// Set the file types to the dialog
		pfd->SetFileTypes(static_cast<UINT>(fileTypes.size()), fileTypes.data());
	}

	std::string WindowsPlatformUtils::OpenInternal(_In_ REFCLSID dialogType, const std::vector<FileFilter>& filters)
	{
		IFileDialog* pfd = NULL;
		HRESULT hr = CoCreateInstance(dialogType, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
		if (SUCCEEDED(hr))
		{
			// Set the options on the dialog
			DWORD dwFlags;

			// Before setting, always get the options first in order not to override existing options.
			hr = pfd->GetOptions(&dwFlags);
			if (SUCCEEDED(hr))
			{
				// Set the file types to display only. Notice that, this is a 1-based array.
				WindowsPlatformUtils::AddFilters(pfd, filters);

				// Create and register the event handler
				CDialogEventHandler* eventHandler = new CDialogEventHandler(filters);
				DWORD cookie = 0;
				hr = pfd->Advise(eventHandler, &cookie);
				if (SUCCEEDED(hr))
				{
					// Get the current working directory
					CHAR currentDir[256];
					if (!GetCurrentDirectoryA(256, currentDir))
					{
						return std::string();
					}

					IShellItem* pFolder;
					hr = SHCreateItemFromParsingName(WindowsPlatformUtils::ToWideString(std::string(currentDir)).c_str(), nullptr, IID_PPV_ARGS(&pFolder));
					if (SUCCEEDED(hr))
					{
						// Set the default directory to the process current directory
						pfd->SetDefaultFolder(pFolder);

						// Set the default extension to the first filter
						hr = pfd->SetDefaultExtension(WindowsPlatformUtils::ToWideString(filters[0].FilterExtension).c_str());
						if (SUCCEEDED(hr))
						{
							// Show the dialog parented to the current native window
							hr = pfd->Show(glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow()));
							if (SUCCEEDED(hr))
							{
								IShellItem* psiResult;
								hr = pfd->GetResult(&psiResult);
								if (SUCCEEDED(hr))
								{
									PWSTR pszFilePath = NULL;
									psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
									if (SUCCEEDED(hr))
									{
										std::string result = ToString(pszFilePath);
										CoTaskMemFree(pszFilePath);
										psiResult->Release();
										pfd->Release();

										// Check and add extension if needed
										std::filesystem::path filePath(result);
										if (!filePath.has_extension() && !filters.empty()) {
											// Add the selected filter extension as default extension at the end of the file name
											filePath.replace_extension(filters[eventHandler->GetSelectedFileTypeIndex()].FilterExtension.substr(1)); // Delete the '*'
											result = filePath.string();
										}

										return result;
									}
								}
								psiResult->Release();
							}
						}
					}
					pFolder->Release();
				}
				eventHandler->Release();
				pfd->Unadvise(cookie);
			}
		}
		pfd->Release();

		return std::string();
	}

	HRESULT CDialogEventHandler::OnTypeChange(IFileDialog* pfd)
	{
		// Get the selected file type
		UINT fileTypeIndex = 0;
		HRESULT hr = pfd->GetFileTypeIndex(&fileTypeIndex);
		if (SUCCEEDED(hr))
		{
			m_SelectedFileTypeIndex = fileTypeIndex;
		}
		return S_OK;
	}

}
