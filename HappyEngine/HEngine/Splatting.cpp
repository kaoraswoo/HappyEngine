#include "Splatting.h"


//---------------------------------------------------------------------------
HSplatting::HSplatting(LPDIRECT3DDEVICE9 DevicePointer){
	m_pd3dDevice=DevicePointer;
	TexNumber=0;

	m_pBaseTexture = NULL;
	for(int i=0; i<=4; ++i)
	{
		bUpdateSplatting[i] = false;
	}
	Init();
}
//---------------------------------------------------------------------------
HSplatting::~HSplatting(){
	REL(m_pBaseTexture);
	for( int i = 0 ; i < TexNumber ; i++ ){
		m_Texture[i].pTexture->Release();
		m_Texture[i].pAlphamap->Release();
	}
}

//---------------------------------------------------------------------------
void HSplatting::Init()
{
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// Texture Splatting 에서 기본 텍스처 이후에 올라오는 텍스처는
    // 멀티텍스처를 이용해서 0번 인덱스에 알파를 1번 인덱스에 이미지를 설정한다.
    // 1번 인덱스에서는 칼라 OP는 정점(버텍스) 칼라와 자신의 텍스처를 이용하며
    // 알파 OP는 0번 인덱스의 알파를 사용해서 출력한다.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

	// 이미지 텍스처의 타일화를 위해서 설정한다.
    m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// 기본 알파 옵션
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, 5);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, 6);

}




//---------------------------------------------------------------------------
void HSplatting::MiniAlphaTextureRender(int num)
{
	static float MiniAlphaTex[4][6] = 
	{
		{ 1920-120,			0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 1920,						0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
		{ 1920-120, 120, 0.0f, 1.0f, 0.0f, 1.0f },
		{ 1920,			   120, 0.0f, 1.0f, 1.0f, 1.0f },
	};

	m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );	
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTexture( 0, m_Texture[num].pAlphamap );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, MiniAlphaTex, sizeof(MiniAlphaTex[0]) );	
}



//-----------------------------------------------------------------------------
// Load the terrain textures
//-----------------------------------------------------------------------------
bool HSplatting::LoadTextures() 
{
	//현재 프로그램 실행되고 있는 실행파일 얻기
	char strFilePath[MAX_PATH];
	::GetModuleFileName(NULL, strFilePath, MAX_PATH);

	//실행파일에서 폴더path경로 추출
	boost::filesystem::path path(strFilePath);
	std::string strFolderPath = path.parent_path().string();  

	bUpdateBaseTex = true;
	strBaseTex.clear();
	strBaseTex.insert(0, strFolderPath + "\\..\\..\\Data\\textures\\연두색풀.jpg");
	

	bUpdateSplatting[0] = true;
	strSplattingTex[0].clear();
	strSplattingTex[0].insert(0, strFolderPath + "\\..\\..\\Data\\textures\\02_basic_grass.jpg");

	bUpdateSplatting[1] = true;
	strSplattingTex[1].clear();
	strSplattingTex[1].insert(0, strFolderPath + "\\..\\..\\Data\\textures\\03_basic_send.jpg");
	

	bUpdateSplatting[2] = true;
	strSplattingTex[2].clear();
	strSplattingTex[2].insert(0, strFolderPath + "\\..\\..\\Data\\textures\\04_basic_volcano.jpg");
	

	bUpdateSplatting[3] = true;
	strSplattingTex[3].clear();
	strSplattingTex[3].insert(0, strFolderPath + "\\..\\..\\Data\\textures\\05_basic_rock.png");
	

	_initTexture();


	CreateAlphaMaps();

	return true;
}

//-----------------------------------------------------------------------------
// 알파맵 생성
//-----------------------------------------------------------------------------
bool HSplatting::CreateAlphaMaps()
{
	for( int i=0 ; i<MAXTEXNUM ; ++i)
	{
		if( FAILED(m_pd3dDevice->CreateTexture( ALPHA_TEXTURE_SIZE, ALPHA_TEXTURE_SIZE, 1, 0, D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED, &m_Texture[i].pAlphamap, NULL )) )
			return MessageBox(NULL, "alphaTexture make error", "Error", MB_OK);

		D3DLOCKED_RECT		AlphaMap_Locked;

		memset( &AlphaMap_Locked, 0, sizeof(D3DLOCKED_RECT) );

		if( FAILED(m_Texture[i].pAlphamap->LockRect(0, &AlphaMap_Locked, NULL, 0 )) )
		{
				return MessageBox(NULL, "alphaTexture Lock error", "Error", MB_OK);
		}
		
		
		LPBYTE pDataDST = (LPBYTE) AlphaMap_Locked.pBits;

		for( int j=0; j<ALPHA_TEXTURE_SIZE; ++j )
		{
			LPDWORD pDWordDST = (LPDWORD) (pDataDST + j * AlphaMap_Locked.Pitch);

			for( int i=0; i<ALPHA_TEXTURE_SIZE; ++i )
			{
				*(pDWordDST + i) = 0x00000000;
			}
		}
		
		if( FAILED(m_Texture[i].pAlphamap->UnlockRect(0)) )	
		{
			return MessageBox(NULL, "alphaTexture UnLock error", "Error", MB_OK);
		}
	}

	return TRUE;
}

//---------------------------------------------------------------------------
void HSplatting::BaseTextureRender()
{
	m_pd3dDevice->SetTexture( 0, m_pBaseTexture );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	/// 0번 텍스처 스테이지의 확대 필터
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );		/// 0번 텍스처 : 1번 텍스처 인덱스 사용
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );	
}

//---------------------------------------------------------------------------
void HSplatting::AlphaTextureRender(int index)
{
	// 0번 인덱스에는 알파 텍스처를 설정하고 1번 인덱스에는 이미지 텍스처를 설정한다.
    // 0번 텍스처는 칼라 OP를 정점(버텍스) 칼라만 사용하며, 알파 OP만 텍스처에서 가지고 온다.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	// 0번 인덱스의 알파 텍스처는 전체에 한번만 그려져야 하기 때문에 타일화 설정을 사용하지 않는다.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// 1번 인덱스의 이미지 텍스처의 칼라와 알파 OP를 ARG1, ARG2를 1:1 혼합한다.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	//m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	//m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );

	// Texture Splatting은 텍스처의 0번 인덱스의 알파를 이용해서 1번 인덱스의
    // 텍스처(이미지)를 일부분 출력하는것이기 때문에 알파블랜딩을 사용한다.
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	
	m_pd3dDevice->SetTexture(0, m_Texture[index].pAlphamap);

	m_pd3dDevice->SetTexture(1, m_Texture[index].pTexture);

	//여기서부터 WRAP 세팅
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );
	D3DXMATRIXA16 matTemp;
	D3DXMatrixScaling( &matTemp, m_XtileScale, m_YtileScale, 1.0f );  //타일갯수만큼 배율을 높이면 됩니다
	m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &matTemp );
}

//---------------------------------------------------------------------------
void HSplatting::HSplattingAlphaSet(int num,float tx, float ty, bool bDel){

	D3DLOCKED_RECT Alpha_Locked;
	ZeroMemory(&Alpha_Locked, sizeof(D3DLOCKED_RECT));

	//m_pATex[num]->LockRect(0, &Alpha_Locked, NULL, 0);
	m_Texture[num].pAlphamap->LockRect(0, &Alpha_Locked, NULL, 0);

	BYTE Data = 0;
	BYTE *DefBits = 0;
	D3DXVECTOR3 p;

	

	// Alpha 맵의 한 픽셀이 월드 상의 한점의 크기를 구한다.
	float PixSize = (float)(WIDTH - 1) / ALPHA_TEXTURE_SIZE;
	// 매끄러움의 정도에 한점의 크기를 나눠서
    // 몇 픽셀을 수정해야하는지 크기를 구한다.
    int EditSize = (int)(SMOOTH / PixSize);

	// 픽킹한 좌표를 이용해서 피킹한 위치의 u, v 좌표를 알아낸다.
	// 그러나 이 예제에서는 픽킹을 지원하지 않으므로 u, v 값을 직접 넣었다.
	// 이 예제의 결과 화면을 출력한다면 u, v가 1.0f인 점을 중심으로 
	// 원 모양의 텍스처가 보임을 알 수 있다.
	int PosX = ALPHA_TEXTURE_SIZE * tx;
    int PosY = ALPHA_TEXTURE_SIZE * ty;

	// 편집할 Alpha 맵의 이미지의 시작 위치 픽셀과 끝 위치 픽셀을 구한다.
	int StartPosX = ((PosX - EditSize) < 0) ? 0 : PosX - EditSize;
    int StartPosY = ((PosY - EditSize) < 0) ? 0 : PosY - EditSize;
    int EndPosX = ((PosX + EditSize) >= ALPHA_TEXTURE_SIZE)? ALPHA_TEXTURE_SIZE - 1 : PosX + EditSize;
    int EndPosY = ((PosY + EditSize) >= ALPHA_TEXTURE_SIZE)? ALPHA_TEXTURE_SIZE - 1 : PosY + EditSize;

	DefBits = static_cast<BYTE*>(Alpha_Locked.pBits);

	// 위에서 얻은 Alpha 맵의 수정할 픽셀의 값들을 설정한다.
	for(int i=StartPosY; i<=EndPosY; i++) 
	{
		for(int j=StartPosX; j<=EndPosX; j++) 
		{
			int in = (Alpha_Locked.Pitch * i) + (j * 4);
			// 해당 픽셀의 값을 읽어온다.
			BYTE Read = DefBits[in];

			// 편집하는 픽셀의 위치를 3D 좌표에 맞게 번경하고, 
			// 픽킹된 위치의 픽셀의 위치 역시 3D 좌표에 맞게 변경하여 
			// 두 픽셀의 거리를 계산한다.
			// 현재 예제에서는 픽킹을 지원하지 않는다.
			D3DXVECTOR3 Pix = D3DXVECTOR3(j * PixSize, 0.0f, ((ALPHA_TEXTURE_SIZE) - i) * PixSize ) -
                              D3DXVECTOR3(PosX * PixSize, 0.0f, ((ALPHA_TEXTURE_SIZE) - PosY) * PixSize);    
			
			float Len = D3DXVec3Length(&Pix);

			// 붓(원)의 크기에 포함될 경우
            // 흰색 알파값을 넣는다.
			// 이건 텍스처를 알파값 없이 표시해줘야 하기 때문
			if(Len < BRUSH_SIZE)
			{
				Data = 0xff;
			}
			// 매끄러움의 정도에 포함될 경우 
			// 매끄러움의 정도까지의 선형보간한
			// 알파값을 구한다.
			else if(Len < (int)SMOOTH-4)
			{
				Len -= SMOOTH;
				int Smooth = SMOOTH - BRUSH_SIZE;
				Data = (BYTE)((Smooth - Len) / Smooth * 0xff);   //바이트로 형변환수정해봤는데...
			}
			else 
			{
				// 포함이 되지 않을 경우 
				// 알파를 셋팅하지 않는다.
				continue;
			}

		

			// 읽어온 픽셀에 새로 생성된 알파값을
            // 지우기 또는 더하기에 따라 새로 셋팅한다.
			if(bDel)
			{
                Read = ( (Read - Data ) < 0x00 )? 0x00 : (Read - Data );
			}else
			{
				Read = (Read < Data) ? Data : Read;
			}
           

			// 알파 텍스쳐에 위에서 구한 알파값을 갱신한다.
			// 중요한것은 A만 바꾸며 되며, 
			// RGB값은 굳이 바꾸지 않아도 된다.
			// 물론 알파값의 확인을 위해서 
			// 모두 동일한 값을 넣어 놓았다.
			DefBits[in++] = Read;
			DefBits[in++] = Read;
			DefBits[in++] = Read;
			DefBits[in++] = Read;
		}
	}
	//m_pATex[num]->UnlockRect(0);
	m_Texture[num].pAlphamap->UnlockRect(0);
}

//--------------------------------------------------------------------------------------------
HRESULT HSplatting::_initTexture(void)
{

	/// Base texture
	if( strBaseTex.empty() || strBaseTex == _T("") )
			m_pBaseTexture = NULL;
	else
	{
		// 텍스쳐 파일이 변경되었는지 확인한다.
		if( bUpdateBaseTex )
		{
			if(m_pBaseTexture != NULL)
				m_pBaseTexture->Release();
			if( FAILED( 
					D3DXCreateTextureFromFile
						( 
							m_pd3dDevice, 
							strBaseTex.c_str(), 
							&m_pBaseTexture
						) 
					  ) 
			  )
			{
				TCHAR *temp = NULL;
				MessageBox(NULL, temp, TEXT("_initTexture FAILED"), 0 );
				return E_FAIL;
			}
			bUpdateBaseTex = false;		// 초기화
		}
	}
	
	// 스플래팅 텍스쳐
	for(int i=0; i<4; ++i)
	{
		// 텍스쳐 파일이 변경되었다면
		if( bUpdateSplatting[i] )
		{
			if( strSplattingTex[i].empty() || strSplattingTex[i] == _T(""))
				m_Texture[i].pTexture = NULL;
			else
			{
				if(m_Texture[i].pTexture != NULL)
					m_Texture[i].pTexture->Release();
				if( FAILED( D3DXCreateTextureFromFile( m_pd3dDevice, strSplattingTex[i].c_str(), 
															&m_Texture[i].pTexture) ) )
				{
					TCHAR *temp = NULL;
					//_swprintf(temp, TEXT("  %s 텍스쳐 파일 없음"), m_mapInfo.wstrSplattingTex[i].c_str());
					MessageBox(NULL, temp, TEXT("_initTexture FAILED"), 0 );
					return E_FAIL;
				}
			}
			bUpdateSplatting[i] = false;
		}
	}
	
	return S_OK;
}

//--------------------------------------------------------------------------------------------
void HSplatting::UpdateBaseTextureFile(LPCTSTR str)
{
	bUpdateBaseTex = true;
	strBaseTex.clear();
	strBaseTex.insert(0, str);
	
	_initTexture();
}

//--------------------------------------------------------------------------------------------
void HSplatting::UpdateTextureFile(int index, LPCTSTR str)
{
	bUpdateSplatting[index] = true;
	strSplattingTex[index].clear();
	strSplattingTex[index].insert(0, str);
	_initTexture();
}

//--------------------------------------------------------------------------------------------
void HSplatting::ChangeTextureFile(int index, std::string str)
{
	bUpdateSplatting[index] = true;
	strSplattingTex[index].clear();
	strSplattingTex[index].insert(0, str);
	_initTexture();
}

//--------------------------------------------------------------------------------------------
// 알파맵파일에서 읽기
void HSplatting::LoadAlphaMap(int index,  LPCTSTR str)
{
	if( FAILED( D3DXCreateTextureFromFile( 
						m_pd3dDevice, 
						str, 
						&m_Texture[index].pAlphamap) )
	  )
	{
				TCHAR *temp = TEXT("LoadAlphaMap FAILED");
				MessageBox(NULL, temp, temp, 0 );
				return;
	}
}

//--------------------------------------------------------------------------------------------
// 알파맵들 일괄로드
void HSplatting::LoadAlphaMaps(std::string FolderName)
{
	for(int i=0; i< MAXTEXNUM ; i++)
	{
		std::stringstream sstr;
		sstr<< i ;
		std::string FilePath = FolderName + "\\" + "AlphaMap" + sstr.str() + ".png";

		m_Texture[i].pAlphamap->Release();
		m_Texture[i].pAlphamap=NULL;

		if( FAILED( D3DXCreateTextureFromFile( 
						m_pd3dDevice, 
						FilePath.c_str(), 
						&m_Texture[i].pAlphamap) )
		  )
		{
					TCHAR *temp = TEXT("LoadAlphaMaps FAILED");
					MessageBox(NULL, temp, temp, 0 );
					return;
		}
	}
}

//--------------------------------------------------------------------------------------------
// 알파맵들 일괄저장
void HSplatting::SaveAlphaMaps(std::string FolderName)
{
	for(int i=0; i< MAXTEXNUM ; i++)
	{
		std::stringstream sstr;
		sstr<< i ;
		std::string FilePath = FolderName + "\\" + "AlphaMap" + sstr.str() + ".png";

		if( FAILED( D3DXSaveTextureToFile( FilePath.c_str() , D3DXIFF_PNG, m_Texture[i].pAlphamap, NULL ) )
		  )
		{
			TCHAR *temp = TEXT("SaveAlphaMaps FAILED");
				MessageBox(NULL, temp, temp, 0 );
				return;
		}
		
	}
}

//--------------------------------------------------------------------------------------------
const char* HSplatting::GetBaseTextureName()
{
	return strBaseTex.c_str();
}
//--------------------------------------------------------------------------------------------
const char* HSplatting::GetTextureName(int i)
{
	return strSplattingTex[i].c_str();
}

//--------------------------------------------------------------------------------------------
void HSplatting::SetScale(float tx, float ty)
{
	m_XtileScale = tx;
	m_YtileScale = ty;
}


//기존 Legacy ZTerrain에서 썼던 스플래팅
/*
void HSplatting::HSplattingLoad(LPSTR lpFilename){  //베이스가 될 것은 미리 다른 지형클래스에서 깔아놓음 이 클래스에서는 그 위에 알파맵으로 합성해서 덮을것만 로드
	if(TexNumber<=3){  //일단 3장으로 제한
	D3DXCreateTextureFromFileEx(m_pd3dDevice, lpFilename, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 
										  0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 
										  0, NULL, NULL, &m_pTex[TexNumber]);

	for(int k=0;k<2;k++){

	m_pd3dDevice->CreateTexture(ALPHA_TEXTURE_SIZE, ALPHA_TEXTURE_SIZE, 1, 0, D3DFMT_A8R8G8B8,
										  D3DPOOL_MANAGED, &m_pATex[TexNumber], NULL);
	D3DLOCKED_RECT Alpha_Locked;
	ZeroMemory(&Alpha_Locked, sizeof(D3DLOCKED_RECT));
	m_pATex[TexNumber]->LockRect(0, &Alpha_Locked, NULL, 0);

	BYTE *DefBits = NULL;
	DefBits = static_cast<BYTE*>(Alpha_Locked.pBits);
	// 텍스처의 모든 픽셀을 0으로 초기화한다.
	// 가장 중요한것은 알파값을 0을 만드는 것이다.
    // 어차피 알파 값 말고는 사용하지 않기 때문이다.
	for(int i=0, Index=0; i<ALPHA_TEXTURE_SIZE; i++)
	{
		for(int j=0; j<ALPHA_TEXTURE_SIZE; j++)
		{
			DefBits[Index++] = 0x00;	// Blue
			DefBits[Index++] = 0x00;	// Green
			DefBits[Index++] = 0x00;	// Red
			DefBits[Index++] = 0x00;	// Alpha
		}

		Index += Alpha_Locked.Pitch - (ALPHA_TEXTURE_SIZE * 4);
	}

	m_pATex[TexNumber]->UnlockRect(0);

	TexNumber++;
	}
	}

	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// Texture Splatting 에서 기본 텍스처 이후에 올라오는 텍스처는
    // 멀티텍스처를 이용해서 0번 인덱스에 알파를 1번 인덱스에 이미지를 설정한다.
    // 1번 인덱스에서는 칼라 OP는 정점(버텍스) 칼라와 자신의 텍스처를 이용하며
    // 알파 OP는 0번 인덱스의 알파를 사용해서 출력한다.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

		// 이미지 텍스처의 타일화를 위해서 설정한다.
    m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		// 기본 알파 옵션
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, 5);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, 6);


}



void HSplatting::HSplattingSet(){
	// 0번 인덱스에는 알파 텍스처를 설정하고 1번 인덱스에는 이미지 텍스처를 설정한다.
    // 0번 텍스처는 칼라 OP를 정점(버텍스) 칼라만 사용하며, 알파 OP만 텍스처에서 가지고 온다.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	// 0번 인덱스의 알파 텍스처는 전체에 한번만 그려져야 하기 때문에 타일화 설정을 사용하지 않는다.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// 1번 인덱스의 이미지 텍스처의 칼라와 알파 OP를 ARG1, ARG2를 1:1 혼합한다.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	// Texture Splatting은 텍스처의 0번 인덱스의 알파를 이용해서 1번 인덱스의
    // 텍스처(이미지)를 일부분 출력하는것이기 때문에 알파블랜딩을 사용한다.
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// Texture Splatting 해야 하는 텍스처(이미지)를 설정하고 그린다.

}

*/