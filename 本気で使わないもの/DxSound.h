#pragma once
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

#include<Windows.h>
#include <dsound.h>
#include <vector>
#include<map>
enum SOUND {
	eDXSOUND_NORMAL,//���ʂɍĐ����܂�('��')
	eDXSOUND_LOOP,//���y�����[�v�Đ����܂�(�M�E�ցE�L)
};

class DxWavData;

class DxSound {
private:

	HWND mHwnd;

	IDirectSound8* pDxS8;

	DSBUFFERDESC mDxS_BuffDesc;

	std::map<int , LPDIRECTSOUNDBUFFER>pDxSoundBuffer;


	static int mHandleCount;

public:

	DxSound(HWND hwnd);
	~DxSound();

	//�T�E���h�t�@�C����ǂݍ��݂܂��B
	int LoadFile(const char* file_name);

	//�T�E���h�̉���𒲐߂��܂�('�D')
	void SetVolume(int num, int Handle);

	//�n���h���̃T�E���h���Đ����Ă���
	void Play(int Handle, int Type);

	//�n���h���̃T�E���h���~���܂��B
	void Stop(int Handle);

	//�n���h���f�[�^�̃T�E���h������������폜�I
	void InitMem(int Handle);

	//�T�E���h��S������������폜�I
	void InitMem();
};