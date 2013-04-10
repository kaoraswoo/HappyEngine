#ifndef _HPICK_H_
#define _HPICK_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "define.h"



struct PickRay
{
	D3DXVECTOR3 _origin;
	D3DXVECTOR3 _direction;
};

class HPick
{

private:
	LPDIRECT3DDEVICE9 m_pd3dDevice;


public:
	PickRay			m_pickRay;
	RECT			m_rect;

	int start_S;
	int end_T;
	float tx,ty;

	void SetTextureCoord( float x, float z , float editInSize, float editOutSize);

	HPick(LPDIRECT3DDEVICE9 device);

	// ��ŷ ���� ����
	void PickTestInit(int mouseX, int mouseY);

	// �ﰢ�� ��ŷ �׽�Ʈ
	// v1,v2,v3 ,dist
	BOOL IntersectTriangle(D3DXVECTOR3 &v0, D3DXVECTOR3 &v1, D3DXVECTOR3 &v2, float &dist);

	// fRange�� �浹�� �Ÿ�
	BOOL IntersectSphere(float &fRange, float fRadius, D3DXVECTOR3 vCenter, D3DXMATRIX &matInvTM); 

	// ���а�, AABB�ڽ� �浹 ���� �������Ѵ�.
	BOOL IntersectAABB(D3DXVECTOR3 vMin, D3DXVECTOR3 vMax, D3DXMATRIX &matInvTM);

};

#endif