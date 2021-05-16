#pragma once
#include <d3d12.h>
#include <wrl.h>
namespace lib {
    class DirectX12Manager;
}
namespace compute {
	class ClothShader {
	private:
        template<typename T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;
        ID3D12RootSignature* root = nullptr;
        ID3DBlob* shader = nullptr;
        ID3D12PipelineState* pipe = nullptr;
        ID3D12DescriptorHeap* heap = nullptr;
        ID3D12Resource* rsc = nullptr;
        void* data = nullptr;
	public:
		ClothShader(ComPtr<ID3D12Device> device);
		~ClothShader();
        void ShaderCalculater(ComPtr<lib::DirectX12Manager>& dx12);
	private:
        long CreateRoot(ComPtr<ID3D12Device> device);// ���[�g�V�O�l�`���̐���
        long CreatePipe(ComPtr<ID3D12Device> device);// �p�C�v���C���̐���
        long CreateHeap(ComPtr<ID3D12Device> device);// �q�[�v�̐���
        long CreateRsc(ComPtr<ID3D12Device> device);// ���\�[�X�̐���
        void CreateUAV(ComPtr<ID3D12Device> device);// UAV�̐���
        long Map();// ���\�[�X�̃}�b�v
	};
}