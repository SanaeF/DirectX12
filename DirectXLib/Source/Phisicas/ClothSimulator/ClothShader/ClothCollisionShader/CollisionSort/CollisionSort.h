#pragma once
#include "../../DirectXLib/Source/DxGraphics/DxGraphics3D/ModelData/ModelData.h"
#include "../../../ClothData/SpringData.h"
namespace phy {
	class CollisionSort {
	private:
		int m_Add_box_id;
		int m_Split_num;
		int m_Box;
		DirectX::XMFLOAT3 m_Position_zero;
		DirectX::XMFLOAT3 m_High_pos;//���f�����p1
		DirectX::XMFLOAT3 m_Low_pos;//���f�����p2
	public:
		CollisionSort(int vert_size, DirectX::XMFLOAT3& max, DirectX::XMFLOAT3& min);
		//��Ԃ����蓖�Ă�ID��Ԃ�
		int spaceInput(int id, DirectX::XMFLOAT3& p);
	private:
		int searchX(int id, int split_num, DirectX::XMFLOAT3& p);
		int searchY(int id, int split_num, int ite_x, DirectX::XMFLOAT3& p);
		int searchZ(int id, int split_num, int ite_x, int ite_y, DirectX::XMFLOAT3& p);
		//p1��a�`b�̊ԂɊ܂܂�Ă��邩�ǂ���
		bool between(float p1, float a, float b);
	};
}