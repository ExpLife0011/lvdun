// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "ExplorerPlugin_i.h"
#include "dllmain.h"

CExplorerPluginModule _AtlModule;

BOOL IsThisDllLoadedByProcessNamedWith(LPCTSTR pszProcessName);

HINSTANCE g_hThisModule = NULL;
BOOL g_bIsLoadedByExplorer = FALSE;
BOOL g_bIsLoadedByNewExplorer = FALSE;
// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (!g_hThisModule)
	{
		g_hThisModule = hInstance;

		// ��һ�ν���ʱ���ж���ͼ���ر�dll�Ľ����Ƿ���explorer.exe
		if (IsThisDllLoadedByProcessNamedWith(_T("explorer.exe")) || IsThisDllLoadedByProcessNamedWith(_T("iexplore.exe")))
		{
			g_bIsLoadedByExplorer = TRUE;
		}
		else
		{
			BOOL bAllowed = FALSE;
			LPTSTR pszName = _T("srcfilename");
			LPTSTR pszValueTarget = _T("mysoftwaresetup");
			TCHAR szBuf[MAX_PATH] = {0};
			LPTSTR pszValue = ((::GetEnvironmentVariable(pszName, szBuf, sizeof(szBuf)/sizeof(szBuf[0])) > 0) ? szBuf : NULL);
			if (pszValue)
			{
				if (_tcscmp(pszValue, pszValueTarget) == 0)
				{
					bAllowed = TRUE;
				}
				else
				{
					bAllowed = IsThisDllLoadedByProcessNamedWith(_T("verclsid.exe"));
				}
			}
			else
			{
				bAllowed = IsThisDllLoadedByProcessNamedWith(_T("verclsid.exe"));
			}

			if (!bAllowed)
			{
				return FALSE;
			}
		}
	}
	if (g_bIsLoadedByExplorer)
	{
		// ���ˣ����ر�dll�Ľ��̱���explorer.exe
		static HANDLE s_hMutex = NULL; // ע��:��������������ֻ��һ��explorer.exe���ص�Ŀ��,û����ͼռ��,Ҳ�Ͳ���Ҫ�ͷ�
		LPCTSTR pszNameOfMutex = ONEPROCESSMUTEX;
		switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			{
				s_hMutex = ::CreateMutex(NULL, FALSE, pszNameOfMutex); // ����֮ʱ������ͼռ����
				DWORD dwLastError = ::GetLastError();
				if (!s_hMutex)
				{
					return FALSE;
				}
				else
				{
					if (ERROR_ALREADY_EXISTS == dwLastError)
					{
						// ��dll�Ѿ���explorer.exe������ʵ�����ع�����˲��ټ���
						// �ر�֮ǰ�Ļ�����
						::CloseHandle(s_hMutex);
						s_hMutex = NULL;
						g_bIsLoadedByNewExplorer = TRUE;
						return FALSE;
					}

					// ���ˣ���dll�Ǳ�explorer.exe�ĵ�һ��ʵ�����أ���������Ҫ��
				}
			}
			break;

		case DLL_PROCESS_DETACH:
			{
				// �ر�֮ǰ�����Ļ�����
				if (s_hMutex)
				{
					::CloseHandle(s_hMutex);
					s_hMutex = NULL;
					//TSDEBUG4CXX(_T("Close the mutex"));
				}
			}
			break;

		case DLL_THREAD_ATTACH:
			{
				// ʲôҲ����
			}
			break;

		case DLL_THREAD_DETACH:
			{
				// ʲôҲ����
			}
			break;

		default:
			{
				// ʲôҲ����
			}
			break;
		}
	}
	return _AtlModule.DllMain(dwReason, lpReserved); 
}


BOOL IsThisDllLoadedByProcessNamedWith(LPCTSTR pszProcessName)
{
	BOOL bIsLoadedByTheProcess = FALSE;

	if (pszProcessName)
	{
		// ��1����ȡ���ر�dll�Ľ�����
		TCHAR szExeFileName[MAX_PATH] = {0};
		::GetModuleFileName(::GetModuleHandle(NULL), szExeFileName, sizeof(szExeFileName) / sizeof(szExeFileName[0]));
		::PathStripPath(szExeFileName);
		if (_tcsncicmp(szExeFileName, pszProcessName, _tcslen(pszProcessName)) == 0)
		{
			bIsLoadedByTheProcess = TRUE;
		}
	}

	return bIsLoadedByTheProcess;
}