#include "ClothProp/RSCollisionSpace.hlsli"
#include "ClothProp/ModelData.hlsli"
#include "ClothProp/CollisionData.hlsli"
RWStructuredBuffer<SpaceData>space : register(u0);
RWStructuredBuffer<int>split_area : register(u1);
RWStructuredBuffer<SendParam>send_param : register(u2);
RWStructuredBuffer<ModelVertex>vertex : register(u3);

bool between(float p1, float a, float b) {
	if (a <= p1 && p1 < b)return true;
	return false;
}
int searchZ(int add_box_id, int max, int split_num, float3 p) {
	SendParam param = send_param[0];
	add_box_id = add_box_id * (split_num * 2);
	if (param.max_pos.z == 0)split_num = 1;//���݂��Ȃ���΁A��Ԃ̕���1��
	for (int ite = 0; ite < split_num; ite++) {
		//��Ԃ̒��S���l�����ăT�C�Y��ite���̋�Ԕ͈�Z�����v�Z
		float size = param.max_pos.z - param.zero_pos.z;
		size = sqrt(size * size) / split_num;
		float min_z = param.zero_pos.z + (size * ite);
		float max_z = min_z + size;
		if (p.z == 0 || p.z >= param.max_pos.z || between(p.z, min_z, max_z)) {
			return add_box_id + ite;
		}
		//��Ԃ̒��S���l�����ăT�C�Y��ite���̋�Ԕ͈�Z�����v�Z
		size = param.min_pos.z - param.zero_pos.z;
		size = sqrt(size * size) / split_num;
		min_z = param.zero_pos.z - (size * (ite + 1));
		max_z = min_z + size;
		if (p.z <= param.min_pos.z || between(p.z, min_z, max_z)) {
			return add_box_id + split_num + ite;
		}
	}
	return -3;
}
int searchY(int add_box_id, int max, int split_num, float3 p) {//Y���ŕ���
	SendParam param = send_param[0];
	add_box_id = add_box_id * (split_num * 2);
	if (param.max_pos.y == 0)split_num = 1;//���݂��Ȃ���΁A��Ԃ̕���1��
	int test = -1;
	for (int ite = 0; ite < split_num; ite++) {
		//��Ԃ̒��S���l�����ăT�C�Y��ite���̋�Ԕ͈�Y�����v�Z
		float size = param.max_pos.y - param.zero_pos.y;
		size = sqrt(size * size) / split_num;
		float min_y = param.zero_pos.y + (size * ite);
		float max_y = min_y + size;
		if (p.y == 0 || p.y >= param.max_pos.y || between(p.y, min_y, max_y)) {
			add_box_id = add_box_id + ite;
			return searchZ(add_box_id, max, split_num, p);
		}
		//��Ԃ̒��S���l�����ăT�C�Y��ite���̋�Ԕ͈�Y�����v�Z
		size = param.min_pos.y - param.zero_pos.y;
		size = sqrt(size * size) / split_num;
		min_y = param.zero_pos.y - (size * (ite + 1));
		max_y = min_y + size;
		if (p.y <= param.min_pos.y || between(p.y, min_y, max_y)) {
			add_box_id = add_box_id + split_num + ite;
			return searchZ(add_box_id, max, split_num, p);
		}
		test = size * 100;
	}
	return -test;
}
int searchX(int split_num, int max, float3 p) {//X���ŕ���
	SendParam param = send_param[0];
	int add_box_id = 0;
	if (param.max_pos.x == 0)split_num = 1;//���݂��Ȃ���΁A��Ԃ̕���1��
	for (int ite = 0; ite < split_num; ite++) {
		//��Ԃ̒��S���l�����ăT�C�Y��ite���̋�Ԕ͈�X�����v�Z
		float size = param.max_pos.x - param.zero_pos.x;
		size = sqrt(size * size) / split_num;
		float min_x = param.zero_pos.x + (size * ite);
		float max_x = min_x + size;
		//���̕����ŕ���
		if (p.x == 0 || p.x >= param.max_pos.x || between(p.x, min_x, max_x)) {
			add_box_id = ite;//0�`split_num
			return searchY(add_box_id, max, split_num, p);
		}
		//��Ԃ̒��S���l�����ăT�C�Y��ite���̋�Ԕ͈�X�����v�Z
		size = param.min_pos.x - param.zero_pos.x;
		size = sqrt(size * size) / split_num;
		min_x = param.zero_pos.x - (size * (ite + 1));
		max_x = min_x + size;
		//���̕����ŕ���
		if (p.x <= param.min_pos.x || between(p.x, min_x, max_x)) {
			add_box_id = split_num + ite;//���̕����Ƃ͕ʂ�0�`split_num
			return searchY(add_box_id, max, split_num, p);
		}
	}
	return -1;//��ԃG���[
}
int inputSpace(float3 p, int max) {
	SendParam param = send_param[0];
	return searchX(param.split_num, max, p);
}

void firstSetting(int space_size) {
	for (int ite = 0; ite < space_size; ite++) {
		space[ite].count = 0;
	}
}

[RootSignature(RS)]
[numthreads(1, 1, 1)]
void CollisionSpace(uint3 th_id : SV_GroupID) {
	int XYZ_ALL = 216;
	int space_max = 80;
	SendParam param = send_param[0];
	int dim = sqrt(param.vertex_size);
	int id = (th_id.x * dim) + th_id.y;
	if (id == 0)firstSetting(XYZ_ALL);
	if (id >= param.vertex_size)return;
	split_area[id] = inputSpace(vertex[id].pos, XYZ_ALL - 1);
	int area_id = split_area[id];
	/*if (space[area_id].count >= space_max) {
		for (int ite = 0; ite < XYZ_ALL; ite++) {
			if (space[ite].count < space_max) {
				area_id = ite;
				break;
			}
		}
	}*/
	space[space[area_id].count].id[area_id] = id;
	space[area_id].count++;
}