#include "ClothNewPosShader.h"
#include "../../DirectXLib/Source/DirectX12Manager/DirectX12Manager.h"
#include "../../DirectXLib/Source/ShaderManager/ComputeShader/ComputeShader.h"

namespace phy {
	std::vector<ClothNewPosShader::CNPSInfo> ClothNewPosShader::shaderHandler;

	ClothNewPosShader::ClothNewPosShader(int model_id, std::shared_ptr<lib::DirectX12Manager>& dx_12):
		m_Dx12 (dx_12),
		m_Model_id(model_id)
	{
		if (shaderHandler.size() <= m_Model_id)shaderHandler.resize(m_Model_id + 1);
	}
	void ClothNewPosShader::execution(
		ClothForce& world_f,
		std::vector<lib::ModelVertex>& vert,
		std::vector<SpringData>& spring
	) {
		if (65535 < vert.size()) {
			assert(0 && "<���̃��f���͏���𒴂��Ă��܂�!>DirectX12�̎g�p�ɂ��A�N���X�V�~�����[�^�[�Ɏg�p�ł��钸�_����65535�����E�ł��B");
		}
		auto& is_input = shaderHandler[m_Model_id].is_created;
		if (!is_input) {
			shaderHandler[m_Model_id].sim_param.resize(1);
			shaderHandler[m_Model_id].frame.resize(1);
			shaderHandler[m_Model_id].sim_param[0].vert_max = vert.size();

			shaderHandler[m_Model_id].sim_param[0].tension = world_f.tension;
			shaderHandler[m_Model_id].sim_param[0].compress = world_f.compress;
			shaderHandler[m_Model_id].sim_param[0].share = world_f.share;
			shaderHandler[m_Model_id].sim_param[0].bend = world_f.bend;

			shaderHandler[m_Model_id].sim_param[0].dt = world_f.dt;
			shaderHandler[m_Model_id].sim_param[0].k = world_f.k;
			int size = sqrt(vert.size());
			int size_out = vert.size() - (size * size);
			auto& thread = shaderHandler[m_Model_id].thread;
			thread.x = size + size_out;
			thread.y = size;
			thread.z = 1;
			m_Shader.reset(
				new lib::ComputeShader(
					"./DirectXLib/Shader/Shader3D/Cloth/ClothNewPosition.hlsl",
					"ClothNewPosition", 6, shaderHandler[m_Model_id].compute_handle, m_Dx12)
			);
			//�擾�p�f�[�^
			m_Shader->inputBufferSize(0, vert.size(), sizeof(lib::ModelVertex));//�V�~�����[�g���ĕς�������f�����
			m_Shader->inputBufferSize(1, spring.size(), sizeof(SpringData));//�V�~�����[�g���ĕς�����o�l���
			m_Shader->inputBufferSize(2, shaderHandler[m_Model_id].frame.size(), sizeof(PositionFrame));//�V�~�����[�g���ĕς�����o�l���
			//�]���p�̃��f���f�[�^
			m_Shader->inputBufferSize(3, shaderHandler[m_Model_id].sim_param.size(), sizeof(SimulateParam));//�V�~�����[�g���
			m_Shader->inputBufferSize(4, vert.size(), sizeof(lib::ModelVertex));//�`��Ɏg�����f�����
			m_Shader->inputBufferSize(5, spring.size(), sizeof(SpringData));//�o�l���f�����
			m_Shader->createUnorderdAccessView();

			m_Shader->mapOutput(0);
			m_Shader->mapOutput(1);
			m_Shader->mapOutput(2);
		}
		else m_Shader.reset(new lib::ComputeShader(shaderHandler[m_Model_id].compute_handle, m_Dx12));
		if (!is_input)m_Shader->mapInput(3, shaderHandler[m_Model_id].sim_param.data());
		m_Shader->mapInput(4, vert.data());
		m_Shader->mapInput(5, spring.data());
		is_input = true;
		m_Shader->execution(
			shaderHandler[m_Model_id].thread.x,
			shaderHandler[m_Model_id].thread.y,
			shaderHandler[m_Model_id].thread.z,
			shaderHandler[m_Model_id].compute_handle
		);
		dataAssign(vert, spring);
	}
	void ClothNewPosShader::dataAssign(std::vector<lib::ModelVertex>& vert, std::vector<SpringData>& spring) {
		vert.assign(
			(lib::ModelVertex*)m_Shader->getData(0),
			(lib::ModelVertex*)m_Shader->getData(0) + vert.size());
		spring.assign(
			(SpringData*)m_Shader->getData(1),
			(SpringData*)m_Shader->getData(1) + spring.size());
		shaderHandler[m_Model_id].frame.assign(
			(PositionFrame*)m_Shader->getData(2),
			(PositionFrame*)m_Shader->getData(2) + shaderHandler[m_Model_id].frame.size());
	}
	ClothNewPosShader::PositionFrame ClothNewPosShader::getFrame(int model_id) {
		return shaderHandler[model_id].frame[0];
	}
	ClothNewPosShader::~ClothNewPosShader() {

	}
}