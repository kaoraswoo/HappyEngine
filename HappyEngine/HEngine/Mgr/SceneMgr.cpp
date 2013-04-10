#include "SceneMgr.h"





void SceneNode::NodeSetPos(float x, float y, float z){
	Pos.x=x;
	Pos.y=y;
	Pos.z=z;
}

D3DXMATRIX SceneNode::NodeMatrixReturn(){
	D3DXMATRIX t;

	D3DXMatrixTranslation(&t, Pos.x, Pos.y, Pos.z);

	

	return t;

}

//Frustum
SceneMgr::SceneMgr()
{
	RenInstance=new RenderMgr;
	printf(" ���Ŵ��������� ");
	m_pFrustum = new ZFrustum();
}

SceneMgr::~SceneMgr(){   //�ϴ��� �ӽù������� ��ü �� ����..
	delete m_pFrustum;
	delete RenInstance;
	for(SnodeIterator=SnodeList.begin();SnodeIterator!=SnodeList.end();)
	{
		delete *SnodeIterator;
		SnodeIterator=SnodeList.erase(SnodeIterator++);

	}
}

void SceneMgr::AddSceneNode(SceneNode *Snode){
	
	//vector
	printf("�ֵ忡��:");
	SnodeList.push_back(Snode);
}

void SceneMgr::AddSceneMapNode(std::string name, SceneNode *Snode){
	//map
	m_maplist.insert( std::pair<std::string, SceneNode*>( name, Snode ) );
}
/*
std::string SceneMgr::GetSceneNode()
{
	for( SnodeMapitor = m_maplist.begin(); SnodeMapitor != m_maplist.end() ; SnodeMapitor++ )
	{

	}
}
*/
SceneNode::SceneNode(RenderMgr* SceneRen){

}


ObjectMgr::ObjectMgr(RenderMgr* SceneRen):SceneNode(SceneRen){
}


void SceneMgr::SetCamera()
{
	D3DXVECTOR3 vEyePt( -70.0f, 70.0f,-70.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    HCamera.CnodeList[0]->SetView(&vEyePt, &vLookatPt, &vUpVec);
	D3DXMatrixPerspectiveFovLH( &m_matProj, D3DX_PI/4, 1.0f, 1.0f, 1000.0f );
	RenInstance->g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );

	D3DXMATRIXA16* pmatView = HCamera.CnodeList[0]->GetViewMatrix();  // ī�޶� ����� ��´�.
	RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // ī�޶� ��� ����

}

void SceneMgr::UpdateCamera()
{
		D3DXMATRIXA16 m_matView;
		D3DXMATRIXA16	m;
		D3DXMATRIXA16	*pView;
		pView = HCamera.CnodeList[0]->GetViewMatrix();	// ī�޶� Ŭ�����κ��� ��������� ��´�.
		m = *pView * m_matProj;				// World��ǥ�� ������ؼ� View * Proj����� ����Ѵ�.
		m_pFrustum->Make( &m );	// View*Proj��ķ� Frustum�� �����.
		m_pFrustum->ViewSet(pView);
		//m_pFrustum->m_vTempPos = HCamera.CnodeList[0]->GetP
		//m_pFrustum->ProjSet( &m_matProj );
		m_pFrustum->m_vTempPos = HCamera.CnodeList[0]->m_vEye;
		m_pFrustum->m_vLookAt = HCamera.CnodeList[0]->m_vLookat;
		m_pFrustum->m_vUpPos = HCamera.CnodeList[0]->m_vUp;
		m_pFrustum->m_vTempCamera = *pView;
}
