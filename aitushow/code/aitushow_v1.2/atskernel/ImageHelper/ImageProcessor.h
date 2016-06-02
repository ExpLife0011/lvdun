#pragma once

#include <GdiPlus.h>
#include <cmath>

class CImageProcessor
{
public:
	CImageProcessor(void);
	~CImageProcessor(void);
	
	static void GetThumbnailSize(const int nSrcWidth, const int nSrcHeight, const int nDstWidth, const int nDstHeight, int& nThumbnailWidth, int& nThumbnailHeight);

	// ����
	static XL_BITMAP_HANDLE RescaleImage(XL_BITMAP_HANDLE hXLBitmap, int nWidth, int nHeight, BOOL IsKeepPercent = FALSE, Gdiplus::InterpolationMode eInterpolationMode = Gdiplus::InterpolationModeBilinear);
	// ��ԭͼ�ϲü�һ��С����
	static XL_BITMAP_HANDLE ClipSubBindBitmap(XL_BITMAP_HANDLE hXLBitmap, int nLeft, int nTop, int nWidth, int nHeight);
	// ��GDI+�е�Imageת����XLBitmap
	static XL_BITMAP_HANDLE ConvertImageToBitmap( Gdiplus::Image* lpImage, UINT width = 0, UINT height = 0 );
	// ��HBitmapת��XLBitmap
	static XL_BITMAP_HANDLE ConvertDIBToXLBitmap32( HBITMAP hBitmap );

	static XL_BITMAP_HANDLE ImageRotate180(XL_BITMAP_HANDLE hXLBitmap); //��ת180��
	static XL_BITMAP_HANDLE ImageLeftRotate(XL_BITMAP_HANDLE hXLBitmap); //����
	static XL_BITMAP_HANDLE GetImageByARGB(int nWidth, int nHeight, int nAlpha, int nRed, int nGreen, int nBlue); //���ָ����ɫ��ָ����С��ͼƬ
	static XL_BITMAP_HANDLE ImageRightRotate(XL_BITMAP_HANDLE hXLBitmap); //����
	static XL_BITMAP_HANDLE ImageLeftRightMirror(XL_BITMAP_HANDLE hXLBitmap); //���ҷ�ת
	static XL_BITMAP_HANDLE ImageTopBottomMirror(XL_BITMAP_HANDLE hXLBitmap); //���·�ת
	static XL_BITMAP_HANDLE RotateImage(XL_BITMAP_HANDLE hXLBitmap, double dAngle, int alpha = 0, int r = 0, int g = 0, int b = 0); //��ת����Ƕ�
	static XL_BITMAP_HANDLE DrawThumbnailViewLayer(int nTotalWidth, int nTotalHeight, int nLeftPos, int nTopPos, int nRectWidth, int nRectHeight);
private:	
	//
};
