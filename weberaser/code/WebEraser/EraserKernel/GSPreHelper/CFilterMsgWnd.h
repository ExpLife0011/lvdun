#pragma once
#include "atlwin.h"
#include "map"

#include <XLLuaRuntime.h>
typedef void (*funResultCallBack) (DWORD userdata1,DWORD userdata2, const char* pszKey,  DISPPARAMS* pParams);

#define WM_FILTERRESULT WM_USER + 201
#define WM_FILTERASK WM_USER + 202
#define WM_REDIRECTRESULT WM_USER + 203

#define WM_FILTEREXIT WM_USER + 300
#define WM_FILTERLOCKING WM_USER + 301

struct CallbackNode
{
	funResultCallBack pCallBack;
	DWORD userData1;
	DWORD userData2;
	const void* luaFunction;
};

class CFilterMsgWindow : public  CWindowImpl<CFilterMsgWindow>
{
public:
	static CFilterMsgWindow* Instance()
	{
        static CFilterMsgWindow s;
		return &s;
	}

	int AttachListener(DWORD userData1,DWORD userData2,funResultCallBack pfn, const void* pfun);
	int DetachListener(DWORD userData1, const void* pfun);

	bool HandleSingleton();

	DECLARE_WND_CLASS(L"{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_weberaser")
	BEGIN_MSG_MAP(CFilterMsgWindow)
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData)
		MESSAGE_HANDLER(WM_FILTERRESULT, HandleFilterResult)
		MESSAGE_HANDLER(WM_FILTERASK, HandleFilterAsk)
		MESSAGE_HANDLER(WM_FILTEREXIT, HandleFilterExit)

		MESSAGE_HANDLER(WM_FILTERLOCKING, HandleFilterLocking)
		MESSAGE_HANDLER(WM_REDIRECTRESULT, HandleRedirectResult)
	END_MSG_MAP()
private:
	CFilterMsgWindow(void);
	~CFilterMsgWindow(void);
	std::vector<CallbackNode> m_allCallBack;

	void Fire_LuaEvent(const char* pszKey, DISPPARAMS* pParams)
	{
		TSAUTO();
		for(size_t i = 0;i<m_allCallBack.size();i++)
		{
 			m_allCallBack[i].pCallBack(m_allCallBack[i].userData1,m_allCallBack[i].userData2, pszKey,pParams);
		}
	}
private:
	HANDLE m_hMutex;
 

private:

public:
	LRESULT OnCopyData(UINT , WPARAM , LPARAM , BOOL& );
	LRESULT HandleFilterResult(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT HandleFilterAsk(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);

	LRESULT HandleFilterExit(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT HandleFilterLocking(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
	LRESULT HandleRedirectResult(UINT uiMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
};
