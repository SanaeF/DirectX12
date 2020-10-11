#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<unordered_map>
#include<wrl.h>

class DXPMDBone {
public:
	struct BoneNode {
		int boneIdx;//�{�[���C���f�b�N�X
		DirectX::XMFLOAT3 startPos;//�{�[����_(��]���S)
		DirectX::XMFLOAT3 endPos;
		std::vector<BoneNode*> children;//�q�m�[�h
	};
private:

	struct Transform {
		//�����Ɏ����Ă�XMMATRIX�����o��16�o�C�g�A���C�����g�ł��邽��
		//Transform��new����ۂɂ�16�o�C�g���E�Ɋm�ۂ���
		void* operator new(size_t size);
		DirectX::XMMATRIX world;
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
	void RecursiveMatrixMultiply(BoneNode* node, const DirectX::XMMATRIX& mat);

	void BoneLoad(FILE* fp);

	std::vector<DirectX::XMMATRIX> getBoneMatrices() {
		return _boneMatrices;
	}
	std::map<std::string, BoneNode> getBoneNodeTable() {
		return _boneNodeTable;
	}

	void setBoneMatrices(BoneNode* node, DirectX::XMMATRIX mat);

	HRESULT CreateTransformView(ComPtr<ID3D12Device> _dev);

	ComPtr<ID3D12DescriptorHeap> getTransHeap();

	void setTransWorld();

	DXPMDBone() {

	}

};