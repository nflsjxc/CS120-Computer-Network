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

	WSAData wsd;           //初始化信息
	SOCKET soRecv;              //接收SOCKET
	char* pszRecv = NULL; //接收数据的数据缓冲区指针
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siRemote, siLocal;    //远程发送机地址和本机接收机地址

	//启动Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		cout << "WSAStartup Error = " << WSAGetLastError() << endl;
		return 0;
	}

	//创建socket
	soRecv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soRecv == SOCKET_ERROR) {
		cout << "socket Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//设置端口号
	int nPort = IN_PORT;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr(IP_ADDR.c_str());

	//绑定本地地址到socket
	if (::bind(soRecv, (SOCKADDR*)&siLocal, sizeof(siLocal)) == SOCKET_ERROR) {
		cout << "bind Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//申请内存
	pszRecv = new char[256];
	if (pszRecv == NULL) {
		cout << "pszRecv new char Error " << endl;
		return 0;
	}

	for (;;) {
		this_thread::sleep_for(chrono::milliseconds(10));//????
		dwSendSize = sizeof(siRemote);
		//开始接受数据
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
	//关闭socket连接
	closesocket(soRecv);
	delete[] pszRecv;
	msgman_thread.join();
	//清理
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

	WSAData wsd;           //初始化信息
	SOCKET soSend;         //发送SOCKET
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siLocal;    //远程发送机地址和本机接收机地址

	//启动Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {/*进行WinSocket的初始化,
		windows 初始化socket网络库，申请2，2的版本，windows socket编程必须先初始化。*/
		cout << "WSAStartup Error = " << WSAGetLastError() << endl;
		return 0;
	}

	//创建socket

	//AF_INET 协议族:决定了要用ipv4地址（32位的）与端口号（16位的）的组合
	//SOCK_DGRAM --  UDP类型，不保证数据接收的顺序，非可靠连接；
	soSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soSend == SOCKET_ERROR) {
		cout << "socket Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//设置端口号
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
		//开始发送数据
		//发送数据到指定的IP地址和端口
		nRet = sendto(soSend, msgtosend.c_str(), msgtosend.size(), 0, (SOCKADDR*)&siLocal, sizeof(SOCKADDR));
		//nRet = sendto(soSend, "123 mutouren", strlen("123 mutouren"), 0, (SOCKADDR*)&siLocal, sizeof(SOCKADDR));
		if (nRet == SOCKET_ERROR) {
			cout << "sendto Error " << WSAGetLastError() << endl;
			break;
		}
	}
	//关闭socket连接
	closesocket(soSend);
	//清理
	WSACleanup();

	return 0;
}

/*
int myserver_test(void)
{
	WSAData wsd;           //初始化信息
	SOCKET soRecv;              //接收SOCKET
	char* pszRecv = NULL; //接收数据的数据缓冲区指针
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siRemote, siLocal;    //远程发送机地址和本机接收机地址

	//启动Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		cout << "WSAStartup Error = " << WSAGetLastError() << endl;
		return 0;
	}

	//创建socket
	soRecv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soRecv == SOCKET_ERROR) {
		cout << "socket Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//设置端口号
	int nPort = PORT;

	//int nPort = 1234;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr("127.0.0.1");

	//绑定本地地址到socket
	if (::bind(soRecv, (SOCKADDR*)&siLocal, sizeof(siLocal)) == SOCKET_ERROR) {
		cout << "bind Error = " << WSAGetLastError() << endl;
		return 1;
	}

	//申请内存
	pszRecv = new char[4096];
	if (pszRecv == NULL) {
		cout << "pszRecv new char Error " << endl;
		return 0;
	}

	for (int i = 0; i < 30; i++) {
		dwSendSize = sizeof(siRemote);
		//开始接受数据
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
	//关闭socket连接
	closesocket(soRecv);
	delete[] pszRecv;

	//清理
	WSACleanup();
	return 0;
}
*/