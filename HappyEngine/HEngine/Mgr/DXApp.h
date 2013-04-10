

#include "WinApp.h"


struct CUSTOMVERTEX
{
    FLOAT x, y, z;      // 정점의 3차원 좌표 The untransformed, 3D position for the vertex
    DWORD color;        // The vertex color
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
class DXApp{
public:
LPDIRECT3D9             g_pD3D;
LPDIRECT3DDEVICE9       g_pd3dDevice;
LPDIRECT3DVERTEXBUFFER9 g_pVB;
LPDIRECT3DINDEXBUFFER9	g_pIB;

HWND m_hWnd;

DXApp();
int InitDXApp(HWND hWnd);
void Render();
void print(){};
void Reset();
};


class CWinApp:public WinApp{
public:
	DXApp DXd;
	
	


};