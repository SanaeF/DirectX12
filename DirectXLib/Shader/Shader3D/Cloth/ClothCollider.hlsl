#include "ClothData.hlsli"

//StructuredBuffer<Vertex> vert : register(u0);
//StructuredBuffer<Vertex> pre_vert : register(u1);
//
//RWStructuredBuffer<SpringData> spring_data : register(u2);
//
//int vertex_max : register(u3)
;
[numthreads(1, 1, 1)]
void ClothCollider( uint3 DTid : SV_DispatchThreadID ){
	//for (int ite = 0; ite < vertex_max; ite++) {
	//	//�S���_�𑍓�����
	//	for (int ite2 = 0; ite2 < vertex_max; ite2++) {
	//		//���݂̒��_���m�̋���
	//		float length = distance(vert[ite].position, vert[ite2].position);
	//		//�f�t�H���g�̒��_���m�̋���
	//		float pre_length = distance(pre_vert[ite].position, pre_vert[ite2].position);
	//		pre_length = pre_length / 2;
	//		//�����蔻��͈͓̔������o
	//		if (length < pre_length) {
	//			float3 n = sub(vert[ite].position, vert[ite2].position);
	//			n = normalize(n);
	//			float3 f = dot(n, pre_length / length);
	//			//�͂̑��
	//			spring_data[ite].force = add(spring_data[ite].force, f);
	//		}
	//	}
	//}
}