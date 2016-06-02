#include "StdAfx.h"
#include "JpegLoaderImpl.h"
#include "..\ImageHelper\exif_parser.h"
#include "..\ImageHelper\ImageUtility.h"
struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;


static void my_error_exit (j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

CJpegLoaderImpl::CJpegLoaderImpl(void)
{
	m_hBitmap = NULL;
	m_nBitmapWidth = 0;
	m_nBitmapHeight = 0;
	m_nExifInfoStatus = 0;
}

CJpegLoaderImpl::~CJpegLoaderImpl(void)
{
}

BOOL CJpegLoaderImpl::MarkerIsIcc(jpeg_saved_marker_ptr marker)
{
	// marker identifying string "ICC_PROFILE" (null-terminated)
	const BYTE icc_signature[12] = { 0x49, 0x43, 0x43, 0x5F, 0x50, 0x52, 0x4F, 0x46, 0x49, 0x4C, 0x45, 0x00 };

	if(marker->marker == ICC_MARKER) {
		// verify the identifying string
		if(marker->data_length >= ICC_HEADER_SIZE) {
			if(memcmp(icc_signature, marker->data, sizeof(icc_signature)) == 0) {
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CJpegLoaderImpl::AutoRotate()
{
	XL_BITMAP_HANDLE hNewBitmap = NULL;
	XL_BITMAP_HANDLE hTmpBitmap = NULL;
	switch (m_nOriention)
	{
	case 1:
		//�����������Ҫת��
		return FALSE;
		break;
	case 2: //ֱ��ˮƽ��ת
		hNewBitmap = CImageProcessor::ImageLeftRightMirror(m_hBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hBitmap);
			m_hBitmap = hNewBitmap;
		}
		break;
	case 3: //180����ת
		hNewBitmap = CImageProcessor::ImageRotate180(m_hBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hBitmap);
			m_hBitmap = hNewBitmap;
		}
		break;
	case 4: //ֱ�Ӵ�ֱ��ת
		hNewBitmap = CImageProcessor::ImageTopBottomMirror(m_hBitmap);
		if (hNewBitmap)
		{
			XL_ReleaseBitmap(m_hBitmap);
			m_hBitmap = hNewBitmap;
		}
		break;
	case 5:
		//��ת90�ȣ�Ȼ�����ҷ�ת
		hTmpBitmap = CImageProcessor::ImageRightRotate(m_hBitmap);
		if (hTmpBitmap)	// ��ת�ɹ�
		{
			// ���ͷŵ���������ռ�ڴ�
			XL_ReleaseBitmap(m_hBitmap);
			hNewBitmap = CImageProcessor::ImageLeftRightMirror(hTmpBitmap);
			if (hNewBitmap) // ���Ҿ���ɹ�
			{
				XL_ReleaseBitmap(hTmpBitmap);
				m_hBitmap = hNewBitmap;
			}
			else
			{
				m_hBitmap = hTmpBitmap;
			}
			// �޸� �ߴ�����
			int nTemp = m_nBitmapWidth;
			m_nBitmapWidth = m_nBitmapHeight;
			m_nBitmapHeight = nTemp;
		}
		break;
	case 6: //��ת90��
		hNewBitmap = CImageProcessor::ImageRightRotate(m_hBitmap);
		if (hNewBitmap) // ��ת�ɹ�
		{
			XL_ReleaseBitmap(m_hBitmap);
			m_hBitmap = hNewBitmap;
			// �޸� �ߴ�����
			int nTemp = m_nBitmapWidth;
			m_nBitmapWidth = m_nBitmapHeight;
			m_nBitmapHeight = nTemp;
		}
		break;
	case 7:
		//��ת90�ȣ�Ȼ�����ҷ�ת
		hTmpBitmap = CImageProcessor::ImageLeftRotate(m_hBitmap);
		if (hTmpBitmap)	// ��ת�ɹ�
		{
			// ���ͷŵ���������ռ�ڴ�
			XL_ReleaseBitmap(m_hBitmap);
			hNewBitmap = CImageProcessor::ImageLeftRightMirror(hTmpBitmap);
			if (hNewBitmap) // ���Ҿ���ɹ�
			{
				XL_ReleaseBitmap(hTmpBitmap);
				m_hBitmap = hNewBitmap;
			}
			else
			{
				m_hBitmap = hTmpBitmap;
			}
			// �޸� �ߴ�����
			int nTemp = m_nBitmapWidth;
			m_nBitmapWidth = m_nBitmapHeight;
			m_nBitmapHeight = nTemp;
		}
		break;
	case 8: //��ת90��
		hNewBitmap = CImageProcessor::ImageLeftRotate(m_hBitmap);
		if (hNewBitmap) // ��ת�ɹ�
		{
			XL_ReleaseBitmap(m_hBitmap);
			m_hBitmap = hNewBitmap;
			// �޸� �ߴ�����
			int nTemp = m_nBitmapWidth;
			m_nBitmapWidth = m_nBitmapHeight;
			m_nBitmapHeight = nTemp;
		}
		break;
	default:
		return FALSE;
		break;
	}
}

BOOL CJpegLoaderImpl::ReadIccProfile(j_decompress_ptr cinfo, JOCTET **icc_data_ptr, unsigned *icc_data_len)
{
	jpeg_saved_marker_ptr marker;
	int num_markers = 0;
	int seq_no;
	JOCTET *icc_data;
	unsigned total_length;

	const int MAX_SEQ_NO = 255;			// sufficient since marker numbers are bytes
	BYTE marker_present[MAX_SEQ_NO+1];	// 1 if marker found
	unsigned data_length[MAX_SEQ_NO+1];	// size of profile data in marker
	unsigned data_offset[MAX_SEQ_NO+1];	// offset for data in marker

	*icc_data_ptr = NULL;		// avoid confusion if FALSE return
	*icc_data_len = 0;

	/**
	this first pass over the saved markers discovers whether there are
	any ICC markers and verifies the consistency of the marker numbering.
	*/

	memset(marker_present, 0, (MAX_SEQ_NO + 1));

	for(marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
		if (MarkerIsIcc(marker)) {
			if (num_markers == 0) {
				// number of markers
				num_markers = GETJOCTET(marker->data[13]);
			}
			else if (num_markers != GETJOCTET(marker->data[13])) {
				return FALSE;		// inconsistent num_markers fields 
			}
			// sequence number
			seq_no = GETJOCTET(marker->data[12]);
			if (seq_no <= 0 || seq_no > num_markers) {
				return FALSE;		// bogus sequence number 
			}
			if (marker_present[seq_no]) {
				return FALSE;		// duplicate sequence numbers 
			}
			marker_present[seq_no] = 1;
			data_length[seq_no] = marker->data_length - ICC_HEADER_SIZE;
		}
	}

	if (num_markers == 0)
		return FALSE;

	/**
	check for missing markers, count total space needed,
	compute offset of each marker's part of the data.
	*/

	total_length = 0;
	for(seq_no = 1; seq_no <= num_markers; seq_no++) {
		if (marker_present[seq_no] == 0) {
			return FALSE;		// missing sequence number
		}
		data_offset[seq_no] = total_length;
		total_length += data_length[seq_no];
	}

	if (total_length <= 0)
		return FALSE;		// found only empty markers ?

	// allocate space for assembled data 
	icc_data = (JOCTET *) malloc(total_length * sizeof(JOCTET));
	if (icc_data == NULL)
		return FALSE;		// out of memory

	// and fill it in
	for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
		if (MarkerIsIcc(marker)) {
			JOCTET FAR *src_ptr;
			JOCTET *dst_ptr;
			unsigned length;
			seq_no = GETJOCTET(marker->data[12]);
			dst_ptr = icc_data + data_offset[seq_no];
			src_ptr = marker->data + ICC_HEADER_SIZE;
			length = data_length[seq_no];
			while (length--) {
				*dst_ptr++ = *src_ptr++;
			}
		}
	}

	*icc_data_ptr = icc_data;
	*icc_data_len = total_length;

	return TRUE;
}

BOOL  CJpegLoaderImpl::ReadExifProfile(j_decompress_ptr cinfo, JOCTET **icc_data_ptr, unsigned *icc_data_len)
{
	*icc_data_ptr = NULL;		// avoid confusion if FALSE return
	*icc_data_len = 0;
	jpeg_saved_marker_ptr marker;
	for(marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
		switch(marker->marker) {
			case EXIF_MARKER:	// Exif marker
				// �ж������Ƿ�Ϸ�
				*icc_data_ptr = marker->data;
				*icc_data_len = marker->data_length;
				return TRUE;
		}
	}
	return FALSE;
}

void CJpegLoaderImpl::GetSrcBitmapSize(int& nWidth, int& nHeight)
{
	nWidth = m_nBitmapWidth;
	nHeight = m_nBitmapHeight;
}
int CJpegLoaderImpl::LoadImage(const wstring& wstrFilePath, bool* pbStop, bool bScale, int nWidth, int nHeight)
{
	TSAUTO();
	DWORD nStartTime = GetTickCount();
	// ����һ��jpg��ѹ����
	struct jpeg_decompress_struct cinfo;
	JSAMPARRAY buffer;
	int row_stride;
	
	FILE * infile;
	if ((infile = _wfopen(wstrFilePath.c_str(), L"rb")) == NULL) {
		return 1;
	}
	
	struct my_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	
	if (setjmp(jerr.setjmp_buffer)) 
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		if (m_hBitmap)
		{
			XL_ReleaseBitmap(m_hBitmap);
			m_hBitmap = NULL;
		}
		return 2;
	}
	// ������ѹ����
	jpeg_create_decompress(&cinfo);
	// ��������Դ
	jpeg_stdio_src(&cinfo, infile);
	// ��ȡmark
	jpeg_save_markers(&cinfo, JPEG_COM, 0xFFFF);
	for(int i = 0; i < 16; i++) {
		jpeg_save_markers(&cinfo, JPEG_APP0 + i, 0xFFFF);
	}
	// ��ȡͷ����Ϣ
	(void) jpeg_read_header(&cinfo, FALSE);
	// ��ȡͼ���ԭʼ�ߴ�
	m_nBitmapWidth = cinfo.image_width;
	m_nBitmapHeight = cinfo.image_height;
	
	if (bScale)// ����������
	{
		double dRatio = 0;
		double nMaxLength = nWidth>nHeight?nWidth:nHeight;
		double nMinLength = nWidth<nHeight?nWidth:nHeight;

		double nImageMaxLength = cinfo.image_width>cinfo.image_height?cinfo.image_width:cinfo.image_height;
		double nImageMinLength = cinfo.image_width<cinfo.image_height?cinfo.image_width:cinfo.image_height;

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
			double dSscale_num = cinfo.scale_denom*dRatio;
			if (int(dSscale_num) >= dSscale_num) 
			{
				cinfo.scale_num = dSscale_num;
			}
			else
			{
				cinfo.scale_num = dSscale_num+1;
			}
		}
	}
	TSINFO4CXX("scale_num is " << cinfo.scale_num);

	// ��ʼ��ѹ����
	(void) jpeg_start_decompress(&cinfo);

	// ���Ի�ȡһЩ����ͼ�������
	row_stride = cinfo.output_width * cinfo.output_components;

	// ����XLBitmap
	m_hBitmap = XL_CreateBitmap(XLGRAPHIC_CT_ARGB32, cinfo.output_width, cinfo.output_height);
	assert(m_hBitmap);
	if (!m_hBitmap)
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return 3;
	}
	XL_ResetAlphaChannel(m_hBitmap, 255);
	BYTE* pSrcBuffer=XL_GetBitmapBuffer(m_hBitmap,0,0);
	BYTE* pNowSrcBuffer;
	int nIndex = 0;
	XLBitmapInfo info;
	XL_GetBitmapInfo(m_hBitmap, &info);
	// ��ȡ ICC profile
	BYTE *icc_profile = NULL;
	unsigned icc_length = 0;
	ReadIccProfile(&cinfo, &icc_profile, &icc_length);

	// ��ȡ Exif profile
	BYTE *exif_profile = NULL;	// ��Ҫ�ͷ�exif_profile�� ReadExifProfile������ֱ�ӷ���cinfo������
	unsigned exif_length = 0;
	ReadExifProfile(&cinfo, &exif_profile, &exif_length);
	exif_struct exifStruct;
	exifStruct.color_space = 0;
	exifStruct.oriention = 1;
	exifStruct.manufactor[0] = 0;
	bool bExifInfoVlaid = false;
	if (exif_profile)
	{
		int nRet = unencode_exif_data(exif_profile, exif_length, &exifStruct);
		if (nRet == 0)
		{
			bExifInfoVlaid = true;
		}
	}
	if (bExifInfoVlaid)
	{
		m_nOriention = exifStruct.oriention;
	}

	// ��ʼ��ȡ����
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	if( cinfo.out_color_space == JCS_CMYK ) // CMYK
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			// ��ȡ������
			if (pbStop && *pbStop)
			{
				if (icc_profile)
				{
					free(icc_profile);
				}
				jpeg_destroy_decompress(&cinfo);
				fclose(infile);
				XL_ReleaseBitmap(m_hBitmap);
				m_hBitmap = NULL;
				TSINFO4CXX("Cancel LoadFile and return -1001");
				return -1001;
			}

			// ��ȡһ������
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);
			pNowSrcBuffer = pSrcBuffer;

			JSAMPROW src = buffer[0];
			for (int i=0; i<cinfo.output_width; i++)
			{
				pNowSrcBuffer[0] = ~src[0];	// C
				pNowSrcBuffer[1] = ~src[1];	// M
				pNowSrcBuffer[2] = ~src[2];	// Y
				pNowSrcBuffer[3] = ~src[3];	// K

				src += 4;
				pNowSrcBuffer += 4;
			}
			pSrcBuffer += info.ScanLineLength;
		}
		// ��ȡ��ʾ����ICC�ļ�·��
		wstring wstrMonitorIccFilePath = CImageUtility::GetDisplayMonitorICCFilePath();
		// ��CMYKת��RGB
		CMYKtosRGB(icc_profile, icc_length, m_hBitmap, wstrMonitorIccFilePath);
	}
	else if (cinfo.output_components == 3)	// ��ͨJPG
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			// ��ȡ������
			if (pbStop && *pbStop)
			{
				if (icc_profile)
				{
					free(icc_profile);
				}
				jpeg_destroy_decompress(&cinfo);
				fclose(infile);
				XL_ReleaseBitmap(m_hBitmap);
				m_hBitmap = NULL;
				TSINFO4CXX("Cancel LoadFile and return -1001");
				return -1001;
			}

			// ��ȡһ������
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);
			pNowSrcBuffer = pSrcBuffer;

			JSAMPROW src = buffer[0];
			for (int i=0; i<cinfo.output_width; i++)
			{
				pNowSrcBuffer[0] = src[2];
				pNowSrcBuffer[1] = src[1];
				pNowSrcBuffer[2] = src[0];
				src += 3;
				pNowSrcBuffer += 4;
			}
			pSrcBuffer += info.ScanLineLength;
		}
		// ��ȡ��ʾ����ICC�ļ�·��
		wstring wstrMonitorIccFilePath = CImageUtility::GetDisplayMonitorICCFilePath();
		if (icc_profile)	//����ǶICC����ô����ת��
		{
			AdobeRGBtosRGB(icc_profile, icc_length, m_hBitmap, wstrMonitorIccFilePath);
		}
		else
		{
			sRGBtoMonitorRGB(m_hBitmap, wstrMonitorIccFilePath);
		}
		/*else	// û����ǶICC
		{	
			if (bExifInfoVlaid) // ��exif��Ϣ
			{
				string strManufactor = exifStruct.manufactor;
				if (exifStruct.color_space == 0xFFFF && !strManufactor.empty())
				{
					AdobeRGBtosRGB(icc_profile, icc_length, m_hBitmap, wstrMonitorIccFilePath);
				}
				else
				{
					sRGBtoMonitorRGB(m_hBitmap, wstrMonitorIccFilePath);
				}
			}
			else
			{
				sRGBtoMonitorRGB(m_hBitmap, wstrMonitorIccFilePath);
			}
		}*/
	}
	else	// �Ҷ�JPG
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			// ��ȡ������
			if (pbStop && *pbStop)
			{
				if (icc_profile)
				{
					free(icc_profile);
				}
				jpeg_destroy_decompress(&cinfo);
				fclose(infile);
				XL_ReleaseBitmap(m_hBitmap);
				m_hBitmap = NULL;
				TSINFO4CXX("Cancel LoadFile and return -1001");
				return -1001;
			}

			// ��ȡһ������
			(void) jpeg_read_scanlines(&cinfo, buffer, 1);
			pNowSrcBuffer = pSrcBuffer;

			JSAMPROW src = buffer[0];
			for (int i=0; i<cinfo.output_width; i++)
			{
				pNowSrcBuffer[0] = src[0];
				pNowSrcBuffer[1] = src[0];
				pNowSrcBuffer[2] = src[0];
				src += 1;
				pNowSrcBuffer += 4;
			}
			pSrcBuffer += info.ScanLineLength;
		}
	}
	// �����ICCProfile,�ͷ�֮
	if (icc_profile)
	{
		free(icc_profile);
	}
	if (exif_profile)	// �����exif��Ϣ
	{
		m_nExifInfoStatus = 1;
	}
	// ��¼�¶�ȡʱ��
	DWORD nEndTime = GetTickCount();
	TSINFO4CXX("LoadFile width="  << cinfo.output_width << " height=" << cinfo.output_height << " Consum "<< nEndTime-nStartTime);
	// ��ѹ���
	(void) jpeg_finish_decompress(&cinfo);
	// ���ٽ�ѹ����
	jpeg_destroy_decompress(&cinfo);
	// �ر��ļ�����
	fclose(infile);
	return 0;
}

