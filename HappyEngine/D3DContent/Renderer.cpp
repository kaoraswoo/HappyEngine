
//+-----------------------------------------------------------------------------
//
//  CRenderer
//
//      An abstract base class that creates a device and a target render 
//      surface. Derive from this class and override Init() and Render() 
//      to do your own rendering. See CTriangleRenderer for an example.


//------------------------------------------------------------------------------

#include "StdAfx.h"



//+-----------------------------------------------------------------------------
//
//  Member:
//      CRenderer ctor
//
//------------------------------------------------------------------------------
CRenderer::CRenderer() : m_pd3dDevice(NULL), m_pd3dDeviceEx(NULL), m_pd3dRTS(NULL)
{
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CRenderer dtor
//
//------------------------------------------------------------------------------
CRenderer::~CRenderer()
{
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pd3dDeviceEx);
	SAFE_RELEASE(m_pd3dRTS);
	delete m_Happy;
	delete m_pFrustum;

}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CRenderer::CheckDeviceState
//
//  Synopsis:
//      Returns the status of the device. 9Ex devices are a special case because 
//      TestCooperativeLevel() has been deprecated in 9Ex.
//
//------------------------------------------------------------------------------
HRESULT 
	CRenderer::CheckDeviceState()
{
	if (m_pd3dDeviceEx)
	{
		return m_pd3dDeviceEx->CheckDeviceState(NULL);
	}
	else if (m_pd3dDevice)
	{
		return m_pd3dDevice->TestCooperativeLevel();
	}
	else
	{
		return D3DERR_DEVICELOST;
	}
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CRenderer::CreateSurface
//
//  Synopsis:
//      Creates and sets the render target
//
//------------------------------------------------------------------------------
HRESULT
	CRenderer::CreateSurface(UINT uWidth, UINT uHeight, bool fUseAlpha, UINT m_uNumSamples)
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(m_pd3dRTS);

	IFC(m_pd3dDevice->CreateRenderTarget(
		uWidth,
		uHeight,
		fUseAlpha ? D3DFMT_A8R8G8B8 : D3DFMT_X8R8G8B8,
		static_cast<D3DMULTISAMPLE_TYPE>(m_uNumSamples),
		0,
		m_pd3dDeviceEx ? FALSE : TRUE,  // Lockable RT required for good XP perf
		&m_pd3dRTS,
		NULL
		));

	IFC(m_pd3dDevice->SetRenderTarget(0, m_pd3dRTS));

Cleanup:
	return hr;
}

//+-----------------------------------------------------------------------------
//
//  Member:
//      CRenderer::Init
//
//  Synopsis:
//      Creates the device
//
//------------------------------------------------------------------------------
HRESULT 
	CRenderer::Init(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter)
{
	HRESULT hr = S_OK;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferHeight = 1;
	d3dpp.BackBufferWidth = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	


	D3DCAPS9 caps;
	DWORD dwVertexProcessing;
	IFC(pD3D->GetDeviceCaps(uAdapter, D3DDEVTYPE_HAL, &caps));
	
	if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		dwVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		dwVertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	if (pD3DEx)
	{
		IDirect3DDevice9Ex *pd3dDevice = NULL;
		
		IFC(pD3DEx->CreateDeviceEx(
			uAdapter,
			D3DDEVTYPE_HAL,
			hwnd,
			dwVertexProcessing | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
			&d3dpp,
			NULL,
			&m_pd3dDeviceEx
			));
		
		IFC(m_pd3dDeviceEx->QueryInterface(__uuidof(IDirect3DDevice9), reinterpret_cast<void**>(&m_pd3dDevice)));  
	}
	else 
	{
	
		assert(pD3D);

		IFC(pD3D->CreateDevice(
			uAdapter,
			D3DDEVTYPE_HAL,
			hwnd,
			dwVertexProcessing | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
			&d3dpp,
			&m_pd3dDevice
			));
	}


	// Init Happy Engine

	// Set up the global state
	IFC(m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
	IFC(m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
	IFC(m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX));

	m_Happy = new SceneMgr();
	m_Happy->RenInstance->g_pd3dDevice = m_pd3dDevice;
	practices=new GameProgrammingClass(m_Happy->RenInstance);
	practices->SetMesh();
	practices->NodeSetPos(0.0f,0.0f,0.0f);

	BspNode=new Q3_BSP(m_Happy->RenInstance);
	BspNode->SetMesh();

	//TileTerrainNode = new TileTerrainMgr(m_Happy->RenInstance);
	//TileTerrainNode->SetMesh();

	//InitCamera
	//D3DXMATRIXA16 matProj;
	D3DXVECTOR3 vEyePt( -10.0f, 10.0f,-10.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_Happy->HCamera.CnodeList[0]->SetView(&vEyePt, &vLookatPt, &vUpVec);
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 1.0f, 1.0f, 400.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	D3DXMATRIXA16* pmatView = m_Happy->HCamera.CnodeList[0]->GetViewMatrix();  // 카메라 행렬을 얻는다.
	m_Happy->RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // 카메라 행렬 셋팅

	//Frustum
	m_pFrustum = new ZFrustum();

Cleanup:
	return hr;
}

//------------------------------------------------------------------------------
HRESULT 
	CRenderer::Create(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter, CRenderer **ppRenderer)
{
	HRESULT hr = S_OK;

	CRenderer *pRenderer = new CRenderer();
	IFCOOM(pRenderer);

	IFC(pRenderer->Init(pD3D, pD3DEx, hwnd, uAdapter));

	*ppRenderer = pRenderer;
	pRenderer = NULL;

Cleanup:
	delete pRenderer;

	return hr;
}



void CRenderer::SetCamera(POINT pos)
{
	const float fDelta = 0.008f; // 마우스의 민감도, 이 값이 커질수록 많이 움직인다.
	m_Happy->HCamera.CnodeList[0]->RotateLocalX( pos.y * fDelta ); // 마우스의 Y축 회전값은 3D world의  X축 회전값
	m_Happy->HCamera.CnodeList[0]->RotateLocalY( pos.x * fDelta ); // 마우스의 X축 회전값은 3D world의  Y축 회전값

	D3DXMATRIXA16* pmatView = m_Happy->HCamera.CnodeList[0]->GetViewMatrix();  // 카메라 행렬을 얻는다.
	m_Happy->RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // 카메라 행렬 셋팅
}

HRESULT	CRenderer::Render()
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

		D3DXMATRIXA16 m_matView;
		D3DXMATRIXA16	m;
		D3DXMATRIXA16	*pView;
		pView = m_Happy->HCamera.CnodeList[0]->GetViewMatrix();	// 카메라 클래스로부터 행렬정보를 얻는다.
		m = *pView * m_matProj;				// World좌표를 얻기위해서 View * Proj행렬을 계산한다.
		m_pFrustum->Make( &m );	// View*Proj행렬로 Frustum을 만든다.
		m_pFrustum->ViewSet(pView);

		BspNode->draw();
		practices->draw();
		//TileTerrainNode->draw(m_pFrustum);

	IFC(m_pd3dDevice->EndScene());
	
Cleanup:
	return hr;
}