#include "VirtualPoint.h"
#include "../../Vector3D/Vector3D.h"

namespace model {
	VirtualPoint::VirtualPoint(int vert_num, std::vector<lib::Vertex> vertex, std::vector<UINT> index) :
		m_Vert_num(vert_num),
		m_Vertex(vertex),
		m_Index(index)
	{
	}
	std::vector<int> VirtualPoint::create() {
		std::vector<int> result(8);
		auto all_index = loadAllIndex();
		if (all_index.count == 6)result = createIndex6(all_index);
		if (all_index.count == 8){ }
		return result;
	}
	std::vector<int> VirtualPoint::createIndex6(IndexData all_index) {
		auto related_index = loadRelatedIndex(all_index);
		auto high_indeces = loadHighIndex(related_index);
		auto low_indeces = loadLowIndex(related_index);
		auto width_indeces = loadWidthIndex(high_indeces, low_indeces, related_index);
		auto high_index = nearestIndexID(high_indeces);
		auto low_index = nearestIndexID(low_indeces);
		//���ʂ���
		std::vector<int> result(4);
		if (width_indeces[0] == -1)result[0] = m_Vertex.size();
		else result[0] = m_Index[width_indeces[0]];
		if (high_index == -1)result[1] = m_Vertex.size();
		else result[1] = m_Index[high_index];
		if (width_indeces[1] == -1)result[2] = m_Vertex.size();
		else result[2] = m_Index[width_indeces[1]];
		if (low_index == -1)result[3] = m_Vertex.size();
		else result[3] = m_Index[low_index];
		auto corners_index = loadCorners(related_index, result);
		result.resize(8);
		for (int ite = 0; ite < 4; ite++) {
			if (corners_index[ite] == -1)result[4 + ite] = m_Vertex.size();
			else result[4 + ite] = m_Index[corners_index[ite]];
		}
		std::vector<float> lengthTest(8);
		for (int ite = 0; ite < 8; ite++) {
			if (result[ite] != m_Vertex.size())lengthTest[ite] = lib::Vector3D::distance(m_Vertex[result[0]].position, m_Vertex[result[ite]].position);
		}
		return result;
	}
	//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ���S�Ď擾
	VirtualPoint::IndexData VirtualPoint::loadAllIndex() {
		IndexData data;
		data.reset(12);
		//���z���_�p�̃C���f�b�N�X��������(-1����)
		for (int ite = 0; ite < 6; ite++) {
			data.constant[ite] = -1;
		}
		for (int num = 0; num < m_Index.size(); num++) {
			if (m_Vert_num == m_Index[num]) {
				data.constant[data.count] = num;
				data.count++;
			}
			if (data.count == 6)break;
		}
		return data;
	}
	//�֘A�̂��钸�_�ԍ���S�Ď擾
	VirtualPoint::IndexData VirtualPoint::loadRelatedIndex(IndexData all_index) {
		IndexData data;
		data.reset(36);
		//�擾�����C���f�b�N�X�ɑΉ�����|���S���̑S�ẴC���f�b�N�X���擾
		for (int ite = 0; ite < all_index.count; ite++) {
			if (all_index.constant[ite] != -1) {
				int id = all_index.constant[ite] % 6;
				for (int ite2 = 0; ite2 < 6; ite2++) {
					data.constant[data.count] = all_index.constant[ite] - id + ite2;
					data.count++;
				}
			}
		}
		return data;
	}
	//�֘A�̃C���f�b�N�X�̒�����A��̗���擾
	std::vector<int>
	VirtualPoint::loadHighIndex(IndexData related_index) {
		IndexData data;
		data.reset(36);
		for (int ite = 0; ite < 36; ite++)data.constant[ite] = -1;
		//���ׂĂ��钸�_�J���[�������ǂ������擾
		auto is_white = isWhiteVertex(m_Vertex[m_Vert_num]);
		for (int ite = 0; ite < related_index.count; ite++) {
			auto id = related_index.constant[ite];
			//���ׂĂ��钸�_�̃J���[�ƈ�v���Ă���Ɠ�����Ȃ̂ňȉ��̏��������s���Ȃ�
			if (is_white == isWhiteVertex(m_Vertex[m_Index[id]]))continue;
			//���ׂĂ��钸�_����ɂ�������z��ɑ��
			if (m_Index[id] != m_Vert_num && m_Vertex[m_Vert_num].position.y > m_Vertex[m_Index[id]].position.y) {
				//ID�ƃf�[�^���C���f�b�N�X�I�ɓ��l���ǂ����𒲂ׂ�
				if (isEquivalent(id, data))continue;
				data.constant[data.count] = id;
				data.count++;
			}
		}
		//�o�͗p�̔z��Ɍ��ʂ���
		std::vector<int> result(data.count);
		for (int ite = 0; ite < data.count; ite++) {
			result[ite] = data.constant[ite];
		}
		return result;
	}
	//�֘A�̃C���f�b�N�X�̒�����A���̗���擾
	std::vector<int>
	VirtualPoint::loadLowIndex(IndexData related_index) {
		IndexData data;
		data.reset(36);
		//���ׂĂ��钸�_�J���[�������ǂ������擾
		auto is_white = isWhiteVertex(m_Vertex[m_Vert_num]);
		for (int ite = 0; ite < related_index.count; ite++) {
			auto id = related_index.constant[ite];
			//���ׂĂ��钸�_�̃J���[�ƈ�v���Ă���Ɠ�����Ȃ̂ňȉ��̏��������s���Ȃ�
			if (is_white == isWhiteVertex(m_Vertex[m_Index[id]]))continue;
			//���ׂĂ��钸�_����ɂ�������z��ɑ��
			if (m_Index[id] != m_Vert_num && m_Vertex[m_Vert_num].position.y < m_Vertex[m_Index[id]].position.y) {
				//ID�ƃf�[�^���C���f�b�N�X�I�ɓ��l���ǂ����𒲂ׂ�
				if (isEquivalent(id, data))continue;
				data.constant[data.count] = id;
				data.count++;
			}
		}
		//�o�͗p�̔z��Ɍ��ʂ���
		std::vector<int> result(data.count);
		for (int ite = 0; ite < data.count; ite++) {
			result[ite] = data.constant[ite];
		}
		return result;
	}
	//�֘A�̃C���f�b�N�X�̒�����A������̃C���f�b�N�X��2�擾
	std::vector<int> 
	VirtualPoint::loadWidthIndex(std::vector<int> high_index, std::vector<int> low_index, IndexData related_index) {
		IndexData data;
		data.reset(2);
		//���ׂĂ��钸�_�J���[�������ǂ������擾
		auto is_white = isWhiteVertex(m_Vertex[m_Vert_num]);
		for (int ite = 0; ite < related_index.count; ite++) {
			auto id = related_index.constant[ite];
			//ID�ƃf�[�^���C���f�b�N�X�I�ɓ��l���ǂ����𒲂ׂ�
			if (isEquivalent(id, data))continue;
			//���ׂĂ��钸�_�̃J���[�ƈ�v���Ă���Ɠ�����Ȃ̂ňȉ��̏��������s����
			if (is_white == isWhiteVertex(m_Vertex[m_Index[id]])) {
				//�E������������Ȃ��̂Ŏ�芸�������Ԃɑ��
				data.constant[data.count] = id;
				data.count++;
				if (data.count >= 2)break;
			}
		}
		return data.constant;
	}
	//���ׂ��\�������̎��_�ȊO���擾
	std::vector<int> 
	VirtualPoint::loadCorners(IndexData related_index, std::vector<int> four_point) {
		IndexData data;
		data.reset(36);
		for (int ite = 0; ite < related_index.count; ite++) {
			bool is_input = true;
			auto id = m_Index[related_index.constant[ite]];
			for (int ite2 = 0; ite2 < data.count; ite2++) {
				if (data.constant[ite2] == -1)continue;
				auto id2 = m_Index[data.constant[ite2]];
				if (id2 == id)is_input = false;
				if (lib::Vector3D::distance(m_Vertex[id].position, m_Vertex[id2].position) == 0)is_input = false;
			}
			for (int ite2 = 0; ite2 < four_point.size(); ite2++) {
				auto id2 = four_point[ite2];
				if (id2 == m_Vertex.size()) continue;
				if (lib::Vector3D::distance(m_Vertex[id].position, m_Vertex[id2].position) == 0)is_input = false;
			}
			if (is_input) {
				data.constant[data.count] = related_index.constant[ite];
				data.count++;
			}
		}
		//�o�͗p�̔z��Ɍ��ʂ���
		std::vector<int> result(4);
		for (int ite = 0; ite < result.size(); ite++) {
			result[ite] = data.constant[ite];
		}
		return result;
	}
	//���ׂĂ��钸�_�ɑ΂��ċ߂��̒��_��T��(loadHighIndex��������loadLowIndex�Ő��������C���f�b�N�X)
	int VirtualPoint::nearestIndexID(std::vector<int> indeces) {
		float dist_max = lib::Vector3D::mulAdd(m_Vertex[m_Vert_num].position, m_Vertex[m_Vert_num].position);
		int index_id = -1;
		if (indeces.size() == 0)return index_id;
		for (int ite = 0; ite < indeces.size(); ite++) {
			auto dist = lib::Vector3D::distance(m_Vertex[m_Vert_num].position, m_Vertex[m_Index[indeces[ite]]].position);
			if (dist_max > dist) {
				dist_max = dist;
				index_id = ite;
			}
		}
		return index_id;
	}
	//���_�J���[�������ǂ������擾
	bool VirtualPoint::isWhiteVertex(lib::Vertex vert) {
		if (vert.color.x == 1.f &&
			vert.color.y == 1.f &&
			vert.color.z == 1.f)return true;
		return false;
	}
	//ID�ƃf�[�^���C���f�b�N�X�I�ɓ��l���ǂ����𒲂ׂ�
	bool VirtualPoint::isEquivalent(int id, IndexData data) {
		if (m_Vert_num == m_Index[id])return true;
		for (int ite2 = 0; ite2 < data.count; ite2++) {
			if (data.constant[ite2] == -1)continue;
			if (m_Index[data.constant[ite2]] == m_Index[id])return true;
		}
		return false;
	}
}