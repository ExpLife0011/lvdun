// Agent.cpp : Implementation of CAgent

#include "stdafx.h"
#include "Agent.h"
#include <comdef.h>
#include <process.h>
#include <string>
// CAgent

enum BrowserTaskType
{
	IMAGE=0,
	OTHER
};

struct FRBrowserTaskInfo
{
	//std::wstring wstrUrl;
	wchar_t wszUrl[1024];
	BrowserTaskType type;
	int posX;
	int posY;
};

HWND GetHwndMsgListenerOK(bool bRuning = false)
{
	HANDLE hMutex;
	DWORD dwRet;
	//互斥量存在则说明消息监听ok
	for(int i = 0; i < 5; ++i){
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"xarmutex_{455EB122-3F18-4139-AE47-255F940CBCF0}");
		if (hMutex == NULL){
			if(!bRuning){
				return NULL;
			} else {
				Sleep(100*i);
			}
		} else {
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			break;
		}
	}
	HWND hwnd;
	for(int i = 0; i < 3; ++i){
		hwnd = ::FindWindowA("{1DA1F328-DB3D-4f6a-A62E-E9B2B22F2B9A}_frmainmsg", NULL);
		if(hwnd != NULL){
			return hwnd;
		} else {
			Sleep(100);
		}
	}
	return NULL;
}

HWND __stdcall CreateProcessAndGetHwnd()
{
	HWND hwnd = GetHwndMsgListenerOK();
	if(hwnd != NULL){
		return hwnd;
	}
	HKEY hKEY;
	LPCSTR data_Set= "Software\\FlyRabbit";
	if (ERROR_SUCCESS != ::RegOpenKeyExA(HKEY_LOCAL_MACHINE,data_Set,0,KEY_READ,&hKEY)){
		::RegCloseKey(hKEY);
		return NULL;
	}
	char szPathValue[MAX_PATH] = {0};
	DWORD dwSize = sizeof(szPathValue);
	DWORD dwType = REG_SZ;
	if (::RegQueryValueExA(hKEY,"Path", 0, &dwType, (LPBYTE)szPathValue, &dwSize) != ERROR_SUCCESS){
		::RegCloseKey(hKEY);
		return NULL;
	}
	char szBrowserPath[MAX_PATH] = {0}, szBrowserName[128] = {0};
	if(!GetModuleFileNameA(NULL, szBrowserPath, MAX_PATH)){
		strcpy(szBrowserName, "unknown");
	} else {
		strcpy(szBrowserName, PathFindFileNameA(szBrowserPath));
	}
	char szCmdLine[MAX_PATH] = {0};
	sprintf(szCmdLine, "/sstartfrom FlyRabbitAgent /browsername %s /embedding", szBrowserName);
	STARTUPINFOA si = {sizeof(si)};
	PROCESS_INFORMATION pi;
	si.dwFlags=STARTF_USESHOWWINDOW;//指定wShowWindow成员有效
	si.wShowWindow=TRUE;//此成员设为TRUE的话则显示新建进程的主窗口
	BOOL bRet=CreateProcessA(
		szPathValue,
		szCmdLine,
		NULL,
		NULL,
		FALSE,
		HIGH_PRIORITY_CLASS,
		NULL,
		NULL,
		&si,
		&pi);
	if(bRet){
		WaitForInputIdle(pi.hProcess, 5000);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	hwnd = GetHwndMsgListenerOK(true);
	::RegCloseKey(hKEY);
	return hwnd;
}

unsigned int __stdcall ThreadFun(PVOID pM)  
{
	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = sizeof(FRBrowserTaskInfo);
	cpd.lpData = pM;
	HWND hwnd = CreateProcessAndGetHwnd();
	if(hwnd != NULL){
		::SendMessageA(hwnd, WM_COPYDATA, 1, (LPARAM)&cpd);
		delete []pM;
	} else {
		delete []pM;
		MessageBoxA(0, "未安装飞兔", "提醒", 0);
	}
	return 0;
};

STDMETHODIMP CAgent::AddTask(VARIANT varUrl, VARIANT varType, VARIANT varPoint)
{
	// TODO: Add your implementation code here
	//
	_bstr_t bstrUrl = varUrl.bstrVal;
	FRBrowserTaskInfo *psBTI = new FRBrowserTaskInfo;
	wcscpy(psBTI->wszUrl, bstrUrl);
	psBTI->posX = 0;
	psBTI->posY = 0;
	psBTI->type = OTHER;
	if(varPoint.vt == VT_DISPATCH){
		CComDispatchDriver spData = varPoint.pdispVal;
		CComVariant varValueX, varValueY;
		spData.GetPropertyByName(L"x", &varValueX);
		spData.GetPropertyByName(L"y", &varValueY);
		psBTI->posX = varValueX.intVal;
		psBTI->posY = varValueY.intVal;
		psBTI->type = IMAGE;
	}
	_beginthreadex(NULL, 0, ThreadFun, psBTI, 0, NULL);
	return S_OK;
}