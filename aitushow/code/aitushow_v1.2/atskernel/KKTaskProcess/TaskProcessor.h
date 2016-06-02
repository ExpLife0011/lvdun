#pragma once
#include <queue>
#include "BaseTask.h"

// ��Ϣ����
enum{
	TASKPROCESSOR_MSG_TASKCOMPLETE = WM_USER + 1,	// �������������һ������
	TASKPROCESSOR_MSG_TASKSTEPCOMPLETE
};

typedef queue<CBaseTask*> TaskQueue;

// ���������������������� �����ļ��������ļ�������ͼƬ
class CTaskProcessor
{
public:
	CTaskProcessor();
	~CTaskProcessor(void);

	// ��ʼ����������Ҫ�Ǵ��������߳� 
	bool Init(void);
	// ����ʼ��
	void UnInit(void);
	
	void ClearTask();
	int AddTask(CBaseTask* baseTask);
public:
	// �ٽ��������ڻ�������������
	CRITICAL_SECTION m_CriSection;
	// �¼����У����̱߳��뻥�������
	TaskQueue	m_TaskQueue;
	// �ź������󣬱�����������/������ģʽ���������߳�Э����������
	HANDLE m_hSemaphore;
	// ��Ϣ���ھ��
	HWND m_hWnd;
	bool m_bStop;
private:
	static DWORD WINAPI  WorkThreadHandle(LPVOID lpParameter);	// �ӹ����̴߳�����
	static LRESULT CALLBACK MsgWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	// ��Ϣ���ڴ�����
	// ���������߳�
	bool CreateWorkThread(void);
	// ������Ϣ����
	bool CreateMsgWnd(void);

	// ����һ������ID
	int GenerateTaskId();
	// �Ƿ��ʼ���ı�־λ
	bool m_bInit;
	

	//
};

