#pragma once
#include <vector>
#include <wrl.h>
#include "../../../Graphics/Graph3D/Vertex/Vertex.h"
namespace phy {
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
		std::vector<lib::Vertex>m_Vertex;
		std::vector<UINT>m_Index;
		std::vector<int> m_Result;
		EDGE_TYPE m_Edge_type;
		int m_Vert_num;
	public:
		MassSpringModel(std::vector<lib::Vertex> vertex, std::vector<UINT> index, std::vector<std::vector<int>>index_group);
		std::vector<int> create(int num);
		~MassSpringModel();
	private:
		//�[���ǂ����𒲂ׂ�
		EDGE_TYPE pointType(IndexData all_index, IndexData related_index);
		//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ���S�Ď擾
		IndexData loadAllIndex();
		//�֘A�̂��钸�_�ԍ���S�Ď擾
		IndexData loadRelatedIndex(IndexData all_index);
		//�\�������̎��_���擾
		std::vector<int> loadNearestIndex(int num, IndexData related_index);
		//���ׂ��\�������̎��_�ȊO���擾
		std::vector<int> loadCorners(int num, IndexData related_index, std::vector<int> four_point);
		void loadAddNearest(std::vector<int> nearest_index);
		IndexData loadNearestSearch(int num, std::vector<int> nearest_index);
		IndexData loadAddNearestIndex(int num, IndexData searched_nearest);
	};
}