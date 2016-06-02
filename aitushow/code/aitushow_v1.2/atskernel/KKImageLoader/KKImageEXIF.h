#pragma once

#include "freeimage/FreeImagePlus.h"
#include <string>
using namespace std;
class CKKImageEXIF
{
public:
	CKKImageEXIF();
	~CKKImageEXIF();

public:
	void Init();
	void AddImageEXIF(std::string strFieldName, fipTag& newTag);
	void SetShutterCount(std::wstring wstrShutterCount);
	void SetLensModel(std::wstring wstrLensModel);
	void SetExifLength(int nExifLength);
	int GetExifLength();

	//������Щ������ʾͼƬ��Ϣʱ
	BOOL GetColorSpace(std::string& strColorSpace); //�����ɫ�ռ�,nCloroSpaceֵΪ0��ʾsRGB��0xFFFFΪadobe RGB����ʵ�����һ����
	BOOL GetMake(std::string& strMake); //������쳧��
	BOOL GetModel(std::string& strModel); //�������ͺ�
	BOOL GetDateTime(std::string& strDateTime); //�����������
	BOOL GetExposureTime(std::string& strExposureTime); //����ع�ʱ�䣨�������ٶ�)
	BOOL GetFNumber(std::string& strFNumber); //��ù�Ȧ
	BOOL GetFocalLength(std::string& strFocalLength); //��ý���
	BOOL GetISO(std::string& strISO); //���ISO
	BOOL GetLens(std::string& strLens); //��������ͷ
	BOOL GetExposureBiasValue(std::string& strExposureBiasValue); //����عⲹ��
	BOOL GetFlash(std::string& strFlash); //��������
	BOOL GetWhiteBalance(std::string& strWhiteBalance); //��ð�ƽ��
	BOOL GetExposureProgram(std::string& strExposureProgram); //����ģʽ
	BOOL GetMeteringMode(std::string& strMeteringMode); //���ģʽ
	BOOL GetShutterCount(std::string& strShutterCount); //��ÿ��Ŵ���
	int GetOrientation();

	//������Щ�ӿ����ڻ�ȡһЩ�����ʽ����Ϣ
	BOOL GetDateDay(std::string& strDateDay, int nFormatType);
	BOOL GetDateTime(std::string& strDateTime, int nFormatType);

	void SetImageEXIF(fipImage* fImage); //��������ڱ���ͼ��ΪJPEG��ʱ�򣬱���EXIF��Ϣ

	std::wstring ApplyExif(std::wstring wstrText, BOOL bIsReplaceSpecialChar = FALSE, std::wstring wstrFormatChar = _T("_")); //��һ�ΰ���exif��Ϣ���ı�Ӧ��exif��Ϣ������Ӧ�ú���ı�

private:
	std::string CalculateWeekDay(int nYear, int nMonth, int nDay);

private:
	std::map<std::string, fipTag> m_exifs;

	wchar_t m_szShutterCount[20];
	wchar_t m_szLensModel[100];

	int m_nExifLength;

private:
	//
};
