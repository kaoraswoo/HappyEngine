#include "TileTerrain.h"
#include "Utility\dib.h"

//--------------------------------------------------------------------------------------------
/// 생성자
TileTerrain::TileTerrain(RenderMgr* SceneRen)
{
	TileTerrainRender=SceneRen;
	
	m_pd3dDevice=TileTerrainRender->g_pd3dDevice;

	// 맵 정점 정보
	m_pVertices = NULL;
	m_pIndices = NULL;
	m_pTileInfo = NULL;
	m_ppIB = NULL;
	
	/*
	// 텍스쳐
	m_pBaseTexture = NULL;
	for(int i=0; i<4; ++i)
		m_ppTexSplatting[i] = NULL;
	m_pTexAlphaMap = NULL;
	
	
	// 베이스 텍스쳐를 초기화 해준다.
	m_mapInfo.bUpdateBaseTex = true;
	m_mapInfo.strBaseTex.insert(0, _T("textures\\water02.jpg"));
	*/


	// 픽킹
	m_bIsPicking = false;
	m_dPickTile = 0;
	m_vPickingPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


	// 브러쉬
	m_bOutBrush = true;
	m_bInBrush = true;
	m_brushOutSize = 5;
	m_brushInSize = 3;
	m_dwBrushOutColor = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	m_dwBrushInColor = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
	m_heightRate = 1;
	m_fSlopeRate = 1.0f;
	m_nUpDownMode = 0;
	m_nTexFillMode = 0;			// 9 = 생성, 0 = 삭제

	m_bDragBrush = false;
	m_fLineFixed = 10.0f;
	m_fLineAdd = 2.0f;

	//LOD간격의 1차가 되게 고정 시킬려면 최소 타일별 셀의 개수 이상의 단위가 되어야 한다.
	//지형 편집시 크랙을 안보이게 하기 위해서는 한화면에 최소 1개의 LOD가 나오는게 좋겠다 
	m_bIsLOD = true;
	m_fLODRate = 30.0f;


	// 쿼드트리
	//m_pQuadTree = NULL;

}

//--------------------------------------------------------------------------------------------
/// 소멸자
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


	// 이중포인터를 가지는 인덱스버퍼 해제
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
	std::ifstream in(filepath); //읽기 스트림 생성
	boost::archive::text_iarchive ia(in); //연 스트림을 넘겨주어서 직렬화객체 초기화
	ia>>m_mapInfo; //읽기

	// 읽은 것을 가지고 높이맵에 다시 구현하기 (구현필요)
	// mapinfo 모든것을 시리얼라이제이션 하는것으로 구현해야한다..
	// 저장도 현재는 임시로 높이값만 넣은상태
	

}

void TileTerrain::SetHeight()
{
	for(DWORD cnt=0; cnt<m_mapInfo.m_heightlist.size(); ++cnt)
	{
			m_pVertices[cnt].p.y = m_mapInfo.m_heightlist[cnt];
	}
}


//--------------------------------------------------------------------------------------------
// 맵 정보 등록( _BuildHeightMap 시 내부에서 불려짐)
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

	// 시작과 끝점
	m_mapInfo.fStartX = -(float)(m_mapInfo.dwMapLineWidth / 2.0f) * scale;
	m_mapInfo.fStartZ =  (float)(m_mapInfo.dwMapLineWidth / 2.0f) * scale;		// Z축은 반대
	m_mapInfo.fEndX	  =  (float)(m_mapInfo.dwMapLineWidth / 2.0f) * scale;
	m_mapInfo.fEndZ   = -(float)(m_mapInfo.dwMapLineWidth / 2.0f) * scale;
	m_mapInfo.fCenterX = (m_mapInfo.fStartX + m_mapInfo.fEndX)/2;
	m_mapInfo.fCenterZ = (m_mapInfo.fStartZ + m_mapInfo.fEndZ)/2;

	//최대 LOD단계 설정
	WORD cnt = 1;				// 0Level 부터 1단계이기 때문에
	int divideCell = cell;
	while( divideCell > 2)				// 각 LODLevel은 1/2의 배수 만큼의 셀이 필요하다.
	{
		divideCell /= 2;
		++cnt;
	}
	m_mapInfo.dwMaxLODLevel = cnt;
	m_mapInfo.olddwMaxLODLevel = cnt;

	// [MaxLOD][5]의 각 삼각형(인덱스)의 수를 위한 공간 생성
	m_mapInfo.pTriCnt = new WORD[cnt][5];
}


//--------------------------------------------------------------------------------------------
void TileTerrain::_ResetGround(void)
{
	DELS(m_pVertices);
	DELS(m_pIndices);
	DELS(m_pTileInfo);

	// 이중포인터를 가지는 인덱스버퍼 해제
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

	// 타일 정보도 비운다.
	m_vecVisibleTile.clear();
	m_vecTexDrawTile.clear();
}

//--------------------------------------------------------------------------------------------
// 지형만들기(strHeight 경로가 비어있으면 평지로 생성(BuildHeightMap호출) / 경로 있을때는 BuildBMPHeightMap호출)
HRESULT TileTerrain::CreateGround(LPCTSTR strHeight, float scale, float heightScale, int tile, int cell)
{

	HRESULT hr;
	D3DXMatrixIdentity(&m_matWorld);

	// 지형 버퍼를 비운다.
	_ResetGround();

	// 높이맵을 등록한다.
	m_mapInfo.strHeightMap.clear();
	m_mapInfo.strHeightMap.insert(0, strHeight);

	

	// 전체 지형정보 생성 
	if( m_mapInfo.strHeightMap.empty() || m_mapInfo.strHeightMap == TEXT("")  )
	{
		hr = _BuildHeightMap(cell, tile, scale);  // 높이맵이 없다면 그냥 평지 생성
		if(FAILED(hr))
		{
			MessageBox(NULL, "_BuildHeightMap failed", "", MB_OK);
		}
	}
	else
	{
		hr = _BuildBMPHeightMap(m_mapInfo.strHeightMap.c_str(), cell, scale, heightScale);	// 높이맵 생성
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

	// TILEINFO를 타일 수 만큼 생성한다.
	m_pTileInfo = new TILEINFO[m_mapInfo.dwMapTotalTileCount];

	// 각 타일별 정점버퍼 생성
	for(DWORD i=0; i<m_mapInfo.dwMapTotalTileCount; i++)
	{
		if( FAILED(_CreateVB(i, m_pTileInfo)) )
			return E_FAIL;
	}

	// LODLevel 인덱스버퍼를 생성
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
	if(	m_pQuadTree != NULL)		// 기존의 쿼드 트리를 제거 하고 다시 생성한다.
		DEL(m_pQuadTree);
	m_pQuadTree = new TileQuadTree(m_mapInfo.dwMapLineTileCount);
	m_pQuadTree->BuildQuadTree( this );
	*/
	
	return S_OK;
}




//--------------------------------------------------------------------------------------------
// 전체 지형 정보를 저장할 정점배열을 만든다.(y값이 0.0인)
HRESULT TileTerrain::_BuildHeightMap(int cell, int tile, float scale)
{
	_SetupMapInfo(cell, tile, scale);			// 맵 정보 등록
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
			v.p.x = (float)(x-dwHeight/2.0f)*m_mapInfo.fCellScale;		// 정점의 x좌표(메시를 원점에 정렬)
			v.p.z = -((float)(z-dwHeight/2.0f)*m_mapInfo.fCellScale);	// z축이 모니터안쪽이므로 -를 곱한다
			v.p.y = 0.0f;
			v.n.x = v.p.x;
			v.n.y = v.p.y;
			v.n.z = v.p.z;
			D3DXVec3Normalize( &v.n, &v.n );
			v.t.x = (float)x / (m_mapInfo.dwMapLineCellCount);
			v.t.y = (float)z / (m_mapInfo.dwMapLineCellCount);
			v.t1.x = 0.0f;
			v.t1.y = 0.0f;
			m_pVertices[cnt++] = v;				// 버텍스 값 유지
		}
	}
	// 법선 벡터 계산
	_ComputeNormal(0, dwHeight, 0, dwHeight);
	return S_OK;
}

//--------------------------------------------------------------------------------------------
// BMP 높이맵을 로딩해서 지형을 생성한다.
// BMP 맵은 가로 새로 2^n+1 크기를 보장해야한다.
// 단, 셀은 32칸으로 고정
// 총 너비-1 / 32칸 = 타일 칸이다.
HRESULT TileTerrain::_BuildBMPHeightMap(LPCTSTR pfileName, int cell, float scale, float heightScale)
{
	//TestLoadTile();
	char str[120];
	strcpy(str, pfileName); 
	LPBYTE pDIB = DibLoadHandle((LPSTR)str);
	/////////////////////////////////////////////////////////////////////
	if(!pDIB) return E_FAIL;
	DWORD cxDIB = DIB_CX(pDIB);					// 전체 가로(새로)의 정점 크기값 2^n+1이다.
	DWORD tile = (cxDIB-1)/cell;				// 2^n으로 변환후 셀크기로 나누면 타일 값이 나온다.
	_SetupMapInfo(cell, tile, scale, heightScale);			// 맵정보 등록	

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
			v.p.x = (float)(x-dwHeight/2.0f)*m_mapInfo.fCellScale;		// 정점의 x좌표(메시를 원점에 정렬)
			v.p.z = -((float)(z-dwHeight/2.0f)*m_mapInfo.fCellScale);	// z축이 모니터안쪽이므로 -를 곱한다
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
			m_pVertices[cnt++] = v;				// 버텍스 값 유지
		}
	}
	DibDeleteHandle(pDIB);
	// 법선 벡터 계산
	_ComputeNormal(0, dwHeight, 0, dwHeight);
	return S_OK;
}


//--------------------------------------------------------------------------------------------
// 각 index번째 타일 정보를 생성한다.
// 타일 정보를 업데이트 할때도 쓴다.
HRESULT TileTerrain::_CreateVB(int index, TILEINFO *pTileInfo)
{
	// 최소 생성시만
	if(pTileInfo[index].pVertices != NULL)
	{
		delete[] pTileInfo[index].pVertices;
	}
		pTileInfo[index].pVertices = new TILEVERTEX[m_mapInfo.dwTileTotalVert];
	
	// 전체 버텍스로 부터 각 타일별 오프셋을 계산한다.
	// 가로 = (타일번호 % 가로타일수) * 한타일의 가로 셀 수
	DWORD offsetX = (DWORD)(index % m_mapInfo.dwMapLineTileCount) * m_mapInfo.dwTileLineCellCount;
	// 새로 = (타일번호 / 새로타일수) * 한타일의 새로 셀수 * 전체 가로 버텍스 수
	DWORD offsetZ = (DWORD)(index / m_mapInfo.dwMapLineTileCount) * m_mapInfo.dwTileLineCellCount * m_mapInfo.dwMapLineWidth;

	int i=0;
	for(DWORD z=0; z<m_mapInfo.dwTileLineWidth; ++z)
	{
		for(DWORD x=0; x<m_mapInfo.dwTileLineWidth; ++x)
		{
			pTileInfo[index].pVertices[i] =	m_pVertices[z*m_mapInfo.dwMapLineWidth + x + offsetX + offsetZ];
			// 스플래팅 텍스쳐는 각 타일 별로 설정해 준다.
			pTileInfo[index].pVertices[i].t1.x = (float)x / (float)m_mapInfo.dwTileLineCellCount;
			pTileInfo[index].pVertices[i].t1.y = (float)z / (float)m_mapInfo.dwTileLineCellCount;
			++i;
		}
	}
	// 처음에만 생성
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

	// TILEINFO의 세부 정보를 설정한다.
	pTileInfo[index].nLODLevel = 0;			// 초기 생성은 0으로 설정한다.
	pTileInfo[index].nCrackLevel = 0;
	// 전체 지형정점 배열의 인덱스 중 타일의 모서리, 중점 인덱스 값을 설정한다.
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
	// 각 레벨 및 크랙별로 필요한 인덱스 개수가 다르기 때문에 인덱스 개수를 확인해야한다.
	// LOD레벨이 증가 할 때마다 이전 레벨 보다 셀의 가로(세로)수는 1/2배, 총 셀의 수는 1/4배가 된다.
	
	int divideOffset = (int)pow((double)4, LODLevel);			// 셀의 총 수에 나눌 단위
	int nextOffset = (int)pow((double)2, LODLevel);				// 담음 셀까지의 증가 값.
	int totalCell = (m_mapInfo.dwTileLineCellCount *  m_mapInfo.dwTileLineCellCount) / divideOffset;		// 총 셀수
	int halfOffset = nextOffset / 2;				// 셀과 셀의 중앙점을 구하기 위해

	int crackIndex=0;
	for( crackIndex=0; crackIndex<5; ++crackIndex )
	{
		// 0번째 레벨일 경우 크랙 인덱스 버퍼가 필요없으므로 패스
		if( LODLevel == 0 && crackIndex > 0 )
		{
			m_ppIB[LODLevel][crackIndex] = NULL;
			continue;
		}

		// 동일 LODLevel이라도, 0번 크랙은 기본셀 수 지만, 1~4번 크랙들은 한쪽 라인들에 크랙 인덱스가 추가된다.
		// 크랙 인덱스는 각셀 마다 3개의 삼각형이 더 필요 하므로 2+3 = 5개가 된다.
		// 크랙 별 한쪽 라인만 크랙인덱스가 추가 된다. MYINDEXDW구조체는 3개의 인덱스를 포함
		int totalIndex = totalCell * 2;		// 셀당 2개의 삼각형
		if( crackIndex > 0)
		{
			totalIndex += (m_mapInfo.dwTileLineCellCount / nextOffset) * 3;
		}
		m_mapInfo.pTriCnt[LODLevel][crackIndex] = totalIndex;
		
		
		// 인덱스를 구성한다. ( D3DFMT_INDEX16 로 구성한다)
		//D3DFMT_INDEX16은 인덱스의 단위가 16비트 라는 것이다.
		// sTriangle 구조체에서 WORD형으로 선언했으므로 D3DFMT_INDEX16을 사용한
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



// base텍스쳐 변경
void TileTerrain::UpdateBaseTextureFile(LPCTSTR str)
{
	m_mapInfo.strBaseTex.clear();
	m_mapInfo.strBaseTex.insert(0, str);
}

// 스플래팅텍스쳐 변경
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
// 법선벡터 계산 gpg3권의빠른 높이필드 법선계산법
// pos[3]
void TileTerrain::_ComputeNormal(int sX, int eX, int sZ, int eZ)
{
	// GPG 3권에 있는 빠른 높이필드 법선 계산을 참조한다.
	float left	= 0.0f;	// 가운데 정점을 중심으로 왼쪽
	float right	= 0.0f;	// 가운데 정점을 중심으로 오른쪽
	float up	= 0.0f;	// 가운데 정점을 중심으로 윗쪽
	float down	= 0.0f;	// 가운데 정점을 중심으로 아랫쪽
	D3DXVECTOR3 normal(0.0f,0.0f,0.0f);	// 법선 벡터를 저장할 임시벡터

	// o--A--o	// A는 윗쪽
	// B--M--C	// B는 왼쪽, M은 현재정점, C는 오른쪽
	// o--D--o	// D는 아랫쪽
	DWORD z=0;
	DWORD x=0;
	DWORD index = 0;
	for( z=sZ; z<eZ; ++z ) // 보이는 타일 세로(끝쪽 가장자리는 제외)
	{
		for( x=sX; x<eX; ++x ) // 보이는 타일 가로(끝쪽 가장자리는 제외)
		{
			// 외곽라인에 대한 처리가 부족하다. 차후 수정 예정
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

			left	= m_pVertices[ index - 1 ].p.y;				// 현재위치를 중심으로 왼쪽 높이값을 저장
			right	= m_pVertices[ index + 1 ].p.y;				// 오른쪽
			up		= m_pVertices[ index - m_mapInfo.dwMapLineWidth ].p.y;	// 윗쪽
			down	= m_pVertices[ index + m_mapInfo.dwMapLineWidth ].p.y;	// 아랫쪽

			normal	= D3DXVECTOR3( (left-right), 2, (down-up) );	// 직접 외적을 구하지 않고 공식으로 평균을 구함
			D3DXVec3Normalize( &normal, &normal );	// 단위벡터로 만듬

			m_pVertices[index].n = normal;			// 높이맵에 노말벡터 저장
		}
	}
}

//--------------------------------------------------------------------------------------------
// shading으로 그린다
void TileTerrain::DrawShadingTex(BOOL bWireFrame, D3DXMATRIXA16 *pReflect)
{
	m_pd3dDevice->SetFVF(D3DFVF_TILEVERTEX);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &(m_matWorld * (*pReflect)));
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);	

	// 컬링후 보이는 타일만 그리기
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
// 0번 텍스쳐로 전체맵 그린다
void TileTerrain::DrawBaseTex(BOOL bWireFrame, D3DXMATRIXA16 *pReflect)
{
	//m_pd3dDevice->SetMaterial(&m_mtrl);
	
	//if(bWireFrame)
	//	m_pd3dDevice->SetTexture( 0, NULL );
	//else
	
	

	m_pd3dDevice->SetFVF(D3DFVF_TILEVERTEX);
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &(m_matWorld * (*pReflect)));
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);	

	// 컬링후 보이는 타일만 그리기
	std::vector<int>::iterator it;
	for(it = m_vecVisibleTile.begin(); it != m_vecVisibleTile.end(); ++it)
	{
		
		m_pd3dDevice->SetIndices(m_ppIB[m_pTileInfo[*it].nLODLevel][m_pTileInfo[*it].nCrackLevel]);
		m_pd3dDevice->SetStreamSource(0, m_pTileInfo[*it].pVB, 0, sizeof(TILEVERTEX));
		
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_mapInfo.dwTileTotalVert, 0,
														m_pTileInfo[*it].dwTriCount);
		

	}
	
	// wireFrame모드 일경우는 tile의 외곽선을 다른선으로 한번더 그려준다.
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
// Alpha텍스쳐 그리기
// AlphaTexture 로 중복해서 그리는 것이지만 텍스쳐세팅은 TileTerrainMgr의 Splatting 클래스에서 세팅후에 여기서는 인덱스만 그려준다
void TileTerrain::DrawAlphaTex()
{
	m_pd3dDevice->SetFVF(D3DFVF_TILEVERTEX);
	//m_pd3dDevice->SetTransform(D3DTS_WORLD, &(m_matWorld * (*pReflect)));
	m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);	

	// 컬링후 보이는 타일만 그리기
	std::vector<int>::iterator it;
	for(it = m_vecVisibleTile.begin(); it != m_vecVisibleTile.end(); ++it)
	{
		
		m_pd3dDevice->SetIndices(m_ppIB[m_pTileInfo[*it].nLODLevel][m_pTileInfo[*it].nCrackLevel]);
		m_pd3dDevice->SetStreamSource(0, m_pTileInfo[*it].pVB, 0, sizeof(TILEVERTEX));
		
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_mapInfo.dwTileTotalVert, 0,
														m_pTileInfo[*it].dwTriCount);
		
	}
	
	
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	// 멀티텍스쳐 0,1번 셋팅을 사용을 중지 한다.
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	// 알파 사용을 중지 한다.
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
}




//--------------------------------------------------------------------------------------------
// 해당 tile번호의 TL, TR, BL, BR값의 하나의 사각형을 다른 색으로 그려준다.
void TileTerrain::_DrawWireFrameTile(int tile)
{
	static BRUSHVERTEX pVertices[8];
	DWORD dwColor;
	switch( m_pTileInfo[tile].nLODLevel )
	{
		case 0:
			dwColor = 0xFFFAEB78;			// 노랑색
			break;
		case 1:
			dwColor = 0xFFFF8200;			// 주황색
			break;
		case 2:
			dwColor = 0xFFFF82FF;			// 보라색
			break;
		case 3:
			dwColor = 0xFFFF0000;			// 빨간색
			break;
		case 4:
			dwColor = 0xFF80E12A;			// 녹색
			break;
		case 5:
			dwColor = 0xFF0000FF;			// 파랑색
			break;
	}


	// 타일의 4 모서리를 사각형으로 그려준다.
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
// EDGE는 DWORD edge[4]로 사각형 셀의 범위를 저장한다.
long int TileTerrain::_GetPositionInMap(D3DXVECTOR3 &pos, EDGE *edge)
{
	// 전체 맵 범위안에 있는지 확인한다.
	if( pos.x < m_mapInfo.fStartX || pos.z > m_mapInfo.fStartZ || pos.z < m_mapInfo.fEndZ || pos.x > m_mapInfo.fEndX )
	{
		// 범위를 벗어나면 -1를 반환한다.
		return -1;
	}	//인덱스로 변환후에 비교를 해야한다.

	// 위치와 인덱스 비교시는 w/2로 보정을 해줘야한다.
	float fX = ( ((float)m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale /2.0f + pos.x) / m_mapInfo.fCellScale );
	float fZ = ( ((float)m_mapInfo.dwMapLineWidth *  m_mapInfo.fCellScale /2.0f - pos.z) / m_mapInfo.fCellScale );

	//좌상단의 점을 찾기 위해 마지막 한줄값은 범위 밖이다.
	if( 0 > fX || fX >= (float)(m_mapInfo.dwMapLineCellCount) ||
		0 > fZ || fZ >= (float)(m_mapInfo.dwMapLineCellCount) )
	{
		return -1;
	}
	
	long int offsetX = (DWORD)::floorf(fX);						// floorf는 소숫점을 없애버린다.
	long int offsetZ = (DWORD)::floorf(fZ);
	
	long int cellIndex = (offsetX + offsetZ * m_mapInfo.dwMapLineWidth );	

	edge->vTL = m_pVertices[cellIndex].p;									// TL
	edge->vTR = m_pVertices[cellIndex + 1].p;								// TR
	edge->vBL = m_pVertices[cellIndex + m_mapInfo.dwMapLineWidth].p;		// BL
	edge->vBR = m_pVertices[cellIndex + m_mapInfo.dwMapLineWidth + 1].p;	// BR

	return cellIndex;
}

//--------------------------------------------------------------------------------------------
// pos가 포함된 좌상단 셀의 사각형과 pos를 비교해서 가장가까운 셀을 찾는다.
// 그리고 그 셀을 중심으로 주변 4개의 사각형을 구한다.(QUADEDGE)
DWORD TileTerrain::_GetNearPositionInCell(D3DXVECTOR3 &pos, DWORD cell, EDGE *edge, QUADEDGE *quadEdge)
{
	DWORD nearCell = 0;
	float offset = m_mapInfo.fCellScale/2.0f;	// 셀의 반값.

	// 가까운 셀을 찾는다.
	// TL과 TR중 어디에 가까운가?
	if( (pos.x - edge->vTL.x) <= offset )
	{	// TL쪽에 가깝다면
		// 위쪽과 아래쪽중 어디에 가까운가?
		if( (pos.z - edge->vTL.z) >= -offset )		// 위쪽
			nearCell = cell;
		else										// 아래쪽
			nearCell = cell + m_mapInfo.dwMapLineWidth;
	}
	else											// 오른쪽
	{
		if( (pos.z - edge->vTL.z) >= -offset )		// 위쪽
			nearCell = cell + 1;
		else										// 아래쪽
			nearCell = cell + m_mapInfo.dwMapLineWidth + 1;
	}

	// nearCell를 중심으로 4개의 사각형을 구성해야한다.
	//	topLeft ----- top ----- topRight
	//		|			|			|
	//		|			|			|
	//	  left ------nearCell--- right	
	//		|			|			|
	//		|			|			|
	//	bottomLeft----bottom-----bottomRight
	// 하지만, 격자형 맵의 상단, 하단, 좌측, 우측 라인의 점이 중점이 될경우 범위가 벗어나게된다.
	// 그래서 각 모서리에 있는 점들은 보정을 해줘야하는데
	// 0-1---------7-8    (0->10, 1->10, 7->16, 8->16) 
	// 9-10       16-17   (9->10, 17->16)
	// . .         . .
	// . .         . .
	// 63-64      70-71   (63->64, 71->70)
	// 72-73------79-80   (72->64, 73->64, 79->70, 80->70)
	// 이런식으로 보정을 해줘서 사각범위가 맵을 벗어나지 않도록 만든다.


	if( nearCell == 0 )																// 좌상단
		nearCell = nearCell + 1 + m_mapInfo.dwMapLineWidth;
	else if( nearCell == m_mapInfo.dwMapLineCellCount )								// 우상단
		nearCell = nearCell + m_mapInfo.dwMapLineWidth - 1;	
	else if( nearCell == m_mapInfo.dwMapLineWidth*(m_mapInfo.dwMapLineCellCount) )	// 좌하단
		nearCell = nearCell - m_mapInfo.dwMapLineWidth + 1;
	else if( nearCell == m_mapInfo.dwMapTotalVert - 1 )								// 우하단
		nearCell = nearCell - m_mapInfo.dwMapLineWidth - 1;
	else if( nearCell>0 && nearCell<m_mapInfo.dwMapLineCellCount )					// 상단
		nearCell += m_mapInfo.dwMapLineWidth;
	else if( nearCell > m_mapInfo.dwMapLineWidth*(m_mapInfo.dwMapLineCellCount) &&
			nearCell < m_mapInfo.dwMapTotalVert )									// 하단
		nearCell -= m_mapInfo.dwMapLineWidth;
	else if( nearCell % m_mapInfo.dwMapLineWidth == m_mapInfo.dwMapLineCellCount 
			&& nearCell !=m_mapInfo.dwMapLineWidth && nearCell != m_mapInfo.dwMapTotalVert - 1)			// 우단
		nearCell -= 1;
	else if( nearCell % m_mapInfo.dwMapLineWidth == 0 && nearCell != 0
				&& nearCell != m_mapInfo.dwMapLineWidth*(m_mapInfo.dwMapLineCellCount))		// 좌단
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
// Pick 정보 받아서 현재 쿼드트리의 한 삼각형을 찾아서 m_vPickTri 정보에 넣는다
void TileTerrain::PickGround(HPickPtr Pick)
{
	
	// 픽킹시작점
	D3DXVECTOR3 vOrig = Pick->m_pickRay._origin;
	vOrig.y = 0.0f;

	// 픽킹 끝점(광선의 길이를 우선 1000으로 제한한다.
	D3DXVECTOR3 vDest = Pick->m_pickRay._origin + Pick->m_pickRay._direction * 1000.0f;
	vDest.y = 0.0f;
	
	// 시작점과 끝점으로 부터 방향 벡터를 만든다.
	D3DXVECTOR3 vDist = vDest - vOrig;
	D3DXVec3Normalize(&vDist, &vDist);

	m_bIsPicking = false;		// 픽킹 상태 초기화
	long int index = -1;
	D3DXVECTOR3 nextPos(0.0f, 0.0f, 0.0f);
	for(int i=0; i<m_mapInfo.dwMapLineWidth*3; ++i)
	{
		nextPos = vDist * (m_mapInfo.fCellScale * i) + vOrig;
		EDGE edge;
		QUADEDGE quadEdge;
		index = _GetPositionInMap(nextPos, &edge);		// nextPos가 포함된 셀의 범위를 구한다.		
		// EDGE를 통해서 범위를 확인한다. 맵을 벗어났다면 계속 수행할 이유가 없기 때문에
		if( index == -1 )
			continue;
		_GetNearPositionInCell(nextPos, index, &edge, &quadEdge);	// 픽킹테스트를 할 4개의 사각형의 범위를 구한다.
		float fDist = 0.0f;

		// 픽킹테스트
		for(int j=0; j<4; ++j)
		{
			// 상단 삼각형
			if( Pick->IntersectTriangle( quadEdge.corner[j].vTL, quadEdge.corner[j].vTR, quadEdge.corner[j].vBL, fDist) )
			{
				m_vPickingPos = Pick->m_pickRay._origin + Pick->m_pickRay._direction*fDist;
				m_bIsPicking = true;

				m_vPickTri[0] = quadEdge.corner[j].vTL;
				m_vPickTri[1] = quadEdge.corner[j].vTR;
				m_vPickTri[2] = quadEdge.corner[j].vBL;

				break;
			}
			// 하단 삼각형
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
			D3DXVECTOR3 p(0.0f, 0.0f, 0.0f);					// 브러쉬 범위의 정점
			D3DXVECTOR3 m(0.0f, 0.0f, 0.0f);					// 지형 배열 중에서 픽킹된 지점의 위치

			// 로컬좌표계의 정점을 텍스쳐좌표계의 텍셀단위로 변경이 필요함
			// 1정점당 몇 픽셀인가? 픽셀의 너비 / 정점의 너비
			float vertexPerPixel = (float)ALPHA_TEXTURE_SIZE / (float)m_mapInfo.dwMapLineWidth;
			// 브러쉬 반지름이 몇픽셀인가? 수정되어야할 픽셀의 범위계산을 위해
			float editOutSize = m_brushOutSize * vertexPerPixel;
			float editInSize = m_brushInSize * vertexPerPixel;
			float smooth = editOutSize - editInSize;				// 부드럽게 칠해야할 길이
	
			// 전체 맵을 기준으로 변경해서 비교한다.
			// 픽킹된 위치는 로컬 좌표로 변환되어있다.
			// 지형은 중점이 0,0 이기 때문에, 픽킹된 지점을 텍스쳐(좌상단이 0,0)와 비교를 위해 변형
			m.x = (float)(m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f + m_vPickingPos.x)/m_mapInfo.fCellScale;
			m.z = (float)(m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f - m_vPickingPos.z)/m_mapInfo.fCellScale;
			// 픽킹정점을 텍스쳐 좌표계 u, v로 변경 정점 / 너비
			m.x = m.x / m_mapInfo.dwMapLineCellCount;
			m.z = m.z / m_mapInfo.dwMapLineCellCount;

			Pick->SetTextureCoord( m.x, m.z, editInSize, editOutSize);


			break;			// 픽킹이되었다면 종료
		}
		index = -1;
	}
}


//--------------------------------------------------------------------------------------------
// 픽킹된 삼각형 그린다
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

//마우스 위치에 원을 그린다
void TileTerrain::BrushCricleDraw(int count)
{
	float radian = D3DX_PI*2/count;					// 회전각 라디안
	float brushOutSize = m_brushOutSize * m_mapInfo.fCellScale;			// 브러쉬 크기 반지름
	float brushInSize = m_brushInSize * m_mapInfo.fCellScale;

	BRUSHVERTEX brushOutLine[2];					// 두점을 D3DTS_LINELIST를 통해 그려나간다.
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

	// 지형의 시작점을 원점으로 이동하는 변환
	x = ((float)m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f) + x;
	z = ((float)m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f) - z;				// z값이 아래로 증가 하게 만든다.
	x = (float)(x / m_mapInfo.fCellScale);
	z = (float)(z / m_mapInfo.fCellScale);					

	// 마우스 위치가 맵을 넘어가면 0.0f로 셋팅
	if( 0.0f >= x || x >= (float)(m_mapInfo.dwMapLineCellCount) ||
		0.0f >= z || z >= (float)(m_mapInfo.dwMapLineCellCount) )
	{
		return 0.0f;
	}

	long int nx = (long int)::floorf(x);						// floorf는 소숫점을 없애버린다.
	long int nz = (long int)::floorf(z);

	//  A   B
    //  *---*
    //  | / |
    //  *---*
    //  C   D (1,1)
	float A = m_pVertices[nx + nz*m_mapInfo.dwMapLineWidth].p.y;	// nx, nz가 포함된 위치의 셀 정점의 Y값을 얻어온다.
	float B = m_pVertices[nx+1 + nz*m_mapInfo.dwMapLineWidth].p.y;
	float C = m_pVertices[nx + (nz+1)*m_mapInfo.dwMapLineWidth].p.y;
	float D = m_pVertices[nx+1 + (nz+1)*m_mapInfo.dwMapLineWidth].p.y;

	// nx, nz가 포함된 셀이 0,0 원점의 위치가 되도록 가상으로 설정한다.(0~1사이 값이된다)
	float dx = x - nx;
	float dz = z - nz;

	// 벡터의 합의 기하학적 의미와 선형보간을 통해 x,z위치의 y값을 얻어온다.
	// 셀은 가로 세로 1의 정사각형이다. 즉 정사각형의 대각선은 y=x이고, dx와 dz의 비교를 통해서 삼각형의 위치를 판별.
	if( dz + dx < 1.0f )  // 위쪽 삼각형
	{
		float uy = B - A;
		float vy = C - A;
		height = A + _Lerp(0.0f, uy, dx) + _Lerp(0.0f, vy, dz);
	}
	else			// 아래쪽 삼각형
	{
		float uy = C - D;
		float vy = B - D;
		height = D + _Lerp(0.0f, uy, 1.0f - dx) + _Lerp(0.0f, vy, 1.0f - dz);
	}
	return height;
}



// 카메라 look값을 통해 각 타일별 _GetLODLevel를 통해 LODLevel 결정
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
	else		// LOD 미 적용이라면 강제로 LODLeve과 크랙을 할당
	{
		for(int i=0; i<m_mapInfo.dwMapTotalTileCount; ++i)
		{
			m_pTileInfo[i].nLODLevel = 0;
			m_pTileInfo[i].nCrackLevel = 0;
			m_pTileInfo[i].dwTriCount = m_mapInfo.pTriCnt[0][0];
		}
	}

}


// look = 카메라의 Look, index = 타일번호
// 타일의 중점과 카메라의 거리를 통해 TILEINFO에 직접 LOD를 설정한다.
void TileTerrain::_GetLODLevel(D3DXVECTOR3 look, int index)
{
	// 실제 벡터의 크기를 비교할 경우 원형 모양의 거리가 결정된다. 그럴경우 인덱스의 크랙이 16개필요하다.
	// 그래서 계산의 단순화를 위해, x,z축을 각각 비교해서 거리가 먼쪽을 기준 값으로 한다.
	D3DXVECTOR3 distX(0.0f, 0.0f, 0.0f);
	distX.x = look.x - m_pVertices[m_pTileInfo[index].dwCenter].p.x;
	float LenX = D3DXVec3Length(&distX);
	D3DXVECTOR3 distZ(0.0f, 0.0f, 0.0f);
	distZ.z = look.z - m_pVertices[m_pTileInfo[index].dwCenter].p.z;
	float LenZ = D3DXVec3Length(&distZ);
	float len;				// 타일의 중점으로 부터 가장 멀리 떨어진 축의 길이
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


// 주변 타일과 비교해서 크랙을 설정한다.
void TileTerrain::_SetupCrack(int index)
{
	int currentIndex = index;
	int temp[4];				// 좌상우하 인덱스값
	int tile = m_mapInfo.dwMapLineTileCount;
	temp[0] = (currentIndex % tile == 0 ) ? currentIndex : (currentIndex - 1);		// LEFT
	temp[1] = (currentIndex / tile == 0 ) ? currentIndex : (currentIndex - tile);	// TOP
	temp[2] = (currentIndex % tile == (tile-1) ) ? currentIndex : (currentIndex + 1);	// RIGHT
	temp[3] = (currentIndex / tile == (tile-1) ) ? currentIndex : (currentIndex + tile);	//BOTTOM
	m_pTileInfo[currentIndex].nCrackLevel = 0;	// 기본 크랙 설정
	for(int i=0; i<4; ++i)
	{
		// 현타일이 주변 타일 보다 크다는건, 크랙이 필요하다는 뜻이다.
		// LOD레벨을 사각형범위로 제한했기 때문에, LOD가 다를 경우는 한쪽 면뿐이다.
		if( m_pTileInfo[currentIndex].nLODLevel > m_pTileInfo[temp[i]].nLODLevel )
		{
			m_pTileInfo[currentIndex].nCrackLevel = i+1;		// 크랙0은 기본 이기때문에 +1를 해준다.
			break;
		}
	}
	// 크랙이 바뀌면, 삼각형의 개수도 바뀐다.
	// 맵정보에 미리 계산된 [LODLeve][CrackLevel]를 통해 업데이트해준다.
	m_pTileInfo[currentIndex].dwTriCount = 
		m_mapInfo.pTriCnt[m_pTileInfo[currentIndex].nLODLevel][m_pTileInfo[currentIndex].nCrackLevel];
}



void TileTerrain::EditHeightInCricle(float frameDelta)
{
	const float EPSILON = 0.001f;

	if(!m_bIsPicking)
		return ;
	D3DXVECTOR3 p(0.0f, 0.0f, 0.0f);							// 브러쉬 범위의 정점
	D3DXVECTOR3 m(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 s = m_vPickingPos / m_mapInfo.fCellScale;		// 픽킹된 중앙 값을 배열의 인덱스와 비교를 위해
	s.y = 0.0f;													

	// 픽킹된 위치는 로컬 좌표로 변환되어있다.
	// 지형 정보는 중점이 0,0 이기 때문에, 픽킹된 지점을 지형 배열의 인덱스로 이용하기 위해서는 변형이 필요.
	m.x = (float)(m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f + m_vPickingPos.x)/m_mapInfo.fCellScale;
	m.z = (float)(m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale/2.0f - m_vPickingPos.z)/m_mapInfo.fCellScale;

	// 원형 브러쉬가 포함되는 정사각형 범위
	int start_X = (int)(m.x) - m_brushOutSize; 
	int start_Z = (int)(m.z) - m_brushOutSize;
	int end_X = (int)(m.x) + m_brushOutSize;
	int end_Z = (int)(m.z) + m_brushOutSize;

	if( start_X < 0 ) { start_X = 0; }
	if( start_Z < 0 ) { start_Z = 0; }
	if( end_X >= m_mapInfo.dwMapLineWidth ) { end_X = m_mapInfo.dwMapLineCellCount; }	// 0부터 시작하기때문에 끝은 -1
	if( end_Z >= m_mapInfo.dwMapLineWidth ) { end_Z = m_mapInfo.dwMapLineCellCount; }	// 0부터 시작하기때문에 끝은 -1
	
	// 테스트
//	g_pLog->Log(TEXT(" start  X:%ld  Z:%ld    end  X:%ld   Z:%ld"), start_X, start_Z, end_X, end_Z);
//	g_pLog->Log(TEXT(" width : %d     heihgt : %d"), end_X - start_X, end_Z - start_Z);

	for(int z=start_Z; z<=end_Z; ++z)
	{
		for(int x=start_X; x<=end_X; ++x)
		{
			DWORD mapIndex = z*m_mapInfo.dwMapLineWidth + x;
			p = m_pVertices[ mapIndex].p / m_mapInfo.fCellScale;
			p.y = 0.0f;										// 1셀크기의 바둑판배열 비교를위해 y값 무시
//			D3DXVECTOR3 lenVec = p - (D3DXVECTOR3(m_vPickingPos.x, 0.0f, m_vPickingPos.z)/m_mapInfo.fCellScale);
//			float len = D3DXVec3Length(&lenVec);
			float len = (p.x - s.x)*(p.x - s.x) + (p.z - s.z)*(p.z - s.z);
	
			// 차후 다른 알고리즘을 적용해서 자연스럽게 증가되게 만든다.
			// 중심점으로 부터 거리가 멀수록 증가값이 작아지게
			float y = -1.0f/m_brushOutSize * len * m_fSlopeRate + m_brushOutSize;

			if(len < m_brushOutSize * m_brushOutSize)  // 정점이 원안에 있다면
			{
				switch(m_nUpDownMode)
				{
				case 0:				// 업모드
					m_pVertices[ mapIndex].p.y += (y)* m_heightRate*frameDelta;
					m_pVertices[ mapIndex].n.y = m_pVertices[ mapIndex].p.y;
					break;

				case 1:				// 다운
					m_pVertices[ mapIndex].p.y -= (y)* m_heightRate*frameDelta;
					m_pVertices[ mapIndex].n.y = m_pVertices[ mapIndex].p.y;
					break;

				case 2:				// 평탄화
					if( abs(m_pVertices[ mapIndex].p.y - m_vPickingPos.y) > EPSILON)  // 같지 않다면
					{
						// 픽킹점보다 낮다면 올리고
						if( m_pVertices[ mapIndex].p.y - m_vPickingPos.y < EPSILON )
						{
							if( (m_pVertices[ mapIndex ].p.y + (y)* m_heightRate*frameDelta) - m_vPickingPos.y < EPSILON )
								m_pVertices[ mapIndex ].p.y += (y)* m_heightRate*frameDelta;
							else
								m_pVertices[ mapIndex].p.y = m_vPickingPos.y;
						}
						else	// 픽킹점보다 높다면 내린다.
						{
							if( (m_pVertices[ mapIndex].p.y - (y)* m_heightRate*frameDelta) - m_vPickingPos.y > EPSILON )
								m_pVertices[ mapIndex].p.y -= (y)* m_heightRate*frameDelta;
							else
								m_pVertices[ mapIndex].p.y = m_vPickingPos.y;
						}
						m_pVertices[ mapIndex].n.y = m_pVertices[ mapIndex].p.y;
					}
					break;

				case 3:		// 4나 다른 숫자면 그냥 평지화
					if( abs(m_pVertices[ mapIndex].p.y) > EPSILON)  // 0즉 평지가 아니라면
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
			}	// if 원안에 있는가? end	
		}	// for x end
	}	// for z end

	// 원형 브러쉬가 포함될 사격형의 타일들을 업데이트 해준다.
	// 각 타일의 테두리 점들은 서로 중복적용이 되어있다. 윗면, 좌측면 같은 경우는 이전 타일로 계산되기 때문에 상관
	// 없지만, 우측, 하단 면들은 그 타임 타일들도 적용이 되어야 한다. 그래서 우측, 하단은 +1크기만큼 증가시켜서
	// 범위를 검색할경우 이런 문제가 해결가능하다.
	if( end_X < m_mapInfo.dwMapLineCellCount )
		end_X += 1;
	if( end_Z < m_mapInfo.dwMapLineCellCount )
		end_Z += 1;
	// 법선 벡터 계산
	// 어차피 법선은 주변 정점의 영향을 받기 때문에, 전체적으로 계산을 해줘야한다.
	// 그래서 대충 계산후, 최종적으로 다시 계산을 할 필요가 있다.
	_ComputeNormal( start_X, end_X, start_Z, end_Z);

	DWORD indexTL = start_X + start_Z * m_mapInfo.dwMapLineWidth;
	DWORD indexTR = end_X + start_Z * m_mapInfo.dwMapLineWidth;
	DWORD indexBL = start_X + end_Z * m_mapInfo.dwMapLineWidth;

	int tileTL = _GetPositionInTile( m_pVertices[indexTL].p );
	int tileTR = _GetPositionInTile( m_pVertices[indexTR].p );
	int tileBL = _GetPositionInTile( m_pVertices[indexBL].p );


	// 타일의 증가 횟수(즉, 행의 개수)
	int offsetZ = (tileBL - tileTL)/m_mapInfo.dwMapLineTileCount;
	
	for(int z = 0; z <= offsetZ; ++z)
	{
		for(int x = tileTL; x <= tileTR; ++x)
		{
			int index = x + z * m_mapInfo.dwMapLineTileCount;
			_CreateVB(index, m_pTileInfo);
		}
	}
		// 테스트
//	for(DWORD i=0; i<m_mapInfo.dwMapTotalTileCount; i++)
//	{
//		_CreateVB(i, m_pTileInfo);
//	}
}


//  pos가 포함된 타일 값을 반환한다.
int TileTerrain::_GetPositionInTile(D3DXVECTOR3 &pos)
{
	// 전체 맵 범위안에 있는지 확인한다.
	if( pos.x < m_mapInfo.fStartX || pos.z > m_mapInfo.fStartZ || pos.z < m_mapInfo.fEndZ || pos.x > m_mapInfo.fEndX )
	{
		// 범위를 벗어나면 -1를 반환한다.
		return -1;
	}	//인덱스로 변환후에 비교를 해야한다.

	// 타일의 간격은 셀*크기 이다. 따라서 0<=  pos/간격 < 1 이라면 0번타일이 된다.
	// 위치와 인덱스 비교시는 -w/2로 보정을 해줘야한다.
	int offsetX =  (int)( (m_mapInfo.dwMapLineWidth * m_mapInfo.fCellScale /2.0f + pos.x) / 
															(m_mapInfo.fCellScale * m_mapInfo.dwTileLineCellCount) );
	int offsetZ =  (int)( (m_mapInfo.dwMapLineWidth *  m_mapInfo.fCellScale /2.0f - pos.z) / 
															(m_mapInfo.fCellScale * m_mapInfo.dwTileLineCellCount) );
	// 타일의 번호는 0부터 tile -1 개이다. 이경우 우측 라인과, 하단 라인의 줄 값은 타일의 개수와 같은 값이된다.
	// 그래서 범위를 벗어날수가 있으니, 줄 값이 타일과 같다면 tile-1로 설정해 준다.
	if( offsetX == m_mapInfo.dwMapLineTileCount )
		offsetX = m_mapInfo.dwMapLineTileCount - 1;
	if( offsetZ == m_mapInfo.dwMapLineTileCount )
		offsetZ = m_mapInfo.dwMapLineTileCount - 1;
	
	return (int)( offsetX + offsetZ * m_mapInfo.dwMapLineTileCount );
}

