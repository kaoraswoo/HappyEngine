//-------------------------------------------------------------------------------------
//! DirectX 객체 및 생성 함수
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include "DXApp.h"


//-------------------------------------------------------------------------------------
DXApp::DXApp(){
	g_pd3dDevice=NULL;
	g_pD3D=NULL;
}


//-------------------------------------------------------------------------------------
void DXApp::Render(){
	//기본으로 띄웠을때 삼각형을 띄우게 하였다
    // Clear the backbuffer to a black color
	g_pd3dDevice->Clear
		( 
			0, 
			NULL, 
			D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , 
			D3DCOLOR_XRGB(97,97,97),
			1.0f, 
			0 
		);

	// Object 렌더링 1
	// 1. Object의 월드행렬을 디바이스에 설정
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld ); // 행렬을 디바이스에 월드행렬로 설정
	// 2. 버텍스 버퍼 렌더링
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
	// 3. 삼각형 회전
	D3DXMATRIX matTriangle;
	INT  iTime  = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
	D3DXMatrixRotationY( &matTriangle, fAngle ); // 회전 행렬
	D3DXMatrixTranslation( &matTriangle, 0.0f, 0.0f, 1.0f ); // 회전 행렬
        
	matWorld=matTriangle;
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
		
}

//-------------------------------------------------------------------------------------
//디바이스 로스트 처리
void DXApp::Reset()
{
	HRESULT hr = g_pd3dDevice->TestCooperativeLevel();

	if (hr == D3DERR_DEVICELOST ) {  // 디바이스를 잃었고 복구될수 없다
	   // 리셋하기 전에 필요한 리소스 제거
	   //freeVolatileResources();
	  } else if(hr == D3DERR_DEVICENOTRESET ) {     // 복구 될수 있다
 
	   // 디바이스를 리셋하고 리소스를 복구하고
		// 게임을 원래대로 재개시킨다.
		D3DPRESENT_PARAMETERS d3dpp;
 
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = m_hWnd;
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dpp.EnableAutoDepthStencil = TRUE;    // automatically run the z-buffer for us
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;    // 16-bit pixel format for the z-buffer
    
		// D3D디바이스 리셋
		g_pd3dDevice->Reset(&d3dpp);
    
		// 정점에 색깔 값이 있으므로 광원 기능을 끈다
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	  }

	/*
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = m_hWnd;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil=TRUE;
	d3dpp.AutoDepthStencilFormat=D3DFMT_D16;

	g_pd3dDevice->Reset(&d3dpp);
	*/
}


//-------------------------------------------------------------------------------------
// D3D 초기화
int DXApp::InitDXApp(HWND hWnd){
	m_hWnd = hWnd;
    // D3D디바이스 생성을 위한 D3D객체 생성 
    // Create the D3D object, which is needed to create the D3DDevice.
    // Create the D3D object.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil=TRUE;
	d3dpp.AutoDepthStencilFormat=D3DFMT_D16;

    // Create the D3DDevice
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // 컬링 - 삼각형의 뒷면을 렌더링하지 않음
    //g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    
	// 정점에 색깔 값이 있으므로 광원 기능을 끈다
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	return S_OK;
	
}