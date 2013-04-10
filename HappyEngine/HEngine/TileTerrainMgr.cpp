//-------------------------------------------------------------------------------------
// Ÿ�Ͻ� ����Ʈ�� �Ŵ���
// ��ŷ �� �ͷ��� ���� ���� �ε� ����
// Ÿ�Ͻ����� ���� ���� ���� ��ó
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

	m_bShading = false; //���̴��� �ؽ��� ���÷��� �ʰ� ������������������ ���÷��ý� false 
	m_selectedindex=0;	//���õ� �ؽ��Ĺ�ȣ(0~3)
	
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
	
	// �⺻�ؽ��� 4�� + ���̽��� �ʱ�ȭ
	m_pHSplat->LoadTextures();
	//m_pHSplat->HSplattingAlphaSet(0,0.5f,0.5f, false);
	//m_pHSplat->HSplattingAlphaSet(1,0.9f,0.9f, false);
	//m_pHSplat->HSplattingAlphaSet(2,0.9f,0.1f, false);

	// ��ŷ
	HPickPtr pPick( new HPick(TileTerrainMgrRender->g_pd3dDevice) );
	m_pPick = pPick;

	//���̵�
	InitShader();
}

/// �Ҹ���
//---------------------------------------------------------------
TileTerrainMgr::~TileTerrainMgr()
{
}

//---------------------------------------------------------------
void TileTerrainMgr::draw()
{
	D3DXMATRIXA16 matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	
	//���̽� �ؽ��ı׸���
	m_pHSplat->BaseTextureRender(); //���̽� �ؽ��� Set
	m_pTileTerrain->DrawBaseTex(false, &matIdentity);  // Render

	//���ĸ� ������ �ؽ��ı׸���
	for( int i=0 ; i<MAXTEXNUM ; ++i)
	{
		m_pHSplat->AlphaTextureRender(i);
		m_pTileTerrain->DrawAlphaTex();
	}
}

//---------------------------------------------------------------
void TileTerrainMgr::draw(ZFrustum* pFrustum)
{

	// Ÿ���� LOD�� ũ���� �����Ѵ�.
	m_pTileTerrain->SetupLODLevel(*(pFrustum->GetPos()));

	m_pTileQuadTree->BuildDrawTiles(m_pTileTerrain, pFrustum);
	// �������������� ���÷��� �׸���
	if(!m_bShading)
	{
		draw();
	}else
	{
	// ���̴��� ���÷��� �׸���
	// ���� ���̴��� �������� wrap �ؽ��� �׸��� �ʱ⿡ ��ü��ũ��� �ؽ��ĺ���
	BeginShader( pFrustum );
		D3DXMATRIXA16 matIdentity;
		D3DXMatrixIdentity(&matIdentity);
		m_pTileTerrain->DrawShadingTex(false, &matIdentity);
	EndShader();
	}

	//�̴� ���ĸ� �׸���
	m_pHSplat->MiniAlphaTextureRender(m_selectedindex);

	//��ŷ�� �κ��� Ÿ�� �귯���׸���
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
	m_pPick->PickTestInit(pos.x, pos.y);			// ��ŷ ���� ����
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
	
	// �⺻�ؽ��� 4�� + ���̽��� �ʱ�ȭ
	m_pHSplat->LoadTextures();

}

void TileTerrainMgr::SaveTerrain(std::string str)
{
	// SplattingŬ������ �ִ� �ؽ��� ���� ���� ������ TIleTerrain�� Set�Ѵ�
	// ��? TileTerrain���� ����Ʈ ������ �Բ� �ϰ������� ���ؼ�
	m_pTileTerrain->UpdateBaseTextureFile( m_pHSplat->GetBaseTextureName() );
	for(int i=0; i< MAXTEXNUM ; i++)
	{
		m_pTileTerrain->UpdateTextureFile(i, m_pHSplat->GetTextureName(i) );
	}
	boost::filesystem::path path( str );

	m_pTileTerrain->SaveTile( path.string() );
	//���ĸ��� �ش� ���ϸ�Ʒ� ������ ������ ���ϸ��̱⿡ �ͷ������� �ȳѱ��,
	//SplattingŬ�������� �����Ѵ�
	std::string filename =  path.stem().string();
	
	boost::filesystem::create_directory(path.parent_path().string() + "\\" + filename);
	m_pHSplat->SaveAlphaMaps( path.parent_path().string() + "\\" + filename );
}

void TileTerrainMgr::LoadTerrain(std::string str)
{

	m_pTileTerrain->GetMAPINFO()->m_heightlist.clear();

	// �ͷ��� ���� �ε�
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

	//�ε�� �ͷ��� �ؽ��ĸ� SplattingŬ������ ����
	m_pHSplat->UpdateBaseTextureFile( m_pTileTerrain->GetBaseTextureName() );
	for(int i=0; i< MAXTEXNUM ; i++)
	{
		m_pHSplat->UpdateTextureFile(i, m_pTileTerrain->GetTextureName(i) );
	}
	
	//���ĸ� �ε�
	//���ĸ��� �ش� ���ϸ�Ʒ� ������ ������ ���ϸ��̱⿡ �ͷ������� �ȳѱ��,
	//SplattingŬ�������� �����Ѵ�
	std::string filename =  path.stem().string();

	//boost::filesystem::create_directory(path.parent_path().string() + "\\" + filename);
	m_pHSplat->LoadAlphaMaps( path.parent_path().string() + "\\" + filename );
	
}


//---------------------------------------------------------------
void TileTerrainMgr::InitShader()
{
	//------------------------------
	//����Ʈ ����
	LPD3DXBUFFER buff = NULL;
	if(FAILED(D3DXCreateEffectFromFile(m_pd3dDevice,".\\Data\\SimpleShader.fx", NULL, NULL, 0, NULL, &mEff, &buff)))
	{
		return;
	}
	//���̴� FVF
	D3DVERTEXELEMENT9 decl[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0,12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
		{0,24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0,32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};
	if(FAILED( m_pd3dDevice->CreateVertexDeclaration(decl, &mDecl)) ) return;
	//����Ʈ ����.
	mEff->SetTechnique("Tech");

}

void TileTerrainMgr::BeginShader(ZFrustum* pFrustum)
{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//���� ����
		//���̴����� ���� ��� ���.
		D3DXMATRIX world; D3DXMatrixIdentity(&world);	
		D3DXMATRIX view = *(pFrustum->m_matView);
		D3DXMATRIX proj = *(pFrustum->m_matProj);
		world *= view * proj;
		
		//���̴� ���� ����.
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

		//�ȼ� ���̴� FVF����.
		//m_pd3dDevice->SetStreamSource(0, mVts, 0, sizeof(CVERTEX3D_TEX2));
		m_pd3dDevice->SetVertexDeclaration(mDecl);
		//m_pd3dDevice->SetIndices(mIdx);

		//����Ʈ ����.
		mEff->Begin(NULL,0);
		mEff->BeginPass(0);
}

void TileTerrainMgr::EndShader()
{
	//����Ʈ ����.
	mEff->EndPass();
	mEff->End();
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	//���� �ѱ�
}

void TileTerrainMgr::SetHeightEditMode(int mode)
{
	m_pTileTerrain->m_nUpDownMode = mode;
}

int TileTerrainMgr::GetHeightEditMode()
{
	return m_pTileTerrain->m_nUpDownMode;
}