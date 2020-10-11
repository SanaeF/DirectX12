#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<unordered_map>
#include<wrl.h>

class DXPMDBone {
private:
	struct BoneNode {
		int boneIdx;//�{�[���C���f�b�N�X
		DirectX::XMFLOAT3 startPos;//�{�[����_(��]���S)
		DirectX::XMFLOAT3 endPos;
		std::vector<BoneNode*> children;//�q�m�[�h
	};

	struct Transform {
		//�����Ɏ����Ă�XMMATRIX�����o��16�o�C�g�A���C�����g�ł��邽��
		//Transform��new����ۂɂ�16�o�C�g���E�Ɋm�ۂ���
		void* operator new(size_t size);
		DirectX::XMMATRIX world;
	};

	struct KeyFrame {
		unsigned int frameNo;//�t���[����(�A�j���[�V�����J�n����̌o�ߎ���)
		DirectX::XMVECTOR quaternion;//�N�H�[�^�j�I��
		DirectX::XMFLOAT2 p1, p2;//�x�W�F�̒��ԃR���g���[���|�C���g
		KeyFrame(
			unsigned int fno,
			DirectX::XMVECTOR& q,
			const DirectX::XMFLOAT2& ip1,
			const DirectX::XMFLOAT2& ip2
		) :
			frameNo(fno),
			quaternion(q),
			p1(ip1),
			p2(ip2) {
		};

	};

	std::vector<DirectX::XMMATRIX> _boneMatrices;
	std::map<std::string, BoneNode> _boneNodeTable;

	DirectX::XMMATRIX* _mappedMatrices = nullptr;

	Transform _transform;

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	ComPtr<ID3D12Resource> _transformBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> _transformHeap = nullptr;//���W�ϊ��q�[�v

public:
	void BoneInitialize();

	std::vector<DirectX::XMMATRIX> getBoneMatrices() {
		return _boneMatrices;
	}
	std::map<std::string, BoneNode> getBoneNodeTable() {
		return _boneNodeTable;
	}

	HRESULT CreateTransformView(ComPtr<ID3D12Device> _dev);

	ComPtr<ID3D12DescriptorHeap> getTransHeap();

	void setTransWorld();

	DXPMDBone() {

	}
private:

	void RecursiveMatrixMultiply(BoneNode* node, const DirectX::XMMATRIX& mat);

};