#include "GameProgrammingClass.h"




GameProgrammingClass::GameProgrammingClass(RenderMgr *SceneRen):SceneNode(SceneRen){
GameRender=SceneRen;
SetupMatrix();
m_name = "Cube";
}

GameProgrammingClass::~GameProgrammingClass(){
		m_pVB->Release();
	m_pIB->Release();
}


void GameProgrammingClass::draw(ZFrustum* pFrustum){
	draw();
}

void GameProgrammingClass::draw(){
		

    
        UpdateMatrix();

		GameRender->g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

		// 정점버퍼의 삼각형을 그린다.
		// 1. 정점정보가 담겨있는 정점버퍼를 출력 스트림으로 할당한다.
		GameRender->g_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
		// 2. D3D에게 정점쉐이더 정보를 지정한다. 대부분의 경우에는 FVF만 지정한다.
		GameRender->g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		// 3. 인덱스버퍼를 지정한다.
		GameRender->g_pd3dDevice->SetIndices( m_pIB );
		// 4. DrawIndexedPrimitive()를 호출한다.
		GameRender->g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12 );

    
    

}

void GameProgrammingClass::SetMesh(){
InitVB();
InitIB();

}



HRESULT GameProgrammingClass::InitVB()
{
    
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

   
    if( FAILED( GameRender->g_pd3dDevice->CreateVertexBuffer( 8*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        return E_FAIL;
    }

    // Fill the vertex buffer.
    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
    m_pVB->Unlock();

    return S_OK;
}

HRESULT GameProgrammingClass::InitIB()
{

	sTriangle indices[] =
	{
		{0, 1, 2}, {0, 2, 3},	// 윗면
		{4, 5, 6}, {4, 6, 7},	// 아래면
		{0, 3, 4}, {0, 4, 7},	// 왼
		{2, 1, 6}, {2, 6, 5},	// 오른
		{3, 2, 5}, {3, 5, 4},	// 앞
		{1, 0, 7}, {1, 7, 6}	// 뒤
	};

	// 인덱스버퍼 생성
	// D3DFMT_INDEX16은 인덱스의 단위가 16비트 라는 것이다.
	// sTriangle 구조체에서 WORD형으로 선언했으므로 D3DFMT_INDEX16을 사용한다.
	if( FAILED( GameRender->g_pd3dDevice->CreateIndexBuffer( 12 * sizeof(sTriangle), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
	{
		return E_FAIL;
	}

	// 인덱스버퍼를 값으로 채운다. 
	// 인덱스버퍼의 Lock()함수를 호출하여 포인터를 얻어온다.
	VOID* pIndices;
	if( FAILED( m_pIB->Lock( 0, sizeof(indices), (void**)&pIndices, 0 ) ) )
		return E_FAIL;
	memcpy( pIndices, indices, sizeof(indices) );
	m_pIB->Unlock();

	return S_OK;

}


void GameProgrammingClass::SetupMatrix()
{
    
    D3DXMATRIXA16 matWorld;

	

	
    UINT  iTime  = timeGetTime() % 1000;
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
    D3DXMatrixRotationY( &matWorld, fAngle );
    GameRender->g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );


    
    D3DXVECTOR3 vEyePt( 6.0f, 6.0f,-10.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    GameRender->g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

   
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 400.0f );
    GameRender->g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


void GameProgrammingClass::UpdateMatrix(){

    D3DXMATRIXA16 matWorld;
    
    UINT  iTime  = timeGetTime() % 1000;
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
    D3DXMatrixRotationY( &matWorld, fAngle );
	
	matWorld=matWorld*NodeMatrixReturn();
    GameRender->g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	

	D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 700.0f );
    GameRender->g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

}
