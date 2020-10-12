#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<unordered_map>
#include<wrl.h>
#include<memory>

class Dx12Wrapper;
class PMDRenderer;

class DXPMDMaterial {
private:

	//�V�F�[�_���ɓ�������}�e���A���f�[�^
	struct MaterialForHlsl {
		DirectX::XMFLOAT3 diffuse; //�f�B�t���[�Y�F
		float alpha; // �f�B�t���[�Y��
		DirectX::XMFLOAT3 specular; //�X�y�L�����F
		float specularity;//�X�y�L�����̋���(��Z�l)
		DirectX::XMFLOAT3 ambient; //�A���r�G���g�F
	};
	//����ȊO�̃}�e���A���f�[�^
	struct AdditionalMaterial {
		std::string texPath;//�e�N�X�`���t�@�C���p�X
		int toonIdx; //�g�D�[���ԍ�
		bool edgeFlg;//�}�e���A�����̗֊s���t���O
	};
	struct Material {
		unsigned int indicesNum;//�C���f�b�N�X��
		MaterialForHlsl material;
		AdditionalMaterial additional;
	};

	Dx12Wrapper& mDX;
	//PMDRenderer& _renderer;
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	//PMD�p���ʃe�N�X�`��(���A���A�O���C�X�P�[���O���f�[�V����)
	ComPtr<ID3D12Resource> _whiteTex = nullptr;
	ComPtr<ID3D12Resource> _blackTex = nullptr;
	ComPtr<ID3D12Resource> _gradTex = nullptr;
	ComPtr<ID3D12Resource> _vb = nullptr;
	ComPtr<ID3D12Resource> _ib = nullptr;
	ComPtr<ID3D12Resource> _transformMat = nullptr;//���W�ϊ��s��(���̓��[���h�̂�)
	ComPtr<ID3D12Resource> _materialBuff = nullptr;
	ComPtr<ID3D12DescriptorHeap> _materialHeap = nullptr;//�}�e���A���q�[�v(5�Ԃ�)
	std::vector<Material> _materials;
	std::vector<ComPtr<ID3D12Resource>> _textureResources;
	std::vector<ComPtr<ID3D12Resource>> _sphResources;
	std::vector<ComPtr<ID3D12Resource>> _spaResources;
	std::vector<ComPtr<ID3D12Resource>> _toonResources;

	D3D12_VERTEX_BUFFER_VIEW _vbView = {};
	D3D12_INDEX_BUFFER_VIEW _ibView = {};

	ID3D12Resource* CreateDefaultTexture(ComPtr< ID3D12Device> _dev,size_t width, size_t height);
	ID3D12Resource* CreateWhiteTexture(ComPtr< ID3D12Device> _dev);//���e�N�X�`���̐���
	ID3D12Resource* CreateBlackTexture(ComPtr< ID3D12Device> _dev);//���e�N�X�`���̐���
	ID3D12Resource* CreateGrayGradationTexture(ComPtr< ID3D12Device> _dev);//�O���[�e�N�X�`���̐���

public:

	void LoadMaterial(Dx12Wrapper& DX12, unsigned int vertNum, std::string strModelPath, FILE* fp);

	HRESULT CreateMaterialData(ComPtr< ID3D12Device> _dev);

	HRESULT CreateMaterialAndTextureView(ComPtr< ID3D12Device> _dev);

	DXPMDMaterial(Dx12Wrapper& DX12);

};