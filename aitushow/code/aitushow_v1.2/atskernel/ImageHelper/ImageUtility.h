#pragma once
#include <strsafe.h>
#include "freeimage/FreeImage.h"

#define SE_GROUP_INTEGRITY                 (0x00000020L)
#define TokenIntegrityLevel					25
class CImageUtility
{
public:
	typedef struct _TOKEN_MANDATORY_LABEL {
		SID_AND_ATTRIBUTES Label;
	} TOKEN_MANDATORY_LABEL, *PTOKEN_MANDATORY_LABEL;

	CImageUtility(void);
	~CImageUtility(void);

	static bool DeleteDir(const std::wstring& wstrTempDirect);
	static bool IsCanHandleFileCheckByExt(const std::wstring& wstrTempDirect);	// ����ǲ��ǿ��Դ�����ļ�
	static bool IsCanSetToWallPaperFile(const std::wstring& wstrTempDirect);	// ����ǲ��ǿ�������Ϊ˵�汳�����ļ�
	static bool IsCanBatchRotateFile(const std::wstring& wstrTempDirect);	// ����ǲ��ǿ�����������ת���ļ�
	static bool IsCanSuperBatchFile(const std::wstring& wstrTempDirect);	// ����ǲ��ǿ������߼�������
	static bool IsCanSaveToSameTypeFile(const std::wstring& wstrTempDirect);	// ����ǲ��ǿ��Ա���Ϊ��ͬ��ʽ���ļ�
	static BOOL IsLegalPath(std::wstring& wstrPathName); //�ж�һ��·���Ƿ��ǺϷ���ϵͳ·��
	static BOOL GetOSInfo(std::wstring &strOSDesc, std::wstring &strOSVersion);	// ��ȡ����ϵͳ�汾
	static wstring LowerStr(wstring& wstrSrcStr);
	static BOOL CreateMediumIntegrityProcess(PCTSTR pszApplicationName, PTSTR pszCommandLine, PPROCESS_INFORMATION pPI,  BOOL bShowWnd = false);
	static BOOL PrintImage(wchar_t* path,wchar_t* file_name);
	static wstring GetFileDesInfo(wstring& wstrFilePath);
	static wstring GetDisplayMonitorICCFilePath();
	static BOOL IS_Vista_Or_More();

private:
	static BOOL GetDisplayMonitorICC( wchar_t* full_path, int char_count );
	//
};

//�滻һ���ַ��������е�ĳ���Ӵ�Ϊ�µ�ֵ
// str:ԭ�ַ���
// old_value: ���滻��ֵ
// new_value: �µ�ֵ
wstring& replace_all_distinct(wstring& str, const wstring& old_value, const wstring& new_value);
