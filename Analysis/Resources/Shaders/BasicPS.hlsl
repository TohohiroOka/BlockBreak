#include "BasicShaderHeader.hlsli"

Texture2D<float4> tex:register(t0);//0番スロットに設定されたテクスチャ
SamplerState smp:register(s0);//0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float diffuse = saturate(dot(-0.1, input.normal));//diffuseを[0,1]の範囲にClampする
	float brightness = diffuse + 0.6f;//アンビエント功を0.3として計算
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	return float4(texcolor.rgb * brightness, texcolor.a) * color;//輝度をRGBに代入して出力
}