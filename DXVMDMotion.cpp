#include "DXVMDMotion.h"
#include <algorithm>
#include<d3dx12.h>

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

void DXVMDMotion::LoadVMDFile(const char* filepath, const char* name) {
	auto fp = fopen(filepath, "rb");
	fseek(fp, 50, SEEK_SET);//�ŏ���50�o�C�g�͔�΂���OK
	unsigned int keyframeNum = 0;
	fread(&keyframeNum, sizeof(keyframeNum), 1, fp);

	vector<VMDKeyFrame> keyframes(keyframeNum);
	for (auto& keyframe : keyframes) {
		fread(keyframe.boneName, sizeof(keyframe.boneName), 1, fp);//�{�[����
		fread(&keyframe.frameNo, sizeof(keyframe.frameNo) +//�t���[���ԍ�
			sizeof(keyframe.location) +//�ʒu(IK�̂Ƃ��Ɏg�p�\��)
			sizeof(keyframe.quaternion) +//�N�I�[�^�j�I��
			sizeof(keyframe.bezier), 1, fp);//��ԃx�W�F�f�[�^
	}

	//VMD�̃L�[�t���[���f�[�^����A���ۂɎg�p����L�[�t���[���e�[�u���֕ϊ�
	for (auto& f : keyframes) {
		mKeyFrameData[f.boneName].emplace_back(KeyFrame(f.frameNo, XMLoadFloat4(&f.quaternion),
			XMFLOAT2((float)f.bezier[3] / 127.0f, (float)f.bezier[7] / 127.0f),
			XMFLOAT2((float)f.bezier[11] / 127.0f, (float)f.bezier[15] / 127.0f)));
	}

	keyMotion();
	
	boneMotion();

	treeBoneCopy();
}

float GetYFromXOnBezier(float x, const XMFLOAT2& a, const XMFLOAT2& b, uint8_t n) {
	if (a.x == a.y && b.x == b.y)return x;//�v�Z�s�v
	float t = x;
	const float k0 = 1 + 3 * a.x - 3 * b.x;//t^3�̌W��
	const float k1 = 3 * b.x - 6 * a.x;//t^2�̌W��
	const float k2 = 3 * a.x;//t�̌W��

	//�덷�͈͓̔����ǂ����Ɏg�p����萔
	constexpr float epsilon = 0.0005f;

	for (int i = 0; i < n; ++i) {
		//f(t)���߂܁[��
		auto ft = k0 * t * t * t + k1 * t * t + k2 * t - x;
		//�������ʂ�0�ɋ߂�(�덷�͈͓̔�)�Ȃ�ł��؂�
		if (ft <= epsilon && ft >= -epsilon)break;

		t -= ft / 2;
	}
	//���ɋ��߂���t�͋��߂Ă���̂�y���v�Z����
	auto r = 1 - t;
	return t * t * t + 3 * t * t * r * b.y + 3 * t * r * r * a.y;
}

void DXVMDMotion::AnimationInit(DWORD startTime) {
	_startTime = startTime;
}

bool DXVMDMotion::Update(DWORD startTime) {
	auto elapsedTime = startTime - _startTime;
	unsigned int frameNo = 30 * (elapsedTime / 1000.0f);
	for (auto& bonemotion : mKeyFrameData) {
		auto node = getBoneNodeTable()[bonemotion.first];
		//���v������̂�T��
		auto keyframes = bonemotion.second;
		auto rit = find_if(keyframes.rbegin(), keyframes.rend(), [frameNo](const KeyFrame& keyframe) {
			return keyframe.frameNo <= frameNo;
		});
		if (rit == keyframes.rend())continue;//���v������̂��Ȃ���Δ�΂�
		XMMATRIX rotation;
		auto it = rit.base();
		if (it != keyframes.end()) {
			auto t = static_cast<float>(frameNo - rit->frameNo) /
				static_cast<float>(it->frameNo - rit->frameNo);
			t = GetYFromXOnBezier(t, it->p1, it->p2, 12);
			rotation = XMMatrixRotationQuaternion(
				XMQuaternionSlerp(rit->quaternion, it->quaternion, t)
			);
		}
		else {
			rotation = XMMatrixRotationQuaternion(rit->quaternion);
		}
		auto& pos = node.startPos;
		auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z) *
			rotation *
			XMMatrixTranslation(pos.x, pos.y, pos.z);
		setBoneMatrices(&node, mat);
	}
	treeBoneCopy();
	return true;
}

void DXVMDMotion::keyMotion() {
	for (auto& motion : mKeyFrameData) {
		sort(motion.second.begin(), motion.second.end(),
			[](const KeyFrame& lval, const KeyFrame& rval) {
			return lval.frameNo <= rval.frameNo;
		});
	}
}

void DXVMDMotion::boneMotion() {
	for (auto& bonemotion : mKeyFrameData) {
		auto node = getBoneNodeTable()[bonemotion.first];
		auto& pos = node.startPos;
		auto mat =
			XMMatrixTranslation(-pos.x, -pos.y, -pos.z) *
			XMMatrixRotationQuaternion(bonemotion.second[0].quaternion) *
			XMMatrixTranslation(pos.x, pos.y, pos.z);
		setBoneMatrices(&node, mat);

	}
}
