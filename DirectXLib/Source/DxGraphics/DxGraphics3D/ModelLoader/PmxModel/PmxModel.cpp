#include "PmxModel.h"
#include <fstream>
#include <array>
#include "../../DirectXLib/Source/VectorMath/VectorMath.h"
namespace model {
	constexpr int NO_DATA_FLAG = -1;
	bool PmxModel::loadFile(const std::wstring& file_path) {
		if (file_path.empty())return false;
		// �t�@�C���I�[�v��
		std::ifstream pmx_file{ file_path, (std::ios::binary | std::ios::in) };
		if (pmx_file.fail()) {
			pmx_file.close();
			return false;
		}
		// �w�b�_�[ -------------------------------
		std::array<byte, 4> header{};
		constexpr std::array<byte, 4> PMX_MAGIC_NUMBER{ 0x50, 0x4d, 0x58, 0x20 };
		enum HeaderDataIndex{
			ENCODING_FORMAT,
			NUMBER_OF_ADD_UV,
			VERTEX_INDEX_SIZE,
			TEXTURE_INDEX_SIZE,
			MATERIAL_INDEX_SIZE,
			BONE_INDEX_SIZE,
			RIGID_BODY_INDEX_SIZE
		};

		for (int ite = 0; ite < 4; ite++){
			header[ite] = pmx_file.get();
		}
		if (header != PMX_MAGIC_NUMBER){
			pmx_file.close();
			return false;
		}

		// ver2.0�ȊO�͔�Ή�
		float version{};
		pmx_file.read(reinterpret_cast<char*>(&version), 4);
		if (!DirectX::XMScalarNearEqual(version, 2.0f, DirectX::g_XMEpsilon.f[0])){
			pmx_file.close();
			return false;
		}

		byte hederDataLength = pmx_file.get();
		if (hederDataLength != 8){
			pmx_file.close();
			return false;
		}
		std::array<byte, 8> hederData{};
		for (int ite = 0; ite < hederDataLength; ite++){
			hederData[ite] = pmx_file.get();
		}
		//UTF-8�͔�Ή�
		if (hederData[0] != 0){
			pmx_file.close();
			return false;
		}
		unsigned array_size{};
		for (int ite = 0; ite < 4; ite++){
			pmx_file.read(reinterpret_cast<char*>(&array_size), 4);
			for (unsigned j = 0; j < array_size; j++)
			{
				pmx_file.get();
			}
		}
		auto& data = lib::ModelData::Object;

		// ���_
		int vertex_size{};
		pmx_file.read(reinterpret_cast<char*>(&vertex_size), 4);
		data.vertex.resize(vertex_size);
		data.param.resize(vertex_size);
		data.index_group.resize(vertex_size);
		m_Is_same.resize(vertex_size);
		m_Is_input_same.resize(vertex_size);
		m_Is_standard.resize(vertex_size);
		for (int ite = 0; ite < vertex_size; ite++) {
			pmx_file.read(reinterpret_cast<char*>(&data.vertex[ite].position), 12);
			pmx_file.read(reinterpret_cast<char*>(&data.vertex[ite].normal), 12);
			pmx_file.read(reinterpret_cast<char*>(&data.vertex[ite].tex), 8);
			if (hederData[NUMBER_OF_ADD_UV] != 0)
			{
				for (int j = 0; j < hederData[NUMBER_OF_ADD_UV]; ++j)
				{
					//pmxFile.read(reinterpret_cast<char*>(&data.vertex[ite].addUV[j]), 16);
				}
			}
			const byte weightMethod = pmx_file.get();
			float w;
			m_Is_same[ite] = false;
			m_Is_input_same[ite] = false;
			m_Is_standard[ite] = false;
			switch (weightMethod){
			case lib::ModelParam::Weight::BDEF1:
				data.param[ite].weight.type = lib::ModelParam::Weight::BDEF1;
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born1), hederData[BONE_INDEX_SIZE]);
				data.param[ite].weight.born2 = NO_DATA_FLAG;
				data.param[ite].weight.born3 = NO_DATA_FLAG;
				data.param[ite].weight.born4 = NO_DATA_FLAG;
				data.param[ite].weight.weight1 = 1.0f;
				break;
			case lib::ModelParam::Weight::BDEF2:
				data.param[ite].weight.type = lib::ModelParam::Weight::BDEF2;
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born1), hederData[BONE_INDEX_SIZE]);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born2), hederData[BONE_INDEX_SIZE]);
				data.param[ite].weight.born3 = NO_DATA_FLAG;
				data.param[ite].weight.born4 = NO_DATA_FLAG;
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.weight1), 4);
				data.param[ite].weight.weight2 = 1.0f - data.param[ite].weight.weight1;
				if (data.param[ite].weight.born2 == 1)m_Fixed_idx.push_back(ite);
				if (data.param[ite].weight.born2 == 2) {
					m_Same_pos.push_back(ite);
					m_Is_same[ite] = true;
				}
				break;
			case lib::ModelParam::Weight::BDEF4:
				data.param[ite].weight.type = lib::ModelParam::Weight::BDEF4;
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born1), hederData[BONE_INDEX_SIZE]);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born2), hederData[BONE_INDEX_SIZE]);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born3), hederData[BONE_INDEX_SIZE]);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born4), hederData[BONE_INDEX_SIZE]);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.weight1), 4);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.weight2), 4);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.weight3), 4);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.weight4), 4);
				m_Fixed_idx.push_back(ite);
				m_Same_pos.push_back(ite);
				m_Is_same[ite] = true;
				break;
			case lib::ModelParam::Weight::SDEF:
				data.param[ite].weight.type = lib::ModelParam::Weight::SDEF;
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born1), hederData[BONE_INDEX_SIZE]);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.born2), hederData[BONE_INDEX_SIZE]);
				data.param[ite].weight.born3 = NO_DATA_FLAG;
				data.param[ite].weight.born4 = NO_DATA_FLAG;
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.weight1), 4);
				data.param[ite].weight.weight2 = 1.0f - data.param[ite].weight.weight1;
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.c), 12);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.r0), 12);
				pmx_file.read(reinterpret_cast<char*>(&data.param[ite].weight.r1), 12);
				break;
			default:
				pmx_file.close();
				return false;
			}
			pmx_file.read(reinterpret_cast<char*>(&data.param[ite].edgeMagnif), 4);
			if (data.param[ite].weight.born1 == NO_DATA_FLAG){
				pmx_file.close();
				return false;
			}
			if (m_Is_same[ite]) {
				setStandardIndex(ite, data);
			}
		}

		// ��
		int numOfSurface{};
		pmx_file.read(reinterpret_cast<char*>(&numOfSurface), 4);
		data.index.resize(numOfSurface);
		for (int ite = 0; ite < numOfSurface; ite++){
			pmx_file.read(reinterpret_cast<char*>(&data.index[ite]), hederData[VERTEX_INDEX_SIZE]);
			if (data.index[ite] == NO_DATA_FLAG){
				pmx_file.close();
				return false;
			}
		}
		// �e�N�X�`�� -----------------------------
		int tex_size{};
		pmx_file.read(reinterpret_cast<char*>(&tex_size), 4);
		data.texturePaths.resize(tex_size);

		std::wstring texturePath{};
		for (int i = 0; i < tex_size; i++)
		{
			data.texturePaths[i] = file_path;
			loadPMXStringUTF16(pmx_file, texturePath, data.texturePaths[i]);
			data.texturePaths[i] += texturePath;
		}

		// �}�e���A�� -----------------------------
		int material_size{};
		pmx_file.read(reinterpret_cast<char*>(&material_size), 4);

		data.materials.resize(material_size);
		for (int i = 0; i < material_size; i++)
		{
			for (int j = 0; j < 2; ++j)
			{
				pmx_file.read(reinterpret_cast<char*>(&array_size), 4);
				for (unsigned i = 0; i < array_size; i++)
				{
					pmx_file.get();
				}
			}

			pmx_file.read(reinterpret_cast<char*>(&data.materials[i].diffuse), 16);
			pmx_file.read(reinterpret_cast<char*>(&data.materials[i].specular), 12);
			pmx_file.read(reinterpret_cast<char*>(&data.materials[i].specularity), 4);
			pmx_file.read(reinterpret_cast<char*>(&data.materials[i].ambient), 12);

			pmx_file.get();
			for (int i = 0; i < 16; i++)
			{
				pmx_file.get();
			}
			for (int i = 0; i < 4; i++)
			{
				pmx_file.get();
			}

			pmx_file.read(reinterpret_cast<char*>(&data.materials[i].colorMapTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
			for (unsigned char i = 0; i < hederData[TEXTURE_INDEX_SIZE]; i++)
			{
				pmx_file.get();
			}
			pmx_file.get();

			const byte shareToonFlag = pmx_file.get();
			if (shareToonFlag)
			{
				pmx_file.get();
			}
			else
			{
				pmx_file.read(reinterpret_cast<char*>(&data.materials[i].toonTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
			}

			pmx_file.read(reinterpret_cast<char*>(&array_size), 4);
			for (unsigned i = 0; i < array_size; i++)
			{
				pmx_file.get();
			}

			pmx_file.read(reinterpret_cast<char*>(&data.materials[i].vertexNum), 4);
		}

		//�Œ蒸�_
		fixedIndex(data);
		//���꒸�_�܂Ƃ�
		sameIndex(data);
		//�C���f�b�N�X���O���[�v������
		groupIndex(data);

		lib::ModelData::Object.pre_vert = lib::ModelData::Object.vertex;
	}
	void PmxModel::setStandardIndex(int id, lib::ModelData& data) {
		for (int ite2 = 0; ite2 < m_Same_pos.size(); ite2++) {
			auto id2 = m_Same_pos[ite2];
			if (id == id2)continue;//���g�Ɣ�r���Ă��邩�ǂ���
			if (m_Is_standard[id2])continue;//�܂��i�[����Ă��Ȃ����_����ǂ���
			auto dist = lib::VectorMath::distance(
				data.vertex[id].position,
				data.vertex[id2].position
			);
			if (dist == 0)m_Is_standard[id] = true;//���꒸�_�̊�ł���
		}
	}
	//�Œ蒸�_�̃J���[���蓖��
	void PmxModel::fixedIndex(lib::ModelData& data) {
		for (int ite = 0; ite < m_Fixed_idx.size(); ite++) {
			auto id = m_Fixed_idx[ite];
			data.vertex[id].color.x = 1.0;
			data.vertex[id].color.y = 0.0;
			data.vertex[id].color.z = 0.0;
		}
	}
	//���꒸�_�̃C���f�b�N�X���܂Ƃ߂�
	void PmxModel::sameIndex(lib::ModelData& data) {
		for (int ite = 0; ite < data.index.size(); ite++) {
			if (data.index[ite] == 7288) {
				int a = 0;
			}
			UINT id1 = data.index[ite];
			if (!m_Is_same[id1])continue;
			//���_������̃C���f�b�N�X�ԍ����擾
			for (int ite2 = 0; ite2 < m_Same_pos.size(); ite2++) {
				auto id2 = m_Same_pos[ite2];
				if (id1 <= id2)continue;
				if (m_Is_standard[id2])continue;
				auto dist = lib::VectorMath::distance(
					data.vertex[id1].position,
					data.vertex[id2].position
				);
				//�C���f�b�N�X�ɓ��꒸�_��ID���i�[
				if (dist == 0) {
					data.vertex[id1].color.x = 1.0f;
					data.vertex[id1].color.y = 0.0f;
					data.vertex[id1].color.z = 0.0f;
					data.index[ite] = id2;
				}
			}
		}
	}
	//�C���f�b�N�X���O���[�v������
	void PmxModel::groupIndex(lib::ModelData& data) {
		for (int ite = 0; ite < data.index.size(); ite++) {
			data.index_group[data.index[ite]].push_back(ite);
		}
	}
	inline bool PmxModel::loadPMXStringUTF16(std::ifstream& _file, std::wstring& output, std::wstring& path){
		//�t�@�C���p�X�Łu/�v���o�Ă���܂Ō�납��폜���Ă���
		int delete_count=0;
		for (int ite = 0; ite < path.size(); ite++) {
			if (path[(path.size() - 1) - ite] == '/')break;
			delete_count++;
		}
		path.resize(path.size() - delete_count);

		std::array<wchar_t, 512> wBuffer{};
		int textSize;

		_file.read(reinterpret_cast<char*>(&textSize), 4);

		_file.read(reinterpret_cast<char*>(&wBuffer), textSize);
		auto tex_path = std::wstring(&wBuffer[0], &wBuffer[0] + textSize / 2);
		output.resize(tex_path.size());
		//�u\\�v�́u/�v�ɕϊ�����
		for (int ite = 0; ite < tex_path.size(); ite++) {
			if (tex_path[ite] == '\\')output[ite] = '/';
			else output[ite] = tex_path[ite];
		}
		return true;
	}
	////���꒸�_���擾
	//int PmxModel::sameIndex(
	//	int num,
	//	std::vector<bool>same_position,
	//	lib::ModelData data
	//) {
	//	for (int ite = 0; ite < same_position.size(); ite++) {
	//		if (!data.same_position[ite])continue;
	//		if (ite == num)continue;
	//		auto dist = lib::VectorMath::distance(
	//			data.vertex[ite].position, 
	//			data.vertex[num].position
	//		);
	//		if (dist == 0)return ite;
	//	}
	//	return -1;
	//}
}