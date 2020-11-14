#include"DXPMDModel.h"
#include"PMDRenderer.h"
#include"Dx12Wrapper.h"
#include"DXPMDMaterial.h"
#include"DXVMDMotion.h"
#include<d3dx12.h>
#include<sstream>

#include "DXHandler.h"

#include "DxPMDManager.h"

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;
#pragma comment(lib,"winmm.lib")
namespace {
	///�e�N�X�`���̃p�X���Z�p���[�^�����ŕ�������
	///@param path �Ώۂ̃p�X������///@param splitter ��؂蕶��///@return �����O��̕�����y�A
	pair<string, string>
		SplitFileName(const std::string& path, const char splitter = '*') {
		int idx = path.find(splitter);
		pair<string, string> ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr(idx + 1, path.length() - idx - 1);
		return ret;
	}
	///�t�@�C��������g���q���擾����
	///@param path �Ώۂ̃p�X������///@return �g���q
	string
		GetExtension(const std::string& path) {
		int idx = path.rfind('.');
		return path.substr(idx + 1, path.length() - idx - 1);
	}
	///���f���̃p�X�ƃe�N�X�`���̃p�X���獇���p�X�𓾂�
	///@param modelPath �A�v���P�[�V�������猩��pmd���f���̃p�X///@param texPath PMD���f�����猩���e�N�X�`���̃p�X///@return �A�v���P�[�V�������猩���e�N�X�`���̃p�X
	std::string GetTexturePathFromModelAndTexPath(const std::string& modelPath, const char* texPath) {
		//�t�@�C���̃t�H���_��؂��\��/�̓��ނ��g�p�����\��������
		//�Ƃ�����������\��/�𓾂���΂����̂ŁA�o����rfind���Ƃ��r����
		//int�^�ɑ�����Ă���̂͌�����Ȃ������ꍇ��rfind��epos(-1��0xffffffff)��Ԃ�����
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');
		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}
}

DXPMDModel::DXPMDModel(std::shared_ptr<Dx12Wrapper> dx) :
	_dx12(dx),
	_angle(0.0f)
{

	Motion.reset(new DXVMDMotion());
	mMaterial.reset(new DXPMDMaterial(*dx));
	Motion->setTransWorld();
}

DXPMDModel::~DXPMDModel(){
}

int DXPMDModel::LoadPMDFile(const char* path,int Handle) {
	//PMD�w�b�_�\����
	struct PMDHeader {
		float version; //��F00 00 80 3F == 1.00
		char model_name[20];//���f����
		char comment[256];//���f���R�����g
	};
	char signature[3];
	PMDHeader pmdheader = {};

	string strModelPath = path;

	auto fp = fopen(strModelPath.c_str(), "rb");
	if (fp == nullptr) {
		//�G���[����
		assert(0);
		return 0;
	}
	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	unsigned int vertNum;//���_��
	fread(&vertNum, sizeof(vertNum), 1, fp);

	mMaterial->LoadMaterial(*_dx12, vertNum, strModelPath, fp);

	Motion->BoneLoad(fp);

	Motion->CreateTransformView(_dx12->Device());
	mMaterial->CreateMaterialData(_dx12->Device()); //CreateMaterialData();
	mMaterial->CreateMaterialAndTextureView(_dx12->Device()); //CreateMaterialAndTextureView();

	DXHandler handler;
	PMDManager PMDdata;
	PMDdata.vertex = mMaterial->getVB_View();
	PMDdata.index = mMaterial->getIB_View();

	return handler.CreateHandler(&PMDdata);
}

void DXPMDModel::LoadVMDFile(const char* filepath, const char* name) {
	Motion->LoadVMDFile(filepath, name);
}

void DXPMDModel::PlayAnimation() {
	Motion->PlayAnimation(timeGetTime());
}

void DXPMDModel::MotionUpdate() {
	_angle += 0.1f;
	//Motion->testUpdateInBone(cos(_angle));
	Motion->Update(timeGetTime());
}

void DXPMDModel::Update() {
	MotionUpdate();
}

void DXPMDModel::CheckData(int Handle) {
	DXHandler handler;
	//PMDManager* PMDdata = (PMDManager*)handler.getHandleData()[Handle];

	//if (&PMDdata->vertex == &mMaterial->getVB_View())assert(0);
}
void DXPMDModel::Draw(int Handle) {
	//DXHandler handler;
	//PMDManager* PMDdata = (PMDManager*)handler.getPMDData()[Handle];

	//_dx12->CommandList()->IASetVertexBuffers(0, 1,& PMDdata->vertex);
	//_dx12->CommandList()->IASetIndexBuffer(&PMDdata->index);

	_dx12->CmdList()->IASetVertexBuffers(0, 1, &mMaterial->getVB_View());
	_dx12->CmdList()->IASetIndexBuffer(&mMaterial->getIB_View());

	ID3D12DescriptorHeap* transheaps[] = { Motion->getTransHeap().Get() };
	_dx12->CmdList()->SetDescriptorHeaps(1, transheaps);
	_dx12->CmdList()->SetGraphicsRootDescriptorTable(1, Motion->getTransHeap()->GetGPUDescriptorHandleForHeapStart());



	ID3D12DescriptorHeap* mdh[] = { mMaterial->getMatHeap().Get() };
	//�}�e���A��
	_dx12->CmdList()->SetDescriptorHeaps(1, mdh);

	auto materialH = mMaterial->getMatHeap()->GetGPUDescriptorHandleForHeapStart();
	unsigned int idxOffset = 0;

	auto cbvsrvIncSize = _dx12->Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;
	for (auto& m : mMaterial->getMaterial()) {
		_dx12->CmdList()->SetGraphicsRootDescriptorTable(2, materialH);
		_dx12->CmdList()->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);
		materialH.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}

}

