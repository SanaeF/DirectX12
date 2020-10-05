#include "Application.h"
#include"Dx12Wrapper.h"
#include "DX_Resource.h"
#include"PMDRenderer.h"
#include"PMDActor.h"
void Application::SetWindow(int width, int height, const char* window_name, const char* Name) {

	mWin.cx = width;
	mWin.cy = height;

	winName[0] = window_name;
	winName[1] = Name;
}
void Application::SetGraphMode(int width, int height) {
	mPix.cx = width;
	mPix.cy = height;
}



SIZE Application::GetWindowSize()const {
	SIZE ret;
	ret.cx = mWin.cx;
	ret.cy = mWin.cy;//_graphSize
	return ret;
}

SIZE Application::GetGraphicSize()const {
	SIZE ret;
	ret.cx = mPix.cx;
	ret.cy = mPix.cy;
	return ret;
}

//�ʓ|�����Ǐ����Ȃ�������
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {//�E�B���h�E���j�����ꂽ��Ă΂�܂�
		PostQuitMessage(0);//OS�ɑ΂��āu�������̃A�v���͏I�����v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�K��̏������s��
}
void Application::CreateGameWindow(HWND &hwnd, WNDCLASSEX &windowClass) {
	HINSTANCE hInst = GetModuleHandle(nullptr);
	//�E�B���h�E�N���X�������o�^
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�֐��̎w��
	windowClass.lpszClassName = _T(winName[0]);//�A�v���P�[�V�����N���X��(�K���ł����ł�)
	windowClass.hInstance = GetModuleHandle(0);//�n���h���̎擾
	RegisterClassEx(&windowClass);//�A�v���P�[�V�����N���X(���������̍�邩���낵������OS�ɗ\������)

	RECT wrc = { 0,0, GetWindowSize().cx, GetWindowSize().cy };//�E�B���h�E�T�C�Y�����߂�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�E�B���h�E�̃T�C�Y�͂�����Ɩʓ|�Ȃ̂Ŋ֐����g���ĕ␳����
	//�E�B���h�E�I�u�W�F�N�g�̐���
	hwnd = CreateWindow(windowClass.lpszClassName,//�N���X���w��
		_T(winName[1]),//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E�ł�
		CW_USEDEFAULT,//�\��X���W��OS�ɂ��C�����܂�
		CW_USEDEFAULT,//�\��Y���W��OS�ɂ��C�����܂�
		wrc.right - wrc.left,//�E�B���h�E��
		wrc.bottom - wrc.top,//�E�B���h�E��
		nullptr,//�e�E�B���h�E�n���h��
		nullptr,//���j���[�n���h��
		windowClass.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);//�ǉ��p�����[�^

}


void Application::ShowWin() {
	ShowWindow(_hwnd, SW_SHOW);//�E�B���h�E�\��
}


bool Application::Init() {
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);
	CreateGameWindow(_hwnd, _windowClass);
	//DirectX12���b�p�[������������
	return true;
}

void Application::Run() {
	pDx12.reset(new Dx12Wrapper(_hwnd));
	//pPMDRenderer[1].reset(new PMDRenderer(*pDx12)); 
	//pPMDRenderer[0].reset(new PMDRenderer(*pDx12));
	//pPMDActor.reset(new PMDActor("model/���͂��Ƃ�i�{�Z�����j�~�N.pmd", *pPMDRenderer[0]));
	//pPMDActor.reset(new PMDActor("model/���q�����i�{�Z�����j�~�N.pmd", *pPMDRenderer[1]));//�e�N�X�`�����㏑�����Ă���
	//pPMDActor->LoadVMDFile("motion/motion.vmd", "pose");
	//pPMDActor->PlayAnimation();
	float angle = 0.0f;
	MSG msg = {};
	unsigned int frame = 0;
	//while (true) {
	//	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
	//		TranslateMessage(&msg);
	//		DispatchMessage(&msg);
	//	}
	//	//�����A�v���P�[�V�������I�����Ď���message��WM_QUIT�ɂȂ�
	//	if (msg.message == WM_QUIT) {
	//		break;
	//	}
	//	//�S�̂̕`�揀��
	//	pDx12->BeginDraw();
	//	//PMD�p�̕`��p�C�v���C���ɍ��킹��
	//	pDx12->CommandList()->SetPipelineState(pPMDRenderer[1]->GetPipelineState());
	//	pDx12->CommandList()->SetPipelineState(pPMDRenderer[0]->GetPipelineState());
	//	//���[�g�V�O�l�`����PMD�p�ɍ��킹��
	//	pDx12->CommandList()->SetGraphicsRootSignature(pPMDRenderer[1]->GetRootSignature());
	//	pDx12->CommandList()->SetGraphicsRootSignature(pPMDRenderer[0]->GetRootSignature());
	//	pDx12->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	pDx12->SetScene();

	//	pPMDActor->Update();
	//	pPMDActor->Draw();

	//	pDx12->EndDraw();

	//	//�t���b�v
	//	pDx12->Swapchain()->Present(1, 0);
	//}
}


void Application::Finaliz() {
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);
}


Application& Application::Instance() {
	static Application instance;
	return instance;
}


Application::Application(){
}



Application::~Application()
{
}