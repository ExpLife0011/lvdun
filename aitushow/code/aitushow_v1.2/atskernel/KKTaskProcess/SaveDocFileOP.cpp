#include "StdAfx.h"
#include "SaveDocFileOP.h"
#include "..\KKImageLoader\ImageLoader.h"
#include "..\Utility\StringOperation.h"
CSaveDocFileOP::CSaveDocFileOP(void)
{
	m_OperationType = KKImg_OperationType_SaveDocFile;
	m_wstrOperationDes = L"������ת����ͼƬ�ļ�";

	m_pDoc = NULL;
	m_bAutoRotate = false;
}

CSaveDocFileOP::~CSaveDocFileOP(void)
{
}

bool CSaveDocFileOP::SetParam(lua_State* luaState)
{
	CLuaKKImageDoc** pp = (CLuaKKImageDoc**)luaL_checkudata(luaState, 2, KKIMAGE_LUADOC_CLASSNAME); 
	if(pp )
	{
		m_pDoc = (*pp)->m_pKKImageDoc;
	}

	// ��ȡ·�������û��·������ô����Ϊ�Ǹ��Ǳ��棬��·�������
	const char* utf8 = lua_tostring(luaState, 3);
	if (utf8 != NULL)
	{
		//xl::text::transcode::UTF8_to_Unicode(utf8, strlen(utf8), m_wstrNewPathFile);
		m_wstrNewPathFile = ultra::_UTF2T(utf8);
	}
	m_bAutoRotate = lua_toboolean(luaState, 4);
	return true;
}

int CSaveDocFileOP::Run(COperationTask* pCOperationTask)
{
	int nRet = 0;
	// ִ�б������
	// ��ȡ��ǰͼƬ�ļ���·��
	wstring wstrFilePath = m_pDoc->GetFilePath();
	
	// ��ȡ��ǰ��Ҫ�����·��
	bool bSaveAs = false;
	wstring wstrSavePath = wstrFilePath;
	if (!m_wstrNewPathFile.empty())
	{
		wstrSavePath = m_wstrNewPathFile;
	}
	if (wstrSavePath != wstrFilePath)
	{
		bSaveAs = true;
	}

	// ��ȡExif��Ϣ
	CImageLoader imageLoader;
	CKKImageEXIF* pKKImageEXIF = imageLoader.LoadImageFileOnlyExif(wstrFilePath.c_str());


	// �ж�ԭͼ��û�м��ؽ���
	XL_BITMAP_HANDLE hSrcBitmap = m_pDoc->GetSrcBitmap();
	if (hSrcBitmap != NULL)
	{
		// ��ԭͼ����ô��ԭͼ�϶���ת���ˣ�ֱ�ӱ��������
		if(!imageLoader.SaveImage(hSrcBitmap, wstrSavePath.c_str(), pKKImageEXIF, FALSE, 95))
		{
			// ����ʧ��
			nRet = 1;
		}
	}
	else
	{
		// û��ԭͼ����Ҫ���¼�
		if(imageLoader.LoadImage(wstrFilePath, NULL, false, 0, 0, m_bAutoRotate) == 0)
		{
			// ���سɹ�
			m_pDoc->SetSrcBitmap(imageLoader.GetXLBitmap());
			XL_BITMAP_HANDLE hSrcBitmap = m_pDoc->GetSrcBitmap();
			if(!imageLoader.SaveImage(hSrcBitmap, wstrSavePath.c_str(), pKKImageEXIF, FALSE, 95))
			{
				nRet = 1;
			}
		}
		else
		{
			// ����ʧ��
			nRet = 1;
		}
	}
	if (bSaveAs) // �������棬��ԭ������ת�ǶȺ�ͼ������
	{
		m_pDoc->ResetRotate();
	}
	else	// ���棬����ת�Ƕ����þͿ�����
	{
		m_pDoc->ResetRotateAngle();
	}
	return nRet;
}

bool CSaveDocFileOP::OnOperationComplete(int m_nStatus, int m_nErrorCode)
{
	ILuaEventEnum* pEventEnum = NULL;
	m_EventContainer.GetEventEnum(_T("OnOperationComplete"), pEventEnum);
	if (pEventEnum )
	{
		CLuaEvent* pEvent = NULL;
		pEventEnum->Reset();
		while (S_OK == pEventEnum->Next(pEvent))
		{
			lua_State* luaState = pEvent->GetLuaState();
			int nowTop = lua_gettop(luaState);

			pEvent->PushFunction();
			lua_pushinteger(luaState, m_nErrorCode);
			pEvent->Call(1, 0);
			lua_settop(luaState, nowTop);
		}
	}
	return true;
}
