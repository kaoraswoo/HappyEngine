//-------------------------------------------------------------------------------------
// ������Ʈ Ŭ����
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include <d3d9.h>

class HObject{
public:
	int Hnumber;
	D3DXVECTOR3 Pos;
	D3DXVECTOR3 Front;
	D3DXVECTOR3 Up;
	D3DXVECTOR3 Speed;
	D3DXVECTOR3 Size;
	float Vel;
	int MeshNumber;

	int JCMODE3;////7. BSP ��ü���
				//8. BSP -������Ʈ3 ����
				//9. BSP -������Ʈ4 ����
				//10. BSP -������Ʈ5 ����
	

};