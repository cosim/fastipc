//////////////////////////////////////////////////////////////////////////  
/// ����IPCͨ���õĿͻ���
/// 
/// Դ�룺http://git.oschina.net/washheart/fastipc
/// Դ�룺https://github.com/washheart/fastipc
/// ˵����https://github.com/washheart/fastipc/wiki
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
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
extern DWORD blockSize;			// һ�δ���ʱ�����ݴ�С

// ��������
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int iCmdShow){
	//MessageBox(NULL, L"����ʱ���ڸ��ӽ��̣�", L"��Ϣ", 0);
	int argc = 0;
	LPTSTR * args = CommandLineToArgvW(szCmdLine, &argc);
	if (argc == 0){
		MessageBox(NULL, L"����IPCʱ����������������ָ������˵����ƣ�", L"��Ϣ", 0);
		return -1;
	}
	serverName.append(args[0]);
	if (argc > 1){
		blockSize = jw::parseLong(args[1]);
		if (blockSize == 0){
			MessageBox(NULL, L"����IPCʱָ���Ŀ��СӦ����0��", L"��Ϣ", 0);
			return -1;
		}
	}	
	if (client.create(serverName, blockSize) != 0){
		MessageBox(NULL, L"����IPC�ͻ���ʧ�ܣ�", L"��Ϣ", 0);
		return -1;
	}	
	if (server.create(fastipc::genServerName(serverName),blockSize) != 0){
		MessageBox(NULL, L"��IPC�ͻ��˴����µķ�����ʧ�ܣ�", L"��Ϣ", 0);
		return -1;
	}
	return createWin(hInstance);
}