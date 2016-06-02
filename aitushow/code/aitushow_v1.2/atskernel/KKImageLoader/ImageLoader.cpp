#include "StdAfx.h"
#include "ImageLoader.h"
#include "JpegLoaderImpl.h"
#include "GifLoaderImpl.h"
#include "WmfLoaderImpl.h"
#include "PsdLoaderImpl.h"
#include "DefultLoaderImpl.h"
#include "..\ImageHelper\exif_thumbnail_length.h"
#include "..\Utility\StringOperation.h"

CImageLoader::CImageLoader(void)
{
	m_LoaderImpl = NULL;
	m_KKImageInfo = NULL;
	m_nType = KKImg_Type_InValid;

}

CImageLoader::~CImageLoader(void)
{
	if (m_LoaderImpl)
	{
		delete m_LoaderImpl;
		m_LoaderImpl = NULL;
	}
	if (m_KKImageInfo)
	{
		delete m_KKImageInfo;
		m_KKImageInfo = NULL;
	}
}

int CImageLoader::LoadImage(const wstring& wstrFilePath, bool* pbStop, bool bScale, int nWidth, int nHeight, bool bAutoRotate)
{
	TSAUTO();
	TSINFO4CXX("���ص��ļ�·����" << wstrFilePath.c_str());
	int nRet = 0;
	if (wstrFilePath.empty())
	{
		nRet = 1;
		TSINFO4CXX("wstrFilePath is empty and return " << nRet);
		return nRet;
	}
	if (!PathFileExists(wstrFilePath.c_str()))
	{
		nRet = 2;
		TSINFO4CXX("File is not Exists and return "<<nRet);
		return nRet;
	}
	// �ͷŵ��ϴεļ�����
	if (m_LoaderImpl)
	{
		delete m_LoaderImpl;
		m_LoaderImpl = NULL;
	}
	// �ж��ļ�����
	// ��ȡ��׺��
	wstring wstrExtName = PathFindExtensionW(wstrFilePath.c_str());
	// ת����Сд
	wstring lowerExtName = ultra::ToLower(wstrExtName);
	if (lowerExtName == L".jpg" || lowerExtName == L".jpeg" || lowerExtName == L".jpe")
	{
		m_nType = KKImg_Type_Jpeg;
		m_LoaderImpl = new CJpegLoaderImpl();
	}
	else if (lowerExtName == L".gif")
	{
		m_nType = KKImg_Type_Gif;
		m_LoaderImpl = new CGifLoaderImpl();
	}
	else if (lowerExtName == L".wmf")
	{
		m_nType = KKImg_Type_Default;
		m_LoaderImpl = new CWmfLoaderImpl();
	}
	else
	{
		if (lowerExtName == L".png")
		{
			m_nType = KKImg_Type_Png;
		}
		else
		{
			m_nType = KKImg_Type_Default;
		}
		m_LoaderImpl = new CDefaultLoaderImpl();
	}
	// ����λͼ
	nRet = m_LoaderImpl->LoadImage(wstrFilePath, pbStop, bScale, nWidth, nHeight);

	if (nRet != 0 && m_nType == KKImg_Type_Jpeg)	// ��׺�����ԣ����¼���
	{
		m_nType = KKImg_Type_Default;
		delete m_LoaderImpl;
		m_LoaderImpl = new CDefaultLoaderImpl();
		nRet = m_LoaderImpl->LoadImage(wstrFilePath, pbStop, bScale, nWidth, nHeight);
	}
	
	// ��ȡͼ����Ϣ
	if (nRet == 0)
	{
		// �ж��Ƿ��Զ�ת��
		if(bAutoRotate)
		{
			m_LoaderImpl->AutoRotate();
		}
		m_KKImageInfo = new CKKImageInfo();
		// ����·��
		m_KKImageInfo->SetFilePath(wstrFilePath);
		// �����ļ���
		m_KKImageInfo->SetFileName(::PathFindFileName(wstrFilePath.c_str()));
		// ����ͼƬ�ߴ�
		int nWidth, nHeight;
		m_LoaderImpl->GetSrcBitmapSize(nWidth, nHeight);
		m_KKImageInfo->SetImageSize(nWidth, nHeight);
		// ����Exif��־
		m_KKImageInfo->SetExifInfoStatus(m_LoaderImpl->GetExifInfoStatus());
		
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = FindFirstFile(wstrFilePath.c_str(), &FindFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			//��ȡ�ļ���С(�ֽ�Ϊ��λ��
			DWORD nFileSize = (FindFileData.nFileSizeHigh * MAXDWORD + 1) + FindFileData.nFileSizeLow;
			m_KKImageInfo->SetFileSize(nFileSize);

			//��ȡ�ļ�����ʱ��
			SYSTEMTIME stUTC, stLocal;
			FileTimeToSystemTime(&(FindFileData.ftCreationTime), &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			TCHAR szCreationTime[100] = {0};
			wsprintf(szCreationTime, _T("%d/%d/%d %d:%d"), stLocal.wYear, stLocal.wMonth, stLocal. wDay, stLocal.wHour, stLocal.wMinute);
			m_KKImageInfo->SetCreateTime(szCreationTime);

			//��ȡ�ļ�����޸�ʱ��
			FileTimeToSystemTime(&(FindFileData.ftLastWriteTime), &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			TCHAR szLastWriteTime[100] = {0};
			wsprintf(szLastWriteTime, _T("%d/%d/%d %d:%d"), stLocal.wYear, stLocal.wMonth, stLocal. wDay, stLocal.wHour, stLocal.wMinute);
			m_KKImageInfo->SetLastEditTime(szLastWriteTime);
			FindClose(hFind);
		}
	}
	return nRet;
}
ImageLoaderImplType CImageLoader::GetLoaderType()
{
	return m_nType;
}

XL_BITMAP_HANDLE CImageLoader::GetXLBitmap()
{
	if (m_LoaderImpl)
	{
		return m_LoaderImpl->GetXLBitmap();
	}
	return NULL;
}

CKKImageEXIF* CImageLoader::GetExifInfo(bool bReset)
{
	return NULL;
}
CKKImageInfo* CImageLoader::GetImageInfo(bool bReset)
{
	CKKImageInfo* pTemp = m_KKImageInfo;
	if (bReset)
	{
		m_KKImageInfo = NULL;
	}
	return pTemp;
}

XLGP_GIF_HANDLE CImageLoader::GetXLGifObj()
{
	if (m_nType != KKImg_Type_Gif)
	{
		return NULL;
	}
	if (m_LoaderImpl)
	{
		return m_LoaderImpl->GetXLGifObj();
	}
	return NULL;
}
//SaveImage������hSaveBitmap��Դ���ͷ�
BOOL CImageLoader::SaveImage(XL_BITMAP_HANDLE hSaveBitmap, LPCTSTR lpszPathName, CKKImageEXIF* pKKImageEXIF, BOOL IsDelExif, int nJPEGQuality, BOOL IsHighQuality, int nDPI)
{
	TSAUTO();

	XLBitmapInfo SaveBmpInfo;
	XL_GetBitmapInfo(hSaveBitmap, &SaveBmpInfo);

	XL_BITMAP_HANDLE hDstBitmap = NULL;

	FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(lpszPathName);
	if (fif == FIF_UNKNOWN)
	{
		TSERROR4CXX(_T("�ļ������ʽ����"));
		return FALSE;
	}

	if (fif != FIF_PNG)
	{
		//�������PNG����ô�Ⱥʹ���ɫ�ĵ׽��л�ϣ��ٱ���
		//�����hSaveBitmap��Doc�и��ƹ��ģ������ǿ����޸ĵ�		
		hDstBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, SaveBmpInfo.Width, SaveBmpInfo.Height);
		assert(hDstBitmap);
		if (hDstBitmap == NULL)
		{
			TSERROR4CXX("Create hDstBitmap Failed!");
			return FALSE;
		}
		XL_FillBitmap(hDstBitmap, XLCOLOR_BGRA(255, 255, 255, 255));
		XL_PreMultiplyBitmap(hSaveBitmap); //���ǰҪԤ��
		XLGP_SrcAlphaBlend(hDstBitmap, 0, 0, hSaveBitmap, 255);
	}

	fipWinImage _fipWinImage;
	if (fif != FIF_PNG)
	{
		//����PNG��ת����24λ��
		//ע�⣬Freeimage�������Ǵ��µ��ϣ���XL_BITMAP_HANDLE���Ǵ��ϵ��£����Կ������ص�ʱ��
		_fipWinImage.setSize(FIT_BITMAP, SaveBmpInfo.Width, SaveBmpInfo.Height, 24);
		unsigned nFipScanLength = _fipWinImage.getScanWidth();
		BYTE* FipBuf = _fipWinImage.accessPixels();
		BYTE* SaveBitmapBuf = XL_GetBitmapBuffer(hDstBitmap, 0, 0);
		for (int j = 0; j < SaveBmpInfo.Height; j++)
		{
			BYTE* FipLine = FipBuf + nFipScanLength*j;
			BYTE* SaveBitmapLine = SaveBitmapBuf + SaveBmpInfo.ScanLineLength*(SaveBmpInfo.Height - 1 -j);
			for (int i = 0; i < SaveBmpInfo.Width; i++)
			{
				BYTE* FipPixel = FipLine + 3*i;
				BYTE* SaveBitmapPixel = SaveBitmapLine + 4*i;
				FipPixel[0] = SaveBitmapPixel[0];
				FipPixel[1] = SaveBitmapPixel[1];
				FipPixel[2] = SaveBitmapPixel[2];
			}
		}

		XL_ReleaseBitmap(hDstBitmap);
	}
	else
	{
		//PNGת����32λ��
		_fipWinImage.setSize(FIT_BITMAP, SaveBmpInfo.Width, SaveBmpInfo.Height, 32);
		unsigned nFipScanLength = _fipWinImage.getScanWidth();
		BYTE* FipBuf = _fipWinImage.accessPixels();
		BYTE* SaveBitmapBuf = XL_GetBitmapBuffer(hSaveBitmap, 0, 0);
		for (int j = 0; j < SaveBmpInfo.Height; j++)
		{
			BYTE* FipLine = FipBuf + nFipScanLength*j;
			BYTE* SaveBitmapLine = SaveBitmapBuf + SaveBmpInfo.ScanLineLength*(SaveBmpInfo.Height - 1 -j);
			for (int i = 0; i < SaveBmpInfo.Width; i++)
			{
				BYTE* FipPixel = FipLine + 4*i;
				BYTE* SaveBitmapPixel = SaveBitmapLine + 4*i;
				FipPixel[0] = SaveBitmapPixel[0];
				FipPixel[1] = SaveBitmapPixel[1];
				FipPixel[2] = SaveBitmapPixel[2];
				FipPixel[3] = SaveBitmapPixel[3];
			}
		}
	}

	if (fif == FIF_JPEG)
	{
		TSINFO4CXX("Save Type is JPEG");
		if (IsDelExif == FALSE)
		{
			//���ﱣ��Exif��Ϣ
			if (pKKImageEXIF != NULL)
			{
				pKKImageEXIF->SetImageEXIF(&_fipWinImage);
			}
		}
	}
	else if (fif == FIF_GIF)
	{
		TSINFO4CXX("����������GIF��");
		//ת����gif֮ǰ����Ҫ�Ƚ�λͼת����8bit��
		if (!_fipWinImage.colorQuantize(FIQ_WUQUANT))
		{
			TSINFO4CXX(_T("ת����8bitλͼʧ�ܣ�"));
			return FALSE;
		}
	}

	// ȷ���ļ�Ŀ¼����
	wchar_t szFilePath[MAX_PATH];
	swprintf(szFilePath, L"%s", lpszPathName);
	::PathRemoveFileSpec(szFilePath);
	// �����Ŀ¼�����ڵĻ����´����µ�Ŀ¼
	if (!::PathFileExists(szFilePath))
	{
		int nRet = SHCreateDirectoryEx(NULL,szFilePath,NULL);
		if (nRet != ERROR_SUCCESS && nRet != ERROR_FILE_EXISTS && nRet != ERROR_ALREADY_EXISTS)
		{
			return FALSE;
		}
	}

	if (fif == FIF_JPEG)
	{
		TSINFO4CXX(L"nJPEGQuality = " << nJPEGQuality);

		DWORD flag = 0;
		if (IsHighQuality == TRUE)
		{
			flag = JPEG_SUBSAMPLING_444;
		}
		TSINFO4CXX("lpszPathName:"<<lpszPathName);
		if (!_fipWinImage.saveU(lpszPathName, nJPEGQuality | flag))
		{
			TSERROR4CXX(_T("����JPEG��ʽʧ�ܣ�"));
			::DeleteFile(lpszPathName);
			return FALSE;
		}

		//�޸���ص�exif��Ϣ
		//void* handle = create_exif_handle(lpszPathName, true);

		//// ɾ������ͼ
		//delete_thumb(handle);
		//// �޸�Exif��ColorֵΪ1
		//update_color_space(handle, 1);
		//// �޸�Exif��OrientationֵΪ1
		//update_orientation(handle, 1);
		//// �����Ƿ�Ҫ�޸�DPI
		//if (nDPI != -1)
		//{
		//	update_dpi(handle, nDPI);
		//}

		//close_exif_handle(handle);
		TSINFO4CXX(_T("����JPEG��ʽ�ɹ���"));

		return TRUE;
	}
	else
	{
		if (!_fipWinImage.saveU(lpszPathName))
		{
			TSINFO4CXX(_T("�����ļ�ʧ�ܣ�"));

			::DeleteFile(lpszPathName);
			return FALSE;
		}
	}

	return TRUE;
}

CKKImageEXIF* CImageLoader::LoadImageFileOnlyExif(LPCTSTR lpszPathName)
{
	TSAUTO();
	CKKImageEXIF* pKKImageEXIF = NULL; 
	


	FREE_IMAGE_FORMAT fif = fipImage::identifyFIFU(lpszPathName);
	if (fif != FIF_JPEG && fif != FIF_RAW)
	{
		//�������JPEG��RAW��ʽ����ôֱ�ӷ���
		TSINFO4CXX("the image is not jpep and raw!");
		return pKKImageEXIF;
	}

	//����ͷ����Ϣ
	fipWinImage newImage;
	//ʹ��JPEG_CMYK��Ŀ���ǣ����jpg��JCS_CMYK, ��Ҫ��freeimage����cmyk-rgb��ת������freeimage��icc��Ϣ�м�¼һ��FIICC_COLOR_IS_CMYK�ı�־
	if (!newImage.loadU(lpszPathName, FIF_LOAD_NOPIXELS|JPEG_CMYK))
	{
		TSINFO4CXX("Load Head Failed!");
		return pKKImageEXIF;
	}

	// ��ȡJPEG�ļ���EXIF��Ϣ
	// FIMD_EXIF_MAIN��FIMD_EXIF_EXIF�����������Ҫ������ȡ��ص���Ϣ������д��
	// FIMD_EXIF_RAW�������Ϣ���ֶΡ�ExifRaw��ֻ����д���ڱ���ͼƬ��ʱ�򣬱������setMetadata�������õ������ͼƬ����ȥ
	
	pKKImageEXIF = new CKKImageEXIF();

	//�Ȼ�ȡ�����һЩExif��Ϣ������ͨ��freeimage����ȡ�ģ�
	/*void* handle = create_exif_handle(lpszPathName, false);
	if (handle != NULL)
	{
		TSINFO4CXX("create_exif_handle successful!");

		//��ȡ���Ŵ���
		wchar_t szShutterCount[20] = {0};
		get_shutter_count_from_maker_note(handle, szShutterCount, 20);
		pKKImageEXIF->SetShutterCount(szShutterCount);

		//��ȡ��ͷ��Ϣ
		wchar_t szLensModel[100] = {0};
		get_lens_model_from_maker_note(handle, szLensModel, 100);
		pKKImageEXIF->SetLensModel(szLensModel);

		//��ȡ����ģʽ
		int nExposureMode = get_exposure_program_from_makenote(handle);
		pKKImageEXIF->SetExposureProgram(nExposureMode);

		close_exif_handle(handle);
	}
	else
	{
		TSINFO4CXX("create_exif_handle failed!");
	}
	*/

	fipMetadataFind MetadataFind;
	fipTag tag;
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAIN, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			pKKImageEXIF->AddImageEXIF(strFieldName, tag);
			//TSINFO4CXX("FIMD_EXIF_MAIN:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
			//<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_EXIF, newImage, tag))
	{
		do
		{
			std::string strFieldName = tag.getKey();
			pKKImageEXIF->AddImageEXIF(strFieldName, tag);
			//TSINFO4CXX("FIMD_EXIF_EXIF:"<<" FieldName:"<<strFieldName<<", ID:"<<tag.getID()<<", Description:"<<tag.getDescription()<<", Type:"<<tag.getType()\
			//<<", Count:"<<tag.getCount()<<", Length:"<<tag.getLength()<<", Value:"<<(char*)tag.getValue());
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_MAKERNOTE, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			pKKImageEXIF->AddImageEXIF(strFieldName, tag);
		} while (MetadataFind.findNextMetadata(tag));
	}
	if (MetadataFind.findFirstMetadata(FIMD_EXIF_RAW, newImage, tag))
	{
		do 
		{
			std::string strFieldName = tag.getKey();
			pKKImageEXIF->AddImageEXIF(strFieldName, tag);


			//����exif��Ϣ����
			DWORD tag_length = tag.getLength();
			DWORD thumbnail_length = get_thumbnail_length((unsigned char*)tag.getValue(), tag_length);
			tag_length += 8;
			tag_length -= thumbnail_length;
			pKKImageEXIF->SetExifLength(tag_length);

		} while (MetadataFind.findNextMetadata(tag));
	}

	return pKKImageEXIF;
}