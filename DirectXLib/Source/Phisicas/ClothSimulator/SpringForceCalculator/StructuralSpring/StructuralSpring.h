#pragma once
#include <vector>
#include "../../../../DxGraphics/DxGraphics3D/ModelData/ModelData.h"
#include "../../ClothData/SpringData.h"
namespace phy {
	class StructuralSpring {
	public:
		//����
		float tension;//����������̂ɂǂ̂��炢��R���邩
		float compression;//���k�����̂ɂǂ̂��炢��R���邩
		//����
		float tension_damping;//����������Ƃ��̌�����
		float compression_damping;//���͎��̌�����
		float damping;//����
	private:
		float m_Shrink;
		float m_Stretch;
	public:
		StructuralSpring(float shrink, float stretch);
		void solver(
			int vert_id,
			SpringData& spring_data,
			std::vector<lib::ModelParam>& vertex,
			std::vector<lib::ModelParam>& pre_vert,
			std::vector<std::vector<int>>& pre_index_id
		);
		void solver2(
			int vert_id,
			std::vector<SpringData>& spring_data,
			std::vector<lib::ModelParam>& vertex,
			std::vector<lib::ModelParam>& pre_vert,
			std::vector<std::vector<int>>& pre_index_id
		);
	private:
		void tension_solver(
			int vert_id,
			std::vector<SpringData>& spring_data,
			std::vector<lib::ModelParam>& vertex,
			std::vector<lib::ModelParam>& pre_vert,
			std::vector<std::vector<int>>& pre_index_id
		);
		void compression_solver(
			int vert_id,
			std::vector<SpringData>& spring_data,
			std::vector<lib::ModelParam>& vertex,
			std::vector<lib::ModelParam>& pre_vert,
			std::vector<std::vector<int>>& pre_index_id
		);
		DirectX::XMFLOAT3 calcForce(
			lib::ModelParam& vert1,
			lib::ModelParam& vert2,
			float length,
			float constant
		);
	};
}