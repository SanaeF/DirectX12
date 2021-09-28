#pragma once
#include <memory>
#include "../../DirectXLib/Source/DxGraphics/DxGraphics3D/ModelData/ModelData.h"
#include "../../DirectXLib/Source/Phisicas/ClothSimulator/ClothData/SpringData.h"
namespace lib {
	class DirectX12Manager;
}
namespace phy {
	class ClothSpringShader {
	private:
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		ID3DBlob* m_Shader;
		ID3DBlob* m_Root_blob;
		std::shared_ptr<lib::DirectX12Manager>& m_Dx12;
		ComPtr<ID3D12Device> m_Device;
		struct SimulateData {
			int vertex_num;
		};
		struct BufferData {
			SpringData spring;
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 pre_pos;
			DirectX::XMFLOAT3 color;
			int id0;
			int id1;
			int id2;
			int id3;
			int id4;
			int id5;
			int id6;
			int id7;
			int id8;
			int id9;
			int id10;
			int id11;
			bool simulate;
			//std::vector<lib::ModelParam> vertex;
			//std::vector<lib::ModelParam> pre_vert;
			//std::vector<int> mass_spring_id[SPRING_NUM];
		};
		/*static std::vector<BufferData> m_Input;
		BufferData* m_pInput;
		std::vector<SpringData> m_Output_spring;
		SpringData* m_pOutput_spring;*/
		struct ShaderInfo {
			SimulateData param;
			D3D12_CPU_DESCRIPTOR_HANDLE desc_handle;
			ID3D12RootSignature* root_sig;
			ID3D12PipelineState* pipeline;
			ID3D12DescriptorHeap* desc_heap;
			ID3D12DescriptorHeap* input_heap;
			ID3D12Resource* output_res;
			ID3D12Resource* input_res;
			std::vector<BufferData> output;
			std::vector<BufferData> input;
			bool is_created;
			void* data;
		};
		static std::vector<ShaderInfo> shaderHandler;
		//	void* data;
		bool m_Is_looped;
		int m_Model_id;
	public:
		ClothSpringShader(int model_id, std::shared_ptr<lib::DirectX12Manager>& dx_12);
		~ClothSpringShader();
		void create(lib::ModelData& model, std::vector<SpringData>& spring, std::vector<std::vector<int>>& mass_spring_id);
		void execution(int steps);
		void dataChange(int model_id, lib::ModelData& model, std::vector<SpringData>& spring);
	private:
		//シェーダーに送信するためにVectorを使わないデータ構造の初期生成
		void createMassSpringforGPU(
			lib::ModelData& model,
			std::vector<std::vector<int>>& mass_spring_id
		);
		void createOutput(
			lib::ModelData& model,
			std::vector<std::vector<int>>& mass_spring_id
		);
		bool loadShader();
		bool createBuffer();
		bool createPipeline();
		bool createHeap();
		bool createOutputResource();
		bool createUAV();
		bool outputMap();

		bool createInputUAV();
		void createInput();
		bool createInputHeap();
		bool createInputResource();
		bool createSRV();
		bool createInputBuffer();
		bool inputMap();
		void output();
	};
}