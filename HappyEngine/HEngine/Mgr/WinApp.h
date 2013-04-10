//-------------------------------------------------------------------------------------
//! 윈도우 매니저 클래스(윈도우 생성)
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------
#include<windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <map>
#include <string>


class WinApp{

public:
	HINSTANCE m_hInstance; //윈도우 인스턴스
	LPSTR m_lpszCmdLine;   //창이름
	int m_nCmdShow;		   //show여부
	WNDCLASS m_wndclass;   //윈도우클래스
	char m_szMessage[80];
	char m_szAppName[80];
	HWND m_hWnd;		   //핸들
	HDC m_hdc;			   //dc

    WinApp(){};
	//virtual ~WinApp();

	void WinInit(HINSTANCE, LPSTR,int);
	void WinCreateClass(WNDPROC,LPSTR,LPSTR);
	void WinCreateWin(LPSTR,DWORD,int,int,int,int,HWND);
    
};