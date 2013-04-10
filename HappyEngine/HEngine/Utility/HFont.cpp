//-------------------------------------------------------------------------------------
// 폰트 출력용
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------

#include "HFont.h"

//-------------------------------------------------------------------------------------
HFont::HFont(LPDIRECT3DDEVICE9 DevicePointer){
	m_pd3dDevice=DevicePointer;
	
	D3DXCreateFont( m_pd3dDevice, 15, 0, 700, 1, FALSE, DEFAULT_CHARSET, 
                     OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                      "Tahoma", &m_pFont );
	D3DXCreateSprite( m_pd3dDevice, &m_pTextSprite );
	
}

//-------------------------------------------------------------------------------------
void HFont::TextNormal(char *desc, int x, int y)
{

	m_pTextSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE );

	RECT rtPos;
	rtPos.bottom=y;
	rtPos.left=x;
	rtPos.right=x;
	rtPos.top=y;

	m_pFont->DrawText( m_pTextSprite, desc, -1, 
								 &rtPos, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255) );
	m_pTextSprite->End();

}

//-------------------------------------------------------------------------------------
void HFont::TextBillBoard(char *desc, D3DXVECTOR3 pos,D3DXMATRIXA16* Bview){

	 D3DXMATRIXA16 matWorld;
	 D3DXMATRIXA16 matTrans;
	 D3DXMATRIXA16 matScale;
	 D3DXMatrixIdentity(&matTrans);
	 

	 D3DXMatrixTranslation(&matTrans,pos.x,pos.y,pos.z);
	 D3DXMatrixScaling(&matScale,0.2f,-0.2f,0.2f);
	
     D3DXMatrixMultiply(&matWorld,&matScale,&matTrans);
	 
	 m_pTextSprite->SetWorldViewLH(&matWorld,Bview);

	 m_pTextSprite->Begin( D3DXSPRITE_BILLBOARD | D3DXSPRITE_SORT_DEPTH_BACKTOFRONT | 
	 D3DXSPRITE_SORT_DEPTH_FRONTTOBACK | D3DXSPRITE_ALPHABLEND | D3DXSPRITE_OBJECTSPACE);

	 m_pTextSprite->SetTransform(&matScale);

	 m_pFont->DrawText( m_pTextSprite, desc, -1, 
								 NULL, DT_NOCLIP, D3DCOLOR_XRGB(255, 255, 255) );

	 m_pTextSprite->End();
}