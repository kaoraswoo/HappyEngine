#include "TileQuadTree.h"

// ���� ������
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
	int offset = (tileNum - 1)/2;					// Ÿ�� ��ȣ�� 0���� �����ϱ� ������ 
	m_nCenter = offset*tileNum + offset;			// �߾Ӱ��� �»��Ÿ���� ���ϴ� �������� �Ѵ�.

	for(int i=0; i<4; ++i)
	{
		m_pChild[i] = NULL;
	}
}

// �ڽĳ�带 ��������� ������
TileQuadTree::TileQuadTree(TileQuadTree *pParent)
{
	m_nCenter	= 0;
	m_bCulled	= false;
	m_fRadius	= 0.0f;
	m_nTileNum	= pParent->m_nTileNum;
	
	for(int i=0; i<4; ++i)
	{
		m_pChild[i] = NULL;
		m_nEdge[i] = 0;				// �ڳʰ��� _SetCorners()�Լ��� �����Ѵ�.
	}
}

TileQuadTree::~TileQuadTree()
{
	for(int i=0; i<4; i++)
	{
		DEL(m_pChild[i]);
	}
}

// m_Edge���� �����Ѵ�.
void TileQuadTree::_SetCorners(int _TL, int _TR, int _BL, int _BR)
{
	m_nEdge[TL] = _TL;
	m_nEdge[TR] = _TR;
	m_nEdge[BL] = _BL;
	m_nEdge[BR] = _BR;
	
	if( _IsLeaf() )
		m_nCenter = _TL;			// ������ �� ��� �ϰ�� TL ���� Ÿ�� �̱� ������
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

// ������ �ڳʰ��� �������� 4���� �ڽĳ�带 �����Ѵ�.
BOOL TileQuadTree::_SetDivide()
{
	if( _IsLeaf() )		// ���̻� ��Ȱ�� �Ұ����ϴٸ�
		return false;
	
	// ��ü Ÿ�� ���� 4��¥�� ������ �� 0 - 1 �� ���
	if( m_nEdge[TR] - m_nEdge[TL] <= 1 )
	{	// �ٷ� �ٳ�带 �����Ѵ�.
		m_pChild[TL] = _AddChild(m_nEdge[TL]);
		m_pChild[TR] = _AddChild(m_nEdge[TR]);
		m_pChild[BL] = _AddChild(m_nEdge[BL]);
		m_pChild[BR] = _AddChild(m_nEdge[BR]);
		return true;
	}

	/*  TL----1----2----TR		  1 = (TL+TR)/2   ,  2 = 1 + offset 
		 | a  |	   |  a |		  offset = 1 - TL;	
		w1----0----0----e1		  w1 = TL + offset*tile   w2 = w1 + tile  	
		 |	  |    |    |			<---- ������ �׻� tile*1�� �����̴�.
		w2----0----0----e2		  e1 = TR + offset*tile   e2 = e1 + tile
		 | a  |	   |  a |
		BL----3----4---BR		  3 = (BL+BR)/2   ,  4 = 3 + offset  	
		
		1-2, 3-4 �� ���� ���� �׻� 1�� �����̴�.
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

// ����Ʈ���� �����.
void TileQuadTree::BuildQuadTree(TileTerrainPtr pGround)
{
	BOOL bDivide = _SetDivide();
	DWORD _TL;
	DWORD _BR;
	
	// ���� ����� ������ ���� ����� �Ѵ�.
	// ����Ʈ������ Ÿ���� ��ȣ�� ���ִ�. �ش� ����� ���� Ÿ���� TL, BR���� ���� �������� ���Ѵ�.
	if( bDivide )
	{
		// �������ٸ�, TL����� TL ����, BR����� BR���� ��ü ������ �ȴ�.
		_TL = (pGround->GetTileInfo())[ m_nEdge[TL]].dwEDGE[TL];
		_BR = (pGround->GetTileInfo())[ m_nEdge[BR]].dwEDGE[BR];
	}
	else
	{
		// �ٳ�� ��� �� ������ Ÿ�� ���̶��, TL���� Ÿ�� ��ȣ �̱⶧��
		_TL = (pGround->GetTileInfo())[ m_nEdge[TL] ].dwEDGE[TL];		// ��ü �����迭�� �ش� Ÿ���� TL�� �ε���
		_BR = (pGround->GetTileInfo())[ m_nEdge[TL] ].dwEDGE[BR];
	}
	
	D3DXVECTOR3 vTL = (pGround->GetTileVertex())[_TL].p; vTL.y = 0.0f;	// x,z���� ���ϸ� �ȴ�.
	D3DXVECTOR3 vBR = (pGround->GetTileVertex())[_BR].p; vBR.y = 0.0f;
	D3DXVECTOR3 v = vBR - vTL;
	m_fRadius = D3DXVec3Length(&v)/2.0f;				// ���� / 2 �� �������� ���Ѵ�.

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
	BOOL b[4];			// ���� 4�𼭸����� ���� ����
	BOOL bSphere;		// ���� ���� ����
	DWORD index_CEN;
	
	if( _IsLeaf() )
		index_CEN	= pGround->GetTileInfo()[m_nCenter].dwCenter;			// �ٳ���� �׳� �߾Ӱ�
	else
		index_CEN = pGround->GetTileInfo()[m_nCenter].dwEDGE[BR];			// �ٳ�尡 �ƴ϶�� BR���� ��ü�� �߾Ӱ��̵ȴ�.

	bSphere = pFrustum->IsInSphere( (D3DXVECTOR3*)&(pGround->GetTileVertex()[index_CEN].p), m_fRadius);
	if( !bSphere ) return TileQuadTree::ALL_OUT;

	if( _IsLeaf() ) return TileQuadTree::ALL_IN;		// �� ���� �׳� �� �˻縸�Ѵ�.
	
	// 4���� �𼭸����� ���� ���θ� �˻��Ѵ�.
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
	int ret;		// ���� ����, ���� ����, �κ� ����, ���� �� ����
	
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
	// �ø��� ����� ����� �ʿ䰡 �����Ƿ� �ٷ� �����Ѵ�.
	if( m_bCulled )
	{
		m_bCulled = false;		// �ʱ�ȭ ��
		return ;
	}

	// �ø��� �ȵ� ��� �̸鼭 �ٳ����, ����ؾ��� ��� �̱⶧���� �����Ѵ�.
	if( _IsLeaf() )
	{
		pGround->m_vecVisibleTile.push_back(m_nEdge[TL]);
		return;
	}
	
	// �ٳ�尡 �ƴ϶�� �ڽĳ�带 ��������� ȣ���ϸ� �ݺ� �����Ѵ�.
	m_pChild[0]->_SearchDrawTile(pGround);
	m_pChild[1]->_SearchDrawTile(pGround);
	m_pChild[2]->_SearchDrawTile(pGround);
	m_pChild[3]->_SearchDrawTile(pGround);
}

void TileQuadTree::BuildDrawTiles(TileTerrainPtr pGround, ZFrustum* pFrustum)
{
	// �ø��Ѵ�.
	_FrustumCull(pGround, pFrustum);

	// m_vecVisibleTile �����̳ʸ� ����.
	pGround->m_vecVisibleTile.clear();

	// ���̴� Ÿ���� �ε����� ����Ѵ�.
	_SearchDrawTile(pGround);
}