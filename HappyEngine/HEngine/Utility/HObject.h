//-------------------------------------------------------------------------------------
// 오브젝트 클래스
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

	int JCMODE3;////7. BSP 전체출력
				//8. BSP -오브젝트3 관점
				//9. BSP -오브젝트4 관점
				//10. BSP -오브젝트5 관점
	

};