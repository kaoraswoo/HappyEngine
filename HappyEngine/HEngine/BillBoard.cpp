//-------------------------------------------------------------------------------------
//! ������ ����
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include "BillBoard.h"

//-------------------------------------------------------------------------------------
BillBoard::BillBoard(RenderMgr *SceneRen):SceneNode(SceneRen)
{
	BillRender=SceneRen;
	SetupMatrix();
	m_name="BillBoard";
}

//-------------------------------------------------------------------------------------
BillBoard::~BillBoard(){
		m_pVB->Release();
	m_pIB->Release();
}

//-------------------------------------------------------------------------------------
void BillBoard::draw(ZFrustum* pFrustum){
	draw2(pFrustum->m_matView); //�� ��Ʈ���� �ѱ�
}

//-------------------------------------------------------------------------------------
void BillBoard::draw2(D3DXMATRIXA16* Bview){

	 D3DXMATRIXA16 matWorld;
	 D3DXMATRIXA16 matTrans;
	 D3DXMATRIXA16 matScale;
	 D3DXMatrixIdentity(&matWorld);
	 D3DXMatrixIdentity(&matTrans);
	 D3DXMatrixIdentity(&matScale);
	 //D3DXMatrixTranslation(&matTrans,3.0f,6.0f,5.0f);
	 //�����ϸ� ����
	 D3DXMatrixScaling(&matScale,0.02f,-0.02f,0.02f);
	 D3DXMatrixMultiply(&matWorld,&matTrans,&matScale);
	 //�޾ƿ� ���Ʈ������ �������� WorldViewLH ����
	 m_pd3dSprite->SetWorldViewLH(&matWorld,Bview);

	 //D3DXSPRITE_BILLBOARD|
	 //������ ��� Begin
	 m_pd3dSprite->Begin(  D3DXSPRITE_SORT_DEPTH_BACKTOFRONT | 
		 D3DXSPRITE_SORT_DEPTH_FRONTTOBACK | D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);

		 //�����ϵ��� ����� �����Ʈ���� ����
 		 m_pd3dSprite->SetTransform(&matWorld);
		 //	D3DXVECTOR2 pos=D3DXVECTOR2(120,20);
		 //  Scale the texture 4 times in each dimension
		 //  D3DXVECTOR2 scaling(1.0f,1.0f);

		 //  out, scaling centre, scaling rotation, scaling, rotation centre, rotation, translation
		 //	D3DXMATRIX mat;
		 //	D3DXMatrixTransformation2D(&mat,NULL,0.0,&scaling,NULL,0,&pos);

		 //  Tell the sprite about the matrix
		 //  m_pd3dSprite->SetTransform(&mat);

		m_pd3dSprite->Draw(m_pTex,NULL,NULL,NULL,0xFFFFFFFF);
	 m_pd3dSprite->End();
}

//-------------------------------------------------------------------------------------
// �׽�Ʈ�� �ﰢ�� �׸���(���� �Ⱦ��δ�)
void BillBoard::draw(){
    // Begin the scene
    // Setup the world, view, and projection matrix
        UpdateMatrix();
		BillRender->g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
		// ���������� �ﰢ���� �׸���.
		// 1. ���������� ����ִ� �������۸� ��� ��Ʈ������ �Ҵ��Ѵ�.
		BillRender->g_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(CUSTOMVERTEX) );
		// 2. D3D���� �������̴� ������ �����Ѵ�. ��κ��� ��쿡�� FVF�� �����Ѵ�.
	    BillRender->g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		// 3. �ε������۸� �����Ѵ�.
		BillRender->g_pd3dDevice->SetIndices( m_pIB );
		// 4. DrawIndexedPrimitive()�� ȣ���Ѵ�.
		BillRender->g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12 );
}

//-------------------------------------------------------------------------------------
void BillBoard::SetMesh(){
	InitVB();
	InitIB();
	InitSprite();
	SetupMatrix();
}

//-------------------------------------------------------------------------------------
//������ ��������Ʈ ��ü ����
HRESULT BillBoard::InitSprite(){
	if(FAILED(D3DXCreateSprite(BillRender->g_pd3dDevice, &m_pd3dSprite)))
	{
		return E_FAIL;
	}
	D3DXIMAGE_INFO ImageInfo;
	ZeroMemory(&ImageInfo,sizeof(D3DXIMAGE_INFO));
	
	// �ʺ�� ���̸� ����Ʈ ���� ����ϸ� �ְ��� �Ͼ�Ƿ� �� �κ��� �����Ͻñ�..
	D3DXCreateTextureFromFileEx
		(
			BillRender->g_pd3dDevice,"..\\..\\Data\\Books.xml",D3DX_DEFAULT,  //�ӽ� books.xml �� ��Ʈ������
			D3DX_DEFAULT,
			1,  // �Ӹ��� �������� �ʽ��ϴ�.
			D3DPOOL_DEFAULT,
			D3DFMT_UNKNOWN,
            D3DPOOL_MANAGED,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			D3DCOLOR_ARGB(0xFF, 255, 0, 255), // �÷�Ű
            &ImageInfo,
            NULL,
            &m_pTex
		);

	
return S_OK;
}

//-------------------------------------------------------------------------------------
HRESULT BillBoard::InitVB()
{
    // Initialize three vertices for rendering a triangle
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

    // Create the vertex buffer.
    if( FAILED( BillRender->g_pd3dDevice->CreateVertexBuffer( 8*sizeof(CUSTOMVERTEX),
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

//-------------------------------------------------------------------------------------
HRESULT BillBoard::InitIB()
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
	if( FAILED( BillRender->g_pd3dDevice->CreateIndexBuffer( 12 * sizeof(sTriangle), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
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

//-------------------------------------------------------------------------------------
void BillBoard::SetupMatrix()
{
    // For our world matrix, we will just rotate the object about the y-axis.
    D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	// Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
    // every 1000 ms. To avoid the loss of precision inherent in very high 
    // floating point numbers, the system time is modulated by the rotation 
    // period before conversion to a radian angle.
    D3DXMatrixTranslation(&matWorld,3.0f,6.0f,5.0f);
    BillRender->g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );


}

//-------------------------------------------------------------------------------------
void BillBoard::UpdateMatrix()
{

    D3DXMATRIXA16 matWorld;
	D3DXMatrixTranslation(&matWorld,3.0f,6.0f,5.0f);
    BillRender->g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
    // Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
    // every 1000 ms. To avoid the loss of precision inherent in very high 
    // floating point numbers, the system time is modulated by the rotation 
    // period before conversion to a radian angle.
    //UINT  iTime  = timeGetTime() % 500;
    //FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
    //D3DXMatrixRotationY( &matWorld, fAngle );
    //BillRender->g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

}
