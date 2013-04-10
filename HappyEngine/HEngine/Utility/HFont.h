//-------------------------------------------------------------------------------------
// 폰트 출력용
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include <d3d9.h>
#include <d3dx9.h>

class HFont{
public:

	LPDIRECT3DDEVICE9		m_pd3dDevice;
	char DESCRIPTION[1000];

	ID3DXFont*   m_pFont;
	ID3DXSprite*  m_pTextSprite;

	HFont(LPDIRECT3DDEVICE9 DevicePointer);

	void TextNormal(char* desc,int x,int y);
	void TextBillBoard(char* desc,D3DXVECTOR3 pos,D3DXMATRIXA16* Bview);





};