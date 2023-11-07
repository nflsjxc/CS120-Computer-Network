#include "comm.h"
#include <JuceHeader.h>
//#pragma comment(lib, "Ws2_32.lib")

using namespace juce;
bool exit_flag_in, exit_flag_out;
void msgmanager_in(dataloader* dl, CriticalSection* lock, Array<int8_t>* buffer)
{
	for (;;)
	{
		if (exit_flag_in)break;
		{
			const ScopedLock sl(*lock);
			while (buffer->size() >= 13)
			{
				//cout << "MSG IN Buffer flushed\n";
				Array<int8_t> _13bytes;
				for (int i = 0; i < 13; i++)
				{
					_13bytes.add((*buffer)[i]);
					//cout << (*buffer)[i] << ' ';
				}
				buffer->removeRange(0, 13);
				dl->add13bytes(_13bytes);
			}
		}
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

void msgmanager_out(datareceiver* dr, CriticalSection* lock, Array<int8_t>* buffer)
{
	for (;;)
	{
		if (exit_flag_out)break;
		{
			const ScopedLock sl(*lock);
			while (!dr->isempty())
			{
				//cout << "MSG OUT Buffer flushed\n";
				Array<int8_t> _13bytes = dr->get13bytes();
				dr->pop13bytes();
			/*	for (int i = 0; i < _13bytes.size(); i++)
				{
					cout << _13bytes[i] << ' ';
				}*/
				buffer->addArray(_13bytes);
			}
		}
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

int myserver(dataloader* dl)
{
	cout << "(IP) UDP Comm server start\n" << "PORT: " << IN_PORT << '\n';
	exit_flag_in = false;
	CriticalSection lock;
	Array<int8_t> buffer;
	thread msgman_thread(&msgmanager_in, dl, &lock, &buffer);

	WSAData wsd;           //��ʼ����Ϣ
	SOCKET soRecv;              //����SOCKET
	char* pszRecv = NULL; //�������ݵ����ݻ�����ָ��
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siRemote, siLocal;    //Զ�̷��ͻ���ַ�ͱ������ջ���ַ

	//����Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		cout << "WSAStartup Error = " << WSAGetLastError() << endl;
		return 0;
	}

	//����socket
	soRecv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soRecv == SOCKET_ERROR) {
		cout << "socket Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//���ö˿ں�
	int nPort = IN_PORT;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr(IP_ADDR.c_str());

	//�󶨱��ص�ַ��socket
	if (::bind(soRecv, (SOCKADDR*)&siLocal, sizeof(siLocal)) == SOCKET_ERROR) {
		cout << "bind Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//�����ڴ�
	pszRecv = new char[256];
	if (pszRecv == NULL) {
		cout << "pszRecv new char Error " << endl;
		return 0;
	}

	for (;;) {
		this_thread::sleep_for(chrono::milliseconds(10));//????
		dwSendSize = sizeof(siRemote);
		//��ʼ��������
		nRet = recvfrom(soRecv, pszRecv, 256, 0, (SOCKADDR*)&siRemote, &dwSendSize);
		if (nRet == SOCKET_ERROR) {
			cout << "recvfrom Error " << WSAGetLastError() << endl;
			break;
		}
		else if (nRet == 0) {
			break;
		}
		else {
			pszRecv[nRet] = '\0';
			//cout << inet_ntoa(siRemote.sin_addr) << ": " << pszRecv << "\n";
				
			string recvmsg(pszRecv);
			cout << "(IP) "<<inet_ntoa(siRemote.sin_addr) << ": " << recvmsg << "\n";
			{
				const ScopedLock sl(lock);
				for (int i = 0; i < recvmsg.size(); i++)
				{
					buffer.add((int8_t)recvmsg[i]);
				}
			}

		}

	}
	//�ر�socket����
	closesocket(soRecv);
	delete[] pszRecv;
	msgman_thread.join();
	//����
	WSACleanup();
	return 0;
}

int myclient(datareceiver* dr)
{
	cout << "(IP) UDP Comm client start\n"<<"PORT: "<<OUT_PORT<<'\n';
	exit_flag_out = false;
	CriticalSection lock;
	Array<int8_t> buffer;
	thread msgman_thread(&msgmanager_out, dr, &lock, &buffer);

	WSAData wsd;           //��ʼ����Ϣ
	SOCKET soSend;         //����SOCKET
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siLocal;    //Զ�̷��ͻ���ַ�ͱ������ջ���ַ

	//����Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {/*����WinSocket�ĳ�ʼ��,
		windows ��ʼ��socket����⣬����2��2�İ汾��windows socket��̱����ȳ�ʼ����*/
		cout << "WSAStartup Error = " << WSAGetLastError() << endl;
		return 0;
	}

	//����socket

	//AF_INET Э����:������Ҫ��ipv4��ַ��32λ�ģ���˿ںţ�16λ�ģ������
	//SOCK_DGRAM --  UDP���ͣ�����֤���ݽ��յ�˳�򣬷ǿɿ����ӣ�
	soSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soSend == SOCKET_ERROR) {
		cout << "socket Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//���ö˿ں�
	int nPort = OUT_PORT;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr(IP_ADDR.c_str());

	for (;;) {
		this_thread::sleep_for(chrono::milliseconds(10));//????
		//string msgtosend="test";

		string msgtosend;
		{
			const ScopedLock sl(lock);
			for (int i = 0; i < buffer.size(); i++)
			{
				msgtosend += buffer[i];
			}
			buffer.clear();
		}
		if (msgtosend.size() == 0)continue;
		//��ʼ��������
		//�������ݵ�ָ����IP��ַ�Ͷ˿�
		nRet = sendto(soSend, msgtosend.c_str(), msgtosend.size(), 0, (SOCKADDR*)&siLocal, sizeof(SOCKADDR));
		//nRet = sendto(soSend, "123 mutouren", strlen("123 mutouren"), 0, (SOCKADDR*)&siLocal, sizeof(SOCKADDR));
		if (nRet == SOCKET_ERROR) {
			cout << "sendto Error " << WSAGetLastError() << endl;
			break;
		}
	}
	//�ر�socket����
	closesocket(soSend);
	//����
	WSACleanup();

	return 0;
}

/*
int myserver_test(void)
{
	WSAData wsd;           //��ʼ����Ϣ
	SOCKET soRecv;              //����SOCKET
	char* pszRecv = NULL; //�������ݵ����ݻ�����ָ��
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siRemote, siLocal;    //Զ�̷��ͻ���ַ�ͱ������ջ���ַ

	//����Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		cout << "WSAStartup Error = " << WSAGetLastError() << endl;
		return 0;
	}

	//����socket
	soRecv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soRecv == SOCKET_ERROR) {
		cout << "socket Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//���ö˿ں�
	int nPort = PORT;

	//int nPort = 1234;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr("127.0.0.1");

	//�󶨱��ص�ַ��socket
	if (::bind(soRecv, (SOCKADDR*)&siLocal, sizeof(siLocal)) == SOCKET_ERROR) {
		cout << "bind Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//�����ڴ�
	pszRecv = new char[4096];
	if (pszRecv == NULL) {
		cout << "pszRecv new char Error " << endl;
		return 0;
	}

	for (int i = 0; i < 30; i++) {
		dwSendSize = sizeof(siRemote);
		//��ʼ��������
		nRet = recvfrom(soRecv, pszRecv, 4096, 0, (SOCKADDR*)&siRemote, &dwSendSize);
		if (nRet == SOCKET_ERROR) {
			cout << "recvfrom Error " << WSAGetLastError() << endl;
			break;
		}
		else if (nRet == 0) {
			break;
		}
		else {
			pszRecv[nRet] = '\0';
			cout << inet_ntoa(siRemote.sin_addr) << endl
				<< pszRecv << endl;
		}

	}
	//�ر�socket����
	closesocket(soRecv);
	delete[] pszRecv;

	//����
	WSACleanup();
	return 0;
}
*/