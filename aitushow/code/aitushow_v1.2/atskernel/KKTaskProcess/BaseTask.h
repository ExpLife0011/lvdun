#pragma once
class CTaskProcessor;
// ����״̬
enum{
	KKIMG_TASKSTATUS_UNKNOWN = 0,
	KKIMG_TASKSTATUS_SUCCESS,
	KKIMG_TASKSTATUS_FAIL,
	KKIMG_TASKSTATUS_IGNORE
};
// ��������
enum{
	KKIMG_TASKTYPE_UNKNOWN = 0,
	KKIMG_TASKTYPE_EXIT,
	KKIMG_TASKTYPE_LOADFILE,
	KKIMG_TASKTYPE_OPERATION
};

class CBaseTask
{
public:
	CBaseTask();
	virtual ~CBaseTask();
	

	// ���õ�ǰ�������ڵĴ�����
	virtual void SetTaskProcessor(CTaskProcessor* pTaskProcessor);
	// �ڽ�������뵽�����б�֮ǰ����ô˺������ú������������߳��б����ã�
	virtual void HandleTask() = 0;
	// �������ʱ����ô˺������ú������������߳��б����ã�
	virtual void OnTaskComplete() = 0;
	// ������ĳһ�����ʱ����ô˺������ú������������߳��б����ã�
	virtual void OnTaskStepComplete();
	
	// ���ú��Ա�־
	virtual void SetIgnoreFlag(bool bIgnore)
	{
		m_bIgnore = bIgnore;
	}

	int m_nStatus;		// 0 δ����1 ����ɹ���2 ����ʧ�� 3 ������
	int m_nErrorCode;	// �����룬��m_nStatus����2ʱ��ֵ��Ч
	int m_nTaskType;	// ��������
	int m_nTaskId;		// ʹ���߲���Ҫ֪�����ֵ
	bool m_bIgnore;		// �Ƿ���Ը�����

	CTaskProcessor* m_pTaskProcessor;	// ������������������
private:
	//
};