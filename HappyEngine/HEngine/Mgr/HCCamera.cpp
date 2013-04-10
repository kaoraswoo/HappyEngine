//-------------------------------------------------------------------------------------
//! ī�޶� ����
//! �ذ� 3d���α׷��� ���� ī�޶� Ŭ���� 
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
	//�⺻ī�޶� ���Ʈ���� ����
	m_vEye=*pvEye;
	m_vLookat=*pvLookat;
	m_vUp=*pvUp;
	D3DXVec3Normalize( &m_vView, &( m_vLookat - m_vEye ) );//ī�޶� ���� ���͸� ���� (Lookat - eyept)
	D3DXVec3Cross( &m_vCross, &m_vUp, &m_vView );//���� ����, Up�� ī�޶� ���� ������ ����
	D3DXMatrixLookAtLH( &m_matView, &m_vEye, &m_vLookat, &m_vUp);//ī�޶� ���

	//ī�޶� ����� ����� (������)
	D3DXMatrixInverse( &m_matBill, NULL, &m_matView );
	m_matBill._41 = 0.0f;
	m_matBill._42 = 0.0f;
	m_matBill._43 = 0.0f;  //������ ������ Ȱ��� �̵� ���
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

	//���� ���Ͱ� X�����̹Ƿ� m_vCorss�� angle��ŭ ȸ���ϴ� ���
	D3DXVECTOR3 vNewDst,vNewUp;
	D3DXVec3TransformCoord( &vNewDst, &m_vView, &matRot );

	// view ���͸� matRot���� ��ȯ�� ���͸� ��´�., �� ī�޶� ���� ���͸� matRot���� ȸ����
	// ��ǥ�� �����Ѵ�.
	// D3DXVec3Cross( &vNewUp, &vNewDst, &m_vCross );
    // cross( dst, x��)���� ����� �� ���ͷ� Upvector�� ���� ����
	// D3DXVec3Normalize( &vNewUp, &vNewUp );     // up vector�� unit vector��...
	vNewDst += m_vEye;          // ���� dst position =  eye Position + dst vector
	//���� eyept + ���� ���� = New LookAt
	//�� SetView���� ī�޶� ���� ���͸� ���α��ϹǷ� ���⼭�� ����

	return SetView( &m_vEye, &vNewDst, &m_vUp );
}

//-------------------------------------------------------------------------------------
D3DXMATRIXA16* HCCameraNode::RotateLocalY( float angle )
{

	D3DXMATRIXA16 matRot;
	D3DXMatrixRotationAxis( &matRot, &m_vUp, angle );

	//���� ���Ͱ� X�����̹Ƿ� m_vCorss�� angle��ŭ ȸ���ϴ� ���
	D3DXVECTOR3 vNewDst,vNewUp;
	D3DXVec3TransformCoord( &vNewDst, &m_vView, &matRot );

	// view ���͸� matRot���� ��ȯ�� ���͸� ��´�., �� ī�޶� ���� ���͸� matRot���� ȸ����
	// ��ǥ�� �����Ѵ�.
	// D3DXVec3Cross( &vNewUp, &vNewDst, &m_vCross );
	// cross( dst, x��)���� ����� �� ���ͷ� Upvector�� ���� ����
	// D3DXVec3Normalize( &vNewUp, &vNewUp );     // up vector�� unit vector��...
	vNewDst += m_vEye;          // ���� dst position =  eye Position + dst vector
	//���� eyept + ���� ���� = New LookAt
	//�� SetView���� ī�޶� ���� ���͸� ���α��ϹǷ� ���⼭�� ����
	return SetView( &m_vEye, &vNewDst, &m_vUp );

}




