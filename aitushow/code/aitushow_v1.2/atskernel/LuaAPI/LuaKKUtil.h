#pragma once

class CLuaKKUtil
{
public:
	CLuaKKUtil(void);
	~CLuaKKUtil(void);

	static long RegisterSelf(XL_LRT_RUNTIME_HANDLE hEnv);
	static void* __stdcall GetObject(void* p);
	static int LuaGc(lua_State* luaState);
	static int ExitApp(lua_State* luaState);
	static int GetTempDir(lua_State* luaState);
	static int SetWndIcon(lua_State* luaState);

	static int GetProfilesDir(lua_State* luaState);
	static int ForceUpdateWndShow(lua_State* luaState);
	static int GetFiles(lua_State* luaState);
	static int GetFolders(lua_State* luaState);
	static int StrColl(lua_State* luaState);
	static int GetScreenRatio(lua_State* luaState);		// ��ȡ��Ļ�ֱ���
	static int GetWorkAreaSize(lua_State* luaState);	// ��ȡ��Ļ���������С
	static int IsPathFileExist(lua_State* luaState);
	static int LogToFile(lua_State* luaState);
	static int GetProductVersion(lua_State* luaState);
	static int GetMd5Str(lua_State* luaState);
	static int RegisterFileRelation(lua_State* luaState);
	static int AttachShellCmdEvent(lua_State* luaState);
	static int DetachShellCmdEvent(lua_State* luaState);
	static int GetCommandPair(lua_State* luaState);
	static int MonitorDirChange(lua_State* luaState);
	static int UnMonitorDirChange(lua_State* luaState);
	static int AttachDirChangeEvent(lua_State* luaState);
	static int DetachDirChangeEvent(lua_State* luaState);
	static int InitFolderMonitor(lua_State* luaState);
	static int IsCanHandleFileCheckByExt(lua_State* luaState);
	static int DelPathFile2RecycleBin(lua_State* luaState);	// ɾ��������վ
	static int GetNeoImagingPath(lua_State* luaState);
	static int PrintImage(lua_State* luaState);
	static int GetOtherSoftList(lua_State* luaState);
	static int ExpandEnvironmentStrings(lua_State* luaState);
	static int CopyFilePathToCLipBoard(lua_State* luaState);
	static int CopyImageToClipboard(lua_State* luaState);
	static int NeoFolderDialog(lua_State* luaState);
	static int CopyFileTo(lua_State* luaState); // ���ļ����Ƶ�
	static int MoveFileTo(lua_State* luaState); // ���ļ��ƶ���
	static int RenameFile(lua_State* luaState);	// ������
	static int SHChangeNotify(lua_State* luaState);
	static int FileRelationCheck(lua_State* luaState);
	static int GetCurveProgress(lua_State* luaState);
	static int GetXiuXiuExePath(lua_State* luaState);
	static int GetNikonNx2ExePath(lua_State* luaState);
	static int GetDPPExePath(lua_State* luaState);
	static int GetLightRoomExePath(lua_State* luaState);
	static int GetPhotoShopExePath(lua_State* luaState);
	static int GetKeNiuExePath(lua_State* luaState);
	static int GetOldNeoImagingPath(lua_State* luaState);
	static int GetIconFromExeFile(lua_State* luaState);
	static int IsCanSetToWallPaperFile(lua_State* luaState);
	static int GetPeerId(lua_State* luaState);
	static int GetOSInfo(lua_State* luaState);
	static int GetSystemRatio(lua_State* luaState);
	static int CopyTextToClipboard(lua_State* luaState); //����һ���ı�����������
	static int ShowColorDialog(lua_State* luaState);
	static int GetExeFileDes(lua_State* luaState);
	static int SetShowCursor(lua_State* luaState);
private:
	
	//
};


