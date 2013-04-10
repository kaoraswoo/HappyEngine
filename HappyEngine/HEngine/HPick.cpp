//-------------------------------------------------------------------------------------
//! ��ŷ ����
//! ���� �ذ�å �� ���ͳ� ����
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
// ��ŷ������ �����Ѵ�.(������ǥ���� ��ȯ��Ų��.)
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

	// VIEW�� ������� ���ؼ� World������ ������ ���Ѵ�.
	m_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	// ���� �̱� ������ w���� 0�� �ȴ�.
	m_pickRay._direction.x = v.x * matView._11 + v.y * matView._21 + v.z * matView._31;
	m_pickRay._direction.y = v.x * matView._12 + v.y * matView._22 + v.z * matView._32;
	m_pickRay._direction.z = v.x * matView._13 + v.y * matView._23 + v.z * matView._33;

	// ������ ��ġ�� ī�޶��� ��ġ���ȴ�.
	m_pickRay._origin.x = matView._41;
	m_pickRay._origin.y = matView._42;
	m_pickRay._origin.z = matView._43;

	// ���� ��ǥ�� ����� ��ȯ�� �� ���� ��ü�� ���� �Ѵ�.
	// �������ٰ� ���� ����� ������� ���� ������ǥ�� ��ȯ.
	//	m_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
	//	D3DXMatrixInverse(&matWorld, NULL, &matWorld);

	//	D3DXVec3TransformCoord(&m_pickRay._origin, &m_pickRay._origin, &matWorld);			// w=1
	//	D3DXVec3TransformNormal(&m_pickRay._direction, &m_pickRay._direction, &matWorld);	// w=0
	::D3DXVec3Normalize(&m_pickRay._direction, &m_pickRay._direction);
}

//-------------------------------------------------------------------------------------
// dist�� ������ ��ü�������� �Ÿ�.
// u, v�� �ﰢ���� ���� ��ǥ.
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
	// ��ŷ������ ������Ʈ�� ������ǥ�� ��ȯ�Ѵ�.
	D3DXVECTOR3 vOrigin, vDest, v;
	::D3DXVec3TransformCoord(&vOrigin, &(m_pickRay._origin), &matInvTM);
	::D3DXVec3TransformNormal(&vDest, &(m_pickRay._direction), &matInvTM);
	::D3DXVec3Normalize(&vDest, &vDest);

	v = vOrigin - vCenter;

	float B = 2.0f * D3DXVec3Dot(&vDest, &v);
	float C = D3DXVec3Dot(&v, &v) - (fRadius * fRadius );

	// �Ǻ����� ã�´�.
	float discriminant = (B * B) - (4.0f * C);

	// ������ ���� ���� �׽�Ʈ
	if( discriminant < 0.0f ) // ������
		return false;
	
	
	discriminant = sqrtf(discriminant);

	float S0 = (-B + discriminant) / 2.0f;
	float S1 = (-B - discriminant) / 2.0f;

	// �ذ� >=0 �� ��� ��ü�� �����ϴ� ���̴�.
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
	// ��ŷ������ ������Ʈ�� ������ǥ�� ��ȯ�Ѵ�.
	D3DXVECTOR3 vOrigin, vDest, v;
	float t1, t2, tMin, tMax, tTemp;
	::D3DXVec3TransformCoord(&vOrigin, &(m_pickRay._origin), &matInvTM);
	::D3DXVec3TransformNormal(&vDest, &(m_pickRay._direction), &matInvTM);
	vDest *= 2000.0f;		// �켱 �������� �����. 

	v = vDest - vOrigin;

	tMin = -2000;			// �켱 �ִ밪�� �����Ѵ�.
	tMax = 2000;
	
	// �켱 ������ ������ �ڽ��ȿ� ���� ��� ���� ���� �ʴ´ٰ� �Ѵ�.
	BOOL bInBoxMin = (vOrigin.x > vMin.x && vOrigin.y > vMin.y && vOrigin.z > vMin.z ) ? true : false;
	BOOL bInBoxMax = (vDest.x < vMax.x && vDest.y < vMax.y && vDest.z < vMax.z) ? true : false;

	if( bInBoxMin && bInBoxMax ) // �ڽ��ȿ� �ִٸ� ���� ���Ѵ�.
	{
		return false;
	}
	else if( bInBoxMin != bInBoxMax)	// �������� ������ �ϳ��� �ڽ� ���̶�� ���� �Ѵ�.
	{
		return true;
	}

	// �� x,y,z ���� ���� ������ �̿��� ���� ������ �Ѵ�.

	const float EPSILON = 0.001f;

	// �� ������ ���Ⱑ 0 �̸鼭, �������� ���� �ܺο� �ִٸ� ���� ���� �ʴ´�.
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

	// �� ������ ���Ⱑ 0 �ƴ� ��� ���� �������Ѵ�.
	// t = (di - Ai) / (Bi - Ai)
	// x���к���
	t1 = (vMin.x - vOrigin.x) / v.x;		// ������ �Ա�
	t2 = (vMax.x - vOrigin.x) / v.x;		// ������ �ⱸ
	
	// ������ ���Ⱑ �������� ������� ������� �ϱ� ���ؼ� ������ ���ش�.
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
	// ������ �Ա��� �ⱸ ���� ũ�ٸ�, ���� ���� �ʴ´�.
	if(tMin > tMax)
		return false;
	// ������ ������ t�� 0 <= t <= 1 �� �ƴ϶�� ���� ���� �ʴ´�.
	if(tMax < 0.0f )
		return false;
	if(tMin > 1.0f)
		return false;

	// y����
	t1 = (vMin.y - vOrigin.y) / v.y;		// ������ �Ա�
	t2 = (vMax.y - vOrigin.y) / v.y;		// ������ �ⱸ	
	// ������ ���Ⱑ �������� ������� ������� �ϱ� ���ؼ� ������ ���ش�.
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
	// ������ �Ա��� �ⱸ ���� ũ�ٸ�, ���� ���� �ʴ´�.
	if(tMin > tMax)
		return false;
	// ������ ������ t�� 0 <= t <= 1 �� �ƴ϶�� ���� ���� �ʴ´�.
	if(tMax < 0.0f )
		return false;
	if(tMin > 1.0f)
		return false;

		// z���к���
	t1 = (vMin.z - vOrigin.z) / v.z;		// ������ �Ա�
	t2 = (vMax.z - vOrigin.z) / v.z;		// ������ �ⱸ
	
	// ������ ���Ⱑ �������� ������� ������� �ϱ� ���ؼ� ������ ���ش�.
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
	// ������ �Ա��� �ⱸ ���� ũ�ٸ�, ���� ���� �ʴ´�.
	if(tMin > tMax)
		return false;
	// ������ ������ t�� 0 <= t <= 1 �� �ƴ϶�� ���� ���� �ʴ´�.
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
		
	// ���� �귯���� ���ԵǴ� ���簢���� �ȼ� ���� (������ ��ü �� �����̴�.)
	int start_S = texSell_S - editOutSize; 
	int start_T = texSell_T - editOutSize;
	int end_S = texSell_S + editOutSize;
	int end_T = texSell_T + editOutSize;
	if( start_S < 0 ) { start_S = 0; }
	if( start_T < 0 ) { start_T = 0; }
	if( end_S >= ALPHA_TEXTURE_SIZE ) { end_S = ALPHA_TEXTURE_SIZE -1; }	
	if( end_T >= ALPHA_TEXTURE_SIZE ) { end_T = ALPHA_TEXTURE_SIZE -1; }	
}