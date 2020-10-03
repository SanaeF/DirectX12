#pragma once
#include<DirectXMath.h>

class VMD_Motion {
private:

	struct KeyFrame {
		unsigned int frameNo;//�t���[����(�A�j���[�V�����J�n����̌o�ߎ���)
		DirectX::XMVECTOR quaternion;//�N�H�[�^�j�I��
		DirectX::XMFLOAT2 p1, p2;//�x�W�F�̒��ԃR���g���[���|�C���g
		KeyFrame(unsigned int fno, DirectX::XMVECTOR& q, const DirectX::XMFLOAT2& ip1, const DirectX::XMFLOAT2& ip2) :
			frameNo(fno),
			quaternion(q),
			p1(ip1),
			p2(ip2) {
		};

	};

	

public:
	void LoadVMDFile(const char* filepath, const char* name);
};