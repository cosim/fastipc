package org.fastipc;

/**
 * FastIPC�Ŀͻ��ˡ�<br/>
 * Դ�룺https://github.com/washheart/fastipc<br/>
 * Դ�룺http://git.oschina.net/washheart/fastipc<br/>
 * ˵����https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public final class FastIPCClient {
	private int nativeClient = 0;// ָ��fastipc::Clientʵ����ָ��

	public FastIPCClient() {}

	public void create(String serverName, int blockSize) {
		nativeClient = FastIPCNative.createClient(serverName, blockSize);
	}

	public void write(String data) {
		FastIPCNative.write(nativeClient, data);
	}

	public void close() {
		FastIPCNative.closeClient(nativeClient);
	}
}