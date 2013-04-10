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

		GameProgrammingClass *practices; //������ü
		Q3_BSP* BspNode;  // BSP ��¥��
		TileTerrainMgr* TileTerrainNode;  //Ÿ�Ͻ� ����Ʈ��,���÷���
		D2TileMgr* AxisNode; //��ǥ�� ���
		ZTerrain* QuadNode; 
		CSkyBox* SkyNode; //��ī�� �ڽ�
		BillBoard* BillBoardNode; //������
		XManager* XfileNode;  //������Ʈ
		CBSP* PotalNode;
		
		bool m_bWireframe;
		

		void Create(IntPtr hWnd)
		{
			//���̾������Ӹ�� false
			m_bWireframe=false;

			// ���� ���޴��� ����
			pHappy = new SceneMgr();
			// ���� Handle�� Renderer ����
			m_hHwnd = (HWND)hWnd.ToInt32(); 
			pHappy->RenInstance->InitDXApp(m_hHwnd);

			

			// �⺻�� �Ǵ� Cube(������ü)
			practices=new GameProgrammingClass(pHappy->RenInstance);
			practices->SetMesh();
			practices->NodeSetPos(0.0f,50.0f,0.0f);
			pHappy->AddSceneNode( practices );
			
			// ��ǥ�� ���
			AxisNode = new D2TileMgr(pHappy->RenInstance);
			AxisNode->SetMesh();
			pHappy->AddSceneNode( AxisNode );
			
			/*
			// �ذ�å�� ������ ������� Legacy������ ����Ʈ��(���÷��� ����)
			QuadNode =new ZTerrain(pHappy->RenInstance);
			QuadNode->SetMesh();
			QuadNode->NodeSetPos(100.0f,100.0f,0.0f);
			pHappy->AddSceneNode( QuadNode );
			*/

			//�޽� �ε�(�� 2���� ������Ʈ���� �޽�Number�� ������ �ڵ����� ��
			XfileNode=new XManager(pHappy->RenInstance);
			pHappy->AddSceneNode(XfileNode);
			XfileNode->NodeSetPos(0.0f,50.0f,0.0f);
			XfileNode->XLoad("..\\..\\Data\\xmesh\\tiger.x");
			XfileNode->XLoad("..\\..\\Data\\xmesh\\tiny_4anim.x");
			
			
			//������
			BillBoardNode = new BillBoard(pHappy->RenInstance);
			BillBoardNode->SetMesh();
			pHappy->AddSceneNode( BillBoardNode );


			
			
			//BSP �� ���
			BspNode=new Q3_BSP(pHappy->RenInstance);
			BspNode->SetMesh();
			pHappy->AddSceneNode( BspNode );
			

			//BSP ��Ż �� PVS ��� ����
			PotalNode=new CBSP(pHappy->RenInstance);
			PotalNode->SetMesh();
			pHappy->AddSceneNode(PotalNode);

			//SkyBox
			SkyNode = new CSkyBox(pHappy->RenInstance);
			SkyNode->SetMesh();
			pHappy->AddSceneNode( SkyNode );

			
			// Ÿ�ϸ� ����Ʈ�� ����(���÷��� ����)
			TileTerrainNode = new TileTerrainMgr(pHappy->RenInstance);
			TileTerrainNode->SetMesh();
			//pHappy->AddSceneNode( TileTerrainNode );

			

			// ī�޶� ���� init
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
				// Update ī�޶�
				pHappy->UpdateCamera();

				// Ÿ�� ����Ʈ��/���÷��� ����
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
				// Update ī�޶�
				pHappy->UpdateCamera();
				/*
				// ������ü �׸���
				practices->draw();
				// Legacy ����Ʈ������
				//QuadNode->draw( pHappy->m_pFrustum );
				// BSP��
				BspNode->draw();
				// Ÿ�� ����Ʈ��/���÷��� ����
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
		
		// ���콺 ��ȭ�� ���� ī�޶� ����
		void SetCamera(int dx, int dy)
		{
			const float fDelta = 0.008f; // ���콺�� �ΰ���, �� ���� Ŀ������ ���� �����δ�.
			pHappy->HCamera.CnodeList[0]->RotateLocalX( dy  * fDelta ); // ���콺�� Y�� ȸ������ 3D world��  X�� ȸ����
			pHappy->HCamera.CnodeList[0]->RotateLocalY( dx * fDelta ); // ���콺�� X�� ȸ������ 3D world��  Y�� ȸ����

			D3DXMATRIXA16* pmatView = pHappy->HCamera.CnodeList[0]->GetViewMatrix();  // ī�޶� ����� ��´�.
			pHappy->RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // ī�޶� ��� ����
		}

		// Ű���� �Է¿� ���� ī�޶� ����
		void MoveZAxis(float dist)
		{
			pHappy->HCamera.CnodeList[0]->MoveLocalZ( dist );
			D3DXMATRIXA16* pmatView = pHappy->HCamera.CnodeList[0]->GetViewMatrix();  // ī�޶� ����� ��´�.
			pHappy->RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // ī�޶� ��� ����
		}
		void MoveXAxis(float dist)
		{
			pHappy->HCamera.CnodeList[0]->MoveLocalX( dist );
			D3DXMATRIXA16* pmatView = pHappy->HCamera.CnodeList[0]->GetViewMatrix();  // ī�޶� ����� ��´�.
			pHappy->RenInstance->g_pd3dDevice->SetTransform( D3DTS_VIEW, pmatView );   // ī�޶� ��� ����
		}

		// ���� ��ŷ
		void PickRay(int x, int y)
		{
			POINT pos;
			pos.x = x;
			pos.y = y;
			TileTerrainNode->UpdatePickRay(pos);

		}

		// ���� ���÷��� ���ĸ� ���� ( bdel: true-����� false-�׸��� )
		void DrawAlpha(int selectedIndex, bool bdel)
		{
			TileTerrainNode->DrawAlphaMap(selectedIndex, bdel); // DrawAlphaMap( ��� index, ������ �׸��� ���� )
		}

		// ���� ����������
		void UpdateHeight(float fdelta)
		{
			TileTerrainNode->UpdateHeight(fdelta);
		}

		// ������ ����
		void NewTerrain(float scale, float heightScale, int tile, int cell)
		{
			TileTerrainNode->NewTerrain(scale, heightScale, tile, cell);
		}

		// ���� ����
		void SaveTerrain( System::String^ filePath )
		{
			std::string _filePath = marshal_as< std::string>(filePath);
			TileTerrainNode->SaveTerrain( _filePath );
		}

		// ���� �ε�
		void LoadTerrain( System::String^ filePath )
		{
			std::string _filePath = marshal_as< std::string>(filePath);
			TileTerrainNode->LoadTerrain( _filePath );
		}

		//���õ� �ؽ��İ�ξ��
		System::String^ GetTexturePath(int index)
		{
			System::String^ s1 = marshal_as<System::String^>(TileTerrainNode->m_pHSplat->GetTextureName(index));
			return s1;
		}

		//���õ� �ؽ��İ�ιٲٱ�
		void SetTexturePath(int index, System::String^ strTex)
		{
			std::string texturefilepath = marshal_as<std::string>( strTex );
			TileTerrainNode->m_pHSplat->ChangeTextureFile( index, texturefilepath );
			//TileTerrainNode->m_pHSplat->GetTextureName(index);
		}

		//�ӽ� PVS �ø� �׽�Ʈ
		void SetPVSObjectNum(int index)
		{
			PotalNode->m_selectObjectNum = index;
		}

		//Height��� ���
		int GetHeightEditMode()
		{
			return TileTerrainNode->GetHeightEditMode();
		}
		//Height��� Set
		void SetHeightEditMode(int mode)
		{
			TileTerrainNode->SetHeightEditMode(mode);
		}
		

	};
}
