package org.fastipc;

/**
 * ����������������ͻ���������д��ʱ���ᴥ����<br/>
 * Դ�룺https://github.com/washheart/fastipc<br/>
 * Դ�룺http://git.oschina.net/washheart/fastipc<br/>
 * ˵����https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public interface FastIPCReadListener {

	void OnRead(int msgType, String packId, byte[] data);
}