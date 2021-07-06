#pragma once
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")

#include<Windows.h>
#include <dsound.h>
#include <vector>
#include<map>
namespace lib {
	enum SOUND_PATTERN {
		DX_SOUND_NORMAL,//���ʂɍĐ����܂�('��')
		DX_SOUND_LOOP,//���y�����[�v�Đ����܂�(�M�E�ցE�L)
	};
	class InputWavData;
	class DxSound {
	private:
		HWND m_Hwnd;
		IDirectSound8* m_pDx_s8;
		DSBUFFERDESC m_Dxs_buff_desc;
		std::map<int, LPDIRECTSOUNDBUFFER>m_pDx_sound_buffer;

		static int m_Handle_count;
	public:
		DxSound(HWND hwnd);
		~DxSound();
		//�T�E���h�t�@�C����ǂݍ��݂܂��B
		int loadFile(const char* file_name);
		//�T�E���h�̉���𒲐߂��܂�('�D')
		void setVolume(int num, int Handle);
		//�n���h���̃T�E���h���Đ����Ă���
		void play(int Handle, int Type);
		//�n���h���̃T�E���h���~���܂��B
		void stop(int Handle);
		//�n���h���f�[�^�̃T�E���h������������폜�I
		void initMem(int Handle);
		//�T�E���h��S������������폜�I
		void initMem();
	};
}