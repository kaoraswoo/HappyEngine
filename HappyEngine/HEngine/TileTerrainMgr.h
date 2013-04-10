#ifndef _TILETERRAINMGR_H_
#define _TILETERRAINMGR_H_

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

#include "define.h"
#include "TileQuadTree.h"
#include "Splatting.h"




class TileTerrainMgr:public SceneNode
{
public:
	typedef boost::shared_ptr<TileTerrain> TileTerrainPtr;
	typedef boost::shared_ptr<TileQuadTree> TileQuadTreePtr;
	typedef boost::shared_ptr<HSplatting> HSplattingPtr;
	typedef boost::shared_ptr<HPick> HPickPtr;
	
	

	RenderMgr* TileTerrainMgrRender;
	LPDIRECT3DDEVICE9		m_pd3dDevice;

	TileTerrainPtr	m_pTileTerrain;		//하이트맵
	TileQuadTreePtr	m_pTileQuadTree;	//쿼드트리
	HSplattingPtr	m_pHSplat;			//스플래팅
	HPickPtr		m_pPick;			// 픽킹

	int m_selectedindex;
	
    virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	virtual void SetMesh();
	void UpdateMatrix();
	
	TileTerrainMgr(RenderMgr* SceneRen);
	~TileTerrainMgr();

	void UpdatePickRay(POINT pos); // 픽킹 레이
	
	void UpdateBaseTextureFile(LPCTSTR str); // Base텍스쳐 파일을 바꿀시
	void UpdateTextureFile(int index, LPCTSTR str); // 다른 스플래팅 텍스쳐 파일을 바꿀시

	void DrawAlphaMap( int index, bool bDel ); //알파맵 수정하기

	//높이값들
	void UpdateHeight(float fdelta);  //높이값 수정하기
	void SetHeightEditMode(int mode);
	int GetHeightEditMode();

	// 터레인파일 저장 로드
	void NewTerrain(float scale, float heightScale, int tile, int cell);
	void SaveTerrain(std::string str);
	void LoadTerrain(std::string str);


	//-----------쉐이딩(알파맵)---------------------------
	bool							m_bShading;		//쉐이더로 맵그릴지 여부
	LPD3DXEFFECT					mEff;			//이펙트
	LPDIRECT3DVERTEXDECLARATION9	mDecl;			//셰이더 FVF
	void InitShader(); //쉐이더 이팩트 세팅
	void BeginShader(ZFrustum* pFrustum);  //쉐이더로 그리기 시작
	void EndShader(); // 쉐이더 그리기 끝

};


#endif