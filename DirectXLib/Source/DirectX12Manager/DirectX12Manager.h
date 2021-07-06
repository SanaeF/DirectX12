#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<map>
#include<unordered_map>
#include<DirectXTex.h>
#include<wrl.h>
#include<string>
#include<functional>
namespace lib {
	class DirectX12Manager {
		HWND m_Hwnd;
		UINT64 m_Fence_value;
		SIZE m_Win_size, m_Graphic_size;
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		//DXGI�܂��
		ComPtr<IDXGIFactory4> m_DxGi_factory = nullptr;//DXGI�C���^�[�t�F�C�X
		ComPtr<IDXGISwapChain4> m_Swapchain = nullptr;//�X���b�v�`�F�C��

		//DirectX12�܂��
		ComPtr<ID3D12Device> m_Device = nullptr;//�f�o�C�X
		ComPtr<ID3D12CommandAllocator> m_Cmd_allocator = nullptr;//�R�}���h�A���P�[�^
		ComPtr<ID3D12GraphicsCommandList> m_Cmd_list = nullptr;//�R�}���h���X�g
		ComPtr<ID3D12CommandQueue> m_Cmd_queue = nullptr;//�R�}���h�L���[

		//�\���Ɋւ��o�b�t�@����
		ComPtr<ID3D12Resource> m_Depth_buffer = nullptr;//�[�x�o�b�t�@
		std::vector<ID3D12Resource*> m_pBack_buffers;//�o�b�N�o�b�t�@(2�ȏ�c�X���b�v�`�F�C�����m��)
		ComPtr<ID3D12DescriptorHeap> m_RTV_heaps = nullptr;//�����_�[�^�[�Q�b�g�p�f�X�N���v�^�q�[�v
		ComPtr<ID3D12DescriptorHeap> m_DSV_heap = nullptr;//�[�x�o�b�t�@�r���[�p�f�X�N���v�^�q�[�v
		std::unique_ptr<D3D12_VIEWPORT> m_View_port;//�r���[�|�[�g
		std::unique_ptr<D3D12_RECT> m_Scissor_rect;//�V�U�[��`
		//�V�[�����\������o�b�t�@�܂��
		ComPtr<ID3D12Resource> m_Scene_const_buff = nullptr;

		struct SceneData {
			DirectX::XMMATRIX view;//�r���[�s��
			DirectX::XMMATRIX proj;//�v���W�F�N�V�����s��
			DirectX::XMFLOAT3 eye;//���_���W
		};
		SceneData* m_pMapped_scene_data;
		ComPtr<ID3D12DescriptorHeap> m_Scene_desc_heap = nullptr;

		//�t�F���X
		ComPtr<ID3D12Fence> m_Fence = nullptr;
		UINT64 m_Fence_val = 0;

		//���[�h�p�e�[�u��
		using LoadLambda_t = std::function<HRESULT(const std::wstring& path, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
		std::map<std::string, LoadLambda_t> m_Load_lambda_table;
		//�e�N�X�`���e�[�u��
		std::unordered_map<std::string, ComPtr<ID3D12Resource>> m_Texture_table;
		std::vector<int> m_Func_count;
		std::vector<int> m_Func_count_3d;
	public:
		DirectX12Manager(HWND hwnd);
		~DirectX12Manager();
		bool init(SIZE pix);
		//void draw(std::shared_ptr<PMDRenderer> renderer);
		//void update();
		void clearScreen();
		void commandClear();
		void setNextCommand();
		///�e�N�X�`���p�X����K�v�ȃe�N�X�`���o�b�t�@�ւ̃|�C���^��Ԃ�
		///@param texpath �e�N�X�`���t�@�C���p�X
		ComPtr<ID3D12Resource> getTextureByPath(const char* texpath);
		ComPtr<ID3D12Device> device();//�f�o�C�X
		ComPtr<ID3D12GraphicsCommandList> cmdList();//�R�}���h���X�g
		ComPtr<IDXGISwapChain4> swapchain();//�X���b�v�`�F�C��
		D3D12_VIEWPORT* getViewPort();//�r���[�|�[�g
		D3D12_RECT* getScissorrect();//�V�U�[��`

		void clearCount();
		void screenFlip();
		void setScene();
		void setCount(int Handle, int num);
		void setCount3D(int Handle, int num);
		int getCount(int Handle);
		void setPixelSize(SIZE pix);
		int getCount3D(int Handle);
		SIZE getWinSize();
		SIZE getPixelSize();

	private:
		//�ŏI�I�ȃ����_�[�^�[�Q�b�g�̐���
		HRESULT	createFinalRenderTargets();
		//�f�v�X�X�e���V���r���[�̐���
		HRESULT createDepthStencilView();
		//�X���b�v�`�F�C���̐���
		HRESULT createSwapChain(const HWND& hwnd, SIZE pix);
		//DXGI�܂�菉����
		HRESULT initializeDXGIDevice();
		//�R�}���h�܂�菉����
		HRESULT initializeCommand();
		//�r���[�v���W�F�N�V�����p�r���[�̐���
		//HRESULT CreateSceneView();
		//�e�N�X�`�����[�_�e�[�u���̍쐬
		//void CreateTextureLoaderTable();
		//�e�N�X�`��������e�N�X�`���o�b�t�@�쐬�A���g���R�s�[
		ID3D12Resource* createTextureFromFile(const char* texpath);
	};
}