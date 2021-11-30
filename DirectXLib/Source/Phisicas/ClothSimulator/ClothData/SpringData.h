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


struct ClothForce {//何も指定しないと、素朴な値が代入されます
	bool is_self_collision;//自己衝突を行うかどうか

	float grid_mass;//質点の重さ
	float gravity;//重力
	float damping;//空気抵抗
	float dt;//速度の乗数
	float k;//バネ定数(指定する必要はない)
	Resistance tension;
	Resistance compress;
	Resistance share;
	Resistance bend;
	DirectX::XMFLOAT3 wind;//風力
	//風力
	void windF(float x, float y, float z) {
		wind.x = x;
		wind.y = y;
		wind.z = z;
	}
	//引張パラメーター(引数1:剛性、引数2:減衰)
	void tensionParam(float stretch, float shrink) {
		tension.stretch = stretch;
		tension.shrink = shrink;
	}
	//圧縮パラメーター(引数1:剛性、引数2:減衰)
	void compressParam(float stretch, float shrink) {
		compress.stretch = stretch;
		compress.shrink = shrink;
	}
	//せん断パラメーター(引数1:剛性、引数2:減衰)
	void shareParam(float stretch, float shrink) {
		share.stretch = stretch;
		share.shrink = shrink;
	}
	//曲げパラメーター(引数1:剛性、引数2:減衰)
	void bendParam(float stretch, float shrink) {
		bend.stretch = stretch;
		bend.shrink = shrink;
	}
	ClothForce() {
		is_self_collision = false;
		tension.stretch = 15;
		compress.stretch = 15;
		share.stretch = 15;
		bend.stretch = 5;
		tension.shrink = 5;
		compress.shrink = 5;
		share.shrink = 5;
		bend.shrink = 0.5;

		grid_mass = 1.f;
		gravity = 9.5;
		damping = 0;
		dt = 1.f;
		k = 12;
		wind = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
	}
};