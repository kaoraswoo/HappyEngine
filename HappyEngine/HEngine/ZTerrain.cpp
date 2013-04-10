#include "ZTerrain.h"
#include "Utility\dib.h"

/// 생성자
ZTerrain::ZTerrain(RenderMgr* SceneRen):SceneNode(SceneRen)
{
	ZTerrainRender=SceneRen;
	//HSplat = new HSplatting(ZTerrainRender->g_pd3dDevice);
	
	m_cxDIB			= 0;
	m_czDIB			= 0;
	m_cyDIB			= 0;
	m_vfScale		= D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_pvHeightMap	= NULL;
	m_pd3dDevice	= NULL;
	for( int i = 0 ; i < MAX_TERRAIN_TEX ; i++ ) m_pTex[i] = NULL;
	m_pVB			= NULL;
	m_pIB			= NULL;
	m_nTriangles	= 0;
	m_pQuadTree		= NULL;
	SplattingAnum=0;
	m_name="Legacy Terrain";
}

void ZTerrain::draw(){
}
void ZTerrain::draw(ZFrustum* pFrustum){
DrawTerrain(pFrustum);
}
void ZTerrain::SetMesh(){
    LPSTR	tex[4] = { "..\\..\\Data\\tile2.tga", "..\\..\\Data\\tile3.tga", "", "" };
	D3DXVECTOR3	vScale;
	vScale.x = vScale.z = 1.0f; vScale.y = 0.1f;
	Create( &vScale, 0.1f, "..\\..\\Data\\map129.bmp", tex );
}


/// 소멸자
ZTerrain::~ZTerrain()
{
	_Destroy();
}

/**
 * @brief 지형객체를 초기화한다.
 * @param pDev : 출력용 D3D디바이스 
 * @param pvfScale : 높이맵의 x,y,z값에 곱할 척도값(scale vector)
 * @param fLODRatio : LOD처리시에 사용될 값
 * @param lpBMPFilename : 높이맵용 BMP파일명
 * @param lpTexFilename : 출력용 D3D디바이스 
 */
HRESULT	ZTerrain::Create( D3DXVECTOR3* pvfScale, float fLODRatio, LPSTR lpBMPFilename, LPSTR lpTEXFilename[4] )
{
	//m_pd3dDevice = pDev;
	m_pd3dDevice=ZTerrainRender->g_pd3dDevice;
	m_vfScale = *pvfScale;
	m_fLODRatio = fLODRatio;
	if( FAILED( _BuildHeightMap( lpBMPFilename ) ) ) { _Destroy(); return E_FAIL; }
	if( FAILED( _LoadTextures( lpTEXFilename ) ) ) { _Destroy(); return E_FAIL; }
	if( FAILED( _CreateVIB() ) ) { _Destroy(); return E_FAIL; }
	m_pQuadTree = new ZQuadTree( m_cxDIB, m_czDIB );
	if( FAILED( _BuildQuadTree() ) ) { _Destroy(); return E_FAIL; }

	//HSplat->HSplattingLoad("tile2.tga");
	//HSplat->HSplattingAlphaSet(0,0.5f,0.5f);
	//HSplat->HSplattingAlphaSet(1,0.1f,0.1f);


	return S_OK;
}

/// 지형객체를 메모리에서 소거한다.
HRESULT	ZTerrain::_Destroy()
{
	DEL( m_pQuadTree );
	DELS( m_pvHeightMap );
	REL( m_pVB );
	REL( m_pIB );
	//delete HSplat;
	for( int i = 0 ; i < 4 ; i++ ) REL( m_pTex[i] );

	return S_OK;
}

/// 지형객체에서 사용할 텍스처를 읽어들인다.
HRESULT	ZTerrain::_LoadTextures( LPSTR lpTexFilename[4] )
{
	for( int i = 0 ; i < 4 ; i++ )
		D3DXCreateTextureFromFile( m_pd3dDevice, lpTexFilename[i], &m_pTex[i] );

	return S_OK;
}

/// BMP파일을 열어서 높이맵을 생성한다.
HRESULT	ZTerrain::_BuildHeightMap( LPSTR lpFilename )
{
	LPBYTE	pDIB = DibLoadHandle( lpFilename );
	if( !pDIB ) return E_FAIL;

	m_cxDIB = DIB_CX( pDIB );
	m_czDIB = DIB_CY( pDIB );
	
	// 여기서 m_cxDIB나 m_czDIB가 (2^n+1)이 아닌경우 E_FAIL을 반환하도록 수정할 것

	m_pvHeightMap = new TERRAINVERTEX[m_cxDIB * m_czDIB];

	TERRAINVERTEX v;
	for( int z = 0 ; z < m_czDIB ; z++ )
	{
		for( int x = 0 ; x < m_cxDIB ; x++ )
		{
			v.p.x = (float)( ( x - m_cxDIB / 2 ) * m_vfScale.x );
			v.p.z = -(float)( ( z - m_czDIB / 2 ) * m_vfScale.z );
			v.p.y = -30.0f+(float)( *( DIB_DATAXY_INV( pDIB, x, z ) ) ) * m_vfScale.y;
			D3DXVec3Normalize( &v.n, &v.p );
			v.t.x = (float)x / (float)( m_cxDIB - 1 );
			v.t.y = (float)z / (float)( m_czDIB - 1 );
			m_pvHeightMap[x + z * m_czDIB] = v;
		}
	}

	DibDeleteHandle( pDIB );
	return S_OK;
}

/// BMP파일의 크기에 맞춰서 쿼드트리를 생성한다.
HRESULT	ZTerrain::_BuildQuadTree()
{
	m_pQuadTree->Build( m_pvHeightMap );
	return S_OK;
}

/// 정점, 인덱스 버퍼를 생성한다.
HRESULT	ZTerrain::_CreateVIB()
{
	// VB생성
	if( FAILED( m_pd3dDevice->CreateVertexBuffer( m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX),
												  0, TERRAINVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
    {
        _Destroy();
		return E_FAIL;
    }
    VOID* pVertices;
    if( FAILED( m_pVB->Lock( 0, m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX), (void**)&pVertices, 0 ) ) )
	{
        _Destroy();
        return E_FAIL;
	}
	memcpy( pVertices, m_pvHeightMap, m_cxDIB*m_czDIB*sizeof(TERRAINVERTEX) );
	m_pVB->Unlock();

#ifdef _USE_INDEX16
	// IB생성
    if( FAILED( m_pd3dDevice->CreateIndexBuffer( (m_cxDIB-1)*(m_czDIB-1)*2 * sizeof(TRIINDEX), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
    {
        _Destroy();
        return E_FAIL;
    }

#else
	// IB생성
    if( FAILED( m_pd3dDevice->CreateIndexBuffer( (m_cxDIB-1)*(m_czDIB-1)*2 * sizeof(TRIINDEX), 0, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
    {
        _Destroy();
        return E_FAIL;
    }
	g_pLog->Log( "Using 32bit index buffer..." );
#endif
    return S_OK;
}

/// 화면에 지형을 출력한다.
HRESULT	ZTerrain::_Render()
{
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
  	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	D3DXMATRIXA16 matTemp;
	D3DXMatrixScaling( &matTemp, 4.0f, 4.0f, 1.0f );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTemp );
	

	m_pd3dDevice->SetTexture( 0, m_pTex[1] );								// 0번 텍스쳐 스테이지에 텍스쳐 고정(색깔맵)
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	// 0번 텍스처 스테이지의 확대 필터
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );		// 0번 텍스처 : 0번 텍스처 인덱스 사용

	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(TERRAINVERTEX) );
	m_pd3dDevice->SetFVF( TERRAINVERTEX::FVF );
	m_pd3dDevice->SetIndices( m_pIB );
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cxDIB * m_czDIB, 0, m_nTriangles );

	//여기서부터
	//HSplat->HSplattingSet();
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTemp );

	//m_pd3dDevice->SetTexture(0, HSplat->m_pATex[SplattingAnum]);
	//m_pd3dDevice->SetTexture(1, HSplat->m_pTex[0]);
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_cxDIB * m_czDIB, 0, m_nTriangles );
	
	// 멀티 텍스처 1번 설정을 사용을 중지한다.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// 알파블랜딩을 끈다.
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	//여기까지...

	//HSplat->MiniAlphaTextureRender(SplattingAnum);  //미니맵출력

	return S_OK;
}

/// 화면에 지형을 출력한다.
HRESULT	ZTerrain::DrawTerrain( ZFrustum* pFrustum )
{
	LPDWORD		pI;

    if( FAILED( m_pIB->Lock( 0, (m_cxDIB-1)*(m_czDIB-1)*2 * sizeof(TRIINDEX), (void**)&pI, 0 ) ) )
        return E_FAIL;
	m_nTriangles = m_pQuadTree->GenerateIndex( pI, m_pvHeightMap, pFrustum, m_fLODRatio );
    m_pIB->Unlock();
//	g_pLog->Log( "Triangles=%d", m_nTriangles );
	SplattingAnum=pFrustum->JCMODE1 -2;
	_Render();

	return S_OK;
}

