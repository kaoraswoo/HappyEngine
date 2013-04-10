//-------------------------------------------------------------------------------------
//! Pvs 추가 //PVS 포털 생성 클래스
//! BSP 부분에서 수정
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef BSP_H
#define BSP_H

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif


#pragma warning(disable : 4996)//fopen쓸때 warning에러 없앨려고...


// BSP 헤더부분 (퀘이크소스 발췌)
enum BSP_TYPES
{
	enEntities=0,
	enTextures,
	enPlanes,
	enNodes,
	enLeaves,
	enLeafFaces,
	enLeafBrushes,
	enModels,
	enBrushes,
	enBrushSides,
	enVertices,
	enMeshIndices,
	enEffect,
	enFaces,
	enLightmaps,
	enLightVols,
	enVisData
};

extern char szBSPTYPES [17][20];				// Defined in bsp.cpp

struct stLump
{
	int		nFileofs;			// Offset to start of lump, relative to beginning of file.
	int		nFileLen;			// Length of lump. Always a multiple of 4.
};

struct stHeader
{
	char   cMagic[4]; 
	int	   nVersion;
	stLump Lumps[17];
};

//----------------------------- Lump Definitions ---------------------------------//

typedef float	Vector2[2];

struct Vector3
{
	union
	{
		struct {float x,y,z;};
		float m[3];
	};
	float operator [](int i){ return m[i]; }

	float Dot(const Vector3 & v)
	{
		return (x*v.x + y*v.y + z*v.z);
	}

	float Dot(const D3DXVECTOR3 & v)
	{
		return (x*v.x + y*v.y + z*v.z);
	}
};

typedef float	Vector4[4];

typedef float	TexCoord[2];

typedef int		nBBox[6]; // Integer bounding box (mins, maxs)

typedef float	fBBox[6]; // Float bounding box
 

//Lump 0
//Entities
//char *entities; // A pointer to text
 

//Lump 1
//Shader Texture Info
struct stTexture
{
	char	Name[64];		// Texture name
	int		nSurfaceFlags;	// Type of surface (See Surface Flags below)
	int		nContentFlags;	// Leaf content (See Content Flags below)
};
 

//Lump 2
//Planes
struct stPlane 
{
	Vector3 Normal;			// Normal vector for plane
	float	fDist;			// Distance from plane to origin
};
 

//Lump 3
//Nodes
struct stNode 
{
	int		nPlane;			// Space partitioning plane
	int		nChildren[2];	// Back and front child nodes
	int		BBoxIMin[3];	// Bounding box of node
	int		BBoxIMax[3];	// Bounding box of node
};
 
//Lump 4
//Leaves 
struct stLeaf
{
	int		nCluster;		// Visibility cluster number
	int		nArea;			// Volume of the leaf
	int		BBoxIMin[3];	// Bounding box of leaf
	int		BBoxIMax[3];	// Bounding box of leaf

	int		nFirstFace, 
			NumFaces;		// Lookup for the face list (indexes 
							// are for faces)

	int		nFirstBrush, 
			NumBrushes;		// Lookup for the brush list (indexes
							// are for brushes)
};
 

//Lump 5
//Leaf Faces
//int *pFaces;				// a pointer to a series of indexes to 
							// a face list
 
//Lump 6
//Leaf Brushes
//int *pBrushes;			// a pointer to a series of indexes to 
							// a brush list
 
//Lump 7
//Models
struct stModel 
{
	fBBox	BBoxF;			// Bounding box of model

	int		nFirstFace,		// First face for model
			NumFaces;		// Number of faces for model 

	int		nFirstBrush,	// First brush for model
			NumBrushes;		// Number of brushes for model
};
 
//Lump 8
//Brushes
struct stBrush 
{
	int		nFirstSide,		// First brushside for brush
			NumSides;		// Number of brushsides for brush

	int		nIndex;			// Texture index
};
 
//Lump 9
//Brush Sides
struct stBrushSide 
{
	int		PlaneNum;		// Lookup for plane
	int		nIndex;			// Texture index
};
 
//Lump 10
//Vertices
struct stVertex 
{
	Vector3  vPoint;		// Vertex Position
	TexCoord Tex;			// Texture coordinates
	TexCoord LightTexCoord; // Light Map texture coordinates
	Vector3  vNormal;		// Normal vector (used for lighting ?)

	unsigned int RGBA;		// Vertex color. RGBA
};
 

//Lump 11
//MeshVert 
//int nOffset;				// Vertex index offset, relative to first 
							// vertex of corresponding face.  

 

//Lump 12
//Effect 
struct stEffect
{
	char cName[64];			// Effect shader.  
	int	 nBrush;			// Brush that generated this effect.  
	int  Unknown;			// Always 5, except in q3dm8, which has 
							// one effect with -1.  
};


 
//Lump 13
//Faces
#pragma pack(1)
struct stFace // size 24*4
{
	int nTexture;			// Refers to a texture
	int nEffect;			// Index into lump 12 (Effects), or -1
	int nFaceType;			// Face type. 1=polygon, 2=patch, 3=mesh, 4=billboard 
	int FirstVert, 
		NumVerts;			// Reference to vertices

	int nFirstMeshVerts,	// Index of first meshvert
							// Every three meshverts describe a triangle
		NumMeshVerts;		// Number of meshverts

	int LMIndex;			// Lightmap index.
	int LMStart[2];			// X,Y Corner of this face's lightmap image in lightmap.
	int LMSize[2];			// Size of lightmap

	float LMOrigin[3];		// World space origin of lightmap.
	float LMVects[2][3];	// World space lightmap s and t unit vectors.

	Vector3 vNormal;		// Face normal

	int nSize[2];			// Patch dimensions.  
};
#pragma pack()


//Lump 14
//Lightmaps
//unsigned char pMap[128][128][3]; // Lightmap color data. RGB.  
 
//Lump 15
//Light Grid
//Unknown
 
//Lump 16
//Visibility Lists
struct stVisData 
{
	int NumVectors;			// Number of vectors.
	int nSizeVectorInBytes;	// Size of each vector, in bytes
	unsigned char *pBits;	// [NumVectors * nSizeVector];  
							// Visibility data. One bit per cluster per vector
};

//Lump 17
//Number of Lumps
//Unknown



//PVS 포털 생성 클래스
class CBSP : public SceneNode
{
public:
	stHeader	m_Header;

	stVertex    *m_pVerts;
	int			m_nNumVerts;

	stFace		*m_pFaces;
	int			m_nNumFaces;

	int			*m_pMeshIndices;
	int			m_nNumMeshIndices;

	stTexture   *m_pTextures;
	int			m_nNumTextures;

	stNode		*m_pNodes;
	int			m_nNumNodes;

	stLeaf		*m_pLeafs;
	int			m_nNumLeafs;

	stPlane		*m_pPlanes;
	int			m_nNumPlanes;

	int			*m_pLeafFaces;
	int			m_nNumLeafFaces;

	stVisData	m_VisData;
	int			m_nNumVisData;

	
	struct stBSPVertex
	{
		D3DXVECTOR3 p;			
		D3DXVECTOR3 n;			
		DWORD		colour;		
		float       tu1,tv1;	
		float		lu2,lv2;	
	};

	CBSP(RenderMgr* SceneRen);
	int m_selectObjectNum;
	HObject tempobjectX[3];
	~CBSP();

    RenderMgr* CBSP_Render;
	LPDIRECT3DDEVICE9 pDevice;

	

	char szbuff[900];

    virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	
	virtual void SetMesh();

	void draw3(HCCameraNode* CameraInfo);


    IDirect3DVertexBuffer9 * m_pVB;
	UINT		m_FVF_Format;

	IDirect3DIndexBuffer9* m_pIB;

	IDirect3DTexture9 ** m_pT;

	
	int			m_nNumTrisDraw;

	
	bool		*m_bFaceVisibility;




	bool Release();
	bool LoadVerticeData(FILE* fp, stLump * pHeaderLump);
	bool LoadFacesData(FILE* fp,
					   stLump * pHeaderLump);
	bool LoadMeshVertsData(	FILE* fp,
							stLump * pHeaderLump);
	bool LoadTextureData(	FILE* fp,
							stLump * pHeaderLump);
	bool LoadNodesData(	FILE* fp,
						stLump * pHeaderLump);
	bool LoadLeafsData(	FILE* fp,
						stLump * pHeaderLump);
	bool LoadPlanesData( FILE* fp,
						 stLump * pHeaderLump);
	bool LoadLeafFacesData( FILE* fp,
						 stLump * pHeaderLump);
	bool LoadVisData( FILE* fp,
					  stLump * pHeaderLump);
	bool CreateDXVertexBuffer();
	bool CreateDXIndexBuffer();
	bool CreateDXTextures();


	bool Create(const char * szBSPFile);
	void Render();


	void RenderFaces(
					 int nStartFace,
					 int nNumFaces);
	void RenderLeaf(
						 int nLeafIndex);

	void DrawPlane(
					   stNode * pNode,
					   stPlane * pPlane);

	void CheckNode( 
					   int nNode,
					   D3DXVECTOR3 pStart,
					   D3DXVECTOR3 pEnd);

	int FindLeaf( 
					  const D3DXVECTOR3& Pos);

	void CheckNodeForLine( 
								D3DXVECTOR3 pStart,
								D3DXVECTOR3 pEnd);

	void CheckMultipleLines(
								 D3DXVECTOR3 pOrigin,
								 D3DXVECTOR3 vDir,
								 float fLen);

	//포털로 출력
	bool isClusterVisible(int visCluster, int testCluster);
	void DrawFace(int nFace);
	void TurnFaceOn(int nFace);
	void TurnAllFacesOff();
	void RenderAllFaces();
	void CheckClusterPoint(
							   D3DXVECTOR3 pPoint );
	D3DXVECTOR3 TransWMatrix(D3DXVECTOR3 PreVec);
	void draw4(HObject *obj);
};

#endif