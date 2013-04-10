//-------------------------------------------------------------------------------------
//! d3d ����̽� ���� �ǽ������� ���� �׽�Ʈ ���
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include "D2TileMgr.h"

//-------------------------------------------------------------------------------------
D2TileMgr::D2TileMgr(RenderMgr* SceneRen):SceneNode(SceneRen)
{
	TileRender=SceneRen;
	m_name = "Axis";
}

//-------------------------------------------------------------------------------------
void D2TileMgr::draw(ZFrustum* pFrustum)
{
	draw();
}

//-------------------------------------------------------------------------------------
void D2TileMgr::draw()
{
const float l = 20.0f;

	struct sVertex
	{
		enum { FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE };
		FLOAT p[3];
		DWORD color;
	};

	sVertex vtx[6] =
	{
		{ 0, 0, 0, D3DCOLOR_XRGB(255, 0, 0) },
		{ l, 0, 0, D3DCOLOR_XRGB(255, 0, 0) },
		{ 0, 0, 0, D3DCOLOR_XRGB(0, 255, 0) },
		{ 0, l, 0, D3DCOLOR_XRGB(0, 255, 0) },
		{ 0, 0, 0, D3DCOLOR_XRGB(0, 0, 255) },
		{ 0, 0, l, D3DCOLOR_XRGB(0, 0, 255) },
	};

	// 1. Object�� ��������� ����̽��� ����
	D3DXMATRIX mIdentity;
	D3DXMatrixIdentity(&mIdentity);
	TileRender->g_pd3dDevice->SetTransform( D3DTS_WORLD, &mIdentity );
	// 2. �ý��� �޸𸮿� �ִ� ���������͸� �ٷ� ������
	TileRender->g_pd3dDevice->SetFVF( sVertex::FVF );
	TileRender->g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 3, vtx, sizeof(sVertex) );


}

//-------------------------------------------------------------------------------------
void D2TileMgr::SetMesh()
{


}