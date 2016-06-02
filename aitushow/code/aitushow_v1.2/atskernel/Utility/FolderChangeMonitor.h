#pragma once
#include "filesystemmonitor.h"
#include "../LuaBase/LuaEventContainer.h"
using namespace KKT::IDE;

class CFolderChangeMonitor
{
public:
	CFolderChangeMonitor(void);
	virtual ~CFolderChangeMonitor(void);

	// ��ȡʵ��
	static CFolderChangeMonitor* Instance();
	// ��ʼ��
	bool Init();
	long MonitorDirChange(const wstring& wstrDirPath);
	void UnMonitorDirChange(long nCookie);


	// �ص��¼�
	static long DirChangeCallback(FileSystemEventNotifyUserData udata, const wchar_t* dirPath, unsigned long changeType,const wchar_t* changePath1, const wchar_t* changePath2);

	// ���¼�
	int AttachDirChangeEvent(lua_State* luaState, int nIndex, DWORD& dwCookie);
	// ж���¼�
	int DetachDirChangeEvent(DWORD dwCookie);
	void FireChangeEvent(const wstring& wstrOldFilePath, const wstring& wstrNewFilePath, int nEventType);
private:
	CLuaMultiEventContainer m_EventContainer;
private:
	//
};
