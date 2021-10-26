#include <iostream>
#include "DirectX12Manager.h"
#include<cassert>
#include<d3dx12.h>
//#include "DXResource.h"
//#include "DX_MultRend.h"
//#include"Application.h"
//#include"PMDRenderer.h"

#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;
namespace {
	std::string GetTexturePathFromModelAndTexPath(const std::string& modelPath, const char* texPath) {
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');
		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}

	string
		GetExtension(const std::string& path) {
		int idx = path.rfind('.');
		return path.substr(idx + 1, path.length() - idx - 1);
	}

	wstring
		GetExtension(const std::wstring& path) {
		int idx = path.rfind(L'.');
		return path.substr(idx + 1, path.length() - idx - 1);
	}

	pair<string, string>
		SplitFileName(const std::string& path, const char splitter = '*') {
		int idx = path.find(splitter);
		pair<string, string> ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr(idx + 1, path.length() - idx - 1);
		return ret;
	}

	std::wstring
		GetWideStringFromString(const std::string& str) {
		//�Ăяo��1���(�����񐔂𓾂�)
		auto num1 = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(), -1, nullptr, 0);

		std::wstring wstr;//string��wchar_t��
		wstr.resize(num1);//����ꂽ�����񐔂Ń��T�C�Y

		//�Ăяo��2���(�m�ۍς݂�wstr�ɕϊ���������R�s�[)
		auto num2 = MultiByteToWideChar(CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(), -1, &wstr[0], num1);

		assert(num1 == num2);//�ꉞ�`�F�b�N
		return wstr;
	}
	///�f�o�b�O���C���[��L���ɂ���
	void EnableDebugLayer() {
		ComPtr<ID3D12Debug> debugLayer = nullptr;
		auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
		debugLayer->EnableDebugLayer();
	}
}
namespace lib {
	bool DirectX12Manager::init(SIZE pix) {
		EnableDebugLayer();
		setPixelSize(pix);
		//DirectX12�֘A������
		if (FAILED(initializeDXGIDevice())) {
			assert(0);
			return false;
		}
		if (FAILED(initializeCommand())) {
			assert(0);
			return false;
		}
		if (FAILED(createSwapChain(m_Hwnd, pix))) {
			assert(0);
			return false;
		}
		if (FAILED(createFinalRenderTargets())) {
			assert(0);
			return false;
		}

		//if (FAILED(CreateSceneView())) {
		//	assert(0);
		//	return false;
		//}

		//�e�N�X�`�����[�_�[�֘A������
		//CreateTextureLoaderTable();

		//�[�x�o�b�t�@�쐬
		if (FAILED(createDepthStencilView())) {
			assert(0);
			return false;
		}

		//�t�F���X�̍쐬
		if (FAILED(m_Device->CreateFence(m_Fence_val, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_Fence.ReleaseAndGetAddressOf())))) {
			assert(0);
			return false;
		}

		/*if (FAILED(
			Resource->CreateScreenResAndView(_dev, _rtvHeaps, _dsvHeap, _cmdList, _backBuffers)
		)) {
			assert(0);
			return false;
		}
		if (FAILED(
			Render->createScreenPolygon(_dev)
		)) {
			assert(0);
			return false;
		}
		if (FAILED(
			Render->pipeline(_dev)
		)) {
			assert(0);
			return false;
		}*/
	}

	HRESULT DirectX12Manager::createDepthStencilView() {
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		auto result = m_Swapchain->GetDesc1(&desc);
		D3D12_RESOURCE_DESC resdesc = {};
		resdesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resdesc.DepthOrArraySize = 1;
		resdesc.Width = desc.Width;
		resdesc.Height = desc.Height;
		resdesc.Format = DXGI_FORMAT_D32_FLOAT;
		resdesc.SampleDesc.Count = 1;
		resdesc.SampleDesc.Quality = 0;
		resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


		//�f�v�X�p�q�[�v�v���p�e�B
		auto depthHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		CD3DX12_CLEAR_VALUE depthClearValue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

		result = m_Device->CreateCommittedResource(
			&depthHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE, //�f�v�X�������݂Ɏg�p
			&depthClearValue,
			IID_PPV_ARGS(m_Depth_buffer.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			//�G���[����
			return result;
		}

		//�[�x�̂��߂̃f�X�N���v�^�q�[�v�쐬
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};//�[�x�Ɏg����Ƃ��������킩��΂���
		dsvHeapDesc.NumDescriptors = 1;//�[�x�r���[1�̂�
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//�f�v�X�X�e���V���r���[�Ƃ��Ďg��


		result = m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_DSV_heap.ReleaseAndGetAddressOf()));

		//�[�x�r���[�쐬
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//�f�v�X�l��32bit�g�p
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;//�t���O�͓��ɂȂ�
		m_Device->CreateDepthStencilView(m_Depth_buffer.Get(), &dsvDesc, m_DSV_heap->GetCPUDescriptorHandleForHeapStart());
	}

	HRESULT DirectX12Manager::initializeDXGIDevice() {
		UINT flagsDXGI = 0;
		flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
		auto result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_DxGi_factory.ReleaseAndGetAddressOf()));
		//DirectX12�܂�菉����
		//�t�B�[�`�����x����
		D3D_FEATURE_LEVEL levels[] = {
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		if (FAILED(result)) {
			return result;
		}
		std::vector <IDXGIAdapter*> adapters;
		IDXGIAdapter* tmpAdapter = nullptr;
		for (int i = 0; m_DxGi_factory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
			adapters.push_back(tmpAdapter);
		}
		for (auto adpt : adapters) {
			DXGI_ADAPTER_DESC adesc = {};
			adpt->GetDesc(&adesc);
			std::wstring strDesc = adesc.Description;
			if (strDesc.find(L"NVIDIA") != std::string::npos) {
				tmpAdapter = adpt;
				break;
			}
		}
		result = S_FALSE;
		//Direct3D�f�o�C�X�̏�����
		D3D_FEATURE_LEVEL featureLevel;
		for (auto l : levels) {
			if (SUCCEEDED(D3D12CreateDevice(tmpAdapter, l, IID_PPV_ARGS(m_Device.ReleaseAndGetAddressOf())))) {
				featureLevel = l;
				result = S_OK;
				break;
			}
		}
		return result;
	}
	///�X���b�v�`�F�C�������֐�
	HRESULT DirectX12Manager::createSwapChain(const HWND& hwnd, SIZE pix) {
		RECT rc = {};
		::GetWindowRect(hwnd, &rc);


		DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
		swapchainDesc.Width = pix.cx;
		swapchainDesc.Height = pix.cy;
		swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapchainDesc.Stereo = false;
		swapchainDesc.SampleDesc.Count = 1;
		swapchainDesc.SampleDesc.Quality = 0;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount = 2;
		swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


		auto result = m_DxGi_factory->CreateSwapChainForHwnd(m_Cmd_queue.Get(),
			hwnd,
			&swapchainDesc,
			nullptr,
			nullptr,
			(IDXGISwapChain1**)m_Swapchain.ReleaseAndGetAddressOf());
		assert(SUCCEEDED(result));
		return result;
	}
	//�R�}���h�܂�菉����
	HRESULT DirectX12Manager::initializeCommand() {
		auto result = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_Cmd_allocator.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			assert(0);
			return result;
		}
		result = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_Cmd_allocator.Get(), nullptr, IID_PPV_ARGS(m_Cmd_list.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			assert(0);
			return result;
		}

		D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
		cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;//�^�C���A�E�g�Ȃ�
		cmdQueueDesc.NodeMask = 0;
		cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;//�v���C�I���e�B���Ɏw��Ȃ�
		cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;//�����̓R�}���h���X�g�ƍ��킹�Ă�������
		result = m_Device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(m_Cmd_queue.ReleaseAndGetAddressOf()));//�R�}���h�L���[����
		assert(SUCCEEDED(result));
		return result;
	}
	////�r���[�v���W�F�N�V�����p�r���[�̐���
	//HRESULT DirectX12Manager::CreateSceneView() {
	//	DXGI_SWAP_CHAIN_DESC1 desc = {};
	//	auto result = _swapchain->GetDesc1(&desc);
	//	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + 0xff) & ~0xff);
	//	//�萔�o�b�t�@�쐬
	//	result = _dev->CreateCommittedResource(
	//		&heapProp,
	//		D3D12_HEAP_FLAG_NONE,
	//		&resDesc,
	//		D3D12_RESOURCE_STATE_GENERIC_READ,
	//		nullptr,
	//		IID_PPV_ARGS(_sceneConstBuff.ReleaseAndGetAddressOf())
	//	);
	//	if (FAILED(result)) {
	//		assert(SUCCEEDED(result));
	//		return result;
	//	}
	//	_mappedSceneData = nullptr;//�}�b�v��������|�C���^
	//	result = _sceneConstBuff->Map(0, nullptr, (void**)&_mappedSceneData);//�}�b�v
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
	//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	//	descHeapDesc.NodeMask = 0;//�}�X�N��0
	//	descHeapDesc.NumDescriptors = 1;//
	//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�f�X�N���v�^�q�[�v���
	//	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_sceneDescHeap.ReleaseAndGetAddressOf()));//����
	//	////�f�X�N���v�^�̐擪�n���h�����擾���Ă���
	//	auto heapHandle = _sceneDescHeap->GetCPUDescriptorHandleForHeapStart();
	//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	//	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
	//	cbvDesc.SizeInBytes = _sceneConstBuff->GetDesc().Width;
	//	//�萔�o�b�t�@�r���[�̍쐬
	//	_dev->CreateConstantBufferView(&cbvDesc, heapHandle);
	//	return result;
	//}

	HRESULT DirectX12Manager::createFinalRenderTargets() {
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		auto result = m_Swapchain->GetDesc1(&desc);

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�����_�[�^�[�Q�b�g�r���[�Ȃ̂œ��RRTV
		heapDesc.NodeMask = 0;
		heapDesc.NumDescriptors = 2;//�\���̂Q��
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//���Ɏw��Ȃ�

		result = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_RTV_heaps.ReleaseAndGetAddressOf()));
		if (FAILED(result)) {
			SUCCEEDED(result);
			return result;
		}
		DXGI_SWAP_CHAIN_DESC swcDesc = {};
		result = m_Swapchain->GetDesc(&swcDesc);
		m_pBack_buffers.resize(swcDesc.BufferCount);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTV_heaps->GetCPUDescriptorHandleForHeapStart();

		//SRGB�����_�[�^�[�Q�b�g�r���[�ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		for (int i = 0; i < swcDesc.BufferCount; ++i) {
			result = m_Swapchain->GetBuffer(i, IID_PPV_ARGS(&m_pBack_buffers[i]));
			assert(SUCCEEDED(result));
			rtvDesc.Format = m_pBack_buffers[i]->GetDesc().Format;
			m_Device->CreateRenderTargetView(m_pBack_buffers[i], &rtvDesc, handle);
			handle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}
		m_View_port.reset(new CD3DX12_VIEWPORT(m_pBack_buffers[0]));
		m_Scissor_rect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));
		return result;
	}

	////�e�N�X�`�����[�_�e�[�u���̍쐬
	//void DirectX12Manager::CreateTextureLoaderTable() {
	//	_loadLambdaTable["sph"] = _loadLambdaTable["spa"] = _loadLambdaTable["bmp"] = _loadLambdaTable["png"] = _loadLambdaTable["jpg"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img)->HRESULT {
	//		return LoadFromWICFile(path.c_str(), 0, meta, img);
	//	};
	//	_loadLambdaTable["tga"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img)->HRESULT {
	//		return LoadFromTGAFile(path.c_str(), meta, img);
	//	};
	//	_loadLambdaTable["dds"] = [](const wstring& path, TexMetadata* meta, ScratchImage& img)->HRESULT {
	//		return LoadFromDDSFile(path.c_str(), 0, meta, img);
	//	};
	//}

	//void DirectX12Manager::update() {
	//	//Render->DrawScreen(
	//	//	Resource->getResource(),
	//	//	_cmdList
	//	//);
	//}

	//void DirectX12Manager::draw(shared_ptr<PMDRenderer> renderer) {
	//	//_cmdList->SetPipelineState(Render->getPipeline());
	//	//_cmdList->SetPipelineState(renderer->GetPipelineState());
	//	////���[�g�V�O�l�`����PMD�p�ɍ��킹��
	//	//_cmdList->SetGraphicsRootSignature(renderer->GetRootSignature());
	//	//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//}

	void DirectX12Manager::clearScreen() {
		//DirectX����
		//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
		auto bbIdx = m_Swapchain->GetCurrentBackBufferIndex();
		//�����_�[�^�[�Q�b�g���w��
		auto rtvH = m_RTV_heaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		auto resBuff = CD3DX12_RESOURCE_BARRIER::Transition(
			m_pBack_buffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		m_Cmd_list->ResourceBarrier(1, &resBuff);
		//�[�x���w��
		auto dsvH = m_DSV_heap->GetCPUDescriptorHandleForHeapStart();
		m_Cmd_list->OMSetRenderTargets(1, &rtvH, false, &dsvH);
		//��ʃN���A
		float clearColor[] = { 0.2f,0.2f,0.2f,1.f };//���F
		m_Cmd_list->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		m_Cmd_list->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	}

	void DirectX12Manager::setScene() {
		//���݂̃V�[��(�r���[�v���W�F�N�V����)���Z�b�g
		ID3D12DescriptorHeap* sceneheaps[] = { m_Scene_desc_heap.Get() };
		m_Cmd_list->SetDescriptorHeaps(1, sceneheaps);
		m_Cmd_list->SetGraphicsRootDescriptorTable(0, m_Scene_desc_heap->GetGPUDescriptorHandleForHeapStart());

	}

	void DirectX12Manager::closeCommand() {
		/*_cmdList->ClearDepthStencilView(_dsvHeap->GetCPUDescriptorHandleForHeapStart(),
			D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);*/


		auto bbIdx = m_Swapchain->GetCurrentBackBufferIndex();
		auto resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_pBack_buffers[bbIdx],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);

		m_Cmd_list->ResourceBarrier(1, &resBarrier);

		//���߂̃N���[�Y
		m_Cmd_list->Close();
	}

	void DirectX12Manager::exeCommand() {
		//�R�}���h���X�g�̎��s
		ID3D12CommandList* cmdlists[] = { m_Cmd_list.Get() };
		m_Cmd_queue->ExecuteCommandLists(1, cmdlists);
		////�҂�
		m_Cmd_queue->Signal(m_Fence.Get(), ++m_Fence_val);

		if (m_Fence->GetCompletedValue() < m_Fence_val) {
			auto event = CreateEvent(nullptr, false, false, nullptr);
			m_Fence->SetEventOnCompletion(m_Fence_val, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		m_Cmd_allocator->Reset();//�L���[���N���A
		m_Cmd_list->Reset(m_Cmd_allocator.Get(), nullptr);//�ĂуR�}���h���X�g�����߂鏀��
	}

	void DirectX12Manager::setPixelSize(SIZE pix) {
		m_Graphic_size = pix;
	}

	void DirectX12Manager::screenFlip() {
		//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//_cmdList->DrawIndexedInstanced(6, getCount(), 0, 0, 0);
		clearCount();
		closeCommand();
		exeCommand();
		auto result = m_Swapchain->Present(0, 0);
		assert(SUCCEEDED(result));
		//BeginDraw();
	}

	void DirectX12Manager::clearCount() {
		m_Func_count.clear();
		m_Func_count_3d.clear();
	}
	void DirectX12Manager::setCount(int Handle, int num) {
		int VectorSize = Handle + 1;
		if (Handle > 0)m_Func_count.resize(VectorSize);
		else {
			m_Func_count.resize(1);
		}
		m_Func_count[Handle] = num;
	}
	int DirectX12Manager::getCount(int Handle) {
		if (m_Func_count.size() > Handle)return m_Func_count[Handle];
		return 0;
	}
	void DirectX12Manager::setCount3D(int Handle, int num) {
		int VectorSize = Handle + 1;
		if (Handle > 0)m_Func_count_3d.resize(VectorSize);
		else {
			m_Func_count_3d.resize(1);
		}
		m_Func_count_3d[Handle] = num;
	}
	int DirectX12Manager::getCount3D(int Handle) {
		if (m_Func_count_3d.size() > Handle)return m_Func_count_3d[Handle];
		return 0;
	}

	SIZE DirectX12Manager::getWinSize() {
		return m_Win_size;
	}

	SIZE DirectX12Manager::getPixelSize() {
		return m_Graphic_size;
	}

	D3D12_VIEWPORT* DirectX12Manager::getViewPort() {
		return m_View_port.get();
	}

	D3D12_RECT* DirectX12Manager::getScissorrect() {
		return m_Scissor_rect.get();
	}
	//�e�N�X�`��������e�N�X�`���o�b�t�@�쐬�A���g���R�s�[
	ID3D12Resource* DirectX12Manager::createTextureFromFile(const char* texpath) {
		string texPath = texpath;
		//�e�N�X�`���̃��[�h
		TexMetadata metadata = {};
		ScratchImage scratchImg = {};
		auto wtexpath = GetWideStringFromString(texPath);//�e�N�X�`���̃t�@�C���p�X
		auto ext = GetExtension(texPath);//�g���q���擾
		auto result = m_Load_lambda_table[ext](wtexpath,
			&metadata,
			scratchImg);
		if (FAILED(result)) {
			return nullptr;
		}
		auto img = scratchImg.GetImage(0, 0, 0);//���f�[�^���o

		//WriteToSubresource�œ]������p�̃q�[�v�ݒ�
		auto texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
		auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(metadata.format, metadata.width, metadata.height, metadata.arraySize, metadata.mipLevels);

		ID3D12Resource* texbuff = nullptr;
		result = m_Device->CreateCommittedResource(
			&texHeapProp,
			D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
			&resDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&texbuff)
		);

		if (FAILED(result)) {
			return nullptr;
		}
		result = texbuff->WriteToSubresource(0,
			nullptr,//�S�̈�փR�s�[
			img->pixels,//���f�[�^�A�h���X
			img->rowPitch,//1���C���T�C�Y
			img->slicePitch//�S�T�C�Y
		);
		if (FAILED(result)) {
			return nullptr;
		}

		return texbuff;
	}

	ComPtr < IDXGISwapChain4> DirectX12Manager::swapchain() {
		return m_Swapchain;
	}

	ComPtr< ID3D12Device> DirectX12Manager::device() {
		return m_Device;
	}

	ComPtr < ID3D12GraphicsCommandList> DirectX12Manager::cmdList() {
		return m_Cmd_list;
	}

	ComPtr<ID3D12Resource> DirectX12Manager::getTextureByPath(const char* texpath) {
		auto it = m_Texture_table.find(texpath);
		if (it != m_Texture_table.end()) {
			//�e�[�u���ɓ��ɂ������烍�[�h����̂ł͂Ȃ��}�b�v����
			//���\�[�X��Ԃ�
			return m_Texture_table[texpath];
		}
		else {
			return ComPtr<ID3D12Resource>(createTextureFromFile(texpath));
		}

	}


	DirectX12Manager::DirectX12Manager(HWND hwnd) :
		m_Hwnd(hwnd), m_Func_count(0) {
#ifdef _DEBUG
		//�f�o�b�O���C���[���I����
		EnableDebugLayer();
#endif
		//Resource.reset(new DXResource());
		//Render.reset(new DX_MultRend());

		//auto& app = Application::Instance();
		//_winSize = app.GetGraphicSize();

	}

	DirectX12Manager::~DirectX12Manager() {
	}
}