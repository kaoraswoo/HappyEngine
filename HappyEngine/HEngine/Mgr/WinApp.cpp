#include "WinApp.h"


void WinApp::WinInit(HINSTANCE hinst,LPSTR cmd,int show){

m_hInstance=hinst;
m_lpszCmdLine=cmd;
m_nCmdShow=show;

}



void WinApp::WinCreateClass(WNDPROC wProc,LPSTR menu,LPSTR name){
lstrcpy(m_szAppName,name);
/*
m_wndclass.style=CS_HREDRAW | CS_VREDRAW;
m_wndclass.lpfnWndProc=wProc;
m_wndclass.cbClsExtra=0;
m_wndclass.cbWndExtra=0;
m_wndclass.hInstance=(HINSTANCE)m_hInstance;
m_wndclass.hIcon=LoadIcon((HINSTANCE)m_hInstance,IDI_APPLICATION);
m_wndclass.hCursor=LoadCursor((HINSTANCE)m_hInstance,IDC_ARROW);
m_wndclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
m_wndclass.lpszMenuName=menu;
m_wndclass.lpszClassName=m_szAppName;

*/
WNDCLASSEX m_wndclass = { sizeof(WNDCLASSEX), CS_CLASSDC, wProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      m_szAppName, NULL };

RegisterClassEx(&m_wndclass);
}


void WinApp::WinCreateWin(LPSTR winname, DWORD Style, int sx, int sy, int Width, int Height, HWND phWnd){

	m_hWnd=CreateWindowEx(NULL, m_szAppName,winname,Style,sx,sy,Width,Height,phWnd,NULL,(HINSTANCE)m_hInstance,NULL);

      //CreateWindow( m_szAppName,winname,DWORD Style, sx, sy, Width, Height,
      //                              phWnd, NULL, (HINSTANCE)m_hInstance, NULL );

	if(Style&WS_HSCROLL)SetScrollRange(m_hWnd,SB_HORZ,0,0,TRUE);
	if(Style&WS_VSCROLL)SetScrollRange(m_hWnd,SB_VERT,0,0,TRUE);
ShowWindow(m_hWnd,m_nCmdShow);
UpdateWindow(m_hWnd);
}