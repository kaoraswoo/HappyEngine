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

	TileTerrainPtr	m_pTileTerrain;		//����Ʈ��
	TileQuadTreePtr	m_pTileQuadTree;	//����Ʈ��
	HSplattingPtr	m_pHSplat;			//���÷���
	HPickPtr		m_pPick;			// ��ŷ

	int m_selectedindex;
	
    virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	virtual void SetMesh();
	void UpdateMatrix();
	
	TileTerrainMgr(RenderMgr* SceneRen);
	~TileTerrainMgr();

	void UpdatePickRay(POINT pos); // ��ŷ ����
	
	void UpdateBaseTextureFile(LPCTSTR str); // Base�ؽ��� ������ �ٲܽ�
	void UpdateTextureFile(int index, LPCTSTR str); // �ٸ� ���÷��� �ؽ��� ������ �ٲܽ�

	void DrawAlphaMap( int index, bool bDel ); //���ĸ� �����ϱ�

	//���̰���
	void UpdateHeight(float fdelta);  //���̰� �����ϱ�
	void SetHeightEditMode(int mode);
	int GetHeightEditMode();

	// �ͷ������� ���� �ε�
	void NewTerrain(float scale, float heightScale, int tile, int cell);
	void SaveTerrain(std::string str);
	void LoadTerrain(std::string str);


	//-----------���̵�(���ĸ�)---------------------------
	bool							m_bShading;		//���̴��� �ʱ׸��� ����
	LPD3DXEFFECT					mEff;			//����Ʈ
	LPDIRECT3DVERTEXDECLARATION9	mDecl;			//���̴� FVF
	void InitShader(); //���̴� ����Ʈ ����
	void BeginShader(ZFrustum* pFrustum);  //���̴��� �׸��� ����
	void EndShader(); // ���̴� �׸��� ��

};


#endif