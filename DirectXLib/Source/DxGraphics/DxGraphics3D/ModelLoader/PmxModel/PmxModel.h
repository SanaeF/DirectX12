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
		std::vector<UINT>m_Fixed_idx;
		std::vector<UINT>m_Same_pos;
		std::vector<bool>m_Is_same;
		std::vector<bool>m_Is_input_same;
		std::vector<bool>m_Is_standard;
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
		//������̓ǂݍ���
		bool loadPMXStringUTF16(std::ifstream& _file, std::wstring& output, std::wstring& path);
	};
}