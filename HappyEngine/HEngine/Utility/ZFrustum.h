#ifndef _ZFRUSTUM_H_
#define _ZFRUSTUM_H_

#include <d3d9.h>
#include <d3dx9.h>

/// �������ҿ� ��Ȯ�ϰ� ���Ե��� �ʴ���, �ణ�� ������ �־ �������ҿ� ���Խ�Ű�� ���� ��
#define PLANE_EPSILON	5.0f

/**
 * �������� �ø��� �ϱ����� Ŭ����
 *
 */
class ZFrustum
{
	public:
	D3DXVECTOR3	m_vtx[8];	/// ���������� ������ ���� 8��
	D3DXVECTOR3	m_vPos;		/// ���� ī�޶��� ������ǥ
	D3DXVECTOR3	m_vPos2;		/// ���� ī�޶��� far ����� ����
	D3DXVECTOR3 m_vLook;    /// ���� ī�޶��� Look ����(m_vPos2 - m_vPos)
		
	D3DXPLANE	m_plane[6];	/// ���������� �����ϴ� 6���� ���

	D3DXMATRIXA16* m_matView;  //ī�޶� ������ּ� ����
	D3DXMATRIXA16* m_matProj;  //ī�޶� ������ּ� ����


		int JCMODE1;//2. ����Ʈ�� - ���÷��� 1��
				//3. ����Ʈ�� - ���÷��� 2��
		int JCMODE2;
				//4. ����Ʈ�� - �������� ī�޶�
				//5. ����Ʈ�� - �������� ������Ʈ1
				//6. ����Ʈ�� - �������� ������Ʈ2


	/// ������
	ZFrustum();
	
	/// ī�޶�(view) * ��������(projection)����� �Է¹޾� 6���� ����� �����.
	BOOL	Make( D3DXMATRIXA16* pmatViewProj );

	BOOL    ViewSet(D3DXMATRIXA16* CameraView);
	BOOL	ProjSet(D3DXMATRIXA16* CameraProj);

	/// ���� v�� �������Ҿȿ� ������ TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
	BOOL	IsIn( D3DXVECTOR3* pv );

	/** �߽�(v)�� ������(radius)�� ���� ��豸(bounding sphere)�� �������Ҿȿ� ������
	 *  TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
	 */
	BOOL	IsInSphere( D3DXVECTOR3* pv, float radius );

	/// ���������� ȭ�鿡 �׷��ش�.
	BOOL	Draw( LPDIRECT3DDEVICE9 pDev );

	/// ���� ī�޶��� ������ǥ�� ����ش�.
	D3DXVECTOR3*	GetPos() { return &m_vPos; }
};

/// ����� �������� �������ϴ� �Լ�, ����� ���ǰ� ���� �ʴ�.
void ZMakePlane( D3DXPLANE* pPlane, D3DXVECTOR3* pv0, D3DXVECTOR3* pv1, D3DXVECTOR3* pv2 );

#endif // _ZFRUSTUM_H_