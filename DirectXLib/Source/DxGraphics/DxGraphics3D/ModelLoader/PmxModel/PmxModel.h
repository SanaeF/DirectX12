#pragma once
#include <d3dx12.h>
#include <string>
#include <memory>
#include <vector>
#include "../../ModelData/ModelData.h"

namespace lib {
	class DirectX12Manager;
}
namespace model {
	class PmxModel {
	private:
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;
		std::vector<UINT>fixed_index;
		std::vector<UINT>same_position;
		std::vector<bool>is_same;
		std::vector<bool>is_input_same;
		std::vector<bool>is_standard;
	public:
		// PMX���f���f�[�^�̃��[�h�֐�
		bool loadFile(const std::wstring& _filePath);
	private:
		void setStandardIndex(int id, lib::ModelData& data);
		//�Œ蒸�_�̃J���[���蓖��
		void fixedIndex(lib::ModelData& data);
		//���꒸�_�̃C���f�b�N�X���܂Ƃ߂�
		void sameIndex(lib::ModelData& data);
		//�C���f�b�N�X���O���[�v������
		void groupIndex(lib::ModelData& data);
	};
}