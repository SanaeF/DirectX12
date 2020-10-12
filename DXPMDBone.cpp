#include"DXPMDBone.h"
#include<d3dx12.h>
#include<sstream>

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

void DXPMDBone::BoneLoad(FILE* fp) {

	unsigned short boneNum = 0;
	fread(&boneNum, sizeof(boneNum), 1, fp);

#pragma pack(1)

	//�ǂݍ��ݗp�{�[���\����
	struct PMDBone {
		char boneName[20];//�{�[����
		unsigned short parentNo;//�e�{�[���ԍ�
		unsigned short nextNo;//��[�̃{�[���ԍ�
		unsigned char type;//�{�[�����
		unsigned short ikBoneNo;//IK�{�[���ԍ�
		XMFLOAT3 pos;//�{�[���̊�_���W
	};

#pragma pack()
	std::vector<PMDBone>pmdBones(boneNum);
	fread(pmdBones.data(), sizeof(PMDBone), boneNum, fp);

	fclose(fp);


	/*std::map<std::string, BoneNode> _boneNodeTable;*/
	vector<string> boneNames(pmdBones.size());
	for (int idx = 0; idx < pmdBones.size(); ++idx) {
		auto& pb = pmdBones[idx];
		boneNames[idx] = pb.boneName;
		auto& node = _boneNodeTable[pb.boneName];
		node.boneIdx = idx;
		node.startPos = pb.pos;
	}

	for (auto& pb : pmdBones) {
		//�e�C���f�b�N�X���`�F�b�N(���蓾�Ȃ��ԍ��Ȃ��΂�)
		if (pb.parentNo >= pmdBones.size()) {
			continue;
		}
		auto parentName = boneNames[pb.parentNo];
		_boneNodeTable[parentName].children.emplace_back(&_boneNodeTable[pb.boneName]);
	}
	_boneMatrices.resize(pmdBones.size());

	std::fill(_boneMatrices.begin(), _boneMatrices.end(), XMMatrixIdentity());

}

void DXPMDBone::treeBoneCopy() {

	RecursiveMatrixMultiply(&_boneNodeTable["�Z���^�["], XMMatrixIdentity());
	copy(_boneMatrices.begin(), _boneMatrices.end(), _mappedMatrices + 1);

}

void DXPMDBone::RecursiveMatrixMultiply(BoneNode* node, const DirectX::XMMATRIX& mat) {
	_boneMatrices[node->boneIdx] = mat;
	for (auto& cnode : node->children) {

		RecursiveMatrixMultiply(cnode, _boneMatrices[cnode->boneIdx] * mat);

	}
}

void DXPMDBone::setBoneMatrices(BoneNode* node, DirectX::XMMATRIX mat) {

	_boneMatrices[node->boneIdx] = mat;

}

void DXPMDBone::setTransWorld() {
	_transform.world= XMMatrixIdentity();
}

void* DXPMDBone::Transform::operator new(size_t size) {
	return _aligned_malloc(size, 16);
}

ComPtr<ID3D12DescriptorHeap> DXPMDBone::getTransHeap() {
	return _transformHeap;
}

HRESULT DXPMDBone::CreateTransformView(ComPtr< ID3D12Device> _dev) {
	//GPU�o�b�t�@�쐬
	auto buffSize = sizeof(XMMATRIX) * (1 + _boneMatrices.size());
	buffSize = (buffSize + 0xff) & ~0xff;
	auto result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(buffSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_transformBuff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0 && "CreateCommittedResource");
		return result;
	}

	//�}�b�v�ƃR�s�[
	result = _transformBuff->Map(0, nullptr, (void**)&_mappedMatrices);
	if (FAILED(result)) {
		assert(0 && "Map");
		return result;
	}
	_mappedMatrices[0] = _transform.world;



	copy(_boneMatrices.begin(), _boneMatrices.end(), _mappedMatrices + 1);

	//�r���[�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};
	transformDescHeapDesc.NumDescriptors = 1;//�Ƃ肠�������[���h�ЂƂ�
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;

	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�f�X�N���v�^�q�[�v���
	result = _dev->CreateDescriptorHeap(&transformDescHeapDesc, IID_PPV_ARGS(_transformHeap.ReleaseAndGetAddressOf()));//����
	if (FAILED(result)) {
		assert(0&&"CreateDescriptorHeap");
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _transformBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = buffSize;
	_dev->CreateConstantBufferView(&cbvDesc, _transformHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}
