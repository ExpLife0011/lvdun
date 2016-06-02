#pragma once
#include "baseoperation.h"
#include "..\ImageHelper\KKImageDoc.h"

// �����ļ�OP
class CLoadFileOP:
	public CBaseOperation
{
public:
	CLoadFileOP(void);
	~CLoadFileOP(void);
	virtual bool SetParam(lua_State* luaState);
	virtual int Run(COperationTask* pCOperationTask=NULL);
	virtual bool OnOperationComplete(int m_nStatus, int m_nErrorCode);

private:
	wstring m_wstrFilePath;
	CKKImageDoc* m_pDoc;
	bool m_bScale;	// �Ƿ���Ҫ����
	int m_nWidth;	// ��ʾ����Ŀ��
	int m_nHeight;	// ��ʾ����ĸ߶�
	bool m_bAutoRotate;	// �Ƿ��Զ�ת��
private:
	//
};
