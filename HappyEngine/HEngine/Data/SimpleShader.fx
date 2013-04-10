//-----------------------------------------------------------------------------
// ����Ʈ ���� ����
//-----------------------------------------------------------------------------

float4x4 worldViewProj : WorldViewProjection;	//���� * ī�޶� ����� * �������� ���

texture BaseTexture;	//�⺻ �ؽ���

texture AlphaTex0;		//���� �ؽ���
texture AlphaTex1;		//���� �ؽ���
texture AlphaTex2;		//���� �ؽ���

texture SplatTex0;		//���÷��� �ؽ���
texture SplatTex1;		//���÷��� �ؽ���
texture SplatTex2;		//���÷��� �ؽ���

float	TileWidth;
float	TileHeight;

//-----------------------------------------------------------------------------
//���� �ؽ��� ���ø� ����.
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
//�⺻ �ؽ��� ���ø� ����.
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
//���÷��� �ؽ��� ���ø� ����.
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
// ���ؽ� INPUT, OUTPUT ����
//-----------------------------------------------------------------------------

struct VS_INPUT
{
    float3 position	: POSITION;
    float3 normal   : NORMAL;
	float2 texture0 : TEXCOORD0;	//alpha
	float2 texture1 : TEXCOORD1;	//diffuse
};

//-----------------------------------------------------------------------------
// �ȼ����̴� ������ �ѱ涧 ����.
// NORMAL ��� Į�� ������ �Ѿ��.
// OUTPUT ������ ���� RHW���� float4(x,y,z,w)�� ���ش�.
//-----------------------------------------------------------------------------

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 color	: COLOR0;
    float2 texture0 : TEXCOORD0;	//alpha
	float2 texture1 : TEXCOORD1;	//diffuse
};

//-----------------------------------------------------------------------------
// �ȼ����̴� ����ü.
//-----------------------------------------------------------------------------

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// ���ؽ� ���̴�
//-----------------------------------------------------------------------------
VS_OUTPUT mainVS( VS_INPUT IN )
{
    VS_OUTPUT OUT;
    
	//position�� ����*��*�������� ����� ������� �� ��ġ�� �׷�����.
	OUT.position = mul(float4(IN.position,1.0f),worldViewProj);
	
	//�ϴ� ���� �⺻������..
	OUT.color =float4(1.0f,1.0f,1.0f,1.0f);
	
	//�ؽ��� UV���� �Ѿ��.
	OUT.texture0 = IN.texture0;						//alpha
	//Ÿ�� �����.
	OUT.texture1.x = IN.texture1.x * TileWidth;		//diffuse
	OUT.texture1.y = IN.texture1.y * TileHeight;		//diffuse
	return OUT; 
}

//-----------------------------------------------------------------------------
// �ȼ� ���̴�
//-----------------------------------------------------------------------------
PS_OUTPUT mainPS( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;
    
    //���ؽ� ���̴����� normal������ color���� ���ߴٸ� ������ ����.
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

