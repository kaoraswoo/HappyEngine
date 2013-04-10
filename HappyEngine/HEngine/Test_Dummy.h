#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

class TestDummy{
public:
	//오브젝트 관련, 제출용 임시 할당
	HObject objectX[30];

	//화면 글자출력, 제출용 임시 선언
	HFont *JClog;
	char Jbuff[300];

	//제출용 모드 전역변수
	int JCMODE;
		// Frustum클래스에 JCMODE1 설정
	//2. 쿼드트리 - 스플래팅 1번
	//3. 쿼드트리 - 스플래팅 2번
		 // Frustum클래스에 JCMODE2 설정
	int JCMODE2;
	//0. 쿼드트리 - 프러스텀 카메라
	//1. 쿼드트리 - 프러스텀 오브젝트1
	//2. 쿼드트리 - 프러스텀 오브젝트2
		// HObject클래스에 JCMODE3 설정
	//3. BSP 전체출력
	//4. BSP -오브젝트3 관점
	//5. BSP -오브젝트4 관점
	//6. BSP -오브젝트5 관점



	void GenerateObject();

};