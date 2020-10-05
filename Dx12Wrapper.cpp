#include <iostream>
#include "Dx12Wrapper.h"
#include<cassert>
#include<d3dx12.h>
#include "DX_Resource.h"
#include"Application.h"

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;

Dx12Wrapper::Dx12Wrapper(HWND hwnd) : _hwnd(hwnd) {

}

bool Dx12Wrapper::Init() {
	DXRender.reset(new DX_Resource);
	//DirectX12�֘A������
	ID3D12Debug* debug;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	debug->EnableDebugLayer();
	debug->Release();

	HRESULT result = S_OK;
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	for (auto level : levels) {
		if (SUCCEEDED(
			D3D12CreateDevice(
				nullptr,
				level,
				IID_PPV_ARGS(&_dev)
			))) {
			break;
		}
	}
	if (_dev == nullptr) {
		return false;
	}

	if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf())))) {
		return false;
	}

	//�R�}���h�L���[�̍쐬
	D3D12_COMMAND_QUEUE_DESC cmdQDesc = {};
	cmdQDesc.NodeMask = 0;//�A�_�v�^�[�}�X�N�Ȃ�
	cmdQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	if (FAILED(_dev->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf())))) {
		return false;
	}

	Application& app = Application::Instance();
	auto wsize = app.GetWindowSize();
	DXGI_SWAP_CHAIN_DESC1 swDesc = {};
	swDesc.BufferCount = 2;//�t���b�v�Ɏg�p���鎆�͂Q��
	swDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swDesc.Flags = 0;
	swDesc.Width = wsize.cx;
	swDesc.Height = wsize.cy;
	swDesc.SampleDesc.Count = 1;
	swDesc.SampleDesc.Quality = 0;
	swDesc.Scaling = DXGI_SCALING_STRETCH;
	swDesc.Stereo = false;
	swDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue.Get(),
		_hwnd,
		&swDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)(_swapchain.ReleaseAndGetAddressOf())
	);
	//if (FAILED(InitializeDXGIDevice())) {
	//	assert(0);
	//	return;
	//}
	//if (FAILED(InitializeCommand())) {
	//	assert(0);
	//	return;
	//}
	//if (FAILED(CreateSwapChain(hwnd))) {
	//	assert(0);
	//	return;
	//}
	if (FAILED(result)) {
		return false;
	}

	if (!CreateRenderTargetView()) {
		return false;
	}

	if (!CreateDepthBuffer()) {
		return false;
	}
	if (!CreateDSV()) {
		return false;
	}

	//�R�}���h���X�g�̍쐬
	if (!CreateCommandList()) {
		return false;
	}

	//if (FAILED(CreateSceneView())) {
	//	assert(0);
	//	return;
	//}

	////�e�N�X�`�����[�_�[�֘A������
	//CreateTextureLoaderTable();

	//�[�x�o�b�t�@�쐬
	//if (FAILED(CreateDepthStencilView())) {
	//	assert(0);
	//	return;
	//}

	////�t�F���X�̍쐬
	//if (FAILED(_dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf())))) {
	//	assert(0);
	//	return;
	//}
	_fenceValue = 0;//��r�p
	result = _dev->CreateFence(
		_fenceValue, //�����l
		D3D12_FENCE_FLAG_NONE, //���Ƀt���O�Ȃ�
		IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf()));//_fence�����
	assert(SUCCEEDED(result));
	//CreatePeraResourcesAndView();

	if (!DXRender->CreatePeraResourcesAndView(
		_dev,
		_rtvHeaps,
		_dsvHeap,
		_cmdList,
		_backBuffers)
		) {
		return false;
	}

	if (!DXRender->createScreenPolygon(_dev)) {
		return false;
	}

	if (!DXRender->pipeline(_dev)) {
		return false;
	}

}

bool Dx12Wrapper::CreateRenderTargetView() {
	DXGI_SWAP_CHAIN_DESC1 swDesc = {};
	auto result = _swapchain->GetDesc1(&swDesc);

	//�����_�[�^�[�Q�b�g�r���[�p�̃q�[�v���쐬
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.NodeMask = 0;
	desc.NumDescriptors = swDesc.BufferCount;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = _dev->CreateDescriptorHeap(&desc,
		IID_PPV_ARGS(&_rtvHeaps));
	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		return false;
	}
	_backBuffers.resize(swDesc.BufferCount);

	auto handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	auto incSize = _dev->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//�X���b�v�`�F�C������A�t���b�v�Ώۂ̃��\�[�X���擾
	for (int i = 0; i < swDesc.BufferCount; ++i) {
		//GetBuffer�̓X���b�v�`�F�C���������Ă�i�Ԗڂ�
		//���\�[�X��������ɓ���Ă����
		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
		assert(SUCCEEDED(result));
		if (FAILED(result)) {
			return false;
		}
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		_dev->CreateRenderTargetView(_backBuffers[i], &rtvDesc, handle);
		handle.ptr += incSize;
	}

	return true;

}

bool Dx12Wrapper::CreateDepthBuffer() {
	D3D12_HEAP_PROPERTIES heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	auto wsize = Application::Instance().GetWindowSize();
	D3D12_RESOURCE_DESC resdesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, wsize.cx, wsize.cy);
	resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE cv = {};
	cv.DepthStencil.Depth = 1.0f;
	cv.Format = DXGI_FORMAT_D32_FLOAT;

	auto result = _dev->CreateCommittedResource(&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&cv,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));

	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		return false;
	}
	else {
		return true;
	}
}

bool Dx12Wrapper::CreateDSV() {
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NodeMask = 0;
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	auto result = _dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		return false;
	}
	D3D12_DEPTH_STENCIL_VIEW_DESC viewDesc = {};
	viewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Flags = D3D12_DSV_FLAG_NONE;
	viewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	_dev->CreateDepthStencilView(_depthBuffer.Get(),
		&viewDesc,
		_dsvHeap->GetCPUDescriptorHandleForHeapStart());


	return true;
}

bool Dx12Wrapper::CreateCommandList() {
	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		return false;
	}
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
	assert(SUCCEEDED(result));
	if (FAILED(result)) {
		return false;
	}
	return true;
}

//
//HRESULT Dx12Wrapper::CreateDepthStencilView() {
//	DXGI_SWAP_CHAIN_DESC1 desc = {};
//	auto result = _swapchain->GetDesc1(&desc);
//	D3D12_RESOURCE_DESC resdesc = {};
//	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	resdesc.DepthOrArraySize = 1;
//	resdesc.Width = desc.Width;
//	resdesc.Height = desc.Height;
//	resdesc.Format = DXGI_FORMAT_D32_FLOAT;
//	resdesc.SampleDesc.Count = 1;
//	resdesc.SampleDesc.Quality = 0;
//	resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
//	resdesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//	resdesc.MipLevels = 1;
//	resdesc.Alignment = 0;
//
//	//�f�v�X�p�q�[�v�v���p�e�B
//	auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
//
//	CD3DX12_CLEAR_VALUE depthClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
//
//	result = _dev->CreateCommittedResource(
//		&depthHeapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&resdesc,
//		D3D12_RESOURCE_STATE_DEPTH_WRITE, //�f�v�X�������݂Ɏg�p
//		&depthClearValue,
//		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//
//	//�[�x�̂��߂̃f�X�N���v�^�q�[�v�쐬
//	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};//�[�x�Ɏg����Ƃ��������킩��΂���
//	dsvHeapDesc.NumDescriptors = 1;//�[�x�r���[1�̂�
//	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//�f�v�X�X�e���V���r���[�Ƃ��Ďg��
//	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//
//
//	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));
//
//	//�[�x�r���[�쐬
//	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
//	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//�f�v�X�l��32bit�g�p
//	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
//	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;//�t���O�͓��ɂȂ�
//	_dev->CreateDepthStencilView(_depthBuffer.Get(), &dsvDesc, _dsvHeap->GetCPUDescriptorHandleForHeapStart());
//}
//

Dx12Wrapper::~Dx12Wrapper()
{
}


ComPtr<ID3D12Resource> Dx12Wrapper::GetTextureByPath(const char* texpath) {
	//auto it = _textureTable.find(texpath);
	//if (it != _textureTable.end()) {
	//	//�e�[�u���ɓ��ɂ������烍�[�h����̂ł͂Ȃ��}�b�v����
	//	//���\�[�X��Ԃ�
	//	return _textureTable[texpath];
	//}
	//else {
	//	return ComPtr<ID3D12Resource>(CreateTextureFromFile(texpath));
	//}
	return _textureTable[texpath];
}

////�e�N�X�`�����[�_�e�[�u���̍쐬
//void Dx12Wrapper::CreateTextureLoaderTable() {
//	_loadLambdaTable["sph"] = _loadLambdaTable["spa"] = _loadLambdaTable["bmp"] = _loadLambdaTable["png"] = _loadLambdaTable["jpg"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img)->HRESULT {
//		return LoadFromWICFile(path.c_str(), 0, meta, img);
//	};
//
//	_loadLambdaTable["tga"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img)->HRESULT {
//		return LoadFromTGAFile(path.c_str(), meta, img);
//	};
//
//	_loadLambdaTable["dds"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img)->HRESULT {
//		return LoadFromDDSFile(path.c_str(), 0, meta, img);
//	};
//}
//
////�e�N�X�`��������e�N�X�`���o�b�t�@�쐬�A���g���R�s�[
//ID3D12Resource* Dx12Wrapper::CreateTextureFromFile(const char* texpath) {
//	string texPath = texpath;
//	//�e�N�X�`���̃��[�h
//	TexMetadata metadata = {};
//	ScratchImage scratchImg = {};
//	auto wtexpath = GetWideStringFromString(texPath);//�e�N�X�`���̃t�@�C���p�X
//	auto ext = GetExtension(texPath);//�g���q���擾
//	auto result = _loadLambdaTable[ext](wtexpath,
//		&metadata,
//		scratchImg);
//	if (FAILED(result)) {
//		return nullptr;
//	}
//	auto img = scratchImg.GetImage(0, 0, 0);//���f�[�^���o
//
//	//WriteToSubresource�œ]������p�̃q�[�v�ݒ�
//	auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
//	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);
//
//	ID3D12Resource* texbuff = nullptr;
//	result = _dev->CreateCommittedResource(
//		&texHeapProp,
//		D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
//		&resDesc,
//		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
//		nullptr,
//		IID_PPV_ARGS(&texbuff)
//	);
//
//	if (FAILED(result)) {
//		return nullptr;
//	}
//	result = texbuff->WriteToSubresource(0,
//		nullptr,//�S�̈�փR�s�[
//		img->pixels,//���f�[�^�A�h���X
//		img->rowPitch,//1���C���T�C�Y
//		img->slicePitch//�S�T�C�Y
//	);
//	if (FAILED(result)) {
//		return nullptr;
//	}
//
//	return texbuff;
//}
//
//HRESULT Dx12Wrapper::InitializeDXGIDevice() {
//	UINT flagsDXGI = 0;
//	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
//	auto result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//	//DirectX12�܂�菉����
//	//�t�B�[�`�����x����
//	D3D_FEATURE_LEVEL levels[] = {
//		D3D_FEATURE_LEVEL_12_1,
//		D3D_FEATURE_LEVEL_12_0,
//		D3D_FEATURE_LEVEL_11_1,
//		D3D_FEATURE_LEVEL_11_0,
//	};
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//	std::vector <IDXGIAdapter*> adapters;
//	IDXGIAdapter* tmpAdapter = nullptr;
//	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
//		adapters.push_back(tmpAdapter);
//	}
//	for (auto adpt : adapters) {
//		DXGI_ADAPTER_DESC adesc = {};
//		adpt->GetDesc(&adesc);
//		std::wstring strDesc = adesc.Description;
//		if (strDesc.find(L"NVIDIA") != std::string::npos) {
//			tmpAdapter = adpt;
//			break;
//		}
//	}
//	result = S_FALSE;
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//	//Direct3D�f�o�C�X�̏�����
//	D3D_FEATURE_LEVEL featureLevel;
//	for (auto l : levels) {
//		if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(_dev.ReleaseAndGetAddressOf())))) {
//			featureLevel = l;
//			result = S_OK;
//			break;
//		}
//	}
//	return result;
//}
//
/////�X���b�v�`�F�C�������֐�
//HRESULT Dx12Wrapper::CreateSwapChain(const HWND& hwnd) {
//	RECT rc = {};
//	::GetWindowRect(hwnd, &rc);
//
//
//	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
//	swapchainDesc.Width = _winSize.cx;
//	swapchainDesc.Height = _winSize.cy;
//	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	swapchainDesc.Stereo = false;
//	swapchainDesc.SampleDesc.Count = 1;
//	swapchainDesc.SampleDesc.Quality = 0;
//	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	swapchainDesc.BufferCount = 2;
//	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
//	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
//	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//
//
//	auto result = _dxgiFactory->CreateSwapChainForHwnd(_cmdQueue.Get(),
//		hwnd,
//		&swapchainDesc,
//		nullptr,
//		nullptr,
//		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());
//	assert(SUCCEEDED(result));
//	return result;
//}
//
////�R�}���h�܂�菉����
//HRESULT Dx12Wrapper::InitializeCommand() {
//	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//
//	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
//	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;//�^�C���A�E�g�Ȃ�
//	cmdQueueDesc.NodeMask = 0;
//	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;//�v���C�I���e�B���Ɏw��Ȃ�
//	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;//�����̓R�}���h���X�g�ƍ��킹�Ă�������
//	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));//�R�}���h�L���[����
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//	return result;
//}
//
////�r���[�v���W�F�N�V�����p�r���[�̐���
//HRESULT Dx12Wrapper::CreateSceneView() {
//	DXGI_SWAP_CHAIN_DESC1 desc = {};
//	auto result = _swapchain->GetDesc1(&desc);
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//	//�萔�o�b�t�@�쐬
//	result = _dev->CreateCommittedResource(
//		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//		D3D12_HEAP_FLAG_NONE,
//		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + 0xff)&~0xff),
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(_sceneConstBuff.ReleaseAndGetAddressOf())
//	);
//
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//
//	_mappedSceneData = nullptr;//�}�b�v��������|�C���^
//	result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//�}�b�v
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//	XMFLOAT3 eye(0, 15, -15);
//	XMFLOAT3 target(0, 15, 0);
//	XMFLOAT3 up(0, 1, 0);
//	_mappedSceneData->view = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
//	_mappedSceneData->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,//��p��45��
//		static_cast<float>(desc.Width) / static_cast<float>(desc.Height),//�A�X��
//		0.1f,//�߂���
//		1000.0f//������
//	);
//	_mappedSceneData->eye = eye;
//
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
//	descHeapDesc.NodeMask = 0;//�}�X�N��0
//	descHeapDesc.NumDescriptors = 1;//
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�f�X�N���v�^�q�[�v���
//	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_sceneDescHeap.ReleaseAndGetAddressOf()));//����
//	if (FAILED(result)) {
//		assert(0);
//		return result;
//	}
//	////�f�X�N���v�^�̐擪�n���h�����擾���Ă���
//	auto heapHandle = _sceneDescHeap->GetCPUDescriptorHandleForHeapStart();
//
//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
//	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
//	cbvDesc.SizeInBytes = _sceneConstBuff->GetDesc().Width;
//	//�萔�o�b�t�@�r���[�̍쐬
//	_dev->CreateConstantBufferView(&cbvDesc, heapHandle);
//	return result;
//
//}
//
//HRESULT Dx12Wrapper::CreateFinalRenderTargets() {
//	DXGI_SWAP_CHAIN_DESC1 desc = {};
//	auto result = _swapchain->GetDesc1(&desc);
//
//	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�����_�[�^�[�Q�b�g�r���[�Ȃ̂œ��RRTV
//	heapDesc.NodeMask = 0;
//	heapDesc.NumDescriptors = 2;//�\���̂Q��
//	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//���Ɏw��Ȃ�
//
//	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_rtvHeaps.ReleaseAndGetAddressOf()));
//	if (FAILED(result)) {
//		SUCCEEDED(result);
//		return result;
//	}
//	DXGI_SWAP_CHAIN_DESC swcDesc = {};
//	result = _swapchain->GetDesc(&swcDesc);
//	_backBuffers.resize(swcDesc.BufferCount);
//
//	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
//
//	//SRGB�����_�[�^�[�Q�b�g�r���[�ݒ�
//	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
//	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
//	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
//
//	for (int i = 0; i < swcDesc.BufferCount; ++i) {
//		result = _swapchain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
//		assert(SUCCEEDED(result));
//		rtvDesc.Format = _backBuffers[i]->GetDesc().Format;
//		_dev->CreateRenderTargetView(_backBuffers[i], &rtvDesc, handle);
//		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//	}
//	_viewport.reset(new CD3DX12_VIEWPORT(_backBuffers[0]));
//	_scissorrect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));
//	return result;
//}

ComPtr< ID3D12Device> Dx12Wrapper::Device() {
	return _dev;
}
ComPtr < ID3D12GraphicsCommandList> Dx12Wrapper::CommandList() {
	return _cmdList;
}



void Dx12Wrapper::Update() {

}

void Dx12Wrapper::BeginDraw() {
	//DXRender->DrawScreen(_cmdList);
	////DirectX����

	////�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

	_cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));


	//�����_�[�^�[�Q�b�g���w��
	auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//�[�x���w��
	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
	_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


	//��ʃN���A
	float clearColor[] = { 0.0f,0.0f,0.0f,1.0f };//���F
	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	//�r���[�|�[�g�A�V�U�[��`�̃Z�b�g
	_cmdList->RSSetViewports(1, _viewport.get());
	_cmdList->RSSetScissorRects(1, _scissorrect.get());


}

void Dx12Wrapper::SetScene() {
	//���݂̃V�[��(�r���[�v���W�F�N�V����)���Z�b�g
	ID3D12DescriptorHeap* sceneheaps[] = { _sceneDescHeap.Get() };
	_cmdList->SetDescriptorHeaps(1, sceneheaps);
	_cmdList->SetGraphicsRootDescriptorTable(0, _sceneDescHeap->GetGPUDescriptorHandleForHeapStart());

}

bool Dx12Wrapper::CreatePeraResourcesAndView(){
	auto heapDesc = _rtvHeaps->GetDesc();
	auto& bbuff = _backBuffers[0];
	auto resDesc = bbuff->GetDesc();
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	float clsClr[4] = { 0.5,0.5,0.5,1.0 };
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_B8G8R8A8_UNORM, clsClr);

	auto result = _dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(mScPolyRes.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return false;
	}
	//D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = 1;

	if (FAILED(_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mScPolyRTV_Heap.ReleaseAndGetAddressOf())))) {
		assert(0);
		return false;
	}

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	_dev->CreateRenderTargetView(
		mScPolyRes.Get(),
		&rtvDesc,
		mScPolyRTV_Heap->GetCPUDescriptorHandleForHeapStart()
	);
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	if (FAILED(_dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(mScPolySRV_Heap.ReleaseAndGetAddressOf())))) {
		assert(0);
		return false;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	_dev->CreateShaderResourceView(
		mScPolyRes.Get(),
		&srvDesc,
		mScPolySRV_Heap->GetCPUDescriptorHandleForHeapStart()
	);
	auto rtvHeapPointer = mScPolyRTV_Heap->GetCPUDescriptorHandleForHeapStart();

	_cmdList->OMSetRenderTargets(
		1,
		&rtvHeapPointer,
		false,
		&_dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);
}

void Dx12Wrapper::EndDraw() {
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();
	_cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//���߂̃N���[�Y
	_cmdList->Close();



	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdlists[] = { _cmdList.Get() };
	_cmdQueue->ExecuteCommandLists(1, cmdlists);
	////�҂�
	_cmdQueue->Signal(_fence.Get(), ++_fenceVal);

	if (_fence->GetCompletedValue() < _fenceVal) {
		auto event = CreateEvent(nullptr, false, false, nullptr);
		_fence->SetEventOnCompletion(_fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}
	_cmdAllocator->Reset();//�L���[���N���A
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);//�ĂуR�}���h���X�g�����߂鏀��
}

ComPtr < IDXGISwapChain4> Dx12Wrapper::Swapchain() {
	return _swapchain;
}