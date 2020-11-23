#pragma once
#include<Windows.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>
#include<map>
#include<d3dcompiler.h>
#include<DirectXTex.h>
#include<d3dx12.h>
#include<wrl.h>
#include<memory>
#include "DirectXLib.h"

class Dx12Wrapper;
class PMDRenderer;
class DXPMDModel;

class DxGraph;
class DxKeyConfig;
class DxSound;

///�V���O���g���N���X
class Application{
private:

	SIZE mWin, mPix;

	const char* winName[2];

	//�����ɕK�v�ȕϐ�(�o�b�t�@��q�[�v�Ȃ�)������
	//�E�B���h�E����
	WNDCLASSEX _windowClass;
	HWND _hwnd;
	//std::shared_ptr<DirectXLib>DirectXlib;

	std::shared_ptr<Dx12Wrapper> mDxWr;
	std::shared_ptr<PMDRenderer> mPMDRenderer;
	std::shared_ptr<DXPMDModel> mPMDModel;
	std::shared_ptr<DxGraph>Graph;
	std::shared_ptr<DxKeyConfig>Key;
	std::shared_ptr<DxSound>Sound;
	//�Q�[���p�E�B���h�E�̐���
	void CreateGameWindow(HWND &hwnd, WNDCLASSEX &windowClass);



	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

public:

	///������
	bool Initialize();

	///���[�v�N��
	void ShowWin();

	void RunTest();
	void Run();

	///�㏈��
	void Finaliz();



	void SetWindow(int width, int height, const char* window_name, const char* name);

	void SetGraphMode(int width, int height);

	SIZE GetWindowSize()const;

	SIZE GetGraphicSize()const;


	static Application& Instance();
	~Application();

};
