//-------------------------------------------------------------------------------------
// 퀘이크용 bsp로드 - 퀘이크 소스 발췌
//! BSP 부분에서 수정(BSP 로드부분 발췌함)
//! 발췌 한 곳 - http://blog.naver.com/ryanii?Redirect=Log&logNo=20017051226
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _QBD_BSP_
#define _QBD_BSP_

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

void ErrLog(char *str);

struct BSP_ENTRY
{
	int offset;
	int length;
};

struct BSP_HEADER
{
	char idNumber[4];
	int version;
	BSP_ENTRY entries[17];
};

enum BSP_LUMP
{
	lEntities,
	lTextures,
	lPlanes,
	lNodes,
	lLeafs,
	lLeaffaces,
	lLeafbrushes,
	lModels,
	lBrushes,
	lBrushSides,
	lVertcies,
	lIndices,
	lEffects,
	lFaces,
	lLightmaps,
	lLightvols,
	lVisdata
};

struct BSP_ENTITY
{
	char *ents;	
};

struct BSP_TEXTURE
{
	char name[64];
	int flags;
	int contents;
};

struct BSP_PLANE
{
	D3DXVECTOR3 normal;
	float dist;
};

struct BSP_NODE
{
	int plane;
	int children[2];
	int mins[3];
	int maxs[3];
};

struct BSP_LEAF
{
	int cluster;
	int area;
	int mins[3];
	int maxs[3];
	int leafface;
	int n_leaffaces;
	int leafbrush;
	int n_leafbrushes;
};

typedef int BSP_LEAFFACES;

typedef int BSP_LEAFBRUSHES;

struct BSP_MODEL
{
	float mins[3];
	float maxs[3];
	int face;
	int n_faces;
	int brush;
	int n_brushes;
};

struct BSP_BRUSH
{
	int brushside;
	int n_brushsides;
	int texture;
};

struct BSP_BRUSHSIDES
{
	int plane;
	int texture;
};

struct BSP_VERTEX
{
	float position[3];
	float texcoord[2];
	float lightmap[2];
	float normal[3];
	BYTE color[4];
};

typedef LONG BSP_INDEX;

struct BSP_EFFECT
{
	char name[64];
	int brush;
	int unknown;
};

struct BSP_FACE
{
	int texture;
	int effect;
	int type;
	int vertex;
	int n_verts;
	int meshvert;
	int n_meshverts;
	int lm_index;
	int lm_start[2];
	int lm_size[2];
	float lm_origin[3];
	float lm_vecs[2][3];
	float normal[3];
	int size[2];
};

struct BSP_LIGHTMAP
{
    byte imageBits[128][128][3];
};

struct BSP_VISDATA
{
	int n_vecs;
	int vecSize;
	byte *vecs; 
};


struct BSP_CUSTOM_VERTEX
{
    float x, y, z;
    float nx, ny, nz;
	D3DCOLOR color;
    float tu, tv;
	float lu, lv;
};

#define BSP_CUSTOM_VERTEX_FVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_TEX2)

enum BSP_RENDER_STATES
{
	rShowLightMap = 0,
	rShowTextureMap,
	rUsePVS,
};

class Q3_BSP : public SceneNode
{
public:
	
	Q3_BSP(RenderMgr* SceneRen);
	
	~Q3_BSP();

    RenderMgr* Q3BSP_Render;
	LPDIRECT3DDEVICE9 m_pDevice;
    virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	virtual void SetMesh();

	// Loads a .bsp file
	bool Load(const char *strFileName);
	

	void Render();
	

	void SetRenderState(BSP_RENDER_STATES state, bool value);
	bool GetRenderState(BSP_RENDER_STATES state);

	
	void Destroy();

private:
	

	void Build();
	

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;
	LPDIRECT3DTEXTURE9 *m_pTextureMaps;
	

	int numVerts;
	int numFaces;
	int numIndex;
	int numTextures;
	
	
	BSP_TEXTURE *m_pTextures;
	BSP_VERTEX *m_pVerts;
	BSP_FACE *m_pFaces;
	BSP_INDEX *m_pIndices;
	
	
	bool m_pRenderState[3];
};


#endif