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

class DXVMDMotion;

class DXPMDModel{
private:
	//PMDRenderer& _renderer;
	std::shared_ptr<PMDRenderer> _renderer;
	std::shared_ptr<Dx12Wrapper> _dx12;

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

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
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

	std::shared_ptr<DXVMDMotion>Motion;

	D3D12_VERTEX_BUFFER_VIEW _vbView = {};
	D3D12_INDEX_BUFFER_VIEW _ibView = {};
	DirectX::XMFLOAT3 _rotator;
	DirectX::XMFLOAT3 _pos;
	DWORD _startTime;

	float _angle;//�e�X�g�pY����]

public:

	DXPMDModel(std::shared_ptr<Dx12Wrapper> dx, std::shared_ptr<PMDRenderer> renderer, const char* path);
	~DXPMDModel();

	///�N���[���͒��_����у}�e���A���͋��ʂ̃o�b�t�@������悤�ɂ���
	DXPMDModel* Clone();

	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void LoadVMDFile(const char* filepath, const char* name);
	void PlayAnimation();
	void Update();
	void Draw();

	const DirectX::XMFLOAT3& GetPosition()const;
	const DirectX::XMFLOAT3& GetRotate()const;

private:
	//PMD�t�@�C���̃��[�h
	void LoadPMDFile(const char* path);

	void MotionUpdate();

	void LoadMaterial(unsigned int vertNum, std::string strModelPath, FILE* fp);

	friend PMDRenderer;

	//�ǂݍ��񂾃}�e���A�������ƂɃ}�e���A���o�b�t�@���쐬
	HRESULT CreateMaterialData();
	//�}�e���A�����e�N�X�`���̃r���[���쐬
	HRESULT CreateMaterialAndTextureView();

};