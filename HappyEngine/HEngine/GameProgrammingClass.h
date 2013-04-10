//-------------------------------------------------------------------------------------
//! 회전하는 정육면체 테스트용 노드(노드 추가시 참조용)
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

class GameProgrammingClass:public SceneNode{
public:
	RenderMgr* GameRender;
	GameProgrammingClass(RenderMgr* SceneRen);
	~GameProgrammingClass();

	LPDIRECT3DVERTEXBUFFER9	m_pVB;			
	LPDIRECT3DINDEXBUFFER9	m_pIB;			

    virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	virtual void SetMesh();

    HRESULT InitVB();
	HRESULT InitIB();
    void SetupMatrix();
	void UpdateMatrix();


};