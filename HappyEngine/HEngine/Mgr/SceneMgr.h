//-------------------------------------------------------------------------------------
//! 씬매니저 클래스
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#ifndef _Renderer_h_
#define _Renderer_h_
#include "Renderer.h"
#endif

#include "HCCamera.h"
#include "Utility\ZXML.h"
#include "Utility\HObject.h"
#include "Utility\HFont.h"

struct sTriangle
{
	WORD	_0, _1, _2;		// 일반적으로 인덱스는 16비트의 크기를 갖는다.
							// 32비트의 크기도 가능하지만 구형 그래픽카드에서는 지원되지 않는다.
};


//-------------------------------------------------------------------------------------
class SceneNode{
public:
	std::string m_name;
	D3DXVECTOR3 Pos;    //월드상의 위치
	D3DXVECTOR3 Front;  //앞벡터
	D3DXVECTOR3 Up;     //Up벡터
	D3DXVECTOR3 Speed;  //속도
	D3DXVECTOR3 Size;   //크기(스케일)

	void NodeSetPos(float x,float y,float z);  //일단 위치설정함수
	D3DXMATRIX NodeMatrixReturn();


	//아래로는 상속할때 쓰는 것들(virtual)
	SceneNode(RenderMgr* SceneRen);
	virtual void draw()=0;
	virtual void SetMesh()=0;
	virtual void draw(ZFrustum* pFrustum)=0;
};


//-------------------------------------------------------------------------------------
class SceneMgr{
public:
	
	RenderMgr* RenInstance;//렌더러 인스턴스 가짐
	std::vector<SceneNode*> SnodeList;//노드리스트(벡터) 가짐
	std::vector<SceneNode*>::iterator SnodeIterator;
	std::map<std::string, SceneNode* > m_maplist;
	std::map<std::string, SceneNode* >::iterator SnodeMapitor;

	HCCameraMgr HCamera; //카메라관리 
	void SetCamera();
	void UpdateCamera();
	D3DXMATRIXA16 m_matProj;

	ZFrustum*	m_pFrustum;	// Frustum 클래스

    SceneMgr(); //지금은 생성자에 넣었지만 나중에는 따로 함수를 만들어서 렌더러에 메모리 할당..왜? 꼭 다이렉트x로 하는것이 아니거든..
    ~SceneMgr();
	void AddSceneNode(SceneNode* Snode);//씬노드 추가함수(SceneNode* Snode);
	void AddSceneMapNode(std::string name, SceneNode *Snode);
	//렌더러 인스턴스 할당함수 1.D3d  2. 쉐이더  3. 2D출력 등등....
	//렌더 함수 (각 씬노드 돌면서..)
	//동적오브젝트컬링 함수(오브젝트 그릴거 안그릴거 리스트)
	//


	//SnodeList 해지함수.. 함수포인터로 받았기에.. 일일히 다 Delete해줘야된다...
	                       //문제는 포인터로 넣기에 똑같은 거를 두번 Delete할수도 있다..
	                       //그래서 전제조건은 한포인터를 두번 Add하지 않는다고 가정해야한다..
	                       //싱글턴이라도 써야되나...

	

};



//-------------------------------------------------------------------------------------
class ObjectMgr:public SceneNode{
public:
	int ObjectNum;

	ObjectMgr(RenderMgr* SceneRen);
	void draw(){ObjectNum=2;
	printf("오브젝트넘버:%d ",ObjectNum);};
	virtual void SetMesh(){}

};


