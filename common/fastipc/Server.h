//////////////////////////////////////////////////////////////////////////  
/// ����fastipc�ķ�����
/// �������ֻ�������Ŀͻ��˵�д��Ϣ��
/// �����Ҫ˫��ͨ�ţ���ô�����(S1)��Ӧ�Ŀͻ���(C1)��Ҫ����һ������ˣ�S2����Ȼ������(S1)����һ���ͻ���(C2)����д��
/// ��S1����C1��д�¼���S2����C2��д�¼���
/// ������÷����������£�create��serListerner��startRead��close��
///  
/// @version	1.0 
/// @author		washheart@163.com
/// @date       2015-10-22  
////////////////////////////////////////////////////////////////////////// 
#pragma once
#ifndef FastIPC_Server_H
#define FastIPC_Server_H

//#ifdef EXPORTS 
//#define EXPORTS __declspec(dllexport)  
//#else  
//#define EXPORTS __declspec(dllimport)  
//#endif  

#include <Windows.h>
#include "FastIPC.h"
//_declspec(dllexport) int add(int a, int b)
//{
//	return a + b;
//} 
namespace fastipc{
	class _declspec(dllexport) ReadListener{
	public:
		virtual void onRead(MemBuff*	memBuf){}
	};

	class  _declspec(dllexport) Server{
	public:
		Server();
		~Server();

	private:
		HANDLE		evtWrited;		// ����һ���¼���д��������ɣ����Զ���
		HANDLE		evtReaded;		// ����һ���¼�������������ɣ�����д��
		HANDLE		mapFile;		// �ڴ�ӳ���ļ����
		MemBuff*	memBuf;			// �������ڴ�����ݽṹ
		ReadListener* listener;		// �������¼��������д���
	public:
		/// ����������
		/// @param serverName ����������
		/// @return 0=�ɹ�������ֵ��ʾʧ�ܣ����峣���μ�FastIPC.h�е�ERR_*����
		int	 create(const std::wstring serverName);

		/// �رշ�����
		void close(void);

		/// Ϊ����������һ�����������Ա��ȡ���ݲ����д���
		/// @param listener һ��������
		void setListener(ReadListener* listener);

		/// ��ʼ�������ݳɹ���ȡ�󣬻�ص�listener.onRead��
		/// ע��˷�����������ǰ�̡߳�
		void startRead();

		/// ������״̬�Ƿ���׼����
		/// @return true=׼����
		bool isStable();
	};
}
#endif