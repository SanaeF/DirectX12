#include "FPSManager.h"
#include <iostream>
#include <sstream>
#pragma warning(disable : 4996)
namespace lib {
	FPSManager::FPSManager(int fps) {
		m_Min_frame = 1.0f / fps;
		m_Frame = 0;
		m_Fps = 0;
		m_Is_use = true;
		if (QueryPerformanceFrequency(&m_Time_freq) == FALSE)m_Is_use = false;
		if (m_Is_use) {
			QueryPerformanceCounter(&m_Time_start);
			//QueryPerformanceCounter(&m_Time_freq);
		}
	}
	void FPSManager::update() {
		if (!m_Is_use)return;
		// (���̎��� - �O�t���[���̎���) / ���g�� = �o�ߎ���(�b�P��)
		QueryPerformanceCounter(&m_Time_end);
		m_Frame = static_cast<float>(m_Time_end.QuadPart - m_Time_start.QuadPart) / static_cast<float>(m_Time_freq.QuadPart);
		if (m_Frame < m_Min_frame) { // ���Ԃɗ]�T������
			DWORD sleepTime = static_cast<DWORD>((m_Min_frame - m_Frame) * 1000);//�~���b�ɕϊ�
			timeBeginPeriod(1);//����\���グ��
			Sleep(sleepTime); //�Q��
			timeEndPeriod(1); //�߂�
			return;
		}
		if (m_Frame > 0.0) { // �o�ߎ��Ԃ�0���傫��(�������Ȃ��Ɖ��̌v�Z�Ń[�����Z�ɂȂ�Ǝv���)
			m_Fps = (m_Fps * 0.99f) + (0.01f / m_Frame); // ����fps���v�Z
		}
		m_Time_start = m_Time_end; // ����ւ�
	}
	void FPSManager::draw() {
		if (!m_Is_use)return;
	}
	void FPSManager::drawLog() {
		if (!m_Is_use)return;
		std::stringstream stream;
		stream << m_Fps << " FPS" << std::endl;
		char* fp = new char[40];
		sprintf(fp, "\nFPS: %lf", m_Fps);
		// �J�E���^�t����10���1��o�́A�Ƃ��ɂ��Ȃ��ƌ��Â炢������
		OutputDebugString(fp);
	}
}