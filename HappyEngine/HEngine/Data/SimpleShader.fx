//-----------------------------------------------------------------------------
// 이펙트 변수 정의
//-----------------------------------------------------------------------------

float4x4 worldViewProj : WorldViewProjection;	//월드 * 카메라 뷰행렬 * 프로젝션 행렬

texture BaseTexture;	//기본 텍스쳐

texture AlphaTex0;		//알파 텍스쳐
texture AlphaTex1;		//알파 텍스쳐
texture AlphaTex2;		//알파 텍스쳐

texture SplatTex0;		//스플래팅 텍스쳐
texture SplatTex1;		//스플래팅 텍스쳐
texture SplatTex2;		//스플래팅 텍스쳐

float	TileWidth;
float	TileHeight;

//-----------------------------------------------------------------------------
//알파 텍스쳐 샘플링 설정.
//-----------------------------------------------------------------------------

sampler alpha0 = sampler_state
{
    Texture   = <AlphaTex0>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler alpha1 = sampler_state
{
    Texture   = <AlphaTex1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler alpha2 = sampler_state
{
    Texture   = <AlphaTex2>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//-----------------------------------------------------------------------------
//기본 텍스쳐 샘플링 설정.
//-----------------------------------------------------------------------------

sampler Sampler = sampler_state
{
    Texture   = <BaseTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

//-----------------------------------------------------------------------------
//스플래팅 텍스쳐 샘플링 설정.
//-----------------------------------------------------------------------------

sampler splat0 = sampler_state
{
    Texture   = <SplatTex0>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler splat1 = sampler_state
{
    Texture   = <SplatTex1>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

sampler splat2 = sampler_state
{
    Texture   = <SplatTex2>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU  = WRAP;
    AddressV  = WRAP;
};

//-----------------------------------------------------------------------------
// 버텍스 INPUT, OUTPUT 선언
//-----------------------------------------------------------------------------

struct VS_INPUT
{
    float3 position	: POSITION;
    float3 normal   : NORMAL;
	float2 texture0 : TEXCOORD0;	//alpha
	float2 texture1 : TEXCOORD1;	//diffuse
};

//-----------------------------------------------------------------------------
// 픽셀쉐이더 쪽으로 넘길때 쓰임.
// NORMAL 대신 칼라 값으로 넘어간다.
// OUTPUT 포지션 값은 RHW같이 float4(x,y,z,w)로 해준다.
//-----------------------------------------------------------------------------

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 color	: COLOR0;
    float2 texture0 : TEXCOORD0;	//alpha
	float2 texture1 : TEXCOORD1;	//diffuse
};

//-----------------------------------------------------------------------------
// 픽셀셰이더 구조체.
//-----------------------------------------------------------------------------

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// 버텍스 셰이더
//-----------------------------------------------------------------------------
VS_OUTPUT mainVS( VS_INPUT IN )
{
    VS_OUTPUT OUT;
    
	//position은 월드*뷰*프로젝션 행렬을 곱해줘야 제 위치에 그려진다.
	OUT.position = mul(float4(IN.position,1.0f),worldViewProj);
	
	//일단 색을 기본색으로..
	OUT.color =float4(1.0f,1.0f,1.0f,1.0f);
	
	//텍스쳐 UV값이 넘어간다.
	OUT.texture0 = IN.texture0;						//alpha
	//타일 만들기.
	OUT.texture1.x = IN.texture1.x * TileWidth;		//diffuse
	OUT.texture1.y = IN.texture1.y * TileHeight;		//diffuse
	return OUT; 
}

//-----------------------------------------------------------------------------
// 픽셀 셰이더
//-----------------------------------------------------------------------------
PS_OUTPUT mainPS( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;
    
    //버텍스 셰이더에서 normal값으로 color값을 구했다면 음영이 들어간다.
	float4 a0 = tex2D( alpha0,  IN.texture0 );
	float4 a1 = tex2D( alpha1,  IN.texture0 );
	float4 a2 = tex2D( alpha2,  IN.texture0 );
	float4 t0 = tex2D( splat0,  IN.texture1 );
	float4 t1 = tex2D( splat1,  IN.texture1 );
	float4 t2 = tex2D( splat2,  IN.texture1 );
	float4 bs = tex2D( Sampler, IN.texture1 );
	
	OUT.color = lerp(bs,t0,a0);
	OUT.color = lerp(OUT.color,t1,a1);
	OUT.color = lerp(OUT.color,t2,a2) * IN.color;
	
    return OUT;
}

//-----------------------------------------------------------------------------
// Simple Effect (1 technique with 1 pass)
//-----------------------------------------------------------------------------
technique Tech
{
    pass Pass0
    {
		VertexShader = compile vs_2_0 mainVS();
		PixelShader  = compile ps_2_0 mainPS();
    }
}

