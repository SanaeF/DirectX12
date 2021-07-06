#include "DxWavData.h"
#include <dsound.h>
#include <stdio.h>


bool DxWavData::mWavProp() {
	riffck_info.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	// data�`�����N�ɐi������
	if (MMSYSERR_NOERROR != mmioDescend(
		mmio_handle,	// MMIO�n���h��
		&riffck_info,	// �i�������`�����N�̏��
		nullptr,	// �e�`�����N
		MMIO_FINDRIFF))// ���̎��
	{
		// �i�����s
		mmioClose(mmio_handle, MMIO_FHOPEN);
		return false;
	}
	return true;
}

bool DxWavData::mFmtProp() {
	ck_info.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (MMSYSERR_NOERROR != mmioDescend(mmio_handle, &ck_info, &riffck_info, MMIO_FINDCHUNK))
	{
		// fmt�`�����N���Ȃ�
		mmioClose(mmio_handle, MMIO_FHOPEN);
		return false;
	}

	read_size = mmioRead(
		mmio_handle,			// �n���h��
		(HPSTR)&mWaveData.WavFormat,	// �ǂݍ��ݗp�o�b�t�@
		sizeof(mWaveData.WavFormat));	// �o�b�t�@�T�C�Y

	if (read_size != sizeof(mWaveData.WavFormat))
	{
		// �ǂݍ��݃T�C�Y����v���ĂȂ��̂ŃG���[
		mmioClose(mmio_handle, MMIO_FHOPEN);
		return false;
	}

	// �t�H�[�}�b�g�`�F�b�N
	if (mWaveData.WavFormat.wFormatTag != WAVE_FORMAT_PCM)
	{
		// �t�H�[�}�b�g�G���[
		mmioClose(mmio_handle, MMIO_FHOPEN);
		return false;
	}

	if (mmioAscend(mmio_handle, &ck_info, 0) != MMSYSERR_NOERROR)
	{
		mmioClose(mmio_handle, 0);
		return false;
	}
	return true;
}

bool DxWavData::mDataProp() {
	ck_info.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(
			mmio_handle,
			&ck_info, 
			&riffck_info,
			MMIO_FINDCHUNK
		) != MMSYSERR_NOERROR)
	{
		// �i�����s
		mmioClose(mmio_handle, MMIO_FHOPEN);
		return false;
	}
	return true;
}

bool DxWavData::mLoadSound() {
	mWaveData.Size = ck_info.cksize;
	char* sound_buffer = new char[ck_info.cksize];
	read_size = mmioRead(mmio_handle, sound_buffer, ck_info.cksize);
	if (read_size != ck_info.cksize)
	{
		mmioClose(mmio_handle, MMIO_FHOPEN);
		delete[] sound_buffer;
		return false;
	}
	mWaveData.SoundBuffer = sound_buffer;
	return true;
}

int DxWavData::LoadFile(const char* file_name) {

	mmio_handle = mmioOpen(
		(char*)file_name,	// �t�@�C����
		nullptr,			// MMIO���
		MMIO_READ);		// ���[�h
	if (mmio_handle == nullptr)return -1;

	if (!mWavProp())return -1;
	if (!mFmtProp())return -1;
	if (!mDataProp())return -1;
	if (!mLoadSound())return -1;

	mmioClose(mmio_handle, MMIO_FHOPEN);

	return 0;
}
