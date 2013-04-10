#include "TileTerrain.h"
#include "Utility\dib.h"

//--------------------------------------------------------------------------------------------
/// ������
TileTerrain::TileTerrain(RenderMgr* SceneRen)
{
	TileTerrainRender=SceneRen;
	
	m_pd3dDevice=TileTerrainRender->g_pd3dDevice;

	// �� ���� ����
	m_pVertices = NULL;
	m_pIndices = NULL;
	m_pTileInfo = NULL;
	m_ppIB = NULL;
	
	/*
	// �ؽ���
	m_pBaseTexture = NULL;
	for(int i=0; i<4; ++i)
		m_ppTexSplatting[i] = NULL;
	m_pTexAlphaMap = NULL;
	
	
	// ���̽� �ؽ��ĸ� �ʱ�ȭ ���ش�.
	m_mapInfo.bUpdateBaseTex = true;
	m_mapInfo.strBaseTex.insert(0, _T("textures\\water02.jpg"));
	*/


	// ��ŷ
	m_bIsPicking = false;
	m_dPickTile = 0;
	m_vPickingPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


	// �귯��
	m_bOutBrush = true;
	m_bInBrush = true;
	m_brushOutSize = 5;
	m_brushInSize = 3;
	m_dwBrushOutColor = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	m_dwBrushInColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	m_heightRate = 1;
	m_fSlopeRate = 1.0f;
	m_nUpDownMode = 0;
	m_nTexFillMode = 0;			// 9 = ����, 0 = ����

	m_bDragBrush = false;
	m_fLineFixed = 10.0f;
	m_fLineAdd = 2.0f;

	//LOD������ 1���� �ǰ� ���� ��ų���� �ּ� Ÿ�Ϻ� ���� ���� �̻��� ������ �Ǿ�� �Ѵ�.
	//���� ������ ũ���� �Ⱥ��̰� �ϱ� ���ؼ��� ��ȭ�鿡 �ּ� 1���� LOD�� �����°� ���ڴ� 
	m_bIsLOD = true;
	m_fLODRate = 30.0f;


	// ����Ʈ��
	//m_pQuadTree = NULL;

}

//--------------------------------------------------------------------------------------------
/// �Ҹ���
TileTerrain::~TileTerrain()
{
	_Destroy();
}

//--------------------------------------------------------------------------------------------
void TileTerrain::_Destroy()
{	
	DELS(m_pVertices);
	DELS(m_pIndices);
	DELS(m_pTileInfo);


	// ���������͸� ������ �ε������� ����
	if(m_ppIB != NULL)
	{
		for(DWORD i=0; i<m_mapInfo.olddwMaxLODLevel; ++i)
		{
			for(DWORD j=0; j<5; ++j)
			{
				REL(m_ppIB[i][j]);
			}
		}
		DEL(m_ppIB);
	}
}

//--------------------------------------------------------------------------------------------
void TileTerrain::SaveTile(std::string filepath)
{
	DWORD cnt = 0;
	for(DWORD j=0; j<m_mapInfo.dwMapLineWidth; ++j)
	{
		for(DWORD i=0; i<m_mapInfo.dwMapLineWidth; ++i)
		{
			float c = m_pVertices[cnt].p.y;
			m_mapInfo.m_heightlist.push_back(c);
			cnt++;
		}
	}


	std::ofstream ofs( filepath );
    {
        boost::archive::text_oarchive oa(ofs);
        oa << m_mapInfo;
    }

}

//--------------------------------------------------------------------------------------------
void TileTerrain::LoadTile(std::string filepath)
{
	std::ifstream in(filepath); //�б� ��Ʈ�� ����
	boost::archive::text_iarchive ia(in); //�� ��Ʈ���� �Ѱ��־ ����ȭ��ü �ʱ�ȭ
	ia>>m_mapInfo; //�б�

	// ���� ���� ������ ���̸ʿ� �ٽ� �����ϱ� (�����ʿ�)
	// mapinfo ������ �ø���������̼� �ϴ°����� �����ؾ��Ѵ�..
	// ���嵵 ����� �ӽ÷� ���̰��� ��������
	

}

void TileTerrain::SetHeight()
{
	for(DWORD cnt=0; cnt<m_mapInfo.m_heightlist.size(); ++cnt)
	{
			m_pVertices[cnt].p.y = m_mapInfo.m_heightlist[cnt];
	}
}


//--------------------------------------------------------------------------------------------
// �� ���� ���( _BuildHeightMap �� ���ο��� �ҷ���)
void TileTerrain::_SetupMapInfo(int cell, int tile, float scale, float heightScale)
{
	m_mapInfo.basic_scale = scale;
	m_mapInfo.basic_heightScale = heightScale;
	m_mapInfo.basic_tile = tile;
	m_mapInfo.basic_cell = cell;


	m_mapInfo.fCellScale = scale;
	m_mapInfo.fHeightScale = heightScale;
	m_mapInfo.dwTileLineCellCount = cell;
	m_mapInfo.dwMapLineTileCount = tile;
	m_mapInfo.dwMapTotalTileCount = tile * tile;
	m_mapInfo.dwMapLineCellCount = cell * tile;
	m_mapInfo.dwTileLineWidth = cell + 1;
	m_mapInfo.dwMapLineWidth = m_mapInfo.dwMapLineCellCount + 1;
	m_mapInfo.dwTileTotalVert = (cell+1)*(cell+1);
	m_mapInfo.dwTileIndxCnt = cell * cell * 2;
	m_mapInfo.dwMapTotalVert = m_mapInfo.dwMapLineWidth * m_mapInfo.dwMapLineWidth;

	// ���۰� ����
	m_mapInfo.fStartX = -(float)(m_mapInfo.dwMapLineWidth / 2.0f) * scale;
	m_mapInfo.fStartZ =  (float)(m_mapInfo.dwMapLineWidth / 2.0f) * scale;		// Z���� �ݴ�
	m_mapInfo.fEndX	  =  (float)(m_mapInfo.dwMapLineWidth / 2.0f) * scale;
	m_mapInfo.fEndZ   = -(float)(m_mapInfo.dwMapLineWidth / 2.0f) * scale;
	m_mapInfo.fCenterX = (m_mapInfo.fStartX + m_mapInfo.fEndX)/2;
	m_mapInfo.fCenterZ = (m_mapInfo.fStartZ + m_mapInfo.fEndZ)/2;

	//�ִ� LOD�ܰ� ����
	WORD cnt = 1;				// 0Level ���� 1�ܰ��̱� ������
	int divideCell = cell;
	while( divideCell > 2)				// �� LODLevel�� 1/2�� ��� ��ŭ�� ���� �ʿ��ϴ�.
	{
		divideCell /= 2;
		++cnt;
	}
	m_mapInfo.dwMaxLODLevel = cnt;
	m_mapInfo.olddwMaxLODLevel = cnt;

	// [MaxLOD][5]�� �� �ﰢ��(�ε���)�� ���� ���� ���� ����
	m_mapInfo.pTriCnt = new WORD[cnt][5];
}


//--------------------------------------------------------------------------------------------
void TileTerrain::_ResetGround(void)
{
	DELS(m_pVertices);
	DELS(m_pIndices);
	DELS(m_pTileInfo);

	// ���������͸� ������ �ε������� ����
	if(m_ppIB != NULL)
	{
		for(DWORD i=0; i<m_mapInfo.olddwMaxLODLevel; ++i)
		{
			for(DWORD j=0; j<5; ++j)
			{
				if(m_ppIB[i][j] != NULL)
				REL(m_ppIB[i][j]);
			}
		}
		DELS(m_ppIB);
	}

	// Ÿ�� ������ ����.
	m_vecVisibleTile.clear();
	m_vecTexDrawTile.clear();
}

//--------------------------------------------------------------------------------------------
// ���������(strHeight ��ΰ� ��������� ������ ����(BuildHeightMapȣ��) / ��� �������� BuildBMPHeightMapȣ��)
HRESULT TileTerrain::CreateGround(LPCTSTR strHeight, float scale, float heightScale, int tile, int cell)
{

	HRESULT hr;
	D3DXMatrixIdentity(&m_matWorld);

	// ���� ���۸� ����.
	_ResetGround();

	// ���̸��� ����Ѵ�.
	m_mapInfo.strHeightMap.clear();
	m_mapInfo.strHeightMap.insert(0, strHeight);

	

	// ��ü �������� ���� 
	if( m_mapInfo.strHeightMap.empty() || m_mapInfo.strHeightMap == TEXT("")  )
	{
		hr = _BuildHeightMap(cell, tile, scale);  // ���̸��� ���ٸ� �׳� ���� ����
		if(FAILED(hr))
		{
			MessageBox(NULL, "_BuildHeightMap failed", "", MB_OK);
		}
	}
	else
	{
		hr = _BuildBMPHeightMap(m_mapInfo.strHeightMap.c_str(), cell, scale, heightScale);	// ���̸� ����
		if(FAILED(hr))
		{
			MessageBox(NULL, "_BuildBMPHeightMap failed", "", MB_OK);
		}
	}

	if( m_mapInfo.m_heightlist.size()>0 )
	{
		for(DWORD cnt=0; cnt<m_mapInfo.m_heightlist.size(); ++cnt)
		{
				m_pVertices[cnt].p.y = m_mapInfo.m_heightlist[cnt];
		}
	}

	// TILEINFO�� Ÿ�� �� ��ŭ �����Ѵ�.
	m_pTileInfo = new TILEINFO[m_mapInfo.dwMapTotalTileCount];

	// �� Ÿ�Ϻ� �������� ����
	for(DWORD i=0; i<m_mapInfo.dwMapTotalTileCount; i++)
	{
		if( FAILED(_CreateVB(i, m_pTileInfo)) )
			return E_FAIL;
	}

	// LODLevel �ε������۸� ����
	m_ppIB = new LPDIRECT3DINDEXBUFFER9[m_mapInfo.dwMaxLODLevel][5];
	for(DWORD i=0; i<m_mapInfo.dwMaxLODLevel; i++)
	{
		if( FAILED(_CreateIB(i)) )
			return E_FAIL;
	}

	
	/*
	if( FAILED(_initTexture()) )
	{
		return E_FAIL;
	}
	*/
	/*
	if(	m_pQuadTree != NULL)		// ������ ���� Ʈ���� ���� �ϰ� �ٽ� �����Ѵ�.
		DEL(m_pQuadTree);
	m_pQuadTree = new TileQuadTree(m_mapInfo.dwMapLineTileCount);
	m_pQuadTree->BuildQuadTree( this );
	*/
	
	return S_OK;
}




//--------------------------------------------------------------------------------------------
// ��ü ���� ������ ������ �����迭�� �����.(y���� 0.0��)
HRESULT TileTerrain::_BuildHeightMap(int cell, int tile, float scale)
{
	_SetupMapInfo(cell, tile, scale);			// �� ���� ���
	if(m_pVertices != NULL)
	{
		delete[] m_pVertices;
		m_pVertices = NULL;
	}
	
		m_pVertices = new TILEVERTEX[m_mapInfo.dwMapTotalVert];
	
	TILEVERTEX v;
	DWORD dwHeight = m_mapInfo.dwMapLineWidth;
	DWORD cnt=0;
	for( DWORD z = 0 ; z < dwHeight ; ++z )
	{
		for( DWORD x = 0 ; x < dwHeight ; ++x )
		{
			v.p.x = (float)(x-dwHeight/2.0f)*m_mapInfo.fCellScale;		// ������ x��ǥ(�޽ø� ������ ����)
			v.p.z = -((float)(z-dwHeight/2.0f)*m_mapInfo.fCellScale);	// z���� ����;����̹Ƿ� -�� ���Ѵ�
			v.p.y = 0.0f;
			v.n.x = v.p.x;
			v.n.y = v.p.y;
			v.n.z = v.p.z;
			D3DXVec3Normalize( &v.n, &v.n );
			v.t.x = (float)x / (m_mapInfo.dwMapLineCellCount);
			v.t.y = (float)z / (m_mapInfo.dwMapLineCellCount);
			v.t1.x = 0.0f;
			v.t1.y = 0.0f;
			m_pVertices[cnt++] = v;				// ���ؽ� �� ����
		}
	}
	// ���� ���� ���
	_ComputeNormal(0, dwHeight, 0, dwHeight);
	return S_OK;
}

//--------------------------------------------------------------------------------------------
// BMP ���̸��� �ε��ؼ� ������ �����Ѵ�.
// BMP ���� ���� ���� 2^n+1 ũ�⸦ �����ؾ��Ѵ�.
// ��, ���� 32ĭ���� ����
// �� �ʺ�-1 / 32ĭ = Ÿ�� ĭ�̴�.
HRESULT TileTerrain::_BuildBMPHeightMap(LPCTSTR pfileName, int cell, float scale, float heightScale)
{
	//TestLoadTile();
	char str[120];
	strcpy(str, pfileName); 
	LPBYTE pDIB = DibLoadHandle((LPSTR)str);
	/////////////////////////////////////////////////////////////////////
	if(!pDIB) return E_FAIL;
	DWORD cxDIB = DIB_CX(pDIB);					// ��ü ����(����)�� ���� ũ�Ⱚ 2^n+1�̴�.
	DWORD tile = (cxDIB-1)/cell;				// 2^n���� ��ȯ�� ��ũ��� ������ Ÿ�� ���� ���´�.
	_SetupMapInfo(cell, tile, scale, heightScale);			// ������ ���	

	if(m_pVertices == NULL)
	{
		m_pVertices = new TILEVERTEX[m_mapInfo.dwMapTotalVert];
	}
	TILEVERTEX v;
	DWORD dwHeight = m_mapInfo.dwMapLineWidth;
	DWORD cnt=0;
	for( DWORD z = 0; z < dwHeight; ++z )
	{
		for( DWORD x = 0; x < dwHeight; ++x )
		{
			v.p.x = (float)(x-dwHeight/2.0f)*m_mapInfo.fCellScale;		// ������ x��ǥ(�޽ø� ������ ����)
			v.p.z = -((float)(z-dwHeight/2.0f)*m_mapInfo.fCellScale);	// z���� ����;����̹Ƿ� -�� ���Ѵ�
			v.p.y = (float)(*(DIB_DATAXY_INV(pDIB, x, z)))*m_mapInfo.fCellScale*heightScale;
			//v.p.y = m_mapInfo.m_heightlist[cnt];
			v.n.x = v.p.x;
			v.n.y = v.p.y;
			v.n.z = v.p.z;
			D3DXVec3Normalize( &v.n, &v.n );
			v.t.x = (float)x / (m_mapInfo.dwMapLineCellCount);
			v.t.y = (float)z / (m_mapInfo.dwMapLineCellCount);
			v.t1.x = 0.0f;
			v.t1.y = 0.0f;
			m_pVertices[cnt++] = v;				// ���ؽ� �� ����
		}
	}
	DibDeleteHandle(pDIB);
	// ���� ���� ���
	_ComputeNormal(0, dwHeight, 0, dwHeight);
	return S_OK;
}


//--------------------------------------------------------------------------------------------
// �� index��° Ÿ�� ������ �����Ѵ�.
// Ÿ�� ������ ������Ʈ �Ҷ��� ����.
HRESULT TileTerrain::_CreateVB(int index, TILEINFO *pTileInfo)
{
	// �ּ� �����ø�
	if(pTileInfo[index].pVertices != NULL)
	{
		delete[] pTileInfo[index].pVertices;
	}
		pTileInfo[index].pVertices = new TILEVERTEX[m_mapInfo.dwTileTotalVert];
	
	// ��ü ���ؽ��� ���� �� Ÿ�Ϻ� �������� ����Ѵ�.
	// ���� = (Ÿ�Ϲ�ȣ % ����Ÿ�ϼ�) * ��Ÿ���� ���� �� ��
	DWORD offsetX = (DWORD)(index % m_mapInfo.dwMapLineTileCount) * m_mapInfo.dwTileLineCellCount;
	// ���� = (Ÿ�Ϲ�ȣ / ����Ÿ�ϼ�) * ��Ÿ���� ���� ���� * ��ü ���� ���ؽ� ��
	DWORD offsetZ = (DWORD)(index / m_mapInfo.dwMapLineTileCount) * m_mapInfo.dwTileLineCellCount * m_mapInfo.dwMapLineWidth;

	int i=0;
	for(DWORD z=0; z<m_mapInfo.dwTileLineWidth; ++z)
	{
		for(DWORD x=0; x<m_mapInfo.dwTileLineWidth; ++x)
		{
			pTileInfo[index].pVertices[i] =	m_pVertices[z*m_mapInfo.dwMapLineWidth + x + offsetX + offsetZ];
			// ���÷��� �ؽ��Ĵ� �� Ÿ�� ���� ������ �ش�.
			pTileInfo[index].pVertices[i].t1.x = (float)x / (float)m_mapInfo.dwTileLineCellCount;
			pTileInfo[index].pVertices[i].t1.y = (float)z / (float)m_mapInfo.dwTileLineCellCount;
			++i;
		}
	}
	// ó������ ����
	if( pTileInfo[index].pVB == NULL )
	{
		if(FAILED(m_pd3dDevice->CreateVertexBuffer( m_mapInfo.dwTileTotalVert*sizeof(TILEVERTEX),
													  0, D3DFVF_TILEVERTEX,
													  D3DPOOL_DEFAULT, &pTileInfo[index].pVB, NULL ) ) )
		{
			return E_FAIL;
		}
	}
	TILEVERTEX *pVB;
	if(FAILED(pTileInfo[index].pVB->Lock(0, m_mapInfo.dwTileTotalVert*sizeof(TILEVERTEX), (void**)&pVB, 0)))
		return E_FAIL;
	memcpy(pVB, pTileInfo[index].pVertices, m_mapInfo.dwTileTotalVert*sizeof(TILEVERTEX));
	pTileInfo[index].pVB->Unlock();

	// TILEINFO�� ���� ������ �����Ѵ�.
	pTileInfo[index].nLODLevel = 0;			// �ʱ� ������ 0���� �����Ѵ�.
	pTileInfo[index].nCrackLevel = 0;
	// ��ü �������� �迭�� �ε��� �� Ÿ���� �𼭸�, ���� �ε��� ���� �����Ѵ�.
	pTileInfo[index].dwEDGE[0] = offsetX + offsetZ;			// TL
	pTileInfo[index].dwEDGE[1] = offsetX + offsetZ + m_mapInfo.dwTileLineCellCount;			// TR
	pTileInfo[index].dwEDGE[2] = offsetX + offsetZ + m_mapInfo.dwTileLineCellCount*m_mapInfo.dwMapLineWidth;			// BL
	pTileInfo[index].dwEDGE[3] = offsetX + offsetZ + m_mapInfo.dwTileLineCellCount*m_mapInfo.dwMapLineWidth 
																+ m_mapInfo.dwTileLineCellCount;// BR
	pTileInfo[index].dwCenter = (pTileInfo[index].dwEDGE[0] + pTileInfo[index].dwEDGE[1] + pTileInfo[index].dwEDGE[2]+
										pTileInfo[index].dwEDGE[3])/4;
	pTileInfo[index].dwTriCount = m_mapInfo.dwTileIndxCnt;
	return S_OK;
}

//--------------------------------------------------------------------------------------------
HRESULT TileTerrain::_CreateIB(int LODLevel )
{
	// �� ���� �� ũ������ �ʿ��� �ε��� ������ �ٸ��� ������ �ε��� ������ Ȯ���ؾ��Ѵ�.
	// LOD������ ���� �� ������ ���� ���� ���� ���� ����(����)���� 1/2��, �� ���� ���� 1/4�谡 �ȴ�.
	
	int divideOffset = (int)pow((double)4, LODLevel);			// ���� �� ���� ���� ����
	int nextOffset = (int)pow((double)2, LODLevel);				// ���� �������� ���� ��.
	int totalCell = (m_mapInfo.dwTileLineCellCount *  m_mapInfo.dwTileLineCellCount) / divideOffset;		// �� ����
	int halfOffset = nextOffset / 2;				// ���� ���� �߾����� ���ϱ� ����

	int crackIndex=0;
	for( crackIndex=0; crackIndex<5; ++crackIndex )
	{
		// 0��° ������ ��� ũ�� �ε��� ���۰� �ʿ�����Ƿ� �н�
		if( LODLevel == 0 && crackIndex > 0 )
		{
			m_ppIB[LODLevel][crackIndex] = NULL;
			continue;
		}

		// ���� LODLevel�̶�, 0�� ũ���� �⺻�� �� ����, 1~4�� ũ������ ���� ���ε鿡 ũ�� �ε����� �߰��ȴ�.
		// ũ�� �ε����� ���� ���� 3���� �ﰢ���� �� �ʿ� �ϹǷ� 2+3 = 5���� �ȴ�.
		// ũ�� �� ���� ���θ� ũ���ε����� �߰� �ȴ�. MYINDEXDW����ü�� 3���� �ε����� ����
		int totalIndex = totalCell * 2;		// ���� 2���� �ﰢ��
		if( crackIndex > 0)
		{
			totalIndex += (m_mapInfo.dwTileLineCellCount / nextOffset) * 3;
		}
		m_mapInfo.pTriCnt[LODLevel][crackIndex] = totalIndex;
		
		
		// �ε����� �����Ѵ�. ( D3DFMT_INDEX16 �� �����Ѵ�)
		//D3DFMT_INDEX16�� �ε����� ������ 16��Ʈ ��� ���̴�.
		// sTriangle ����ü���� WORD������ ���������Ƿ� D3DFMT_INDEX16�� �����
		if( FAILED( m_pd3dDevice->CreateIndexBuffer( totalIndex * sizeof(TRIINDEX),
					0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ppIB[LODLevel][crackIndex], NULL ) ) )
		{ return E_FAIL; }

		TRIINDEX *pI;
		if( FAILED( m_ppIB[LODLevel][crackIndex]->Lock( 0, totalIndex * sizeof(TRIINDEX), (void**)&pI, NULL ) ) )
		{ return E_FAIL; }

		TRIINDEX i;
		int cnt=0;
		int temp = m_mapInfo.dwTileLineCellCount - nextOffset;
		int half = nextOffset/2;
		for(WORD z=0; z<m_mapInfo.dwTileLineCellCount; z+=nextOffset )
		{
			for(WORD x=0; x<m_mapInfo.dwTileLineCellCount; x+=nextOffset )
			{
				// 0--1--2
				// 3--4--5
				// 6--7--8
				DWORD _0 = 	    z * m_mapInfo.dwTileLineWidth + x;		// 0
				DWORD _1 = 	    z * m_mapInfo.dwTileLineWidth + x+halfOffset;	// 1
				DWORD _2 = 	    z * m_mapInfo.dwTileLineWidth + x+nextOffset;	// 2
				DWORD _3 = (z+halfOffset) * m_mapInfo.dwTileLineWidth + x;		// 3
				DWORD _4 = (z+halfOffset) * m_mapInfo.dwTileLineWidth + x+halfOffset;	// 4
				DWORD _5 = (z+halfOffset) * m_mapInfo.dwTileLineWidth + x+nextOffset;	// 5
				DWORD _6 = (z+nextOffset) * m_mapInfo.dwTileLineWidth + x;		// 6
				DWORD _7 = (z+nextOffset) * m_mapInfo.dwTileLineWidth + x+halfOffset;	// 7
				DWORD _8 = (z+nextOffset) * m_mapInfo.dwTileLineWidth + x+nextOffset;	// 8
				
				BOOL bCheck = false;
				switch( crackIndex )
				{
				case LOD_BASIC:
					bCheck = true;
					break;

			// LEFT
				case LOD_LEFT:
					if( x == 0 )
					{
						i._0 =	_4;		i._1 = _0;		i._2 = _2;  pI[cnt++] = i;	// 4, 0, 2
						i._0 = _4;		i._1 = _2;		i._2 = _8;	pI[cnt++] = i;// 4, 2, 8
						i._0 = _4;		i._1 = _8;		i._2 = _6;	pI[cnt++] = i;// 4, 8, 6
						i._0 = _4;		i._1 = _6;		i._2 = _3;	pI[cnt++] = i;// 4, 6, 3
						i._0 = _4;		i._1 = _3;		i._2 = _0;	pI[cnt++] = i;// 4, 3, 0
						bCheck = false;
					}
					else{ bCheck = true; }
					break;

			// TOP
				case LOD_TOP:
					if( z == 0 )
					{
						i._0 =	_4;		i._1 = _0;		i._2 = _1;	pI[cnt++] = i;// 4, 0, 1
						i._0 =	_4;		i._1 = _1;		i._2 = _2;	pI[cnt++] = i;// 4, 1, 2
						i._0 =	_4;		i._1 = _2;		i._2 = _8;	pI[cnt++] = i;// 4, 2, 8
						 i._0=	_4;		i._1 = _8;		i._2 = _6;	pI[cnt++] = i;// 4, 8, 6
						i._0 =	_4;		i._1 = _6;		i._2 = _0;	pI[cnt++] = i;// 4, 6, 0
						bCheck = false;
					}
					else{ bCheck = true; }
					break;

			// RIGHT
				case LOD_RIGHT:
					if( x == temp )
					{
						i._0 =	_4;		i._1 = _0;		i._2 = _2;	pI[cnt++] = i;// 4, 0, 2
						i._0 =	_4;		i._1 = _2;		i._2 = _5;	pI[cnt++] = i;// 4, 2, 5
						i._0 =	_4;		i._1 = _5;		i._2 = _8;	pI[cnt++] = i;// 4, 5, 8
						i._0 =	_4;		i._1 = _8;		i._2 = _6;	pI[cnt++] = i;// 4, 8, 6
						i._0 =	_4;		i._1 = _6;		i._2 = _0;	pI[cnt++] = i;// 4, 6, 0
						bCheck = false;
					}
					else{ bCheck = true; }
					break;			

			// BOTTOM
				case LOD_BOTTOM:
					if( z == temp )
					{
						i._0 =	_4;		i._1 = _0;		i._2 = _2;	pI[cnt++] = i;// 4, 0, 2
						i._0 =	_4;		i._1 = _2;		i._2 = _8;	pI[cnt++] = i;// 4, 2, 8
						i._0 =	_4;		i._1 = _8;		i._2 = _7;	pI[cnt++] = i;// 4, 8, 7
						i._0 =	_4;		i._1 = _7;		i._2 = _6;	pI[cnt++] = i;// 4, 7, 6
						i._0 =	_4;		i._1 = _6;		i._2 = _0;	pI[cnt++] = i;// 4, 6, 0
						bCheck = false;
					}
					else{ bCheck = true; }
					break;

				} // end case

				if( bCheck )
				{
					i._0 =	_0;		i._1 = _2;		i._2 = _6;	pI[cnt++] = i;// 0, 2, 6
					i._0 =	_6;		i._1 = _2;		i._2 = _8;	pI[cnt++] = i;// 6, 2, 8
				}
			} 
		}	// end for
		m_ppIB[LODLevel][crackIndex]->Unlock();			
	} // end_of_crack

	return S_OK;
}



// base�ؽ��� ����
void TileTerrain::UpdateBaseTextureFile(LPCTSTR str)
{
	m_mapInfo.strBaseTex.clear();
	m_mapInfo.strBaseTex.insert(0, str);
}

// ���÷����ؽ��� ����
void TileTerrain::UpdateTextureFile(int index, LPCTSTR str)
{
	m_mapInfo.strSplattingTex[index].clear();
	m_mapInfo.strSplattingTex[index].insert(0, str);
}

const char* TileTerrain::GetBaseTextureName()
{
	return m_mapInfo.strBaseTex.c_str();
}
const char* TileTerrain::GetTextureName(int i)
{
	return m_mapInfo.strSplattingTex[i].c_str();
}


//--------------------------------------------------------------------------------------------
// �������� ��� gpg3���Ǻ��� �����ʵ� ��������
// pos[3]
void TileTerrain::_ComputeNormal(int sX, int eX, int sZ, int eZ)
{
	// GPG 3�ǿ� �ִ� ���� �����ʵ� ���� ����� �����Ѵ�.
	float left	= 0.0f;	// ��� ������ �߽����� ����
	float right	= 0.0f;	// ��� ������ �߽����� ������
	float up	= 0.0f;	// ��� ������ �߽����� ����
	float down	= 0.0f;	// ��� ������ �߽����� �Ʒ���
	D3DXVECTOR3 normal(0.0f,0.0f,0.0f);	// ���� ���͸� ������ �ӽú���

	// o--A--o	// A�� ����
	// B--M--C	// B�� ����, M�� ��������, C�� ������
	// o--D--o	// D�� �Ʒ���
	DWORD z=0;
	DWORD x=0;
	DWORD index = 0;
	for( z=sZ; z<eZ; ++z ) // ���̴� Ÿ�� ����(���� �����ڸ��� ����)
	{
		for( x=sX; x<eX; ++x ) // ���̴� Ÿ�� ����(���� �����ڸ��� ����)
		{
			// �ܰ����ο� ���� ó���� �����ϴ�. ���� ���� ����
			DWORD index = z*m_mapInfo.dwMapLineWidth+x;
			if( (index % m_mapInfo.dwMapLineWidth) == 0 )	// left line
			{
				m_pVertices[index].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				continue;
			}
			if( (index / m_mapInfo.dwMapLineWidth) == 0)	// top line
			{
				m_pVertices[index].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				continue;
			}
			if( (index % m_mapInfo.dwMapLineWidth) == (m_mapInfo.dwMapLineCellCount) )		// right line
			{
				m_pVertices[index].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				continue;
			}
			if( (index / m_mapInfo.dwMapLineWidth) == (m_mapInfo.dwMapLineCellCount) )		// bottom line
			{
				m_pVertices[index].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				continue;
			}

			left	= m_pVertices[ index - 1 ].p.y;				// ������ġ�� �߽����� ���� ���̰��� ����
			right	= m_pVertices[ index + 1 ].p.y;				// ������
			up		= m_pVertices[ index - m_mapInfo.dwMapLineWidth ].p.y;	// ����
			down	= m_pVertices[ index + m_mapInfo.dwMapLineWidth ].p.y;	// �Ʒ���

			normal	= D3DXVECTOR3( (left-right), 2, (down-up) );	// ���� ������ ������ �ʰ� �������� ����� ����
			D3DXVec3Normalize( &normal, &normal );	// �������ͷ� ����

			m_pVertices[index].n = normal;			// ���̸ʿ� �븻���� ����
		}
	}
}

//--------------------------------------------------------------------------------------------
// shading���� �׸���
void TileTerrain::DrawShadingTex(BOOL bWireFrame, D3DXMATRIXA16 *pReflect)
{
	m_pd3dDevice->SetFVF(D3DFVF_TILEVERTEX);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &(m_matWorld * (*pReflect)));
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);	

	// �ø��� ���̴� Ÿ�ϸ� �׸���
	std::vector<int>::iterator it;
	for(it = m_vecVisibleTile.begin(); it != m_vecVisibleTile.end(); ++it)
	{
		
		m_pd3dDevice->SetIndices(m_ppIB[m_pTileInfo[*it].nLODLevel][m_pTileInfo[*it].nCrackLevel]);
		m_pd3dDevice->SetStreamSource(0, m_pTileInfo[*it].pVB, 0, sizeof(CVERTEX3D_TEX2));
		
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_mapInfo.dwTileTotalVert, 0,
														m_pTileInfo[*it].dwTriCount);

	}
}

//--------------------------------------------------------------------------------------------
// 0�� �ؽ��ķ� ��ü�� �׸���
void TileTerrain::DrawBaseTex(BOOL bWireFrame, D3DXMATRIXA16 *pReflect)
{
	//m_pd3dDevice->SetMaterial(&m_mtrl);
	
	//if(bWireFrame)
	//	m_pd3dDevice->SetTexture( 0, NULL );
	//else
	
	

	m_pd3dDevice->SetFVF(D3DFVF_TILEVERTEX);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &(m_matWorld * (*pReflect)));
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);	

	// �ø��� ���̴� Ÿ�ϸ� �׸���
	std::vector<int>::iterator it;
	for(it = m_vecVisibleTile.begin(); it != m_vecVisibleTile.end(); ++it)
	{
		
		m_pd3dDevice->SetIndices(m_ppIB[m_pTileInfo[*it].nLODLevel][m_pTileInfo[*it].nCrackLevel]);
		m_pd3dDevice->SetStreamSource(0, m_pTileInfo[*it].pVB, 0, sizeof(TILEVERTEX));
		
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_mapInfo.dwTileTotalVert, 0,
														m_pTileInfo[*it].dwTriCount);
		

	}
	
	// wireFrame��� �ϰ��� tile�� �ܰ����� �ٸ������� �ѹ��� �׷��ش�.
	if(bWireFrame)
	{
		for(int i=0; i<m_mapInfo.dwMapTotalTileCount; i++)
		{
			_DrawWireFrameTile(i);
		}
	}
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
}


//--------------------------------------------------------------------------------------------
// Alpha�ؽ��� �׸���
// AlphaTexture �� �ߺ��ؼ� �׸��� �������� �ؽ��ļ����� TileTerrainMgr�� Splatting Ŭ�������� �����Ŀ� ���⼭�� �ε����� �׷��ش�
void TileTerrain::DrawAlphaTex()
{
	m_pd3dDevice->SetFVF(D3DFVF_TILEVERTEX);
	//m_pd3dDevice->SetTransform(D3DTS_WORLD, &(m_matWorld * (*pReflect)));
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);	

	// �ø��� ���̴� Ÿ�ϸ� �׸���
	std::vector<int>::iterator it;
	for(it = m_vecVisibleTile.begin(); it != m_vecVisibleTile.end(); ++it)
	{
		
		m_pd3dDevice->SetIndices(m_ppIB[m_pTileInfo[*it].nLODLevel][m_pTileInfo[*it].nCrackLevel]);
		m_pd3dDevice->SetStreamSource(0, m_pTileInfo[*it].pVB, 0, sizeof(TILEVERTEX));
		
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_mapInfo.dwTileTotalVert, 0,
														m_pTileInfo[*it].dwTriCount);
		
	}
	
	
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	// ��Ƽ�ؽ��� 0,1�� ������ ����� ���� �Ѵ�.
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// ���� ����� ���� �Ѵ�.
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
}




//--------------------------------------------------------------------------------------------
// �ش� tile��ȣ�� TL, TR, BL, BR���� �ϳ��� �簢���� �ٸ� ������ �׷��ش�.
void TileTerrain::_DrawWireFrameTile(int tile)
{
	static BRUSHVERTEX pVertices[8];
	DWORD dwColor;
	switch( m_pTileInfo[tile].nLODLevel )
	{
		case 0:
			dwColor = 0xFFFAEB78;			// �����
			break;
		case 1:
			dwColor = 0xFFFF8200;			// ��Ȳ��
			break;
		case 2:
			dwColor = 0xFFFF82FF;			// �����
			break;
		case 3:
			dwColor = 0xFFFF0000;			// ������
			break;
		case 4:
			dwColor = 0xFF80E12A;			// ���
			break;
		case 5:
			dwColor = 0xFF0000FF;			// �Ķ���
			break;
	}


	// Ÿ���� 4 �𼭸��� �簢������ �׷��ش�.
	pVertices[0].p =  m_pVertices[ m_pTileInfo[tile].dwEDGE[0] ].p;
	pVertices[1].p =  m_pVertices[ m_pTileInfo[tile].dwEDGE[1] ].p;
	pVertices[2].p =  m_pVertices[ m_pTileInfo[tile].dwEDGE[2] ].p;
	pVertices[3].p =  m_pVertices[ m_pTileInfo[tile].dwEDGE[3] ].p;

	pVertices[4].p =  m_pVertices[ m_pTileInfo[tile].dwEDGE[0] ].p;
	pVertices[5].p =  m_pVertices[ m_pTileInfo[tile].dwEDGE[2] ].p;
	pVertices[6].p =  m_pVertices[ m_pTileInfo[tile].dwEDGE[1] ].p;
	pVertices[7].p =  m_pVertices[ m_pTileInfo[tile].dwEDGE[3] ].p;

	for(int i=0; i<8; ++i)
	{
		pVertices[i].p.y = 0.0f;
		pVertices[i].color = dwColor;
	}
			
	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pd3dDevice->SetFVF(D3DFVF_BRUSHVERTEX);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_LINELIST, 4, pVertices, sizeof(BRUSHVERTEX));
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
	
}


//--------------------------------------------------------------------------------------------
// EDGE�� DWORD edge[4]�� �簢�� ���� ������ �����Ѵ�.
long int TileTerrain::_GetPositionInMap(D3DXVECTOR3 &pos, EDGE *edge)
{
	// ��ü �� �����ȿ� �ִ��� Ȯ���Ѵ�.
	if( pos.x < m_mapInfo.fStartX || pos.z > m_mapInfo.fStartZ || pos.z < m_mapInfo.fEndZ || pos.x > m_mapInfo.fEndX )
	{
		// ������ ����� -1�� ��ȯ�Ѵ�.
		return -1;
	}	//�ε����� ��ȯ�Ŀ� �񱳸� �ؾ��Ѵ�.

	// ��ġ�� �ε��� �񱳽ô� w/2�� ������ ������Ѵ�.
	float fX = ( ((float)m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale /2.0f + pos.x) / m_mapInfo.fCellScale );
	float fZ = ( ((float)m_mapInfo.dwMapLineWidth *  m_mapInfo.fCellScale /2.0f - pos.z) / m_mapInfo.fCellScale );

	//�»���� ���� ã�� ���� ������ ���ٰ��� ���� ���̴�.
	if( 0 > fX || fX >= (float)(m_mapInfo.dwMapLineCellCount) ||
		0 > fZ || fZ >= (float)(m_mapInfo.dwMapLineCellCount) )
	{
		return -1;
	}
	
	long int offsetX = (DWORD)::floorf(fX);						// floorf�� �Ҽ����� ���ֹ�����.
	long int offsetZ = (DWORD)::floorf(fZ);
	
	long int cellIndex = (offsetX + offsetZ * m_mapInfo.dwMapLineWidth );	

	edge->vTL = m_pVertices[cellIndex].p;									// TL
	edge->vTR = m_pVertices[cellIndex + 1].p;								// TR
	edge->vBL = m_pVertices[cellIndex + m_mapInfo.dwMapLineWidth].p;		// BL
	edge->vBR = m_pVertices[cellIndex + m_mapInfo.dwMapLineWidth + 1].p;	// BR

	return cellIndex;
}

//--------------------------------------------------------------------------------------------
// pos�� ���Ե� �»�� ���� �簢���� pos�� ���ؼ� ���尡��� ���� ã�´�.
// �׸��� �� ���� �߽����� �ֺ� 4���� �簢���� ���Ѵ�.(QUADEDGE)
DWORD TileTerrain::_GetNearPositionInCell(D3DXVECTOR3 &pos, DWORD cell, EDGE *edge, QUADEDGE *quadEdge)
{
	DWORD nearCell = 0;
	float offset = m_mapInfo.fCellScale/2.0f;	// ���� �ݰ�.

	// ����� ���� ã�´�.
	// TL�� TR�� ��� ����?
	if( (pos.x - edge->vTL.x) <= offset )
	{	// TL�ʿ� �����ٸ�
		// ���ʰ� �Ʒ����� ��� ����?
		if( (pos.z - edge->vTL.z) >= -offset )		// ����
			nearCell = cell;
		else										// �Ʒ���
			nearCell = cell + m_mapInfo.dwMapLineWidth;
	}
	else											// ������
	{
		if( (pos.z - edge->vTL.z) >= -offset )		// ����
			nearCell = cell + 1;
		else										// �Ʒ���
			nearCell = cell + m_mapInfo.dwMapLineWidth + 1;
	}

	// nearCell�� �߽����� 4���� �簢���� �����ؾ��Ѵ�.
	//	topLeft ----- top ----- topRight
	//		|			|			|
	//		|			|			|
	//	  left ------nearCell--- right	
	//		|			|			|
	//		|			|			|
	//	bottomLeft----bottom-----bottomRight
	// ������, ������ ���� ���, �ϴ�, ����, ���� ������ ���� ������ �ɰ�� ������ ����Եȴ�.
	// �׷��� �� �𼭸��� �ִ� ������ ������ ������ϴµ�
	// 0-1---------7-8    (0->10, 1->10, 7->16, 8->16) 
	// 9-10       16-17   (9->10, 17->16)
	// . .         . .
	// . .         . .
	// 63-64      70-71   (63->64, 71->70)
	// 72-73------79-80   (72->64, 73->64, 79->70, 80->70)
	// �̷������� ������ ���༭ �簢������ ���� ����� �ʵ��� �����.


	if( nearCell == 0 )																// �»��
		nearCell = nearCell + 1 + m_mapInfo.dwMapLineWidth;
	else if( nearCell == m_mapInfo.dwMapLineCellCount )								// ����
		nearCell = nearCell + m_mapInfo.dwMapLineWidth - 1;	
	else if( nearCell == m_mapInfo.dwMapLineWidth*(m_mapInfo.dwMapLineCellCount) )	// ���ϴ�
		nearCell = nearCell - m_mapInfo.dwMapLineWidth + 1;
	else if( nearCell == m_mapInfo.dwMapTotalVert - 1 )								// ���ϴ�
		nearCell = nearCell - m_mapInfo.dwMapLineWidth - 1;
	else if( nearCell>0 && nearCell<m_mapInfo.dwMapLineCellCount )					// ���
		nearCell += m_mapInfo.dwMapLineWidth;
	else if( nearCell > m_mapInfo.dwMapLineWidth*(m_mapInfo.dwMapLineCellCount) &&
			nearCell < m_mapInfo.dwMapTotalVert )									// �ϴ�
		nearCell -= m_mapInfo.dwMapLineWidth;
	else if( nearCell % m_mapInfo.dwMapLineWidth == m_mapInfo.dwMapLineCellCount 
			&& nearCell !=m_mapInfo.dwMapLineWidth && nearCell != m_mapInfo.dwMapTotalVert - 1)			// ���
		nearCell -= 1;
	else if( nearCell % m_mapInfo.dwMapLineWidth == 0 && nearCell != 0
				&& nearCell != m_mapInfo.dwMapLineWidth*(m_mapInfo.dwMapLineCellCount))		// �´�
		nearCell += 1;

	DWORD top = nearCell - m_mapInfo.dwMapLineWidth;
	DWORD topLeft = top - 1;
	DWORD topRight = top + 1;
	DWORD left = nearCell - 1;
	DWORD right = nearCell + 1;
	DWORD bottom = nearCell + m_mapInfo.dwMapLineWidth;
	DWORD bottomLeft = bottom - 1;
	DWORD bottomRight = bottom + 1;

	quadEdge->corner[0].vTL = m_pVertices[topLeft].p; 
	quadEdge->corner[0].vTR = m_pVertices[top].p; 
	quadEdge->corner[0].vBL = m_pVertices[left].p; 
	quadEdge->corner[0].vBR = m_pVertices[nearCell].p;

	quadEdge->corner[1].vTL = m_pVertices[top].p; 
	quadEdge->corner[1].vTR = m_pVertices[topRight].p; 
	quadEdge->corner[1].vBL = m_pVertices[nearCell].p; 
	quadEdge->corner[1].vBR = m_pVertices[right].p; 

	quadEdge->corner[2].vTL = m_pVertices[left].p; 
	quadEdge->corner[2].vTR = m_pVertices[nearCell].p; 
	quadEdge->corner[2].vBL = m_pVertices[bottomLeft].p; 
	quadEdge->corner[2].vBR = m_pVertices[bottom].p; 

	quadEdge->corner[3].vTL = m_pVertices[nearCell].p; 
	quadEdge->corner[3].vTR = m_pVertices[right].p; 
	quadEdge->corner[3].vBL = m_pVertices[bottom].p; 
	quadEdge->corner[3].vBR = m_pVertices[bottomRight].p; 
	return nearCell;
}



//--------------------------------------------------------------------------------------------
// Pick ���� �޾Ƽ� ���� ����Ʈ���� �� �ﰢ���� ã�Ƽ� m_vPickTri ������ �ִ´�
void TileTerrain::PickGround(HPickPtr Pick)
{
	
	// ��ŷ������
	D3DXVECTOR3 vOrig = Pick->m_pickRay._origin;
	vOrig.y = 0.0f;

	// ��ŷ ����(������ ���̸� �켱 1000���� �����Ѵ�.
	D3DXVECTOR3 vDest = Pick->m_pickRay._origin + Pick->m_pickRay._direction * 1000.0f;
	vDest.y = 0.0f;
	
	// �������� �������� ���� ���� ���͸� �����.
	D3DXVECTOR3 vDist = vDest - vOrig;
	D3DXVec3Normalize(&vDist, &vDist);

	m_bIsPicking = false;		// ��ŷ ���� �ʱ�ȭ
	long int index = -1;
	D3DXVECTOR3 nextPos(0.0f, 0.0f, 0.0f);
	for(int i=0; i<m_mapInfo.dwMapLineWidth*3; ++i)
	{
		nextPos = vDist * (m_mapInfo.fCellScale * i) + vOrig;
		EDGE edge;
		QUADEDGE quadEdge;
		index = _GetPositionInMap(nextPos, &edge);		// nextPos�� ���Ե� ���� ������ ���Ѵ�.		
		// EDGE�� ���ؼ� ������ Ȯ���Ѵ�. ���� ����ٸ� ��� ������ ������ ���� ������
		if( index == -1 )
			continue;
		_GetNearPositionInCell(nextPos, index, &edge, &quadEdge);	// ��ŷ�׽�Ʈ�� �� 4���� �簢���� ������ ���Ѵ�.
		float fDist = 0.0f;

		// ��ŷ�׽�Ʈ
		for(int j=0; j<4; ++j)
		{
			// ��� �ﰢ��
			if( Pick->IntersectTriangle( quadEdge.corner[j].vTL, quadEdge.corner[j].vTR, quadEdge.corner[j].vBL, fDist) )
			{
				m_vPickingPos = Pick->m_pickRay._origin + Pick->m_pickRay._direction*fDist;
				m_bIsPicking = true;

				m_vPickTri[0] = quadEdge.corner[j].vTL;
				m_vPickTri[1] = quadEdge.corner[j].vTR;
				m_vPickTri[2] = quadEdge.corner[j].vBL;

				break;
			}
			// �ϴ� �ﰢ��
			if( Pick->IntersectTriangle( quadEdge.corner[j].vTR, quadEdge.corner[j].vBR, quadEdge.corner[j].vBL, fDist) )
			{
				m_vPickingPos = Pick->m_pickRay._origin + Pick->m_pickRay._direction*fDist;
				m_bIsPicking = true;

				m_vPickTri[0] = quadEdge.corner[j].vTR;
				m_vPickTri[1] = quadEdge.corner[j].vBR;
				m_vPickTri[2] = quadEdge.corner[j].vBL;

				break;
			}
		}
		if( m_bIsPicking )
		{

			DWORD dwChangeColor;			
			D3DXVECTOR3 p(0.0f, 0.0f, 0.0f);					// �귯�� ������ ����
			D3DXVECTOR3 m(0.0f, 0.0f, 0.0f);					// ���� �迭 �߿��� ��ŷ�� ������ ��ġ

			// ������ǥ���� ������ �ؽ�����ǥ���� �ؼ������� ������ �ʿ���
			// 1������ �� �ȼ��ΰ�? �ȼ��� �ʺ� / ������ �ʺ�
			float vertexPerPixel = (float)ALPHA_TEXTURE_SIZE / (float)m_mapInfo.dwMapLineWidth;
			// �귯�� �������� ���ȼ��ΰ�? �����Ǿ���� �ȼ��� ��������� ����
			float editOutSize = m_brushOutSize * vertexPerPixel;
			float editInSize = m_brushInSize * vertexPerPixel;
			float smooth = editOutSize - editInSize;				// �ε巴�� ĥ�ؾ��� ����
	
			// ��ü ���� �������� �����ؼ� ���Ѵ�.
			// ��ŷ�� ��ġ�� ���� ��ǥ�� ��ȯ�Ǿ��ִ�.
			// ������ ������ 0,0 �̱� ������, ��ŷ�� ������ �ؽ���(�»���� 0,0)�� �񱳸� ���� ����
			m.x = (float)(m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f + m_vPickingPos.x)/m_mapInfo.fCellScale;
			m.z = (float)(m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f - m_vPickingPos.z)/m_mapInfo.fCellScale;
			// ��ŷ������ �ؽ��� ��ǥ�� u, v�� ���� ���� / �ʺ�
			m.x = m.x / m_mapInfo.dwMapLineCellCount;
			m.z = m.z / m_mapInfo.dwMapLineCellCount;

			Pick->SetTextureCoord( m.x, m.z, editInSize, editOutSize);


			break;			// ��ŷ�̵Ǿ��ٸ� ����
		}
		index = -1;
	}
}


//--------------------------------------------------------------------------------------------
// ��ŷ�� �ﰢ�� �׸���
void TileTerrain::BrushTriangleDraw(D3DXCOLOR color)
{
	BRUSHVERTEX pVertices[3];
	pVertices[0].p =  m_vPickTri[0];
	pVertices[0].color = color;
	pVertices[1].p =  m_vPickTri[1];
	pVertices[1].color = color;
	pVertices[2].p =  m_vPickTri[2];
	pVertices[2].color = color;

	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pd3dDevice->SetFVF(D3DFVF_BRUSHVERTEX);
	m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, pVertices, sizeof(BRUSHVERTEX));
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);

//	g_pLog->Log(TEXT(" x: %0.2f     z : %0.2f"), pVertices[0].p.x, pVertices[0].p.z);
}

//���콺 ��ġ�� ���� �׸���
void TileTerrain::BrushCricleDraw(int count)
{
	float radian = D3DX_PI*2/count;					// ȸ���� ����
	float brushOutSize = m_brushOutSize * m_mapInfo.fCellScale;			// �귯�� ũ�� ������
	float brushInSize = m_brushInSize * m_mapInfo.fCellScale;

	BRUSHVERTEX brushOutLine[2];					// ������ D3DTS_LINELIST�� ���� �׷�������.
	BRUSHVERTEX brushInLine[2];
	D3DXVECTOR3 curPos(1.0f, 0.0f, 0.0f);
	D3DXVECTOR3 newPos;
	D3DXMATRIX mat;
	
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pd3dDevice->SetTexture(0, NULL);
	m_pd3dDevice->SetFVF( D3DFVF_BRUSHVERTEX );
	
	brushOutLine[1].p = curPos * brushOutSize + m_vPickingPos;
	brushOutLine[1].p.y = GetHeightY(brushOutLine[1].p.x, brushOutLine[1].p.z)+0.5f;
	brushOutLine[1].color = m_dwBrushOutColor;
	
	brushInLine[1].p = curPos * brushInSize + m_vPickingPos;
	brushInLine[1].p.y = GetHeightY(brushInLine[1].p.x, brushInLine[1].p.z)+0.5f;
	brushInLine[1].color = m_dwBrushInColor;
	
	for(int i=1; i<count+1; ++i)
	{
		D3DXMatrixRotationY(&mat, radian*i);
		D3DXVec3TransformCoord(&newPos, &curPos, &mat);
		D3DXVec3Normalize(&newPos, &newPos);
		
		if(m_bOutBrush)
		{
			brushOutLine[0] = brushOutLine[1];
			brushOutLine[1].p = newPos * brushOutSize + m_vPickingPos;
			brushOutLine[1].p.y = GetHeightY(brushOutLine[1].p.x, brushOutLine[1].p.z);
			brushOutLine[1].color = m_dwBrushOutColor;
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, brushOutLine, sizeof(BRUSHVERTEX) );
		}
		if(m_bInBrush)
		{
			brushInLine[0] = brushInLine[1];
			brushInLine[1].p = newPos * brushInSize + m_vPickingPos;
			brushInLine[1].p.y = GetHeightY(brushInLine[1].p.x, brushInLine[1].p.z);
			brushInLine[1].color = m_dwBrushInColor;			
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, brushInLine, sizeof(BRUSHVERTEX) );
		}
	}
	//m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);
}



float TileTerrain::GetHeightY(float x, float z)
{

	float height=0.0f;

	// ������ �������� �������� �̵��ϴ� ��ȯ
	x = ((float)m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f) + x;
	z = ((float)m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f) - z;				// z���� �Ʒ��� ���� �ϰ� �����.
	x = (float)(x / m_mapInfo.fCellScale);
	z = (float)(z / m_mapInfo.fCellScale);					

	// ���콺 ��ġ�� ���� �Ѿ�� 0.0f�� ����
	if( 0.0f >= x || x >= (float)(m_mapInfo.dwMapLineCellCount) ||
		0.0f >= z || z >= (float)(m_mapInfo.dwMapLineCellCount) )
	{
		return 0.0f;
	}

	long int nx = (long int)::floorf(x);						// floorf�� �Ҽ����� ���ֹ�����.
	long int nz = (long int)::floorf(z);

	//  A   B
    //  *---*
    //  | / |
    //  *---*
    //  C   D (1,1)
	float A = m_pVertices[nx + nz*m_mapInfo.dwMapLineWidth].p.y;	// nx, nz�� ���Ե� ��ġ�� �� ������ Y���� ���´�.
	float B = m_pVertices[nx+1 + nz*m_mapInfo.dwMapLineWidth].p.y;
	float C = m_pVertices[nx + (nz+1)*m_mapInfo.dwMapLineWidth].p.y;
	float D = m_pVertices[nx+1 + (nz+1)*m_mapInfo.dwMapLineWidth].p.y;

	// nx, nz�� ���Ե� ���� 0,0 ������ ��ġ�� �ǵ��� �������� �����Ѵ�.(0~1���� ���̵ȴ�)
	float dx = x - nx;
	float dz = z - nz;

	// ������ ���� �������� �ǹ̿� ���������� ���� x,z��ġ�� y���� ���´�.
	// ���� ���� ���� 1�� ���簢���̴�. �� ���簢���� �밢���� y=x�̰�, dx�� dz�� �񱳸� ���ؼ� �ﰢ���� ��ġ�� �Ǻ�.
	if( dz + dx < 1.0f )  // ���� �ﰢ��
	{
		float uy = B - A;
		float vy = C - A;
		height = A + _Lerp(0.0f, uy, dx) + _Lerp(0.0f, vy, dz);
	}
	else			// �Ʒ��� �ﰢ��
	{
		float uy = C - D;
		float vy = B - D;
		height = D + _Lerp(0.0f, uy, 1.0f - dx) + _Lerp(0.0f, vy, 1.0f - dz);
	}
	return height;
}



// ī�޶� look���� ���� �� Ÿ�Ϻ� _GetLODLevel�� ���� LODLevel ����
void TileTerrain::SetupLODLevel(D3DXVECTOR3 look)
{
	if(m_bIsLOD)
	{
		for(int i=0; i<m_mapInfo.dwMapTotalTileCount; ++i)
		{
			_GetLODLevel(look, i);
		}
		for(int i=0; i<m_mapInfo.dwMapTotalTileCount; ++i)
		{
			_SetupCrack(i);
		}
	}
	else		// LOD �� �����̶�� ������ LODLeve�� ũ���� �Ҵ�
	{
		for(int i=0; i<m_mapInfo.dwMapTotalTileCount; ++i)
		{
			m_pTileInfo[i].nLODLevel = 0;
			m_pTileInfo[i].nCrackLevel = 0;
			m_pTileInfo[i].dwTriCount = m_mapInfo.pTriCnt[0][0];
		}
	}

}


// look = ī�޶��� Look, index = Ÿ�Ϲ�ȣ
// Ÿ���� ������ ī�޶��� �Ÿ��� ���� TILEINFO�� ���� LOD�� �����Ѵ�.
void TileTerrain::_GetLODLevel(D3DXVECTOR3 look, int index)
{
	// ���� ������ ũ�⸦ ���� ��� ���� ����� �Ÿ��� �����ȴ�. �׷���� �ε����� ũ���� 16���ʿ��ϴ�.
	// �׷��� ����� �ܼ�ȭ�� ����, x,z���� ���� ���ؼ� �Ÿ��� ������ ���� ������ �Ѵ�.
	D3DXVECTOR3 distX(0.0f, 0.0f, 0.0f);
	distX.x = look.x - m_pVertices[m_pTileInfo[index].dwCenter].p.x;
	float LenX = D3DXVec3Length(&distX);
	D3DXVECTOR3 distZ(0.0f, 0.0f, 0.0f);
	distZ.z = look.z - m_pVertices[m_pTileInfo[index].dwCenter].p.z;
	float LenZ = D3DXVec3Length(&distZ);
	float len;				// Ÿ���� �������� ���� ���� �ָ� ������ ���� ����
	if( LenX >= LenZ)
	{
		len = LenX;
	}
	else
		len = LenZ;
	for(int i=0; i<m_mapInfo.dwMaxLODLevel; ++i)
	{
		if( len >= (float)(i*m_fLODRate) )
		{
			m_pTileInfo[index].nLODLevel = i;
		}
	}
}


// �ֺ� Ÿ�ϰ� ���ؼ� ũ���� �����Ѵ�.
void TileTerrain::_SetupCrack(int index)
{
	int currentIndex = index;
	int temp[4];				// �»���� �ε�����
	int tile = m_mapInfo.dwMapLineTileCount;
	temp[0] = (currentIndex % tile == 0 ) ? currentIndex : (currentIndex - 1);		// LEFT
	temp[1] = (currentIndex / tile == 0 ) ? currentIndex : (currentIndex - tile);	// TOP
	temp[2] = (currentIndex % tile == (tile-1) ) ? currentIndex : (currentIndex + 1);	// RIGHT
	temp[3] = (currentIndex / tile == (tile-1) ) ? currentIndex : (currentIndex + tile);	//BOTTOM
	m_pTileInfo[currentIndex].nCrackLevel = 0;	// �⺻ ũ�� ����
	for(int i=0; i<4; ++i)
	{
		// ��Ÿ���� �ֺ� Ÿ�� ���� ũ�ٴ°�, ũ���� �ʿ��ϴٴ� ���̴�.
		// LOD������ �簢�������� �����߱� ������, LOD�� �ٸ� ���� ���� ����̴�.
		if( m_pTileInfo[currentIndex].nLODLevel > m_pTileInfo[temp[i]].nLODLevel )
		{
			m_pTileInfo[currentIndex].nCrackLevel = i+1;		// ũ��0�� �⺻ �̱⶧���� +1�� ���ش�.
			break;
		}
	}
	// ũ���� �ٲ��, �ﰢ���� ������ �ٲ��.
	// �������� �̸� ���� [LODLeve][CrackLevel]�� ���� ������Ʈ���ش�.
	m_pTileInfo[currentIndex].dwTriCount = 
		m_mapInfo.pTriCnt[m_pTileInfo[currentIndex].nLODLevel][m_pTileInfo[currentIndex].nCrackLevel];
}



void TileTerrain::EditHeightInCricle(float frameDelta)
{
	const float EPSILON = 0.001f;

	if(!m_bIsPicking)
		return ;
	D3DXVECTOR3 p(0.0f, 0.0f, 0.0f);							// �귯�� ������ ����
	D3DXVECTOR3 m(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 s = m_vPickingPos / m_mapInfo.fCellScale;		// ��ŷ�� �߾� ���� �迭�� �ε����� �񱳸� ����
	s.y = 0.0f;													

	// ��ŷ�� ��ġ�� ���� ��ǥ�� ��ȯ�Ǿ��ִ�.
	// ���� ������ ������ 0,0 �̱� ������, ��ŷ�� ������ ���� �迭�� �ε����� �̿��ϱ� ���ؼ��� ������ �ʿ�.
	m.x = (float)(m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f + m_vPickingPos.x)/m_mapInfo.fCellScale;
	m.z = (float)(m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f - m_vPickingPos.z)/m_mapInfo.fCellScale;

	// ���� �귯���� ���ԵǴ� ���簢�� ����
	int start_X = (int)(m.x) - m_brushOutSize; 
	int start_Z = (int)(m.z) - m_brushOutSize;
	int end_X = (int)(m.x) + m_brushOutSize;
	int end_Z = (int)(m.z) + m_brushOutSize;

	if( start_X < 0 ) { start_X = 0; }
	if( start_Z < 0 ) { start_Z = 0; }
	if( end_X >= m_mapInfo.dwMapLineWidth ) { end_X = m_mapInfo.dwMapLineCellCount; }	// 0���� �����ϱ⶧���� ���� -1
	if( end_Z >= m_mapInfo.dwMapLineWidth ) { end_Z = m_mapInfo.dwMapLineCellCount; }	// 0���� �����ϱ⶧���� ���� -1
	
	// �׽�Ʈ
//	g_pLog->Log(TEXT(" start  X:%ld  Z:%ld    end  X:%ld   Z:%ld"), start_X, start_Z, end_X, end_Z);
//	g_pLog->Log(TEXT(" width : %d     heihgt : %d"), end_X - start_X, end_Z - start_Z);

	for(int z=start_Z; z<=end_Z; ++z)
	{
		for(int x=start_X; x<=end_X; ++x)
		{
			DWORD mapIndex = z*m_mapInfo.dwMapLineWidth + x;
			p = m_pVertices[ mapIndex].p / m_mapInfo.fCellScale;
			p.y = 0.0f;										// 1��ũ���� �ٵ��ǹ迭 �񱳸����� y�� ����
//			D3DXVECTOR3 lenVec = p - (D3DXVECTOR3(m_vPickingPos.x, 0.0f, m_vPickingPos.z)/m_mapInfo.fCellScale);
//			float len = D3DXVec3Length(&lenVec);
			float len = (p.x - s.x)*(p.x - s.x) + (p.z - s.z)*(p.z - s.z);
	
			// ���� �ٸ� �˰����� �����ؼ� �ڿ������� �����ǰ� �����.
			// �߽������� ���� �Ÿ��� �ּ��� �������� �۾�����
			float y = -1.0f/m_brushOutSize * len * m_fSlopeRate + m_brushOutSize;

			if(len < m_brushOutSize * m_brushOutSize)  // ������ ���ȿ� �ִٸ�
			{
				switch(m_nUpDownMode)
				{
				case 0:				// �����
					m_pVertices[ mapIndex].p.y += (y)* m_heightRate*frameDelta;
					m_pVertices[ mapIndex].n.y = m_pVertices[ mapIndex].p.y;
					break;

				case 1:				// �ٿ�
					m_pVertices[ mapIndex].p.y -= (y)* m_heightRate*frameDelta;
					m_pVertices[ mapIndex].n.y = m_pVertices[ mapIndex].p.y;
					break;

				case 2:				// ��źȭ
					if( abs(m_pVertices[ mapIndex].p.y - m_vPickingPos.y) > EPSILON)  // ���� �ʴٸ�
					{
						// ��ŷ������ ���ٸ� �ø���
						if( m_pVertices[ mapIndex].p.y - m_vPickingPos.y < EPSILON )
						{
							if( (m_pVertices[ mapIndex ].p.y + (y)* m_heightRate*frameDelta) - m_vPickingPos.y < EPSILON )
								m_pVertices[ mapIndex ].p.y += (y)* m_heightRate*frameDelta;
							else
								m_pVertices[ mapIndex].p.y = m_vPickingPos.y;
						}
						else	// ��ŷ������ ���ٸ� ������.
						{
							if( (m_pVertices[ mapIndex].p.y - (y)* m_heightRate*frameDelta) - m_vPickingPos.y > EPSILON )
								m_pVertices[ mapIndex].p.y -= (y)* m_heightRate*frameDelta;
							else
								m_pVertices[ mapIndex].p.y = m_vPickingPos.y;
						}
						m_pVertices[ mapIndex].n.y = m_pVertices[ mapIndex].p.y;
					}
					break;

				case 3:		// 4�� �ٸ� ���ڸ� �׳� ����ȭ
					if( abs(m_pVertices[ mapIndex].p.y) > EPSILON)  // 0�� ������ �ƴ϶��
					{
						if( m_pVertices[ mapIndex].p.y > EPSILON)
						{
							if( m_pVertices[ mapIndex].p.y - (y)* m_heightRate*frameDelta > EPSILON)
								m_pVertices[ mapIndex].p.y -= (y)* m_heightRate*frameDelta;
							else
								m_pVertices[ mapIndex].p.y = 0.0f;
						}
						else
						{
							if( m_pVertices[ mapIndex].p.y + (y)* m_heightRate*frameDelta < EPSILON)
								m_pVertices[ mapIndex].p.y += (y)* m_heightRate*frameDelta;
							else
								m_pVertices[ mapIndex].p.y = 0.0f;
						}
						m_pVertices[ mapIndex].n.y = m_pVertices[ mapIndex].p.y;
					}
				}	// switch end
			}	// if ���ȿ� �ִ°�? end	
		}	// for x end
	}	// for z end

	// ���� �귯���� ���Ե� ������� Ÿ�ϵ��� ������Ʈ ���ش�.
	// �� Ÿ���� �׵θ� ������ ���� �ߺ������� �Ǿ��ִ�. ����, ������ ���� ���� ���� Ÿ�Ϸ� ���Ǳ� ������ ���
	// ������, ����, �ϴ� ����� �� Ÿ�� Ÿ�ϵ鵵 ������ �Ǿ�� �Ѵ�. �׷��� ����, �ϴ��� +1ũ�⸸ŭ �������Ѽ�
	// ������ �˻��Ұ�� �̷� ������ �ذᰡ���ϴ�.
	if( end_X < m_mapInfo.dwMapLineCellCount )
		end_X += 1;
	if( end_Z < m_mapInfo.dwMapLineCellCount )
		end_Z += 1;
	// ���� ���� ���
	// ������ ������ �ֺ� ������ ������ �ޱ� ������, ��ü������ ����� ������Ѵ�.
	// �׷��� ���� �����, ���������� �ٽ� ����� �� �ʿ䰡 �ִ�.
	_ComputeNormal( start_X, end_X, start_Z, end_Z);

	DWORD indexTL = start_X + start_Z * m_mapInfo.dwMapLineWidth;
	DWORD indexTR = end_X + start_Z * m_mapInfo.dwMapLineWidth;
	DWORD indexBL = start_X + end_Z * m_mapInfo.dwMapLineWidth;

	int tileTL = _GetPositionInTile( m_pVertices[indexTL].p );
	int tileTR = _GetPositionInTile( m_pVertices[indexTR].p );
	int tileBL = _GetPositionInTile( m_pVertices[indexBL].p );


	// Ÿ���� ���� Ƚ��(��, ���� ����)
	int offsetZ = (tileBL - tileTL)/m_mapInfo.dwMapLineTileCount;
	
	for(int z = 0; z <= offsetZ; ++z)
	{
		for(int x = tileTL; x <= tileTR; ++x)
		{
			int index = x + z * m_mapInfo.dwMapLineTileCount;
			_CreateVB(index, m_pTileInfo);
		}
	}
		// �׽�Ʈ
//	for(DWORD i=0; i<m_mapInfo.dwMapTotalTileCount; i++)
//	{
//		_CreateVB(i, m_pTileInfo);
//	}
}


//  pos�� ���Ե� Ÿ�� ���� ��ȯ�Ѵ�.
int TileTerrain::_GetPositionInTile(D3DXVECTOR3 &pos)
{
	// ��ü �� �����ȿ� �ִ��� Ȯ���Ѵ�.
	if( pos.x < m_mapInfo.fStartX || pos.z > m_mapInfo.fStartZ || pos.z < m_mapInfo.fEndZ || pos.x > m_mapInfo.fEndX )
	{
		// ������ ����� -1�� ��ȯ�Ѵ�.
		return -1;
	}	//�ε����� ��ȯ�Ŀ� �񱳸� �ؾ��Ѵ�.

	// Ÿ���� ������ ��*ũ�� �̴�. ���� 0<=  pos/���� < 1 �̶�� 0��Ÿ���� �ȴ�.
	// ��ġ�� �ε��� �񱳽ô� -w/2�� ������ ������Ѵ�.
	int offsetX =  (int)( (m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale /2.0f + pos.x) / 
															(m_mapInfo.fCellScale * m_mapInfo.dwTileLineCellCount) );
	int offsetZ =  (int)( (m_mapInfo.dwMapLineWidth *  m_mapInfo.fCellScale /2.0f - pos.z) / 
															(m_mapInfo.fCellScale * m_mapInfo.dwTileLineCellCount) );
	// Ÿ���� ��ȣ�� 0���� tile -1 ���̴�. �̰�� ���� ���ΰ�, �ϴ� ������ �� ���� Ÿ���� ������ ���� ���̵ȴ�.
	// �׷��� ������ ������� ������, �� ���� Ÿ�ϰ� ���ٸ� tile-1�� ������ �ش�.
	if( offsetX == m_mapInfo.dwMapLineTileCount )
		offsetX = m_mapInfo.dwMapLineTileCount - 1;
	if( offsetZ == m_mapInfo.dwMapLineTileCount )
		offsetZ = m_mapInfo.dwMapLineTileCount - 1;
	
	return (int)( offsetX + offsetZ * m_mapInfo.dwMapLineTileCount );
}

