#ifndef _SceneNode_h_
 #define _SceneNode_h_
 #include "Mgr\SceneMgr.h"
#endif

class TestDummy{
public:
	//������Ʈ ����, ����� �ӽ� �Ҵ�
	HObject objectX[30];

	//ȭ�� �������, ����� �ӽ� ����
	HFont *JClog;
	char Jbuff[300];

	//����� ��� ��������
	int JCMODE;
		// FrustumŬ������ JCMODE1 ����
	//2. ����Ʈ�� - ���÷��� 1��
	//3. ����Ʈ�� - ���÷��� 2��
		 // FrustumŬ������ JCMODE2 ����
	int JCMODE2;
	//0. ����Ʈ�� - �������� ī�޶�
	//1. ����Ʈ�� - �������� ������Ʈ1
	//2. ����Ʈ�� - �������� ������Ʈ2
		// HObjectŬ������ JCMODE3 ����
	//3. BSP ��ü���
	//4. BSP -������Ʈ3 ����
	//5. BSP -������Ʈ4 ����
	//6. BSP -������Ʈ5 ����



	void GenerateObject();

};