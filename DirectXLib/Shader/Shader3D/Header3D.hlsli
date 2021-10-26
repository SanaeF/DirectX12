struct Output {
	float4 svpos:SV_POSITION;
	float4 pos:POSITION;//�V�X�e���p���_���W
	float4 normal:NORMAL;
	float4 vnormal:NORMAL1;//�@���x�N�g��
	float2 uv:TEXCOORD;//UV�l
	float3 ray:VECTOR;//�x�N�g��
};

Texture2D<float4> tex:register(t0);
Texture2D<float4> sph:register(t1);//1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(��Z)
Texture2D<float4> spa:register(t2);//2�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(���Z)
Texture2D<float4> toon:register(t3);//3�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(�g�D�[��)

SamplerState smp : register(s0);

//�萔�o�b�t�@
cbuffer SceneMat : register(b0) {
	matrix world;//���[���h�ϊ��s��
	matrix view;
	matrix proj;//�r���[�v���W�F�N�V�����s��
	float3 eye;
};

cbuffer Material : register(b1) {
	float4 diffuse;//�f�B�t���[�Y�F
	float4 specular;//�X�y�L����
	float3 ambient;//�A���r�G���g
};