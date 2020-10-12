#include "DXPMDMaterial.h"
#include<d3dx12.h>
#include"Dx12Wrapper.h"
#include"PMDRenderer.h"

namespace {
	///�e�N�X�`���̃p�X���Z�p���[�^�����ŕ�������
	///@param path �Ώۂ̃p�X������///@param splitter ��؂蕶��///@return �����O��̕�����y�A
	std::pair<std::string, std::string>
		SplitFileName(const std::string& path, const char splitter = '*') {
		int idx = path.find(splitter);
		std::pair<std::string, std::string> ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr(idx + 1, path.length() - idx - 1);
		return ret;
	}
	///�t�@�C��������g���q���擾����
	///@param path �Ώۂ̃p�X������///@return �g���q
	std::string
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

DXPMDMaterial::DXPMDMaterial(Dx12Wrapper& DX12) :
	mDX(DX12)
{

}

void DXPMDMaterial::LoadMaterial(Dx12Wrapper& DX12, unsigned int vertNum, std::string strModelPath, FILE* fp) {
#pragma pack(1)//��������1�o�C�g�p�b�L���O�c�A���C�����g�͔������Ȃ�
	//PMD�}�e���A���\����
	struct PMDMaterial {
		DirectX::XMFLOAT3 diffuse; //�f�B�t���[�Y�F
		float alpha; // �f�B�t���[�Y��
		float specularity;//�X�y�L�����̋���(��Z�l)
		DirectX::XMFLOAT3 specular; //�X�y�L�����F
		DirectX::XMFLOAT3 ambient; //�A���r�G���g�F
		unsigned char toonIdx; //�g�D�[���ԍ�(��q)
		unsigned char edgeFlg;//�}�e���A�����̗֊s���t���O
		//2�o�C�g�̃p�f�B���O�������I�I
		unsigned int indicesNum; //���̃}�e���A�������蓖����C���f�b�N�X��
		char texFilePath[20]; //�e�N�X�`���t�@�C����(�v���X�A���t�@�c��q)
	};//70�o�C�g�̂͂��c�ł��p�f�B���O���������邽��72�o�C�g
#pragma pack()//1�o�C�g�p�b�L���O����

	constexpr unsigned int pmdvertex_size = 38;//���_1������̃T�C�Y
	std::vector<unsigned char> vertices(vertNum * pmdvertex_size);//�o�b�t�@�m��
	fread(vertices.data(), vertices.size(), 1, fp);//��C�ɓǂݍ���

	unsigned int indicesNum;//�C���f�b�N�X��
	fread(&indicesNum, sizeof(indicesNum), 1, fp);//
	
	//UPLOAD(�m�ۂ͉\)
	auto result = DX12.Device()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertices.size()),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_vb.ReleaseAndGetAddressOf()));

	unsigned char* vertMap = nullptr;
	result = _vb->Map(0, nullptr, (void**)&vertMap);
	std::copy(vertices.begin(), vertices.end(), vertMap);
	_vb->Unmap(0, nullptr);


	_vbView.BufferLocation = _vb->GetGPUVirtualAddress();//�o�b�t�@�̉��z�A�h���X
	_vbView.SizeInBytes = vertices.size();//�S�o�C�g��
	_vbView.StrideInBytes = pmdvertex_size;//1���_������̃o�C�g��

	std::vector<unsigned short> indices(indicesNum);
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);//��C�ɓǂݍ���


	//�ݒ�́A�o�b�t�@�̃T�C�Y�ȊO���_�o�b�t�@�̐ݒ���g���܂킵��
	//OK���Ǝv���܂��B
	result = DX12.Device()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0])),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_ib.ReleaseAndGetAddressOf()));

	//������o�b�t�@�ɃC���f�b�N�X�f�[�^���R�s�[
	unsigned short* mappedIdx = nullptr;
	_ib->Map(0, nullptr, (void**)&mappedIdx);
	std::copy(indices.begin(), indices.end(), mappedIdx);
	_ib->Unmap(0, nullptr);


	//�C���f�b�N�X�o�b�t�@�r���[���쐬
	_ibView.BufferLocation = _ib->GetGPUVirtualAddress();
	_ibView.Format = DXGI_FORMAT_R16_UINT;
	_ibView.SizeInBytes = indices.size() * sizeof(indices[0]);

	unsigned int materialNum;
	fread(&materialNum, sizeof(materialNum), 1, fp);
	_materials.resize(materialNum);
	_textureResources.resize(materialNum);
	_sphResources.resize(materialNum);
	_spaResources.resize(materialNum);
	_toonResources.resize(materialNum);

	std::vector<PMDMaterial> pmdMaterials(materialNum);
	fread(pmdMaterials.data(), pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);
	//�R�s�[
	for (int i = 0; i < pmdMaterials.size(); ++i) {
		_materials[i].indicesNum = pmdMaterials[i].indicesNum;
		_materials[i].material.diffuse = pmdMaterials[i].diffuse;
		_materials[i].material.alpha = pmdMaterials[i].alpha;
		_materials[i].material.specular = pmdMaterials[i].specular;
		_materials[i].material.specularity = pmdMaterials[i].specularity;
		_materials[i].material.ambient = pmdMaterials[i].ambient;
		_materials[i].additional.toonIdx = pmdMaterials[i].toonIdx;
	}

	for (int i = 0; i < pmdMaterials.size(); ++i) {
		//�g�D�[�����\�[�X�̓ǂݍ���
		char toonFilePath[32];
		sprintf(toonFilePath, "toon/toon%02d.bmp", pmdMaterials[i].toonIdx + 1);
		_toonResources[i] = DX12.GetTextureByPath(toonFilePath);

		if (strlen(pmdMaterials[i].texFilePath) == 0) {
			_textureResources[i] = nullptr;
			continue;
		}

		std::string texFileName = pmdMaterials[i].texFilePath;
		std::string sphFileName = "";
		std::string spaFileName = "";
		if (count(texFileName.begin(), texFileName.end(), '*') > 0) {//�X�v���b�^������
			auto namepair = SplitFileName(texFileName);
			if (GetExtension(namepair.first) == "sph") {
				texFileName = namepair.second;
				sphFileName = namepair.first;
			}
			else if (GetExtension(namepair.first) == "spa") {
				texFileName = namepair.second;
				spaFileName = namepair.first;
			}
			else {
				texFileName = namepair.first;
				if (GetExtension(namepair.second) == "sph") {
					sphFileName = namepair.second;
				}
				else if (GetExtension(namepair.second) == "spa") {
					spaFileName = namepair.second;
				}
			}
		}
		else {
			if (GetExtension(pmdMaterials[i].texFilePath) == "sph") {
				sphFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
			else if (GetExtension(pmdMaterials[i].texFilePath) == "spa") {
				spaFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
			else {
				texFileName = pmdMaterials[i].texFilePath;
			}
		}
		//���f���ƃe�N�X�`���p�X����A�v���P�[�V��������̃e�N�X�`���p�X�𓾂�
		if (texFileName != "") {
			auto texFilePath = GetTexturePathFromModelAndTexPath(strModelPath, texFileName.c_str());
			_textureResources[i] = DX12.GetTextureByPath(texFilePath.c_str());
		}
		if (sphFileName != "") {
			auto sphFilePath = GetTexturePathFromModelAndTexPath(strModelPath, sphFileName.c_str());
			_sphResources[i] = DX12.GetTextureByPath(sphFilePath.c_str());
		}
		if (spaFileName != "") {
			auto spaFilePath = GetTexturePathFromModelAndTexPath(strModelPath, spaFileName.c_str());
			_spaResources[i] = DX12.GetTextureByPath(spaFilePath.c_str());
		}
	}
}

HRESULT DXPMDMaterial::CreateMaterialData(ComPtr< ID3D12Device> _dev) {

	//�}�e���A���o�b�t�@���쐬
	auto materialBuffSize = sizeof(MaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	auto result = _dev->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(materialBuffSize * _materials.size()),//�ܑ̂Ȃ����ǎd���Ȃ��ł���
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_materialBuff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}

	//�}�b�v�}�e���A���ɃR�s�[
	char* mapMaterial = nullptr;
	result = _materialBuff->Map(0, nullptr, (void**)&mapMaterial);
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}
	for (auto& m : _materials) {
		*((MaterialForHlsl*)mapMaterial) = m.material;//�f�[�^�R�s�[
		mapMaterial += materialBuffSize;//���̃A���C�����g�ʒu�܂Ői�߂�
	}
	_materialBuff->Unmap(0, nullptr);

	return S_OK;

}

HRESULT DXPMDMaterial::CreateMaterialAndTextureView(ComPtr< ID3D12Device> _dev) {
	_whiteTex = CreateWhiteTexture(_dev);
	_blackTex = CreateBlackTexture(_dev);
	_gradTex = CreateGrayGradationTexture(_dev);
	D3D12_DESCRIPTOR_HEAP_DESC materialDescHeapDesc = {};
	materialDescHeapDesc.NumDescriptors = _materials.size() * 5;//�}�e���A�����Ԃ�(�萔1�A�e�N�X�`��3��)
	materialDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	materialDescHeapDesc.NodeMask = 0;

	materialDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�f�X�N���v�^�q�[�v���
	auto result = _dev->CreateDescriptorHeap(&materialDescHeapDesc, IID_PPV_ARGS(_materialHeap.ReleaseAndGetAddressOf()));//����
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return result;
	}
	auto materialBuffSize = sizeof(MaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
	matCBVDesc.BufferLocation = _materialBuff->GetGPUVirtualAddress();
	matCBVDesc.SizeInBytes = materialBuffSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;//��q
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;//�~�b�v�}�b�v�͎g�p���Ȃ��̂�1
	CD3DX12_CPU_DESCRIPTOR_HANDLE matDescHeapH(_materialHeap->GetCPUDescriptorHandleForHeapStart());
	auto incSize = _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (int i = 0; i < _materials.size(); ++i) {
		//�}�e���A���Œ�o�b�t�@�r���[
		_dev->CreateConstantBufferView(&matCBVDesc, matDescHeapH);
		matDescHeapH.ptr += incSize;
		matCBVDesc.BufferLocation += materialBuffSize;
		if (_textureResources[i] == nullptr) {
			srvDesc.Format = _whiteTex->GetDesc().Format;
			_dev->CreateShaderResourceView(_whiteTex.Get(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = _textureResources[i]->GetDesc().Format;
			_dev->CreateShaderResourceView(_textureResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.Offset(incSize);

		if (_sphResources[i] == nullptr) {
			srvDesc.Format = _whiteTex->GetDesc().Format;
			_dev->CreateShaderResourceView(_whiteTex.Get(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = _sphResources[i]->GetDesc().Format;
			_dev->CreateShaderResourceView(_sphResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;

		if (_spaResources[i] == nullptr) {
			srvDesc.Format = _blackTex->GetDesc().Format;
			_dev->CreateShaderResourceView(_blackTex.Get(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = _spaResources[i]->GetDesc().Format;
			_dev->CreateShaderResourceView(_spaResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;


		if (_toonResources[i] == nullptr) {
			srvDesc.Format = _gradTex->GetDesc().Format;
			_dev->CreateShaderResourceView(_gradTex.Get(), &srvDesc, matDescHeapH);
		}
		else {
			srvDesc.Format = _toonResources[i]->GetDesc().Format;
			_dev->CreateShaderResourceView(_toonResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;
	}
}


ID3D12Resource* DXPMDMaterial::CreateDefaultTexture(ComPtr< ID3D12Device> _dev,size_t width, size_t height) {
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	ID3D12Resource* buff = nullptr;
	auto result = _dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	if (FAILED(result)) {
		assert(SUCCEEDED(result));
		return nullptr;
	}
	return buff;
}
ID3D12Resource* DXPMDMaterial::CreateWhiteTexture(ComPtr< ID3D12Device> _dev) {

	ID3D12Resource* whiteBuff = CreateDefaultTexture(_dev,4, 4);

	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0xff);

	auto result = whiteBuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	assert(SUCCEEDED(result));
	return whiteBuff;
}
ID3D12Resource* DXPMDMaterial::CreateBlackTexture(ComPtr< ID3D12Device> _dev) {

	ID3D12Resource* blackBuff = CreateDefaultTexture(_dev, 4, 4);
	std::vector<unsigned char> data(4 * 4 * 4);
	std::fill(data.begin(), data.end(), 0x00);

	auto result = blackBuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	assert(SUCCEEDED(result));
	return blackBuff;
}
ID3D12Resource* DXPMDMaterial::CreateGrayGradationTexture(ComPtr< ID3D12Device> _dev) {
	ID3D12Resource* gradBuff = CreateDefaultTexture(_dev, 4, 256);
	//�オ�����ĉ��������e�N�X�`���f�[�^���쐬
	std::vector<unsigned int> data(4 * 256);
	auto it = data.begin();
	unsigned int c = 0xff;
	for (; it != data.end(); it += 4) {
		auto col = (c << 0xff) | (c << 16) | (c << 8) | c;
		std::fill(it, it + 4, col);
		--c;
	}

	auto result = gradBuff->WriteToSubresource(0, nullptr, data.data(), 4 * sizeof(unsigned int), sizeof(unsigned int) * data.size());
	assert(SUCCEEDED(result));
	return gradBuff;
}

D3D12_VERTEX_BUFFER_VIEW DXPMDMaterial::getVB_View() {
	return _vbView;
}
D3D12_INDEX_BUFFER_VIEW DXPMDMaterial::getIB_View() {
	return _ibView;
}
