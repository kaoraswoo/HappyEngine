//-------------------------------------------------------------------------------------
// 퀘이크용 bsp로드 - 퀘이크 소스 발췌
//! BSP 부분에서 수정(BSP 로드부분 발췌함)
//! 발췌 한 곳 - http://blog.naver.com/ryanii?Redirect=Log&logNo=20017051226
//! \author Hwang Je Hyun
//! \copy Hwang Je Hyun. All rights reserved.
//-------------------------------------------------------------------------------------
#include "Q3_BSP.h"

//-------------------------------------------------------------------------------------
void FindTextureExtension(char *strFileName)
{
	char strJPGPath[MAX_PATH] = {0};
	char strTGAPath[MAX_PATH] = {0}; 
	FILE *fp = NULL;

	GetCurrentDirectory(MAX_PATH, strJPGPath);

	// Add on a '\' and the file name to the end of the current path.
	// We create 2 seperate strings to test each image extension.
	strcat(strJPGPath, "\\..\\..\\Data\\");
	strcat(strJPGPath, strFileName);
	strcpy(strTGAPath, strJPGPath);
	
	// Add the extensions on to the file name and path
	strcat(strJPGPath, ".jpg");
	strcat(strTGAPath, ".tga");

	// Check if there is a jpeg file with the texture name
	if((fp = fopen(strJPGPath, "rb")) != NULL)
	{
		// If so, then let's add ".jpg" onto the file name and return
		strcat(strJPGPath, ".jpg");
		return;
	}

	// Check if there is a targa file with the texture name
	if((fp = fopen(strTGAPath, "rb")) != NULL)
	{
		// If so, then let's add a ".tga" onto the file name and return
		strcat(strTGAPath, ".tga");
		return;
	}
}

//******************************************************************//
//	Not much to see here. Just yer good ol' Constructor
//******************************************************************//
//-------------------------------------------------------------------------------------
Q3_BSP::Q3_BSP(RenderMgr* SceneRen):SceneNode(SceneRen)
{
	m_name = "BSP Map";
	m_pDevice=SceneRen->g_pd3dDevice;
	//	Lot's of Nullifying Pointers and such.
	//	It's a good habit to get into
	//	(yet one I still don't always follow) 
	m_pVB = NULL;
	m_pIB = NULL;
	m_pTextureMaps = NULL;

	numVerts = 0;
	numFaces = 0;
	numIndex = 0;
	numTextures = 0;

	m_pTextures = NULL;
	m_pVerts = NULL;
	m_pFaces = NULL;
	m_pIndices = NULL;

	m_pRenderState[rShowTextureMap] = true;
}

//-------------------------------------------------------------------------------------
void Q3_BSP::draw(){
	
	D3DXMATRIXA16 matWorld;
	D3DXMATRIXA16 rotateWorld;
	D3DXMATRIXA16 tranWorld;
	// Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
    // every 1000 ms. To avoid the loss of precision inherent in very high 
    // floating point numbers, the system time is modulated by the rotation 
    // period before conversion to a radian angle.
    D3DXMatrixIdentity(&matWorld);
	D3DXMatrixScaling(&matWorld,0.04f,0.04f,0.04f);
	D3DXMatrixRotationX( &rotateWorld, -D3DX_PI / 2 );
	D3DXMatrixTranslation(&tranWorld,100.0f,0.0f,0.0f);
    
    m_pDevice->SetTransform( D3DTS_WORLD, &(matWorld*rotateWorld*tranWorld) );
    m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	
	Render();

    m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	
}
//-------------------------------------------------------------------------------------
void Q3_BSP::draw(ZFrustum* pFrustum){
	draw();
}
//-------------------------------------------------------------------------------------
//ToDo : 임시 하드코딩
void Q3_BSP::SetMesh(){
   Load("../../Data/maps/Tutorial.bsp");
}

//******************************************************************//
//	And now we reach the big one!
//
//	This function loads in all the BSP file data.
//	Its actually quite nice, because everything is
//	Grouped together in nice big chunks of data
//	that look really confusing in a text editor
//	but read really nicely in C(++)
//******************************************************************//
//-------------------------------------------------------------------------------------
bool Q3_BSP::Load(const char *strFileName)
{	
	FILE *fp = NULL;
	
	// Check if the .bsp file could be opened
	if((fp = fopen(strFileName, "rb")) == NULL)
	{ 

		return false;
	}

	// Initialize the header and lump structures
	BSP_HEADER header;

	// Read in the header
	// This is a really cool chunk of info that
	// Tells us where to find all the good stuff
	// in the file. Kind of like a "Table of Contents"
	fread(&header, 1, sizeof(BSP_HEADER), fp);

	//allocate the space needed for our data structures 
	numVerts = header.entries[lVertcies].length / sizeof(BSP_VERTEX);
	m_pVerts = new BSP_VERTEX[numVerts];

	numFaces = header.entries[lFaces].length / sizeof(BSP_FACE);
	m_pFaces = new BSP_FACE[numFaces];

	numIndex = header.entries[lIndices].length / sizeof(BSP_INDEX);
	m_pIndices = new BSP_INDEX[numIndex];

	numTextures = header.entries[lTextures].length / sizeof(BSP_TEXTURE);
	m_pTextures = new BSP_TEXTURE [numTextures];
	m_pTextureMaps = new LPDIRECT3DTEXTURE9 [numTextures];

	// Now this part is kinda messy. In reality, I should be setting this
	// up so that we never have to seek backwards in the file, BUUUUUTTT...
	// since this isn't a commercial project, and we're not too concerned
	// about load times we're just gonna let it slide for now.
	
	// Seek to the position in the file that stores the vertex information
	fseek(fp, header.entries[lVertcies].offset, SEEK_SET);
	
	// Read in the vertex information 
	fread(m_pVerts, numVerts, sizeof(BSP_VERTEX), fp);

	// Seek to the position in the file that stores the index information
	fseek(fp, header.entries[lIndices].offset, SEEK_SET);
	
	// Read in the index information 
	fread(m_pIndices, numIndex, sizeof(BSP_INDEX), fp);

	// Seek to the position in the file that stores the face information
	fseek(fp, header.entries[lFaces].offset, SEEK_SET);

	// Read in all the face information
	fread(m_pFaces, numFaces, sizeof(BSP_FACE), fp);

	// Seek to the position in the file that stores the texture information
	fseek(fp, header.entries[lTextures].offset, SEEK_SET);

	// Read in all the texture information
	fread(m_pTextures, numTextures, sizeof(BSP_TEXTURE), fp);

	// Close the file
	fclose(fp);
	
	//Take all the data we just loaded and format it for DirectX use!
	Build();

	// Return a success
	return true;
}

//******************************************************************//
//	Loading: Part 2!
//
//	This function takes all the data we just loaded
//	and pushes it into DirectX buffers and textures 
//******************************************************************//

void Q3_BSP::Build()
{	
	// First, we're going to go through all of the textures
	// All we're given is file path (not even an extension!)
	// So we're going to first locate the file, and then load
	// it into a DirectX texture.

	for(int i = 0; i < numTextures; i++)
	{
		// Find the texture path/extension
		FindTextureExtension(m_pTextures[i].name);
		
		char strFinalPath[MAX_PATH] = {0};
		strcpy(strFinalPath, "../../Data/");
		strcat(strFinalPath, m_pTextures[i].name);
		strcat(strFinalPath, ".jpg");
		
		// Load the texture based off of what we found.
		if(FAILED(D3DXCreateTextureFromFile( m_pDevice, strFinalPath,
                                   &m_pTextureMaps[i] )))
		{

			// If the texture can't be loaded, simply make it NULL.
			m_pTextureMaps[i] = NULL;
		}
	}
	
	// Create Vertex Buffer. I shouldn't need to explain this.
	// The only thing to notice is that we're creating exactly
	// enough space for all of our Vertices, yet we're using a
	// different struct than what we read the verts into.
	// (We read them into a BSP_VERTEX, and are creating space
	// for a BSP_CUSTOM_VERTEX)
	m_pDevice->CreateVertexBuffer( numVerts * sizeof(BSP_CUSTOM_VERTEX), 0,
								   BSP_CUSTOM_VERTEX_FVF, D3DPOOL_DEFAULT, &m_pVB, NULL );
	
	BSP_CUSTOM_VERTEX *pVertices = NULL;
	
	// Again, no explanation needed, right? Typical DirectX stuff.
	m_pVB->Lock(0, numVerts * sizeof(BSP_CUSTOM_VERTEX), (VOID**) &pVertices, 0);
	
	// Now, at this point it would be simply LOVELY if we could 
	// simply do this:
	//
	// memcpy(pVertices, m_pVerts, numVerts * sizeof(BSP_VERTEX));
	//
	// However, no such luck here, since there's a slight 
	// Quirk in how DirectX processes it's vertices. You see,
	// The information inside the vertex had to be in a 
	// particular order for DirectX to draw it correctly. In
	// this case that order is: 
	//
	// Position->Normal->Color->Texture1->Texture2
	//
	// DirectX is happy with this, and all is right in the world.
	// However, the BSP file says differently! It's data comes to
	// you in THIS order:
	//
	// Position->Texture1->Texture2->Normal->Color
	//
	// Feed this to DirectX and it will puke random red triangles 
	// all over your screen. (Trust me, I've tried!)
	// SO, what this means is that instead of a nice and easy memcopy
	// we get to loop through each vert like so:

	for(int i = 0; i < numVerts; i++)
	{
		pVertices[i].x = m_pVerts[i].position[0];
		pVertices[i].y = m_pVerts[i].position[1];
		pVertices[i].z = m_pVerts[i].position[2];

		pVertices[i].nx = m_pVerts[i].normal[0];
		pVertices[i].ny = m_pVerts[i].normal[1];
		pVertices[i].nz = m_pVerts[i].normal[2];
				
		pVertices[i].color = D3DCOLOR_RGBA(m_pVerts[i].color[0],m_pVerts[i].color[1],
										   m_pVerts[i].color[2],m_pVerts[i].color[3]);

		pVertices[i].tu = m_pVerts[i].texcoord[0];
		pVertices[i].tv = m_pVerts[i].texcoord[1];

		pVertices[i].lu = m_pVerts[i].lightmap[0];
		pVertices[i].lv = m_pVerts[i].lightmap[1];
	}

	m_pVB->Unlock();

	
	// Create Index Buffer
	// NOTICE: This has been altered from past tutorials since we had several
	// complaints that it wasn't working on older cards. Now it will check
	// the caps for the Index Buffer Type and create the proper buffer accordingly.

	
		m_pDevice->CreateIndexBuffer( numIndex * sizeof(BSP_INDEX), 0,
									D3DFMT_INDEX32, D3DPOOL_DEFAULT, &m_pIB, NULL );
		
		BSP_INDEX *pIndices = NULL;
			
		m_pIB->Lock(0, numIndex * sizeof(BSP_INDEX), (VOID**) &pIndices, 0);
		
		// Ah, now HERE we have the luxury of a memcpy, because, well
		// a number is a number. And there was much rejoicing!
		memcpy(pIndices, m_pIndices, numIndex * sizeof(BSP_INDEX));

		m_pIB->Unlock();
	
	

	//And we're done building the level! Was that too hard?
}

//******************************************************************//
//	Finally, we draw!
//
//	At this point, the hard parts over! Once our data is in
//	place in our Vertex and Index buffers, it's remarkably
//	simple to draw it.
//******************************************************************//
//-------------------------------------------------------------------------------------
void Q3_BSP::Render()
{
	// All of this info is really rather redundant now, but
	// once we get into lightmapping it will help, so let's
	// just leave it be, mmkay?
	m_pDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	
	m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	// Tell DirectX what Vertex Format we're using.
	m_pDevice->SetFVF(BSP_CUSTOM_VERTEX_FVF);
	
	// Set the Vertex Buffer we're streaming from.
	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(BSP_CUSTOM_VERTEX));
	
	// And the Indices we're using.
	m_pDevice->SetIndices(m_pIB);
	
	// I like to Null out both of these before rendering, just 
	// incase we still have a texture set from a previous model  
	m_pDevice->SetTexture(0, NULL);
	m_pDevice->SetTexture(1, NULL);
	
	// Finally, we Draw! I'm checking here to see if the texture for 
	// each face has changed so that we only call SetTexture() when needed.

	int lastTexture = -1;
	
	for(int i = 0; i < numFaces; i++)
	{
		if((m_pFaces[i].type == 1 || m_pFaces[i].type == 3))
		{
				if(m_pRenderState[rShowTextureMap])
				{
					//If the texture is different from the one previously use, swap.
					if(m_pFaces[i].texture != lastTexture)
					{
						lastTexture = m_pFaces[i].texture;
						
						// A negative texture index indicates no texture.
						if(m_pFaces[i].texture > -1)
						{
							m_pDevice->SetTexture(0,  m_pTextureMaps[m_pFaces[i].texture]);
						}
						else
						{
							m_pDevice->SetTexture(0,  NULL);
						}
						
					}
				}

				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , m_pFaces[i].vertex, 0, m_pFaces[i].n_verts,
												m_pFaces[i].meshvert, (m_pFaces[i].n_meshverts/3));
		}
	}
}

//******************************************************************//
// None of the rest of this is too complicated, so I'll
// leave you to figure it out.
//-------------------------------------------------------------------------------------
void Q3_BSP::SetRenderState(BSP_RENDER_STATES state, bool value)
{
	m_pRenderState[state] = value;
}
//-------------------------------------------------------------------------------------
bool Q3_BSP::GetRenderState(BSP_RENDER_STATES state)
{
	return m_pRenderState[state];
}
//-------------------------------------------------------------------------------------
void Q3_BSP::Destroy()
{
	m_pIB->Release();
	m_pVB->Release();

	delete [] m_pVerts;
	delete [] m_pIndices;
	delete [] m_pFaces;
	delete [] m_pTextures;
}
//-------------------------------------------------------------------------------------
Q3_BSP::~Q3_BSP()
{
	// Call our destroy function
	Destroy();
}