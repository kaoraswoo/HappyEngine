#include "XMesh.h"

//--------------------------------------------------------------------------------------------
XMesh::XMesh(LPDIRECT3DDEVICE9 Device)
{

	m_pDevice=Device;
	
}

//--------------------------------------------------------------------------------------------
XMesh::~XMesh()
{

}

//--------------------------------------------------------------------------------------------
BOOL XMesh::LoadX(char *pName)
{

	LPD3DXBUFFER pD3DXMtrlBuffer;

	char textureFolder[300];
 
    // Load the mesh from the specified file
    if( FAILED( D3DXLoadMeshFromX( pName, D3DXMESH_SYSTEMMEM, 
		m_pDevice, NULL, 
                                   &pD3DXMtrlBuffer, NULL, &nMat, 
                                   &pMesh ) ) )
		return FALSE;

    // We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    if((pMat = new D3DMATERIAL9[nMat])==0)			return FALSE;
    if((pTex = new LPDIRECT3DTEXTURE9[nMat])==0)	return FALSE;

    for( DWORD i = 0; i < nMat; i++ )
    {
        // Copy the material
        pMat[i] = d3dxMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        pMat[i].Ambient = pMat[i].Diffuse;

        pTex[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL && 
            lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
        {
            strcpy(textureFolder,"..\\..\\Data\\xmesh\\");
			strcat(textureFolder,d3dxMaterials[i].pTextureFilename);
            // Create the texture
            if( FAILED( D3DXCreateTextureFromFile( m_pDevice, 
						textureFolder, 
						&pTex[i] ) ) )
				MessageBox(NULL, "Could not find texture map", "Meshes.exe", MB_OK);
        }
    }

    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

    return TRUE;

}

//--------------------------------------------------------------------------------------------
void XMesh::Disp(){
	//m_pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	//m_pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	    m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);

	for( DWORD i=0; i < nMat; i++ )
    {
        m_pDevice->SetMaterial( &pMat[i] );
        m_pDevice->SetTexture( 0, pTex[i] );
        pMesh->DrawSubset(i);
    }
}


//--------------------------------------------------------------------------------------------
XManager::XManager(RenderMgr* SceneRen):SceneNode(SceneRen)
{
	XMeshRender=SceneRen;
	
	XobjectFont=new HFont(XMeshRender->g_pd3dDevice);
	m_name="XMesh";
	m_dummy.GenerateObject();  //테스트용 및 제출용으로 임의 생성

}
//--------------------------------------------------------------------------------------------
XManager::~XManager()
{
	delete XobjectFont;
}
//--------------------------------------------------------------------------------------------
void XManager::draw()
{
}
//--------------------------------------------------------------------------------------------
void XManager::draw(ZFrustum* pFrustum)
{

	for(int j=0;j<30;j++)
	{
	   DrawSpecific(&m_dummy.objectX[j]);  //X파일오브젝트
	}

}
//--------------------------------------------------------------------------------------------
void XManager::SetMesh()
{
}

//--------------------------------------------------------------------------------------------
BOOL XManager::XLoad(char *pName)
{

	XMesh DumyX(XMeshRender->g_pd3dDevice);
	DumyX.LoadX(pName);
	XMeshList.push_back(DumyX);
    return TRUE;

}

//--------------------------------------------------------------------------------------------
void XManager::DrawSpecific(HObject *obj){
	
	char obnumber[15];
	D3DXMATRIXA16 s, t, r, w, MatView;
	
	D3DXMatrixScaling(&s, obj->Size.x, obj->Size.y, obj->Size.z);
	ChangeCoord(&r, &obj->Front, &obj->Up);
	D3DXMatrixTranslation(&t, obj->Pos.x, obj->Pos.y, obj->Pos.z);
	w = s * r * t;
	XMeshRender->g_pd3dDevice->SetTransform(D3DTS_WORLD, &w);
	
	XMeshList[obj->MeshNumber].Disp();

	//글자출력
	
	//num2str(obj->Hnumber,obnumber);
	sprintf_s(obnumber,"%d",obj->Hnumber);
	itoa(obj->Hnumber,obnumber,10);
	
	XMeshRender->g_pd3dDevice->GetTransform(D3DTS_VIEW, &MatView);
	D3DXVECTOR3 dragoon,dragoon2;
	dragoon.x=0;
	dragoon.y=0;
	dragoon.z=0;
	D3DXMatrixTranslation(&t, obj->Speed.x, obj->Speed.y, obj->Speed.z);
	D3DXVec3TransformCoord(&dragoon2,&dragoon,&t);
	XobjectFont->TextBillBoard(obnumber,dragoon2,&MatView);
	
}
