#pragma once

#include<d3d12.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<unordered_map>
#include<wrl.h>
#include<memory>

//class DXPMDBone;
class DXVMDMotion;

class DXPMDModel;
class Dx12Wrapper;
class PMDRenderer;

class PMDActor {
	friend PMDRenderer;
private:

	PMDRenderer& _renderer;
	Dx12Wrapper& _dx12;

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	//std::shared_ptr<DXPMDModel> VMD_model;
	//std::shared_ptr<DXPMDBone>Bone;

	std::shared_ptr<DXVMDMotion>Motion;

	//���_�֘A
	ComPtr<ID3D12Resource> _vb = nullptr;
	ComPtr<ID3D12Resource> _ib = nullptr;
	D3D12_VERTEX_BUFFER_VIEW _vbView = {};
	D3D12_INDEX_BUFFER_VIEW _ibView = {};

	ComPtr<ID3D12Resource> _transformMat = nullptr;//���W�ϊ��s��(���̓��[���h�̂�)

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

	//�܂Ƃ߂�����
	struct Material {
		unsigned int indicesNum;//�C���f�b�N�X��
		MaterialForHlsl material;
		AdditionalMaterial additional;
	};

	//�}�e���A���֘A
	std::vector<Material> _materials;
	ComPtr<ID3D12Resource> _materialBuff = nullptr;
	std::vector<ComPtr<ID3D12Resource>> _textureResources;
	std::vector<ComPtr<ID3D12Resource>> _sphResources;
	std::vector<ComPtr<ID3D12Resource>> _spaResources;
	std::vector<ComPtr<ID3D12Resource>> _toonResources;


	//�ǂݍ��񂾃}�e���A�������ƂɃ}�e���A���o�b�t�@���쐬
	HRESULT CreateMaterialData();

	ComPtr< ID3D12DescriptorHeap> _materialHeap = nullptr;//�}�e���A���q�[�v(5�Ԃ�)
	//�}�e���A�����e�N�X�`���̃r���[���쐬
	HRESULT CreateMaterialAndTextureView();


	//PMD�t�@�C���̃��[�h
	HRESULT LoadPMDFile(const char* path);

	DWORD _startTime;

	void MotionUpdate();


	float _angle;//�e�X�g�pY����]

public:
	PMDActor(const char* filepath, PMDRenderer& renderer);
	~PMDActor();

	///�N���[���͒��_����у}�e���A���͋��ʂ̃o�b�t�@������悤�ɂ���

	PMDActor* Clone();

	void LoadVMDFile(const char* filepath, const char* name);
	void PlayAnimation();
	void Update();
	void Draw();

};