cbuffer cbuff0:register(b0)
{
	float4 color;//F(RGBA)
	matrix mat;//3D•ÏŠ·s—ñ
};

struct VSOutput
{
	float4 svpos:SV_POSITION;
	float2 uv : TEXCOORD;
};