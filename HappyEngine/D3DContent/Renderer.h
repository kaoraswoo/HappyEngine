
#pragma once


class CRenderer
{
public:
	~CRenderer();

	static HRESULT Create(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter, CRenderer **ppRenderer);


	HRESULT CheckDeviceState();
	HRESULT CreateSurface(UINT uWidth, UINT uHeight, bool fUseAlpha, UINT m_uNumSamples);

	HRESULT Render();
	IDirect3DSurface9 *GetSurfaceNoRef() { return m_pd3dRTS; }

	IDirect3DDevice9* GetDevice(){return m_pd3dDevice;}

	void SetCamera(POINT pos);

protected:
	CRenderer();

	HRESULT Init(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter);

	IDirect3DDevice9   *m_pd3dDevice;
	IDirect3DDevice9Ex *m_pd3dDeviceEx;

	IDirect3DSurface9 *m_pd3dRTS;

	D3DXMATRIXA16 m_matProj;

	ZFrustum*	m_pFrustum;	// Frustum Å¬·¡½º
	SceneMgr *m_Happy;
	GameProgrammingClass *practices;
	Q3_BSP* BspNode;
	TileTerrainMgr* TileTerrainNode;

	

};
