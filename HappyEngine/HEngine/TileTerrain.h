#ifndef _TILETERRAIN_H_
#define _TILETERRAIN_H_

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

#include "define.h"
#include "HPick.h"

// ���̴� �ؽ��� �������� �� ����
struct	CVERTEX3D_TEX2		
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;	
	float tu1,tv1;	
	float tu2,tv2; 
};
#define	CVERTEX3D_TEX2_FVF	( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEX2 )


// �� ������
struct EDGE
{
	D3DXVECTOR3 vTL;
	D3DXVECTOR3 vTR;
	D3DXVECTOR3 vBL;
	D3DXVECTOR3 vBR;
};

// ����Ʈ���� �� ������
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
	D3DXVECTOR3 startPos;		// �巹�� ������
	D3DXVECTOR3 endPos;			// �巹�� ������
};

// Ÿ�� ����
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

	DWORD dwEDGE[4];				// ��ü ���� �迭�� Ÿ���� TL, TR, BL, BR ��
	DWORD dwCenter;					// ��ü ���� �迭��Ÿ���� �߾� ��
	int nLODLevel;					// ���� ����� LOD����
	int nCrackLevel;				// ���� ����� ũ��
	DWORD dwTriCount;				// ���� ����� �ε���(�ﰢ��)�� ��

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
		//ar & pVertices;  // ���ؽ��� �ø���������̼��� �ȵǴ� �� Ȯ��
		//ar & pVB;        // ���ؽ��� �ø���������̼��� �ȵǴ� �� Ȯ��
    }

};

// ������
struct MAPINFO
{
public:
	static enum SELTEX { BASE=0, SPLATTING0, SPLATTING1, SPLATTING2, SPLATTING3, HEIGHTMAP };

	BOOL	bUpdateBaseTex;				// �ؽ����� ���� ����
	BOOL	bUpdateHeightMap;			// ����Ʈ�� ���� ����	

	std::string			strSplattingTex[4];			// ���÷��ÿ� �ؽ��� �̸�
	std::string			strBaseTex;					// ���̽� �ؽ���
	std::string			strHeightMap;					// ���̸� �̸�

	WORD	olddwMaxLODLevel;
	WORD	dwMaxLODLevel;				// �� LOD�ܰ�(0�ܰ����)
	WORD	(*pTriCnt)[5];				// [MAXLOD][5] ��ŭ ������ �ﰢ��(�ε���) ���� ����� ���̺�
	WORD	dwTileLineCellCount;		// Ÿ�� �ȿ� �ִ� ���� ����(����) ����   2^n
	WORD	dwTileLineWidth;			// �� Ÿ���� ����(����) ������ dwTileLineCellCountt+1                2^n+1
	WORD	dwTileTotalVert;			// �� Ÿ���� ���� ���� (dwTileLineWidth * dwTileLineWidth)
	WORD	dwMapLineTileCount;			// ���� ���� ������ Ÿ�� ����(����) ���� 2n * 2n ¦���� ����
	WORD	dwMapLineWidth;				// ���� ����(����) ���� (dwMapLineTileCount * dwTileLineCellCount +1)    2^n+1

	WORD	dwMapLineCellCount;			// ���� ���� ������  �� �� (dwMapLineTileCount * dwTileLineCellCount)
	WORD	dwMapTotalTileCount;		// ���� ��ü Ÿ�ϼ�	
	DWORD	dwMapTotalVert;				// ���� ���� ����(dwMapLineWidth * dwMapLineWidth)
	DWORD	dwTileIndxCnt;				// �� Ÿ���� �ε��� ����(dwTileLineCellCount * dwTileLineCellCount * 2)
	
	
	float	fCellScale;					// �� ���� ũ��
	float	fHeightScale;				// ���� ������
	float	fStartX;					// ��ü ���� ���� ��ġ
	float	fStartZ;
	float	fCenterX;					// �߽ɰ�
	float	fCenterZ;
	float	fEndX;						// ��ü ���� �� ��ġ
	float	fEndZ;

	float basic_scale;
	float basic_heightScale;
	int basic_tile;
	int basic_cell;


	std::vector<float>  m_heightlist; // ����Ʈ������ ������ �ִ� ����Ʈ(����/�ε��)

	
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


		ar & 	dwMaxLODLevel;				// �� LOD�ܰ�(0�ܰ����)
		ar & 	(*pTriCnt)[5];				// [MAXLOD][5] ��ŭ ������ �ﰢ��(�ε���) ���� ����� ���̺�
		ar & 	dwTileLineCellCount;		// Ÿ�� �ȿ� �ִ� ���� ����(����) ����   2^n
		ar & 	dwTileLineWidth;			// �� Ÿ���� ����(����) ������ dwTileLineCellCountt+1                2^n+1
		ar & 	dwTileTotalVert;			// �� Ÿ���� ���� ���� (dwTileLineWidth * dwTileLineWidth)
		ar & 	dwMapLineTileCount;			// ���� ���� ������ Ÿ�� ����(����) ���� 2n * 2n ¦���� ����
		ar & 	dwMapLineWidth;				// ���� ����(����) ���� (dwMapLineTileCount * dwTileLineCellCount +1)    2^n+1

		ar & 	dwMapLineCellCount;			// ���� ���� ������  �� �� (dwMapLineTileCount * dwTileLineCellCount)
		ar & 	dwMapTotalTileCount;		// ���� ��ü Ÿ�ϼ�	
		ar & 	dwMapTotalVert;				// ���� ���� ����(dwMapLineWidth * dwMapLineWidth)
		ar & 	dwTileIndxCnt;				// �� Ÿ���� �ε��� ����(dwTileLineCellCount * dwTileLineCellCount * 2)
		ar & 	fCellScale;					// �� ���� ũ��
		ar & 	fHeightScale;				// ���� ������
		ar & 	fStartX;					// ��ü ���� ���� ��ġ
		ar & 	fStartZ;
		ar & 	fCenterX;					// �߽ɰ�
		ar & 	fCenterZ;
		ar & 	fEndX;						// ��ü ���� �� ��ġ
		ar & 	fEndZ;
    }

};


// Tile Terrain Ŭ����
class TileTerrain
{
	RenderMgr* TileTerrainRender;						//������
	
	typedef boost::shared_ptr<HPick> HPickPtr;			//��ŷ���� ������

public:
	//--------------------����, �Ҹ�, ����------------------------------------
	TileTerrain(){};
	TileTerrain(RenderMgr* SceneRen);
	~TileTerrain();
	void _Destroy();
	void _ResetGround();

	// ó�� ���� ����
	HRESULT CreateGround(LPCTSTR strHeight, float scale, float heightScale, int tile, int cell);
	// �������� ����
	HRESULT _BuildHeightMap(int cell, int tile, float scale);							// cell, tile �������� ����
	// BMP ���̸����� ����(��, ���� 32ĭ ����)
	HRESULT _BuildBMPHeightMap(LPCTSTR pfileName, int cell, float scale, float heightScale);    	

	HRESULT _CreateVB(int index, TILEINFO *pTileInfo );		// index��° TILEINFO�� �����Ѵ�.
	HRESULT _CreateIB(int LODLevel);						// LODLevel�� �ش��ϴ� �ε��� ���۸� ����

	// base�ؽ��� ��� ����
	void UpdateBaseTextureFile(LPCTSTR str);
	// ���÷����ؽ��� ��� ����
	void UpdateTextureFile(int index, LPCTSTR str);
	const char* GetBaseTextureName();
	const char* GetTextureName(int i);

	//LOD
	void SetupLODLevel(D3DXVECTOR3 look);		
	void _GetLODLevel(D3DXVECTOR3 look, int index);		// ī�޶�Look�� Ÿ�Ϲ�ȣ �� ���� Ÿ���� LOD������ �����Ѵ�.
	void _SetupCrack(int index);						// index = Ÿ�Ϲ�ȣ, �����¿� Ÿ�ϰ� ���ؼ� ũ���� �����Ѵ�.

	float GetLODrate(){return m_fLODRate;}
	void SetLODrate(float rate){m_fLODRate = rate;}

	//���̸� ���� Set
	void SetHeight();

	// MAPINFO ���� 
	void _SetupMapInfo(int cell, int tile, float scale, float heightScale = 1.0f);	// BuildHeightMap, _BuilBMPHeightMap ���ο��� ȣ��
	
	

	// ���� �����
	float _Lerp(float a, float b, float t) { return a -(a*t) + (b*t); }			// ��������
	void _ComputeNormal(int sX, int eX, int sZ, int eZ);

	//��ŷ
	void PickGround(HPickPtr Pick);
	DWORD _GetNearPositionInCell(D3DXVECTOR3 &pos, DWORD cell, EDGE *edge, QUADEDGE *quadEdge);
	long int _GetPositionInMap(D3DXVECTOR3 &pos, EDGE *edge);
	int	_GetPositionInTile(D3DXVECTOR3 &pos);			// pos�� ���Ե� Ÿ�� �ε����� ��ȯ�Ѵ�.
	void BrushTriangleDraw(D3DXCOLOR color);
	void BrushCricleDraw(int count);

	// ���̸ʿ��� x, z��ġ�� ���̰� ���ϱ�
	float GetHeightY(float x, float z);					

	// ���� ����
	void EditHeightInCricle(float frameDelta);


	// pReflect�� �ݻ�� ���ҽ� Ȱ��. ���ô� ��������� �Ѱ��ش�.
	void DrawBaseTex(BOOL bWireFrame, D3DXMATRIXA16 *pReflect);	
	
	// pReflect�� �ݻ�� ���ҽ� Ȱ��. ���ô� ��������� �Ѱ��ش�.
	void DrawShadingTex(BOOL bWireFrame, D3DXMATRIXA16 *pReflect);

	void DrawAlphaTex();
	void _DrawWireFrameTile(int tile);

	MAPINFO*				GetMAPINFO() { return &m_mapInfo; }
	const TILEINFO*			GetTileInfo() { return m_pTileInfo; }
	const TILEVERTEX*		GetTileVertex() { return m_pVertices; };

public:
	LPDIRECT3DDEVICE9		m_pd3dDevice;
	LPDIRECT3DINDEXBUFFER9	(*m_ppIB)[5];					// *�� 5���� IB�� ������ ��������Ʈ ����(������ �⺻+4���� ũ��)

	TILEVERTEX				*m_pVertices;					// ��ü ���� ���� ����
	TRIINDEX				*m_pIndices;
	D3DXMATRIXA16			m_matWorld;						//  ���� ���� ���

	//---------------- �� -------------------------------------------------
	MAPINFO					m_mapInfo;						// �� ���� ����
	TILEINFO				*m_pTileInfo;					// Ÿ�� ����


	BOOL					m_bIsLOD;						// LOD ���� ����
	float					m_fLODRate;						// LOD�Ÿ� ��������

	//-----------��ŷ----------------------------------------------------
	BOOL				m_bIsPicking;								// ��ŷ ����
	D3DXVECTOR3			m_vPickingPos;								// ��ŷ ��ġ
	D3DXVECTOR3			m_vPickTri[3];								// ��ŷ�� �ﰢ�� �׽�Ʈ��
	DWORD				m_dPickTile;								// ��ŷ�� Ÿ���� �ε���

	
	//-----------�귯��------------------------------------------------------
	int		m_nUpDownMode;	// �귯�� ���ٿ� ��� 0��, 1�ٿ�, 2��źȭ, 3����,4������,5��絵,
							// 6���ΰ���, 7��������, 8���ΰ���
							
	BOOL	m_bOutBrush;
	BOOL	m_bInBrush;
	int		m_brushOutSize;										// �ܺ� �귯�� ũ��
	int		m_brushInSize;										// ���� �귯�� ũ��
	DWORD	m_dwBrushOutColor;									// �ܺ� �귯�� ��
	DWORD	m_dwBrushInColor;									// ���� �귯�� ��
	int		m_heightRate;									    // ���� ���� ��
	float	m_fSlopeRate;										// ��絵
	int		m_nTexFillMode;									// �ؽ��� ���� / ���� ���

	float	m_fLineFixed;									// ���ΰ��� �귯��
	float	m_fLineAdd;										// �������� �귯��
	BOOL		m_bDragBrush;									// ��絵 �귯�� ���� ����
	RampRect	m_RampRect;									// ��絵 �귯���� ����
	

public:
	
	// ����Ʈ���� ������ Ÿ�� ��ȣ �� �ø� �� ������ �Ǵ� Ÿ�� ��ȣ ����.
	std::vector<int>	m_vecVisibleTile;	// ���̴� Ÿ�Ϲ�ȣ��
	std::vector<int>	m_vecTexDrawTile;	// �ؽ��� ���÷����� ���� �ٽ� �׷��� Ÿ�Ϲ�ȣ��


	// �ø���������̼����� ������ ����(����� ���̸ʸ� �����ϹǷ�, �ؽ��İ�� ���� �ʿ�)
	void SaveTile( std::string filepath );
	void LoadTile( std::string filepath );
};


#endif