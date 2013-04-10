
//+-----------------------------------------------------------------------------
//
//  CTriangleRenderer
//
//      Subclass of CRenderer that renders a single, spinning triangle
//
//------------------------------------------------------------------------------

#include "StdAfx.h"
/*
struct CUSTOMVERTEX
{
	FLOAT x, y, z; 
	DWORD color;
};


#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

struct sTriangle
{
	WORD	_0, _1, _2;		// �Ϲ������� �ε����� 16��Ʈ�� ũ�⸦ ���´�.
							// 32��Ʈ�� ũ�⵵ ���������� ���� �׷���ī�忡���� �������� �ʴ´�.
};

*/
//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer ctor
//
//------------------------------------------------------------------------------
CTriangleRenderer::CTriangleRenderer() : CRenderer(), m_pd3dVB(NULL)
{
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer dtor
//
//------------------------------------------------------------------------------
CTriangleRenderer::~CTriangleRenderer()
{
	SAFE_RELEASE(m_pd3dVB);
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer::Create
//
//  Synopsis:
//      Creates the renderer
//
//------------------------------------------------------------------------------
HRESULT 
	CTriangleRenderer::Create(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter, CRenderer **ppRenderer)
{
	HRESULT hr = S_OK;

	CTriangleRenderer *pRenderer = new CTriangleRenderer();
	IFCOOM(pRenderer);

	IFC(pRenderer->Init(pD3D, pD3DEx, hwnd, uAdapter));

	*ppRenderer = pRenderer;
	pRenderer = NULL;

Cleanup:
	delete pRenderer;

	return hr;
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer::Init
//
//  Synopsis:
//      Override of CRenderer::Init that calls base to create the device and 
//      then creates the CTriangleRenderer-specific resources
//
//------------------------------------------------------------------------------
HRESULT 
	CTriangleRenderer::Init(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter)
{
	HRESULT hr = S_OK;
	//D3DXMATRIXA16 matView, matProj;
	//D3DXVECTOR3 vEyePt(0.0f, 0.0f,-5.0f);
	//D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	//D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);

	// Call base to create the device and render target
	IFC(CRenderer::Init(pD3D, pD3DEx, hwnd, uAdapter));

	/*
	// Set up the VB
	CUSTOMVERTEX vertices[] =
	{
		{ -1.0f, -1.0f, 0.0f, 0xffff0000, }, // x, y, z, color
		{  1.0f, -1.0f, 0.0f, 0xff00ff00, },
		{  0.0f,  1.0f, 0.0f, 0xff00ffff, },
	};

	IFC(m_pd3dDevice->CreateVertexBuffer(sizeof(vertices), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &m_pd3dVB, NULL));

	void *pVertices;
	IFC(m_pd3dVB->Lock(0, sizeof(vertices), &pVertices, 0));
	memcpy(pVertices, vertices, sizeof(vertices));
	m_pd3dVB->Unlock();

	// Set up the camera
	
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	IFC(m_pd3dDevice->SetTransform(D3DTS_VIEW, &matView));
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
	IFC(m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj));
	*/

	




	// ������ ���� ���� �����Ƿ� ���� ����� ����
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// ����, ��, �������� ��� ����


	//�ﰢ�� ����
	CUSTOMVERTEX g_Vertices[] =
    {
        { -1.0f, 1.0f, 1.0f, 0xffff0000, },
        { 1.0f,  1.0f, 1.0f, 0xff0000ff, },
        {  1.0f, 1.0f, -1.0f, 0xffffffff, },
		{ -1.0f, 1.0f, -1.0f, 0xffff0000, },
        
		{ -1.0f,-1.0f, -1.0f, 0xff0000ff, },
        {  1.0f,-1.0f, -1.0f, 0xffffffff, },
		{  1.0f,-1.0f, 1.0f, 0xffff0000, },
        { -1.0f, -1.0f, 1.0f, 0xff0000ff, },
        
		
    };

   
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 8*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pd3dVB, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( m_pd3dVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
    m_pd3dVB->Unlock();

	sTriangle indices[] =
	{
		{0, 1, 2}, {0, 2, 3},	// ����
		{4, 5, 6}, {4, 6, 7},	// �Ʒ���
		{0, 3, 4}, {0, 4, 7},	// ��
		{2, 1, 6}, {2, 6, 5},	// ����
		{3, 2, 5}, {3, 5, 4},	// ��
		{1, 0, 7}, {1, 7, 6}	// ��
	};

	// �ε������� ����
	// D3DFMT_INDEX16�� �ε����� ������ 16��Ʈ ��� ���̴�.
	// sTriangle ����ü���� WORD������ ���������Ƿ� D3DFMT_INDEX16�� ����Ѵ�.
	if( FAILED( m_pd3dDevice->CreateIndexBuffer( 12 * sizeof(sTriangle), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
	{
		return E_FAIL;
	}

	// �ε������۸� ������ ä���. 
	// �ε��������� Lock()�Լ��� ȣ���Ͽ� �����͸� ���´�.
	VOID* pIndices;
	if( FAILED( m_pIB->Lock( 0, sizeof(indices), (void**)&pIndices, 0 ) ) )
		return E_FAIL;
	memcpy( pIndices, indices, sizeof(indices) );
	m_pIB->Unlock();
	D3DXMATRIXA16 matWorld;

	

	
    UINT  iTime  = timeGetTime() % 1000;
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
    D3DXMatrixRotationY( &matWorld, fAngle );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );


    
    D3DXVECTOR3 vEyePt( 6.0f, 6.0f,-10.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

   
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 400.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );


	// Set up the global state
	IFC(m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
	IFC(m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
	IFC(m_pd3dDevice->SetStreamSource(0, m_pd3dVB, 0, sizeof(CUSTOMVERTEX)));
	IFC(m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX));

	/*
	m_Happy = new SceneMgr();
	m_Happy->RenInstance->g_pd3dDevice = m_pd3dDevice;
	practices=new GameProgrammingClass(m_Happy->RenInstance);
	practices->SetMesh();
	practices->NodeSetPos(0.0f,0.0f,0.0f);

	BspNode=new Q3_BSP(m_Happy->RenInstance);
	BspNode->SetMesh();
	*/

Cleanup:
	return hr;
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CTriangleRenderer::Render
//
//  Synopsis:
//      Renders the rotating triangle
//
//------------------------------------------------------------------------------
HRESULT
	CTriangleRenderer::Render()
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
	/*
	// Set up the rotation
	UINT  iTime  = GetTickCount() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
	D3DXMatrixRotationY(&matWorld, fAngle);
	IFC(m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld));

	//IFC(m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1));


		

		m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		// ���������� �ﰢ���� �׸���.
		// 1. ���������� ����ִ� �������۸� ��� ��Ʈ������ �Ҵ��Ѵ�.
		m_pd3dDevice->SetStreamSource( 0, m_pd3dVB, 0, sizeof(CUSTOMVERTEX) );
		// 2. D3D���� �������̴� ������ �����Ѵ�. ��κ��� ��쿡�� FVF�� �����Ѵ�.
		m_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		// 3. �ε������۸� �����Ѵ�.
		m_pd3dDevice->SetIndices( m_pIB );
		// 4. DrawIndexedPrimitive()�� ȣ���Ѵ�.
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12 );
		*/


		//practices->draw();
		//BspNode->draw();
	IFC(m_pd3dDevice->EndScene());

Cleanup:
	return hr;
}
