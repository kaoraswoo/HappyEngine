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

		// ���������� �ﰢ���� �׸���.
		// 1. ���������� ����ִ� �������۸� ��� ��Ʈ������ �Ҵ��Ѵ�.
		GameRender->g_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
		// 2. D3D���� �������̴� ������ �����Ѵ�. ��κ��� ��쿡�� FVF�� �����Ѵ�.
		GameRender->g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		// 3. �ε������۸� �����Ѵ�.
		GameRender->g_pd3dDevice->SetIndices( m_pIB );
		// 4. DrawIndexedPrimitive()�� ȣ���Ѵ�.
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
	if( FAILED( GameRender->g_pd3dDevice->CreateIndexBuffer( 12 * sizeof(sTriangle), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
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
