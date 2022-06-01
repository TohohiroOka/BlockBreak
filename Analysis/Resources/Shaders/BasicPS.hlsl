#include "BasicShaderHeader.hlsli"

Texture2D<float4> tex:register(t0);//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp:register(s0);//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
	float diffuse = saturate(dot(-0.1, input.normal));//diffuse��[0,1]�͈̔͂�Clamp����
	float brightness = diffuse + 0.6f;//�A���r�G���g����0.3�Ƃ��Čv�Z
	float4 texcolor = float4(tex.Sample(smp, input.uv));
	return float4(texcolor.rgb * brightness, texcolor.a) * color;//�P�x��RGB�ɑ�����ďo��
}