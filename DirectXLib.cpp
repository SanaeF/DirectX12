#include "DirectXLib.h"
#include<tchar.h>

#include"Dx12Wrapper.h"
#include"PMDRenderer.h"
#include"DXPMDModel.h"
#include "DxGraph.h"
#include "DxKeyConfig.h"
#include "DxSound.h"
//
//LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
//	if (msg == WM_DESTROY) {//�E�B���h�E���j�����ꂽ��Ă΂�܂�
//		PostQuitMessage(0);//OS�ɑ΂��āu�������̃A�v���͏I�����v�Ɠ`����
//		return 0;
//	}
//	return DefWindowProc(hwnd, msg, wparam, lparam);//�K��̏������s��
//}
//
//
//bool DirectXLib::Initialize() {
//	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);
//	CreateGameWindow(_hwnd, _windowClass);
//	mDxWr.reset(new Dx12Wrapper(_hwnd));
//	mDxWr->Init(mPix);
//	Sound.reset(new DxSound(_hwnd));
//	Graph.reset(new DxGraph(mDxWr));
//	KeyConfig.reset(new DxKeyConfig(_windowClass));
//	return true;
//}
//
//void DirectXLib::Finaliz() {
//	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);
//}
//
//void DirectXLib::CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass) {
//	HINSTANCE hInst = GetModuleHandle(nullptr);
//	//�E�B���h�E�N���X�������o�^
//	windowClass.cbSize = sizeof(WNDCLASSEX);
//	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�֐��̎w��
//	windowClass.lpszClassName = _T(winName[0]);//�A�v���P�[�V�����N���X��(�K���ł����ł�)
//	windowClass.hInstance = GetModuleHandle(0);//�n���h���̎擾
//	auto result = RegisterClassEx(&windowClass);//�A�v���P�[�V�����N���X(���������̍�邩���낵������OS�ɗ\������)
//
//	RECT wrc = { 0,0, GetWindowSize().cx, GetWindowSize().cy };//�E�B���h�E�T�C�Y�����߂�
//	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�E�B���h�E�̃T�C�Y�͂�����Ɩʓ|�Ȃ̂Ŋ֐����g���ĕ␳����
//
//	int x = GetSystemMetrics(SM_CXSCREEN);
//	int y = GetSystemMetrics(SM_CYSCREEN);
//	x = x - (wrc.right - wrc.left);
//	y = y - (wrc.bottom - wrc.top);
//	//�E�B���h�E�I�u�W�F�N�g�̐���
//	hwnd = CreateWindow(
//		windowClass.lpszClassName,//�N���X���w��
//		_T(winName[1]),//�^�C�g���o�[�̕���
//		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E�ł�
//		x / 2,//�\��X���W��OS�ɂ��C�����܂�
//		y / 2,//�\��Y���W��OS�ɂ��C�����܂�
//		wrc.right - wrc.left,//�E�B���h�E��
//		wrc.bottom - wrc.top,//�E�B���h�E��
//		nullptr,//�e�E�B���h�E�n���h��
//		nullptr,//���j���[�n���h��
//		windowClass.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
//		nullptr);//�ǉ��p�����[�^
//	if (hwnd == NULL)assert(0);
//	return;
//}
//
//void DirectXLib::SetShowWindow() {
//	ShowWindow(_hwnd, SW_SHOW);//�E�B���h�E�\��
//}
//
//
//void DirectXLib::SetWindow(int width, int height, const char* window_name, const char* Name) {
//	mWin.cx = width;
//	mWin.cy = height;
//	winName[0] = window_name;
//	winName[1] = Name;
//}
//
//void DirectXLib::SetGraphMode(int width, int height) {
//	mPix.cx = width;
//	mPix.cy = height;
//}
//
//SIZE DirectXLib::GetWindowSize()const {
//	SIZE ret;
//	ret.cx = mWin.cx;
//	ret.cy = mWin.cy;//_graphSize
//	return ret;
//}
//
//SIZE DirectXLib::GetGraphicSize()const {
//	SIZE ret;
//	ret.cx = mPix.cx;
//	ret.cy = mPix.cy;
//	return ret;
//}
//
//DirectXLib::DirectXLib(){
//	this->SetWindow(100, 100, "TestWindow", "DirectXLibrary");
//	this->SetGraphMode(100, 100);
//}
//DirectXLib::~DirectXLib() {
//
//}