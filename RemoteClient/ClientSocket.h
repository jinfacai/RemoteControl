#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define MAXCONN 1
class CClientSocket
{
public:

private:
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
		SOCKET client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a TCP socket	
		if (client_sock == INVALID_SOCKET) {//windows失败返回 INVALID_SOCKET linux失败返回 -1
			WSAGetLastError(); // Get the last error code
			return FALSE; // Return FALSE if socket creation fails
		}
		else {
			// Set up the server address structure
			//socket connect send recv close
			SOCKADDR_IN serv_adr; // Windows
			//sockaddr_in serv_adr; linux
			//serv_adr初始化
			memset(&serv_adr, 0, sizeof(serv_adr));//serv_adr所有结构体成员初始化为0
			serv_adr.sin_family = AF_INET; // Set address family to IPv4
			//网络通信规定必须使用大端序（网络字节序）
			//转换为一个 in_addr_t 类型（32 位整数）的函数
			//serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set the IP address to any available address 32位转换
			if (inet_pton(AF_INET, "127.0.0.1", &serv_adr.sin_addr) <= 0) {
				MessageBoxW(NULL, L"无效的IP地址，请检查网络设置！", L"IP地址错误", MB_OK | MB_ICONERROR);
				exit(1);
			}
			serv_adr.sin_port = htons(8888); // Set the port number  16位转换
			//connect
			if (connect(client_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR) {
				MessageBoxW(NULL, L"连接失败，请检查网络设置！", L"连接错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
				exit(1);
			}

			//客户端buffer
			char sendbuffer[1024] = { 0 };
			char recvbuffer[1024] = { 0 };
			//recv大于 0：表示成功接收到的字节数。等于 0：表示连接被对方关闭（TCP连接正常关闭）。
			//等于 SOCKET_ERROR（通常是 -1）：表示接收失败，
			//服务端recv send,客户端send 再recv
			if (send(client_sock, sendbuffer, sizeof(sendbuffer), 0) == SOCKET_ERROR)// Echo the received data back to the client)
			{
				MessageBox(NULL, L"发送数据失败，请检查网络设置！", L"发送错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
			}
			if (recv(client_sock, recvbuffer, sizeof(recvbuffer), 0) == SOCKET_ERROR) {// Receive data from the client
				MessageBox(NULL, L"接收数据失败，请检查网络设置！", L"接收错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
			}
			closesocket(client_sock);
		}


		return TRUE; // Return TRUE if successful, FALSE otherwise	
	}

};

#pragma once
