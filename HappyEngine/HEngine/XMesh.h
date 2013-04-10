//-------------------------------------------------------------------------------------
// ������Ʈ ���� Ŭ����(�⺻ X����)
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
	LPDIRECT3DDEVICE9       m_pDevice;	//����̽��� �Ѱܼ� ó��
    LPD3DXMESH				pMesh;		//�Ž� ��ü
	D3DMATERIAL9*			pMat;		//���׸���
	LPDIRECT3DTEXTURE9*		pTex;		//�ؽ���
	DWORD					nMat;

	XMesh(LPDIRECT3DDEVICE9 Device);	//Device�Ѱܾ���..
	~XMesh();							//�Ƹ� ����� �ؾ߰���..�Ŵ������� ���ͷ� �ڵ��Ҹ���״�..
	BOOL LoadX(char *pName);
	void Disp();						//���÷����Լ�
};



class XManager:public SceneNode{
public:
//��ӿ� ���� �Լ���
	RenderMgr* XMeshRender;
	XManager(RenderMgr* SceneRen);
	~XManager();
	virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	virtual void SetMesh();
	
	TestDummy m_dummy;				//�׽�Ʈ ���̿�(��ġ ���� ������)
	std::vector<XMesh> XMeshList;	//XMesh����Ʈ
	

	HFont* XobjectFont;				//������Ʈ�� ��Ʈ��¿�

	BOOL XLoad(char *pName);
	void DrawSpecific(HObject *obj);  //obj���� ��ľ��� ��� ��, type��, number�� ���ͼ� number�� Hmesh�� ���


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
