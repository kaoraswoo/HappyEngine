
#include "SkyBox.h"

CSkyBox::CSkyBox(RenderMgr* SceneRen):SceneNode(SceneRen)
{
    SkyRender=SceneRen;
	m_pd3dDevice=SceneRen->g_pd3dDevice;
	
    m_pTexture		= NULL;
	m_pVB			= NULL;
	m_pIB			= NULL;
	D3DXMatrixIdentity( &m_localTM );

	m_name="SkyBox";
}

void CSkyBox::draw(){
	Render();
}
void CSkyBox::draw(ZFrustum* pFrustum){
	Render();
}
void CSkyBox::SetMesh(){
	Create();
}


CSkyBox::~CSkyBox(void)
{
	Destroy();
}


HRESULT	CSkyBox::Create()
{


	// Cleanup previous buffers if any
	//Destroy();

	m_pd3dDevice->AddRef();

	// 버텍스 버퍼 생성
 InitVB( );

	// 인덱스 버퍼 생성
	InitIB();
LoadTexture(LPCWSTR("..\\..\\Data\\skybox.bmp"));
	return S_OK;
}


void	CSkyBox::Render( )
{

	D3DXMATRIX mIdentity;
	D3DXMatrixIdentity(&mIdentity);
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &mIdentity );
	D3DXMATRIX w, m, t;
	//D3DXMatrixIdentity(&m);
	D3DXMatrixScaling(&m, 200, 200, 200);
	D3DXMatrixTranslation(&t, 32., 20., 32.);
	w=m*t;
	m_pd3dDevice->SetTransform( D3DTS_WORLD, &w );

	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

	m_pd3dDevice->SetTexture(0, m_pTexture);
	// 정점버퍼의 삼각형을 그린다.
	// 1. 정점정보가 담겨있는 정점버퍼를 출력 스트림으로 할당한다.
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(_sVertex) );
	// 2. D3D에게 정점쉐이더 정보를 지정한다. 대부분의 경우에는 FVF만 지정한다.
	m_pd3dDevice->SetFVF( SkyFVF );
	// 3. 인덱스버퍼를 지정한다.
	m_pd3dDevice->SetIndices( m_pIB );
	// 4. DrawIndexedPrimitive()를 호출한다.
	m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 18, 0, 12 );
}


void	CSkyBox::Destroy( )
{
	m_pIB->Release();
	m_pVB->Release();
	m_pTexture ->Release();
}


HRESULT	CSkyBox::InitVB( )
{



	_sVertex vertices[] =
	{
		 1.000000f, -1.000000f,  1.000000f, 0.997500f, 0.325700f,
		-1.000000f, -1.000000f, -1.000000f, 0.505100f, 0.004300f,
		-1.000000f, -1.000000f,  1.000000f, 0.505100f, 0.325700f,
		 1.000000f, -1.000000f, -1.000000f, 0.997500f, 0.004300f,
		 1.000000f,  1.000000f,  1.000000f, 0.001500f, 0.332500f,
		-1.000000f,  1.000000f, -1.000000f, 0.499800f, 0.001200f,
		 1.000000f,  1.000000f, -1.000000f, 0.001500f, 0.001200f,
		-1.000000f,  1.000000f,  1.000000f, 0.499800f, 0.332500f,
		 1.000000f,  1.000000f, -1.000000f, 0.499900f, 0.662300f,
		-1.000000f, -1.000000f, -1.000000f, 0.000000f, 0.999000f,
		 1.000000f, -1.000000f, -1.000000f, 0.500000f, 0.999700f,
		-1.000000f,  1.000000f, -1.000000f, 0.000000f, 0.660200f,
		 1.000000f,  1.000000f,  1.000000f, 0.999900f, 0.662300f,
		 1.000000f, -1.000000f,  1.000000f, 1.000000f, 1.000000f,
		 1.000000f, -1.000000f,  1.000000f, 0.001500f, 0.657800f,
		-1.000000f, -1.000000f,  1.000000f, 0.499800f, 0.658000f,
		-1.000000f,  1.000000f, -1.000000f, 0.998700f, 0.332200f,
		-1.000000f, -1.000000f, -1.000000f, 0.998700f, 0.658200f

	};

	if( FAILED( m_pd3dDevice->CreateVertexBuffer( 18*sizeof(_sVertex),
		0,SkyFVF ,
		D3DPOOL_DEFAULT, &m_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	// Fill the vertex buffer.
	VOID* pVertices;
	if( FAILED( m_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, vertices, sizeof(vertices) );
	m_pVB->Unlock();


	return S_OK;
}


HRESULT	CSkyBox::InitIB( )
{


	_sTriangle indices[] =
	{
		0,1,2,
		1,0,3,
		4,5,6,
		5,4,7,
		8,9,10,
		9,8,11,
		12,10,13,
		10,12,8,
		7,14,15,
		14,7,4,
		16,15,17,
		15,16,7
	};

	if( FAILED( m_pd3dDevice->CreateIndexBuffer( 12 * sizeof(sTriangle), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL ) ) )
	{
		return E_FAIL;
	}

	// 인덱스버퍼를 값으로 채운다. 
	// 인덱스버퍼의 Lock()함수를 호출하여 포인터를 얻어온다.
	VOID* pIndices;
	if( FAILED( m_pIB->Lock( 0, sizeof(indices), (void**)&pIndices, 0 ) ) )
		return E_FAIL;
	memcpy( pIndices, indices, sizeof(indices) );
	m_pIB->Unlock();


	return S_OK;
}


HRESULT	CSkyBox::LoadTexture( LPCWSTR strFileName )
{

 D3DXCreateTextureFromFile( m_pd3dDevice, LPCSTR(strFileName), &m_pTexture );
	return S_OK;
}


void	CSkyBox::SetEyePt ( CONST D3DXVECTOR3* pEyePt )
{
	
}
