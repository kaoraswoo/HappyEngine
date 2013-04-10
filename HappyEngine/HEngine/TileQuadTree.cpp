#include "TileQuadTree.h"

// 최초 생성자
TileQuadTree::TileQuadTree(int tileNum)
{
	SetInfo(tileNum);
}

void TileQuadTree::SetInfo( int tileNum )
{
	m_nTileNum = tileNum;
	m_bCulled = false;
	m_nCenter = tileNum;
	m_fRadius = 0.0f;

	m_nEdge[TL] = 0;
	m_nEdge[TR] = tileNum - 1;
	m_nEdge[BL] = (tileNum - 1)*tileNum;
	m_nEdge[BR] = tileNum*tileNum - 1;
	int offset = (tileNum - 1)/2;					// 타일 번호는 0부터 시작하기 때문에 
	m_nCenter = offset*tileNum + offset;			// 중앙값은 좌상단타일의 우하단 정점으로 한다.

	for(int i=0; i<4; ++i)
	{
		m_pChild[i] = NULL;
	}
}

// 자식노드를 만들기위한 생성자
TileQuadTree::TileQuadTree(TileQuadTree *pParent)
{
	m_nCenter	= 0;
	m_bCulled	= false;
	m_fRadius	= 0.0f;
	m_nTileNum	= pParent->m_nTileNum;
	
	for(int i=0; i<4; ++i)
	{
		m_pChild[i] = NULL;
		m_nEdge[i] = 0;				// 코너값은 _SetCorners()함수가 설정한다.
	}
}

TileQuadTree::~TileQuadTree()
{
	for(int i=0; i<4; i++)
	{
		DEL(m_pChild[i]);
	}
}

// m_Edge값을 설정한다.
void TileQuadTree::_SetCorners(int _TL, int _TR, int _BL, int _BR)
{
	m_nEdge[TL] = _TL;
	m_nEdge[TR] = _TR;
	m_nEdge[BL] = _BL;
	m_nEdge[BR] = _BR;
	
	if( _IsLeaf() )
		m_nCenter = _TL;			// 마지막 잎 노드 일경우 TL 값이 타일 이기 때문에
	else
	{
		int offset =  ((_TL + _TR)/2) - _TL;
		m_nCenter = _TL + offset + offset*m_nTileNum;
	}
}


TileQuadTree* TileQuadTree::_AddChild(int _TL, int _TR, int _BL, int _BR)
{
	TileQuadTree *pChild = new TileQuadTree(this);

	pChild->_SetCorners(_TL, _TR, _BL, _BR);

	return pChild;
}

// 설정된 코너값을 기준으로 4개의 자식노드를 생성한다.
BOOL TileQuadTree::_SetDivide()
{
	if( _IsLeaf() )		// 더이상 분활이 불가능하다면
		return false;
	
	// 전체 타일 갯수 4개짜리 나누기 즉 0 - 1 일 경우
	if( m_nEdge[TR] - m_nEdge[TL] <= 1 )
	{	// 바로 잎노드를 생성한다.
		m_pChild[TL] = _AddChild(m_nEdge[TL]);
		m_pChild[TR] = _AddChild(m_nEdge[TR]);
		m_pChild[BL] = _AddChild(m_nEdge[BL]);
		m_pChild[BR] = _AddChild(m_nEdge[BR]);
		return true;
	}

	/*  TL----1----2----TR		  1 = (TL+TR)/2   ,  2 = 1 + offset 
		 | a  |	   |  a |		  offset = 1 - TL;	
		w1----0----0----e1		  w1 = TL + offset*tile   w2 = w1 + tile  	
		 |	  |    |    |			<---- 이줄은 항상 tile*1의 간격이다.
		w2----0----0----e2		  e1 = TR + offset*tile   e2 = e1 + tile
		 | a  |	   |  a |
		BL----3----4---BR		  3 = (BL+BR)/2   ,  4 = 3 + offset  	
		
		1-2, 3-4 의 간격 역시 항상 1의 간격이다.
	*/
	int top = (m_nEdge[TL] + m_nEdge[TR])/2;
	int bottom = (m_nEdge[BL] + m_nEdge[BR])/2;
	int offset = top - m_nEdge[TL];
	int w1 = m_nEdge[TL] + offset*m_nTileNum;
	int w2 = w1 + m_nTileNum;
	int e1 = m_nEdge[TR] + offset*m_nTileNum;
	int e2 = e1 + m_nTileNum;

	m_pChild[TL] = _AddChild( m_nEdge[TL],		   top,			 w1,  w1+offset);
	m_pChild[TR] = _AddChild(		top+1, m_nEdge[TR], e1 - offset,	     e1);
	m_pChild[BL] = _AddChild(		   w2,  w2+offset, m_nEdge[BL],		 bottom);
	m_pChild[BR] = _AddChild( e2 - offset,			e2,   bottom+1, m_nEdge[BR]);
	return true;
}

// 쿼드트리를 만든다.
void TileQuadTree::BuildQuadTree(TileTerrainPtr pGround)
{
	BOOL bDivide = _SetDivide();
	DWORD _TL;
	DWORD _BR;
	
	// 현재 노드의 반지름 값을 계산을 한다.
	// 쿼드트리에는 타일의 번호가 들어가있다. 해당 노드의 실제 타일의 TL, BR값을 얻어내서 반지름을 구한다.
	if( bDivide )
	{
		// 나눠진다면, TL노드의 TL 점과, BR노드의 BR점이 전체 범위가 된다.
		_TL = (pGround->GetTileInfo())[ m_nEdge[TL]].dwEDGE[TL];
		_BR = (pGround->GetTileInfo())[ m_nEdge[BR]].dwEDGE[BR];
	}
	else
	{
		// 잎노드 라면 즉 마지막 타일 값이라면, TL값이 타일 번호 이기때문
		_TL = (pGround->GetTileInfo())[ m_nEdge[TL] ].dwEDGE[TL];		// 전체 정점배열중 해당 타일의 TL의 인덱스
		_BR = (pGround->GetTileInfo())[ m_nEdge[TL] ].dwEDGE[BR];
	}
	
	D3DXVECTOR3 vTL = (pGround->GetTileVertex())[_TL].p; vTL.y = 0.0f;	// x,z값만 비교하면 된다.
	D3DXVECTOR3 vBR = (pGround->GetTileVertex())[_BR].p; vBR.y = 0.0f;
	D3DXVECTOR3 v = vBR - vTL;
	m_fRadius = D3DXVec3Length(&v)/2.0f;				// 지름 / 2 로 반지름을 구한다.

	if(bDivide)
	{
		m_pChild[TL]->BuildQuadTree(pGround);
		m_pChild[TR]->BuildQuadTree(pGround);
		m_pChild[BL]->BuildQuadTree(pGround);
		m_pChild[BR]->BuildQuadTree(pGround);
	}
}

int TileQuadTree::_IsInFrustum(TileTerrainPtr pGround, ZFrustum* pFrustum )
{
	BOOL b[4];			// 구의 4모서리점의 포함 여부
	BOOL bSphere;		// 구의 포함 여부
	DWORD index_CEN;
	
	if( _IsLeaf() )
		index_CEN	= pGround->GetTileInfo()[m_nCenter].dwCenter;			// 잎노드라면 그냥 중앙값
	else
		index_CEN = pGround->GetTileInfo()[m_nCenter].dwEDGE[BR];			// 잎노드가 아니라면 BR값이 전체의 중앙값이된다.

	bSphere = pFrustum->IsInSphere( (D3DXVECTOR3*)&(pGround->GetTileVertex()[index_CEN].p), m_fRadius);
	if( !bSphere ) return TileQuadTree::ALL_OUT;

	if( _IsLeaf() ) return TileQuadTree::ALL_IN;		// 잎 노드는 그냥 구 검사만한다.
	
	// 4개의 모서리점의 포함 여부를 검사한다.
	DWORD index_TL = pGround->GetTileInfo()[ m_nEdge[TL] ].dwEDGE[TL];
	DWORD index_TR = pGround->GetTileInfo()[ m_nEdge[TR] ].dwEDGE[TR];
	DWORD index_BL = pGround->GetTileInfo()[ m_nEdge[BL] ].dwEDGE[BL];
	DWORD index_BR = pGround->GetTileInfo()[ m_nEdge[BR] ].dwEDGE[BR];
	
	//b[0] = pFrustum->IsIn( (D3DXVECTOR3*)(pHeightMap+m_nCorner[0]) );
	b[0] = pFrustum->IsIn( (D3DXVECTOR3*)&(pGround->GetTileVertex()[index_TL].p) );
	b[1] = pFrustum->IsIn( (D3DXVECTOR3*)&(pGround->GetTileVertex()[index_TR].p) );
	b[2] = pFrustum->IsIn( (D3DXVECTOR3*)&(pGround->GetTileVertex()[index_BL].p) );
	b[3] = pFrustum->IsIn( (D3DXVECTOR3*)&(pGround->GetTileVertex()[index_BR].p) );

	if( b[0] + b[1] + b[2] + b[3] == 4 )
		return TileQuadTree::ALL_IN;
	
	return TileQuadTree::PART_IN;
}

void TileQuadTree::_FrustumCull(TileTerrainPtr pGround , ZFrustum* pFrustum )
{
	int ret;		// 포함 여부, 완전 포함, 부분 포함, 완전 미 포함
	
	ret = _IsInFrustum(pGround, pFrustum);

	switch(ret)
	{
	case TileQuadTree::ALL_OUT:
		m_bCulled = true;
		return;
		
	case TileQuadTree::ALL_IN:
		m_bCulled = false;
		return;

	case TileQuadTree::PART_IN:
		m_bCulled = false;
		break;
	}

	if(m_pChild[0])	m_pChild[0]->_FrustumCull(pGround, pFrustum);
	if(m_pChild[0])	m_pChild[1]->_FrustumCull(pGround, pFrustum);
	if(m_pChild[0]) m_pChild[2]->_FrustumCull(pGround, pFrustum);
	if(m_pChild[0])	m_pChild[3]->_FrustumCull(pGround, pFrustum);
}

void TileQuadTree::_SearchDrawTile(TileTerrainPtr pGround)
{
	// 컬링된 노드라면 출력할 필요가 없으므로 바로 리턴한다.
	if( m_bCulled )
	{
		m_bCulled = false;		// 초기화 후
		return ;
	}

	// 컬링이 안된 노드 이면서 잎노드라면, 출력해야할 노드 이기때문에 저장한다.
	if( _IsLeaf() )
	{
		pGround->m_vecVisibleTile.push_back(m_nEdge[TL]);
		return;
	}
	
	// 잎노드가 아니라면 자식노드를 재귀적으로 호출하며 반복 수행한다.
	m_pChild[0]->_SearchDrawTile(pGround);
	m_pChild[1]->_SearchDrawTile(pGround);
	m_pChild[2]->_SearchDrawTile(pGround);
	m_pChild[3]->_SearchDrawTile(pGround);
}

void TileQuadTree::BuildDrawTiles(TileTerrainPtr pGround, ZFrustum* pFrustum)
{
	// 컬링한다.
	_FrustumCull(pGround, pFrustum);

	// m_vecVisibleTile 컨테이너를 비운다.
	pGround->m_vecVisibleTile.clear();

	// 보이는 타일의 인덱스를 등록한다.
	_SearchDrawTile(pGround);
}