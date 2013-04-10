
#include "StdAfx.h"

//------------------------------------------------------------------------------
WPFRenderer::WPFRenderer() : CRenderer()//, m_pd3dVB(NULL)
{
}


//------------------------------------------------------------------------------
WPFRenderer::~WPFRenderer()
{
//	SAFE_RELEASE(m_pd3dVB);
}


//------------------------------------------------------------------------------
HRESULT 
	WPFRenderer::Create(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter, CRenderer **ppRenderer)
{
	HRESULT hr = S_OK;

	WPFRenderer *pRenderer = new WPFRenderer();
	IFCOOM(pRenderer);

	IFC(pRenderer->Init(pD3D, pD3DEx, hwnd, uAdapter));

	*ppRenderer = pRenderer;
	pRenderer = NULL;

Cleanup:
	delete pRenderer;

	return hr;
}


//------------------------------------------------------------------------------
HRESULT 
	WPFRenderer::Init(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter)
{
	HRESULT hr = S_OK;

	// Call base to create the device and render target
	IFC(CRenderer::Init(pD3D, pD3DEx, hwnd, uAdapter));

	// Set up the global state
	IFC(m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
	IFC(m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
//	IFC(m_pd3dDevice->SetStreamSource(0, m_pd3dVB, 0, sizeof(CUSTOMVERTEX)));
	IFC(m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX));


	m_Happy = new SceneMgr();
	m_Happy->RenInstance->g_pd3dDevice = m_pd3dDevice;
	practices=new GameProgrammingClass(m_Happy->RenInstance);
	practices->SetMesh();
	practices->NodeSetPos(0.0f,0.0f,0.0f);

	BspNode=new Q3_BSP(m_Happy->RenInstance);
	BspNode->SetMesh();

	//InitCamera
	D3DXMATRIXA16 matProj;
	D3DXVECTOR3 vEyePt( 50.0f, 50.0f,-50.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_Happy->HCamera.CnodeList[0]->SetView(&vEyePt, &vLookatPt, &vUpVec);
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 400.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

Cleanup:
	return hr;
}

//------------------------------------------------------------------------------
HRESULT
	WPFRenderer::Render()
{
	HRESULT hr = S_OK;
	D3DXMATRIXA16 matWorld;

	IFC(m_pd3dDevice->BeginScene());
	IFC(m_pd3dDevice->Clear(
		0,
		NULL,
		D3DCLEAR_TARGET,
		D3DCOLOR_ARGB(128, 0, 0, 128),  // NOTE: Premultiplied alpha!
		1.0f,
		0
		));

		
		BspNode->draw();
		practices->draw();

	IFC(m_pd3dDevice->EndScene());

Cleanup:
	return hr;
}
