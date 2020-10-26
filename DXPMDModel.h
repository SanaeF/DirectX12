#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<unordered_map>
#include<wrl.h>
#include<memory>


class Dx12Wrapper;
class PMDRenderer;
class DXPMDMaterial;

class DXVMDMotion;


class DXPMDModel{
private:

	std::shared_ptr<Dx12Wrapper> _dx12;
	std::shared_ptr <DXPMDMaterial> mMaterial;
	std::shared_ptr<DXVMDMotion>Motion;

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;


	float _angle;//�e�X�g�pY����]

public:

	DXPMDModel(std::shared_ptr<Dx12Wrapper> dx);
	~DXPMDModel();

	///�N���[���͒��_����у}�e���A���͋��ʂ̃o�b�t�@������悤�ɂ���
	DXPMDModel* Clone();

	void LoadVMDFile(const char* filepath, const char* name);
	void PlayAnimation();
	void Update();
	void Draw(int Handle);

	//PMD�t�@�C���̃��[�h
	int LoadPMDFile(const char* path, int Handle=0);

	void CheckData(int Handle);
private:

	void MotionUpdate();


	friend PMDRenderer;

};