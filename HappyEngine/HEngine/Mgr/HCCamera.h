#ifndef _Renderer_h_
#define _Renderer_h_
#include "Renderer.h"
#endif

//�ذ� 3d���α׷��� ���� ī�޶� Ŭ���� 
class HCCameraNode{
	public:
		D3DXVECTOR3 m_vEye;
		D3DXVECTOR3 m_vLookat;

		D3DXVECTOR3 m_vUp;
		D3DXVECTOR3 m_vView;
		D3DXVECTOR3 m_vCross;
		
		
		D3DXMATRIXA16 m_matView;
		D3DXMATRIXA16 m_matBill;
		//std::vector<D3DXMATRIXA16> CameraAniList;
		// D3DXMATRIX16 std::vector has problem - http://thetweaker.wordpress.com/2010/05/05/stdvector-of-aligned-elements/



		D3DXMATRIXA16* GetViewMatrix(){return &m_matView;}
		D3DXMATRIXA16* GetBillMatrix(){return &m_matBill;}

		D3DXMATRIXA16* SetView(D3DXVECTOR3* pvEye,D3DXVECTOR3* pvLookat,D3DXVECTOR3* pvUp);

		void SetEye(D3DXVECTOR3* pv){m_vEye=*pv;}
		D3DXVECTOR3* GetEye(){ return &m_vEye;}
		void SetLookat(D3DXVECTOR3* pv){m_vLookat=*pv;}
		D3DXVECTOR3* GetLookat(){return &m_vLookat;}

		void SetUp(D3DXVECTOR3* pv){m_vUp=*pv;}

		D3DXVECTOR3* GetUp(){return &m_vUp;}
		void Flush(){SetView(&m_vEye,&m_vLookat,&m_vUp);}

		D3DXMATRIXA16* RotateLocalX(float angle);
		D3DXMATRIXA16* RotateLocalY(float angle);

		D3DXMATRIXA16* MoveTo(D3DXVECTOR3* pv);
		D3DXMATRIXA16* MoveLocalX(float dist);
		D3DXMATRIXA16* MoveLocalY(float dist);
		D3DXMATRIXA16* MoveLocalZ(float dist);


};


class HCCameraMgr{
	public:
		std::vector<HCCameraNode*> CnodeList;   //�Ѱ��ӳ� �������� ī�޶� ����
		std::vector<HCCameraNode*>::iterator CameraIter;

		HCCameraMgr();   //SnodeList�� �⺻ī�޶� ����(0���� ��)
		~HCCameraMgr();   //ī�޶� �޸� ����

};

