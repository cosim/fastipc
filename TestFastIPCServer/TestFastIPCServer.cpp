// TestFastIPCServer.cpp : ����Ӧ�ó������ڵ㡣
//

#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#include "common/util/StrUtil.h"
#include "common/test/ipccom.h"
#include <thread>
#include <string>
#include <sstream>

extern std::wstring serverName;	// ���������
extern fastipc::Server server;  // IPC�����
extern DWORD blockSize;			// һ�δ���ʱ�����ݴ�С

LPTSTR getWindowTitle(){ return L"IPC����֮�����"; }
std::wstring getClientExe(){
	// �õ���������Ŀ¼
	TCHAR   szPath[1000];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	std::wstring path(szPath);
	path = path.substr(0, path.find_last_of('\\') + 1);
	path.append(L"TestFastIPCClient.exe");
	return path;
}
DWORD startClientProcess(std::wstring clientExe, std::wstring serverName, DWORD blockSize){
	STARTUPINFO sui;
	ZeroMemory(&sui, sizeof(STARTUPINFO));
	sui.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi; // �������������ӽ��̵���Ϣ
	DWORD dwCreationFlags = CREATE_NEW_PROCESS_GROUP;
	// ����������
	std::wstringstream wss; wss << clientExe << L" " << serverName << L" " << blockSize;
	if (CreateProcess(NULL, LPTSTR(wss.str().c_str()), // ��һ�������ÿգ���ִ���ļ��������зŵ�һ���������������
		NULL, NULL, FALSE, dwCreationFlags, NULL, NULL, &sui, &pi)){
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return pi.dwThreadId;
	}
	return 0;
}

// ��������
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow){
	blockSize = 2048;// 
	blockSize = 2;// ָ��һ����С�Ļ������ռ䣬�Ա�������ݵķֶδ���

	serverName = fastipc::genServerName();
	std::wstring clientExee = getClientExe();
	if (_waccess(LPTSTR(clientExee.c_str()), 0) != 0){
		serverName.append(L"�ļ��Ҳ�����");
		MessageBox(NULL, LPTSTR(serverName.c_str()), L"��Ϣ", 0);// ����ļ�������
		return 0;
	}
	if (server.create(serverName, blockSize) != 0){
		MessageBox(NULL, L"����IPC������ʧ��", L"��Ϣ", 0);
		return -1;
	}
	else{
		if (startClientProcess(clientExee, serverName, blockSize) == 0){// �����ӽ��̣�ʧ�ܺ󷵻�
			return -1;
		}
	}
	return createWin(hInstance);
}