#pragma once
#include "BaseOperation.h"

#define KKIMAGE_OPERATION_CLASSNAME "KKImage.Operation.ClassName"
class CLuaOperation
{
public:
	CLuaOperation(void);
	~CLuaOperation(void);

	static void RegisterClass(XL_LRT_ENV_HANDLE hEnv);
	static int AddRef(lua_State* luaState);
	static int Release(lua_State* luaState);

	static int SetParam(lua_State* luaState);
	static int GetParam(lua_State* luaState);
	static int Execute(lua_State* luaState);
	static int ExecuteSpecial(lua_State* luaState);	// ������ڴ�����1�����е�
	static int AttachListener(lua_State* luaState);
	static int DetachListener(lua_State* luaState);

	// �������
	
	int AddRef()
	{
		m_nRef++;
		return m_nRef;
	}
	int Release()
	{
		TSINFO4CXX("Release-----------------");
		m_nRef--;
		if (m_nRef == 0)
		{
			delete this;
			return 0;
		}
		return m_nRef;
	}

	int m_nRef;
	CBaseOperation* m_Operation;
private:
	// �����־
	//
};
