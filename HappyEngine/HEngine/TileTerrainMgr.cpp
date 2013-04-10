//-------------------------------------------------------------------------------------
// 타일식 쿼드트리 매니저
// 픽킹 및 터레인 파일 저장 로드 로직
// 타일식으로 만든 강좌 참조 출처
// http://blog.naver.com/motonhj82?Redirect=Log&logNo=130032333560
// 
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include "TileTerrainMgr.h"


//--------------------------------------------------------------------------------------------
TileTerrainMgr::TileTerrainMgr(RenderMgr* SceneRen):SceneNode(SceneRen)
{
	m_name = "Tile Terrain";

	m_bShading = false; //쉐이더로 텍스쳐 스플래팅 않고 고정파이프라인으로 스플래팅시 false 
	m_selectedindex=0;	//선택된 텍스쳐번호(0~3)
	
	TileTerrainMgrRender=SceneRen;
	m_pd3dDevice = TileTerrainMgrRender->g_pd3dDevice;

	// Tile Terrain
	TileTerrainPtr pTileTerrain( new TileTerrain(SceneRen) );
	m_pTileTerrain = pTileTerrain;
	m_pTileTerrain->CreateGround( _T("..\\..\\Data\\heightmap\\map129.bmp"), 1.0f, 0.2f, 2, 32 );
	//m_pTileTerrain->TestSaveTile();

	// Tile QuadTree
	TileQuadTreePtr	pTileQuadTree( new TileQuadTree( m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount ) );
	m_pTileQuadTree = pTileQuadTree;
	m_pTileQuadTree->BuildQuadTree( m_pTileTerrain );

	// Splatting
	HSplattingPtr pHSplat( new HSplatting(TileTerrainMgrRender->g_pd3dDevice) );
	m_pHSplat = pHSplat;
	m_pHSplat->SetScale( m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount, m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount );
	
	// 기본텍스쳐 4개 + 베이스로 초기화
	m_pHSplat->LoadTextures();
	//m_pHSplat->HSplattingAlphaSet(0,0.5f,0.5f, false);
	//m_pHSplat->HSplattingAlphaSet(1,0.9f,0.9f, false);
	//m_pHSplat->HSplattingAlphaSet(2,0.9f,0.1f, false);

	// 픽킹
	HPickPtr pPick( new HPick(TileTerrainMgrRender->g_pd3dDevice) );
	m_pPick = pPick;

	//쉐이딩
	InitShader();
}

/// 소멸자
//---------------------------------------------------------------
TileTerrainMgr::~TileTerrainMgr()
{
}

//---------------------------------------------------------------
void TileTerrainMgr::draw()
{
	D3DXMATRIXA16 matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	
	//베이스 텍스쳐그리기
	m_pHSplat->BaseTextureRender(); //베이스 텍스쳐 Set
	m_pTileTerrain->DrawBaseTex(false, &matIdentity);  // Render

	//알파맵 블랜딩된 텍스쳐그리기
	for( int i=0 ; i<MAXTEXNUM ; ++i)
	{
		m_pHSplat->AlphaTextureRender(i);
		m_pTileTerrain->DrawAlphaTex();
	}
}

//---------------------------------------------------------------
void TileTerrainMgr::draw(ZFrustum* pFrustum)
{

	// 타일의 LOD및 크랙을 설정한다.
	m_pTileTerrain->SetupLODLevel(*(pFrustum->GetPos()));

	m_pTileQuadTree->BuildDrawTiles(m_pTileTerrain, pFrustum);
	// 고정파이프라인 스플래팅 그리기
	if(!m_bShading)
	{
		draw();
	}else
	{
	// 쉐이더로 스플래팅 그리기
	// 현재 쉐이더는 배율별로 wrap 텍스쳐 그리지 않기에 전체맵크기로 텍스쳐블랜딩
	BeginShader( pFrustum );
		D3DXMATRIXA16 matIdentity;
		D3DXMatrixIdentity(&matIdentity);
		m_pTileTerrain->DrawShadingTex(false, &matIdentity);
	EndShader();
	}

	//미니 알파맵 그리기
	m_pHSplat->MiniAlphaTextureRender(m_selectedindex);

	//픽킹된 부분의 타일 브러쉬그리기
	m_pTileTerrain->BrushTriangleDraw(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	m_pTileTerrain->BrushCricleDraw(50);

}
//---------------------------------------------------------------
void TileTerrainMgr::SetMesh()
{
}

//---------------------------------------------------------------
void TileTerrainMgr::UpdatePickRay(POINT pos)
{
	m_pPick->PickTestInit(pos.x, pos.y);			// 픽킹 광선 생성
	m_pTileTerrain->PickGround(m_pPick);
}

//---------------------------------------------------------------
void TileTerrainMgr::UpdateBaseTextureFile(LPCTSTR str)
{
	m_pHSplat->UpdateBaseTextureFile( str );
}

//---------------------------------------------------------------
void TileTerrainMgr::UpdateTextureFile(int index, LPCTSTR str)
{
	m_pHSplat->UpdateTextureFile(index, str);
}

//---------------------------------------------------------------
void TileTerrainMgr::DrawAlphaMap(int index, bool bDel)
{
	if(m_pPick->tx != -1 && m_pPick->ty != -1 )
		m_pHSplat->HSplattingAlphaSet(index, m_pPick->tx, m_pPick->ty, bDel);
}

//---------------------------------------------------------------
void TileTerrainMgr::UpdateHeight(float fdelta)
{
	m_pTileTerrain->EditHeightInCricle(fdelta);
}

//---------------------------------------------------------------
void TileTerrainMgr::NewTerrain(float scale, float heightScale, int tile, int cell)
{
	m_pTileTerrain->GetMAPINFO()->m_heightlist.clear();
	m_pTileTerrain->CreateGround( _T(""), scale, heightScale, tile, cell );

	// Tile QuadTree
	m_pTileQuadTree->SetInfo(m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount );
	m_pTileQuadTree->BuildQuadTree( m_pTileTerrain );

	// Splatting
	m_pHSplat->SetScale( m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount, m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount );
	
	// 기본텍스쳐 4개 + 베이스로 초기화
	m_pHSplat->LoadTextures();

}

void TileTerrainMgr::SaveTerrain(std::string str)
{
	// Splatting클래스에 있는 텍스쳐 파일 네임 정보를 TIleTerrain에 Set한다
	// 왜? TileTerrain에서 하이트 정보와 함께 일괄저장을 위해서
	m_pTileTerrain->UpdateBaseTextureFile( m_pHSplat->GetBaseTextureName() );
	for(int i=0; i< MAXTEXNUM ; i++)
	{
		m_pTileTerrain->UpdateTextureFile(i, m_pHSplat->GetTextureName(i) );
	}
	boost::filesystem::path path( str );

	m_pTileTerrain->SaveTile( path.string() );
	//알파맵은 해당 파일명아래 폴더에 동일한 파일명이기에 터레인으로 안넘기고,
	//Splatting클래스에서 저장한다
	std::string filename =  path.stem().string();
	
	boost::filesystem::create_directory(path.parent_path().string() + "\\" + filename);
	m_pHSplat->SaveAlphaMaps( path.parent_path().string() + "\\" + filename );
}

void TileTerrainMgr::LoadTerrain(std::string str)
{

	m_pTileTerrain->GetMAPINFO()->m_heightlist.clear();

	// 터레인 파일 로드
	boost::filesystem::path path( str );
	m_pTileTerrain->LoadTile( path.string() );

	
	m_pTileTerrain->CreateGround
		(
			_T(""),
			m_pTileTerrain->GetMAPINFO()->basic_scale,
			m_pTileTerrain->GetMAPINFO()->basic_heightScale,
			m_pTileTerrain->GetMAPINFO()->basic_tile,
			m_pTileTerrain->GetMAPINFO()->basic_cell
		);

	m_pTileTerrain->SetHeight();

	// Tile QuadTree
	m_pTileQuadTree->SetInfo(m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount );
	m_pTileQuadTree->BuildQuadTree( m_pTileTerrain );

	// Splatting
	m_pHSplat->SetScale( m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount, m_pTileTerrain->GetMAPINFO()->dwMapLineTileCount );

	//로드된 터레인 텍스쳐를 Splatting클래스에 세팅
	m_pHSplat->UpdateBaseTextureFile( m_pTileTerrain->GetBaseTextureName() );
	for(int i=0; i< MAXTEXNUM ; i++)
	{
		m_pHSplat->UpdateTextureFile(i, m_pTileTerrain->GetTextureName(i) );
	}
	
	//알파맵 로드
	//알파맵은 해당 파일명아래 폴더에 동일한 파일명이기에 터레인으로 안넘기고,
	//Splatting클래스에서 저장한다
	std::string filename =  path.stem().string();

	//boost::filesystem::create_directory(path.parent_path().string() + "\\" + filename);
	m_pHSplat->LoadAlphaMaps( path.parent_path().string() + "\\" + filename );
	
}


//---------------------------------------------------------------
void TileTerrainMgr::InitShader()
{
	//------------------------------
	//이펙트 생성
	LPD3DXBUFFER buff = NULL;
	if(FAILED(D3DXCreateEffectFromFile(m_pd3dDevice,".\\Data\\SimpleShader.fx", NULL, NULL, 0, NULL, &mEff, &buff)))
	{
		return;
	}
	//셰이더 FVF
	D3DVERTEXELEMENT9 decl[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0,24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0,32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};
	if(FAILED( m_pd3dDevice->CreateVertexDeclaration(decl, &mDecl)) ) return;
	//이펙트 적용.
	mEff->SetTechnique("Tech");

}

void TileTerrainMgr::BeginShader(ZFrustum* pFrustum)
{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//조명 끄기
		//셰이더에서 쓰일 행렬 계산.
		D3DXMATRIX world; D3DXMatrixIdentity(&world);	
		D3DXMATRIX view = *(pFrustum->m_matView);
		D3DXMATRIX proj = *(pFrustum->m_matProj);
		world *= view * proj;
		
		//셰이더 변수 맵핑.
		mEff->SetMatrix("worldViewProj",&world);
		mEff->SetTexture("BaseTexture",m_pHSplat->m_pBaseTexture);
		mEff->SetTexture("AlphaTex0",m_pHSplat->m_Texture[0].pAlphamap);
		mEff->SetTexture("AlphaTex1",m_pHSplat->m_Texture[1].pAlphamap);
		mEff->SetTexture("AlphaTex2",m_pHSplat->m_Texture[2].pAlphamap);
		mEff->SetTexture("SplatTex0",m_pHSplat->m_Texture[0].pTexture);
		mEff->SetTexture("SplatTex1",m_pHSplat->m_Texture[1].pTexture);
		mEff->SetTexture("SplatTex2",m_pHSplat->m_Texture[2].pTexture);

		mEff->SetFloat("TileWidth",(float)m_pTileTerrain->GetMAPINFO()->dwMapLineCellCount);
		mEff->SetFloat("TileHeight",(float)m_pTileTerrain->GetMAPINFO()->dwMapLineCellCount);

		//픽셀 쉐이더 FVF설정.
		//m_pd3dDevice->SetStreamSource(0, mVts, 0, sizeof(CVERTEX3D_TEX2));
		m_pd3dDevice->SetVertexDeclaration(mDecl);
		//m_pd3dDevice->SetIndices(mIdx);

		//이펙트 시작.
		mEff->Begin(NULL,0);
		mEff->BeginPass(0);
}

void TileTerrainMgr::EndShader()
{
	//이펙트 종료.
	mEff->EndPass();
	mEff->End();
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	//조명 켜기
}

void TileTerrainMgr::SetHeightEditMode(int mode)
{
	m_pTileTerrain->m_nUpDownMode = mode;
}

int TileTerrainMgr::GetHeightEditMode()
{
	return m_pTileTerrain->m_nUpDownMode;
}