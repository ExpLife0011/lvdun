#pragma once
#include "..\KKImageLoader\KKImageEXIF.h"
#include "..\KKImageLoader\KKImageInfo.h"


class CKKImageDoc
{
public:
	
	CKKImageDoc(void);
	~CKKImageDoc(void);	
	void SetGifObj(XLGP_GIF_HANDLE hGifObj);
	XLGP_GIF_HANDLE GetGifObj();
	
	void SetSrcBitmap(XL_BITMAP_HANDLE hXLBitmap);		// ����ԭʼλͼ
	XL_BITMAP_HANDLE GetSrcBitmap();					// ��ȡԭʼλͼ
	void GetSrcBitmapSize(int& nWidth, int& nHeight);	// ��ȡԭʼλͼ�Ĵ�С
	void ReleaseSrcBitmap();		// �ͷ�ԭʼλͼ							

	
	void SetAdaptedBitmap(XL_BITMAP_HANDLE hXLBitmap);		// ��������λͼ
	XL_BITMAP_HANDLE GetAdaptedBitmap();					// ��ȡ����λͼ
	
	void SetExifInfo(CKKImageEXIF* pExifInfo);		// ����Exif��Ϣ
	void SetImageInfo(CKKImageInfo* pImageInfo);	// ����ͼƬ�ļ���Ϣ
	
	void SetExifInfoStatus(int nExifInfoStatus);
	int GetExifInfoStatus();			// ��ȡexif��Ϣ״̬
	void SetDocType(int nType);		// �����ĵ�����
	int GetDocType();				// ��ȡ�ĵ�����
	void ResetRotate();
	void ResetRotateAngle();
	void RotateBitmap(int nOffsetAngle);					// ��תλͼ
	int GetRotateAngle();									// ��ȡ��ת�Ƕ�
	
	// ͼƬ��Ϣ���
	wstring GetFileName();
	void SetFileName(wstring& wstrFileName);
	wstring GetFilePath();
	void SetFilePath(wstring& wstrFilePath);
	wstring GetFileType();
	DWORD GetFileSize();
	void GetFileImageSize(int& nWidth, int& nHeight);
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

	void SetExposureCompensation(wstring& wstrExposureCompensation);
	wstring GetExposureCompensation();

	void SetFlashStatus(int nFlashStatus);
	int GetFlashStatus();

	void SetWhiteBalance(wstring& wstrWhiteBalance);
	wstring GetWhiteBalance();

	void SetExposureProgram(wstring& wstrExposureProgram);
	wstring GetExposureProgram();

	void SetMeteringMode(wstring& wstrMeteringMode);
	wstring GetMeteringMode();


	int CreateDocId();
	int GetDocId()
	{
		return m_nDocId;
	}
private:
	int m_nDocId;
	int m_nDocType;	//��ͼ���Ƕ�ͼ 0:δ֪ 1:��ͼ 2:��ͼ
	// ��ͨλͼ��Ϣ
	XL_BITMAP_HANDLE m_hSrcBitmap;
	XL_BITMAP_HANDLE m_hAdaptedBitmap;	// ��ԭʼͼ���������ͼ��
	int m_nRotateAngle;		// ��ת�Ƕ�

	// Gif
	XLGP_GIF_HANDLE m_hGifObj;

	// ͼƬ��Ϣ���
	CKKImageEXIF* m_KKImageEXIF; 
	CKKImageInfo* m_KKImageInfo;
private:
	//
};
