#pragma once
#include<d3d12.h>
#include<map>
#include<wrl.h>
#include<memory>

#include "DxPMDManager.h"
 
class DXHandler {
private:

	//static std::map<int, void*>::iterator it;
	//static std::map<std::string,int>::iterator it2;
	static std::map<int, void*>mHandler;

	static int next;
	

public:

	int CreateHandle(void* data,int* HandleName);

	//�n���h����o�^�����ԍ���Ԃ�
	int CreateHandler(void* data);

	std::map<int, void*>getHandleData();

	DXHandler(){

	}

private:


	//�A�h���X��ID�����݂��邩���m�F����
	bool CheckHandleID(int* pHandleName);
};