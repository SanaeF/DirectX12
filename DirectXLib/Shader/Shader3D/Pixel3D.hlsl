#include "Header3D.hlsli"

float4 Pixel3D(Output input) : SV_TARGET
{
	float3 light = normalize(float3(1.f,-1.0,1.f));
	float brightness = dot(-light, input.normal);
	float4 texColor = tex.Sample(smp, input.uv); //�e�N�X�`���J���[
	return float4(mul(texColor.x, brightness), mul(texColor.y, brightness), mul(texColor.z, brightness), texColor.w); //return float4(0,0,0,1);
	//return float4(brightness, brightness, brightness, 1); //return float4(0,0,0,1);
}
//
//float4 Pixel3D(Output input) : SV_TARGET{
//	float3 light = normalize(float3(1,-1,1));//���̌������x�N�g��(���s����)
//	float3 lightColor = float3(1,1,1);//���C�g�̃J���[(1,1,1�Ő^����)
//
//	//�f�B�t���[�Y�v�Z
//	float diffuseB = saturate(dot(-light, input.normal));
//	float4 toonDif = tex.Sample(smp, float2(0, 1.0 - diffuseB));
//
//	//���̔��˃x�N�g��
//	float3 refLight = normalize(reflect(light, input.normal.xyz));
//	float specularB = pow(saturate(dot(refLight, -input.ray)), specular.a);
//
//	//�X�t�B�A�}�b�v�pUV
//	float2 sphereMapUV = input.vnormal.xy;
//	sphereMapUV = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);
//
//	float4 texColor = tex.Sample(smp, input.uv); //�e�N�X�`���J���[
//
//	return max(
//		saturate(toonDif * diffuse * texColor * tex.Sample(smp, sphereMapUV)) +
//		saturate(tex.Sample(smp, sphereMapUV) * texColor + float4(specularB * specular.rgb, 1)),
//		float4(texColor * ambient, 1)
//	);
//}