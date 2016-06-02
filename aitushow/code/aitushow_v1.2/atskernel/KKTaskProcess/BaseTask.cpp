#include "StdAfx.h"
#include ".\basetask.h"

CBaseTask::CBaseTask()
{
	m_nStatus = KKIMG_TASKSTATUS_UNKNOWN;	// 0 δ����1 ����ɹ���2 ����ʧ��
	m_nTaskId = 0;
	m_nErrorCode = 0;
	m_nTaskType = KKIMG_TASKTYPE_UNKNOWN;
	m_pTaskProcessor = NULL;
	m_bIgnore = false;
}

CBaseTask::~CBaseTask(void)
{

}

void CBaseTask::SetTaskProcessor(CTaskProcessor* pTaskProcessor)
{
	m_pTaskProcessor = pTaskProcessor;
}
void CBaseTask::OnTaskStepComplete()
{
}