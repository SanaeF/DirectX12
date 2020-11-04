#pragma once
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

#include<Windows.h>
#include <dsound.h>
#include <vector>
#include<map>
enum SOUND {
	eDXSOUND_NORMAL,
	eDXSOUND_LOOP,
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

	//�T�E���h�t�@�C����ǂݍ���
	int LoadFile(const char* file_name);

	void SetVolume(int num, int Handle);

	//�n���h���̃T�E���h���Đ�����
	void Play(int Handle, int Type);

	//�n���h���̃T�E���h���~����
	void Stop(int Handle);

	//�n���h���f�[�^�̃T�E���h���������������
	void InitMem(int Handle);

	//�T�E���h��S���������������
	void InitMem();
};