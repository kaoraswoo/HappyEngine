
#pragma once


class WPFRenderer : public CRenderer
{
public:
	static HRESULT Create(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter, CRenderer **ppRenderer);
	~WPFRenderer();

	HRESULT Render();

protected:
	HRESULT Init(IDirect3D9 *pD3D, IDirect3D9Ex *pD3DEx, HWND hwnd, UINT uAdapter);

private:
	WPFRenderer();

	//IDirect3DVertexBuffer9 *m_pd3dVB;
	//LPDIRECT3DINDEXBUFFER9	m_pIB;

	


};