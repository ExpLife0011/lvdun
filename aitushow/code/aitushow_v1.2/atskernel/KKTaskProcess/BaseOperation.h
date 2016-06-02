#pragma once
#include "../LuaBase/LuaEventContainer.h"
class COperationTask;

// ����ִ�н��
typedef enum{
	KKImg_ErrorCode_SUCCESS = 0,	// �ɹ�
	KKImg_ErrorCode_RESERVED,
	KKImg_ErrorCode_PARAMERROR,	// ��������
	KKImg_ErrorCode_NOMEMORY,	// �ڴ治��
	KKImg_ErrorCode_UNKOWNERROR,	// δ֪����Ĭ�ϣ�
}Operation_ErrorCode;


typedef enum{
	KKImg_OperationType_Invalid = 0x0000,
	KKImg_OperationType_LoadImageFile = 0x0001,
	KKImg_OperationType_LoadBitmap = 0x0002,
	KKImg_OperationType_ScaleBitmap = 0x0003,
	KKImg_OperationType_GetExif = 0x0004,
	KKImg_OperationType_SaveDocFile = 0x0005
}OperationType;

// ������Ļ���
class CBaseOperation
{
public:
	CBaseOperation(void);
	virtual ~CBaseOperation(void);

	virtual bool SetParam(lua_State* luaState);		// ����Ҫ���ò�����ʱ��������Բ��ù���
	virtual int GetParam(lua_State* luaState);		// ��ȡ������������಻��������Ĳ����Ļ������Բ��ùܸú���
	virtual int Run(COperationTask* pCOperationTask=NULL) = 0;

	virtual bool OnOperationComplete(int m_nStatus, int m_nErrorCode) = 0;
	virtual bool OnOperationStepComplete(int m_nStatus, int m_nErrorCode);
	virtual int AttachListener(const wstring& wstrEventName, lua_State* luaState, int nIndex, DWORD& dwCookie);
	virtual int DetachListener(const wstring& wstrEventName, DWORD dwCookie);
	int GetOperationId()
	{
		static int index = 0;
		return index++;
	}
	int m_nOperationId;
protected:
	// ����������
	OperationType m_OperationType;	// �������ͣ�������Ҫ�����£����ó��Լ���
	wstring m_wstrOperationDes;		// ����������������Ҫ�����£����ó��Լ���
	CLuaMultiEventContainer m_EventContainer;
	
private:
	//
};
