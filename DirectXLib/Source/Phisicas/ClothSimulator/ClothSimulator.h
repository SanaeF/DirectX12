#pragma once
#include <vector>
#include <wrl.h>
#include "../../DxGraphics/DxGraphics3D/ModelData/ModelData.h"
#include "ClothData/SpringData.h"
namespace phy {
	class ClothSimulator {
	private:
		std::vector<std::vector<int>> m_Pre_IndexID;
		static int m_time;
	public:
		ClothSimulator();
		//���_���f��-�C���f�b�N�XID�̐����p������
		ClothSimulator(std::vector<lib::ModelData> vertex, std::vector<UINT> index, std::vector<std::vector<int>> index_group);
		static void resetPower(std::vector<SpringData>& spring_data);
		//�������Z�̌v�Z���X�V
		static void update(
			std::vector<lib::ModelData>& vertex,
			std::vector<UINT> index,
			std::vector<lib::ModelData> pre_vertex,
			std::vector<std::vector<int>> pre_index_id,
			std::vector<SpringData>& spring_data
		);
		//���_���f��-�C���f�b�N�XID�̎󂯎��
		std::vector<std::vector<int>> getPreIndexID();
		~ClothSimulator();
	};
}