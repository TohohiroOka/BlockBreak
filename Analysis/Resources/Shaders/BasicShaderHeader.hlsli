cbuffer cbuff0:register(b0)
{
	float4 color;//F
	matrix mat;//3D•ÏŠ·s—ñ
};

struct VSOutput
{
	float4 svpos:SV_POSITION;
	float3 normal:NORMAL;
	float2 uv : TEXCOORD;
};