#pragma once
#include <DirectXMath.h>

enum HIT_SPLIT {
	SINGLE=3,
	XYZ_ALL= (SINGLE * SINGLE * SINGLE) * 8//480
};
struct Resistance {
	float stretch;
	float shrink;
};
struct SimulateParam {
	int vert_max;
	Resistance tension;
	Resistance compress;
	Resistance share;
	Resistance bend;
	float dt;
	float k;
	bool is_no_add_vel;
};

struct SpringData {
	DirectX::XMFLOAT3 force;
	DirectX::XMFLOAT3 velocity;
	float mass;
};

struct ClothData {
	SpringData spring;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 pre_pos;
	DirectX::XMFLOAT3 color;
	int id0;
	int id1;
	int id2;
	int id3;
	int id4;
	int id5;
	int id6;
	int id7;
	int id8;
	int id9;
	int id10;
	int id11;
	bool simulate;
	int vertex_size;
	int step;
};

struct MassModel {
	int id0;
	int id1;
	int id2;
	int id3;
	int id4;
	int id5;
	int id6;
	int id7;
	int id8;
	int id9;
	int id10;
	int id11;
};

struct PolygonModel {
	DirectX::XMINT3 id[6];
	int polygon_num;
};

struct ClothForce {//�����w�肵�Ȃ��ƁA�f�p�Ȓl���������܂�
	enum COLLISION_TYPE {
		OUT_STEP,//�N���X�̃X�e�b�v���Ɋ܂߂Ȃ�(�ᕉ��)
		IN_STEP //�N���X�̃X�e�b�v���Ɋ܂߂�(���i��)
	};
	bool is_self_collision;//���ȏՓ˂��s�����ǂ���
	int collision_type;//�����蔻��̎�ނ��w��
	float collision_power;//�����蔻��̍U���͂��w��
	float collision_size;
	float grid_mass;//���_�̏d��
	float gravity;//�d��
	float damping;//��C��R
	float dt;//���x�̏搔
	float k;//�o�l�萔(�w�肷��K�v�͂Ȃ�)
	Resistance tension;
	Resistance compress;
	Resistance share;
	Resistance bend;
	DirectX::XMFLOAT3 wind;//����
	//����
	void windF(float x, float y, float z) {
		wind.x = x;
		wind.y = y;
		wind.z = z;
	}
	//�����p�����[�^�[(����1:�����A����2:����)
	void tensionParam(float stretch, float shrink) {
		tension.stretch = stretch;
		tension.shrink = shrink;
	}
	//���k�p�����[�^�[(����1:�����A����2:����)
	void compressParam(float stretch, float shrink) {
		compress.stretch = stretch;
		compress.shrink = shrink;
	}
	//����f�p�����[�^�[(����1:�����A����2:����)
	void shareParam(float stretch, float shrink) {
		share.stretch = stretch;
		share.shrink = shrink;
	}
	//�Ȃ��p�����[�^�[(����1:�����A����2:����)
	void bendParam(float stretch, float shrink) {
		bend.stretch = stretch;
		bend.shrink = shrink;
	}
	ClothForce() {
		is_self_collision = false;
		collision_type = COLLISION_TYPE::OUT_STEP;
		collision_power = 1.38f;
		collision_size=0.15;
		tension.stretch = 15;
		compress.stretch = 15;
		share.stretch = 15;
		bend.stretch = 5;
		tension.shrink = 5;
		compress.shrink = 5;
		share.shrink = 5;
		bend.shrink = 0.5;

		grid_mass = 1.f;
		gravity = 9.8;
		damping = 0;
		dt = 1.f;
		k = 1.2f;
		wind = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	}
};