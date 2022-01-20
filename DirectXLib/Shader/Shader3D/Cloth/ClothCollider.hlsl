#include "RootSignature/RSClothCollider.hlsli"
#include "ClothProp/ModelData.hlsli"
#include "ClothProp/ClothData.hlsli"
#include "ClothProp/SpringData.hlsli"
#include "ClothProp/CollisionData.hlsli"
#include "SpringCalculator/SpringCalculator.hlsli"
#include "SpringCalculator/CollisionCalculator.hlsli"

RWStructuredBuffer<SpringData>out_spring : register(u0);
RWStructuredBuffer<CollisionParam>collision_param : register(u1);
RWStructuredBuffer<ModelVertex>pre_vert : register(u2);
RWStructuredBuffer<ModelVertex>in_vert : register(u3);
RWStructuredBuffer<SpaceData>space : register(u4);
RWStructuredBuffer<int>split_area : register(u5);
RWStructuredBuffer<ModelVertex>last_vert : register(u6);
RWStructuredBuffer<SpringData>spring : register(u7);
RWStructuredBuffer<PolygonModel> polygon_model : register(u8);

//�Ώۂ̒��_����A�ߋ��̒��_�Ɍ������Ė߂�
float3 extrudeSphereForce(float3 vec, float dist, float power) {
	vec = normalize(vec);
	float size = dist + power;
	//if (size > 1)size = 1;
	return scale(vec, size);
}

//��_�̈ʒu�Ɍ������Č�_�܂ł̋����ƃT�C�Y���������o��
float3 extrudeForce(float3 now_p1, float3 last_p, float3 cross_p, float power) {
	float dist = distance(now_p1, cross_p);
	float3 vec = subtract(last_p, now_p1);
	vec = normalize(vec);
	float size = dist + power;
	//if (size > 1)size = 1;
	return scale(vec, size);
}
//�Ώۂ̒��_�Ɠ���̒��_���������|���S�����Ȃ������m�F
bool isEffectivPolygon(int id, int3 polygon_id) {
	bool result = true;
	if (polygon_id.x == id &&
		polygon_id.y == id &&
		polygon_id.z == id)result = false;
	return result;
}
//��̒��_�ɑ΂��ĉ�������̓����蔻������
bool isHit(int id1, int id2) {
	bool result = false;
	int size = polygon_model[id2].polygon_num;
	//�O�p�`�̐��������[�v
	for (int ite = 0; ite < size; ite++) {
		//�L���ȎO�p�`���m�F
		int3 polygon = polygon_model[id2].id[ite];
		if (isEffectivPolygon(id1, polygon)) {
			//�O�p�`�Ɠ_�̓����蔻������
			if (isHitPolygonAndPoint(
				in_vert[polygon.x].pos,
				in_vert[polygon.y].pos,
				in_vert[polygon.z].pos,
				in_vert[id1].pos)
				)result = true;
		}
	}
	return result;
}

[RootSignature(RS)]
[numthreads(1, 1, 1)] 
void ClothCollider(uint3 th_id : SV_GroupID) {
	CollisionParam param = collision_param[0];
	int dim = sqrt(param.vertex_size);
	int id1 = (th_id.x * dim) + th_id.y;
	out_spring[id1] = spring[id1];
	if (isFixed(in_vert[id1].color))return;
	int space_id = split_area[id1];

	//��ԓ����m�̓����蔻��
	int hit_type = 1;
	if (in_vert[id1].pos.x == last_vert[id1].pos.x &&
		in_vert[id1].pos.y == last_vert[id1].pos.y && 
		in_vert[id1].pos.z == last_vert[id1].pos.z) return;
	int target_size = space[space_id].count;
	float hit_size = param.hit_size;

	float3 normal_vec = float3(in_vert[id1].normal.x, in_vert[id1].normal.y, in_vert[id1].normal.z);
	out_spring[id1].force = add(out_spring[id1].force, scale(normal_vec, hit_size/2));

	for (int ite1 = 0; ite1 < target_size; ite1++) {
		int id2 = space[ite1].id[space_id];
		if (id1 == id2)continue;
		//��̒��_�ɑ΂��ĉ�������̓����蔻������
		bool is_hit_sphere = false;
		//�ђʂ��Ă��Ȃ��ꍇ�̓����蔻��
		float hit_dist = distance(in_vert[id1].pos, in_vert[id2].pos);
		if (hit_dist < hit_size * 10) {
			float3 vec = subtract(in_vert[id2].pos, in_vert[id1].pos);
			float3 f = extrudeSphereForce(vec, hit_dist, hit_size * 10);
			out_spring[id2].force = add(out_spring[id2].force, f);

			vec = subtract(last_vert[id1].pos, in_vert[id1].pos);
			f = extrudeSphereForce(vec, hit_dist, hit_size * 1);
			out_spring[id1].force = add(out_spring[id1].force, f);
			//is_hit_sphere = true;
		}
		//���g�����O�p�`�ƑΏۂ̒��_(P0,P1)�̔���
		int pol_size = polygon_model[id1].polygon_num;
		for (int ite2 = 0; ite2 < pol_size; ite2++) {
			//�L���ȎO�p�`���m�F
			int3 polygon = polygon_model[id1].id[ite2];
			if (isEffectivPolygon(id2, polygon)) {
				//�O�p�`�ƒ����̌�_�����߂�
				float3 cross_p =
					CrossPolygonAndLine(
						in_vert[id2].pos, last_vert[id2].pos, in_vert[polygon.x].pos, in_vert[polygon.y].pos, in_vert[polygon.z].pos
					);
				//�O�p�`�ƒ����̌�_�œ����蔻������
				if (isHitPolygonAndPoint(in_vert[polygon.x].pos, in_vert[polygon.y].pos, in_vert[polygon.z].pos, cross_p)) {
					float3 f = extrudeForce(in_vert[id2].pos, last_vert[id2].pos, cross_p, hit_size * 10);
					if (isnan(f.x) && isnan(f.y) && isnan(f.z))f = float3(0, 0, 0);
					out_spring[id2].force = add(out_spring[id2].force, f);
				}
			}
		}
		//�O�p�`�̐��������[�v
		pol_size = polygon_model[id2].polygon_num;
		for (int ite2 = 0; ite2 < pol_size; ite2++) {
			//�L���ȎO�p�`���m�F
			int3 polygon = polygon_model[id2].id[ite2];
			if (isEffectivPolygon(id1, polygon)) {
				//�O�p�`�ƒ����̌�_�����߂�
				float3 cross_p =
					CrossPolygonAndLine(
						in_vert[id1].pos, last_vert[id1].pos, in_vert[polygon.x].pos, in_vert[polygon.y].pos, in_vert[polygon.z].pos
					);
				//�O�p�`�ƒ����̌�_�œ����蔻������
				if (isHitPolygonAndPoint(in_vert[polygon.x].pos, in_vert[polygon.y].pos, in_vert[polygon.z].pos, cross_p)) {
					float3 f = extrudeForce(in_vert[id1].pos, last_vert[id1].pos, cross_p, hit_size * 10);
					if (isnan(f.x) && isnan(f.y) && isnan(f.z))f = float3(0, 0, 0);
					out_spring[id1].force = add(out_spring[id1].force, f);
				}
			}
		}
	}
}