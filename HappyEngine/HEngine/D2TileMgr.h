//-------------------------------------------------------------------------------------
//! d3d 디바이스 생성 실습용으로 만든 테스트 노드
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif


class D2TileMgr:public SceneNode{
public:
	int TileNum;
	RenderMgr* TileRender;
	D2TileMgr(RenderMgr* SceneRen);

	virtual void draw();
	virtual void SetMesh();
	virtual void draw(ZFrustum* pFrustum);

};
