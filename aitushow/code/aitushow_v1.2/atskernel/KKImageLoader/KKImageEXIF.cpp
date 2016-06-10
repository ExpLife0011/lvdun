#include "stdafx.h"
#include "KKImageEXIF.h"
#include "../Utility/StringOperation.h"
#include "../ImageHelper/ImageUtility.h"
#include <strsafe.h>

using namespace std;
using namespace Gdiplus;

CKKImageEXIF::CKKImageEXIF()
{
	TSAUTO();

	Init();
}

CKKImageEXIF::~CKKImageEXIF()
{

}

void CKKImageEXIF::Init()
{
	TSAUTO();

	m_exifs.clear();

	ZeroMemory(m_szShutterCount, 40);
	ZeroMemory(m_szLensModel, 200);

	m_nExifLength = 0;
}

void CKKImageEXIF::AddImageEXIF(std::string strFieldName, fipTag& newTag)
{
	if (m_exifs.find(strFieldName) == m_exifs.end())
	{
		m_exifs[strFieldName] = newTag;
	}
}

void CKKImageEXIF::SetShutterCount(std::wstring wstrShutterCount)
{
	TSAUTO();

	TSINFO4CXX("wstrShutterCount:"<<wstrShutterCount);

	ZeroMemory(m_szShutterCount, 40); //40���ֽ�

	StringCchCopy(m_szShutterCount, 20, wstrShutterCount.c_str());

	TSINFO4CXX("m_szShutterCount:"<<m_szShutterCount);
}

void CKKImageEXIF::SetLensModel(std::wstring wstrLensModel)
{
	TSAUTO();

	TSINFO4CXX("wstrLensModel:"<<wstrLensModel);

	ZeroMemory(m_szLensModel, 200); //40���ֽ�

	StringCchCopy(m_szLensModel, 100, wstrLensModel.c_str());

	TSINFO4CXX("m_szLensModel:"<<m_szLensModel);	
}

void CKKImageEXIF::SetExifLength(int nExifLength)
{
	TSAUTO();

	m_nExifLength = nExifLength + 8;
}

int CKKImageEXIF::GetExifLength()
{
	TSAUTO();

	return m_nExifLength;
}

BOOL CKKImageEXIF::GetColorSpace(std::string& strColorSpace)
{
	TSAUTO();

	if (m_exifs.find("ColorSpace") == m_exifs.end())
	{
		TSINFO4CXX("There is no ColorSpace Info!");
		return FALSE;
	}

	UINT16 nColorSpace;
	memcpy(&nColorSpace,  m_exifs["ColorSpace"].getValue(),  m_exifs["ColorSpace"].getLength());

	TSINFO4CXX("nColorSpace:"<<nColorSpace);

	if (nColorSpace == 0x0001)
	{
		TSINFO4CXX("This is a sRGB JPEG!");
		strColorSpace = "sRGB";
	}
	else if (nColorSpace = 0xFFFF)
	{
		//���ColorSpace��ֵΪ65535������������ͺ���Ϣ�����ǲ���Ϊ��Adobe RGB
		if (m_exifs.find("Make") != m_exifs.end())
		{
			char szMake[100] = {0};
			memcpy(szMake, m_exifs["Make"].getValue(),  m_exifs["Make"].getLength());
			std::string strMake = szMake;
			TSINFO4CXX("Make:"<<strMake);

			if (strMake.size() != 0)
			{
				TSINFO4CXX("This is a Adobe RGB JPEG!");
				strColorSpace = "Adobe RGB";
			}
			else
			{
				TSINFO4CXX("Length of Make is 0!");
				strColorSpace = "unknow";
			}
		}
		else
		{
			strColorSpace = "unknow";
			TSINFO4CXX("There is no Make Info, it is not a adobe RGB image!");
		}		
	}
	else
	{
		TSINFO4CXX("The ColorSpace of this JPEG is unknow");
		strColorSpace = "unknow";
	}

	return TRUE;
}

BOOL CKKImageEXIF::GetMake(std::string& strMake)
{
	TSAUTO();

	if (m_exifs.find("Make") == m_exifs.end())
	{
		TSINFO4CXX("There is no Make Info!");
		return FALSE;
	}

	strMake = (char*)m_exifs["Make"].getValue();

	return TRUE;
}

BOOL CKKImageEXIF::GetModel(std::string& strModel)
{
	TSAUTO();

	if (m_exifs.find("Model") == m_exifs.end())
	{
		TSINFO4CXX("There is no Model Info!");
		return FALSE;
	}

	strModel = (char*)m_exifs["Model"].getValue();

	return TRUE;
}

BOOL CKKImageEXIF::GetDateTime(std::string& strDateTime)
{
	TSAUTO();

	if (m_exifs.find("DateTimeOriginal") == m_exifs.end())
	{
		//���û��DateTimeOriginal��Ϣ���Ͷ�DateTime��Ϣ����Ҫ�Ƕ�raw��ʽ��
		if (m_exifs.find("DateTime") == m_exifs.end())
		{
			TSINFO4CXX("There is no DateTime Info!");
			return FALSE;
		}
		strDateTime = (char*)m_exifs["DateTime"].getValue();
	}
	else
	{
		strDateTime = (char*)m_exifs["DateTimeOriginal"].getValue();
	}

	return TRUE;
}

/*
	�������������ȡ�����ʽ���������ڣ�����������ֵĲ���Exif��Ϣ��
	nType��ֵ������ʽ���ͣ����յ�������������й�
	nType:1 YYYY-MM-DD
	nType:2 YY-MM-DD
	nType:3 MM/DD/YYYY
	nType:4 MMM.DD.YY
	nType:5 YYYY
	nType:6 YY
	nType:7 M
	nType:8 MM
	nType:9 MMM
	nType:10 D
	nType:11 DD
	nType:12 DDD
*/

BOOL CKKImageEXIF::GetDateDay(std::string& strDateDay, int nFormatType)
{
	TSAUTO();

	std::string strSrcDateTime;
	if (m_exifs.find("DateTimeOriginal") == m_exifs.end())
	{
		if (m_exifs.find("DateTime") == m_exifs.end())
		{
			TSINFO4CXX("There is no DateTime Info!");
			return FALSE;
		}
		strSrcDateTime = (char*)m_exifs["DateTime"].getValue(); //��ԭʼ������ʱ��
	}
	else
	{
		strSrcDateTime = (char*)m_exifs["DateTimeOriginal"].getValue();
	}

	std::string strYear = strSrcDateTime.substr(0, 4);
	std::string strMonth = strSrcDateTime.substr(5, 2);
	std::string strDay = strSrcDateTime.substr(8, 2);
	TSINFO4CXX("Year:"<<strYear<<" Month:"<<strMonth<<" Day:"<<strDay);

	//�����д
	std::string strYearYY = strYear.substr(2, 2);

	//�·���д
	std::string strMonthM; //һλ�·ݣ���ȥ���·�ǰ���0
	if (strMonth.find("0") == 0)
	{
		strMonthM = strMonth.substr(1, 1);
	}
	else
	{
		strMonthM = strMonth;
	}

	std::string strMonthMMM;
	if (strMonth == "01")
	{
		strMonthMMM = "Jan";
	}
	else if (strMonth == "02")
	{
		strMonthMMM = "Feb";
	}
	else if (strMonth == "03")
	{
		strMonthMMM = "Mar";
	}
	else if (strMonth == "04")
	{
		strMonthMMM = "Apr";
	}
	else if (strMonth == "05")
	{
		strMonthMMM = "May";
	}
	else if (strMonth == "06")
	{
		strMonthMMM = "Jun";
	}
	else if (strMonth == "07")
	{
		strMonthMMM = "Jul";
	}
	else if (strMonth == "08")
	{
		strMonthMMM = "Aug";
	}
	else if (strMonth == "09")
	{
		strMonthMMM = "Sep";
	}
	else if (strMonth == "10")
	{
		strMonthMMM = "Oct";
	}
	else if (strMonth == "11")
	{
		strMonthMMM = "Nov";
	}
	else if (strMonth == "12")
	{
		strMonthMMM = "Dec";
	}


	//������д
	std::string strDayD;
	if (strDay.find("0") == 0)
	{
		strDayD = strDay.substr(1, 1);
	}
	else
	{
		strDayD = strDay;
	}

	std::string strDayDDD;
	int nYear, nMonth, nDay;
	nYear = atoi(strYear.c_str());
	nMonth = atoi(strMonth.c_str());
	nDay = atoi(strDay.c_str());

	std::string WeekDay = CalculateWeekDay(nYear, nMonth, nDay);
	strDayDDD = WeekDay;

	if (nFormatType == 1)
	{
		strDateDay = strYear + "-" + strMonth + "-" + strDay;
	}
	else if (nFormatType == 2)
	{
		strDateDay = strYearYY + "-" + strMonth + "-" + strDay;
	}
	else if (nFormatType == 3)
	{
		strDateDay = strMonth + "/" + strDay + "/" + strYear;
	}
	else if (nFormatType == 4)
	{
		strDateDay = strMonthMMM + "." + strDay + "." + strYear;
	}
	else if (nFormatType == 5)
	{
		strDateDay = strYear;
	}
	else if (nFormatType == 6)
	{
		strDateDay = strYearYY;
	}
	else if (nFormatType == 7)
	{
		strDateDay = strMonthM;
	}
	else if (nFormatType == 8)
	{
		strDateDay = strMonth;
	}
	else if (nFormatType == 9)
	{
		strDateDay = strMonthMMM;
	}
	else if (nFormatType == 10)
	{
		strDateDay = strDayD;
	}
	else if (nFormatType == 11)
	{
		strDateDay = strDay;
	}
	else if (nFormatType == 12)
	{
		strDateDay = strDayDDD;
	}

	return TRUE;
}

/*
	�������������ȡ�����ʽ������ʱ�䣨����������ֵĲ���Exif��Ϣ��
	nType��ֵ������ʽ���ͣ����յ�������������й�
	nType:1 HH:MM AP/PM
	nType:2 HH:MM:SS
	nType:3 HH
	nType:4 MM
	nType:5 SS
*/
BOOL CKKImageEXIF::GetDateTime(std::string& strDateTime, int nFormatType)
{
	TSAUTO();

	std::string strSrcDateTime;
	if (m_exifs.find("DateTimeOriginal") == m_exifs.end())
	{
		if (m_exifs.find("DateTime") == m_exifs.end())
		{
			TSINFO4CXX("There is no DateTime Info!");
			return FALSE;
		}
		strSrcDateTime = (char*)m_exifs["DateTime"].getValue(); //��ԭʼ������ʱ��
	}
	else
	{
		strSrcDateTime = (char*)m_exifs["DateTimeOriginal"].getValue();
	}

	std::string strHour = strSrcDateTime.substr(11, 2);
	std::string strMinute = strSrcDateTime.substr(14, 2);
	std::string strSecond = strSrcDateTime.substr(17, 2);
	TSINFO4CXX("Hour:"<<strHour<<" Minute:"<<strMinute<<" Second:"<<strSecond);

	int nHour = atoi(strHour.c_str());
	int nMinute = atoi(strMinute.c_str());
	int nSecond = atoi(strSecond.c_str());

	int nNewHour; //����Ǳ�ʾ��24Сʱ�ƻ���12Сʱ��֮���
	std::string strAMPM = "AM";
	if (nHour > 12)
	{
		nNewHour = nHour - 12;
		strAMPM = "PM";
	}
	else if (nHour == 12)
	{
		nNewHour = 12;
		strAMPM = "PM";
	}
	else if (nHour == 0)
	{
		nNewHour = 12;
	}
	else
	{
		nNewHour = nHour;
	}
	char szNewHour[100] = {0};
	sprintf(szNewHour, "%2d", nNewHour);
	std::string strNewHour = szNewHour;

	if (nFormatType == 1)
	{
		strDateTime = strNewHour + ":" + strMinute + " " + strAMPM;
	}
	else if (nFormatType == 2)
	{
		strDateTime = strHour + ":" + strMinute + ":" + strSecond;
	}
	else if (nFormatType == 3)
	{
		strDateTime = strHour;
	}
	else if (nFormatType == 4)
	{
		strDateTime = strMinute;
	}
	else if (nFormatType == 5)
	{
		strDateTime = strSecond;
	}

	return TRUE;
}

BOOL CKKImageEXIF::GetExposureTime(std::string& strExposureTime)
{
	TSAUTO();

	if (m_exifs.find("ExposureTime") == m_exifs.end())
	{
		TSINFO4CXX("There is no ExposureTime Info!");
		return FALSE;
	}

	UINT32 nExposureTime[2];
	memcpy(nExposureTime, m_exifs["ExposureTime"].getValue(), m_exifs["ExposureTime"].getLength());

	if (nExposureTime[0] == 0)
	{
		strExposureTime = "0s";
		return TRUE;
	}

	UINT32 nDenominator = nExposureTime[1]/nExposureTime[0];

	char szExposureTime[100] = {0};
	::StringCchPrintfA(szExposureTime, 100, "1/%d", nDenominator);

	strExposureTime = szExposureTime;

	strExposureTime = strExposureTime + "s";

	TSINFO4CXX("ExposureTime:"<<strExposureTime);

	return TRUE;
}

BOOL CKKImageEXIF::GetFNumber(std::string& strFNumber)
{
	TSAUTO();

	if (m_exifs.find("FNumber") == m_exifs.end())
	{
		TSINFO4CXX("There is no FNumber Info!");
		return FALSE;
	}

	UINT32 nFNumber[2];
	memcpy(nFNumber, m_exifs["FNumber"].getValue(), m_exifs["FNumber"].getLength());

	float fFNumber = (float)nFNumber[0]/nFNumber[1];

	char szFNumber[100] = {0};
	::StringCchPrintfA(szFNumber, 100, "%.1f", fFNumber);

	strFNumber = szFNumber;

	strFNumber = "F" + strFNumber;

	TSINFO4CXX("FNumber:"<<strFNumber);

	return TRUE;
}

BOOL CKKImageEXIF::GetFocalLength(std::string& strFocalLength)
{
	TSAUTO();

	if (m_exifs.find("FocalLength") == m_exifs.end())
	{
		TSINFO4CXX("There is no FocalLength Info!");
		return FALSE;
	}

	UINT32 nFocalLength[2];
	memcpy(nFocalLength, m_exifs["FocalLength"].getValue(), m_exifs["FocalLength"].getLength());

	float fFocalLength = (float)nFocalLength[0]/nFocalLength[1];

	char szFocalLength[100] = {0};
	::StringCchPrintfA(szFocalLength, 100, "%.2f", fFocalLength);

	strFocalLength = szFocalLength;

	TSINFO4CXX("FocalLength:"<<strFocalLength);

	return TRUE;
}

BOOL CKKImageEXIF::GetISO(std::string& strISO)
{
	TSAUTO();

	if (m_exifs.find("ISOSpeedRatings") == m_exifs.end())
	{
		TSINFO4CXX("There is no ISOSpeedRatings Info!");
		return FALSE;
	}

	UINT16 nISO;
	memcpy(&nISO, m_exifs["ISOSpeedRatings"].getValue(), m_exifs["ISOSpeedRatings"].getLength());

	char szISO[100] = {0};
	::StringCchPrintfA(szISO, 100, "ISO%d", nISO);

	strISO = szISO;

	TSINFO4CXX("ISO:"<<strISO);

	return TRUE;
}

BOOL CKKImageEXIF::GetLens(std::string& strLens)
{
	TSAUTO();

	std::wstring wstrLens = m_szLensModel;
	
	strLens = ultra::_T2UTF(wstrLens);
	TSINFO4CXX("strLens:"<<strLens);

	return TRUE;
}

BOOL CKKImageEXIF::GetFlash(std::string& strFlash)
{
	TSAUTO();

	if (m_exifs.find("Flash") == m_exifs.end())
	{
		TSINFO4CXX("There is no Flash Info!");
		return FALSE;
	}

	UINT16 nFlash;
	memcpy(&nFlash, m_exifs["Flash"].getValue(), m_exifs["Flash"].getLength());

	std::wstring wstrFlash;
	if ((nFlash & 0x0001) == 0x0001)
	{
		wstrFlash = _T("��");
	}
	else
	{
		wstrFlash = _T("��");
	}

	strFlash = ultra::_T2UTF(wstrFlash);
	return TRUE;
}

BOOL CKKImageEXIF::GetWhiteBalance(std::string& strWhiteBalance)
{
	TSAUTO();

	if (m_exifs.find("WhiteBalance") == m_exifs.end())
	{
		TSINFO4CXX("There is no WhiteBalance Info!");
		return FALSE;
	}

	UINT16 nWhiteBalance;
	//ע�����ﲻ����m_exifs["WhiteBalance"].getLength()������Ӧ����2��������NIKON D3100�ϲ���13���ӵ���;
	memcpy(&nWhiteBalance, m_exifs["WhiteBalance"].getValue(), 2);

	TSINFO4CXX("nWhiteBalance:"<<nWhiteBalance);

	std::wstring wstrWhiteBalance;
	if (nWhiteBalance == 0)
	{
		wstrWhiteBalance = _T("�Զ�");
	}
	else if (nWhiteBalance == 1)
	{
		wstrWhiteBalance = _T("�ֶ�");
	}
	else
	{
		wstrWhiteBalance = _T("");
	}

	strWhiteBalance = ultra::_T2UTF(wstrWhiteBalance);
	return TRUE;
}

BOOL CKKImageEXIF::GetExposureProgram(std::string& strExposureProgram)
{
	TSAUTO();

	if (m_exifs.find("ExposureProgram") == m_exifs.end() && m_exifs.find("CameraSettings:CanonExposureMode") == m_exifs.end())
	{
		TSINFO4CXX("There is no ExposureProgram Info!");
		return FALSE;
	}

	UINT16 nExposureProgram;
	if (m_exifs.find("ExposureProgram") != m_exifs.end())
	{
		TSINFO4CXX("ExposureProgram!");
		memcpy(&nExposureProgram, m_exifs["ExposureProgram"].getValue(), m_exifs["ExposureProgram"].getLength());
	}
	else
	{
		TSINFO4CXX("CameraSettings:CanonExposureMode!");
		memcpy(&nExposureProgram, m_exifs["CameraSettings:CanonExposureMode"].getValue(), m_exifs["CameraSettings:CanonExposureMode"].getLength());
	}
	TSINFO4CXX("nExposureProgram:"<<nExposureProgram);

	//��ͬ��ȡֵ����Ӧ��ͬ�Ĳ��ģʽ���ο�exif�ĵ�exif2.2.pdf 38ҳ
	/*
	0:δ����(��׼��
	1:�ֶ��ع�
	2:�Զ��ع�
	3:��Ȧ����
	4:��������
	5:����ģʽ
	6:�˶�ģʽ
	7;����ģʽ
	8:�羰ģʽ
	other:reserved
	*/
	
	std::wstring wstrExposureProgram;
	if (nExposureProgram == 0)
	{
		wstrExposureProgram = _T("��׼ģʽ");
	}
	else if (nExposureProgram == 1)
	{
		wstrExposureProgram = _T("�ֶ��ع�");
	}
	else if (nExposureProgram == 2)
	{
		wstrExposureProgram = _T("�Զ��ع�");
	}
	else if (nExposureProgram == 3)
	{
		wstrExposureProgram = _T("��Ȧ����");
	}
	else if (nExposureProgram == 4)
	{
		wstrExposureProgram = _T("��������");
	}
	else if (nExposureProgram == 5)
	{
		wstrExposureProgram = _T("����ģʽ");
	}
	else if (nExposureProgram == 6)
	{
		wstrExposureProgram = _T("�˶�ģʽ");
	}
	else if (nExposureProgram == 7)
	{
		wstrExposureProgram = _T("����ģʽ");
	}
	else if (nExposureProgram == 8)
	{
		wstrExposureProgram = _T("�羰ģʽ");
	}
	else
	{
		wstrExposureProgram = _T("");
	}

	strExposureProgram = ultra::_T2UTF(wstrExposureProgram);
	return TRUE;
}

BOOL CKKImageEXIF::GetMeteringMode(std::string& strMeteringMode)
{
	TSAUTO();

	if (m_exifs.find("MeteringMode") == m_exifs.end())
	{
		TSINFO4CXX("There is no MeteringMode Info!");
		return FALSE;
	}

	UINT16 nMeteringMode;
	memcpy(&nMeteringMode, m_exifs["MeteringMode"].getValue(), m_exifs["MeteringMode"].getLength());

	//��ͬ��ȡֵ����Ӧ��ͬ�Ĳ��ģʽ���ο�exif�ĵ�exif2.2.pdf 40ҳ
	/*
	0:δ֪ģʽ
	1:ƽ�����
	2:�����ص�ƽ�����
	3:����
	4:�����
	5:��Ȩ��⣨���۲�⣩���������⣩�������⣩
	6:�ֲ����
	other:reserved
	255:other
	*/
	
	std::wstring wstrMeteringMode;
	if (nMeteringMode == 0)
	{
		wstrMeteringMode = _T("δ֪���ģʽ");
	}
	else if (nMeteringMode == 1)
	{
		wstrMeteringMode = _T("ƽ�����");
	}
	else if (nMeteringMode == 2)
	{
		wstrMeteringMode = _T("�����ص�ƽ�����");
	}
	else if (nMeteringMode == 3)
	{
		wstrMeteringMode = _T("����");
	}
	else if (nMeteringMode == 4)
	{
		wstrMeteringMode = _T("�����");
	}
	else if (nMeteringMode == 5)
	{
		wstrMeteringMode = _T("��Ȩ���");//���ﱣ���Ϲ�ħ�Ľз�
	}
	else if (nMeteringMode == 6)
	{
		wstrMeteringMode = _T("�ֲ����");
	}
	else if (nMeteringMode == 255)
	{
		wstrMeteringMode = _T("�������ģʽ");
	}
	else
	{
		wstrMeteringMode = _T("");
	}

	strMeteringMode = ultra::_T2UTF(wstrMeteringMode);
	return TRUE;
}

BOOL CKKImageEXIF::GetShutterCount(std::string& strShutterCount)
{
	TSAUTO();

	std::wstring wstrShutterCount = m_szShutterCount;
	strShutterCount = ultra::_T2UTF(wstrShutterCount);
	TSINFO4CXX("wstrShutterCount:"<<wstrShutterCount<<" strShutterCount:"<<strShutterCount);

	return TRUE;
}

BOOL CKKImageEXIF::GetExposureBiasValue(std::string& strExposureBiasValue)
{
	TSAUTO();

	if (m_exifs.find("ExposureBiasValue") == m_exifs.end())
	{
		TSINFO4CXX("There is no ExposureBiasValue Info!");
		return FALSE;
	}

	INT32 nExposureBiasValue[2];
	memcpy(nExposureBiasValue, m_exifs["ExposureBiasValue"].getValue(), m_exifs["ExposureBiasValue"].getLength());

	float fExposureBiasValue = (float)nExposureBiasValue[0]/nExposureBiasValue[1];

	char szExposureBiasValue[100] = {0};
	::StringCchPrintfA(szExposureBiasValue, 100, "%.1f", fExposureBiasValue);

	strExposureBiasValue = szExposureBiasValue;

	TSINFO4CXX("ExposureBiasValue:"<<strExposureBiasValue);

	return TRUE;
}

void CKKImageEXIF::SetImageEXIF(fipImage* fImage)
{
	TSAUTO();

	for (std::map<std::string, fipTag>::iterator pi = m_exifs.begin(); pi != m_exifs.end(); pi++)
	{
		if (pi->first == "ExifRaw")
		{
			fImage->setMetadata(FIMD_EXIF_RAW, (pi->first).c_str(), pi->second);
			break;
		}		
	}
}

std::wstring CKKImageEXIF::ApplyExif(std::wstring wstrText, BOOL bIsReplaceSpecialChar, std::wstring wstrFormatChar)
{
	TSAUTO();

	std::wstring wstrRealText = wstrText;

	//�滻�������
	std::string strMake;
	if (GetMake(strMake) == FALSE)
	{
		strMake = "";
	}
	std::wstring wstrMake;
	wstrMake = ultra::_UTF2T(strMake);

	replace_all_distinct(wstrRealText, L"<Exif:�������>", wstrMake);

	//�滻����ͺ�
	std::string strModel;
	if (GetModel(strModel) == FALSE)
	{
		strModel = "";
	}
	std::wstring wstrModel;
	wstrModel = ultra::_UTF2T(strModel);
	replace_all_distinct(wstrRealText, L"<Exif:����ͺ�>", wstrModel);

	//�滻��������
	std::string strDateDay;
	if (GetDateDay(strDateDay, 1) == FALSE)
	{
		strDateDay = "";
	}
	std::wstring wstrDateDay;
	wstrDateDay = ultra::_UTF2T(strDateDay);

	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�YYYY-MM-DD��>", wstrDateDay);

	if (GetDateDay(strDateDay, 2) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�YY-MM-DD��>", wstrDateDay);

	if (GetDateDay(strDateDay, 3) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�MM/DD/YYYY��>", wstrDateDay);

	if (GetDateDay(strDateDay, 4) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�MMM.DD.YY��>", wstrDateDay);

	if (GetDateDay(strDateDay, 5) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�YYYY��>", wstrDateDay);

	if (GetDateDay(strDateDay, 6) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�YY��>", wstrDateDay);

	if (GetDateDay(strDateDay, 7) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�M��>", wstrDateDay);

	if (GetDateDay(strDateDay, 8) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�MM��>", wstrDateDay);

	if (GetDateDay(strDateDay, 9) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�MMM��>", wstrDateDay);

	if (GetDateDay(strDateDay, 10) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�D��>", wstrDateDay);

	if (GetDateDay(strDateDay, 11) == FALSE)
	{
		strDateDay = "";
	}
	wstrDateDay = ultra::_UTF2T(strDateDay);
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�DD��>", wstrDateDay);

	if (GetDateDay(strDateDay, 12) == FALSE)
	{
		strDateDay = "";
	}
	if (strDateDay == "Monday")
	{
		wstrDateDay = L"����һ";
	}
	else if (strDateDay == "Tuesday")
	{
		wstrDateDay = L"���ڶ�";
	}
	else if (strDateDay == "Wednesday")
	{
		wstrDateDay = L"������";
	}
	else if (strDateDay == "Thursday")
	{
		wstrDateDay = L"������";
	}
	else if (strDateDay == "Friday")
	{
		wstrDateDay = L"������";
	}
	else if (strDateDay == "Saturday")
	{
		wstrDateDay = L"������";
	}
	else if (strDateDay == "Sunday")
	{
		wstrDateDay = L"������";
	}
	replace_all_distinct(wstrRealText, L"<Exif:�������ڣ�DDD��>", wstrDateDay);

	//�滻����ʱ��
	std::string strDateTime;
	std::wstring wstrDateTime;
	if (GetDateTime(strDateTime, 1) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:����ʱ�䣨HH:MM AM/PM��>", wstrDateTime);

	if (GetDateTime(strDateTime, 2) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:����ʱ�䣨HH:MM:SS��>", wstrDateTime);

	if (GetDateTime(strDateTime, 3) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:����ʱ�䣨HH��>", wstrDateTime);

	if (GetDateTime(strDateTime, 4) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:����ʱ�䣨MM��>", wstrDateTime);

	if (GetDateTime(strDateTime, 5) == FALSE)
	{
		strDateTime = "";
	}
	wstrDateTime = ultra::_UTF2T(strDateTime);
	replace_all_distinct(wstrRealText, L"<Exif:����ʱ�䣨SS��>", wstrDateTime);

	//�滻��Ȧ
	std::string strFNumber;
	if (GetFNumber(strFNumber) == FALSE)
	{
		strFNumber = "";
	}
	std::wstring wstrFNumber;
	wstrFNumber = ultra::_UTF2T(strFNumber);
	replace_all_distinct(wstrRealText, L"<Exif:��Ȧ>", wstrFNumber);

	//�滻����
	std::string strExposureTime;
	if (GetExposureTime(strExposureTime) == FALSE)
	{
		strExposureTime = "";
	}
	std::wstring wstrExposureTime;
	wstrExposureTime = ultra::_UTF2T(strExposureTime);
	replace_all_distinct(wstrRealText, L"<Exif:����>", wstrExposureTime);

	//�滻ISO
	std::string strISO;
	if (GetISO(strISO) == FALSE)
	{
		strISO = "";
	}
	std::wstring wstrISO;
	wstrISO = ultra::_UTF2T(strISO);
	replace_all_distinct(wstrRealText, L"<Exif:ISO>", wstrISO);

	//�滻�عⲹ��
	std::string strExposureBiasValue;
	if (GetExposureBiasValue(strExposureBiasValue) == FALSE)
	{
		strExposureBiasValue = "";
	}
	std::wstring wstrExposureBiasValue;
	wstrExposureBiasValue = ultra::_UTF2T(strExposureBiasValue);
	replace_all_distinct(wstrRealText, L"<Exif:�عⲹ��>", wstrExposureBiasValue);

	//�滻����
	std::string strFocalLength;
	if (GetFocalLength(strFocalLength) == FALSE)
	{
		strFocalLength = "";
	}
	std::wstring wstrFocalLength;
	wstrFocalLength = ultra::_UTF2T(strFocalLength);
	replace_all_distinct(wstrRealText, L"<Exif:����>", wstrFocalLength);

	//�滻��ͷ
	std::string strLens;
	if (GetLens(strLens) == FALSE)
	{
		strLens = "";
	}
	std::wstring wstrLens;
	wstrLens = ultra::_UTF2T(strLens);
	replace_all_distinct(wstrRealText, L"<Exif:��ͷ>", wstrLens);

	std::wstring wstrExifSummary = wstrModel + L" " + wstrFNumber + L" " + wstrExposureTime + L" " + wstrISO;
	replace_all_distinct(wstrRealText, L"<Exif:ExifժҪ>", wstrExifSummary);

	if (bIsReplaceSpecialChar == TRUE)
	{
		replace_all_distinct(wstrRealText, L"\\", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"/", wstrFormatChar);
		replace_all_distinct(wstrRealText, L":", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"*", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"?", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"\"", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"<", wstrFormatChar);
		replace_all_distinct(wstrRealText, L">", wstrFormatChar);
		replace_all_distinct(wstrRealText, L"|", wstrFormatChar);
	}

	return wstrRealText;
}

std::string CKKImageEXIF::CalculateWeekDay(int nYear, int nMonth, int nDay)
{
	int week = 0; 
	if(nMonth == 1)
	{
		nMonth = 13;
		nYear--;
	}
	if(nMonth == 2)
	{
		nMonth = 14;
		nYear--;
	}
	if((nYear < 1752) || (( nYear == 1752) && (nMonth < 9)) || ((nYear == 1752) && (nMonth == 9) && (nDay < 3))) //�ж��Ƿ���1752��9��3��֮ǰ 
		week =(nDay + 2*nMonth + 3*(nMonth+1)/5 + nYear + nYear/4+5)%7; //1752��9��3��֮ǰ�Ĺ�ʽ 
	else 
		week =(nDay + 2*nMonth + 3*(nMonth + 1)/5 + nYear + nYear/4 - nYear/100 + nYear/400)%7; //1752��9��3��֮��Ĺ�ʽ 
	string weekstr = "";
	switch(week)
	{
	case 0: {weekstr="Monday"; break;}
	case 1: {weekstr="Tuesday"; break;}
	case 2: {weekstr="Wednesday"; break;}
	case 3: {weekstr="Thursday"; break;}
	case 4: {weekstr="Friday"; break;}
	case 5: {weekstr="Saturday"; break;}
	case 6: {weekstr="Sunday"; break;}
	}

	return weekstr; 
}
int CKKImageEXIF::GetOrientation()
{
	TSAUTO();

	if (m_exifs.find("Orientation") == m_exifs.end())
	{
		TSINFO4CXX("There is no Orientation Info!");
		return -1;
	}

	UINT16 nOrientation = 0;
	memcpy(&nOrientation, m_exifs["Orientation"].getValue(), m_exifs["Orientation"].getLength());
	return nOrientation;
}
