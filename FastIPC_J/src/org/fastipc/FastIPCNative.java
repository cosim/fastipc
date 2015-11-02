package org.fastipc;

import java.util.UUID;

/**
 * FastIPC��native�������塣<br/>
 * Դ�룺https://github.com/washheart/fastipc<br/>
 * Դ�룺http://git.oschina.net/washheart/fastipc<br/>
 * ˵����https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public class FastIPCNative {
	public static int // ������Ϣ���ͳ���
			MSG_TYPE_NORMAL = 0, // ��������Ϣ
			MSG_TYPE_PART = 1,// ����Ϣ��һ�������ݵ�һ����
			MSG_TYPE_END = 2; // ����Ϣ��һ�������ݵĽ�β

	static int//
				// ERR_ServerCreate = 100, // ����������ʧ��
				// ERR_ClientCreate = 101, // �ͻ��˴���ʧ��
				// ERR_EventCreate_W = 201, // ����������д�¼�ʧ��
				// ERR_EventCreate_R = 202, // �������������¼�ʧ��
				// ERR_EventOpen_W = 203, // �ͻ��˴�д�¼�ʧ��
				// ERR_EventOpen_R = 204, // �ͻ��˴򿪶��¼�ʧ��
				// ERR_EventWait_W = 205,// �ͻ��˵ȴ���д�¼�ʧ��
				// ERR_EventWait_R = 206, // �������ȴ��ɶ��¼�ʧ��
				// ERR_MappingCreate = 301,// ����������ӳ���ļ�ʧ��
				// ERR_MappingOpen = 302, // ���ͻ��˴�ӳ���ļ�ʧ��
				// ERR_MappingMap = 303, // ������/�ͻ���ӳ���ڴ��ļ�ʧ��
			ERR_ServerName = 1, // �ͻ��˴���ʧ��
			ERR_NONE = 0;// û���κδ���

	private native static int[] nCreateServer(String serverName, int blockSize);

	private native static void nStartRead(int nativeServer, FastIPCReadListener listener);

	private native static void nCloseServer(int nativeServer);

	private native static int[] nCreateClient(String serverName, int blockSize);

	private native static int nWriteClient(int nativeClient, String data);

	private native static void nCloseClient(int nativeClient);

	static int createServer(String serverName, int blockSize) {
		FastIPCNative.checkServerName(serverName);
		int[] rtn = FastIPCNative.nCreateServer(serverName, blockSize);
		if (rtn[0] != FastIPCNative.ERR_NONE) {
			throw new FastIPCException("���������ʧ�ܣ�" + rtn + "����");
		}
		return rtn[1];
	}

	static void closeServer(int nativeServer) {
		FastIPCNative.nCloseServer(nativeServer);
	}

	static void startRead(int nativeServer, FastIPCReadListener listener) {
		nStartRead(nativeServer, listener);
	}

	static int createClient(String serverName, int blockSize) {
		FastIPCNative.checkServerName(serverName);
		int[] rtn = FastIPCNative.nCreateClient(serverName, blockSize);
		if (rtn[0] != FastIPCNative.ERR_NONE) {
			throw new FastIPCException("���������ʧ�ܣ�" + rtn + "����");
		}
		return rtn[1];
	}

	private static void checkServerName(String serverName) {
		if (serverName == null) {
			throw new FastIPCException("��ָ�����������ƣ�" + FastIPCNative.ERR_ServerName + "����");
		} else {
			int len = serverName.length();
			if (len == 0) throw new FastIPCException("��ָ�����������ƣ�" + FastIPCNative.ERR_ServerName + "����");
			for (int i = 0; i < len; i++) {
				char c = serverName.charAt(i);
				if (c == '-') continue;
				if ('0' <= c || c >= '-') continue;
				if ('A' <= c || c >= 'Z') continue;
				if ('a' <= c || c >= 'z') continue;
				throw new FastIPCException("���������ư�����Ч�ַ���" + FastIPCNative.ERR_ServerName + "����");
			}
		}
	}

	static void write(int nativeClient, String data) {
		nWriteClient(nativeClient, data);
	}

	public static String genServerName() {
		return UUID.randomUUID().toString();
	}

	public static String genServerName(String serverName) {
		return serverName + "_c";
	}

	static void closeClient(int nativeClient) {
		nCloseClient(nativeClient);
	}
}