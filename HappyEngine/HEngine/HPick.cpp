//-------------------------------------------------------------------------------------
//! 픽킹 구현
//! 원리 해골책 및 인터넷 참조
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include "HPick.h"

//-------------------------------------------------------------------------------------
HPick::HPick(LPDIRECT3DDEVICE9 device)
{

	m_pd3dDevice = device;
	tx = -1;
	ty = -1;

}

//-------------------------------------------------------------------------------------
// 픽킹광선을 생성한다.(지역좌표까지 변환시킨다.)
void HPick::PickTestInit(int mouseX, int mouseY)
{
	D3DXMATRIX matProj, matView, matWorld;
	D3DVIEWPORT9 vp;
	D3DXVECTOR3 v;

	m_pd3dDevice->GetViewport(&vp);
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	v.x = ( ( (  ( (mouseX - vp.X)*2.0f/vp.Width ) - 1.0f)) - matProj._31) / matProj._11;
	v.y = ( ( -(  ( (mouseY - vp.Y)*2.0f/vp.Height ) - 1.0f)) - matProj._32) / matProj._22;
	v.z = 1.0f;

	// VIEW의 역행렬을 곱해서 World시점의 광선을 구한다.
	m_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	// 벡터 이기 때문에 w값은 0이 된다.
	m_pickRay._direction.x = v.x * matView._11 + v.y * matView._21 + v.z * matView._31;
	m_pickRay._direction.y = v.x * matView._12 + v.y * matView._22 + v.z * matView._32;
	m_pickRay._direction.z = v.x * matView._13 + v.y * matView._23 + v.z * matView._33;

	// 광선의 위치는 카메라의 위치가된다.
	m_pickRay._origin.x = matView._41;
	m_pickRay._origin.y = matView._42;
	m_pickRay._origin.z = matView._43;

	// 월드 좌표의 역행렬 변환은 각 각의 객체로 가서 한다.
	// 광선에다가 월드 행렬의 역행렬을 곱해 지역좌표로 변환.
	//	m_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	//	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	//	D3DXVec3TransformCoord(&m_pickRay._origin, &m_pickRay._origin, &matWorld);			// w=1
	//	D3DXVec3TransformNormal(&m_pickRay._direction, &m_pickRay._direction, &matWorld);	// w=0
	::D3DXVec3Normalize(&m_pickRay._direction, &m_pickRay._direction);
}

//-------------------------------------------------------------------------------------
// dist는 광선과 교체점까지의 거리.
// u, v는 삼각형의 교점 좌표.
BOOL HPick::IntersectTriangle(D3DXVECTOR3 &v0, D3DXVECTOR3 &v1, D3DXVECTOR3 &v2, float &dist)
{
	float det, u, v;								
	D3DXVECTOR3 pvec, tvec, qvec;
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	D3DXVec3Cross(&pvec, &m_pickRay._direction, &edge2);	
	det = D3DXVec3Dot(&edge1, &pvec);

	if(det < 0.0001f)
		return false;

	tvec = m_pickRay._origin - v0;
	u = D3DXVec3Dot(&tvec, &pvec);
	if(u < 0.0f || u > det)
		return false;

	D3DXVec3Cross(&qvec, &tvec, &edge1);
	v = D3DXVec3Dot(&m_pickRay._direction, &qvec);
	if(v < 0.0f || u + v > det)
		return false;

	dist = D3DXVec3Dot(&edge2, &qvec);
	dist *= (1.0f / det);

	return true;
}

//-------------------------------------------------------------------------------------
BOOL HPick::IntersectSphere(float &fRange, float fRadius, D3DXVECTOR3 vCenter, D3DXMATRIX &matInvTM)
{
	// 픽킹광선을 오브젝트의 지역좌표로 변환한다.
	D3DXVECTOR3 vOrigin, vDest, v;
	::D3DXVec3TransformCoord(&vOrigin, &(m_pickRay._origin), &matInvTM);
	::D3DXVec3TransformNormal(&vDest, &(m_pickRay._direction), &matInvTM);
	::D3DXVec3Normalize(&vDest, &vDest);

	v = vOrigin - vCenter;

	float B = 2.0f * D3DXVec3Dot(&vDest, &v);
	float C = D3DXVec3Dot(&v, &v) - (fRadius * fRadius );

	// 판별식을 찾는다.
	float discriminant = (B * B) - (4.0f * C);

	// 가상의 수에 대한 테스트
	if( discriminant < 0.0f ) // 허수라면
		return false;
	
	
	discriminant = sqrtf(discriminant);

	float S0 = (-B + discriminant) / 2.0f;
	float S1 = (-B - discriminant) / 2.0f;

	// 해가 >=0 일 경우 구체를 교차하는 것이다.
	if( S0 >= 0.0f || S1 >= 0.0f)
	{
		if( S0 >= 0.0f )
			fRange = S0;

		if( S1 >= 0.0f )
			if( S1 < fRange)
				fRange = S1;

		return true;
	}
	fRange = 0.0f;

	return false;
}

//-------------------------------------------------------------------------------------
BOOL HPick::IntersectAABB(D3DXVECTOR3 vMin, D3DXVECTOR3 vMax, D3DXMATRIX &matInvTM)
{
	// 픽킹광선을 오브젝트의 지역좌표로 변환한다.
	D3DXVECTOR3 vOrigin, vDest, v;
	float t1, t2, tMin, tMax, tTemp;
	::D3DXVec3TransformCoord(&vOrigin, &(m_pickRay._origin), &matInvTM);
	::D3DXVec3TransformNormal(&vDest, &(m_pickRay._direction), &matInvTM);
	vDest *= 2000.0f;		// 우선 선분으로 만든다. 

	v = vDest - vOrigin;

	tMin = -2000;			// 우선 최대값을 설정한다.
	tMax = 2000;
	
	// 우선 선분이 완전히 박스안에 있을 경우 교차 하지 않는다고 한다.
	BOOL bInBoxMin = (vOrigin.x > vMin.x && vOrigin.y > vMin.y && vOrigin.z > vMin.z ) ? true : false;
	BOOL bInBoxMax = (vDest.x < vMax.x && vDest.y < vMax.y && vDest.z < vMax.z) ? true : false;

	if( bInBoxMin && bInBoxMax ) // 박스안에 있다면 교차 안한다.
	{
		return false;
	}
	else if( bInBoxMin != bInBoxMax)	// 시작점과 끝점중 하나만 박스 안이라면 교차 한다.
	{
		return true;
	}

	// 각 x,y,z 성분 별로 슬랩을 이용해 교차 판정을 한다.

	const float EPSILON = 0.001f;

	// 각 성분의 기울기가 0 이면서, 시작점이 슬랩 외부에 있다면 교차 하지 않는다.
	if( abs(v.x) < EPSILON )
	{
		if( vOrigin.x < vMin.x || vOrigin.x > vMax.x )
		{
			return false;
		}
	}
	if( abs(v.y) < EPSILON )
	{
		if( vOrigin.y < vMin.y || vOrigin.y > vMax.y )
		{
			return false;
		}
	}
	if( abs(v.z) < EPSILON )
	{
		if( vOrigin.z < vMin.z || vOrigin.z > vMax.z )
		{
			return false;
		}
	}

	// 각 성분의 기울기가 0 아닐 경우 교차 판정을한다.
	// t = (di - Ai) / (Bi - Ai)
	// x성분부터
	t1 = (vMin.x - vOrigin.x) / v.x;		// 슬랩의 입구
	t2 = (vMax.x - vOrigin.x) / v.x;		// 슬랩의 출구
	
	// 선분의 기울기가 음수인지 양수인지 상관없이 하기 위해서 정렬을 해준다.
	if( t1 > t2 )
	{
		tTemp = t1;
		t1 = t2;
		t2 = tTemp;
	}
	if(t1 > tMin)
		tMin = t1;
	if(t2 < tMax)
		tMax = t2;
	// 슬랩의 입구가 출구 보다 크다면, 교차 하지 않는다.
	if(tMin > tMax)
		return false;
	// 선분의 범위인 t가 0 <= t <= 1 이 아니라면 교차 하지 않는다.
	if(tMax < 0.0f )
		return false;
	if(tMin > 1.0f)
		return false;

	// y성분
	t1 = (vMin.y - vOrigin.y) / v.y;		// 슬랩의 입구
	t2 = (vMax.y - vOrigin.y) / v.y;		// 슬랩의 출구	
	// 선분의 기울기가 음수인지 양수인지 상관없이 하기 위해서 정렬을 해준다.
	if( t1 > t2 )
	{
		tTemp = t1;
		t1 = t2;
		t2 = tTemp;
	}
	if(t1 > tMin)
		tMin = t1;
	if(t2 < tMax)
		tMax = t2;
	// 슬랩의 입구가 출구 보다 크다면, 교차 하지 않는다.
	if(tMin > tMax)
		return false;
	// 선분의 범위인 t가 0 <= t <= 1 이 아니라면 교차 하지 않는다.
	if(tMax < 0.0f )
		return false;
	if(tMin > 1.0f)
		return false;

		// z성분부터
	t1 = (vMin.z - vOrigin.z) / v.z;		// 슬랩의 입구
	t2 = (vMax.z - vOrigin.z) / v.z;		// 슬랩의 출구
	
	// 선분의 기울기가 음수인지 양수인지 상관없이 하기 위해서 정렬을 해준다.
	if( t1 > t2 )
	{
		tTemp = t1;
		t1 = t2;
		t2 = tTemp;
	}

	if(t1 > tMin)
		tMin = t1;
	if(t2 < tMax)
		tMax = t2;
	// 슬랩의 입구가 출구 보다 크다면, 교차 하지 않는다.
	if(tMin > tMax)
		return false;
	// 선분의 범위인 t가 0 <= t <= 1 이 아니라면 교차 하지 않는다.
	if(tMax < 0.0f )
		return false;
	if(tMin > 1.0f)
		return false;

	return true;
}

//-------------------------------------------------------------------------------------
void HPick::SetTextureCoord( float x, float z, float editInSize, float editOutSize)
{
	tx = x;
	ty = z;

	int texSell_S = (int)(x * ALPHA_TEXTURE_SIZE);
	int texSell_T = (int)(z * ALPHA_TEXTURE_SIZE);
		
	// 원형 브러쉬가 포함되는 정사각형의 픽셀 범위 (아직은 전체 맵 기준이다.)
	int start_S = texSell_S - editOutSize; 
	int start_T = texSell_T - editOutSize;
	int end_S = texSell_S + editOutSize;
	int end_T = texSell_T + editOutSize;
	if( start_S < 0 ) { start_S = 0; }
	if( start_T < 0 ) { start_T = 0; }
	if( end_S >= ALPHA_TEXTURE_SIZE ) { end_S = ALPHA_TEXTURE_SIZE -1; }	
	if( end_T >= ALPHA_TEXTURE_SIZE ) { end_T = ALPHA_TEXTURE_SIZE -1; }	
}