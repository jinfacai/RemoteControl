#pragma once
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define MAXCONN 1
class CServerSocket
{
public:
	//你可以通过实现单例模式来封装这个类，确保全局只有一个实例并提供公共访问点
	static CServerSocket* GetInstance() {//静态局部变量，确保只创建一个实例
		if (m_instance == NULL) {//静态函数没有this指针无法直接访问成员变量
			m_instance = new CServerSocket(); // 创建一个新的实例
		}
		return m_instance; // 返回该实例的引用
	}
	//static CServerSocket* GetInstance()
private:
	//构造函数
	CServerSocket() {
		if (InitSocket() == FALSE) {
			MessageBoxW(NULL, L"无法初始化环境，请检查网络设置！", L"初始化错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标	
			exit(1); // 0正常退出 1异常退出
		}
	}
	//拷贝构造函数
	CServerSocket(const CServerSocket& ss) {
		//执行拷贝操作
		//如果有指针成员变量，需要深拷贝
		//如果有资源需要释放，需要在析构函数中释放
		//这里可以根据需要实现深拷贝逻辑
		if (InitSocket() == FALSE) {
			MessageBoxW(NULL, L"无法初始化环境，请检查网络设置！", L"初始化错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标	
			exit(1); // 0正常退出 1异常退出
		}
	}
	CServerSocket& operator=(const CServerSocket& ss) {
		if (this != &ss) {
			//执行拷贝操作
			//如果有指针成员变量，需要深拷贝
			//如果有资源需要释放，需要在析构函数中释放
		}
		return *this; //返回当前对象的引用
	}
	//析构函数
	~CServerSocket() {
		WSACleanup(); 
	}
	BOOL InitSocket() {
		WSADATA Data;// Initialize Winsock
		if (WSAStartup(MAKEWORD(2, 2), &Data) != 0) { //Winsock version 2.2 传入变量地址
			return FALSE; // Return FALSE if initialization fails
		}
		SOCKET serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a TCP socket	
		if (serv_sock == INVALID_SOCKET) {//windows失败返回 INVALID_SOCKET linux失败返回 -1
			WSAGetLastError(); // Get the last error code
			return FALSE; // Return FALSE if socket creation fails
		}
		else {
			// Set up the server address structure
			//socket bind listen accpet recv send close 
			SOCKADDR_IN serv_adr, client_adr; // Windows
			//sockaddr_in serv_adr; linux
			//serv_adr初始化
			memset(&serv_adr, 0, sizeof(serv_adr));//serv_adr所有结构体成员初始化为0
			serv_adr.sin_family = AF_INET; // Set address family to IPv4
			//网络通信规定必须使用大端序（网络字节序）
			serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // Set the IP address to any available address 32位转换
			serv_adr.sin_port = htons(8888); // Set the port number  16位转换
			//bind
			//bind需要的是const sockaddr*类型的地址，所以需要强制转换
			//Winsock 编程时，bind 函数失败的返回值是 SOCKET_ERROR
			if (bind(serv_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR) {
				MessageBoxW(NULL, L"绑定失败，请检查端口是否被占用！", L"绑定错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
				exit(1);
			}
			if (listen(serv_sock, MAXCONN) == SOCKET_ERROR) {// Listen for incoming connections, MAXCONN is the maximum number of pending connections
				MessageBoxW(NULL, L"请检查端口是否被占用！", L"绑定错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
				exit(1);
			}
			//服务端buffer
			char sendbuffer[1024] = { 0 };
			char recvbuffer[1024] = { 0 };
			// Accept a client connection
			//成功时返回一个新的 SOCKET，代表与客户端的连接句柄，用于后续的收发数据。
			//失败时返回 INVALID_SOCKET（通常是(SOCKET)(~0)，值为 - 1）。
			SOCKET client_sock = accept(serv_sock, (sockaddr*)&client_adr, (int*)sizeof(client_adr));
			if (client_sock == INVALID_SOCKET) {
				MessageBoxW(NULL, L"连接失败，请检查网络设置！", L"连接错误", MB_OK | MB_ICONERROR);
				exit(1);
			}
			//recv大于 0：表示成功接收到的字节数。等于 0：表示连接被对方关闭（TCP连接正常关闭）。
			//等于 SOCKET_ERROR（通常是 -1）：表示接收失败，
			if (recv(client_sock, recvbuffer, sizeof(recvbuffer), 0) == SOCKET_ERROR) {// Receive data from the client
				MessageBox(NULL, L"接收数据失败，请检查网络设置！", L"接收错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
			}
			if (send(client_sock, sendbuffer, sizeof(sendbuffer), 0) == SOCKET_ERROR)// Echo the received data back to the client)
			{
				MessageBox(NULL, L"发送数据失败，请检查网络设置！", L"发送错误", MB_OK | MB_ICONERROR);//只有“确定”按钮 ,MB_ICONERROR信息图标
			}
			closesocket(serv_sock);
		}


		return TRUE; // Return TRUE if successful, FALSE otherwise	
	}
	static CServerSocket *m_instance;// 静态成员变量，用于存储单例实例

}server;

