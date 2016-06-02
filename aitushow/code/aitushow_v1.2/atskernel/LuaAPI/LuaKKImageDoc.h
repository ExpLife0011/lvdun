#pragma once
#include "..\ImageHelper\KKImageDoc.h"


#define KKIMAGE_LUADOC_CLASSNAME "KKImage.Doc.Class"

class CLuaKKImageDoc
{
public:
	CLuaKKImageDoc(void);
	~CLuaKKImageDoc(void);

	static void RegisterLuaClass(XL_LRT_ENV_HANDLE hEnv);
	static int AddRef(lua_State* luaState);
	static int Release(lua_State* luaState);
	static int GetGifObj(lua_State* luaState);			// ��ȡGif����
	static int GetAdaptedBitmap(lua_State* luaState);	// ��ȡ������С��λͼ
	static int SetSrcBitmap(lua_State* luaState);		// ����ԭʼλͼ
	static int GetSrcBitmapSize(lua_State* luaState);	// ��ȡԭʼλͼ
	static int ReleaseSrcBitmap(lua_State* luaState);	// �ͷ�ԭʼλͼ
	static int GetSrcBitmap(lua_State* luaState);		// ��ȡͼ��ԭʼ��С��λͼ

	static int GetDocId(lua_State* luaState);			// ��ȡ�ĵ�����ID
	static int GetDocType(lua_State* luaState);			// ��ȡ�ĵ����ͣ������ж��ǵ�ͼ�ĵ����Ƕ�ͼ�ĵ�
	static int SetExifInfoStatus(lua_State* luaState);	
	static int GetExifInfoStatus(lua_State* luaState);

	static int LeftRotate(lua_State* luaState);			// ����
	static int RightRotate(lua_State* luaState);		// ����
	static int ResetRotate(lua_State* luaState);		// ������ת�Ƕ�
	static int GetRotateAngle(lua_State* luaState);
	// ���û�ȡExif��Ϣ
	static int GetFileName(lua_State* luaState);
	static int SetFileName(lua_State* luaState);
	static int GetFilePath(lua_State* luaState);
	static int SetFilePath(lua_State* luaState);
	static int GetFileType(lua_State* luaState);
	static int GetFileSize(lua_State* luaState);
	static int GetFileImageSize(lua_State* luaState);
	static int SetDateTimeOriginal(lua_State* luaState);	// ��������ʱ��
	static int GetDateTimeOriginal(lua_State* luaState);	// ��ȡ����ʱ��

	static int SetMake(lua_State* luaState);				// �����豸����
	static int GetMake(lua_State* luaState);				// ��ȡ�豸����

	static int SetModel(lua_State* luaState);				// �����豸�ͺ�
	static int GetModel(lua_State* luaState);				// ��ȡ�豸�ͺ�

	static int SetLensType(lua_State* luaState);			// ���þ�ͷ��Ϣ
	static int GetLensType(lua_State* luaState);			// ��ȡ��ͷ��Ϣ

	static int SetShutterCount(lua_State* luaState);		// ���ÿ��Ŵ���
	static int GetShutterCount(lua_State* luaState);		// ��ȡ���Ŵ���

	static int SetShutterSpeed(lua_State* luaState);		// ���ÿ����ٶ�
	static int GetShutterSpeed(lua_State* luaState);		// ��ȡ�����ٶ�

	static int SetFNumber(lua_State* luaState);				// ���ù�Ȧֵ
	static int GetFNumber(lua_State* luaState);				// ��ȡ��Ȧֵ

	static int SetFocalLength(lua_State* luaState);			// ���ý���
	static int GetFocalLength(lua_State* luaState);			// ��ȡ����

	static int SetISO(lua_State* luaState);					// ����ISO
	static int GetISO(lua_State* luaState);					// ��ȡISO

	static int SetExposureCompensation(lua_State* luaState);	// �����عⲹ��
	static int GetExposureCompensation(lua_State* luaState);	// ��ȡ�عⲹ��

	static int SetFlashStatus(lua_State* luaState);			// ���������״̬
	static int GetFlashStatus(lua_State* luaState);			// ��ȡ�����״̬

	static int SetWhiteBalance(lua_State* luaState);			// ���ð�ƽ��
	static int GetWhiteBalance(lua_State* luaState);			// ��ȡ��ƽ��

	static int SetExposureProgram(lua_State* luaState);			// ��������ģʽ
	static int GetExposureProgram(lua_State* luaState);			// ��ȡ����ģʽ

	static int SetMeteringMode(lua_State* luaState);			// ���ò��ģʽ
	static int GetMeteringMode(lua_State* luaState);			// ��ȡ���ģʽ

public:
	void SetKKImageDoc(CKKImageDoc* pKKImageDoc)	// �����ڲ��ĵ�����
	{
		m_pKKImageDoc = pKKImageDoc;
	}
	CKKImageDoc* m_pKKImageDoc;
private:
	// �������
	int m_nRef;
	int AddRef()
	{
		m_nRef++;
		return m_nRef;
	}
	int Release()
	{
		m_nRef--;
		int nTempRef = m_nRef;
		if (m_nRef == 0)
		{
			delete this;
			return 0;
		}
		return nTempRef;
	}

private:
	//
};
