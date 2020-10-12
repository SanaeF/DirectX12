#pragma once
#include<DirectXMath.h>
#include<unordered_map>
#include<map>

#include"DXPMDBone.h"

class DXVMDMotion : public DXPMDBone {
private:
	DWORD _startTime;

	struct VMDKeyFrame {
		char boneName[15]; // �{�[����
		unsigned int frameNo; // �t���[���ԍ�(�Ǎ����͌��݂̃t���[���ʒu��0�Ƃ������Έʒu)
		DirectX::XMFLOAT3 location; // �ʒu
		DirectX::XMFLOAT4 quaternion; // Quaternion // ��]
		unsigned char bezier[64]; // [4][4][4]  �x�W�F�⊮�p�����[�^
	};

	struct KeyFrame {
		unsigned int frameNo;//�t���[����(�A�j���[�V�����J�n����̌o�ߎ���)
		DirectX::XMVECTOR quaternion;//�N�H�[�^�j�I��
		DirectX::XMFLOAT2 p1, p2;//�x�W�F�̒��ԃR���g���[���|�C���g
		KeyFrame(unsigned int fno, const DirectX::XMVECTOR& q, const DirectX::XMFLOAT2& ip1, const DirectX::XMFLOAT2& ip2) :
			frameNo(fno),
			quaternion(q),
			p1(ip1),
			p2(ip2) {}
	};
	std::unordered_map<std::string, std::vector<KeyFrame>> mKeyFrameData;

public:
	void LoadVMDFile(const char* filepath, const char* name);

	bool Update(DWORD startTime);

	void PlayAnimation(DWORD _startTime);

	DXVMDMotion() {
	}
private:

	void keyMotion();
	void boneMotion();

};