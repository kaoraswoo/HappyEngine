//-------------------------------------------------------------------------------------
// 스플래팅 관리 클래스
// Legacy 코드는 볼따구 강좌 참조
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

	// 지형픽킹 포인터 - 픽킹된 위치를 기반으로 알파맵 수정
	typedef boost::shared_ptr<HPick> HPickPtr;

	int TexNumber;									//총 몇개의 텍스쳐 불러왔는지 갯수

	BOOL	bUpdateBaseTex;							// 텍스쳐의 변경 여부
	BOOL	bUpdateSplatting[4];					// 알파텍스쳐의 변경 여부
	std::string			strSplattingTex[4];			// 스플래팅용 텍스쳐 이름
	std::string			strBaseTex;					// 베이스 텍스쳐 이름

	float m_XtileScale;
	float m_YtileScale;


	LPDIRECT3DDEVICE9		m_pd3dDevice;
	LPDIRECT3DTEXTURE9		m_pBaseTexture;			// 베이스 텍스쳐

	// 4장으로 고정파이프라인 스플래팅
	Texture					m_Texture[MAXTEXNUM];				//스프레팅텍스쳐 객체

	HSplatting(){};
	HSplatting(LPDIRECT3DDEVICE9 DevicePointer);
	~HSplatting();
	void Init();
	void SetScale(float tx, float ty); //타일 x,y 갯수를 넣어줘야 배율대로 텍스쳐를 찍는다
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
	// 알파맵 파일에서 읽기
	void LoadAlphaMap(int index,  LPCTSTR str);
	// 알파맵 일괄로드
	void LoadAlphaMaps(std::string FolderName);
	// 알파맵 일괄저장
	void SaveAlphaMaps(std::string FolderName);
	// 미니맵 렌더러
	void MiniAlphaTextureRender(int num);
	void HSplattingAlphaSet(int num,float tx, float ty, bool bDel);  //바꿀 알파텍스쳐 번호와, 마우스로 픽킹한 위치 넣어주면 해당위치 알파값 변경
	

	//기존 ZTerrain에서 썼던 스플래팅
	/*
	LPDIRECT3DTEXTURE9		m_pTex[4];	/// 텍스처  현재는 4장으로 일단 설정
    LPDIRECT3DTEXTURE9		m_pATex[4];	/// 알파맵텍스처  현재는 4장으로 일단 설정
	void HSplattingSet();   //텍스쳐모드 on, off용도로 쓸것임
	void HSplattingLoad(LPSTR lpFilename);  //텍스쳐 불러오고, 
	*/
};

#endif