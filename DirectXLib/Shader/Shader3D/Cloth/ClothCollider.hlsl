#include "ClothProp/CCRootSig.hlsli"
#include "ClothProp/ClothData.hlsli"
#include "SpringCalculator/SpringCalculator.hlsli"
struct ColliderData {
	SpringData cloth;
	int area_id;
};
struct SpaceData {
	int space_ount;
	int spaceID[216];
};
RWStructuredBuffer<ColliderData> output : register(u0);//RWStructuredBuffer<outputSpringData> output : register(u0);
RWStructuredBuffer<ColliderData> input_model : register(u1);
RWStructuredBuffer<SpaceData> input_space : register(u2);

float3 solver(float3 p1, float3 p2, float hit_dist) {
	float3 n = subtract(p1, p2);
	n = normalize(n);//���K��
	return scale(n, hit_dist);
}

[RootSignature(RS)]
[numthreads(1, 1, 1)]
void ClothCollider(uint3 th_id : SV_GroupID) {
	int dim = sqrt(input_model[0].cloth.vertex_size);
	int id1 = (th_id.x * dim) + th_id.y;
	if (isFixed(input_model[id1].cloth.col))return;
	output[0].cloth.simulate = true;
	int space_id = input_model[id1].area_id;
	for (int ite = 0; ite < input_space[space_id].space_ount; ite++) {
		int id2 = input_space[ite].spaceID[space_id];
		float now_dist = distance(input_model[id1].cloth.pos, input_model[id2].cloth.pos);
		float hit_dist = distance(input_model[id1].cloth.pre_pos, input_model[id2].cloth.pre_pos) / 2;
		if (now_dist < hit_dist) {
			float f = solver(input_model[id1].cloth.pos, input_model[id2].cloth.pos, hit_dist);
			output[id1].cloth.spring.force = f;
		}
	}
}