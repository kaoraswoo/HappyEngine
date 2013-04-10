//-------------------------------------------------------------------------------------
//! //PVS 포털 생성 클래스
//! BSP 부분에서 수정(BSP 로드부분 발췌함)
//! 발췌 한 곳 - http://blog.naver.com/ryanii?Redirect=Log&logNo=20017051226
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------


#include "PvsBsp.h"

//-------------------------------------------------------------------------------------
void ErrLog(char *str)
{
	FILE *fp = fopen("ErrLog.txt", "a+");
	fprintf(fp, "%s", str);
	fclose(fp);
}

//-------------------------------------------------------------------------------------
CBSP::CBSP(RenderMgr* SceneRen):SceneNode(SceneRen)
{
		//씬노드 추가 및 멤버변수 초기화
		pDevice=SceneRen->g_pd3dDevice;
		m_nNumVerts			= 0;
		m_nNumFaces			= 0;
		m_nNumMeshIndices	= 0;
		m_nNumTextures		= 0;
		m_nNumNodes			= 0;
		m_nNumLeafs			= 0;
		m_nNumPlanes		= 0;
		m_nNumLeafFaces		= 0;
		m_nNumVisData		= 0;
		m_nNumTrisDraw		= 0;

	
		m_pFaces			= NULL;
		m_pVerts			= NULL;
		m_pMeshIndices		= NULL;
		m_pTextures			= NULL;
		m_pNodes			= NULL;
		m_pLeafs			= NULL;
		m_pPlanes			= NULL;
		m_pLeafFaces		= NULL;
		m_VisData.pBits		= NULL;
		m_bFaceVisibility   = NULL;

		m_pVB				= NULL;
		m_pIB				= NULL;
		m_pT				= NULL;

		m_name="PVS - BSP";
		m_selectObjectNum=0;
		tempobjectX[0].Pos=  D3DXVECTOR3(-29.26f,2.05f,-3.51f);
		tempobjectX[1].Pos=  D3DXVECTOR3(-57.30f,-1.11f,-60.14f);
		tempobjectX[2].Pos=  D3DXVECTOR3(-75.13f,2.77f,-28.90f);

}

//-------------------------------------------------------------------------------------
void CBSP::draw(){
	
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 rotateWorld;
	D3DXMATRIXA16 tranWorld;
	//To do: 임시 위치 조정(하드코딩)
    D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matWorld,0.04f,0.04f,0.04f);
	D3DXMatrixRotationX( &rotateWorld, -D3DX_PI / 2 );
	D3DXMatrixTranslation(&tranWorld,0.0f,0.0f,0.0f);
    
    pDevice->SetTransform( D3DTS_WORLD, &(matWorld*rotateWorld*tranWorld) );
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	
	Render();

    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

//-------------------------------------------------------------------------------------
void CBSP::draw(ZFrustum* pFrustum){
	/*
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 rotateWorld;
	D3DXMATRIXA16 tranWorld;
	D3DXMATRIXA16 g_matProj;
	D3DXMATRIXA16	m;

    D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matWorld,0.04f,0.04f,0.04f);
	D3DXMatrixRotationX( &rotateWorld, -D3DX_PI / 2 );
	D3DXMatrixTranslation(&tranWorld,0.0f,0.0f,0.0f);

	g_matProj = matWorld*rotateWorld*tranWorld;
    
	D3DXVECTOR3 p1;
    D3DXVECTOR3 p2=pFrustum->m_vPos;
	D3DXVec3TransformCoord(&p1,&p2,&g_matProj);
	D3DXVECTOR3 p3;
	D3DXVECTOR3 p4=pFrustum->m_vPos2*500;
	D3DXVec3TransformCoord(&p3,&p4,&g_matProj);

    pDevice->SetTransform( D3DTS_WORLD, &(matWorld*rotateWorld*tranWorld) );
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	
	//CheckNodeForLine(pFrustum->m_vPos,pFrustum->m_vPos2*500);
	CheckMultipleLines(p1,p3,500);

    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	*/

	if(m_selectObjectNum >2 )
	{
		//전부 그리기
		draw();
	}else
	{
		//유닛 위치에 따라서 컬링 draw
		draw4(&tempobjectX[m_selectObjectNum]);
	}
}

//-------------------------------------------------------------------------------------
// BSP맵 로드 하드코딩
void CBSP::SetMesh(){
  Create("../../Data/maps/tutorial.bsp");
}

//-------------------------------------------------------------------------------------
// 카메라 정보에 의한 컬링 그리기
void CBSP::draw3(HCCameraNode* CameraInfo){

	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 rotateWorld;
	D3DXMATRIXA16 tranWorld;
	D3DXMATRIXA16 g_matProj;
	D3DXMATRIXA16	m;
	D3DXMATRIXA16 matInv;
	
    D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matWorld,0.04f,0.04f,0.04f);
	D3DXMatrixRotationX( &rotateWorld, -D3DX_PI / 2 );
	D3DXMatrixTranslation(&tranWorld,0.0f,0.0f,0.0f);

		
    g_matProj = matWorld*rotateWorld*tranWorld;

	D3DXMatrixInverse(&matInv, NULL, &g_matProj );

	D3DXVECTOR3 p1;
    D3DXVECTOR3 p2=CameraInfo->m_vEye;
	D3DXVec3TransformCoord(&p1,&p2,&matInv);
	D3DXVECTOR3 p3;
	D3DXVECTOR3 p4=CameraInfo->m_vLookat;
	D3DXVec3TransformCoord(&p3,&p4,&matInv);

	pDevice->SetTransform( D3DTS_WORLD, &(g_matProj) );
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	//CheckMultipleLines(p1,p3,5000);
	//카메라 위치에 따라서 클러스터 체크
	CheckClusterPoint(p1);

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

}

//-------------------------------------------------------------------------------------
D3DXVECTOR3 CBSP::TransWMatrix(D3DXVECTOR3 PreVec){
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 rotateWorld;
	D3DXMATRIXA16 tranWorld;
	D3DXMATRIXA16 g_matProj;
	D3DXMATRIXA16	m;
	D3DXMATRIXA16 matInv;
	
    D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matWorld,0.04f,0.04f,0.04f);
	D3DXMatrixRotationX( &rotateWorld, -D3DX_PI / 2 );
	D3DXMatrixTranslation(&tranWorld,0.0f,0.0f,0.0f);
		
    g_matProj = matWorld*rotateWorld*tranWorld;
	pDevice->SetTransform( D3DTS_WORLD, &(g_matProj) );

	D3DXMatrixInverse(&matInv, NULL, &g_matProj );

	D3DXVECTOR3 p1;
    D3DXVECTOR3 p2=PreVec;
	D3DXVec3TransformCoord(&p1,&p2,&matInv);
	return p1;

}

//-------------------------------------------------------------------------------------
//오브젝트 위치에 따른 클러스트 컬링 후 그리기
void CBSP::draw4(HObject *obj)
{
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	CheckClusterPoint(TransWMatrix(obj->Pos));
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//-------------------------------------------------------------------------------------
bool CBSP::Release()
	{
		delete[] m_pVerts;
		m_pVerts=NULL;

		delete[] m_pFaces;
		m_pFaces=NULL;

		delete[] m_pMeshIndices;
		m_pMeshIndices=NULL;

		delete[] m_pTextures;
		m_pTextures=NULL;
		
		delete[] m_pNodes;
		m_pNodes=NULL;

		delete[] m_pLeafs;
		m_pLeafs=NULL;

		delete[] m_pPlanes;
		m_pPlanes=NULL;

		delete[] m_pLeafFaces;
		m_pLeafFaces=NULL;

		delete[] m_VisData.pBits;
		m_VisData.pBits		 =NULL;
		m_VisData.NumVectors =0;
		m_VisData.nSizeVectorInBytes=0;

		delete[] m_bFaceVisibility;
		m_bFaceVisibility=NULL;

		SafeRelease(m_pVB);
		SafeRelease(m_pIB);

		if( m_pT!=NULL )
		{
			for(int i=0; i<m_nNumTextures; i++)
			{
				SafeRelease(m_pT[i]);
			}
			delete[] m_pT;
			m_pT=NULL;
		}

		m_nNumVerts = 0;
		m_nNumFaces = 0;
		m_nNumMeshIndices = 0;
		m_nNumTextures = 0;
		m_nNumNodes	= 0;
		m_nNumLeafs = 0;
		m_nNumPlanes = 0;
		m_nNumLeafFaces = 0;
		m_nNumVisData = 0;
		m_nNumTrisDraw = 0;
		return true;					
	}

//-------------------------------------------------------------------------------------
bool CBSP::LoadVerticeData(FILE* fp,
						 stLump * pHeaderLump)
	{
		ErrLog("<->Start Loading Vertices Data\n");

		stLump * pLump = &pHeaderLump[enVertices];
		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		m_pVerts = (stVertex*)new char[pLump->nFileLen];
		assert(m_pVerts);

		fread(	(void*)m_pVerts,		
				1,						
				pLump->nFileLen,		
				fp );					

		
		m_nNumVerts = pLump->nFileLen / sizeof(stVertex); 

		
		char buf[100];					
		sprintf_s(buf, "<?>Num Vertices: %d\n", m_nNumVerts);
		ErrLog(buf);

		ErrLog("<->End Loading Vertices Data\n");
		return true;
	}

//-------------------------------------------------------------------------------------
bool CBSP::LoadFacesData(FILE* fp,
					   stLump * pHeaderLump)
	{
		ErrLog("<->Start Loading Faces Data\n");

		stLump * pLump = &pHeaderLump[enFaces];
		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		m_pFaces = (stFace*)new char[pLump->nFileLen];
		assert(m_pFaces);

		fread(	(void*)m_pFaces,		
				pLump->nFileLen,		
				1,						
				fp );					


		m_nNumFaces = pLump->nFileLen / sizeof(stFace); 

		
		char buf[200]; 
		sprintf_s(buf, "<?>Num Faces: %d\n", m_nNumFaces);
		ErrLog(buf);

		ErrLog("<->End Loading Vertices Data\n");
		return true;
	}

//-------------------------------- Mesh Vert Tris -------------------------------------//
	bool CBSP::LoadMeshVertsData(	FILE* fp,
							stLump * pHeaderLump)
	{
		ErrLog("<->Start MeshVert Data\n");

		stLump * pLump = &pHeaderLump[enMeshIndices];
		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		m_pMeshIndices = (int*)new char[pLump->nFileLen];
		assert(m_pMeshIndices);

		fread(	(void*)m_pMeshIndices,	
				pLump->nFileLen,		
				1,						
				fp );					


		m_nNumMeshIndices = pLump->nFileLen / sizeof(int); 

		
		char buf[200]; 
		sprintf_s(buf, "<?>Num Mesh Verts: %d\n", m_nNumMeshIndices);
		ErrLog(buf);

		ErrLog("<->End MeshVert Data\n");
		return true;
	}
	//-------------------------------------------------------------------------------------
	bool CBSP::LoadTextureData(	FILE* fp,
							stLump * pHeaderLump)
	{
		ErrLog("<->Start Loading Texture Data\n");

		stLump * pLump = &pHeaderLump[enTextures];
		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		m_pTextures = (stTexture*)new char[pLump->nFileLen];
		assert(m_pTextures);

		fread(	(void*)m_pTextures,		
				pLump->nFileLen,		
				1,						
				fp );					


		m_nNumTextures = pLump->nFileLen / sizeof(stTexture); 

		
		char buf[200]; 
		sprintf_s(buf, "<?>Num Textures: %d\n", m_nNumTextures);
		ErrLog(buf);

		ErrLog("<->End Loading Texture Data\n");

		return true;
	}

	//-------------------------------------------------------------------------------------
	bool CBSP::LoadNodesData(	FILE* fp,
						stLump * pHeaderLump)
	{
		ErrLog("<->Start Loading Nodes Data\n");

		stLump * pLump = &pHeaderLump[enNodes];
		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		m_pNodes = (stNode*)new char[pLump->nFileLen];
		assert(m_pNodes);

		fread(	(void*)m_pNodes,		
				pLump->nFileLen,		
				1,						
				fp );					


		m_nNumNodes = pLump->nFileLen / sizeof(stNode); 

		
		char buf[200]; 
		sprintf_s(buf, "<?>Num Nodes: %d\n", m_nNumNodes);
		ErrLog(buf);

		ErrLog("<->End Loading Nodes Data\n");
		return true;
	}
	//-------------------------------------------------------------------------------------
	bool CBSP::LoadLeafsData(	FILE* fp,
						stLump * pHeaderLump)
	{
		ErrLog("<->Start Loading Leafs Data\n");

		stLump * pLump = &pHeaderLump[enLeaves];

		m_nNumLeafs = pLump->nFileLen / sizeof(stLeaf);

		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		m_pLeafs = (stLeaf*)new char[pLump->nFileLen];
		assert(m_pLeafs);

		fread(	(void*)m_pLeafs,		
				sizeof(stLeaf),			
				m_nNumLeafs,			
				fp );					


		 

		
		char buf[200]; 
		sprintf_s(buf, "<?>Num Leafs: %d\n", m_nNumLeafs);
		ErrLog(buf);

		ErrLog("<->End Loading Leafs Data\n");
		return true;
	}
	//-------------------------------------------------------------------------------------
	bool CBSP::LoadPlanesData( FILE* fp,
						 stLump * pHeaderLump)
	{
		ErrLog("<->Start Loading Planes Data\n");

		stLump * pLump = &pHeaderLump[enPlanes];
		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		m_pPlanes = (stPlane*)new char[pLump->nFileLen];
		assert(m_pPlanes);

		fread(	(void*)m_pPlanes,		
				pLump->nFileLen,		
				1,						
				fp );					


		m_nNumPlanes = pLump->nFileLen / sizeof(stPlane); 

		
		char buf[200]; 
		sprintf_s(buf, "<?>Num Planes: %d\n", m_nNumPlanes);
		ErrLog(buf);

		ErrLog("<->End Loading Planes Data\n");
		return true;
	}
	//-------------------------------------------------------------------------------------
	bool CBSP::LoadLeafFacesData( FILE* fp,
						 stLump * pHeaderLump)
	{
		ErrLog("<->Start Loading LeafFaces Data\n");

		stLump * pLump = &pHeaderLump[enLeafFaces];
		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		m_pLeafFaces = (int*)new char[pLump->nFileLen];
		assert(m_pLeafFaces);

		fread(	(void*)m_pLeafFaces,	
				pLump->nFileLen,		
				1,						
				fp );					


		m_nNumLeafFaces = pLump->nFileLen / sizeof(int); 

		
		char buf[200]; 
		sprintf_s(buf, "<?>Num LeafFaces: %d\n", m_nNumLeafFaces);
		ErrLog(buf);

		ErrLog("<->End Loading LeafFaces Data\n");
		return true;
	}
	//-------------------------------------------------------------------------------------
	bool CBSP::LoadVisData( FILE* fp,
					  stLump * pHeaderLump)
	{
		ErrLog("<->Start Loading VisData Data\n");

		stLump * pLump = &pHeaderLump[enVisData];
		fseek(	fp,
				pLump->nFileofs,
				SEEK_SET );

		fread(	(void*)&m_VisData,		
				sizeof(int),			
				2,						
				fp );					

		m_nNumVisData = m_VisData.NumVectors;

		int size = (m_VisData.NumVectors * m_VisData.nSizeVectorInBytes);

		m_VisData.pBits = new unsigned char[size];
		assert(m_VisData.pBits);

		

		fread((void*)(m_VisData.pBits),		
		sizeof(byte),						
		size,								
		fp );								


		
		char buf[200]; 
		sprintf_s(buf, "<?>Num VisData: %d\n", m_nNumVisData);
		ErrLog(buf);

		ErrLog("<->End Loading VisData Data\n");
		return true;
	}

	//-------------------------------------------------------------------------------------
	bool CBSP::CreateDXTextures()
	{
		HRESULT hr = S_OK;				
		if( m_nNumTextures==0 )
		{
			ErrLog("<->No Textures To Load\n");
			return true;
		}

		m_pT = new IDirect3DTexture9*[m_nNumTextures];

		for(int i=0; i<m_nNumTextures; i++)
		{
			if( m_pTextures[i].nContentFlags != 1 )
			{
				m_pT[i] = NULL;
				continue;
			}

			
			char * str = NULL;
			int len = 0;
			
			if( m_pTextures[i].Name==NULL )
			{
				m_pT[i]=NULL;
				continue;
			}

			len = (int)strlen(m_pTextures[i].Name);
			if( len<=1 )
			{
				m_pT[i]=NULL;
				continue;
			}
			
			
			str = new char[len+17]; // +4 is for "../../Data/  .jpg"
			strcpy(str, "../../Data/");
			strcat(str, m_pTextures[i].Name);
			strcat(str, ".jpg");
			

			IDirect3DTexture9* pTexture = NULL;
			hr =
			D3DXCreateTextureFromFile(	pDevice,				// DirectX Device
										str,					// File Name
										&pTexture );			// Loaded Texture
			if( hr==S_OK )
				m_pT[i] = pTexture;
			else
				m_pT[i] = NULL;

			delete[] str;
		}

		return true;
	}
	//-------------------------------------------------------------------------------------
	bool CBSP::CreateDXIndexBuffer()
	{
		HRESULT hr = S_OK;

		hr =
		pDevice->CreateIndexBuffer( m_nNumMeshIndices * sizeof(WORD),
									D3DUSAGE_WRITEONLY,
									D3DFMT_INDEX16,
									D3DPOOL_DEFAULT,
									&m_pIB,
									NULL);
		if( hr != S_OK )
		{
			ErrLog("<!>Error Creating Index Buffer/n");
			return false;
		}

		
		WORD * pIB;
		m_pIB->Lock(0, 
					m_nNumMeshIndices*sizeof(WORD), 
					(void**)&pIB, 
					0);
		for(int i=0; i<m_nNumMeshIndices; i++)
			pIB[i] = m_pMeshIndices[i];
		m_pIB->Unlock();

		return true;
	}
	//-------------------------------------------------------------------------------------
	bool CBSP::CreateDXVertexBuffer()
	{
		HRESULT hr = S_OK;

		m_FVF_Format = D3DFVF_XYZ | 
					   D3DFVF_NORMAL |
					   D3DFVF_DIFFUSE |
					   D3DFVF_TEX2 |
					   D3DFVF_TEXCOORDSIZE2(0) |
					   D3DFVF_TEXCOORDSIZE2(1);
		hr =
		pDevice->CreateVertexBuffer( m_nNumVerts * sizeof(stBSPVertex),
									 0,
									 m_FVF_Format,
									 D3DPOOL_MANAGED,
									 &m_pVB,
									 NULL );
		if( hr != S_OK )
		{
			ErrLog("<!>Error Creating Vertex Buffer/n");
			return false;
		}

		
		stBSPVertex *pVB;
		m_pVB->Lock(0,0, (void**)&pVB, 0);

		for(int i=0; i<m_nNumVerts; i++)
		{
			
			pVB[i].p.x = m_pVerts[i].vPoint[0];
			pVB[i].p.y = m_pVerts[i].vPoint[1];
			pVB[i].p.z = m_pVerts[i].vPoint[2];

			
			pVB[i].tu1 = m_pVerts[i].Tex[0];
			pVB[i].tv1 = m_pVerts[i].Tex[1];
			pVB[i].lu2 = m_pVerts[i].LightTexCoord[0];
			pVB[i].lv2 = m_pVerts[i].LightTexCoord[1];

			pVB[i].n.x   = m_pVerts[i].vNormal[0];
			pVB[i].n.y   = m_pVerts[i].vNormal[1];
			pVB[i].n.z   = m_pVerts[i].vNormal[2];

			
			pVB[i].colour =   0x10ffffff & m_pVerts->RGBA; // ARGB
		}
		m_pVB->Unlock();


		return true;
	}
	//-------------------------------------------------------------------------------------
	bool CBSP::Create(const char *szBSPFile){


		if( szBSPFile==NULL )
		{
			ErrLog("<->Error Invalid File Name in BSP::Create(..)");
			return false;
		}

		Release();

		ErrLog("\n<->Opening File\n");

		
		FILE * fp = fopen(szBSPFile, "rb");

		char buf[200]; 

		if( fp==NULL )
		{
			sprintf(buf, 
				    "\n<?>Unable to open file: %s\n", 
					szBSPFile);
			ErrLog(buf);
			return false;
		}

		
		fread(&m_Header,				
			1,							
			sizeof(stHeader),			
			fp);						

		
		stLump * pLumps = m_Header.Lumps;


	
		if( !LoadVerticeData(fp, pLumps)	) return false;
		
		if( !LoadFacesData(fp, pLumps)		) return false;

		if( !LoadMeshVertsData(fp, pLumps)	) return false;

		if( !LoadTextureData(fp, pLumps)	) return false;

		if( !CreateDXVertexBuffer()	) return false;

		if( !CreateDXIndexBuffer()	) return false;

		if( !CreateDXTextures()		) return false;
		
		

		if( !LoadNodesData(fp, pLumps)		) return false;
		
		if( !LoadLeafsData(fp, pLumps)		) return false;

		if( !LoadPlanesData(fp, pLumps)		) return false;

		if( !LoadLeafFacesData(fp, pLumps)	) return false;

		if( !LoadVisData(fp, pLumps)		) return false;

		
		m_bFaceVisibility = new bool[m_nNumFaces];
		assert(m_bFaceVisibility);

		ErrLog("\n<->Closing File\n");
	
		fclose(fp);

		return true;					// No Error
	}


	//-------------------------------------------------------------------------------------
	void CBSP::Render()
	{
	//#if(1)
		
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	//#endif
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
		pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	    pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);



	
		
		pDevice->SetIndices(m_pIB);

		pDevice->SetStreamSource(0, m_pVB, 0, sizeof(stBSPVertex));

		pDevice->SetFVF( m_FVF_Format );


		for(int i=0; i<m_nNumFaces; i++)
		{
			
			if( m_pFaces[i].nFaceType != 1 ) continue;

			int nFirstF = m_pFaces[i].nFirstMeshVerts;
			int nNumF   = m_pFaces[i].NumMeshVerts;

			int nFirstV = m_pFaces[i].FirstVert;
			int nNumV   = m_pFaces[i].NumVerts;

			int nTexIndex = m_pFaces[i].nTexture;
	//#if(0)
			pDevice->SetTexture( 0, 
								 m_pT[nTexIndex] );
	//#endif

			int nNumTris = nNumF / 3;

			m_nNumTrisDraw += nNumTris;

			pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,		
					                        nFirstV,				
											0,						
											nNumV,					
											nFirstF,				
											nNumTris );				
		}
	}


//-------------------------------------------------------------------------------------
void CBSP::RenderFaces(
					 int nStartFace,
					 int nNumFaces)
	{

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG2);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,	D3DTA_TEXTURE);
		
		pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); 
		pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	    pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);

		pDevice->SetIndices(m_pIB);

		pDevice->SetStreamSource(0, m_pVB, 0, sizeof(stBSPVertex));

		pDevice->SetFVF( m_FVF_Format );


		for(int i=nStartFace; i<nStartFace+nNumFaces; i++)
		{
			
			if( m_pFaces[i].nFaceType != 1)continue;

			int nFirstF = m_pFaces[i].nFirstMeshVerts;
			int nNumF   = m_pFaces[i].NumMeshVerts;

			int nFirstV = m_pFaces[i].FirstVert;
			int nNumV   = m_pFaces[i].NumVerts;

			int nTexIndex = m_pFaces[i].nTexture;
			pDevice->SetTexture( 0, 
								 m_pT[nTexIndex] );

			int nNumTris = nNumF / 3;

			m_nNumTrisDraw += nNumTris;

			pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,		// Type
					                        nFirstV,				// BaseVertexIndex
											0,						// MinIndex
											nNumV,					// NumVertices
											nFirstF,				// StartIndex
											nNumTris );				// PrimitiveCount
		}

	}

//-------------------------------------------------------------------------------------
void CBSP::RenderLeaf( int nLeafIndex )
{
		if( nLeafIndex > m_nNumLeafs ) assert(0 && "Passed Index");

		stLeaf * pLeaf = &m_pLeafs[nLeafIndex];

		int nFirst = pLeaf->nFirstFace;
		int nNum   = pLeaf->NumFaces;

		for(int i=0; i<nNum; i++)
		{
			int nFirstFace = m_pLeafFaces[nFirst + i];

			RenderFaces(
						nFirstFace,
						1);
		}
}

//-------------------------------------------------------------------------------------
void CBSP::DrawPlane(
				   stNode * pNode,
		           stPlane * pPlane)
{
		
		D3DXVECTOR3 vN;
		vN.x = pPlane->Normal[0];
		vN.y = pPlane->Normal[1];
		vN.z = pPlane->Normal[2];

		float d = pPlane->fDist;

		
		D3DXVECTOR3 pMin = D3DXVECTOR3( (float)pNode->BBoxIMin[0],
										(float)pNode->BBoxIMin[1],
										(float)pNode->BBoxIMin[2]);

		D3DXVECTOR3 pMax = D3DXVECTOR3( (float)pNode->BBoxIMax[0],
										(float)pNode->BBoxIMax[1],
										(float)pNode->BBoxIMax[2]);

		
		D3DXVECTOR3 pA = d * vN;
	
		D3DXVECTOR3 vNN = vN;

		D3DXMATRIX matRot;
		D3DXMatrixRotationYawPitchRoll(&matRot, 0, 0, D3DX_PI/2.0f);
		D3DXVec3TransformCoord(&vNN, &vN, &matRot);

	
		pA.z = 100;
//		DrawLine3D(pDevice, pA, pA+(vN*50.0f), 0xff0f0fff);

		pA = pA + (vN*5.0f);

		D3DXVECTOR3 vLen = pMax - pMin;
		float len = 0.7071f * D3DXVec3Length(&vLen)/2.0f;

		D3DXVECTOR3 pB = pA - (vNN*len);
		D3DXVECTOR3 pC = pA + (vNN*len);

		pC.z = 100;
		pB.z = 100;
//		DrawLine3D(pDevice, pC, pB, 0xffffffff);

}


//-------------------------------------------------------------------------------------
void CBSP::CheckNode( 
				   int nNode,
				   D3DXVECTOR3 pStart,
				   D3DXVECTOR3 pEnd)
{

		if (nNode < 0)
		{
			int nLeafIndex = -(nNode+1);

			RenderLeaf( nLeafIndex);

			return;
		}


		stNode  *pNode  = &m_pNodes[nNode];
		stPlane *pPlane = &m_pPlanes[pNode->nPlane];

		DrawPlane(	
					pNode,
					pPlane);

		//Vector3 vNormal = SwizzleYZ(pPlane->Normal);
		Vector3 vNormal = pPlane->Normal;
		float fStart	= vNormal.Dot(pStart)- pPlane->fDist;
		float fEnd		= vNormal.Dot(pEnd)  - pPlane->fDist;

		if (fStart >= 0 && fEnd >= 0)
		{
			CheckNode(  
						pNode->nChildren[0], // front
				        pStart, pEnd);

		}
		else if (fStart < 0 && fEnd < 0)
		{
			CheckNode(  
						pNode->nChildren[1], // back
				        pStart, pEnd);
		}
		else
		{
			CheckNode(  
						pNode->nChildren[0], 
				        pStart, pEnd);
			CheckNode(  
			    		pNode->nChildren[1], 
				        pStart, pEnd);
		}

}


//-------------------------------------------------------------------------------------
int CBSP::FindLeaf( const D3DXVECTOR3& Pos)
{
		int index = 0;

		while (index >= 0) 
		{
			stNode*  pNode  = &m_pNodes[index];
			stPlane* pPlane = &m_pPlanes[pNode->nPlane];

			DrawPlane(	
						pNode,
						pPlane);

			
			Vector3 vNormal			= pPlane->Normal;
			const float distance	= vNormal.Dot(Pos)- pPlane->fDist;

			

			if (distance >= 0) 
			{
				index = pNode->nChildren[0];// front
			} else 
			{
				index = pNode->nChildren[1]; //back;
			}

		
		}

		return -index - 1;
}


//-------------------------------------------------------------------------------------
void CBSP::CheckNodeForLine( 
							D3DXVECTOR3 pStart,
							D3DXVECTOR3 pEnd)
{
	CheckNode(0, pStart,pEnd);
}

//-------------------------------------------------------------------------------------
void CBSP::CheckMultipleLines(
							 D3DXVECTOR3 pOrigin,
							 D3DXVECTOR3 vDir,
							 float fLen)
{
		D3DXMATRIX mRotR, mRotL;
		D3DXMatrixRotationYawPitchRoll(&mRotR, 0, 0, D3DX_PI/8);
		D3DXMatrixRotationYawPitchRoll(&mRotL, 0, 0, -D3DX_PI/8);

		D3DXVECTOR3 vDirMiddle, vDirRight, vDirLeft;
		D3DXVec3Normalize(&vDirMiddle,	&vDir);
		D3DXVec3Normalize(&vDirRight,	&vDir);
		D3DXVec3Normalize(&vDirLeft,	&vDir);

		D3DXVec3TransformCoord(&vDirRight, &vDirRight, &mRotR);
		D3DXVec3TransformCoord(&vDirLeft, &vDirLeft, &mRotL);

		vDirMiddle	= pOrigin	+ (vDirMiddle*fLen);
		vDirRight	= pOrigin	+ (vDirRight*fLen);
		vDirLeft	= pOrigin	+ (vDirLeft*fLen);

		
		CheckNode(	
					0, 
					pOrigin,
					vDirMiddle);

		CheckNode(	
					0, 
					pOrigin,
					vDirLeft);

		CheckNode(	
					0, 
					pOrigin,
					vDirRight);


}




//-------------------------------------------------------------------------------------
//해당 클러스터가 보이는지 안보이는지 여부 리턴
bool CBSP::isClusterVisible(int visCluster, int testCluster)
	{
		if( visCluster		< 0) return 0;
		if( testCluster		< 0) return 0;
		if( m_VisData.pBits==NULL) return 0;

		int bytesPerCluster = m_VisData.nSizeVectorInBytes;

		byte visSet = m_VisData.pBits[(visCluster * bytesPerCluster) + (testCluster >> 3)];

		return (visSet & (1 << (testCluster & 7))) != 0;

		// MACRO VERSION
		// #define M_ISVISIBLE(VD,x,y) ( ( (1<<((y)&7)) & (VD).vecs[(x)*(VD).sz_vecs+((y)>>3)] ) != 0   )

	}

//-------------------------------------------------------------------------------------
// 면그리기
void CBSP::DrawFace( int nFace )
	{

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG2);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,	D3DTA_TEXTURE);
		
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE); 
		pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	    pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);

		pDevice->SetIndices(m_pIB);

		pDevice->SetStreamSource(0, m_pVB, 0, sizeof(stBSPVertex));

		pDevice->SetFVF( m_FVF_Format );

		// If its not a tri mesh
		if( m_pFaces[nFace].nFaceType != 1)return;
		
		int nFirstF = m_pFaces[nFace].nFirstMeshVerts;
		int nNumF   = m_pFaces[nFace].NumMeshVerts;

		int nFirstV = m_pFaces[nFace].FirstVert;
		int nNumV   = m_pFaces[nFace].NumVerts;

		int nTexIndex = m_pFaces[nFace].nTexture;

		pDevice->SetTexture( 0, m_pT[nTexIndex] );

		int nNumTris = nNumF / 3;

		/*
		pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST,		// Type
					                    nFirstV,				// BaseVertexIndex
										0,						// MinIndex
										nNumV,					// NumVertices
										nFirstF,				// StartIndex
										nNumTris );				// PrimitiveCount
		*/

		m_nNumTrisDraw += (m_pFaces[nFace].NumMeshVerts/3);

		pDevice->DrawIndexedPrimitive(	D3DPT_TRIANGLELIST , 
										m_pFaces[nFace].FirstVert, 
										0, 
										m_pFaces[nFace].NumVerts,
										m_pFaces[nFace].nFirstMeshVerts, 
										(m_pFaces[nFace].NumMeshVerts/3) );
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);

	}

//-------------------------------------------------------------------------------------
//해당 face의 Visibility On
void CBSP::TurnFaceOn(int nFace)
{
	m_bFaceVisibility[nFace] = true;
}

//-------------------------------------------------------------------------------------
//Visibility 초기화
void CBSP::TurnAllFacesOff()
{
	for(int i=0; i<m_nNumFaces; i++)
		m_bFaceVisibility[i]=false;
}

//-------------------------------------------------------------------------------------
// Visibility On인 것 그리기
void CBSP::RenderAllFaces()
{
	for(int i=0; i<m_nNumFaces; i++)
	{
		if( !m_bFaceVisibility[i] ) continue;
			DrawFace(  i );
	}
}


//-------------------------------------------------------------------------------------
//해당 pPoint에서 모든 노드들의 보이는 여부 체크 및 draw
void CBSP::CheckClusterPoint( D3DXVECTOR3 pPoint )
	{
		int indx = FindLeaf(
						 pPoint);

		//RenderLeaf(pDevice, indx);

		int nCluster = m_pLeafs[indx].nCluster;

		//Check if where outside our world!
		if( nCluster == -1 )return; 

		//그리는 여부를 off로 만들고
		TurnAllFacesOff();

		//리프리스트 돌면서 그릴지 여부 정하기
		for(int i=0; i<m_nNumLeafs; i++)
		{
			int nOtherCluster = m_pLeafs[i].nCluster;

			//위치와 해당 클러스트가 보이는 위치라면
			if( isClusterVisible( nCluster, nOtherCluster ) )
			{
				//char buf[200];
				//sprintf(buf, "Index:%d  (Cluster:%d, OtherClus:%d)  Leaf: %d  [Leaffaces(%d %d)]", 
				//		        i, nCluster, nOtherCluster,
				//				i, m_pLeafs[i].nFirstFace, m_pLeafs[i].NumFaces);
				//abc(buf);

				//면의 갯수대로 반복하면서 그리는여부 turn on 시키기
				for(int f = 0; f < m_pLeafs[i].NumFaces; f++)
				{
					//DrawFace( pDevice,
					//	m_pLeafFaces[m_pLeafs[i].nFirstFace+f] );
					
					//해당 리프의 면을 turn on
					TurnFaceOn( m_pLeafFaces[m_pLeafs[i].nFirstFace+f] );

					//sprintf(buf, "%.2d  LeafFace: %.2d", (i+f), m_pLeafFaces[m_pLeafs[i].nFirstFace+f]);
					//abc(buf);

				}
			}
		}

		// 최종 그리기
		RenderAllFaces();
	}