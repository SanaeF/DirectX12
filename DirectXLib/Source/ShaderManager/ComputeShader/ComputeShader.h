#pragma once
#include <utility>
#include <vector>
#include<d3dx12.h>
#include <wrl.h>
#include "../../DxGraphics/DxGraphics3D/ModelData/ModelData.h"
#include "../../Phisicas/ClothSimulator/ClothData/SpringData.h"
namespace lib {
    class DirectX12Manager;
	class ComputeShader {
	private:
        struct Element{
            int size_of;
            int num;
        };
        struct ComputeShaderInfo {
            ID3DBlob* shader;
            ID3D12RootSignature* root_sig;
            ID3D12PipelineState* pipeline_state;
            ID3D12DescriptorHeap* desc_heap;
            std::vector<ID3D12Resource*> resource;
            std::vector<Element> element;
            std::vector<void*> data;
        };
        std::shared_ptr<lib::DirectX12Manager> m_Dx12;
        static std::vector<ComputeShaderInfo> m_Shader_handle;
        static int m_Handle_count;
        int m_Handle_id;
    public:
        //�V�F�[�_�[�ɑ���f�[�^�\���̌����w��
        ComputeShader(const char* file_path, const char* func_name, int slot_max, int& handle, std::shared_ptr<lib::DirectX12Manager> dx12);
        //�����ς݂̃V�F�[�_�[�p�̏�����
        ComputeShader(int handle, std::shared_ptr<lib::DirectX12Manager> dx12);

        //����f�[�^�̃T�C�Y���X���b�g�w��Ŕz�u
        void inputBufferSize(int slot_id, int size, int type);

        //�A���C�����g���邩�ǂ���
        void createResource(bool is_alingnment, int slot_id);

        //UAV������܂�
        void createUnorderdAccessView();

        //���M�p�f�[�^�̃}�b�v
        void mapInput(int slot_id, const void* data);

        //�擾�p�f�[�^�̃}�b�v
        void mapOutput(int slot_id);

        //�V�F�[�_�[���s
        void execution(int x, int y, int z);

        void* getData(int slot_id);
    private:
        //�R���p�C������V�F�[�_�[�t�@�C���̓ǂݍ���
        void loadShaderFile(const char* file_path, const char* func_name);

        //�p�C�v���C���X�e�C�g�̍쐬
        void createPipline();

        //�q�[�v�̍쐬
        void createHeap(int slot_max);
	};
}