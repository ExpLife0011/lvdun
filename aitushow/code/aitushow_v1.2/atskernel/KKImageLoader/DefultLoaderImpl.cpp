#include "StdAfx.h"
#include "DefultLoaderImpl.h"
#include <cmath>
#include <strsafe.h>
#include "..\ImageHelper\exif_thumbnail_length.h"
#include "..\ImageHelper\ImageProcessor.h"
#include "..\ImageHelper\ImageUtility.h"

CDefaultLoaderImpl::CDefaultLoaderImpl(void)
{
	m_hXLBitmap = NULL;
	m_nSrcBitmapWidth = 0;
	m_nSrcBitmapHeight = 0;
	m_nExifInfoStatus = 0;
}

CDefaultLoaderImpl::~CDefaultLoaderImpl(void)
{
}

BOOL CDefaultLoaderImpl::AutoRotate()
{
	int nOrientationValue = m_KKImageEXIF->GetOrientation();
	XL_BITMAP_HANDLE hNewBitmap = NULL;
	XL_BITMAP_HANDLE hTmpBitmap = NULL;
	switch (nOrientationValue)
	{
	case 1:
		//�����������Ҫת��
		return FALSE;
		break;
	case 2: //ֱ��ˮƽ��ת
		hNewBitmap = CImageProcessor::ImageLeftRightMirror(m_hXLBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
		}
		break;
	case 3: //180����ת
		hNewBitmap = CImageProcessor::ImageRotate180(m_hXLBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
		}
		break;
	case 4: //ֱ�Ӵ�ֱ��ת
		hNewBitmap = CImageProcessor::ImageTopBottomMirror(m_hXLBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
		}
		break;
	case 5:
		//��ת90�ȣ�Ȼ�����ҷ�ת
		hTmpBitmap = CImageProcessor::ImageRightRotate(m_hXLBitmap);
		if (hTmpBitmap)	// ��ת�ɹ�
		{
			// ���ͷŵ���������ռ�ڴ�
			XL_ReleaseBitmap(m_hXLBitmap);
			hNewBitmap = CImageProcessor::ImageLeftRightMirror(hTmpBitmap);
			if (hNewBitmap) // ���Ҿ���ɹ�
			{
				XL_ReleaseBitmap(hTmpBitmap);
				m_hXLBitmap = hNewBitmap;
			}
			else
			{
				m_hXLBitmap = hTmpBitmap;
			}
			// �޸� �ߴ�����
			int nTemp = m_nSrcBitmapWidth;
			m_nSrcBitmapWidth = m_nSrcBitmapHeight;
			m_nSrcBitmapHeight = nTemp;
		}
		break;
	case 6: //��ת90��
		hNewBitmap = CImageProcessor::ImageRightRotate(m_hXLBitmap);
		if (hNewBitmap) // ��ת�ɹ�
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
			// �޸� �ߴ�����
			int nTemp = m_nSrcBitmapWidth;
			m_nSrcBitmapWidth = m_nSrcBitmapHeight;
			m_nSrcBitmapHeight = nTemp;
		}
		break;
	case 7:
		//��ת90�ȣ�Ȼ�����ҷ�ת
		hTmpBitmap = CImageProcessor::ImageLeftRotate(m_hXLBitmap);
		if (hTmpBitmap)	// ��ת�ɹ�
		{
			// ���ͷŵ���������ռ�ڴ�
			XL_ReleaseBitmap(m_hXLBitmap);
			hNewBitmap = CImageProcessor::ImageLeftRightMirror(hTmpBitmap);
			if (hNewBitmap) // ���Ҿ���ɹ�
			{
				XL_ReleaseBitmap(hTmpBitmap);
				m_hXLBitmap = hNewBitmap;
			}
			else
			{
				m_hXLBitmap = hTmpBitmap;
			}
			// �޸� �ߴ�����
			int nTemp = m_nSrcBitmapWidth;
			m_nSrcBitmapWidth = m_nSrcBitmapHeight;
			m_nSrcBitmapHeight = nTemp;
		}
		break;
	case 8: //��ת90��
		hNewBitmap = CImageProcessor::ImageLeftRotate(m_hXLBitmap);
		if (hNewBitmap) // ��ת�ɹ�
		{
			XL_ReleaseBitmap(m_hXLBitmap);
			m_hXLBitmap = hNewBitmap;
			// �޸� �ߴ�����
			int nTemp = m_nSrcBitmapWidth;
			m_nSrcBitmapWidth = m_nSrcBitmapHeight;
			m_nSrcBitmapHeight = nTemp;
		}
		break;
	default:
		return FALSE;
		break;
	}
}

int CDefaultLoaderImpl::LoadImage(const wstring& wstrFilePath, bool* pbStop, bool bScale, int nWidth, int nHeight)
{
	TSAUTO();
	TSINFO4CXX(L"wstrFilePath:"<<wstrFilePath);

	// ��ʼ��Exif��Ϣ����
	if (m_KKImageEXIF)
	{
		delete m_KKImageEXIF;
		m_KKImageEXIF = NULL;
	}
	m_KKImageEXIF = new CKKImageEXIF();

	// �����ļ�
	fipWinImage newImage;
	FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(wstrFilePath.c_str());

	if (fif == FIF_UNKNOWN)
	{
		TSERROR4CXX(_T("����ʶ����ļ���ʽ��"));
		return 3;
	}
	//����raw�ļ�
	else if (fif == FIF_RAW)
	{
		TSINFO4CXX("This is a raw format file!");
		if (!LoadRAWImage(wstrFilePath.c_str(), newImage))
		{
			TSERROR4CXX("Load raw image failed!");
			return 4;
		}
		m_nExifInfoStatus = 1;
	}
	//����JPEG�ļ�
	//else if (fif == FIF_JPEG)
	//{
	//	TSINFO4CXX("This is a jpeg format file!");
	//	m_hXLBitmap = LoadJPEGImage(wstrFilePath.c_str(), TRUE);
	//	assert(m_hXLBitmap);
	//	if (m_hXLBitmap == NULL)
	//	{
	//		return 4;
	//	}
	//	m_nExifInfoStatus = 1;
	//}
	else
	{
		
		if (fif == FIF_PSD)	// PSD�ļ�
		{
			// ����
			if (!newImage.loadU(wstrFilePath.c_str(), PSD_CMYK))
			{
				TSERROR4CXX(_T("����ͼƬʧ�ܣ�"));
				return 4;
			}
		}
		else
		{
			if (!newImage.loadU(wstrFilePath.c_str()))
			{
				TSERROR4CXX(_T("����ͼƬʧ�ܣ�"));
				return 4;
			}
		}

		// �����Ƿ���EXIF��Ϣ״̬
		if (fif == FIF_TIFF)
		{
			m_nExifInfoStatus = 1;
		}
		// �����⴦��
		wstring wstrMonitorIccFilePath = CImageUtility::GetDisplayMonitorICCFilePath();

		// �ж��ǲ���CMYK
		FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(newImage);
		if(pImageICC)
		{
			if (pImageICC->flags & FIICC_COLOR_IS_CMYK)
			{
				CMYKtosRGB(newImage, wstrMonitorIccFilePath);
			}
			else	// ����CMYK���ж���û��ICC������У�Ҳ��ת��
			{
				if(pImageICC->data) // ��ICC
				{
					ApplyICC(newImage, wstrMonitorIccFilePath);
				}
				else	// û��ICC
				{
					if (PathFileExists(wstrMonitorIccFilePath.c_str()))
					{	
						sRGBtoMonitorRGB(newImage, wstrMonitorIccFilePath);
					}
				}
			}
		}
		else // �ж���ʾ����ICC
		{
			if (PathFileExists(wstrMonitorIccFilePath.c_str()))
			{
				sRGBtoMonitorRGB(newImage, wstrMonitorIccFilePath);
			}
		}
		
	}
	if (m_hXLBitmap == NULL)	// ֻ����Raw��ʽ�²Ż���õ�
	{
		m_hXLBitmap = ConvertFipImagetoXLBitmapHandle(newImage);
	}
	
	assert(m_hXLBitmap);
	if (m_hXLBitmap == NULL)
	{
		return 4;
	}

	if (fif == FIF_BMP)
	{
		//����32λBMP����������ﻹ���Ƚ�����alphaȫ����Ϊ255
		XL_ResetAlphaChannel(m_hXLBitmap, 255);
	}
	XLBitmapInfo bmpInfo;
	XL_GetBitmapInfo(m_hXLBitmap, &bmpInfo);
	m_nSrcBitmapWidth = bmpInfo.Width;
	m_nSrcBitmapHeight = bmpInfo.Height;
	// ������
	if (bScale)// ����������
	{
		double dRatio = 0;
		double nMaxLength = nWidth>nHeight?nWidth:nHeight;
		double nMinLength = nWidth<nHeight?nWidth:nHeight;

		double nImageMaxLength = m_nSrcBitmapWidth>m_nSrcBitmapHeight?m_nSrcBitmapWidth:m_nSrcBitmapHeight;
		double nImageMinLength = m_nSrcBitmapWidth<m_nSrcBitmapHeight?m_nSrcBitmapWidth:m_nSrcBitmapHeight;

		if (nMaxLength >= nImageMaxLength && nMinLength >= nImageMinLength) // ������
		{
		}
		else	// ����
		{
			if (nMaxLength/nImageMaxLength > nMinLength/nImageMinLength)
			{
				dRatio = nMinLength/nImageMinLength;
			}
			else
			{
				dRatio = nMaxLength/nImageMaxLength;
			}
			TSINFO4CXX("LoadFile by default method, dRatio:" << dRatio);
			XL_BITMAP_HANDLE hNewBitmap = CImageProcessor::RescaleImage(m_hXLBitmap, m_nSrcBitmapWidth*dRatio, m_nSrcBitmapHeight*dRatio);
			if (hNewBitmap)
			{
				XL_ReleaseBitmap(m_hXLBitmap);
				m_hXLBitmap = hNewBitmap;
			}
		}
	}
	return 0;
}

BOOL CDefaultLoaderImpl::LoadRAWImage(LPCTSTR lpszPathName, fipWinImage& newImage)
{
	TSAUTO();

	if (!newImage.loadU(lpszPathName, RAW_PREVIEW))
	{
		TSERROR4CXX(_T("����rawͼƬʧ�ܣ�"));
		return FALSE;
	}

	// ��ȡJPEG�ļ���EXIF��Ϣ
	// FIMD_EXIF_MAIN��FIMD_EXIF_EXIF�����������Ҫ������ȡ��ص���Ϣ������д��
	// FIMD_EXIF_RAW�������Ϣ���ֶΡ�ExifRaw��ֻ����д���ڱ���ͼƬ��ʱ�򣬱������setMetadata�������õ������ͼƬ����ȥ

	fipMetadataFind MetadataFind;
	fipTag tag;
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAIN, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_MAIN:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_EXIF, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_EXIF:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_GPS, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_GPS:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAKERNOTE, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_MAKERNOTE:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_INTEROP, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_INTEROP:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_RAW, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
			TSINFO4CXX("FIMD_EXIF_RAW:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
				<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());

			//����exif��Ϣ����
			DWORD tag_length = tag.getLength();
			DWORD thumbnail_length = get_thumbnail_length((unsigned char*)tag.getValue(), tag_length);
			tag_length += 8;
			tag_length -= thumbnail_length;
			m_KKImageEXIF->SetExifLength(tag_length);

		} while (MetadataFind.findNextMetadata(tag));
	}

	// ��ȡ��ʾ����ICC�ļ�·��
	wstring wstrMonitorIccFilePath = CImageUtility::GetDisplayMonitorICCFilePath();
	//�ж��Ƿ���Adobe RGB��ɫ�ռ䣬����ǣ��������ɫ�ռ��ת��
	std::string strColorSpace;
	if (m_KKImageEXIF->GetColorSpace(strColorSpace) && strColorSpace == "Adobe RGB")
	{
		TSINFO4CXX("this is a adobe RGB image!");
		//�����Adobe RGB��ɫ�ռ䣬�������ת��
		if (!AdobeRGBtosRGB(newImage, wstrMonitorIccFilePath))
		{
			TSERROR4CXX("convert adobe RGB to sRGB failed!");
			return FALSE;
		}
	}
	else
	{
		// �ж���ʾ����û��ICC
		if (PathFileExists(wstrMonitorIccFilePath.c_str()))
		{
			sRGBtoMonitorRGB(newImage, wstrMonitorIccFilePath);
		}
	}
	return TRUE;

}

//���ð����ṩ�Ľӿڣ��ٶȿ���200ms���ú��������ϲ��ᱻ����
XL_BITMAP_HANDLE CDefaultLoaderImpl::LoadJPEGImage(LPCTSTR lpszPathName, BOOL IsLoadExif)
{
	TSAUTO();

	//Ϊ�˱�֤����ɫ�ʿռ��ͼ���������ƴͼ���涼����ʾ��ǿ��loadexif
	IsLoadExif = TRUE;

	fipWinImage newImage;

	//����ͷ����Ϣ
	//ʹ��JPEG_CMYK��Ŀ���ǣ����jpg��JCS_CMYK, ��Ҫ��freeimage����cmyk-rgb��ת������freeimage��icc��Ϣ�м�¼һ��FIICC_COLOR_IS_CMYK�ı�־
	if (!newImage.loadU(lpszPathName, JPEG_CMYK))
	{
		TSINFO4CXX("Load Head Failed!");
		return NULL;
	}

	int nWidth = newImage.getWidth();
	int nHeight = newImage.getHeight();

	//����Exif��Ϣ
	if (IsLoadExif == TRUE)
	{
		// ��ȡJPEG�ļ���EXIF��Ϣ
		// FIMD_EXIF_MAIN��FIMD_EXIF_EXIF�����������Ҫ������ȡ��ص���Ϣ������д��
		// FIMD_EXIF_RAW�������Ϣ���ֶΡ�ExifRaw��ֻ����д���ڱ���ͼƬ��ʱ�򣬱������setMetadata�������õ������ͼƬ����ȥ

		fipMetadataFind MetadataFind;
		fipTag tag;
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAIN, newImage, tag))
		{
			do
			{
				std::string strFieldName = tag.getKey();
				m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
				TSINFO4CXX("FIMD_EXIF_MAIN:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
					<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
			} while (MetadataFind.findNextMetadata(tag));
		}
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_EXIF, newImage, tag))
		{
			do
			{
				std::string strFieldName = tag.getKey();
				m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
				TSINFO4CXX("FIMD_EXIF_EXIF:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
					<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
			} while (MetadataFind.findNextMetadata(tag));
		}
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAKERNOTE, newImage, tag))
		{
			do 
			{
				std::string strFieldName = tag.getKey();
				m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
				TSINFO4CXX("FIMD_EXIF_MAKERNOTE:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
					<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
			} while (MetadataFind.findNextMetadata(tag));
		}
		/*if (MetadataFind.findFirstMetadata(FIMD_EXIF_GPS, newImage, tag))
		{
		do
		{
		std::string strFieldName = tag.getKey();
		m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
		//TSINFO4CXX("FIMD_EXIF_GPS:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
		//<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
		}		
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_INTEROP, newImage, tag))
		{
		do 
		{
		std::string strFieldName = tag.getKey();
		m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
		TSINFO4CXX("FIMD_EXIF_INTEROP:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
		<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
		}*/
		if (MetadataFind.findFirstMetadata(FIMD_EXIF_RAW, newImage, tag))
		{
			do 
			{
				std::string strFieldName = tag.getKey();
				m_KKImageEXIF->AddImageEXIF(strFieldName, tag);
				TSINFO4CXX("FIMD_EXIF_RAW:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
					<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());

				//����exif��Ϣ����
				DWORD tag_length = tag.getLength();
				DWORD thumbnail_length = get_thumbnail_length((unsigned char*)tag.getValue(), tag_length);
				tag_length += 8;
				tag_length -= thumbnail_length;
				m_KKImageEXIF->SetExifLength(tag_length);

			} while (MetadataFind.findNextMetadata(tag));
		}
	}

	DWORD time1 = ::GetTickCount();

	XL_BITMAP_HANDLE hNewBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, nWidth, nHeight);
	assert(hNewBitmap);
	if (hNewBitmap == NULL)
	{
		TSINFO4CXX("XL_CreateBitmap Failed!");
		return NULL;
	}

	DWORD time2 = ::GetTickCount();

	BYTE* BmpBuf = XL_GetBitmapBuffer(hNewBitmap, 0, 0);
	
	int ret = -1;
	//int ret = FreeImage_Jpeg_LoadU(lpszPathName, BmpBuf);
	if (ret != 0)
	{
		TSINFO4CXX("Load Jpeg Failed!");
		return NULL;
	}

	//�ж��Ƿ���Adobe RGB��cmyk��ɫ�ռ䣬����ǣ��������ɫ�ռ��ת��
	if (IsLoadExif == TRUE)
	{
		FIICCPROFILE* pImageICC = FreeImage_GetICCProfile(newImage);
		if(pImageICC && (pImageICC->flags & FIICC_COLOR_IS_CMYK) )
		{
			if (!CMYKtosRGB(newImage,hNewBitmap))
			{
				TSERROR4CXX("convert CMYK to sRGB failed!");
				return FALSE;
			}
		}
		else
		{
			std::string strColorSpace;
			if (m_KKImageEXIF->GetColorSpace(strColorSpace) && strColorSpace == "Adobe RGB")
			{
				TSINFO4CXX("this is a adobe RGB image!");
				//�����Adobe RGB��ɫ�ռ䣬�������ת��
				if (!AdobeRGBtosRGB(newImage, hNewBitmap))
				{
					TSERROR4CXX("convert adobe RGB to sRGB failed!");
					return NULL;
				}
			}
		}
	}

	DWORD time3 = ::GetTickCount();

	TSINFO4CXX("time2 - time1:"<<time2 - time1<<" time3 - time2:"<<time3 - time2);

	return hNewBitmap;
}