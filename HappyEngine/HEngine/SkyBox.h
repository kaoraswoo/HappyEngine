//-------------------------------------------------------------------------------------
// 스카이박스
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#pragma once

#ifndef _SceneNode_h_
#define _SceneNode_h_
#include "Mgr\SceneMgr.h"
#endif

#define SkyFVF (D3DFVF_XYZ|D3DFVF_TEX1)

class CSkyBox:public SceneNode
{
public:
	RenderMgr* SkyRender;


	~CSkyBox(void);

	CSkyBox(RenderMgr* SceneRen);
    virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	virtual void SetMesh();


	HRESULT	Create();
	void	Render();
	void	Destroy();

	HRESULT	LoadTexture( LPCWSTR strFileName );
	void	SetTransform( CONST D3DXMATRIX* pLocalTM ) { m_localTM = *pLocalTM; }
	void	SetEyePt ( CONST D3DXVECTOR3* pEyePt );

private:
	struct _sVertex
	{
		enum { FVF=D3DFVF_XYZ|D3DFVF_TEX1 };
		float p[3];
		float t[2];
	};

	struct _sTriangle
	{
		WORD	_0, _1, _2;		// 16비트 인덱스
	};

	LPDIRECT3DDEVICE9		m_pd3dDevice;
	IDirect3DVertexBuffer9*	m_pVB;
	IDirect3DIndexBuffer9*	m_pIB;
	LPDIRECT3DTEXTURE9		m_pTexture;
	D3DXMATRIX				m_localTM;

	HRESULT	InitVB( );
	HRESULT	InitIB( );
};
