#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <d3d9.h>
#include <d3dx9.h>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp> // std::vector를 serialize 하고 싶은 경우 필요

#include <boost/filesystem.hpp> //파일경로, 파일명 추출등


#include <fstream>
#include <sstream>



#include <boost/shared_ptr.hpp>





#ifndef MAINBODY
#define DECLARE extern
#else
#define DECLARE
#endif

#define SafeRelease(pObject) if(pObject != NULL) {pObject->Release(); pObject=NULL;}

#ifndef DEL
	#define DEL(p)		{ if(p) { delete (p); (p) = NULL; } }
#endif

#ifndef DELS
	#define DELS(p)		{ if(p) { delete[] (p); (p) = NULL; } }
#endif

#ifndef REL
	#define REL(p)		{ if(p) { (p)->Release(); (p) = NULL; } }
#endif

struct TERRAINVERTEX
{
	enum _FVF { FVF=(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) };
	D3DXVECTOR3	p;
	D3DXVECTOR3	n;
	D3DXVECTOR2	t;
};

#define _USE_INDEX16

struct TRIINDEX
{
#ifdef _USE_INDEX16
	WORD _0, _1, _2;
#else
	DWORD	_0, _1, _2;
#endif
};

enum	LODCRACK	{ LOD_BASIC=0, LOD_LEFT, LOD_TOP, LOD_RIGHT, LOD_BOTTOM };

struct TILEVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	D3DXVECTOR2 t;			// 알파
	D3DXVECTOR2 t1;			// 멀티 텍스쳐
	/*
	friend class boost::serialization::access;
    template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
    {
        ar & p;
        ar & n;
        ar & t;
		ar & t1;
    }
	*/
};
#define D3DFVF_TILEVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2)


struct BRUSHVERTEX
{
	D3DXVECTOR3 p;
	DWORD color;
};
#define D3DFVF_BRUSHVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

#endif // _DEFINE_H_ 


//Splatting AlphaMap
#define MAXTEXNUM		4
#define MINIALPHASIZE	128
#define WIDTHMAP 800


#define ALPHA_TEXTURE_SIZE	256
#define WIDTH	129.0f
//#define HEIGHT	129.0f
#define SMOOTH		10
#define BRUSH_SIZE	2
