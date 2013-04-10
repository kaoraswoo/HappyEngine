//-------------------------------------------------------------------------------------
// 해골책 참조한 쿼드트리(현재 쓰이지 않음)
// 
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _ZQUADTREE_H_
#define _ZQUADTREE_H_

#include "define.h"
#include "Utility\ZFrustum.h"


/**
 * @brief QuadTree의 베이스 클래스
 *
 */ 
class ZQuadTree
{
	/// 쿼드트리에 보관되는 4개의 코너값에 대한 상수값
	enum			CornerType { CORNER_TL, CORNER_TR, CORNER_BL, CORNER_BR };

	/// 쿼드트리와 프러스텀간의 관계
	enum			QuadLocation { FRUSTUM_OUT = 0,				/// 프러스텀에서 완전벗어남
								   FRUSTUM_PARTIALLY_IN = 1,	/// 프러스텀에 부분포함
								   FRUSTUM_COMPLETELY_IN = 2,	/// 프러스텀에 완전포함
								   FRUSTUM_UNKNOWN = -1 };		/// 모르겠음(^^;)

private:
	ZQuadTree*	m_pChild[4];		/// QuadTree의 4개의 자식노드

	int			m_nCenter;			/// QuadTree에 보관할 첫번째 값
	int			m_nCorner[4];		/// QuadTree에 보관할 두번째 값
									///    TopLeft(TL)      TopRight(TR)
									///              0------1
									///              |      |
									///              |      |
									///              2------3
									/// BottomLeft(BL)      BottomRight(BR)
	BOOL		m_bCulled;			/// 프러스텀에서 컬링된 노드인가?
	float		m_fRadius;			/// 노드를 감싸는 경계구(bounding sphere)의 반지름
private:
	/// 자식 노드를 추가한다.
	ZQuadTree*	_AddChild( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );

	/// 4개의 코너값을 셋팅한다.
	BOOL		_SetCorners( int nCornerTL, int nCornerTR, int nCornerBL, int nCornerBR );

	/// Quadtree를 4개의 하위 트리로 부분분할(subdivide)한다.
	BOOL		_SubDivide();		// Quadtree를 subdivide한다.

	/// 출력할 폴리곤의 인덱스를 생성한다.
	int			_GenTriIndex( int nTris, LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio );

	/// 메모리에서 쿼드트리를 삭제한다.
	void		_Destroy();

	/// 두점 사이의 거리를 구한다.
	float		_GetDistance( D3DXVECTOR3* pv1, D3DXVECTOR3* pv2 ) 
				{
					return D3DXVec3Length( &(*pv2 - *pv1) );
				}

	/// 카메라와 현재 노드와의 거리값을 기준으로 LOD값을 구한다.
	int			_GetLODLevel( TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio )
				{
					float d = _GetDistance( (D3DXVECTOR3*)(pHeightMap+m_nCenter), pCamera );
					return max( (int)( d * fLODRatio ), 1 );
				}

	/// 현재 노드가 LOD등급으로 볼때  출력이 가능한 노드인가?
	BOOL		_IsVisible( TERRAINVERTEX* pHeightMap, D3DXVECTOR3* pCamera, float fLODRatio )
				{ 
					return ( (m_nCorner[CORNER_TR] - m_nCorner[CORNER_TL]) <= _GetLODLevel( pHeightMap, pCamera, fLODRatio ) ); 
				}

	/// 현재노드가 프러스텀에 포함되는가?
	int			_IsInFrustum( TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum );

	/// _IsInFrustum()함수의 결과에 따라 프러스텀 컬링 수행
	void		_FrustumCull( TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum );
public:

				/// 최초 루트노드 생성자
				ZQuadTree( int cx, int cy );

				/// 하위 자식노드 생성자
				ZQuadTree( ZQuadTree* pParent );

				/// 소멸자
				~ZQuadTree();

	/// QuadTree를 구축한다.
	BOOL		Build( TERRAINVERTEX* pHeightMap );	// 현재의 정보를 기준으로 QuadTree를 build한다.

	///	삼각형의 인덱스를 만들고, 출력할 삼각형의 개수를 반환한다.
	int			GenerateIndex( LPVOID pIndex, TERRAINVERTEX* pHeightMap, ZFrustum* pFrustum, float fLODRatio );
};

#endif // _ZQUADTREE_H_
