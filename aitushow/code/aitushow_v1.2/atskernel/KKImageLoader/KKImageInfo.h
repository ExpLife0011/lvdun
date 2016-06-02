#pragma once

#include <string>

class CKKImageInfo
{
public:
	CKKImageInfo();
	~CKKImageInfo();

public:
	void Init();

	void SetFilePath(std::wstring strFilePath);
	std::wstring GetFilePath();

	void SetFileSize(DWORD nFileSize);
	DWORD GetFileSize();

	void SetFileName(std::wstring strFileName);
	std::wstring GetFileName();

	void SetCreateTime(std::wstring strCreateTime);
	std::wstring GetCreateTime();

	void SetLastEditTime(std::wstring strLastEditTime);
	std::wstring GetLastEditTime();

	void SetImageSize(int nWidth, int nHeight);
	void GetImageSize(int& nWidth, int& nHeight);

	void SetExifInfoStatus(int nExifInfoStatus);
	int GetExifInfoStatus();

	void SetDateTimeOriginal(wstring& wstrDateTimeOriginal);
	wstring GetDateTimeOriginal();

	void SetMake(wstring& wstrMake);
	wstring GetMake();

	void SetModel(wstring& wstrModel);
	wstring GetModel();

	void SetLensType(wstring& wstrLensType);
	wstring GetLensType();

	void SetShutterCount(wstring& wstrShutterCount);
	wstring GetShutterCount();

	void SetShutterSpeed(wstring& wstrLensType);
	wstring GetShutterSpeed();

	void SetFNumber(wstring& wstrFNumber);
	wstring GetFNumber();
	
	void SetFocalLength(wstring& wstrFocalLength);
	wstring GetFocalLength();

	void SetISO(wstring& wstrISO);
	wstring GetISO();
	
	void SetExposureCompensation(wstring& wstrfExposureCompensation);
	wstring GetExposureCompensation();

	void SetFlashStatus(int nFlashStatus);
	int GetFlashStatus();

	void SetWhiteBalance(wstring& wstrWhiteBalance);
	wstring GetWhiteBalance();

	void SetExposureProgram(wstring& wstrExposureProgram);
	wstring GetExposureProgram();

	void SetMeteringMode(wstring& wstrMeteringMode);
	wstring GetMeteringMode();

private:
	std::wstring m_strFilePath;	// ͼƬ�ļ�·��
	DWORD m_nFileSize;			//	ͼƬ�ļ���С
	std::wstring m_strFileName;	// ͼƬ�ļ�����
	std::wstring m_strCreateTime;	// ͼƬ�ļ�����ʱ��
	std::wstring m_strLastEditTime;	// ͼƬ�ļ�����޸�ʱ��
	int m_nWidth;					// ͼƬ��ԭʼ���
	int m_nHeight;					// ͼƬ��ԭʼ�߶�
	int m_nExifInfoStatus;				// ͼƬ��Exif��Ϣ״̬��-1����֪����û�� 0��û�� 1���У����ǻ�û�л�ȡ����2 �У����Ѿ���ȡ����
	wstring m_wstrDateTimeOriginal;	// ����ʱ��
	wstring m_wstrMake;					// �������
	wstring m_wstrModel;				// ����ͺ�
	wstring m_wstrLensType;				// �����ͷ
	wstring m_wstrShutterCount;				// ���Ŵ���
	wstring m_wstrShutterSpeed;			// �����ٶ�
	wstring m_wstrFNumber;					// ��Ȧֵ
	wstring m_wstrFocalLength;			// ����
	wstring m_wstrISO;							// ISO
	wstring m_wstrExposureCompensation;		// �عⲹ��
	int m_nFlashStatus;					// �����״̬��-1 ��֪�� 0 �أ�1��
	wstring m_wstrWhiteBalance;			// ��ƽ��
	wstring m_wstrExposureProgram;		// ����ģʽ
	wstring m_wstrMeteringMode;			// ���ģʽ
private:
	//
};
