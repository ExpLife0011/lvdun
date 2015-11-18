// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

using namespace ATL;


#define TSLOG
#define TSLOG_GROUP "ExplorerPlugin"
#include <tslog\tslog.h>

#ifdef MYCALENDAR
	//�˻�������֤��dllֻ��1���������̼���
	#define ONEPROCESSMUTEX L"{B6A3F13B-ED68-4115-9EC9-60E4E4408589}"
	//�˻�������ֻ֤��1�������߼���ִ��(�����ɶ�δ��������߼�)
	#define GLOBALMUTXNAME L"Global\\{C3F5E7B7-46CF-445a-ACE9-7DCC9FD345F2}_ExplorerPlugin"
	//IconOverlay ����
	#define ICONOVERLAYNAME L".mycalendarremind"
	//CopyHook ����
	#define COPYHOOKNAME L"AMCSharing"
	//BHO ����
	#define BHONAME L"MCBHO"
	//��Ʒע���·��
	#define REGEDITPATH L"Software\\mycalendar"
	//���򿪹�ע������
	#define ZONESWITCH L"laopen" 
	//�ϴ�����ʱ��ע������
	#define LASTLAUNCHUTC L"LastLaunchTime"
	//��������������
	#define SYSBOOTNAME L""
#elif defined LVDUN_0000
	//�˻�������֤��dllֻ��1���������̼���
	#define ONEPROCESSMUTEX L"{2C55D214-C83F-4a69-A5C3-1955F3FC0ACD}"
	//�˻�������ֻ֤��1�������߼���ִ��(�����ɶ�δ��������߼�)
	#define GLOBALMUTXNAME L"Global\\{FB215D88-ACE8-48f0-8C92-D5ABAC4DD8DF}_ExplorerPlugin"
	//IconOverlay ����
	#define ICONOVERLAYNAME L".gsremind"
	//CopyHook ����
	#define COPYHOOKNAME L"AGSSharing"
	//BHO ����
	#define BHONAME L"GSBHO"
	//��Ʒע���·��
	#define REGEDITPATH L"Software\\GreenShield"
	//���򿪹�ע������
	#define ZONESWITCH L"laopen" 
	//�ϴ�����ʱ��ע������
	#define LASTLAUNCHUTC L"LastLaunchTime"
	//��������������
	#define SYSBOOTNAME L"GreenShield"
#endif
