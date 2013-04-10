//-------------------------------------------------------------------------------------
//! ������ �Ŵ��� Ŭ����(������ ����)
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
	HINSTANCE m_hInstance; //������ �ν��Ͻ�
	LPSTR m_lpszCmdLine;   //â�̸�
	int m_nCmdShow;		   //show����
	WNDCLASS m_wndclass;   //������Ŭ����
	char m_szMessage[80];
	char m_szAppName[80];
	HWND m_hWnd;		   //�ڵ�
	HDC m_hdc;			   //dc

    WinApp(){};
	//virtual ~WinApp();

	void WinInit(HINSTANCE, LPSTR,int);
	void WinCreateClass(WNDPROC,LPSTR,LPSTR);
	void WinCreateWin(LPSTR,DWORD,int,int,int,int,HWND);
    
};