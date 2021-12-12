#pragma once
#include <vector>
#include <wrl.h>
#include "../ClothData/SpringData.h"
#include "../../../DxGraphics/DxGraphics3D/ModelData/ModelData.h"
namespace phy {
	class VertexPolygonModel {
	private:
		std::vector<PolygonModel>m_Polygon_model;
	public:
		VertexPolygonModel(lib::ModelData& model);
		//���_���Ƃɐ���������������O�p�`�̒��_�f�[�^���擾����
		std::vector<PolygonModel> getData();
	private:
		//id�Ԗڂ̃|���S�����f���ɕ����́u�O�p�`�̒��_ID�v�ƎO�p�`�̐����擾����
		void createPolygonID(int id, std::vector<UINT>& index, std::vector<std::vector<int>>& index_group);
		//�O�p�`�̒��_���C���f�b�N�X�ԍ�int[3]�ŕԂ�
		std::vector<int> createIndex(int vert_id, int group_id, std::vector<std::vector<int>>& index_group);
		//index�ԍ��𒸓_ID�ɕϊ�
		int getVertex(int id, std::vector<UINT>& index);
	};
}