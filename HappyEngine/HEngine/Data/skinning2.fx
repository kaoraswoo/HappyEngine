//**************************************************************//
//  Effect File exported by RenderMonkey 1.6
//
//  - Although many improvements were made to RenderMonkey FX  
//    file export, there are still situations that may cause   
//    compilation problems once the file is exported, such as  
//    occasional naming conflicts for methods, since FX format 
//    does not support any notions of name spaces. You need to 
//    try to create workspaces in such a way as to minimize    
//    potential naming conflicts on export.                    
//    
//  - Note that to minimize resulting name collisions in the FX 
//    file, RenderMonkey will mangle names for passes, shaders  
//    and function names as necessary to reduce name conflicts. 
//**************************************************************//

//--------------------------------------------------------------//
// Default_DirectX_Effect
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string Default_DirectX_Effect_Pass_0_Model : ModelData = "..\\..\\..\\program files (x86)\\amd\\rendermonkey 1.82\\Examples\\Media\\Models\\Sphere.3ds";

float4x4 WorldViewProjection;
float4x4 World;
float4x3 WorldInverseTranspose;
float3 vCameraPos;
float4x3 gBoneMatrix[60];

struct VS_INPUT 
{
   float3 Position : POSITION;
   float3 Normal : NORMAL;
   float4 BlendIndices : BLENDINDICES;
   float4 BlendWeights : BLENDWEIGHT;
   
  
};

struct VS_OUTPUT 
{
   float4 Position : POSITION;
   float3 ViewDir : TEXCOORD0;
   float3 Normal : TEXCOORD1;
   
};

VS_OUTPUT Default_DirectX_Effect_Pass_0_Vertex_Shader_vs_main( VS_INPUT IN )
{
   VS_OUTPUT Out;
   
   float4 weights = IN.BlendWeights;
   weights.w = 1.0f - dot( weights.xyz, float3( 1, 1, 1));
   float4 localPos = float4( IN.Position, 1.0f);
   float3 objPos = mul( localPos, gBoneMatrix[IN.BlendIndices.x]) * weights.x;
   objPos += mul( localPos, gBoneMatrix[IN.BlendIndices.y]) * weights.y;
   objPos += mul( localPos, gBoneMatrix[IN.BlendIndices.z]) * weights.z;
   objPos += mul( localPos, gBoneMatrix[IN.BlendIndices.w]) * weights.w;

   Out.Position = mul( float4( objPos, 1.0f), WorldViewProjection);
   //float3 WorldPos = mul( float4( objPos, 1.0f), World);
   
   
   //Out.Position = mul( float4( IN.Position, 1.0f), WorldViewProjection);
   
   float3 WorldPos = mul( float4( IN.Position, 1.0f), World);
   Out.ViewDir = vCameraPos - WorldPos;
   Out.Normal = mul(IN.Normal,WorldInverseTranspose);
   
   return( Out );
   
}




// light colors
float3 afLightColor[5];
float3 afLightDir[5];
   
struct PS_INPUT
{
   float4 Position : POSITION;
   float3 ViewDir : TEXCOORD0;
   float3 Normal : TEXCOORD1;
};

float4 Default_DirectX_Effect_Pass_0_Pixel_Shader_ps_main(PS_INPUT IN) : COLOR
{   
   float4 OUT = float4(0.0f,0.0f,0.0f,1.0f);
   float3 Normal = normalize(IN.Normal);
   float3 ViewDir = normalize(IN.ViewDir);
   
   float L1 = dot(Normal,afLightDir[0]) * 0.5f + 0.5f;
   float3 Reflect = reflect (Normal,afLightDir[0]);
   float fHalfLambert = L1*L1;
   OUT.rgb += afLightColor[0] * (fHalfLambert +
      saturate(fHalfLambert * 4.0f) * pow(dot(Reflect,ViewDir),9));
   
   return( OUT);
   
}




//--------------------------------------------------------------//
// Technique Section for Default_DirectX_Effect
//--------------------------------------------------------------//
technique Default_DirectX_Effect
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 Default_DirectX_Effect_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 Default_DirectX_Effect_Pass_0_Pixel_Shader_ps_main();
   }

}

