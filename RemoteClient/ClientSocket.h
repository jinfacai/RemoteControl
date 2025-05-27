#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define MAXCONN 1
class CClientSocket
{
public:
	static CClientSocket* GetInstance() {//静态局部变量，确保只创建一个实例
		if (m_instance == NULL) {//静态函数没有this指针无法直接访问成员变量
			m_instance = new CClientSocket(); // 创建一个新的实例
		}
		return m_instance; // 返回该实例的引用
	}
	bool StartServer() {
		client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a TCP socket
		if (client_sock == INVALID_SOCKET) {//windows失败返回 INVALID_SOCKET linux失败返回 -1
			WSAGetLastError(); // Get the last error code
			return FALSE; // Return FALSE if socket creation fails
		}
		// Set up the server address structure
		memset(&client_adr, 0, sizeof(client_adr));//serv_adr所有结构体成员初始化为0
		client_adr.sin_family = AF_INET; // Set address family to IPv4
		//网络通信规定必须使用大端序（网络字节序）
		if (inet_pton(AF_INET, "127.0.0.1", &client_adr.sin_addr) <= 0) {
			MessageBoxW(NULL, L"无效的IP地址，请检查网络设置！", L"IP地址错误", MB_OK | MB_ICONERROR);
			exit(1);
		}
		client_adr.sin_port = htons(8888); // Set the port number  16位转换
		return TRUE;
	}
	bool ConnectServer() {
		if (connect(client_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR) {
			MessageBoxW(NULL, L"连接失败，请检查网络设置！", L"连接错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
			return FALSE;
		}
		return TRUE;
	}
	bool SendtoServer() {
		if(send(client_sock, sendbuffer, sizeof(sendbuffer), 0) == SOCKET_ERROR) {// Echo the received data back to the client
			MessageBoxW(NULL, L"发送数据失败，请检查网络设置！", L"发送错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
			return FALSE;
		}
		return TRUE;
	}
	bool RecvfromServer(){
		if (recv(client_sock, recvbuffer, sizeof(recvbuffer), 0) == SOCKET_ERROR) {// Receive data from the client
			MessageBoxW(NULL, L"接收数据失败，请检查网络设置！", L"接收错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
			return FALSE;
		}
		return TRUE;
	}

private:
	//变量初始化
	static CClientSocket* m_instance;// 静态成员变量，用于存储单例实例
	SOCKET serv_sock;
	SOCKET client_sock;
	SOCKADDR_IN serv_adr, client_adr;
	//客户端buffer
	char sendbuffer[1024] = { 0 };
	char recvbuffer[1024] = { 0 };
	CClientSocket() {
		if (InitSocket() == FALSE) {
			MessageBoxW(NULL, L"无法初始化环境，请检查网络设置！", L"初始化错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标	
			exit(1); // 0正常退出 1异常退出
		}
	}

	~CClientSocket() {
		WSACleanup();
	}
	BOOL InitSocket() {
		WSADATA Data;// Initialize Winsock
		if (WSAStartup(MAKEWORD(2, 2), &Data) != 0) { //Winsock version 2.2 传入变量地址
			return FALSE; // Return FALSE if initialization fails
		}
		return TRUE; // Return TRUE if successful, FALSE otherwise	
	}

};

#pragma once
