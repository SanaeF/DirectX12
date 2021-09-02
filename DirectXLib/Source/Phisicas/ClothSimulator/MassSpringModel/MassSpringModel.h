#pragma once
#include <vector>
#include <wrl.h>
#include "../../../DxGraphics/DxGraphics3D/ModelData/ModelData.h"
namespace phy {
	enum MODEL_FILE {
		FBX,PMX
	};
	class MassSpringModel {
	private:
		enum EDGE_TYPE {
			CORNER,//�p�̏ꍇ
			EDGE,//�ӂ̏ꍇ
			NONE,//�[����Ȃ��ꍇ
			INDEX_ERROR
		};
		struct IndexData {
			std::vector<int> constant;
			int count;
			void reset(int size) {
				constant.resize(size);
				count = 0;
				for (int ite = 0; ite < constant.size(); ite++) {
					constant[ite] = -1;
				}
			}
		};
		std::vector<std::vector<int>>m_Index_group;
		std::vector<lib::ModelParam>m_Vertex;
		std::vector<UINT>m_Index;
		std::vector<int> m_Result;
		MODEL_FILE m_File_type;
		EDGE_TYPE m_Edge_type;
	public:
		MassSpringModel(
			MODEL_FILE model_type,
			std::vector<lib::ModelParam>& vertex,
			std::vector<UINT>& index,
			std::vector<std::vector<int>>& index_group
		);
		std::vector<int> create(int num);
		~MassSpringModel();
	private:
		//3x3�s��̎��_���f���𐶐�
		void createMatrix3x3(int vertex_id);
		//3x3�s��O�ɂ���4�̂̎��_���f���𐶐�
		void createOutOfP4(int vertex_id);
		//�[���ǂ����𒲂ׂ�
		EDGE_TYPE pointType(IndexData& all_index, IndexData& related_index);
		//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ���S�Ď擾
		IndexData loadAllIndex(int vertex_id);
		//�֘A�̂��钸�_�ԍ���S�Ď擾
		IndexData loadRelatedIndex(int vertex_id, IndexData& all_index);
		//�\�������̎��_���擾
		std::vector<int> loadNearestIndex(int num, int target_index, IndexData& related_index);
		//���ׂ��\�������̎��_�ȊO���擾
		std::vector<int> loadCorners(int num, IndexData& related_index, std::vector<int>& four_point);
		IndexData exclusionMatrix3x3(int vertex_id, std::vector<int>& out_of_matrix_index);
	};
}