// ManagedHEngine.h

#pragma once

#include <msclr/marshal_cppstd.h>
#include <HappyEngine.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections;

using namespace System;
using namespace msclr::interop;

namespace ManagedHEngine {

	public ref class ManagedHappyScene
	{
	public:

		HWND m_hHwnd; 
		SceneMgr *pHappy;

		GameProgrammingClass *practices; //정육면체
		Q3_BSP* BspNode;  // BSP 통짜맵
		TileTerrainMgr* TileTerrainNode;  //타일식 쿼드트리,스플래팅
		D2TileMgr* AxisNode; //좌표축 출력
		ZTerrain* QuadNode; 
		CSkyBox* SkyNode; //스카이 박스
		BillBoard* BillBoardNode; //빌보드
		XManager* XfileNode;  //오브젝트
		CBSP* PotalNode;
		
		bool m_bWireframe;
		

		void Create(IntPtr hWnd)
		{
			//와이어프레임모드 false
			m_bWireframe=false;

			// 메인 씬메니져 생성
			pHappy = new SceneMgr();
			// 받은 Handle로 Renderer 설정
			m_hHwnd = (HWND)hWnd.ToInt32(); 
			pHappy->RenInstance->InitDXApp(m_hHwnd);

			

			// 기본이 되는 Cube(정육면체)
			practices=new GameProgrammingClass(pHappy->RenInstance);
			practices->SetMesh();
			practices->NodeSetPos(0.0f,50.0f,0.0f);
			pHappy->AddSceneNode( practices );
			
			// 좌표축 출력
			AxisNode = new D2TileMgr(pHappy->RenInstance);
			AxisNode->SetMesh();
			pHappy->AddSceneNode( AxisNode );
			
			/*
			// 해골책을 참조해 만들었던 Legacy버전의 쿼드트리(스플래팅 포함)
			QuadNode =new ZTerrain(pHappy->RenInstance);
			QuadNode->SetMesh();
			QuadNode->NodeSetPos(100.0f,100.0f,0.0f);
			pHappy->AddSceneNode( QuadNode );
			*/

			//메쉬 로드(이 2개로 오브젝트에는 메쉬Number만 넣으면 자동으로 됨
			XfileNode=new XManager(pHappy->RenInstance);
			pHappy->AddSceneNode(XfileNode);
			XfileNode->NodeSetPos(0.0f,50.0f,0.0f);
			XfileNode->XLoad("..\\..\\Data\\xmesh\\tiger.x");
			XfileNode->XLoad("..\\..\\Data\\xmesh\\tiny_4anim.x");
			
			
			//빌보드
			BillBoardNode = new BillBoard(pHappy->RenInstance);
			BillBoardNode->SetMesh();
			pHappy->AddSceneNode( BillBoardNode );


			
			
			//BSP 맵 출력
			BspNode=new Q3_BSP(pHappy->RenInstance);
			BspNode->SetMesh();
			pHappy->AddSceneNode( BspNode );
			

			//BSP 포탈 및 PVS 노드 생성
			PotalNode=new CBSP(pHappy->RenInstance);
			PotalNode->SetMesh();
			pHappy->AddSceneNode(PotalNode);

			//SkyBox
			SkyNode = new CSkyBox(pHappy->RenInstance);
			SkyNode->SetMesh();
			pHappy->AddSceneNode( SkyNode );

			
			// 타일맵 쿼드트리 지형(스플래팅 포함)
			TileTerrainNode = new TileTerrainMgr(pHappy->RenInstance);
			TileTerrainNode->SetMesh();
			//pHappy->AddSceneNode( TileTerrainNode );

			

			// 카메라 설정 init
			pHappy->SetCamera();

		
		}

		ArrayList^ GetSceneList()
		{
			ArrayList^ TempList2 = gcnew ArrayList();

			array<System::String ^>^ TempList = {"ok"};
			
			int nsize = pHappy->SnodeList.size();
			for(int index = 0; index< nsize; index++)
			{
				TempList2->Add( marshal_as<String^>( pHappy->SnodeList[index]->m_name ));
			}
			
			return TempList2;
		}

		void Destroy()
		{
			delete practices;
			delete TileTerrainNode;
			delete pHappy;

		}

		void TerrainRender()
		{
			pHappy->RenInstance->g_pd3dDevice->BeginScene();
			pHappy->RenInstance->g_pd3dDevice->SetRenderState(  D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID );
			pHappy->RenInstance->g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(97,97,97), 1.0f, 0 );
				// Update 카메라
				pHappy->UpdateCamera();

				// 타일 쿼드트리/스플래팅 지형
				TileTerrainNode->draw( pHappy->m_pFrustum );
				

				

			pHappy->RenInstance->g_pd3dDevice->EndScene();
			
			if( pHappy->RenInstance->g_pd3dDevice->Present( NULL, NULL, NULL, NULL ) == D3DERR_DEVICELOST )
			{
				//pHappy->RenInstance->Reset();
			}
		}

		void Render( System::String^ nodeName )
		{
			std::string _nodeName = marshal_as< std::string>(nodeName);

			pHappy->RenInstance->g_pd3dDevice->BeginScene();
			
			pHappy->RenInstance->g_pd3dDevice->SetRenderState(  D3DRS_FILLMODE, m_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID );

			pHappy->RenInstance->g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(97,97,97), 1.0f, 0 );
				// Update 카메라
				pHappy->UpdateCamera();
				/*
				// 정육면체 그리기
				practices->draw();
				// Legacy 쿼드트리지형
				//QuadNode->draw( pHappy->m_pFrustum );
				// BSP맵
				BspNode->draw();
				// 타일 쿼드트리/스플래팅 지형
				TileTerrainNode->draw( pHappy->m_pFrustum );
				*/

				int nsize = pHappy->SnodeList.size();
				for(int index = 0; index< nsize; index++)
				{
					if( _nodeName=="" || _nodeName == pHappy->SnodeList[index]->m_name )
					{
						pHappy->SnodeList[index]->draw( pHappy->m_pFrustum );
					}
				}

			pHappy->RenInstance->g_pd3dDevice->EndScene();
			
			if( pHappy->RenInstance->g_pd3dDevice->Present( NULL, NULL, NULL, NULL ) == D3DERR_DEVICELOST )
			{
				//pHappy->RenInstance->Reset();
			}
		}
		
		// 마우스 변화량 따른 카메라 설정
		void SetCamera(int dx, int dy)
		{
			const float fDelta = 0.008f; // 마우스의 민감도, 이 값이 커질수록 많이 움직인다.
			pHappy->HCamera.CnodeList[0]->RotateLocalX( dy  * fDelta ); // 마우스의 Y축 회전값은 3D world의  X축 회전값
			pHappy->HCamera.CnodeList[0]->RotateLocalY( dx * fDelta ); // 마우스의 X축 회전값은 3D world의  Y축 회전값

			D3DXMATRIXA16* pmatView = pHappy->HCamera.CnodeList[0]->GetViewMatrix();  // 카메라 행렬을 얻는다.
			pHappy->RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // 카메라 행렬 셋팅
		}

		// 키보드 입력에 따른 카메라 설정
		void MoveZAxis(float dist)
		{
			pHappy->HCamera.CnodeList[0]->MoveLocalZ( dist );
			D3DXMATRIXA16* pmatView = pHappy->HCamera.CnodeList[0]->GetViewMatrix();  // 카메라 행렬을 얻는다.
			pHappy->RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // 카메라 행렬 셋팅
		}
		void MoveXAxis(float dist)
		{
			pHappy->HCamera.CnodeList[0]->MoveLocalX( dist );
			D3DXMATRIXA16* pmatView = pHappy->HCamera.CnodeList[0]->GetViewMatrix();  // 카메라 행렬을 얻는다.
			pHappy->RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // 카메라 행렬 셋팅
		}

		// 지형 픽킹
		void PickRay(int x, int y)
		{
			POINT pos;
			pos.x = x;
			pos.y = y;
			TileTerrainNode->UpdatePickRay(pos);

		}

		// 지형 스플래팅 알파맵 편집 ( bdel: true-지우기 false-그리기 )
		void DrawAlpha(int selectedIndex, bool bdel)
		{
			TileTerrainNode->DrawAlphaMap(selectedIndex, bdel); // DrawAlphaMap( 대상 index, 지울지 그릴지 여부 )
		}

		// 지형 높낮이조정
		void UpdateHeight(float fdelta)
		{
			TileTerrainNode->UpdateHeight(fdelta);
		}

		// 새지형 생성
		void NewTerrain(float scale, float heightScale, int tile, int cell)
		{
			TileTerrainNode->NewTerrain(scale, heightScale, tile, cell);
		}

		// 지형 저장
		void SaveTerrain( System::String^ filePath )
		{
			std::string _filePath = marshal_as< std::string>(filePath);
			TileTerrainNode->SaveTerrain( _filePath );
		}

		// 지형 로드
		void LoadTerrain( System::String^ filePath )
		{
			std::string _filePath = marshal_as< std::string>(filePath);
			TileTerrainNode->LoadTerrain( _filePath );
		}

		//선택된 텍스쳐경로얻기
		System::String^ GetTexturePath(int index)
		{
			System::String^ s1 = marshal_as<System::String^>(TileTerrainNode->m_pHSplat->GetTextureName(index));
			return s1;
		}

		//선택된 텍스쳐경로바꾸기
		void SetTexturePath(int index, System::String^ strTex)
		{
			std::string texturefilepath = marshal_as<std::string>( strTex );
			TileTerrainNode->m_pHSplat->ChangeTextureFile( index, texturefilepath );
			//TileTerrainNode->m_pHSplat->GetTextureName(index);
		}

		//임시 PVS 컬링 테스트
		void SetPVSObjectNum(int index)
		{
			PotalNode->m_selectObjectNum = index;
		}

		//Height모드 얻기
		int GetHeightEditMode()
		{
			return TileTerrainNode->GetHeightEditMode();
		}
		//Height모드 Set
		void SetHeightEditMode(int mode)
		{
			TileTerrainNode->SetHeightEditMode(mode);
		}
		

	};
}
