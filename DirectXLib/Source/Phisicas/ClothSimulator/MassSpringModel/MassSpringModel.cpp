#include "MassSpringModel.h"
#include "../../../VectorMath/VectorMath.h"

namespace phy {
	MassSpringModel::MassSpringModel(std::vector<lib::ModelData> vertex, std::vector<UINT> index, std::vector<std::vector<int>>index_group) :
		m_Vert_num(0),
		m_Vertex(vertex),
		m_Index(index),
		m_Index_group(index_group) {
		m_Result.resize(12);
	}
	std::vector<int> MassSpringModel::create(int num) {
		for (int ite = 0; ite < 12; ite++)m_Result[ite] = -1;
		m_Vert_num = num;
		auto all_index = loadAllIndex();
		auto related_index = loadRelatedIndex(all_index);
		m_Edge_type = pointType(all_index, related_index);
		std::vector<int> nearest_index;
		if (m_Edge_type == EDGE_TYPE::CORNER) {//�p�̏ꍇ(����6�̃C���f�b�N�X)
			nearest_index = loadNearestIndex(2, related_index);//2�̍Ŋ񎿓_���擾
			loadCorners(1, related_index, nearest_index);
		}
		if (m_Edge_type == EDGE_TYPE::EDGE) {//�[�̏ꍇ(����12�̃C���f�b�N�X)
			nearest_index = loadNearestIndex(3, related_index);//3�̍Ŋ񎿓_���擾
			loadCorners(2, related_index, nearest_index);
		}
		if (m_Edge_type == EDGE_TYPE::NONE) {//�[�łȂ��ꍇ(����24�̃C���f�b�N�X)
			nearest_index = loadNearestIndex(4, related_index);//4�̍Ŋ񎿓_���擾
			loadCorners(4, related_index, nearest_index);
		}
		loadAddNearest(nearest_index);
		return m_Result;
	}
	//�[���ǂ����𒲂ׂ�
	MassSpringModel::EDGE_TYPE
		MassSpringModel::pointType(IndexData all_index, IndexData related_index) {
		if (related_index.count == 3)return EDGE_TYPE::CORNER;//�p�ł����B
		if (related_index.count == 5)return EDGE_TYPE::EDGE;//�ӂ���B
		if (related_index.count == 8)return EDGE_TYPE::NONE;//���ǁA�[����Ȃ�
		assert("�z�V�~�����[�^�[�ɑΉ��o���Ȃ����f�����g�p���Ă��܂��B�q���g:�|���S���͎l�p�`�Ő�������B");
		return EDGE_TYPE::INDEX_ERROR;
	}
	//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ���S�Ď擾
	MassSpringModel::IndexData
		MassSpringModel::loadAllIndex() {
		IndexData data;
		data.reset(12);
		//���z���_�p�̃C���f�b�N�X��������(-1����)
		for (int ite = 0; ite < 6; ite++) {
			data.constant[ite] = -1;
		}
		//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ����m��
		for (int num = 0; num < m_Index_group[m_Vert_num].size(); num++) {
			data.constant[data.count] = m_Index_group[m_Vert_num][num];
			data.count++;
		}
		return data;
	}
	//�֘A�̂��钸�_�ԍ���S�Ď擾
	MassSpringModel::IndexData
		MassSpringModel::loadRelatedIndex(IndexData all_index) {
		IndexData data;
		data.reset(48);
		//�擾�����C���f�b�N�X�ɑΉ�����|���S���̑S�ẴC���f�b�N�X���擾
		for (int ite = 0; ite < all_index.count; ite++) {
			if (all_index.constant[ite] != -1) {
				int id = all_index.constant[ite] % 6;
				for (int ite2 = 0; ite2 < 6; ite2++) {
					auto id2 = all_index.constant[ite] - id + ite2;
					//����̃C���f�b�N�X�����O
					bool is_input = true;
					for (int ite3 = 0; ite3 < data.count; ite3++) {
						if (m_Index[id2] == m_Index[data.constant[ite3]]) {
							is_input = false;
							break;
						}
					}
					if (is_input)for (int ite3 = 0; ite3 < all_index.count; ite3++) {
						if (id2 == all_index.constant[ite3]) {
							is_input = false;
							break;
						}
					}
					if (!is_input)continue;
					data.constant[data.count] = id2;
					data.count++;
				}
			}
		}
		return data;
	}
	//�\�������̎��_���擾
	std::vector<int>
		MassSpringModel::loadNearestIndex(int num, IndexData related_index) {
		IndexData result;
		result.reset(related_index.count);
		std::vector<float> distance(related_index.count);
		//���_�ԍ����m�̍��W���߂����ŕ��ѕς���
		for (int ite = 0; ite < related_index.count; ite++) {
			auto dist = lib::VectorMath::distance(
				m_Vertex[m_Vert_num].position, m_Vertex[m_Index[related_index.constant[ite]]].position
			);
			//���_���߂����ɕ��ёւ�
			for (int ite2 = 0; ite2 < related_index.count; ite2++) {
				if (distance[ite2] == 0 || distance[ite2] > dist) {
					for (int ite3 = 0; ite3 < num; ite3++) {
						auto cid = num - ite3 - 2;
						if (cid < 0)continue;
						result.constant[cid + 1] = result.constant[cid];
					}
					result.constant[ite2] = related_index.constant[ite];
					break;
				}
			}
		}
		for (int ite = 0; ite < num; ite++) {
			m_Result[ite] = m_Index[result.constant[ite]];
		}
		result.constant.resize(num);
		return result.constant;
	}
	//���ׂ��\�������̎��_�ȊO���擾
	std::vector<int>
		MassSpringModel::loadCorners(int num, IndexData related_index, std::vector<int> four_point) {
		IndexData data;
		data.reset(36);
		//�擾�������͂̃C���f�b�N�X�f�[�^�𒲂ׂ�
		for (int ite = 0; ite < related_index.count; ite++) {
			bool is_input = true;
			//�\�������̎��_��Ώۂ��珜�O����
			for (int ite2 = 0; ite2 < four_point.size(); ite2++) {
				if (related_index.constant[ite] == four_point[ite2]) {
					is_input = false;
					break;
				}
			}
			if (is_input) {
				data.constant[data.count] = related_index.constant[ite];
				data.count++;
			}
		}
		//�o�͗p�̔z��Ɍ��ʂ���
		std::vector<int> result(num);
		for (int ite = 0; ite < num; ite++) {
			result[ite] = data.constant[ite];
			m_Result[4 + ite] = m_Index[result[ite]];
		}
		data.constant.reserve(num);
		return data.constant;
	}
	void MassSpringModel::loadAddNearest(std::vector<int> nearest_index) {
		for (int ite = 0; ite < 4; ite++) {
			auto searched_nearest = loadNearestSearch(ite, nearest_index);
			loadAddNearestIndex(ite, searched_nearest);
		}
	}
	MassSpringModel::IndexData
		MassSpringModel::loadNearestSearch(int num, std::vector<int> nearest_index) {
		IndexData result;
		if (m_Result[num] == -1)return result;
		result.reset(12);
		//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ����m��
		for (int ite = 0; ite < m_Index_group[m_Result[num]].size(); ite++) {
			result.constant[result.count] = m_Index_group[m_Result[num]][ite];
			result.count++;
		}
		result = loadRelatedIndex(result);
		for (int ite = 0; ite < result.constant.size(); ite++) {
			if (result.constant[ite] == -1)continue;
			//3x3�̃C���f�b�N�XID�����O
			for (int ite2 = 0; ite2 < 8; ite2++) {
				if (m_Index[result.constant[ite]] == m_Result[ite2] || m_Index[result.constant[ite]] == m_Vert_num) {
					result.constant[ite] = -1;
					break;
				}
			}
		}
		return result;
	}
	MassSpringModel::IndexData
		MassSpringModel::loadAddNearestIndex(int num, IndexData searched_nearest) {
		IndexData result;
		if (searched_nearest.constant.size() == 0)return result;
		result.reset(searched_nearest.count);
		//���_�ԍ����m�̍��W���߂����ŕ��ѕς���
		std::vector<float> distance(searched_nearest.count);
		for (int ite2 = 0; ite2 < searched_nearest.count; ite2++) {
			if (searched_nearest.constant[ite2] == -1)continue;
			auto dist = lib::VectorMath::distance(
				m_Vertex[m_Result[num]].position, m_Vertex[m_Index[searched_nearest.constant[ite2]]].position
			);
			//���_���߂����ɕ��ёւ�
			for (int ite3 = 0; ite3 < searched_nearest.count; ite3++) {
				if (distance[ite3] == 0 || distance[ite3] > dist) {
					for (int ite4 = 0; ite4 < searched_nearest.count; ite4++) {
						auto cid = searched_nearest.count - ite4 - 2;
						if (cid < 0)continue;
						result.constant[cid + 1] = result.constant[cid];
					}
					result.constant[ite3] = searched_nearest.constant[ite2];
					break;
				}
			}
			m_Result[8 + num] = m_Index[result.constant[0]];
		}
		return result;
	}
	MassSpringModel::~MassSpringModel() {

	}
}