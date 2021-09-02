#include "MassSpringModel.h"
#include "../../../VectorMath/VectorMath.h"

namespace phy {
	MassSpringModel::MassSpringModel(
		MODEL_FILE model_type, 
		std::vector<lib::ModelParam>& vertex,
		std::vector<UINT>& index,
		std::vector<std::vector<int>>& index_group
	) :
		m_File_type(model_type),
		m_Vertex(vertex),
		m_Index(index),
		m_Index_group(index_group)
	{
		m_Result.resize(8);
	}
	std::vector<int> MassSpringModel::create(int num) {
		for (int ite = 0; ite < 8; ite++)m_Result[ite] = -1;
		createMatrix3x3(num);
		createOutOfP4(num);
		return m_Result;
	}
	//3x3�s��̎��_���f���𐶐�
	void MassSpringModel::createMatrix3x3(int vertex_id) {
		//�Ή�����S�C���f�b�N�X���擾
		auto all_index = loadAllIndex(vertex_id);
		//���g�ȊO�̎���(3x3)�̃C���f�b�N�X���擾
		auto related_index = loadRelatedIndex(vertex_id, all_index);
		//����(3x3)�̃C���f�b�N�X���ɉ����ă^�C�v�𕪂���
		m_Edge_type = pointType(all_index, related_index);
		//���S����\�������̃C���f�b�N�X�ƁA4�p�̃C���f�b�N�X�𐶐�
		std::vector<int> nearest_index;
		std::vector<int> corners_index;
		if (m_Edge_type == EDGE_TYPE::CORNER) {//�p�̏ꍇ(����6�̃C���f�b�N�X)
			nearest_index = loadNearestIndex(2, vertex_id, related_index);//2�̍Ŋ񎿓_���擾
			corners_index = loadCorners(1, related_index, nearest_index);
		}
		if (m_Edge_type == EDGE_TYPE::EDGE) {//�[�̏ꍇ(����12�̃C���f�b�N�X)
			nearest_index = loadNearestIndex(3, vertex_id, related_index);//3�̍Ŋ񎿓_���擾
			corners_index = loadCorners(2, related_index, nearest_index);
		}
		if (m_Edge_type == EDGE_TYPE::NONE) {//�[�łȂ��ꍇ(����24�̃C���f�b�N�X)
			nearest_index = loadNearestIndex(4, vertex_id, related_index);//4�̍Ŋ񎿓_���擾
			corners_index = loadCorners(4, related_index, nearest_index);
		}
		//�o�͗p�̃x�N�^�[�z��Ɋi�[
		for (int ite = 0; ite < nearest_index.size(); ite++) {
			m_Result[ite] = m_Index[nearest_index[ite]];
		}
		for (int ite = 0; ite < corners_index.size(); ite++) {
			m_Result[4 + ite] = m_Index[corners_index[ite]];
		}
	}
	//3x3�s��O�ɂ���4�̂̎��_���f���𐶐�
	void MassSpringModel::createOutOfP4(int vertex_id) {
		//int point_num = 0;
		//if (m_Edge_type == EDGE_TYPE::CORNER)point_num = 2;
		//if (m_Edge_type == EDGE_TYPE::EDGE)point_num = 3;
		//if (m_Edge_type == EDGE_TYPE::NONE)point_num = 4;
		//for (int ite = 0; ite < point_num; ite++) {
		//	//�Ή�����S�C���f�b�N�X���擾
		//	auto all_index = loadAllIndex(m_Result[ite]);
		//	//���g�ȊO�̎���(3x3)�̃C���f�b�N�X���擾
		//	auto related_index = loadRelatedIndex(m_Result[ite], all_index);
		//	//3x3�̃C���f�b�N�X�����O
		//	auto data = exclusionMatrix3x3(vertex_id, related_index.constant);
		//	//���S����߂��C���f�b�N�X
		//	auto nearest_index = loadNearestIndex(data.count, vertex_id, data);
		//	if (nearest_index.size() != 0)m_Result[8 + ite] = m_Index[nearest_index[0]];
		//}
	}
	//�[���ǂ����𒲂ׂ�
	MassSpringModel::EDGE_TYPE
		MassSpringModel::pointType(IndexData& all_index, IndexData& related_index) {
		if (related_index.count == 3)return EDGE_TYPE::CORNER;//�p�ł����B
		if (related_index.count == 5)return EDGE_TYPE::EDGE;//�ӂ���B
		if (related_index.count == 8)return EDGE_TYPE::NONE;//���ǁA�[����Ȃ�
		if (m_File_type == MODEL_FILE::FBX) {
			if (related_index.count == 3)return EDGE_TYPE::CORNER;//�p�ł����B
			if (related_index.count == 5)return EDGE_TYPE::EDGE;//�ӂ���B
			if (related_index.count == 8)return EDGE_TYPE::NONE;//���ǁA�[����Ȃ�
		}
		if (m_File_type == MODEL_FILE::PMX) {
			if (related_index.count == 3)return EDGE_TYPE::CORNER;//�p�ł����B
			if (related_index.count == 5)return EDGE_TYPE::EDGE;//�ӂ���B
			if (related_index.count == 8)return EDGE_TYPE::NONE;//���ǁA�[����Ȃ�
		}
		assert(0 && "�z�V�~�����[�^�[�ɑΉ��o���Ȃ����f�����g�p���Ă��܂��B���߂��");
		return EDGE_TYPE::INDEX_ERROR;
	}
	//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ���S�Ď擾
	MassSpringModel::IndexData
		MassSpringModel::loadAllIndex(int vertex_id) {
		IndexData data;
		data.reset(12);
		//���z���_�p�̃C���f�b�N�X��������(-1����)
		for (int ite = 0; ite < 6; ite++) {
			data.constant[ite] = -1;
		}
		//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ����m��
		for (int num = 0; num < m_Index_group[vertex_id].size(); num++) {
			data.constant[data.count] = m_Index_group[vertex_id][num];
			data.count++;
		}
		return data;
	}
	//�֘A�̂��钸�_�ԍ���S�Ď擾
	MassSpringModel::IndexData
		MassSpringModel::loadRelatedIndex(int vertex_id, IndexData& all_index) {
		IndexData data;
		data.reset(48);
		if (m_File_type == MODEL_FILE::FBX) {
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
							if (m_Index[id2] == m_Index[all_index.constant[ite3]]) {
								is_input = false;
								break;
							}
						}
						if (is_input) {
							if (m_Index[id2] == vertex_id)is_input = false;
						}
						if (!is_input)continue;
						data.constant[data.count] = id2;
						data.count++;
					}
				}
			}
			return data;
		}
		if (m_File_type == MODEL_FILE::PMX) {
			//���_������̃C���f�b�N�X�ɑΉ�����O�p�`�̃C���f�b�N�X�ԍ������ׂĎ擾����
			for (int ite = 0; ite < all_index.count; ite++) {

			}
		}
	}
	//�\�������̎��_���擾
	std::vector<int>
		MassSpringModel::loadNearestIndex(int num, int target_index, IndexData& related_index) {
		IndexData result;
		result.reset(related_index.count);
		std::vector<float> distance(related_index.count);
		//���_�ԍ����m�̍��W���߂����ŕ��ѕς���
		for (int ite = 0; ite < related_index.count; ite++) {
			auto dist = lib::VectorMath::distance(
				m_Vertex[target_index].position, m_Vertex[m_Index[related_index.constant[ite]]].position
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
		result.constant.resize(num);
		return result.constant;
	}
	//���ׂ��\�������̎��_�ȊO���擾
	std::vector<int>
		MassSpringModel::loadCorners(int num, IndexData& related_index, std::vector<int>& four_point) {
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
		data.constant.resize(num);
		return data.constant;
	}
	MassSpringModel::IndexData 
		MassSpringModel::exclusionMatrix3x3(int vertex_id, std::vector<int>& out_of_matrix_index) {
		auto data = out_of_matrix_index;
		for (int ite = 0; ite < data.size(); ite++) {
			for (int ite2 = 0; ite2 < m_Result.size(); ite2++) {
				if (data[ite] == -1)continue;
				auto id = m_Index[data[ite]];
				if (id == m_Result[ite2] || id == vertex_id) {
					if (m_Result[ite2] != -1)data[ite] = -1;
				}
			}
		}
		IndexData result;
		result.reset(data.size());
		for (int ite = 0; ite < data.size(); ite++) {
			if (data[ite] != -1) {
				result.constant[result.count] = data[ite];
				result.count++;
			}
		}
		result.constant.resize(result.count);
		return result;
	}
	MassSpringModel::~MassSpringModel() {

	}
}