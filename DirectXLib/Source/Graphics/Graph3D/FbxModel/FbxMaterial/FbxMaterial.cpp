#include "FbxMaterial.h"
#include"../../../../DirectX12/DirectX12Manager.h"

namespace model {
	FbxMaterial::FbxMaterial(std::shared_ptr<lib::DirectX12Manager>& dx12) :
		mDx12(dx12)
	{
	}
	void FbxMaterial::loadMaterial(FbxMesh* mesh) {
		if (mesh->GetElementMaterialCount() == 0)return;
		FbxLayerElementMaterial* material = mesh->GetElementMaterial(0);
		int index = material->GetIndexArray().GetAt(0);
		FbxSurfaceMaterial* surface_material = mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(index);
	}
	void FbxMaterial::createBuffer() {
		////�}�e���A���o�b�t�@���쐬
		//auto materialBuffSize = sizeof(MaterialForHlsl);
		//materialBuffSize = (materialBuffSize + 0xff) & ~0xff;

		//auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(materialBuffSize * _materials.size());

		//auto result = mDx12->Device()->CreateCommittedResource(
		//	&heapProp,
		//	D3D12_HEAP_FLAG_NONE,
		//	&resDesc,//�ܑ̂Ȃ����ǎd���Ȃ��ł���
		//	D3D12_RESOURCE_STATE_GENERIC_READ,
		//	nullptr,
		//	IID_PPV_ARGS(_materialBuff.ReleaseAndGetAddressOf())
		//);

		////�}�b�v�}�e���A���ɃR�s�[
		//char* mapMaterial = nullptr;
		//result = _materialBuff->Map(0, nullptr, (void**)&mapMaterial);
		//for (auto& m : _materials) {
		//	*((MaterialForHlsl*)mapMaterial) = m.material;//�f�[�^�R�s�[
		//	mapMaterial += materialBuffSize;//���̃A���C�����g�ʒu�܂Ői�߂�
		//}
		//_materialBuff->Unmap(0, nullptr);

	}
	void FbxMaterial::heap() {
		//D3D12_DESCRIPTOR_HEAP_DESC materialDescHeapDesc = {};
		//materialDescHeapDesc.NumDescriptors = _materials.size() * 5;//�}�e���A�����Ԃ�(�萔1�A�e�N�X�`��3��)
		//materialDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		//materialDescHeapDesc.NodeMask = 0;

		//materialDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�f�X�N���v�^�q�[�v���
		//auto result = mDx12->Device()->CreateDescriptorHeap(&materialDescHeapDesc, IID_PPV_ARGS(_materialHeap.ReleaseAndGetAddressOf()));//����
	}
}