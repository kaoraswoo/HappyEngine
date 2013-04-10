#ifndef _ZTERRAIN_H_
#define _ZTERRAIN_H_

#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

#include "define.h"
#include "ZQuadTree.h"
#include "Splatting.h"

#define MAX_TERRAIN_TEX	4


/**
 * //�ذ�å �����κ� ����
 * 
 */

class ZTerrain:public SceneNode
{
	public:
	RenderMgr* ZTerrainRender;
	//HSplatting *HSplat;
	int SplattingAnum;
	

	//HSplatting *HSplat;
	ZTerrain(RenderMgr* SceneRen);

	
    virtual void draw();
	virtual void draw(ZFrustum* pFrustum);
	virtual void SetMesh();

private:
	int						m_cxDIB;		/// DIB�� �����ȼ���
	int						m_czDIB;		/// DIB�� �����ȼ���
	int						m_cyDIB;		/// DIB�� �ִ���̰�(�� 0 ~ 255������ ��)
	D3DXVECTOR3				m_vfScale;		/// x scale, y scale, z scale
	TERRAINVERTEX*			m_pvHeightMap;	/// ���̸��� �����迭
	LPDIRECT3DDEVICE9		m_pd3dDevice;	/// ��¿� D3D����̽�
	LPDIRECT3DTEXTURE9		m_pTex[MAX_TERRAIN_TEX];	/// �ؽ�ó
	LPDIRECT3DVERTEXBUFFER9	m_pVB;			/// ������¿� ��������
	LPDIRECT3DINDEXBUFFER9	m_pIB;			/// ������¿� �ε�������
	int						m_nTriangles;	/// ����� �ﰢ���� ����
	ZQuadTree*				m_pQuadTree;	/// ����Ʈ�� ��ü�� ������
	float					m_fLODRatio;	/// LODó���ÿ� ���� ��

	
	

	

public:
	// ������ ���õ� �Լ���
				
				~ZTerrain();

	HRESULT		Create( D3DXVECTOR3* pvfScale, float fLODRatio, LPSTR lpBMPFilename, LPSTR lpTexFilename[MAX_TERRAIN_TEX] );

	// ZTerrain���ο��� ���Ǵ� �Լ���
private:
	/// ������ü�� �޸𸮿��� �Ұ��Ѵ�.
	HRESULT		_Destroy();

	/// ������ü���� ����� �ؽ�ó�� �о���δ�.
	HRESULT		_LoadTextures( LPSTR lpTexFilename[MAX_TERRAIN_TEX] );

	/// BMP������ ��� ���̸��� �����Ѵ�.
	HRESULT		_BuildHeightMap( LPSTR lpFilename );

	/// BMP������ ũ�⿡ ���缭 ����Ʈ���� �����Ѵ�.
	HRESULT		_BuildQuadTree();

	/// ����, �ε��� ���۸� �����Ѵ�.
	HRESULT		_CreateVIB();

	/// ȭ�鿡 ������ ����Ѵ�.
	HRESULT		_Render();
public:
	/// x, z��ġ�� �������� ����.
	TERRAINVERTEX*	GetVertex( int x, int z ) { return (m_pvHeightMap+x+z*m_cxDIB); }

	/// x, z��ġ�� ����(y)���� ����.
	float		GetHeight( int x, int z ) { return (GetVertex( x, z ))->p.y; }

	/// ���̸� BMP������ �����ȼ���
	int			GetCXDIB() { return m_cxDIB; }

	/// ���̸� BMP������ �����ȼ���
	int			GetCZDIB() { return m_czDIB; }

	/// ���̸��� ���� x�� ��(column)��
	float		GetCXTerrain() { return m_cxDIB * m_vfScale.x; }

	/// ���̸��� ���� y�� ����(height)��
	float		GetCYTerrain() { return m_cyDIB * m_vfScale.y; }

	/// ���̸��� ���� z�� ����(row)��
	float		GetCZTerrain() { return m_czDIB * m_vfScale.z; }

	/// ȭ�鿡 ������ ����Ѵ�.
	HRESULT		DrawTerrain( ZFrustum* pFrustum );
};

#endif // _ZTERRAIN_H_