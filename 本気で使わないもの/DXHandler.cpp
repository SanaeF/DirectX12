#include <iostream>
#include "DXHandler.h"
#include<d3dx12.h>
#include<cassert>
#include<d3dcompiler.h>
#include<string>
#include<algorithm>

 
std::map<int,void*>DXHandler::mHandler;

int DXHandler::next = 0;

int DXHandler::CreateHandle(void* data, int *HandleName) {
	if (CheckHandleID(HandleName)) {
		*HandleName = CreateHandler(data);

		return 0;
	}
	else {
		return -1;
	}
	return -1;//�G���[
}

int DXHandler::CreateHandler(void* data) {
	int HandleID = next;

	mHandler[HandleID] = &data;//�f�[�^������
	next++;
	return HandleID;
}


bool DXHandler::CheckHandleID(int* pHandleName) {
	if (pHandleName == 0) {
		return true;
	}
	return false;
}

std::map<int, void*> DXHandler::getHandleData() {

	return mHandler;
}