#pragma once
#include "ImageLoaderImpl.h"

typedef enum{
	KKImg_Type_InValid = 0,
	KKImg_Type_Jpeg,
	KKImg_Type_Gif,
	KKImg_Type_Png,
	KKImg_Type_Psd,
	KKImg_Type_Default

}ImageLoaderImplType;

// ͼƬ�ļ������࣬�����ṩ�ӿڣ��ڲ������ļ����͵�����Ӧ��ʵ�ʼ�����
class CImageLoader
{
public:
	CImageLoader(void);
	~CImageLoader(void);

	// ͨ��

	// ����ֵ˵����0 �ɹ������Ե���GetXLBitmap��ȡ��������λͼ�� 1 ·��Ϊ�� 2 �ļ������� 3 ����ʶ��ĸ�ʽ 4 ����ʧ��
	int LoadImage(const wstring& wstrFilePath, bool* pbStop = NULL, bool bScale = false, int nWidth = 0, int nHeight = 0, bool bAutoRotate = false);
	CKKImageEXIF* LoadImageFileOnlyExif(LPCTSTR lpszPathName); //������ͼƬ��exif��Ϣ
	ImageLoaderImplType GetLoaderType();				// ��ȡͼƬ�ļ�����������
	CKKImageEXIF* GetExifInfo(bool bReset = true);		// ��ȡExif��Ϣ
	CKKImageInfo* GetImageInfo(bool bReset = true);	// ��ȡͼƬ�ļ�������Ϣ
	BOOL SaveImage(XL_BITMAP_HANDLE hSaveBitmap, LPCTSTR lpszPathName, CKKImageEXIF* pKKImageEXIF = NULL, BOOL IsDelExif = FALSE,	int nJPEGQuality = 100, BOOL IsHighQuality = FALSE, int nDPI = -1);

	
	// ��ͨͼƬ�ļ����
	XL_BITMAP_HANDLE GetXLBitmap();
	XLGP_GIF_HANDLE GetXLGifObj();
	
private:
	ImageLoaderImplType m_nType;	// ��������������
	CImageLoaderImpl* m_LoaderImpl;	// ����ļ�����
	CKKImageInfo* m_KKImageInfo;
	//
};
