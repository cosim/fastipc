package org.fastipc;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.WindowEvent;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.util.HashMap;
import java.util.Map;

import javax.swing.AbstractAction;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;

/**
 * ����FastIPC��java�����/�ͻ��ˡ�<br/>
 * ���ഴ��һ������ˣ�Ȼ������c++����TestFastIPCClient.exe������֮������<br/>
 * Դ�룺https://github.com/washheart/fastipc<br/>
 * Դ�룺http://git.oschina.net/washheart/fastipc<br/>
 * ˵����https://github.com/washheart/fastipc/wiki<br/>
 * 
 * @author washheart@163.com
 */
public class TestFastIPCServer {
	static void initDLL(String dir) throws Exception {
		File dllFile = new File(dir, "FastIPC.dll");
		if (!dllFile.exists()) {
			throw new Exception("DLL�ļ�[" + dllFile + "]������");
		}
		System.load(dllFile.getAbsolutePath());
		dllFile = new File(dir, "FastIPC_JNI.dll");
		if (!dllFile.exists()) {
			throw new Exception("DLL�ļ�[" + dllFile + "]������");
		}
		System.load(dllFile.getAbsolutePath());
	}

	private String serverName;
	private int blockSize;
	private FastIPCServer server;
	private FastIPCClient client;
	JFrame mainFrame = null;
	private JTextArea txtReceived = new JTextArea("��������ʾ���յ�����Ϣ"), txtSended = new JTextArea("��������ʾ�ѷ��͵���Ϣ");
	private JTextField txtMsg = new JTextField("����������Ҫ���͵���Ϣ");

	class MYReadListener implements FastIPCReadListener {
		Map<String, ByteArrayOutputStream> caches = new HashMap<String, ByteArrayOutputStream>();

		@Override
		public void OnRead(int msgType, String packId, byte[] data) {
			System.out.println("msgType = " + msgType + " packId = " + packId);
			try {
				if (msgType == FastIPCNative.MSG_TYPE_NORMAL) {
					received(new String(data, "utf-8"));
				} else {
					ByteArrayOutputStream bos = caches.get(packId);
					if (bos == null) {
						bos = new ByteArrayOutputStream();
						caches.put(packId, bos);
					}
					bos.write(data);
					if (msgType == FastIPCNative.MSG_TYPE_END) {
						caches.remove(packId);
						received(new String(bos.toByteArray(), "utf-8"));
					}
				}
			} catch (Throwable e) {
				e.printStackTrace();
			}
		}
	}

	void startServer() throws Exception {
		final String dir = new File("..\\Release\\").getCanonicalPath() + File.separator;
		final File clientExe = new File(dir + "TestFastIPCClient.exe");
		System.out.println("�ͻ��˿�ִ���ļ� = " + clientExe.getAbsolutePath());
		if (!clientExe.exists()) {
			throw new Exception("�ͻ��˿�ִ���ļ�[" + clientExe + "]������");
		}
		initDLL(dir);
		this.server = new FastIPCServer();
		this.serverName = FastIPCNative.genServerName();
		System.out.println("serverName = " + serverName);
		this.blockSize = 5;
		server.create(serverName, blockSize);
		server.setListener(new MYReadListener());
		new Thread() {
			@Override
			public void run() {
				server.startRead();
			}
		}.start();
		String[] cmds = { clientExe.getAbsolutePath(), serverName, Integer.toString(blockSize) };
		try {
			Runtime.getRuntime().exec(cmds, null, clientExe.getParentFile());
		} catch (Throwable e) {
			e.printStackTrace();
		}
	}

	public void received(String data) {
		txtReceived.setText(txtReceived.getText()//
				+ "���� " + data + "\r\n");
	}

	protected void wrtieMsgTo() {
		if (client == null) {
			client = new FastIPCClient();
			client.create(FastIPCNative.genServerName(serverName), blockSize);
		}
		client.write(txtMsg.getText());
		txtSended.setText(txtSended.getText()//
				+ "���� " + txtMsg.getText() + "\r\n");
	}

	void createView() {
		mainFrame = new JFrame() {
			@Override
			protected void processWindowEvent(WindowEvent e) {
				if (e.getID() == WindowEvent.WINDOW_CLOSING) {
					if (client != null) {
						client.close();
						client = null;
					}
					if (server != null) {
						server.close();
						server = null;
					}
				}
				super.processWindowEvent(e);
			}
		};
		JPanel toolPanel = new JPanel(new BorderLayout());
		toolPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		JPanel toolLeft = new JPanel(new FlowLayout(FlowLayout.LEFT, 5, 0));
		JPanel toolRight = new JPanel(new FlowLayout(FlowLayout.RIGHT, 5, 0));
		toolLeft.add(new JButton(new AbstractAction("��շ���") {
			@Override
			public void actionPerformed(ActionEvent e) {
				txtSended.setText("");
			}
		}));
		toolLeft.add(new JButton(new AbstractAction("��ս���") {
			@Override
			public void actionPerformed(ActionEvent e) {
				txtReceived.setText("");
			}
		}));
		toolRight.add(new JButton(new AbstractAction("������Ϣ") {
			@Override
			public void actionPerformed(ActionEvent e) {
				wrtieMsgTo();
			}
		}));
		toolPanel.add(toolLeft, BorderLayout.WEST);
		toolPanel.add(txtMsg, BorderLayout.CENTER);
		toolPanel.add(toolRight, BorderLayout.EAST);
		JSplitPane splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
		splitPane.add(new JScrollPane(txtSended), JSplitPane.TOP);
		splitPane.add(new JScrollPane(txtReceived), JSplitPane.BOTTOM);
		mainFrame.add(toolPanel, BorderLayout.NORTH);
		mainFrame.add(splitPane, BorderLayout.CENTER);
		mainFrame.setTitle("IPC����֮�����");
		mainFrame.setSize(800, 600);
		splitPane.setDividerLocation(300);
		mainFrame.setVisible(true);
		mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	}

	public static void main(String[] args) {
		TestFastIPCServer test = null;
		try {
			test = new TestFastIPCServer();
			test.createView();
			test.startServer();
		} catch (Throwable e) {
			e.printStackTrace();
			JOptionPane.showMessageDialog(test == null ? null : test.mainFrame, e.getMessage());
		} finally {}
	}
}