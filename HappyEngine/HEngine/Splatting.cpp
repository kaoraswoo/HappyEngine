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

	// Texture Splatting ���� �⺻ �ؽ�ó ���Ŀ� �ö���� �ؽ�ó��
    // ��Ƽ�ؽ�ó�� �̿��ؼ� 0�� �ε����� ���ĸ� 1�� �ε����� �̹����� �����Ѵ�.
    // 1�� �ε��������� Į�� OP�� ����(���ؽ�) Į��� �ڽ��� �ؽ�ó�� �̿��ϸ�
    // ���� OP�� 0�� �ε����� ���ĸ� ����ؼ� ����Ѵ�.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

	// �̹��� �ؽ�ó�� Ÿ��ȭ�� ���ؼ� �����Ѵ�.
    m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// �⺻ ���� �ɼ�
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
	//���� ���α׷� ����ǰ� �ִ� �������� ���
	char strFilePath[MAX_PATH];
	::GetModuleFileName(NULL, strFilePath, MAX_PATH);

	//�������Ͽ��� ����path��� ����
	boost::filesystem::path path(strFilePath);
	std::string strFolderPath = path.parent_path().string();  

	bUpdateBaseTex = true;
	strBaseTex.clear();
	strBaseTex.insert(0, strFolderPath + "\\..\\..\\Data\\textures\\���λ�Ǯ.jpg");
	

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
// ���ĸ� ����
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
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );	/// 0�� �ؽ�ó ���������� Ȯ�� ����
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );		/// 0�� �ؽ�ó : 1�� �ؽ�ó �ε��� ���
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );	
}

//---------------------------------------------------------------------------
void HSplatting::AlphaTextureRender(int index)
{
	// 0�� �ε������� ���� �ؽ�ó�� �����ϰ� 1�� �ε������� �̹��� �ؽ�ó�� �����Ѵ�.
    // 0�� �ؽ�ó�� Į�� OP�� ����(���ؽ�) Į�� ����ϸ�, ���� OP�� �ؽ�ó���� ������ �´�.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	// 0�� �ε����� ���� �ؽ�ó�� ��ü�� �ѹ��� �׷����� �ϱ� ������ Ÿ��ȭ ������ ������� �ʴ´�.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// 1�� �ε����� �̹��� �ؽ�ó�� Į��� ���� OP�� ARG1, ARG2�� 1:1 ȥ���Ѵ�.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	//m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	//m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );

	// Texture Splatting�� �ؽ�ó�� 0�� �ε����� ���ĸ� �̿��ؼ� 1�� �ε�����
    // �ؽ�ó(�̹���)�� �Ϻκ� ����ϴ°��̱� ������ ���ĺ����� ����Ѵ�.
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	
	m_pd3dDevice->SetTexture(0, m_Texture[index].pAlphamap);

	m_pd3dDevice->SetTexture(1, m_Texture[index].pTexture);

	//���⼭���� WRAP ����
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP  );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP  );
	D3DXMATRIXA16 matTemp;
	D3DXMatrixScaling( &matTemp, m_XtileScale, m_YtileScale, 1.0f );  //Ÿ�ϰ�����ŭ ������ ���̸� �˴ϴ�
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

	

	// Alpha ���� �� �ȼ��� ���� ���� ������ ũ�⸦ ���Ѵ�.
	float PixSize = (float)(WIDTH - 1) / ALPHA_TEXTURE_SIZE;
	// �Ų������� ������ ������ ũ�⸦ ������
    // �� �ȼ��� �����ؾ��ϴ��� ũ�⸦ ���Ѵ�.
    int EditSize = (int)(SMOOTH / PixSize);

	// ��ŷ�� ��ǥ�� �̿��ؼ� ��ŷ�� ��ġ�� u, v ��ǥ�� �˾Ƴ���.
	// �׷��� �� ���������� ��ŷ�� �������� �����Ƿ� u, v ���� ���� �־���.
	// �� ������ ��� ȭ���� ����Ѵٸ� u, v�� 1.0f�� ���� �߽����� 
	// �� ����� �ؽ�ó�� ������ �� �� �ִ�.
	int PosX = ALPHA_TEXTURE_SIZE * tx;
    int PosY = ALPHA_TEXTURE_SIZE * ty;

	// ������ Alpha ���� �̹����� ���� ��ġ �ȼ��� �� ��ġ �ȼ��� ���Ѵ�.
	int StartPosX = ((PosX - EditSize) < 0) ? 0 : PosX - EditSize;
    int StartPosY = ((PosY - EditSize) < 0) ? 0 : PosY - EditSize;
    int EndPosX = ((PosX + EditSize) >= ALPHA_TEXTURE_SIZE)? ALPHA_TEXTURE_SIZE - 1 : PosX + EditSize;
    int EndPosY = ((PosY + EditSize) >= ALPHA_TEXTURE_SIZE)? ALPHA_TEXTURE_SIZE - 1 : PosY + EditSize;

	DefBits = static_cast<BYTE*>(Alpha_Locked.pBits);

	// ������ ���� Alpha ���� ������ �ȼ��� ������ �����Ѵ�.
	for(int i=StartPosY; i<=EndPosY; i++) 
	{
		for(int j=StartPosX; j<=EndPosX; j++) 
		{
			int in = (Alpha_Locked.Pitch * i) + (j * 4);
			// �ش� �ȼ��� ���� �о�´�.
			BYTE Read = DefBits[in];

			// �����ϴ� �ȼ��� ��ġ�� 3D ��ǥ�� �°� �����ϰ�, 
			// ��ŷ�� ��ġ�� �ȼ��� ��ġ ���� 3D ��ǥ�� �°� �����Ͽ� 
			// �� �ȼ��� �Ÿ��� ����Ѵ�.
			// ���� ���������� ��ŷ�� �������� �ʴ´�.
			D3DXVECTOR3 Pix = D3DXVECTOR3(j * PixSize, 0.0f, ((ALPHA_TEXTURE_SIZE) - i) * PixSize ) -
                              D3DXVECTOR3(PosX * PixSize, 0.0f, ((ALPHA_TEXTURE_SIZE) - PosY) * PixSize);    
			
			float Len = D3DXVec3Length(&Pix);

			// ��(��)�� ũ�⿡ ���Ե� ���
            // ��� ���İ��� �ִ´�.
			// �̰� �ؽ�ó�� ���İ� ���� ǥ������� �ϱ� ����
			if(Len < BRUSH_SIZE)
			{
				Data = 0xff;
			}
			// �Ų������� ������ ���Ե� ��� 
			// �Ų������� ���������� ����������
			// ���İ��� ���Ѵ�.
			else if(Len < (int)SMOOTH-4)
			{
				Len -= SMOOTH;
				int Smooth = SMOOTH - BRUSH_SIZE;
				Data = (BYTE)((Smooth - Len) / Smooth * 0xff);   //����Ʈ�� ����ȯ�����غôµ�...
			}
			else 
			{
				// ������ ���� ���� ��� 
				// ���ĸ� �������� �ʴ´�.
				continue;
			}

		

			// �о�� �ȼ��� ���� ������ ���İ���
            // ����� �Ǵ� ���ϱ⿡ ���� ���� �����Ѵ�.
			if(bDel)
			{
                Read = ( (Read - Data ) < 0x00 )? 0x00 : (Read - Data );
			}else
			{
				Read = (Read < Data) ? Data : Read;
			}
           

			// ���� �ؽ��Ŀ� ������ ���� ���İ��� �����Ѵ�.
			// �߿��Ѱ��� A�� �ٲٸ� �Ǹ�, 
			// RGB���� ���� �ٲ��� �ʾƵ� �ȴ�.
			// ���� ���İ��� Ȯ���� ���ؼ� 
			// ��� ������ ���� �־� ���Ҵ�.
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
		// �ؽ��� ������ ����Ǿ����� Ȯ���Ѵ�.
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
			bUpdateBaseTex = false;		// �ʱ�ȭ
		}
	}
	
	// ���÷��� �ؽ���
	for(int i=0; i<4; ++i)
	{
		// �ؽ��� ������ ����Ǿ��ٸ�
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
					//_swprintf(temp, TEXT("  %s �ؽ��� ���� ����"), m_mapInfo.wstrSplattingTex[i].c_str());
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
// ���ĸ����Ͽ��� �б�
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
// ���ĸʵ� �ϰ��ε�
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
// ���ĸʵ� �ϰ�����
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


//���� Legacy ZTerrain���� ��� ���÷���
/*
void HSplatting::HSplattingLoad(LPSTR lpFilename){  //���̽��� �� ���� �̸� �ٸ� ����Ŭ�������� ��Ƴ��� �� Ŭ���������� �� ���� ���ĸ����� �ռ��ؼ� �����͸� �ε�
	if(TexNumber<=3){  //�ϴ� 3������ ����
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
	// �ؽ�ó�� ��� �ȼ��� 0���� �ʱ�ȭ�Ѵ�.
	// ���� �߿��Ѱ��� ���İ��� 0�� ����� ���̴�.
    // ������ ���� �� ����� ������� �ʱ� �����̴�.
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

		// Texture Splatting ���� �⺻ �ؽ�ó ���Ŀ� �ö���� �ؽ�ó��
    // ��Ƽ�ؽ�ó�� �̿��ؼ� 0�� �ε����� ���ĸ� 1�� �ε����� �̹����� �����Ѵ�.
    // 1�� �ε��������� Į�� OP�� ����(���ؽ�) Į��� �ڽ��� �ؽ�ó�� �̿��ϸ�
    // ���� OP�� 0�� �ε����� ���ĸ� ����ؼ� ����Ѵ�.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

		// �̹��� �ؽ�ó�� Ÿ��ȭ�� ���ؼ� �����Ѵ�.
    m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
    m_pd3dDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		// �⺻ ���� �ɼ�
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, 5);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, 6);


}



void HSplatting::HSplattingSet(){
	// 0�� �ε������� ���� �ؽ�ó�� �����ϰ� 1�� �ε������� �̹��� �ؽ�ó�� �����Ѵ�.
    // 0�� �ؽ�ó�� Į�� OP�� ����(���ؽ�) Į�� ����ϸ�, ���� OP�� �ؽ�ó���� ������ �´�.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

	// 0�� �ε����� ���� �ؽ�ó�� ��ü�� �ѹ��� �׷����� �ϱ� ������ Ÿ��ȭ ������ ������� �ʴ´�.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	// 1�� �ε����� �̹��� �ؽ�ó�� Į��� ���� OP�� ARG1, ARG2�� 1:1 ȥ���Ѵ�.
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	// Texture Splatting�� �ؽ�ó�� 0�� �ε����� ���ĸ� �̿��ؼ� 1�� �ε�����
    // �ؽ�ó(�̹���)�� �Ϻκ� ����ϴ°��̱� ������ ���ĺ����� ����Ѵ�.
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	// Texture Splatting �ؾ� �ϴ� �ؽ�ó(�̹���)�� �����ϰ� �׸���.

}

*/