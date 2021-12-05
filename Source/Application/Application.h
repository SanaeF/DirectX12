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
namespace lib {
	class DirectX12Manager;
	class DxKeyConfig;
	class DxSound;
	class DxGraphics;
	class FPSManager;
}
///�V���O���g���N���X
class Application{
private:
	std::shared_ptr<lib::DirectX12Manager> m_Dx12;
	std::shared_ptr<lib::DxGraphics>m_Graphics;
	std::shared_ptr<lib::DxKeyConfig>m_Key;
	std::shared_ptr<lib::DxSound>m_Sound;
	std::shared_ptr<lib::FPSManager>m_Fps;
	SIZE m_Win, m_Pix;
	const char* m_WinName[2];
	//�E�B���h�E����
	WNDCLASSEX m_Window_class;
	HWND m_Hwnd;
public:
	///������
	bool initialize();
	///���[�v�N��
	void showWin();
	void run();
	///�㏈��
	void finaliz();

	void setWindow(int width, int height, const char* window_name, const char* name);
	void setGraphMode(int width, int height);
	SIZE getWindowSize()const;
	SIZE getGraphicSize()const;
	static Application& getInstance();
	~Application();
private:
	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;
	//�Q�[���p�E�B���h�E�̐���
	void createGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);
};
