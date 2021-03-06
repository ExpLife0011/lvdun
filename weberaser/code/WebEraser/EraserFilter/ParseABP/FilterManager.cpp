#include "stdafx.h"
#include "boost/regex.hpp"

#include "FilterManager.h"
#include <fstream>
#include "ADBFilter.h"
#include <TlHelp32.h>
#include <string.h>

ConfigRuleMap map_VideoState;

rwmutex rw_cfgmutex;

struct FilterRuleList {
	FilterRule * r;
	FilterRuleList * next;
	inline int doFilter(const Url & mainURL,const Url & url, FilterType t) {
		FilterRuleList * l = this;
		while (l) 
		{
			int iRet = l->r->shouldFilter(mainURL,url,t);
			if (iRet != 0)
				return iRet;
			l = l->next;
		}
		return 0;
	}
};

struct HideRuleList {
	HideRule * r;
	HideRuleList * next;
};


struct ReplaceRuleList {
	ReplaceRule * r;
	ReplaceRuleList * next;
	inline void getRelpace(const Url & url,std::vector<std::string> &v_Parse) {
		ReplaceRuleList * l = this;
		while (l) {
			if (l->r->shouldReplace(url))
				v_Parse.push_back(l->r->getReplace());
			l = l->next;
		}
		return ;
	}
};

struct RedirectRuleList {
	RedirectRule * r;
	RedirectRuleList * next;
	inline bool shouldRedirect(const Url & url,std::string &strRedirect) {
		RedirectRuleList * l = this;
		while (l) {
			if (l->r->shouldRedirect(url))
			{
				strRedirect = l->r->getRedirect();
				return true;
			}
			l = l->next;
		}
		return false;
	}
};


#define RULE_KEY_HASH_LENGTH 8
inline int abpmin(int f1,int f2)
{
    return f1<f2?f1:f2;
}

static bool collectShortcuts(const std::string & str, StringVector & vs) 
{
	static boost::unordered_set<std::string> commonFilters;
	if(commonFilters.empty()) {
#if RULE_KEY_HASH_LENGTH==7 // 7
		commonFilters.insert("http://");
		commonFilters.insert("ttp://w");
		commonFilters.insert("tp://ww");
		commonFilters.insert("p://www");
		commonFilters.insert("://www.");
#elif RULE_KEY_HASH_LENGTH==8 // 8
		commonFilters.insert("http://w");
		commonFilters.insert("ttp://ww");
		commonFilters.insert("tp://www");
		commonFilters.insert("p://www.");
#elif RULE_KEY_HASH_LENGTH==9 // 9
		commonFilters.insert("http://ww");
		commonFilters.insert("ttp://www");
		commonFilters.insert("tp://www.");
#endif
	}

	int i = 0;
	bool isFindShoutcut = false;
	while (i < abpmin(str.length() - RULE_KEY_HASH_LENGTH,80)) 
	{
		unsigned int j = i;
		for (; j < str.length(); j++) {
			if ((str[j] == '*' || str[j] == '^'))
			{
				break;
			}
		}
		for (unsigned int k = i; j - k >= RULE_KEY_HASH_LENGTH; k++) 
		{
			std::string key = str.substr(k, RULE_KEY_HASH_LENGTH);
			if(commonFilters.find(key)!=commonFilters.end())
				continue;
			isFindShoutcut = true;
			vs.push_back(key); //append(key);
		}
		i = j + 1;
	}
	return isFindShoutcut;
}

int RuleMap::FilterRuleMap::doFilter(const Url & mainURL,const std::string &key, const Url & url,FilterType t) {
	FilterRuleMap::iterator mapit=find(key);
	if (mapit != end()) 
	{
		unsigned int address = (unsigned int) mapit->second;
		if (unMatchRules.find(address) != unMatchRules.end()) // already match this rules;
			return 0;
		this->unMatchRules.insert(address);  //add(address)
		int iRet = mapit->second->doFilter(mainURL, url, t);
		if (iRet != 0)
			return iRet;
	}
	return 0;
}

RuleMap::FilterRuleMap::~FilterRuleMap() {
	for (FilterRuleMap::iterator it = this->begin(); it != this->end(); ++it) {
		FilterRuleList * l = it->second;
		FilterRuleList * n = l->next;
		while (n) {
			FilterRuleList * temp = n->next;
			delete n;
			n = temp;
		}
		delete l;
	}
}


void RuleMap::ReplaceRuleMap::getReplace(const std::string &key,const Url & url,std::vector<std::string> &v_Parse) {
	ReplaceRuleMap::iterator mapit=find(key);
	if (mapit != end())
	{
		unsigned int address = (unsigned int) mapit->second;
		if (unMatchRules.find(address) != unMatchRules.end()) // already match this rules;
			return ;
		this->unMatchRules.insert(address);  //add(address)
		mapit->second->getRelpace(url,v_Parse);
	}
	return ;
}

bool RuleMap::RedirectRuleMap::shouldRedirect(const std::string &key,const Url & url,std::string &strRedirect) {
	RedirectRuleMap::iterator mapit=find(key);
	if (mapit != end())
	{
		unsigned int address = (unsigned int) mapit->second;
		if (unMatchRules.find(address) != unMatchRules.end()) // already match this rules;
			return false;
		this->unMatchRules.insert(address);  //add(address)
		return mapit->second->shouldRedirect(url,strRedirect);
	}
	return false;
}

RuleMap::ReplaceRuleMap::~ReplaceRuleMap() {
	for (ReplaceRuleMap::iterator it = this->begin(); it != this->end(); ++it) 
	{
		ReplaceRuleList * l = it->second;
		ReplaceRuleList * n = l->next;
		while (n) 
		{
			ReplaceRuleList * temp = n->next;
			delete n;
			n = temp;
		}
		delete l;
	}
}

RuleMap::RedirectRuleMap::~RedirectRuleMap() {
	for (RedirectRuleMap::iterator it = this->begin(); it != this->end(); ++it) 
	{
		RedirectRuleList * l = it->second;
		RedirectRuleList * n = l->next;
		while (n) 
		{
			RedirectRuleList * temp = n->next;
			delete n;
			n = temp;
		}
		delete l;
	}
}

RuleMap::~RuleMap()
{
	for (FilterRuleVector::iterator it=m_AllFilterRules.begin();
		it!=m_AllFilterRules.end();++it) {
			delete *it;
	}

	for (ReplaceRuleVector::iterator it=m_AllReplaceRules.begin();
		it!=m_AllReplaceRules.end();++it) {
			delete *it;
	}
	for (RedirectRuleVector::iterator it=m_AllRedirectRules.begin();
		it!=m_AllRedirectRules.end();++it) {
			delete *it;
	}
}

void RuleMap::insertRuleToFilterRuleMap(FilterRuleMap * rules,const std::string & key, FilterRule * r)
{
	FilterRuleMap::iterator it = rules->find(key);
	if (it != rules->end()) { // already exists
		FilterRuleList * l = it->second;
		while (l->next)
			l = l->next;
		FilterRuleList * n = new FilterRuleList();
		n->r = r;
		n->next = NULL;
		l->next = n;
	} else {
		FilterRuleList * n = new FilterRuleList();
		n->r = r;
		n->next = NULL;
		rules->insert(FilterRuleMap::value_type(key, n)); //add(key, n)
	}
}

void RuleMap::addRule(FilterRule * r)
{
	this->m_AllFilterRules.push_back(r); //for delete FilterRule;
	FilterRuleMap * rules;
	FilterRuleVector * unshortcutRules;
	StringVector domains;
	r->getDomains(domains);
	if (r->isWhiteFilter())
	{
		rules = domains.empty()?&m_ShortcutWhiteRules:&m_DomainWhiteRules;
		unshortcutRules = &m_UnshortcutWhiteRules;
	}
	else 
	{
		rules = domains.empty() ? &m_ShortcutFilterRules : &m_DomainFilterRules;
		unshortcutRules = &m_UnshortcutFilterRules;
	}
	if(!domains.empty()) 
	{
		for(UINT i=0;i<domains.size();i++) 
		{
			insertRuleToFilterRuleMap(rules,domains[i],r);
		}
		return;
	}
	/*
	 * 没有域名信息才会进行8个字符8个字符的索引
	 */
	const std::string & reFilter = r->getRegularFilter();
	StringVector shortcuts;
	if (collectShortcuts(reFilter, shortcuts)) 
	{
		unsigned int i = 0;
		for (; i < shortcuts.size(); i++) 
		{
			insertRuleToFilterRuleMap(rules,shortcuts[i],r);
		}
	} 
	else
		unshortcutRules->push_back(r);//append(r);
}

void RuleMap::addRule(HideRule * r) 
{
	const StringVector & domains=r->domains();
	const std::string & sel=r->selector();
	for(StringVector::const_iterator it=domains.begin();it!=domains.end();it++) 
	{
		const std::string & key=*it;
		HideRuleMap::iterator hit=this->m_HideRules.find(key);
		if(hit!=m_HideRules.end()) {
			hit->second=hit->second+","+sel;
		}
		else {
			m_HideRules.insert(HideRuleMap::value_type(*it, sel));
		}
	}
}

void RuleMap::insertRuleToFilterRuleMap(ReplaceRuleMap * rules,const std::string & key, ReplaceRule * r)
{
	ReplaceRuleMap::iterator it = rules->find(key);
	if (it != rules->end())
	{ // already exists
		ReplaceRuleList * l = it->second;
		while (l->next)
			l = l->next;
		ReplaceRuleList * n = new ReplaceRuleList();
		n->r = r;
		n->next = NULL;
		l->next = n;
	}
	else 
	{
		ReplaceRuleList * n = new ReplaceRuleList();
		n->r = r;
		n->next = NULL;
		rules->insert(ReplaceRuleMap::value_type(key, n)); //add(key, n)
	}
}

void RuleMap::addRule(ReplaceRule * r)
{
	this->m_AllReplaceRules.push_back(r); 
	ReplaceRuleMap * rules = &m_DomainReplaceRules;

	std::string domain;
	r->getDomains(domain);
	if(!domain.empty()) 
	{
		insertRuleToFilterRuleMap(rules,domain,r);
		return;
	}
}

void RuleMap::insertRuleToFilterRuleMap(RedirectRuleMap * rules,const std::string & key, RedirectRule * r)
{
	RedirectRuleMap::iterator it = rules->find(key);
	if (it != rules->end())
	{ // already exists
		RedirectRuleList * l = it->second;
		while (l->next)
			l = l->next;
		RedirectRuleList * n = new RedirectRuleList();
		n->r = r;
		n->next = NULL;
		l->next = n;
	}
	else 
	{
		RedirectRuleList * n = new RedirectRuleList();
		n->r = r;
		n->next = NULL;
		rules->insert(RedirectRuleMap::value_type(key, n)); //add(key, n)
	}
}

void RuleMap::addRule(RedirectRule * r)
{
	this->m_AllRedirectRules.push_back(r); 
	RedirectRuleMap * rules = &m_DomainRedirectRules;

	std::string domain;
	r->getDomains(domain);
	if(!domain.empty()) 
	{
		insertRuleToFilterRuleMap(rules,domain,r);
		return;
	}
}

int RuleMap::shouldFilterByDomain(const Url & mainURL,const Url & url,FilterType t,bool & isFind)
{
	int iRet = 0;
	StringVector dotsplits;
	this->m_DomainFilterRules.prepareStartFind();
	this->m_DomainWhiteRules.prepareStartFind();
	boost::split(dotsplits,url.GetHost(),boost::is_any_of("."));
	if (dotsplits.size() <= 1) //domain error? no dot? at least g.cn
		return false;
	for (int i = dotsplits.size() - 2; i >= 0; i--) {
		std::string domain = dotsplits[i];
		for (int j = i + 1; j < dotsplits.size(); j++) {
			domain = domain + "." + dotsplits[j];
		}
		iRet=m_DomainWhiteRules.doFilter(mainURL, domain, url, t);
		if (iRet != 0)
		{
			isFind = true;
			return 0;
		}
		iRet=m_DomainFilterRules.doFilter(mainURL, domain, url, t);
		if (iRet != 0)
		{
			isFind = true;
			return iRet;
		}
	}
	return iRet;
}

int RuleMap::shouldFilterByShortcut(const Url & mainURL,const Url & url,FilterType t)
{
	int iRet = 0;
	StringVector shortcuts;
	collectShortcuts(url.GetString(), shortcuts);
	this->m_ShortcutFilterRules.prepareStartFind();
	for (StringVector::iterator it = shortcuts.begin(); it != shortcuts.end(); ++it) 
	{
		iRet = m_ShortcutWhiteRules.doFilter(mainURL, *it, url, t);
		if (iRet != 0)
			return 0;
	}
	for (FilterRuleVector::iterator it =
		this->m_UnshortcutWhiteRules.begin(); it
		!= this->m_UnshortcutWhiteRules.end(); ++it) 
	{
			iRet = (*it)->shouldFilter(mainURL, url, t);
			if (iRet != 0)
				return 0;
	}
	for (StringVector::iterator it = shortcuts.begin(); it != shortcuts.end(); ++it)
	{
		iRet = this->m_ShortcutFilterRules.doFilter(mainURL, *it, url, t);
		if (iRet != 0)
			return iRet;
	}
	for (FilterRuleVector::iterator it = this->m_UnshortcutFilterRules.begin(); it != this->m_UnshortcutFilterRules.end(); ++it) 
	{
		iRet = (*it)->shouldFilter(mainURL, url, t);
		if (iRet != 0)
			return iRet;
	}
	return iRet;
}

int RuleMap::shouldFilter(const Url & mainURL,const Url & url,FilterType t)
{
	bool bFind =false;
	int iRet = shouldFilterByDomain(mainURL,url,t,bFind);
	if(!bFind)
		iRet = shouldFilterByShortcut(mainURL,url,t);
	return iRet;
}

std::string RuleMap::getcssRules(const Url & url)
{
	std::string res;
	StringVector dotsplits;
	boost::split(dotsplits,url.GetHost(),boost::is_any_of("."));
	if(dotsplits.size()<=1) //domain error? no dot? at least g.cn
		return res;
	int ignore=2;
	std::string l2domain=dotsplits[dotsplits.size()-2];
	if(l2domain =="com" ||
		l2domain=="net" ||
		l2domain=="org" ||
		l2domain=="edu")
		ignore=3;
	for(int i=dotsplits.size()-ignore;i>=0;i--) 
	{
		std::string domain=dotsplits[i];
		for(int j=i+1;j<dotsplits.size();j++) 
		{
			domain=domain+"."+dotsplits[j];
		}
		HideRuleMap::iterator it=this->m_HideRules.find(domain);

		if(it!=m_HideRules.end()) 
		{
			res=res.empty()?it->second:res+","+it->second;
		}
	}
	return res;
}

void RuleMap::getreplaceRules(const Url & url,std::vector<std::string> & v_replace)
{
	StringVector dotsplits;
	this->m_DomainReplaceRules.prepareStartFind();

	boost::split(dotsplits,url.GetHost(),boost::is_any_of("."));
	if (dotsplits.size() >= 1) //domain error? no dot? at least g.cn
	{
		for (int i = dotsplits.size() - 2; i >= 0; i--) 
		{
			std::string domain = dotsplits[i];
			for (int j = i + 1; j < dotsplits.size(); j++) 
			{
				domain = domain + "." + dotsplits[j];
			}
			m_DomainReplaceRules.getReplace(domain,url,v_replace);
		}
	}
}


bool RuleMap::shouldRedirect(const Url & mainURL,const Url & url, std::string& strRedirect)
{
	StringVector dotsplits;
	this->m_DomainRedirectRules.prepareStartFind();

	boost::split(dotsplits,url.GetHost(),boost::is_any_of("."));
	if (dotsplits.size() >= 1) //domain error? no dot? at least g.cn
	{
		for (int i = dotsplits.size() - 2; i >= 0; i--) 
		{
			std::string domain = dotsplits[i];
			for (int j = i + 1; j < dotsplits.size(); j++) 
			{
				domain = domain + "." + dotsplits[j];
			}
			if ( m_DomainRedirectRules.shouldRedirect(domain,url,strRedirect))
			{
				return true;
			}
		}
	}
	return false;
}


// 0:弹框
// 1:过滤
// 2:不过滤
bool FilterManager::updateConfigVideoHost(const Url & url,int istate)
{
	writeLock wrlock(rw_cfgmutex);
	std::string host = url.GetHost();
	if (host.empty())
	{
		return false;
	}
	ConfigRuleMap::iterator hit=map_VideoState.find(host);
	if(hit!=map_VideoState.end())
	{
		hit->second=istate;
	}
	else {
		map_VideoState.insert(ConfigRuleMap::value_type(host, istate));
	}

	return true;
}

// true:添加到白名单
// false:从白名单中删除
bool FilterManager::updateConfigWhiteHost(const Url & url,bool bEnable)
{
	writeLock wrlock(rw_cfgmutex);
	std::string host = url.GetHost();
	//ConfigRuleMap::iterator hit=this->m_ConfigWhileRules.find(host);
	if (bEnable)
	{
		ConfigRuleMap::iterator hit=this->m_ConfigWhiteRules.find(host);
		if(hit == m_ConfigWhiteRules.end())
		{
			m_ConfigWhiteRules.insert(ConfigRuleMap::value_type(host, 0));
		}
	}
	else
	{
		m_ConfigWhiteRules.erase(host);
	}
	return true;
}


static FilterManager * m=NULL;

FilterManager * FilterManager::getManager()
{
	if(NULL == m)
		m = new FilterManager;
	return m;
}

bool FilterManager::getWebRules(const std::wstring & filename)
{
	return getRules(filename,FILE_RULE_WEB);
}

bool FilterManager::getVideoRules(const std::wstring & filename)
{
	return getRules(filename,FILE_RULE_VIDEO);
}

bool FilterManager::getUsersRules(const std::wstring & filename)
{
	return getRules(filename,FILE_RULE_UESR);
}

bool FilterManager::getRules(const std::wstring & filename,int ruleType)
{
	RuleMap * pRuleMap;
	if (ruleType == FILE_RULE_WEB)
	{
		pRuleMap = &m_WebRules;
	}
	else if (ruleType == FILE_RULE_VIDEO)
	{
		pRuleMap = &m_VideoRules;
	}
	else if (ruleType == FILE_RULE_UESR)
	{
		pRuleMap = &m_UserRules;
	}
	else
	{
		return false;
	}	
	ifstream infile(filename.c_str());
	if (!infile)
	{ 
		return false;
	}
	else 
	{
		char rule[1000];
		infile.getline(rule, 1000); //忽略第一行版本号

		std::string strMatch;
		if (ruleType == FILE_RULE_VIDEO)
			strMatch = ".*\\$s@.*@.*@.*";
		else
			strMatch = ".*\\$s@.*@.*@$";
		boost::regex expression(strMatch);
		
		std::string strredirect = "^@#@.*";
		boost::regex expredirect(strredirect);
		while (!infile.eof())
		{
			memset(rule, 0, 1000);
			infile.getline(rule, 1000);
			if (rule[0] == '!')
			{
				//行注释
			}
			else
			{
				int n = strlen(rule);
				if (rule[n - 1] == '\n' || rule[n - 1] == '\r') {
					rule[n - 1] = '\0';
				}
				if (rule[n - 2] == '\n' || rule[n - 2] == '\r') {
					rule[n - 2] = '\0';
				} //clear \n \r
				if (strlen(rule) <= 3) //too short rule ,maybe some error
					continue;
				std::string s(rule);

				if (boost::icontains(s,"##")) {
					HideRule hr(s);
					pRuleMap->addRule(&hr);
				} 
				else if (boost::regex_match(s, expression))
				{
					pRuleMap->addRule(new ReplaceRule(s));
				}
				else if (boost::regex_match(s, expredirect))
				{
					pRuleMap->addRule(new RedirectRule(s));
				}
				else 
				{
					pRuleMap->addRule(new FilterRule(s));
				}
			}
		}
	}
	return true;
}


int FilterManager::checkConfigHost(const Url & url,bool &bFind,std::string &strMatchHost,int cfgFlag)
{
	readLock rdLock(rw_cfgmutex);
	std::string res;
	StringVector dotsplits;
	boost::split(dotsplits,url.GetHost(),boost::is_any_of("."));
	if(dotsplits.size()<=1) //domain error? no dot? at least g.cn
		return false;
	int ignore=2;
	std::string l2domain=dotsplits[dotsplits.size()-2];
	if(l2domain =="com" ||
		l2domain=="net" ||
		l2domain=="org" ||
		l2domain=="edu")
		ignore=3;
	for(int i=dotsplits.size()-ignore;i>=0;i--) 
	{
		std::string domain=dotsplits[i];
		for(int j=i+1;j<dotsplits.size();j++) {
			domain=domain+"."+dotsplits[j];
		}
		if (cfgFlag & CFG_CHECK_WHITE)
		{
			ConfigRuleMap::iterator it_White=this->m_ConfigWhiteRules.find(domain);
			if(it_White!=m_ConfigWhiteRules.end()) 
			{
				bFind = true;
				return 0;
			}

		}	

		if (cfgFlag & CFG_CHECK_VIDEO)
		{
			ConfigRuleMap::iterator it_video=map_VideoState.find(domain);
			if(it_video!=map_VideoState.end()) 
			{
				strMatchHost = domain;
				bFind = true;
				return it_video->second;
			}
		}
	}
	return 0;
}

static bool IsProExist(const wchar_t* strProName)
{
	bool bExist = false;
	HANDLE hSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		BOOL bResult = ::Process32First(hSnap, &pe);
		while (bResult)
		{
			if(_wcsicmp(pe.szExeFile,strProName) == 0)
			{
				bExist = true;
				break;
			}
			bResult = ::Process32Next(hSnap, &pe);
		}
		::CloseHandle(hSnap);
	}
	return bExist;
}

static void PostVideoFilterResult(const Url & url,DWORD dwFilterType)
{
	static HWND hNotifyWnd = ::FindWindow(L"{D8BD00DC-74BF-45ad-B8CB-61CB31C2CE84}_weberaser", NULL);
	if(hNotifyWnd != NULL) 
	{
		int len = url.GetString().length();
		char *szUrl = new char[len+1];
		memset(szUrl,0,len+1);
		strcpy_s(szUrl,len+1,url.GetString().c_str());

		if (!::PostMessage(hNotifyWnd, WM_USER + 302,WPARAM(dwFilterType), LPARAM(szUrl)))
		{
			delete szUrl;
		}
	}
}

int FilterManager::shouldFilter(const Url & mainURL,const Url & url, FilterType t)
{
	bool bCfgVideo = false;
	bool bCfgWeb = false;
	std::string strMatchHost;
	bool bFind =false;
	int iResult= 0;
	int iRetCheck = m_VideoRules.shouldFilter(mainURL,url,t);
	if (iRetCheck != 0)
	{
		//PostVideoFilterResult(url,1);
		return iRetCheck;
	}

	bFind = false;
	iResult = checkConfigHost(mainURL,bFind,strMatchHost,CFG_CHECK_WHITE);
	if (!bFind)
	{
		iResult = checkConfigHost(url,bFind,strMatchHost,CFG_CHECK_WHITE);
	}
	if (bFind)
	{
		return 0;
	}
	return m_WebRules.shouldFilter(mainURL,url,t);
}

std::string FilterManager::getcssRules(const Url & url)
{

	std::string strMatchHost;
	bool bFind =false;
	std::string strRules = "";
	int iResult;
	//int iResult = checkConfigHost(url,bFind,strMatchHost,CFG_CHECK_VIDEO);
	//if (bFind && iResult == 1 )
	//{
	//	strRules += m_VideoRules.getcssRules(url);
	//}
	bFind = false;
	iResult = checkConfigHost(url,bFind,strMatchHost,CFG_CHECK_WHITE);
	if (!bFind)
	{
		std::string strWebRules =  m_WebRules.getcssRules(url);
		if (!strRules.empty() && !strWebRules.empty())
		{
			strRules += ",";
		}
		
		strRules += strWebRules;
	}
	if (!strRules.empty())
	{
		strRules+=" {display:none !important;}";
	}
	return strRules;
}

void FilterManager::getreplaceRules(const Url & url,std::vector<std::string> & v_replace)
{
	std::string strMatchHost;
    m_VideoRules.getreplaceRules(url,v_replace);
	if (!v_replace.empty())
	{
		//PostVideoFilterResult(url,3);
	}
	
	bool bFind =false;
	int iResult = checkConfigHost(url,bFind,strMatchHost,CFG_CHECK_WHITE);
	if (!bFind)
	{
		m_WebRules.getreplaceRules(url,v_replace);
	}	
	return;
}


bool FilterManager::shouldRedirect(const Url & mainURL,const Url & url, std::string &strRedirect)
{
	bool bRedirect = m_VideoRules.shouldRedirect(mainURL,url,strRedirect);
	if (!bRedirect)
	{
		bRedirect = m_WebRules.shouldRedirect(mainURL,url,strRedirect);
	}	
	else
	{
		//PostVideoFilterResult(url,4);
	}
	if (bRedirect)
	{
		bRedirect = boost::iequals(mainURL.GetString(),strRedirect.c_str())?false:true;
	}
	
	return bRedirect;
}