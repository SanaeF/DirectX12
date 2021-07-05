#pragma once

#include<d3d12.h>
#include<vector>
#include<wrl.h>
#include<memory>

class Dx12Wrapper;
class DXPMDModel;

class PMDRenderer{
	friend DXPMDModel;
private:
	//Dx12Wrapper& _dx12;

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr< ID3D12PipelineState> _pipeline = nullptr;//PMD�p�p�C�v���C��
	ComPtr< ID3D12RootSignature> _rootSignature = nullptr;//PMD�p���[�g�V�O�l�`��

	//PMD�p���ʃe�N�X�`��(���A���A�O���C�X�P�[���O���f�[�V����)
	ComPtr<ID3D12Resource> _whiteTex = nullptr;
	ComPtr<ID3D12Resource> _blackTex = nullptr;
	ComPtr<ID3D12Resource> _gradTex = nullptr;

	ID3D12Resource* CreateDefaultTexture(size_t width, size_t height);
	ID3D12Resource* CreateWhiteTexture();//���e�N�X�`���̐���
	ID3D12Resource*	CreateBlackTexture();//���e�N�X�`���̐���
	ID3D12Resource*	CreateGrayGradationTexture();//�O���[�e�N�X�`���̐���

	std::vector<std::shared_ptr<DXPMDModel>> _actors;
	std::shared_ptr<Dx12Wrapper> _dx12;

	//�p�C�v���C��������
	HRESULT CreateGraphicsPipelineForPMD();
	//���[�g�V�O�l�`��������
	HRESULT CreateRootSignature();

	bool CheckShaderCompileResult(HRESULT result, ID3DBlob* error = nullptr);

public:
	PMDRenderer(std::shared_ptr<Dx12Wrapper> dx12);
	~PMDRenderer();

	void Init();

	void AddActor(std::shared_ptr<DXPMDModel> actor);
	void AddActor(const char* filepath);

	ID3D12PipelineState* GetPipelineState();
	ID3D12RootSignature* GetRootSignature();

	void Update();
	void BeforeDraw();

	void Draw();

	void AnimationStart();
};
