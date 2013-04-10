
#pragma once
//#include "HappyEngine.h"

class CTriangleRenderer : public CRenderer
{
public:
	static HRESULT Create(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter, CRenderer **ppRenderer);
	~CTriangleRenderer();

	HRESULT Render();

protected:
	HRESULT Init(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter);

private:
	CTriangleRenderer();

	IDirect3DVertexBuffer9 *m_pd3dVB;
	LPDIRECT3DINDEXBUFFER9	m_pIB;

	//SceneMgr *m_Happy;
	//GameProgrammingClass *practices;
	//Q3_BSP* BspNode;


};
