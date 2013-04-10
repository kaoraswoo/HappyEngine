//-------------------------------------------------------------------------------------
//! ���Ŵ��� Ŭ����
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
	WORD	_0, _1, _2;		// �Ϲ������� �ε����� 16��Ʈ�� ũ�⸦ ���´�.
							// 32��Ʈ�� ũ�⵵ ���������� ���� �׷���ī�忡���� �������� �ʴ´�.
};


//-------------------------------------------------------------------------------------
class SceneNode{
public:
	std::string m_name;
	D3DXVECTOR3 Pos;    //������� ��ġ
	D3DXVECTOR3 Front;  //�պ���
	D3DXVECTOR3 Up;     //Up����
	D3DXVECTOR3 Speed;  //�ӵ�
	D3DXVECTOR3 Size;   //ũ��(������)

	void NodeSetPos(float x,float y,float z);  //�ϴ� ��ġ�����Լ�
	D3DXMATRIX NodeMatrixReturn();


	//�Ʒ��δ� ����Ҷ� ���� �͵�(virtual)
	SceneNode(RenderMgr* SceneRen);
	virtual void draw()=0;
	virtual void SetMesh()=0;
	virtual void draw(ZFrustum* pFrustum)=0;
};


//-------------------------------------------------------------------------------------
class SceneMgr{
public:
	
	RenderMgr* RenInstance;//������ �ν��Ͻ� ����
	std::vector<SceneNode*> SnodeList;//��帮��Ʈ(����) ����
	std::vector<SceneNode*>::iterator SnodeIterator;
	std::map<std::string, SceneNode* > m_maplist;
	std::map<std::string, SceneNode* >::iterator SnodeMapitor;

	HCCameraMgr HCamera; //ī�޶���� 
	void SetCamera();
	void UpdateCamera();
	D3DXMATRIXA16 m_matProj;

	ZFrustum*	m_pFrustum;	// Frustum Ŭ����

    SceneMgr(); //������ �����ڿ� �־����� ���߿��� ���� �Լ��� ���� �������� �޸� �Ҵ�..��? �� ���̷�Ʈx�� �ϴ°��� �ƴϰŵ�..
    ~SceneMgr();
	void AddSceneNode(SceneNode* Snode);//����� �߰��Լ�(SceneNode* Snode);
	void AddSceneMapNode(std::string name, SceneNode *Snode);
	//������ �ν��Ͻ� �Ҵ��Լ� 1.D3d  2. ���̴�  3. 2D��� ���....
	//���� �Լ� (�� ����� ���鼭..)
	//����������Ʈ�ø� �Լ�(������Ʈ �׸��� �ȱ׸��� ����Ʈ)
	//


	//SnodeList �����Լ�.. �Լ������ͷ� �޾ұ⿡.. ������ �� Delete����ߵȴ�...
	                       //������ �����ͷ� �ֱ⿡ �Ȱ��� �Ÿ� �ι� Delete�Ҽ��� �ִ�..
	                       //�׷��� ���������� �������͸� �ι� Add���� �ʴ´ٰ� �����ؾ��Ѵ�..
	                       //�̱����̶� ��ߵǳ�...

	

};



//-------------------------------------------------------------------------------------
class ObjectMgr:public SceneNode{
public:
	int ObjectNum;

	ObjectMgr(RenderMgr* SceneRen);
	void draw(){ObjectNum=2;
	printf("������Ʈ�ѹ�:%d ",ObjectNum);};
	virtual void SetMesh(){}

};


