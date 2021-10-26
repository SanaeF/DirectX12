#pragma once
#include <vector>
#include <wrl.h>
#include "../../../DxGraphics/DxGraphics3D/ModelData/ModelData.h"
namespace phy {
	enum MODEL_FILE {
		FBX,PMX
	};
	//pmx���f���݂̂ɑΉ��������_�̐���(���f���`���ɂ���āA�C���f�b�N�X�̊֌W���ς���Ă��Ă��܂��B)
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
		int m_Mid_point[4];//���S����\��������4�_
		int m_Mid_points[4][4];
		int m_Cor_point[4];//3x3�̏\�������ȊO4�_
		int m_Cor_points[4][4];//3x3�̏\�������ȊO4�_
		int m_More_point[4];//3x3����ɊO�̏\������4�_
		int m_Polygon_point[6];//3x3����ɊO�̏\������4�_
		int m_Polygon_points[4][6];
		float m_Semi_high_p4;//�\�������Ƃ̓_�̍ŏ�����(�܂肽���܂ꂽ�|���S����3x3���珜�O���邽��)
		float m_Semi_high_p4s[4];
		bool m_Is_bad_num;
		std::vector<int> m_CheckIGP;
		std::vector<std::vector<int>>m_Index_group;
		std::vector<lib::ModelVertex>m_Vertex;
		std::vector<UINT>m_Index;
		std::vector<int> m_Result;
		MODEL_FILE m_File_type;
		EDGE_TYPE m_Edge_type;
		EDGE_TYPE m_Edge_types[4];
	public:
		MassSpringModel(
			MODEL_FILE model_type,
			std::vector<lib::ModelVertex>& vertex,
			std::vector<UINT>& index,
			std::vector<std::vector<int>>& index_group
		);
		std::vector<int> create(int num);
		~MassSpringModel();
	private:
		//3x3�s��̎��_���f���𐶐�
		void createMatrix3x3(int vertex_id);

		//�\�����_����
		void createCross(int vertex_id, int* data);

		//3x3�ŏ\�������ȊO��4�_���擾
		void createCorner(int vertex_id, int mid_points[4][4], int* data);

		//3x3���炳��ɊO�̎��_���擾
		void createMoreP(int vertex_id);

		//���_�̎��(�p�A�ӁA���S)�𔻒肷��
		EDGE_TYPE pointType(IndexData& related_index);

		//���ׂĂ��钸�_�ԍ��ƈ�v����C���f�b�N�X�ԍ���S�Ď擾
		IndexData loadAllIndex(int vertex_id);

		//�֘A�̂��钸�_�ԍ���S�Ď擾
		IndexData loadRelatedIndex(int vertex_id, IndexData& all_index);


		IndexData sortIndex(int vertex_id, int sort_max, IndexData& data);
		IndexData sortIndexUsingTextureMap(int vertex_id, int sort_max, IndexData& data);
		bool isThrowVertex(lib::ModelVertex& vert);
		IndexData changeMatrixData(int num, int* data);
		IndexData deleteNullIndex(IndexData& data);
		//�d�Ȃ������_���폜
		IndexData deleteOverIndex(IndexData& data);
		std::vector<int> checkIndexID(IndexData& data);
	};
}