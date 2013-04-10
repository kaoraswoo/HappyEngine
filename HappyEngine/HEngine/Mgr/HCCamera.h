//-------------------------------------------------------------------------------------
//! 카메라 생성
//! 해골 3d프로그래밍 참조 카메라 클래스 
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _Renderer_h_
#define _Renderer_h_
#include "Renderer.h"
#endif

//해골 3d프로그래밍 참조 카메라 클래스 
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
		// D3DXMATRIX16 std::vector has problem -  아래 링크 참조하면 알 수 있다
		//http://thetweaker.wordpress.com/2010/05/05/stdvector-of-aligned-elements/

		
		D3DXMATRIXA16* GetViewMatrix(){return &m_matView;} //뷰매트릭스
		D3DXMATRIXA16* GetBillMatrix(){return &m_matBill;} //빌보드 매트릭스

		//뷰 매트릭스를 만든다
		D3DXMATRIXA16* SetView
			(
				D3DXVECTOR3* pvEye,
				D3DXVECTOR3* pvLookat,
				D3DXVECTOR3* pvUp
			); 
		// 카메라 기본 변수 Getter / Setter들
		void SetEye(D3DXVECTOR3* pv){m_vEye=*pv;}
		D3DXVECTOR3* GetEye(){ return &m_vEye;}
		void SetLookat(D3DXVECTOR3* pv){m_vLookat=*pv;}
		D3DXVECTOR3* GetLookat(){return &m_vLookat;}
		void SetUp(D3DXVECTOR3* pv){m_vUp=*pv;}
		D3DXVECTOR3* GetUp(){return &m_vUp;}
		void Flush(){SetView(&m_vEye,&m_vLookat,&m_vUp);}

		// 이동, 회전 로직
		D3DXMATRIXA16* RotateLocalX(float angle);
		D3DXMATRIXA16* RotateLocalY(float angle);
		D3DXMATRIXA16* MoveTo(D3DXVECTOR3* pv);
		D3DXMATRIXA16* MoveLocalX(float dist);
		D3DXMATRIXA16* MoveLocalY(float dist);
		D3DXMATRIXA16* MoveLocalZ(float dist);

};

//카메라 매니저
class HCCameraMgr{
	public:
		std::vector<HCCameraNode*> CnodeList;   //한게임내 여러대의 카메라 관리
		std::vector<HCCameraNode*>::iterator CameraIter;
		HCCameraMgr();   //SnodeList에 기본카메라 생성(0번이 됨)
		~HCCameraMgr();   //카메라 메모리 해지

};

