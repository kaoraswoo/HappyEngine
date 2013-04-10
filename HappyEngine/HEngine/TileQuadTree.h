#ifndef _TILEQUADTREE_H_
#define _TILEQUADTREE_H_

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

#include "define.h"
#include "TileTerrain.h"

typedef boost::shared_ptr<TileTerrain> TileTerrainPtr;

// Ÿ���� ��ȣ�� ����Ʈ���� ����� ����.
// ���� ����Ʈ���� 2^n+1���� ������ ���� ������ִ�.(�ذ�å����)
// ������ Ÿ���� ������ 2^n+1���� �Ѵٸ�, ����Ʈ���� �ߺ� ���� ����.
// ������ ���� ����Ʈ���� ������, ������ �ߺ��� �ɼ��� ����. ������ Ÿ���� �ߺ��� �� �ʿ䰡 ����.
// ���� Ÿ���� ������ 2^n���� �����Ѵ�.(�ߺ��� ���� ���� ���� ����)
// Ÿ�� ��ȣ�� �������� �޸� Ȧ�� �ǹ̰� �ֱ� ������ �ߺ��� ������ ����.
// 0----1----2----3			4*4 Ÿ���� ���� Ʈ�� ������ a�� ǥ�õ� �𼭸� �簢�� 4���� 16���� Ÿ�� ��ȣ�� ǥ���� ����
// | a  |	 |	a |			�̷��� ����Ʈ���� ������ ���ٰ�, ������ ���̻� ��Ȱ�� �Ұ����Ұ��(TR - TL) < 1 �� ���	
// 4----5----6----7			���������� �� 4���� ���� ���� Ÿ���� �ѹ��� �ɰ����ָ�Ǵµ�, ���⼭ 
// |	|	 |	  |			4���� �ڽĳ�忡 TL(TR, BL, BR)�� �ϳ����������ؼ� �ڽĳ���� TL���� �����ϰ� 
// 8----9---10---11			�ڽĳ���� ������ ���� -1�� �����Ѵ�.
// | a  |	 |	a |			�׸��� ��尪�� �̿��� ��� �� ����� TL���� �̿��ϸ�ȴ�.
// 12--13---14---15			���� �߾Ӱ� ������� 5���� BR���� �߽��� �ȴ�.
//
// 5----6  ������ Ÿ�� ��ȣ���� �� �𼭸������� �ߺ��Ǵ� �������� �����Եȴ�. �׷��� �߾Ӱ���
// |    |  5�� Ÿ���� ���ϴ� = 6��Ÿ���� ���ϴ� = 9�� Ÿ���� ���� = 10�� Ÿ���� �»��  ��� ������ �����̵ȴ�.
// 9---10  ���� �߾Ӱ��� �׳� �»�� �� 5�� Ÿ���� ���ϴ� ������ ������Ų��.
//
//
class TileQuadTree
{
	enum	EDGE_TYPE	{ TL, TR, BL, BR };
	enum	INTERSECT	{ ALL_OUT=0, PART_IN, ALL_IN };		// ����ü �ø��� ���� ����

private:
	int				m_nCenter;			// ����ü �ø���, �߽����� �̵��ؼ� �� �׽�Ʈ�� �Ѵ�.
										// �߾� �ε����� BR���� �߽ɰ��̴�. ��, �ٳ���� ���� �״�� �߽ɰ����
	int				m_nEdge[4];
	int				m_nTileNum;			// ��� ����� ���ؼ� �˰��־�� ���ϴ�.
	BOOL			m_bCulled;			// �ø����� Ȯ��, �ٳ�� �̸鼭 false�� ���� �������� ����̴�.
	float			m_fRadius;			// ���� ��带 ���δ� ���� ������ ��

	TileQuadTree*		m_pChild[4];

private:
	void			_SetCorners( int _TL, int _TR, int _BL, int _BR );
	TileQuadTree*	_AddChild( int _TL, int _TR=-1, int _BL=-1, int _BR=-1 );	// Leaf�϶� �ٸ��μ� ��� -1
	BOOL			_SetDivide();

	BOOL			_IsLeaf() { return (m_nEdge[TR] == -1) ? true : false; }
	int				_IsInFrustum( TileTerrainPtr pGround, ZFrustum* pFrustum  );		// �ش� ����� ����ü ���� ���¸� ��ȯ�Ѵ�.
	void			_FrustumCull( TileTerrainPtr pGround, ZFrustum* pFrustum  );		// �� ����� m_bCulled ���¸� �����Ѵ�.
	void			_SearchDrawTile( TileTerrainPtr pGround );	// ���̴� ���(����ҳ��)�� ã�Ƽ� m_vecVisibleTile�� �����Ѵ�.

public:
	TileQuadTree( int tileNum );					// tileNum�� ������ ¦�� ���� ����
	TileQuadTree( TileQuadTree* pParent );			// �ڽĳ�� ������ �̿�
	~TileQuadTree();

	void SetInfo( int tileNum );

	void BuildQuadTree( TileTerrainPtr pGround );

	// �ø��� ������ ������ Ÿ�� �ε����� ���Ѵ�.
	void BuildDrawTiles( TileTerrainPtr pGround, ZFrustum* pFrustum );
};





#endif