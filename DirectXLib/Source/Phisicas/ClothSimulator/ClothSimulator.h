#pragma once
#include <vector>
#include <memory>
#include <wrl.h>
#include "../../DxGraphics/DxGraphics3D/ModelData/ModelData.h"
#include "ClothData/SpringData.h"

namespace lib {
	class DirectX12Manager;
}
namespace phy {
	class ClothSimulator {
	private:
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		std::vector<std::vector<int>> m_Pre_IndexID;
		static std::vector<std::vector<ClothData>> m_ClothData;
		static int m_time;
	public:
		ClothSimulator();
		//���_���f��-�C���f�b�N�XID�̐����p������
		ClothSimulator(std::vector<lib::ModelParam> vertex, std::vector<UINT> index, std::vector<std::vector<int>> index_group);
		static void createClothData(
			int handle,
			lib::ModelData& model,
			std::vector<std::vector<int>>& mass_spring_id,
			std::vector<SpringData>& spring_data
		);
		static void resetPower(std::vector<SpringData>& spring_data);
		//�������Z�̌v�Z���X�V
		static void update(
			lib::ModelData& model,
			std::vector<SpringData>& spring_data,
			std::vector<std::vector<int>>& mass_spring_id
		);
		static void execution(
			int model_id,
			lib::ModelData& model,
			std::vector<SpringData>& spring_data,
			std::vector<std::vector<int>>& mass_spring_id,
			std::shared_ptr<lib::DirectX12Manager>& dx_12
		);
		//���_���f��-�C���f�b�N�XID�̎󂯎��
		std::vector<std::vector<int>> getPreIndexID();
		~ClothSimulator();
	private:
	};
}