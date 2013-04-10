#ifndef _TILEQUADTREE_H_
#define _TILEQUADTREE_H_

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

#include "define.h"
#include "TileTerrain.h"

typedef boost::shared_ptr<TileTerrain> TileTerrainPtr;

// 타일의 번호를 쿼드트리로 만들기 위해.
// 보통 쿼드트리는 2^n+1개로 만들어야 쉽게 만들수있다.(해골책참고)
// 하지만 타일의 개수를 2^n+1개로 한다면, 쿼드트리에 중복 값이 들어간다.
// 정점에 대해 쿼드트리를 만들경우, 정점은 중복이 될수록 좋다. 하지만 타일은 중복이 될 필요가 없다.
// 따라서 타일의 개수를 2^n개로 제한한다.(중복이 없는 가장 좋은 상태)
// 타일 번호는 정점과는 달리 홀로 의미가 있기 때문에 중복될 이유가 없다.
// 0----1----2----3			4*4 타일의 쿼드 트리 구성시 a가 표시된 모서리 사각형 4개로 16개의 타일 번호를 표현이 가능
// | a  |	 |	a |			이렇게 쿼드트리를 구성해 가다가, 마지막 더이상 분활이 불가능할경우(TR - TL) < 1 일 경우	
// 4----5----6----7			최종적으로 각 4개의 점을 가진 타일을 한번더 쪼개어주면되는데, 여기서 
// |	|	 |	  |			4개의 자식노드에 TL(TR, BL, BR)를 하나씩만전달해서 자식노드의 TL값에 저장하고 
// 8----9---10---11			자식노드의 마지막 값은 -1로 설정한다.
// | a  |	 |	a |			그리고 노드값을 이용할 경우 잎 노드의 TL값만 이용하면된다.
// 12--13---14---15			또한 중앙값 같은경우 5번의 BR값이 중심이 된다.
//
// 5----6  인접한 타일 번호들은 각 모서리점에서 중복되는 정점값을 가지게된다. 그래서 중앙값은
// |    |  5번 타일의 우하단 = 6번타일의 좌하단 = 9번 타일의 우상단 = 10번 타일의 좌상단  모두 동일한 정점이된다.
// 9---10  이중 중앙값은 그냥 좌상단 즉 5번 타일의 우하단 값으로 고정시킨다.
//
//
class TileQuadTree
{
	enum	EDGE_TYPE	{ TL, TR, BL, BR };
	enum	INTERSECT	{ ALL_OUT=0, PART_IN, ALL_IN };		// 절두체 컬링의 포함 상태

private:
	int				m_nCenter;			// 절두체 컬링시, 중심점을 이동해서 구 테스트를 한다.
										// 중앙 인덱스의 BR값이 중심값이다. 단, 잎노드일 경우는 그대로 중심값사용
	int				m_nEdge[4];
	int				m_nTileNum;			// 노드 계산을 위해서 알고있어야 편하다.
	BOOL			m_bCulled;			// 컬링상태 확인, 잎노드 이면서 false인 노드는 보여지는 노드이다.
	float			m_fRadius;			// 현재 노드를 감싸는 원의 반지름 값

	TileQuadTree*		m_pChild[4];

private:
	void			_SetCorners( int _TL, int _TR, int _BL, int _BR );
	TileQuadTree*	_AddChild( int _TL, int _TR=-1, int _BL=-1, int _BR=-1 );	// Leaf일때 다른인수 모두 -1
	BOOL			_SetDivide();

	BOOL			_IsLeaf() { return (m_nEdge[TR] == -1) ? true : false; }
	int				_IsInFrustum( TileTerrainPtr pGround, ZFrustum* pFrustum  );		// 해당 노드의 절두체 포함 상태를 반환한다.
	void			_FrustumCull( TileTerrainPtr pGround, ZFrustum* pFrustum  );		// 각 노드의 m_bCulled 상태를 결정한다.
	void			_SearchDrawTile( TileTerrainPtr pGround );	// 보이는 노드(출력할노드)를 찾아서 m_vecVisibleTile에 저장한다.

public:
	TileQuadTree( int tileNum );					// tileNum의 값으로 짝수 값을 전달
	TileQuadTree( TileQuadTree* pParent );			// 자식노드 생성에 이용
	~TileQuadTree();

	void SetInfo( int tileNum );

	void BuildQuadTree( TileTerrainPtr pGround );

	// 컬링후 실제로 보여질 타일 인덱스를 구한다.
	void BuildDrawTiles( TileTerrainPtr pGround, ZFrustum* pFrustum );
};





#endif