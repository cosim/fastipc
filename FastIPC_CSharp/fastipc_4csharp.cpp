#include "fastipc_4csharp.h"
#include "common/util/StrUtil.h"
#include "common/fastipc/Server.h"
#include "common/fastipc/Client.h"
#ifdef JWebTopLog
#include "common/tests/TestUtil.h"
#endif
using namespace std;

// ��wstringתΪchar*���ظ�csharp��
char* wstring2char(wstring ws){
	string s = jw::w2s(ws);
	char* c;
	const int len = s.length();
	c = new char[len + 1];
	//strcpy(c, s.c_str());
	memcpy(c, s.c_str(), len);
	c[len] = '\0';
	return c;
}

class CSharpIPCReadListener :public fastipc::ReadListener{// ���ﲻ����RebuildedBlockListener�����ݵ���װ����csharp������ȥ��
private:
	CSharpCallBack g_CSharpCallBack;  //�ظ���Ϣ�Ļص�����
public:
	void setListener(CSharpCallBack listener){
		g_CSharpCallBack = listener;
	}
	// ���з���˶�ȡ�����ݺ󣬻���ô˷���֪ͨ�ȴ��߽��д���
	// memBlock�ڷַ�����ɷ��������٣��ⲿ�����������������
	void onRead(fastipc::MemBlock* memBlock) override{
		if (!g_CSharpCallBack)return;
		// FIXME:packId��userShortValue�Ƿ�Ҳ���Բ������½���ֱ�Ӵ���ȥ�ã�
		//LPSTR packId = NULL;
		char * packId = NULL;
		if (memBlock->msgType != MSG_TYPE_NORMAL){
			packId = (char *)malloc(PACK_ID_LEN + 1);
			memcpy(packId, memBlock->packId, PACK_ID_LEN);
			packId[PACK_ID_LEN] = '\0';
			//string pid = memBlock->getPackId();
			//packId = LPSTR(pid.c_str());
		}
		//LPSTR userShortValue = NULL;
		//userShortValue = (char *)malloc(PACK_ID_LEN + 1);
		//memcpy(userShortValue, memBlock->userShortStr, PACK_ID_LEN);
		//userShortValue[PACK_ID_LEN] = '\0';
		// FIXME:data�Ƿ�Ҳ���Բ������½�����len��Ϊ��������
		int len = memBlock->dataLen;
		char * data = NULL;
		data = (char *)malloc(len);
		memcpy(data, memBlock->data, len);

		g_CSharpCallBack(memBlock->msgType, packId, data);
	}
};


/*
* �÷������ڴ���һ��fastipc�����
* serverName	���������
* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
* result		�������������Ƿ񴴽��ɹ���0=�ɹ�
* return		������Server��ָ��
*/
EXPORT long WINAPI nCreateServer(LPTSTR serverName, int blockSize, int& result){
	fastipc::Server * server = new fastipc::Server();
	result = server->create(serverName, blockSize);
	return (int)server;
}
EXPORT void WINAPI nAbc(){

}
/*
* �÷������ڿ���fastipc����˵Ķ�ȡ�������ɹ���������ڴ˴�
* nativeServer	ָ������ʵ����ָ��
* listener		��Ӧ��ȡ������ָ��
*/
EXPORT void WINAPI nStartRead(int nativeServer, CSharpCallBack callBack){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	CSharpIPCReadListener * ipcListener = new CSharpIPCReadListener();
	ipcListener->setListener(callBack);
	server->setListener(ipcListener);
	server->startRead();
}
/*
* �÷������ڹر�fastipc����ˣ�ִ�к�nStartRead��������ֹͣ
* nativeServer	ָ������ʵ����ָ��
*/
EXPORT void WINAPI nCloseServer(int nativeServer){
	fastipc::Server * server = (fastipc::Server *) nativeServer;
	server->close();
}

/*
* �÷������ڴ���һ��fastipc�ͻ���
* serverName	�ͻ��˶�Ӧ�ķ���˵�����
* blockSize		�������ͻ���ͨ��ʱÿ�ε�������
* result		�������������Ƿ񴴽��ɹ���0=�ɹ�
* return		������Client��ָ��
*/
EXPORT int WINAPI nCreateClient(LPTSTR serverName, int blockSize, int& result){
	fastipc::Client * client = new fastipc::Client();
	result = client->create(serverName, blockSize);
	return (int)client;
}

/*
* �÷���������fastipc�ͻ���д�����ݣ�����д��ɺ󣬻ᴥ������˵�����
* nativeClient	ָ��ͻ���ʵ����ָ��
* data			Ҫд�������
* return		�ɹ�д������ݳ���
*/
EXPORT int WINAPI nWriteClient(int nativeClient, LPTSTR data){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	char * shortStr = NULL;
	char * str = NULL;
	int len = 0;
	// data
	if (data != NULL){
		char*  tmp = wstring2char(data);
		len = lstrlenA(tmp);
		str = new char[len + 1];
		memcpy(str, tmp, len);
		delete tmp;
		str[len] = '\0';
	}	
	// д���ͻ��ˣ�������ǰ������ڴ�
#ifdef JWebTopLog
	std::wstringstream wss;
	wss << L"Writed "
		<< L" userMsgType=" << userMsgType
		<< L" userValue=" << userValue;
	if (shortStr != NULL)wss << L" userShortStr=" << shortStr;
	if (str != NULL)wss << L" data=" << str;
	wss << L"||\r\n";
	writeLog(wss.str());
#endif
	client->write(str, len);
	delete[] str;
	delete[] shortStr;
	return 0;
}

/*
* �÷������ڹر�fastipc�ͻ���
* nativeClient	ָ��ͻ���ʵ����ָ��
*/
EXPORT void WINAPI nCloseClient(int nativeClient){
	fastipc::Client * client = (fastipc::Client *) nativeClient;
	client->close();
}