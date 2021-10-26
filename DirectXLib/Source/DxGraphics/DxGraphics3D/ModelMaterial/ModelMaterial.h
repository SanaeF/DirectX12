#pragma once
#include "../ModelData/ModelData.h"
namespace lib {
	class ModelMaterial {
	private:
		struct MaterialForHlsl {
			DirectX::XMFLOAT3 diffuse; //�f�B�t���[�Y�F
			float alpha; // �f�B�t���[�Y��
			DirectX::XMFLOAT3 specular; //�X�y�L�����F
			float specularity;//�X�y�L�����̋���(��Z�l)
			DirectX::XMFLOAT3 ambient; //�A���r�G���g�F
		};
	public:
	private:
	};
}