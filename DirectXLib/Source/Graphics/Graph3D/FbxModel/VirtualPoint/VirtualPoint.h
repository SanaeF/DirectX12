#pragma once
#include "../../Vertex/Vertex.h"
#include <vector>
#include <wrl.h>
namespace model {
	class VirtualPoint {
	private:
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
		std::vector<lib::Vertex>m_Vertex;
		std::vector<UINT>m_Index;
		int m_Vert_num;
	public:
		VirtualPoint(int vert_num, std::vector<lib::Vertex> vertex, std::vector<UINT> index);
		std::vector<int> create();
	private:
		std::vector<int> createIndex6(IndexData all_index);
		//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ���S�Ď擾
		IndexData loadAllIndex();
		//�֘A�̂��钸�_�ԍ���S�Ď擾
		IndexData loadRelatedIndex(IndexData all_index);
		//�֘A�̃C���f�b�N�X�̒�����A��̗���擾
		std::vector<int> loadHighIndex(IndexData related_index);
		//�֘A�̃C���f�b�N�X�̒�����A���̗���擾
		std::vector<int> loadLowIndex(IndexData related_index);
		//�֘A�̃C���f�b�N�X�̒�����A������̃C���f�b�N�X��2�擾
		std::vector<int> loadWidthIndex(std::vector<int> high_index, std::vector<int> low_index, IndexData related_index);
		//���ׂ��\�������̎��_�ȊO���擾
		std::vector<int> loadCorners(IndexData related_index, std::vector<int> four_point);
		//���ׂĂ��钸�_�ɑ΂��ċ߂��̒��_��T��(loadHighIndex��������loadLowIndex�Ő��������C���f�b�N�X)
		int nearestIndexID(std::vector<int> indeces);
		//���_�J���[�������ǂ������擾
		bool isWhiteVertex(lib::Vertex vert);
		//ID�ƃf�[�^���C���f�b�N�X�I�ɓ��l���ǂ����𒲂ׂ�
		bool isEquivalent(int id, IndexData data);
	};
}