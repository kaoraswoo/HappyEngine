//-------------------------------------------------------------------------------------
// ���÷��� ���� Ŭ����
// Legacy �ڵ�� ������ ���� ����
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _SPLATTING_H_
#define _SPLATTING_H_

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

#include "HPick.h"


typedef struct _Texture
{
	IDirect3DTexture9* pTexture;
	IDirect3DTexture9* pAlphamap;
	_Texture()
	{
		pTexture = NULL;
		pAlphamap = NULL;
	}

}Texture;


class HSplatting{
public:

	// ������ŷ ������ - ��ŷ�� ��ġ�� ������� ���ĸ� ����
	typedef boost::shared_ptr<HPick> HPickPtr;

	int TexNumber;									//�� ��� �ؽ��� �ҷ��Դ��� ����

	BOOL	bUpdateBaseTex;							// �ؽ����� ���� ����
	BOOL	bUpdateSplatting[4];					// �����ؽ����� ���� ����
	std::string			strSplattingTex[4];			// ���÷��ÿ� �ؽ��� �̸�
	std::string			strBaseTex;					// ���̽� �ؽ��� �̸�

	float m_XtileScale;
	float m_YtileScale;


	LPDIRECT3DDEVICE9		m_pd3dDevice;
	LPDIRECT3DTEXTURE9		m_pBaseTexture;			// ���̽� �ؽ���

	// 4������ �������������� ���÷���
	Texture					m_Texture[MAXTEXNUM];				//���������ؽ��� ��ü

	HSplatting(){};
	HSplatting(LPDIRECT3DDEVICE9 DevicePointer);
	~HSplatting();
	void Init();
	void SetScale(float tx, float ty); //Ÿ�� x,y ������ �־���� ������� �ؽ��ĸ� ��´�
	HRESULT _initTexture(void);
	

	bool CreateAlphaMaps();
	bool LoadTextures();
	void AlphaTextureRender(int index);
	void BaseTextureRender();
	//void MiniAlphaTextureRender(int index);
	const char* GetBaseTextureName();
	const char* GetTextureName(int i);
	void UpdateBaseTextureFile(LPCTSTR str);
	void UpdateTextureFile(int index, LPCTSTR str);
	void ChangeTextureFile(int index, std::string str);
	// ���ĸ� ���Ͽ��� �б�
	void LoadAlphaMap(int index,  LPCTSTR str);
	// ���ĸ� �ϰ��ε�
	void LoadAlphaMaps(std::string FolderName);
	// ���ĸ� �ϰ�����
	void SaveAlphaMaps(std::string FolderName);
	// �̴ϸ� ������
	void MiniAlphaTextureRender(int num);
	void HSplattingAlphaSet(int num,float tx, float ty, bool bDel);  //�ٲ� �����ؽ��� ��ȣ��, ���콺�� ��ŷ�� ��ġ �־��ָ� �ش���ġ ���İ� ����
	

	//���� ZTerrain���� ��� ���÷���
	/*
	LPDIRECT3DTEXTURE9		m_pTex[4];	/// �ؽ�ó  ����� 4������ �ϴ� ����
    LPDIRECT3DTEXTURE9		m_pATex[4];	/// ���ĸ��ؽ�ó  ����� 4������ �ϴ� ����
	void HSplattingSet();   //�ؽ��ĸ�� on, off�뵵�� ������
	void HSplattingLoad(LPSTR lpFilename);  //�ؽ��� �ҷ�����, 
	*/
};

#endif