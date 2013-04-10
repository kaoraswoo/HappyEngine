#ifndef _TILETERRAIN_H_
#define _TILETERRAIN_H_

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

#include "define.h"
#include "HPick.h"

// 쉐이더 텍스쳐 블랜딩에서 쓸 형식
struct	CVERTEX3D_TEX2		
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;	
	float tu1,tv1;	
	float tu2,tv2; 
};
#define	CVERTEX3D_TEX2_FVF	( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEX2 )


// 각 꼭지점
struct EDGE
{
	D3DXVECTOR3 vTL;
	D3DXVECTOR3 vTR;
	D3DXVECTOR3 vBL;
	D3DXVECTOR3 vBR;
};

// 쿼드트리의 각 꼭지점
struct QUADEDGE
{
	EDGE corner[4];				// TL, TR, BL, BR
};


struct RampRect
{
	D3DXVECTOR2	leftT;
	D3DXVECTOR2	leftB;
	D3DXVECTOR2	rightT;
	D3DXVECTOR2	rightB;
	D3DXVECTOR3 startPos;		// 드레그 시작점
	D3DXVECTOR3 endPos;			// 드레그 종료점
};

// 타일 정보
struct TILEINFO
{
public:
	TILEVERTEX *pVertices;
	LPDIRECT3DVERTEXBUFFER9 pVB;

	TILEINFO() : pVertices(NULL), pVB(NULL) { }
	~TILEINFO()
	{
		DELS(pVertices);
		REL(pVB);
	}

	DWORD dwEDGE[4];				// 전체 지형 배열중 타일의 TL, TR, BL, BR 값
	DWORD dwCenter;					// 전체 지형 배열중타일의 중앙 값
	int nLODLevel;					// 현재 적용될 LOD레벨
	int nCrackLevel;				// 현재 적용될 크랙
	DWORD dwTriCount;				// 현재 출력할 인덱스(삼각형)의 수

private:
	friend class boost::serialization::access;
    template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
    {
        ar & dwEDGE;
        ar & dwCenter;
        ar & nLODLevel;
		ar & nCrackLevel;
		ar & dwTriCount;
		//ar & pVertices;  // 버텍스는 시리얼라이제이션이 안되는 것 확인
		//ar & pVB;        // 버텍스는 시리얼라이제이션이 안되는 것 확인
    }

};

// 맵정보
struct MAPINFO
{
public:
	static enum SELTEX { BASE=0, SPLATTING0, SPLATTING1, SPLATTING2, SPLATTING3, HEIGHTMAP };

	BOOL	bUpdateBaseTex;				// 텍스쳐의 변경 여부
	BOOL	bUpdateHeightMap;			// 하이트맵 변경 여부	

	std::string			strSplattingTex[4];			// 스플래팅용 텍스쳐 이름
	std::string			strBaseTex;					// 베이스 텍스쳐
	std::string			strHeightMap;					// 높이맵 이름

	WORD	olddwMaxLODLevel;
	WORD	dwMaxLODLevel;				// 총 LOD단계(0단계부터)
	WORD	(*pTriCnt)[5];				// [MAXLOD][5] 만큼 각각의 삼각형(인덱스) 수를 계산한 테이블
	WORD	dwTileLineCellCount;		// 타일 안에 있는 셀의 가로(새로) 갯수   2^n
	WORD	dwTileLineWidth;			// 한 타일의 가로(새로) 정점수 dwTileLineCellCountt+1                2^n+1
	WORD	dwTileTotalVert;			// 한 타일의 정점 갯수 (dwTileLineWidth * dwTileLineWidth)
	WORD	dwMapLineTileCount;			// 맵의 한쪽 라인의 타일 가로(새로) 갯수 2n * 2n 짝수로 제한
	WORD	dwMapLineWidth;				// 맵의 가로(새로) 길이 (dwMapLineTileCount * dwTileLineCellCount +1)    2^n+1

	WORD	dwMapLineCellCount;			// 맵의 한쪽 라인의  셀 수 (dwMapLineTileCount * dwTileLineCellCount)
	WORD	dwMapTotalTileCount;		// 맵의 전체 타일수	
	DWORD	dwMapTotalVert;				// 맵의 정점 갯수(dwMapLineWidth * dwMapLineWidth)
	DWORD	dwTileIndxCnt;				// 한 타일의 인덱스 갯수(dwTileLineCellCount * dwTileLineCellCount * 2)
	
	
	float	fCellScale;					// 한 셀의 크기
	float	fHeightScale;				// 높이 보정값
	float	fStartX;					// 전체 맵의 시작 위치
	float	fStartZ;
	float	fCenterX;					// 중심값
	float	fCenterZ;
	float	fEndX;						// 전체 맵의 끝 위치
	float	fEndZ;

	float basic_scale;
	float basic_heightScale;
	int basic_tile;
	int basic_cell;


	std::vector<float>  m_heightlist; // 하이트정보만 가지고 있는 리스트(저장/로드용)

	
	MAPINFO() : pTriCnt(NULL)  
	{
		bUpdateBaseTex = false;
		bUpdateHeightMap = false;
	}
	~MAPINFO()
	{
		DELS(pTriCnt);
	}

	private:
	friend class boost::serialization::access;
    template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
    {

		ar & basic_scale;
		ar & basic_heightScale;
		ar & basic_tile;
		ar & basic_cell;



        ar & m_heightlist;
		ar & strBaseTex;
		ar & strSplattingTex;


		ar & 	dwMaxLODLevel;				// 총 LOD단계(0단계부터)
		ar & 	(*pTriCnt)[5];				// [MAXLOD][5] 만큼 각각의 삼각형(인덱스) 수를 계산한 테이블
		ar & 	dwTileLineCellCount;		// 타일 안에 있는 셀의 가로(새로) 갯수   2^n
		ar & 	dwTileLineWidth;			// 한 타일의 가로(새로) 정점수 dwTileLineCellCountt+1                2^n+1
		ar & 	dwTileTotalVert;			// 한 타일의 정점 갯수 (dwTileLineWidth * dwTileLineWidth)
		ar & 	dwMapLineTileCount;			// 맵의 한쪽 라인의 타일 가로(새로) 갯수 2n * 2n 짝수로 제한
		ar & 	dwMapLineWidth;				// 맵의 가로(새로) 길이 (dwMapLineTileCount * dwTileLineCellCount +1)    2^n+1

		ar & 	dwMapLineCellCount;			// 맵의 한쪽 라인의  셀 수 (dwMapLineTileCount * dwTileLineCellCount)
		ar & 	dwMapTotalTileCount;		// 맵의 전체 타일수	
		ar & 	dwMapTotalVert;				// 맵의 정점 갯수(dwMapLineWidth * dwMapLineWidth)
		ar & 	dwTileIndxCnt;				// 한 타일의 인덱스 갯수(dwTileLineCellCount * dwTileLineCellCount * 2)
		ar & 	fCellScale;					// 한 셀의 크기
		ar & 	fHeightScale;				// 높이 보정값
		ar & 	fStartX;					// 전체 맵의 시작 위치
		ar & 	fStartZ;
		ar & 	fCenterX;					// 중심값
		ar & 	fCenterZ;
		ar & 	fEndX;						// 전체 맵의 끝 위치
		ar & 	fEndZ;
    }

};


// Tile Terrain 클래스
class TileTerrain
{
	RenderMgr* TileTerrainRender;						//렌더러
	
	typedef boost::shared_ptr<HPick> HPickPtr;			//픽킹위한 포인터

public:
	//--------------------생성, 소멸, 해제------------------------------------
	TileTerrain(){};
	TileTerrain(RenderMgr* SceneRen);
	~TileTerrain();
	void _Destroy();
	void _ResetGround();

	// 처음 지형 생성
	HRESULT CreateGround(LPCTSTR strHeight, float scale, float heightScale, int tile, int cell);
	// 지형정보 생성
	HRESULT _BuildHeightMap(int cell, int tile, float scale);							// cell, tile 선택으로 구현
	// BMP 높이맵으로 구현(단, 셀은 32칸 고정)
	HRESULT _BuildBMPHeightMap(LPCTSTR pfileName, int cell, float scale, float heightScale);    	

	HRESULT _CreateVB(int index, TILEINFO *pTileInfo );		// index번째 TILEINFO를 생성한다.
	HRESULT _CreateIB(int LODLevel);						// LODLevel에 해당하는 인덱스 버퍼를 생성

	// base텍스쳐 경로 변경
	void UpdateBaseTextureFile(LPCTSTR str);
	// 스플래팅텍스쳐 경로 변경
	void UpdateTextureFile(int index, LPCTSTR str);
	const char* GetBaseTextureName();
	const char* GetTextureName(int i);

	//LOD
	void SetupLODLevel(D3DXVECTOR3 look);		
	void _GetLODLevel(D3DXVECTOR3 look, int index);		// 카메라Look과 타일번호 를 통해 타일의 LOD레벨을 설정한다.
	void _SetupCrack(int index);						// index = 타일번호, 상하좌우 타일과 비교해서 크랙을 설정한다.

	float GetLODrate(){return m_fLODRate;}
	void SetLODrate(float rate){m_fLODRate = rate;}

	//높이맵 정보 Set
	void SetHeight();

	// MAPINFO 정보 
	void _SetupMapInfo(int cell, int tile, float scale, float heightScale = 1.0f);	// BuildHeightMap, _BuilBMPHeightMap 내부에서 호출
	
	

	// 내부 연산용
	float _Lerp(float a, float b, float t) { return a -(a*t) + (b*t); }			// 선형보간
	void _ComputeNormal(int sX, int eX, int sZ, int eZ);

	//픽킹
	void PickGround(HPickPtr Pick);
	DWORD _GetNearPositionInCell(D3DXVECTOR3 &pos, DWORD cell, EDGE *edge, QUADEDGE *quadEdge);
	long int _GetPositionInMap(D3DXVECTOR3 &pos, EDGE *edge);
	int	_GetPositionInTile(D3DXVECTOR3 &pos);			// pos가 포함된 타일 인덱스를 반환한다.
	void BrushTriangleDraw(D3DXCOLOR color);
	void BrushCricleDraw(int count);

	// 높이맵에서 x, z위치의 높이값 구하기
	float GetHeightY(float x, float z);					

	// 높이 편집
	void EditHeightInCricle(float frameDelta);


	// pReflect는 반사맵 구할시 활용. 평상시는 단위행렬을 넘겨준다.
	void DrawBaseTex(BOOL bWireFrame, D3DXMATRIXA16 *pReflect);	
	
	// pReflect는 반사맵 구할시 활용. 평상시는 단위행렬을 넘겨준다.
	void DrawShadingTex(BOOL bWireFrame, D3DXMATRIXA16 *pReflect);

	void DrawAlphaTex();
	void _DrawWireFrameTile(int tile);

	MAPINFO*				GetMAPINFO() { return &m_mapInfo; }
	const TILEINFO*			GetTileInfo() { return m_pTileInfo; }
	const TILEVERTEX*		GetTileVertex() { return m_pVertices; };

public:
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	LPDIRECT3DINDEXBUFFER9	(*m_ppIB)[5];					// *당 5개의 IB를 가지는 이중포인트 선언(레벨당 기본+4방향 크랙)

	TILEVERTEX				*m_pVertices;					// 전체 지형 정점 정보
	TRIINDEX				*m_pIndices;
	D3DXMATRIXA16			m_matWorld;						//  지형 월드 행렬

	//---------------- 맵 -------------------------------------------------
	MAPINFO					m_mapInfo;						// 맵 세부 사항
	TILEINFO				*m_pTileInfo;					// 타일 정보


	BOOL					m_bIsLOD;						// LOD 적용 여부
	float					m_fLODRate;						// LOD거리 결정단위

	//-----------픽킹----------------------------------------------------
	BOOL				m_bIsPicking;								// 픽킹 여부
	D3DXVECTOR3			m_vPickingPos;								// 픽킹 위치
	D3DXVECTOR3			m_vPickTri[3];								// 픽킹된 삼각형 테스트용
	DWORD				m_dPickTile;								// 픽킹된 타일의 인덱스

	
	//-----------브러쉬------------------------------------------------------
	int		m_nUpDownMode;	// 브러쉬 업다운 모드 0업, 1다운, 2평탄화, 3평지,4스무스,5경사도,
							// 6라인고정, 7라인증가, 8라인감소
							
	BOOL	m_bOutBrush;
	BOOL	m_bInBrush;
	int		m_brushOutSize;										// 외부 브러쉬 크기
	int		m_brushInSize;										// 내부 브러쉬 크기
	DWORD	m_dwBrushOutColor;									// 외부 브러쉬 색
	DWORD	m_dwBrushInColor;									// 내부 브러쉬 색
	int		m_heightRate;									    // 높이 증가 율
	float	m_fSlopeRate;										// 경사도
	int		m_nTexFillMode;									// 텍스쳐 생성 / 삭제 모드

	float	m_fLineFixed;									// 라인고정 브러쉬
	float	m_fLineAdd;										// 라인증감 브러쉬
	BOOL		m_bDragBrush;									// 경사도 브러시 시작 여부
	RampRect	m_RampRect;									// 경사도 브러시의 범위
	

public:
	
	// 쿼드트리로 구성된 타일 번호 중 컬링 후 렌더링 되는 타일 번호 저장.
	std::vector<int>	m_vecVisibleTile;	// 보이는 타일번호들
	std::vector<int>	m_vecTexDrawTile;	// 텍스쳐 스플래팅을 통해 다시 그려줄 타일번호들


	// 시리얼라이제이션으로 맵정보 저장(현재는 높이맵만 저장하므로, 텍스쳐경로 저장 필요)
	void SaveTile( std::string filepath );
	void LoadTile( std::string filepath );
};


#endif