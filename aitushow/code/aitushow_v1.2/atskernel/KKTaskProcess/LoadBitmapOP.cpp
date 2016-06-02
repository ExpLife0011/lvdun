#include "StdAfx.h"
#include "LoadBitmapOP.h"
#include "..\KKImageLoader\ImageLoader.h"
#include "..\Utility\StringOperation.h"
#include "OperationTask.h"

CLoadBitmapOP::CLoadBitmapOP(void)
{
	m_OperationType = KKImg_OperationType_LoadBitmap;
	m_wstrOperationDes = L"����λͼ";

	m_hBitmap = NULL;
	m_bAutoRotate = false;
}

CLoadBitmapOP::~CLoadBitmapOP(void)
{
	TSAUTO();
	TSINFO4CXX("CLoadBitmapOP Id is --" << m_nOperationId);

}
bool CLoadBitmapOP::SetParam(lua_State* luaState)
{
	const char* utf8Text = luaL_checkstring(luaState, 2);
	m_wstrFilePath = ultra::_UTF2T(utf8Text);
	m_bAutoRotate = lua_toboolean(luaState, 3);
	return true;
}

int CLoadBitmapOP::Run(COperationTask* pCOperationTask)
{
	TSAUTO();
	TSINFO4CXX("CLoadBitmapOP Run CurId is " << m_nOperationId);

	CImageLoader loader;
	int nRet = loader.LoadImage(m_wstrFilePath, &(pCOperationTask->m_pTaskProcessor->m_bStop), false, 0, 0, m_bAutoRotate);
	if (nRet == 0)	// ���سɹ���
	{
		// ����λͼ
		if (loader.GetLoaderType() == KKImg_Type_Gif)	// Gif���ж��λͼ�ģ��հ�
		{
			nRet = 1;
		}
		else
		{
			m_hBitmap = loader.GetXLBitmap();
		}
	}
	return nRet;
}

bool CLoadBitmapOP::OnOperationComplete(int m_nStatus, int m_nErrorCode)
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
			if (KKIMG_TASKSTATUS_SUCCESS != m_nStatus)	// ����ִ��ʧ��
			{
				lua_pushnil(luaState);
				lua_pushinteger(luaState, m_nErrorCode);
			}
			else	// �ɹ�
			{
				XLGP_PushBitmap(luaState, m_hBitmap);
				XL_ReleaseBitmap(m_hBitmap);
				lua_pushinteger(luaState, m_nErrorCode);
			}
			pEvent->Call(2, 0);
			lua_settop(luaState, nowTop);
		}
	}
	return true;
}