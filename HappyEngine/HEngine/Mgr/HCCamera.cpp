//-------------------------------------------------------------------------------------
//! 카메라 생성
//! 해골 3d프로그래밍 참조 카메라 클래스 
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include "HCCamera.h"

//-------------------------------------------------------------------------------------
HCCameraMgr::HCCameraMgr()
{

	HCCameraNode *CameraList;
	CameraList=new HCCameraNode;
    CnodeList.push_back(CameraList);

}

//-------------------------------------------------------------------------------------
HCCameraMgr::~HCCameraMgr()
{

	for(CameraIter=CnodeList.begin();CameraIter!=CnodeList.end();){
		delete *CameraIter;
		CameraIter=CnodeList.erase(CameraIter++);

	}
}



//-------------------------------------------------------------------------------------
D3DXMATRIXA16* HCCameraNode::SetView( D3DXVECTOR3* pvEye,D3DXVECTOR3* pvLookat,D3DXVECTOR3* pvUp)
{
	//기본카메라 뷰매트릭스 세팅
	m_vEye=*pvEye;
	m_vLookat=*pvLookat;
	m_vUp=*pvUp;
	D3DXVec3Normalize( &m_vView, &( m_vLookat - m_vEye ) );//카메라 방향 벡터를 구함 (Lookat - eyept)
	D3DXVec3Cross( &m_vCross, &m_vUp, &m_vView );//측면 벡터, Up과 카메라 정면 벡터의 외적
	D3DXMatrixLookAtLH( &m_matView, &m_vEye, &m_vLookat, &m_vUp);//카메라 행렬

	//카메라 행렬의 역행렬 (빌보드)
	D3DXMatrixInverse( &m_matBill, NULL, &m_matView );
	m_matBill._41 = 0.0f;
	m_matBill._42 = 0.0f;
	m_matBill._43 = 0.0f;  //빌보드 여러개 활용시 이동 행렬
	return &m_matView;

}

//-------------------------------------------------------------------------------------
D3DXMATRIXA16* HCCameraNode::MoveLocalZ(float dist){

	D3DXVECTOR3 vNewEye=m_vEye;
	D3DXVECTOR3 vNewDst=m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove,&m_vView);
	vMove*=dist;
	vNewEye+=vMove;
	vNewDst+=vMove;
	
	return SetView(&vNewEye,&vNewDst,&m_vUp);

}

//-------------------------------------------------------------------------------------
D3DXMATRIXA16* HCCameraNode::MoveLocalX(float dist){

	D3DXVECTOR3 vNewEye=m_vEye;
	D3DXVECTOR3 vNewDst=m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove,&m_vCross);
	vMove*=dist;
	vNewEye+=vMove;
	vNewDst+=vMove;
	
	return SetView(&vNewEye,&vNewDst,&m_vUp);

}

//-------------------------------------------------------------------------------------
D3DXMATRIXA16* HCCameraNode::MoveLocalY(float dist){

	D3DXVECTOR3 vNewEye=m_vEye;
	D3DXVECTOR3 vNewDst=m_vLookat;

	D3DXVECTOR3 vMove;
	D3DXVec3Normalize(&vMove,&m_vUp);
	vMove*=dist;
	vNewEye+=vMove;
	vNewDst+=vMove;
	
	return SetView(&vNewEye,&vNewDst,&m_vUp);

}

//-------------------------------------------------------------------------------------
D3DXMATRIXA16* HCCameraNode::RotateLocalX( float angle )
{

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis( &matRot, &m_vCross, angle );

	//측변 벡터가 X벡터이므로 m_vCorss측 angle만큼 회전하는 행렬
	D3DXVECTOR3 vNewDst,vNewUp;
	D3DXVec3TransformCoord( &vNewDst, &m_vView, &matRot );

	// view 벡터를 matRot으로 변환된 벡터를 얻는다., 즉 카메라 방향 벡터를 matRot으로 회전된
	// 좌표로 변경한다.
	// D3DXVec3Cross( &vNewUp, &vNewDst, &m_vCross );
    // cross( dst, x축)으로 방향과 측 벡터로 Upvector도 새로 구함
	// D3DXVec3Normalize( &vNewUp, &vNewUp );     // up vector를 unit vector로...
	vNewDst += m_vEye;          // 실제 dst position =  eye Position + dst vector
	//기존 eyept + 변위 벡터 = New LookAt
	//단 SetView에서 카메라 방향 벡터를 새로구하므로 여기서는 생략

	return SetView( &m_vEye, &vNewDst, &m_vUp );
}

//-------------------------------------------------------------------------------------
D3DXMATRIXA16* HCCameraNode::RotateLocalY( float angle )
{

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis( &matRot, &m_vUp, angle );

	//측변 벡터가 X벡터이므로 m_vCorss측 angle만큼 회전하는 행렬
	D3DXVECTOR3 vNewDst,vNewUp;
	D3DXVec3TransformCoord( &vNewDst, &m_vView, &matRot );

	// view 벡터를 matRot으로 변환된 벡터를 얻는다., 즉 카메라 방향 벡터를 matRot으로 회전된
	// 좌표로 변경한다.
	// D3DXVec3Cross( &vNewUp, &vNewDst, &m_vCross );
	// cross( dst, x축)으로 방향과 측 벡터로 Upvector도 새로 구함
	// D3DXVec3Normalize( &vNewUp, &vNewUp );     // up vector를 unit vector로...
	vNewDst += m_vEye;          // 실제 dst position =  eye Position + dst vector
	//기존 eyept + 변위 벡터 = New LookAt
	//단 SetView에서 카메라 방향 벡터를 새로구하므로 여기서는 생략
	return SetView( &m_vEye, &vNewDst, &m_vUp );

}




