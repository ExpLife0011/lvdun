#include "stdafx.h"
#include "HttpRequestFilter.h"
#include <process.h>

#include <Sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>

#include "ScopeResourceHandle.h"

HttpRequestFilter::HttpRequestFilter() : m_enable(false), m_enableRedirect(false), m_hIPCFileMapping(NULL)
{
}

namespace {

bool SetObjectToLowIntegrity(HANDLE hObject, SE_OBJECT_TYPE type = SE_KERNEL_OBJECT)
{
	bool bRet = false;
	DWORD dwErr = ERROR_SUCCESS;
	PSECURITY_DESCRIPTOR pSD = NULL;
	PACL pSacl = NULL;
	BOOL fSaclPresent = FALSE;
	BOOL fSaclDefaulted = FALSE;

	if(ConvertStringSecurityDescriptorToSecurityDescriptor(L"S:(ML;;NW;;;LW)", SDDL_REVISION_1, &pSD, NULL)) {
		if (GetSecurityDescriptorSacl(pSD, &fSaclPresent, &pSacl, &fSaclDefaulted)) {
			dwErr = SetSecurityInfo(hObject, type, LABEL_SECURITY_INFORMATION,NULL, NULL, NULL, pSacl);
			bRet = (ERROR_SUCCESS == dwErr);
		}
		LocalFree (pSD);
    }
	return bRet;
}

}

bool HttpRequestFilter::Enable(bool enable, unsigned short listen_port)
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	this->m_enable = enable;
	if(this->m_hIPCFileMapping == NULL) {
		this->m_hIPCFileMapping = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4 * 1024, L"Local\\{ED30EC84-F8F0-4D7E-83B5-942E4E3DD5DA}WebEarserFilterEnable");
		if(this->m_hIPCFileMapping == NULL) {
			return false;
		}
		SetObjectToLowIntegrity(this->m_hIPCFileMapping);
	}
	char* sharedMemoryBuffer = reinterpret_cast<char*>(::MapViewOfFile(this->m_hIPCFileMapping, FILE_MAP_WRITE, 0, 0, 256));
	if(sharedMemoryBuffer == NULL) {
		return false;
	}

	// �Զ�Unmap
	ScopeResourceHandle<HANDLE, BOOL(WINAPI*)(LPCVOID)> autoUnmapViewOfFile(sharedMemoryBuffer, ::UnmapViewOfFile);

	if(enable) {
		sharedMemoryBuffer[3] = '\x01';
	}
	else {
		sharedMemoryBuffer[3] = '\x00';
	}

	// 4~5����
	sharedMemoryBuffer[4] = '\x00';
	sharedMemoryBuffer[5] = '\x00';

	// �˿�
	assert(sizeof(unsigned short) == 2);
	if(enable) {
		union {
			unsigned short from;
			char to[2];
		}cvt;

		cvt.from = listen_port;

		sharedMemoryBuffer[6] = cvt.to[0];
		sharedMemoryBuffer[7] = cvt.to[1];
	}

	sharedMemoryBuffer[2] = 'C';
	sharedMemoryBuffer[1] = 'D';
	sharedMemoryBuffer[0] = 'A';
	return true;
}

void HttpRequestFilter::EnableRedirect(bool enable)
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	this->m_enableRedirect = enable;
}

bool HttpRequestFilter::IsEnable() const
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	return this->m_enable;
}

bool HttpRequestFilter::IsEnableRedirect() const
{
	XMLib::CriticalSectionLockGuard lck(this->cs);
	return this->m_enableRedirect;
}

namespace {
	XMLib::CriticalSection getInstanceCS;
}

HttpRequestFilter& HttpRequestFilter::GetInstance()
{
	XMLib::CriticalSectionLockGuard lck(getInstanceCS);
	static HttpRequestFilter instance;
	return instance;
}
