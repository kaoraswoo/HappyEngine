//-------------------------------------------------------------------------------------
// 오브젝트 관리 클래스(기본 X파일)
// 
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

#include "Test_Dummy.h"


class XMesh{
public:
	LPDIRECT3DDEVICE9       m_pDevice;	//디바이스를 넘겨서 처리
    LPD3DXMESH				pMesh;		//매쉬 객체
	D3DMATERIAL9*			pMat;		//마테리얼
	LPDIRECT3DTEXTURE9*		pTex;		//텍스쳐
	DWORD					nMat;

	XMesh(LPDIRECT3DDEVICE9 Device);	//Device넘겨야지..
	~XMesh();							//아마 릴리즈를 해야겠지..매니저에서 벡터로 자동소멸될테니..
	BOOL LoadX(char *pName);
	void Disp();						//디스플레이함수
};



class XManager:public SceneNode{
public:
//상속에 의한 함수들
	RenderMgr* XMeshRender;
	XManager(RenderMgr* SceneRen);
	~XManager();
	virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	virtual void SetMesh();
	
	TestDummy m_dummy;				//테스트 더미용(위치 정보 얻어오기)
	std::vector<XMesh> XMeshList;	//XMesh리스트
	

	HFont* XobjectFont;				//오브젝트별 폰트출력용

	BOOL XLoad(char *pName);
	void DrawSpecific(HObject *obj);  //obj에서 행렬얻어와 계산 후, type비교, number도 얻어와서 number의 Hmesh만 출력


};



inline void TransCoord(D3DXMATRIXA16 *m, D3DXVECTOR3 *x, D3DXVECTOR3 *y, D3DXVECTOR3 *z)
{
	m->_11 = x->x; m->_12 = y->x; m->_13 = z->x; m->_14 = 0.0f;
	m->_21 = x->y; m->_22 = y->y; m->_23 = z->y; m->_24 = 0.0f;
	m->_31 = x->z; m->_32 = y->z; m->_33 = z->z; m->_34 = 0.0f;
	m->_41 = m->_42 = m->_43 = 0.0f; m->_44 = 1.0f;
}

inline void ChangeCoord(D3DXMATRIXA16 *m, const D3DXVECTOR3 *front, const D3DXVECTOR3 *up)
{
	D3DXVECTOR3 cross;
	D3DXVec3Cross(&cross, up, front);
	D3DXMatrixIdentity(m);
	m->_11 = cross.x;	m->_12 = cross.y;	m->_13 = cross.z;
	m->_21 = up->x;		m->_22 = up->y;		m->_23 = up->z;
	m->_31 = front->x;	m->_32 = front->y;	m->_33 = front->z;
}
