//-------------------------------------------------------------------------------------
//! DirectX ��ü �� ���� �Լ�
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
	//�⺻���� ������� �ﰢ���� ���� �Ͽ���
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

	// Object ������ 1
	// 1. Object�� ��������� ����̽��� ����
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld ); // ����� ����̽��� ������ķ� ����
	// 2. ���ؽ� ���� ������
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
	// 3. �ﰢ�� ȸ��
	D3DXMATRIX matTriangle;
	INT  iTime  = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
	D3DXMatrixRotationY( &matTriangle, fAngle ); // ȸ�� ���
	D3DXMatrixTranslation( &matTriangle, 0.0f, 0.0f, 1.0f ); // ȸ�� ���
        
	matWorld=matTriangle;
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
		
}

//-------------------------------------------------------------------------------------
//����̽� �ν�Ʈ ó��
void DXApp::Reset()
{
	HRESULT hr = g_pd3dDevice->TestCooperativeLevel();

	if (hr == D3DERR_DEVICELOST ) {  // ����̽��� �Ҿ��� �����ɼ� ����
	   // �����ϱ� ���� �ʿ��� ���ҽ� ����
	   //freeVolatileResources();
	  } else if(hr == D3DERR_DEVICENOTRESET ) {     // ���� �ɼ� �ִ�
 
	   // ����̽��� �����ϰ� ���ҽ��� �����ϰ�
		// ������ ������� �簳��Ų��.
		D3DPRESENT_PARAMETERS d3dpp;
 
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = m_hWnd;
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
		d3dpp.EnableAutoDepthStencil = TRUE;    // automatically run the z-buffer for us
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;    // 16-bit pixel format for the z-buffer
    
		// D3D����̽� ����
		g_pd3dDevice->Reset(&d3dpp);
    
		// ������ ���� ���� �����Ƿ� ���� ����� ����
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
// D3D �ʱ�ȭ
int DXApp::InitDXApp(HWND hWnd){
	m_hWnd = hWnd;
    // D3D����̽� ������ ���� D3D��ü ���� 
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

    // �ø� - �ﰢ���� �޸��� ���������� ����
    //g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
    
	// ������ ���� ���� �����Ƿ� ���� ����� ����
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	return S_OK;
	
}