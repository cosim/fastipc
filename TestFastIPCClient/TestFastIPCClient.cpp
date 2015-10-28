
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include "common/test/ipccom.h"
#include <windows.h>
#include <thread>
#include <sstream>
#include <string>

LPTSTR getWindowTitle(){ return L"IPC����֮�ͻ���"; }

extern std::wstring serverName;	// ���������
extern fastipc::Server server;  // IPC�����
extern fastipc::Client client;

// ��������
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow){
	serverName.append(szCmdLine);
	if (serverName.size() == 0){
		MessageBox(NULL, L"����IPCʱ����������������ָ������˵����ƣ�", L"��Ϣ", 0);
		return -1;
	}
	if (client.create(serverName) != 0){
		MessageBox(NULL, L"����IPC�ͻ���ʧ�ܣ�", L"��Ϣ", 0);
		return -1;
	}	
	if (server.create(fastipc::genServerName(serverName)) != 0){
		MessageBox(NULL, L"��IPC�ͻ��˴����µķ�����ʧ�ܣ�", L"��Ϣ", 0);
		return -1;
	}
	return createWin(hInstance);
}