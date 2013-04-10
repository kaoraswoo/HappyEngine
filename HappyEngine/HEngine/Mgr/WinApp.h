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
HINSTANCE m_hInstance;
LPSTR m_lpszCmdLine;
int m_nCmdShow;
WNDCLASS m_wndclass;
char m_szMessage[80];
char m_szAppName[80];
HWND m_hWnd;
HDC m_hdc;


     WinApp(){};
	//virtual ~WinApp();

	void WinInit(HINSTANCE, LPSTR,int);
	void WinCreateClass(WNDPROC,LPSTR,LPSTR);
	void WinCreateWin(LPSTR,DWORD,int,int,int,int,HWND);


     
};