#include "Client.h"
#include "common/util/StrUtil.h"
namespace fastipc{
	Client::Client(void){
		evtWrited = NULL;
		evtReaded = NULL;
		mapFile = NULL;
		memBuf = NULL;
	};
	Client::~Client(void){
		close();
	};
	void Client::close(void){
		if (evtWrited) {// �ر��¼����
			HANDLE handle = evtWrited;
			evtWrited = NULL;
			CloseHandle(handle);
		}
		if (evtReaded) {// �ر��¼����
			HANDLE handle = evtReaded;
			evtReaded = NULL;
			CloseHandle(handle);
		}
		if (memBuf) {// ȡ���ڴ�ӳ��
			MemBuff *pBuff = memBuf;
			memBuf = NULL;
			UnmapViewOfFile(pBuff);
		}
		if (mapFile) {// �ر�ӳ���ļ�
			HANDLE handle = mapFile;
			mapFile = NULL;
			CloseHandle(handle);
		}
	};
	bool Client::isStable(){
		return memBuf != NULL;
	};
	int	Client::create(const std::wstring serverName/*���������ƣ����ݴ����������¼����ƣ�*/){
		// ���������¼��ֱ�����֪ͨ�ɶ�����д
		evtWrited = OpenEvent(EVENT_ALL_ACCESS, FALSE, LPTSTR(genWritedEventName(serverName).c_str()));
		if (evtWrited == NULL || evtWrited == INVALID_HANDLE_VALUE)  return ERR_EventCreate_W;
		evtReaded = OpenEvent(EVENT_ALL_ACCESS, FALSE, LPTSTR(genReadedEventName(serverName).c_str()));
		if (evtReaded == NULL || evtReaded == INVALID_HANDLE_VALUE) return ERR_EventCreate_R;
		// ���ڴ�ӳ���ļ�
		mapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (genMappingFileName(serverName).c_str()));
		if (mapFile == NULL || mapFile == INVALID_HANDLE_VALUE)return ERR_MappingOpen;
		memBuf = (MemBuff*)MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MemBuff));
		if (memBuf == NULL) { return ERR_MappingMap; }
		return 0;
	};

	DWORD Client::write(char *pBuff, DWORD len){
		if (!memBuf)return ERR_ClientCreate;
		if (len <= MEM_SIZE)return writeBlock(pBuff, len, NULL, MSG_TYPE_NORMAL); // ����һ����д��
		DWORD idx = 0, tmp = len%MEM_SIZE;
		DWORD result = -1;
		len = len - tmp;
		char * id = jw::GenerateGuid();
		len = len - MEM_SIZE;// ���һ�Σ�������whileѭ�����ж��Ƿ����������ݰ�
		while (idx < len){// �����ݷ�Ϊ�������д
			result = writeBlock(pBuff + idx, MEM_SIZE, id, MSG_TYPE_PART);
			if (result != 0)return result;
			idx += MEM_SIZE;
		}
		if (tmp == 0){// ���ñ���Ϊ������������ݰ�
			result = writeBlock(pBuff + len, MEM_SIZE, id, MSG_TYPE_END); // �������һ�������Լ��������
		}
		else{
			result = writeBlock(pBuff + len, MEM_SIZE, id, MSG_TYPE_PART); // ���͵����ڶ������������Լ��������
			result = writeBlock(pBuff + len + MEM_SIZE, tmp, id, MSG_TYPE_END); //����ʣ��İ����Լ��������
		}
		delete id;
		return 0;
	}

	DWORD Client::writeBlock(char *pBuff, DWORD len, char* packId, int msgType){
	writeAble:
		if (memBuf->state == MEM_CAN_WRITE){
			InterlockedCompareExchange(&memBuf->state, MEM_IS_BUSY, MEM_CAN_WRITE);// ͨ��ԭ�Ӳ��������ù�������״̬Ϊæµ״̬
			if (memBuf->state != MEM_IS_BUSY)goto waitForWrite;	// ������ú���æµ״̬����ô�����������߳��ڲ������ݣ���ʱ�����ȴ� 
			memBuf->dataLen = len;
			memBuf->msgType = msgType;
			memcpy(memBuf->data, pBuff, len);
			if (msgType > MSG_TYPE_NORMAL){
				ZeroMemory(memBuf->packId, PACK_ID_LEN);
				memcpy(memBuf->packId, packId, min(PACK_ID_LEN, lstrlenA(packId)));
			}
			InterlockedExchange(&memBuf->state, MEM_CAN_READ);	// �����ڴ�״̬Ϊ�ɶ�
			SetEvent(evtWrited);								// �����¼���֪ͨ�ȴ��̣߳����Զ���
		}
		else{
		waitForWrite:
			if (WaitForSingleObject(evtReaded, INFINITE) == WAIT_OBJECT_0){
				goto writeAble;// �ȴ���д�¼��ĵ�����Ȼ��ȥд
			}
			else{
				return ERR_EventWait_W;
			}
		}
		return 0;
	};
}