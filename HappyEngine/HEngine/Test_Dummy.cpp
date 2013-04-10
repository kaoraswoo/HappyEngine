#include "Test_Dummy.h"

void TestDummy::GenerateObject()
	{

	/////////////////////////////////////////////// 제출용으로 아래의 오브젝트들 생성(아무리 생성해도 메시는 2개 로딩해서 쓰기 때문에 부담없음)
		//Object생성 좌표축에 일렬로 30개
		for(int obcount=0;obcount<10;obcount++){
		objectX[obcount].Pos= D3DXVECTOR3(obcount*4,-2,-0.5);
		objectX[obcount].Front = D3DXVECTOR3(0,1,0);
		objectX[obcount].Up = D3DXVECTOR3(0,0,1);
		objectX[obcount].Speed = D3DXVECTOR3(obcount*4,1,-0.5);
		objectX[obcount].Vel = 0;
		objectX[obcount].Size = D3DXVECTOR3(0.01f,0.01f,0.01f);
		objectX[obcount].MeshNumber=1;
		objectX[obcount].Hnumber=obcount;
		}
		for(int obcount=10;obcount<20;obcount++){
			objectX[obcount].Pos= D3DXVECTOR3((obcount-10)*4,-4,-4);
			objectX[obcount].Front = D3DXVECTOR3(0,0,1);
			objectX[obcount].Up = D3DXVECTOR3(0,1,0);
			objectX[obcount].Speed = D3DXVECTOR3((obcount-10)*4,-2,-4);
			objectX[obcount].Vel = 0;
			objectX[obcount].Size = D3DXVECTOR3(1.0f,1.0f,1.0f);
			objectX[obcount].MeshNumber=0;
			objectX[obcount].Hnumber=obcount;
		}
		for(int obcount=20;obcount<30;obcount++){
			objectX[obcount].Pos= D3DXVECTOR3((obcount-20)*4,-4,-6);
			objectX[obcount].Front = D3DXVECTOR3(0,0,1);
			objectX[obcount].Up = D3DXVECTOR3(0,1,0);
			objectX[obcount].Speed = D3DXVECTOR3((obcount-20)*4,-2,-6);
			objectX[obcount].Vel = 0;
			objectX[obcount].Size = D3DXVECTOR3(1.0f,1.0f,1.0f);
			objectX[obcount].MeshNumber=0;
			objectX[obcount].Hnumber=obcount;
		}
		objectX[20].MeshNumber=0;
		objectX[20].Size = D3DXVECTOR3(2.0f,2.0f,2.0f);
		objectX[20].Speed = D3DXVECTOR3(8,-1,-15);
		objectX[20].Pos= D3DXVECTOR3(8,-4,-15);


		//포탈 가시검사용 - 오브젝트0-1-2
	
		objectX[0].MeshNumber=1;
		objectX[0].Speed = D3DXVECTOR3(-29.26f,5.05f,-3.51f);
		objectX[0].Pos= D3DXVECTOR3(-29.26f,2.05f,-3.51f);
	    
	
		objectX[1].MeshNumber=1;
		objectX[1].Speed = D3DXVECTOR3(-57.30f,2.11f,-60.14f);
		objectX[1].Pos= D3DXVECTOR3(-57.30f,-1.11f,-60.14f);

		objectX[2].MeshNumber=1;
		objectX[2].Speed = D3DXVECTOR3(-75.13f,5.77f,-28.90f);
		objectX[2].Pos= D3DXVECTOR3(-75.13f,2.77f,-28.90f);
}

