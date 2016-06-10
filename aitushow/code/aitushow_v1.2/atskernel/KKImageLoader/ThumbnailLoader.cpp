#include "stdafx.h"
#include "ThumbnailLoader.h"
#include "ImageLoader.h"
#include "..\ImageHelper\ImageProcessor.h"
#include "../Utility/StringOperation.h"

void GetThumbnailSize(const int nSrcWidth, const int nSrcHeight, const int nDstWidth, const int nDstHeight, int& nThumbnailWidth, int& nThumbnailHeight)
{
	if (nSrcWidth <= nDstWidth && nSrcHeight <= nDstHeight)
	{
		nThumbnailWidth = nSrcWidth;
		nThumbnailHeight = nSrcHeight;
		return;
	}

	double dWidthZoom = (double)nSrcWidth/nDstWidth;
	double dHeightZoom = (double)nSrcHeight/nDstHeight;

	if (dWidthZoom > dHeightZoom)
	{
		nThumbnailWidth = nDstWidth;
		nThumbnailHeight = std::floor(nSrcHeight/dWidthZoom + 0.5);
		nThumbnailHeight = nThumbnailHeight>0?nThumbnailHeight:1;
	}
	else
	{
		nThumbnailWidth = std::floor(nSrcWidth/dHeightZoom + 0.5);
		nThumbnailWidth = nThumbnailWidth>0?nThumbnailWidth:1;
		nThumbnailHeight = nDstHeight;
	}
}
CThumbnailLoader::CThumbnailLoader(void)
{
	m_hWnd = NULL;
	m_hSemaphore = NULL;
	m_bInit = false;
	m_hExitEvent = NULL;
}

CThumbnailLoader::~CThumbnailLoader(void)
{
	UnInit();
}

void CThumbnailLoader::LoadThumbnails(wstring& wstrFilePath, wstring& wstrCacheFilePath, int nWidth, int nHeight)
{
	if (!m_bInit)	// �����ʼ��ʧ�ܣ�ֱ�ӷ���
	{
		return;
	}
	ThumbnailLoaderData loaderData;
	loaderData.m_wstrFilePath = wstrFilePath;
	loaderData.m_wstrCacheFilePath = wstrCacheFilePath;
	loaderData.m_nWidth = nWidth;
	loaderData.m_nHeight = nHeight;
	// �����ٽ���
	EnterCriticalSection(&m_CriSection);
		m_LoaderQueue.push(loaderData);
	LeaveCriticalSection(&m_CriSection);
	// �����ź���
	ReleaseSemaphore(m_hSemaphore, 1, NULL);
}
void CThumbnailLoader::Clear()
{
	if (!m_bInit)
	{
		return;
	}
	EnterCriticalSection(&m_CriSection);
	while (!m_LoaderQueue.empty())
	{
		m_LoaderQueue.pop();
	}
	LeaveCriticalSection(&m_CriSection);
}

bool CThumbnailLoader::Init()
{
	if (m_bInit)	// ����Ѿ���ʼ���ˣ���ֱ�ӷ���
	{
		return true;
	}
	// ������Ϣ����
	if (!CreateMsgWnd())
	{
		return false;
	}
	// �����˳��¼��ں˶���
	m_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == m_hExitEvent)
	{
		// ���ٴ���
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		TSERROR4CXX(L"CreateEvent failed");
		return false;
	}
	// �����ź����ں˶���
	m_hSemaphore = CreateSemaphore(NULL, 0, 32767, NULL);
	if (m_hSemaphore == NULL)
	{
		// ���ٴ���
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		// �ر��¼�����
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
		TSERROR4CXX(L"CreateSemaphore failed");
		return false;
	}

	// ���������߳�
	DWORD dwThreadId;
	HANDLE hThread = ::CreateThread(NULL, 0, CThumbnailLoader::WorkThreadHandle, this, 0, &dwThreadId);
	if (INVALID_HANDLE_VALUE == hThread)
	{
		// ���ٴ���
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
		// �ر��¼�����
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
		// �ر��ź���
		CloseHandle(m_hSemaphore);
		m_hSemaphore = NULL;
		TSERROR4CXX(L"CreateThread failed!");
		return false;
	}
	CloseHandle(hThread);


	// ��ʼ���ٽ����ṹ��
	InitializeCriticalSection(&m_CriSection);
	m_bInit = true;
	return true;
}

void CThumbnailLoader::UnInit()
{
	if (m_bInit)
	{
		// �ùر��¼�Ϊ��Ч
		SetEvent(m_hExitEvent);
		// �ر��ź���
		if (m_hSemaphore)
		{
			CloseHandle(m_hSemaphore);
			m_hSemaphore = NULL;
		}

		// ������Ϣ����
		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		}

		// �ر��¼�����
		if (m_hExitEvent)
		{
			CloseHandle(m_hExitEvent);
			m_hExitEvent = NULL;
		}
		
		// ɾ���ٽ���
		DeleteCriticalSection(&m_CriSection);
	}
	m_bInit = false;
}
bool CThumbnailLoader::CreateMsgWnd()
{
	static bool bRegisterClass = false;
	static int nWndIndex = 0;
	const HINSTANCE hInst = ::GetModuleHandle(NULL);
	wstring strClassName = L"{9E18143F-FAF9-4438-8FC1-32371276AED4}__KKIMAGE_CLASS"; 
	if (!bRegisterClass)	// ���û��ע��������࣬����ע����
	{
		WNDCLASS wc;
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = CThumbnailLoader::MsgWndProc; 
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = hInst;
		wc.hIcon         = ::LoadIcon(NULL, IDI_APPLICATION); 
		wc.hCursor       = ::LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = strClassName.c_str();

		if (::RegisterClass(&wc) == 0)
		{
			return false;
		}
		bRegisterClass = true;
	}

	wstring strWndName = L"{9E18143F-FAF9-4438-8FC1-32371276AED4}__KKIMAGE_TITLE";
	wchar_t wszWndIndex[10];
	wmemset(wszWndIndex, 0, 10);
	nWndIndex++;
	wsprintf(wszWndIndex, L"_%d", nWndIndex);
	strWndName = strWndName + wszWndIndex;
	m_hWnd = ::CreateWindow(strClassName.c_str(), strWndName.c_str(), WS_OVERLAPPEDWINDOW, 
		0, 0, 0, 0, NULL, NULL, hInst, this);
	if(m_hWnd == NULL)
	{
		return false;
	}
	::ShowWindow(m_hWnd, SW_HIDE);
	return true;
}

DWORD WINAPI  CThumbnailLoader::WorkThreadHandle(LPVOID lpParameter)
{
	TSAUTO();
	CThumbnailLoader* pThis = (CThumbnailLoader*)lpParameter;
	assert(pThis);
	while(1)
	{
		// �ȴ���������������б�
		HANDLE hWaitHandleList[2];
		hWaitHandleList[0] = pThis->m_hExitEvent;
		hWaitHandleList[1] = pThis->m_hSemaphore;
		DWORD dwRet = WaitForMultipleObjects(2, hWaitHandleList, FALSE, INFINITE);
		if (dwRet == WAIT_OBJECT_0)	// �˳�
		{
			break;
		}
		ThumbnailLoaderData LoaderData;
		// �����ٽ���
		EnterCriticalSection(&(pThis->m_CriSection));
		if (pThis->m_LoaderQueue.empty())	// ����������Ϊ�գ�����������
		{
			LeaveCriticalSection(&(pThis->m_CriSection));
			continue;
		}
		else	// ������в�Ϊ�գ���ȡ����
		{
			LoaderData = pThis->m_LoaderQueue.front();
			pThis->m_LoaderQueue.pop();
		}
		LeaveCriticalSection(&(pThis->m_CriSection));
		
		// ��ʵ�ʵļ��ز���
		XL_BITMAP_HANDLE hThumbnailBitmap = NULL;
		CImageLoader imageLoader;
		if (!LoaderData.m_wstrCacheFilePath.empty() && PathFileExists(LoaderData.m_wstrCacheFilePath.c_str())) // �������·����Ϊ�գ����л����ļ���ֱ�Ӵӻ����ж�ȡ
		{
			int nRet = imageLoader.LoadImage(LoaderData.m_wstrCacheFilePath.c_str(), NULL);
			if (nRet == 0)	// ���سɹ���
			{
				hThumbnailBitmap = imageLoader.GetXLBitmap();
			}
		}
		else if (!LoaderData.m_wstrFilePath.empty())// ���ļ����Լ�����һ������
		{
			int nRet = imageLoader.LoadImage(LoaderData.m_wstrFilePath.c_str(), NULL, true, LoaderData.m_nWidth, LoaderData.m_nHeight);
			if (nRet == 0)
			{
				XL_BITMAP_HANDLE hBitmap = NULL;
				if (imageLoader.GetLoaderType() == KKImg_Type_Gif)	// Gif���ж��λͼ�ģ��հ�
				{
					XLGP_GIF_HANDLE hGifObj = imageLoader.GetXLGifObj();
					unsigned int nFrameCount = XLGP_GifGetFrameCount(hGifObj);
					if (nFrameCount > 0)
					{
						hBitmap = XLGP_GifGetFrame(hGifObj, 0);
					}
					XLGP_ReleaseGif(hGifObj);					
				}
				else
				{
					hBitmap = imageLoader.GetXLBitmap();
				}
				if (hBitmap)
				{
					// ������
					XLBitmapInfo bmpInfo;
					XL_GetBitmapInfo(hBitmap, &bmpInfo);
					int nThumbnailWidth, nThumbnailHeight;
					CImageProcessor::GetThumbnailSize(bmpInfo.Width, bmpInfo.Height, LoaderData.m_nWidth, LoaderData.m_nHeight, nThumbnailWidth, nThumbnailHeight);
					hThumbnailBitmap = CImageProcessor::RescaleImage(hBitmap, nThumbnailWidth, nThumbnailHeight);
					if (hThumbnailBitmap)
					{
						XL_ReleaseBitmap(hBitmap);
						hBitmap = NULL;
					}
					else
					{
						hThumbnailBitmap = hBitmap;
					}
				}
			}
			// �����ȡ����ͼ�ɹ����л���·����Ϊ�գ���������
			if (hThumbnailBitmap && !LoaderData.m_wstrCacheFilePath.empty()) // ������
			{
				imageLoader.SaveImage(hThumbnailBitmap, LoaderData.m_wstrCacheFilePath.c_str());
			}
		}
		ThumbnailLoaderCompleteData* pCompleteData = new ThumbnailLoaderCompleteData();
		pCompleteData->m_pLoader = pThis;
		pCompleteData->m_hBitmap = hThumbnailBitmap;
		pCompleteData->m_wstrFilePath = LoaderData.m_wstrFilePath;
		if (hThumbnailBitmap)	// ����ͼ��ȡ�ɹ�
		{
			TSINFO4CXX("�ɹ�");
			PostMessage(pThis->m_hWnd, THUMBNAILLOADER_MSG_LOADCOMPLETE, 1, LPARAM(pCompleteData));
		}
		else	// ����ͼ��ȡʧ��
		{
			TSINFO4CXX("ʧ��");
			PostMessage(pThis->m_hWnd, THUMBNAILLOADER_MSG_LOADCOMPLETE, 0, LPARAM(pCompleteData));
		}
	}
	TSINFO4CXX(L"��ȡ����ͼ���߳̽�����--------------------------------");
	return 0;
}
LRESULT CALLBACK CThumbnailLoader::MsgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == THUMBNAILLOADER_MSG_LOADCOMPLETE)// �������
	{
		ThumbnailLoaderCompleteData* pCompleteData = (ThumbnailLoaderCompleteData*)lParam;
		if (pCompleteData)
		{
			if (wParam == 1)	// �ɹ�
			{
				pCompleteData->m_pLoader->FireLoadCompleteEvent(pCompleteData->m_wstrFilePath, pCompleteData->m_hBitmap);
			}
			else	// ʧ��
			{
				pCompleteData->m_pLoader->FireLoadCompleteEvent(pCompleteData->m_wstrFilePath, NULL);
			}
			delete pCompleteData; // �ͷ��ڴ棻
		}
		return 0;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}
DWORD CThumbnailLoader::AttatchLoadCompleteEvent(lua_State* luaState)
{
	DWORD dwCookie = 0;
	m_LoadEventContainer.AttachEvent(luaState, 2, dwCookie);
	return dwCookie;
}

void CThumbnailLoader::DetatchLoadCompleteEvent(lua_State* luaState)
{
	CLuaEvent* pEvent = NULL;
	DWORD dwCookie = (DWORD)luaL_checknumber(luaState, 2);
	m_LoadEventContainer.DetachEvent(dwCookie);
	delete pEvent;
}
void CThumbnailLoader::FireLoadCompleteEvent(const wstring wstrFilePath, XL_BITMAP_HANDLE hBitmap)
{
	IEventEnum<CLuaEvent>* pEventEnum = NULL;
	m_LoadEventContainer.GetEventEnum(pEventEnum);
	CLuaEvent* pEvent = NULL;
	while (S_OK == pEventEnum->Next(pEvent))
	{
		lua_State* luaState = pEvent->GetLuaState();
		int nowTop = lua_gettop(luaState);

		pEvent->PushFunction();
		if (hBitmap)
		{
			TSINFO4CXX("���ǳɹ�");
			XLGP_PushBitmap(luaState, hBitmap);

			XLBitmapInfo bmpInfo;
			XL_GetBitmapInfo(hBitmap, &bmpInfo);
			lua_pushnumber(luaState, bmpInfo.Width);
			lua_pushnumber(luaState, bmpInfo.Height);

			string utf8FilePath;
			utf8FilePath = ultra::_T2UTF(wstrFilePath);
			lua_pushstring(luaState, utf8FilePath.c_str());

			pEvent->Call(4, 0);
		}
		else
		{
			TSINFO4CXX("����ʧ��");
			lua_pushnil(luaState);
			lua_pushnumber(luaState, 0);
			lua_pushnumber(luaState, 0);
			string utf8FilePath;
			utf8FilePath = ultra::_T2UTF(wstrFilePath);
			lua_pushstring(luaState, utf8FilePath.c_str());
			pEvent->Call(4, 0);
		}
		lua_settop(luaState, nowTop);
	}
	if (hBitmap)
	{
		XL_ReleaseBitmap(hBitmap);
	}
}