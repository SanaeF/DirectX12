#include "ClothCollisionShader.h"
#include "../../DirectXLib/Source/DirectX12Manager/DirectX12Manager.h"
#include "../../DirectXLib/Source/ShaderManager/ComputeShader/ComputeShader.h"
#include "../../DirectXLib/Source/VectorMath/VectorMath.h"
namespace phy {
	std::vector<ClothCollisionShader::CSSInfo> ClothCollisionShader::shaderHandler;

	ClothCollisionShader::ClothCollisionShader(
		int id,
		std::shared_ptr<lib::DirectX12Manager>& dx12):
		m_Model_id(id),
		m_Dx12(dx12)
	{
		if (shaderHandler.size() <= m_Model_id)shaderHandler.resize(m_Model_id + 1);
		if (!shaderHandler[m_Model_id].is_created) {
			shaderHandler[m_Model_id].send_param.resize(1);
			shaderHandler[m_Model_id].collision_param.resize(1);
		}

	}
	void ClothCollisionShader::create(DirectX::XMFLOAT3 max, DirectX::XMFLOAT3 min, lib::ModelData& model, std::vector<MassModel>& mass_model) {
		if (65535 < model.vertex.size()) {
			assert(0 && "<���̃��f���͏���𒴂��Ă��܂�!>DirectX12�̎g�p�ɂ��A�N���X�V�~�����[�^�[�Ɏg�p�ł��钸�_����65535�����E�ł��B");
		}
		shaderHandler[m_Model_id].collision_param[0].vertex_size = model.vertex.size();
		int size = sqrt(model.vertex.size());
		int size_out = model.vertex.size() - (size * size);
		auto& thread = shaderHandler[m_Model_id].thread;
		thread.x = size + size_out;
		thread.y = size;
		auto& is_input = shaderHandler[m_Model_id].is_created;
		//�\�[�g�V�F�[�_�[
		executeSortShader(is_input, model, max, min);
		//�����蔻��V�F�[�_�[
		if (!is_input) {
			m_Shader.reset(
				new lib::ComputeShader(
					"./DirectXLib/Shader/Shader3D/Cloth/ClothCollider.hlsl",
					"ClothCollider", 7, shaderHandler[m_Model_id].compute_handle, m_Dx12)
			);
			//�擾�p�f�[�^
			m_Shader->inputBufferSize(0, model.vertex.size(), sizeof(lib::ModelVertex));//���f�����
			//�]���p�̃f�[�^
			m_Shader->inputBufferSize(1, shaderHandler[m_Model_id].collision_param.size(), sizeof(CollisionParam));
			m_Shader->inputBufferSize(2, model.vertex.size(), sizeof(lib::ModelVertex));//���f�����
			m_Shader->inputBufferSize(3, model.vertex.size(), sizeof(lib::ModelVertex));//���f�����
			m_Shader->inputBufferSize(4, mass_model.size(), sizeof(MassModel));//��ԏ��
			m_Shader->inputBufferSize(5, space.size(), sizeof(SpaceData));//��ԏ��
			m_Shader->inputBufferSize(6, split_area.size(), sizeof(int));//���_���Ƃ̋��ID
			m_Shader->createUnorderdAccessView();

			m_Shader->mapOutput(0);
			m_Shader->mapInput(1, shaderHandler[m_Model_id].collision_param.data());
			m_Shader->mapInput(2, model.pre_vert.data());
			m_Shader->mapInput(4, mass_model.data());
		}
		else m_Shader.reset(new lib::ComputeShader(shaderHandler[m_Model_id].compute_handle, m_Dx12));
		m_Shader->mapInput(3, model.vertex.data());
		m_Shader->mapInput(5, space.data());
		m_Shader->mapInput(6, split_area.data());
		is_input = true;
	}
	void ClothCollisionShader::executeSortShader(bool is_input, lib::ModelData& model, DirectX::XMFLOAT3 max_pos, DirectX::XMFLOAT3 min_pos) {
		//����
		std::vector<SendParam> send_param(1);
		send_param[0].vertex_size = model.vertex.size();
		send_param[0].split_num = HIT_SPLIT::SINGLE;
		send_param[0].max_pos = max_pos;
		send_param[0].min_pos = min_pos;
		send_param[0].zero_pos= lib::VectorMath::slplitPoint(max_pos, min_pos);
		space.resize(XYZ_ALL);
		split_area.resize(model.vertex.size());
		if (!is_input) {
			m_Shader.reset(
				new lib::ComputeShader(
					"./DirectXLib/Shader/Shader3D/Cloth/CollisionSpace.hlsl",
					"CollisionSpace", 4, shaderHandler[m_Model_id].sort_handle, m_Dx12)
			);
			//�擾�p�f�[�^
			m_Shader->inputBufferSize(0, space.size(), sizeof(SpaceData));//��ԏ��
			m_Shader->inputBufferSize(1, split_area.size(), sizeof(int));//���_���Ƃ̋��ID
			//�]���p�̃f�[�^
			m_Shader->inputBufferSize(2, send_param.size(), sizeof(SendParam));//��ԃT�C�Y
			m_Shader->inputBufferSize(3, model.vertex.size(), sizeof(lib::ModelVertex));//���f�����
			m_Shader->createUnorderdAccessView();

			m_Shader->mapOutput(0);
			m_Shader->mapOutput(1);
		}
		else m_Shader.reset(new lib::ComputeShader(shaderHandler[m_Model_id].sort_handle, m_Dx12));
		m_Shader->mapInput(2, send_param.data());
		m_Shader->mapInput(3, model.vertex.data());
		//���s
		m_Shader->execution(
			shaderHandler[m_Model_id].thread.x,
			shaderHandler[m_Model_id].thread.y,
			1, 
			shaderHandler[m_Model_id].sort_handle
		);
		//�o��
		space.assign(
			(SpaceData*)m_Shader->getData(0),
			(SpaceData*)m_Shader->getData(0) + space.size());
		split_area.assign(
			(int*)m_Shader->getData(1),
			(int*)m_Shader->getData(1) + split_area.size());
		for (int ite = 0; ite < split_area.size(); ite++) {
			if (split_area[ite] < 0) {
				int a = 0;
			}
		}
	}
	void ClothCollisionShader::execution(lib::ModelData& model) {
		m_Shader->execution(
			shaderHandler[m_Model_id].thread.x,
			shaderHandler[m_Model_id].thread.y,
			1,
			shaderHandler[m_Model_id].compute_handle
		);
		dataAssign(model);
	}
	void ClothCollisionShader::dataAssign(lib::ModelData& model) {
		model.vertex.assign(
			(lib::ModelVertex*)m_Shader->getData(0),
			(lib::ModelVertex*)m_Shader->getData(0) + model.vertex.size());
	}
}