#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

class BillBoard:public SceneNode{
public:
	RenderMgr* BillRender;
	BillBoard(RenderMgr* SceneRen);
	~BillBoard();

	LPDIRECT3DTEXTURE9		m_pTex;

	LPDIRECT3DVERTEXBUFFER9	m_pVB;			
	LPDIRECT3DINDEXBUFFER9	m_pIB;
	LPD3DXSPRITE  m_pd3dSprite;


    virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	void draw2(D3DXMATRIXA16* Bview);
	virtual void SetMesh();

    HRESULT InitVB();
	HRESULT InitIB();
	HRESULT InitSprite();
    void SetupMatrix();
	void UpdateMatrix();


};