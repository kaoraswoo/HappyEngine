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

	// 픽킹 광선 생성
	void PickTestInit(int mouseX, int mouseY);

	// 삼각형 픽킹 테스트
	// v1,v2,v3 ,dist
	BOOL IntersectTriangle(D3DXVECTOR3 &v0, D3DXVECTOR3 &v1, D3DXVECTOR3 &v2, float &dist);

	// fRange는 충돌시 거리
	BOOL IntersectSphere(float &fRange, float fRadius, D3DXVECTOR3 vCenter, D3DXMATRIX &matInvTM); 

	// 선분과, AABB박스 충돌 교차 판정만한다.
	BOOL IntersectAABB(D3DXVECTOR3 vMin, D3DXVECTOR3 vMax, D3DXMATRIX &matInvTM);

};

#endif