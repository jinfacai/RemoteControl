#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define MAXCONN 1
class CClientSocket
{
public:
	static CClientSocket* GetInstance() {//��̬�ֲ�������ȷ��ֻ����һ��ʵ��
		if (m_instance == NULL) {//��̬����û��thisָ���޷�ֱ�ӷ��ʳ�Ա����
			m_instance = new CClientSocket(); // ����һ���µ�ʵ��
		}
		return m_instance; // ���ظ�ʵ��������
	}
	bool StartServer() {
		client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a TCP socket
		if (client_sock == INVALID_SOCKET) {//windowsʧ�ܷ��� INVALID_SOCKET linuxʧ�ܷ��� -1
			WSAGetLastError(); // Get the last error code
			return FALSE; // Return FALSE if socket creation fails
		}
		// Set up the server address structure
		memset(&client_adr, 0, sizeof(client_adr));//serv_adr���нṹ���Ա��ʼ��Ϊ0
		client_adr.sin_family = AF_INET; // Set address family to IPv4
		//����ͨ�Ź涨����ʹ�ô���������ֽ���
		if (inet_pton(AF_INET, "127.0.0.1", &client_adr.sin_addr) <= 0) {
			MessageBoxW(NULL, L"��Ч��IP��ַ�������������ã�", L"IP��ַ����", MB_OK | MB_ICONERROR);
			exit(1);
		}
		client_adr.sin_port = htons(8888); // Set the port number  16λת��
		return TRUE;
	}
	bool ConnectServer() {
		if (connect(client_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR) {
			MessageBoxW(NULL, L"����ʧ�ܣ������������ã�", L"���Ӵ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			return FALSE;
		}
		return TRUE;
	}
	bool SendtoServer() {
		if(send(client_sock, sendbuffer, sizeof(sendbuffer), 0) == SOCKET_ERROR) {// Echo the received data back to the client
			MessageBoxW(NULL, L"��������ʧ�ܣ������������ã�", L"���ʹ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			return FALSE;
		}
		return TRUE;
	}
	bool RecvfromServer(){
		if (recv(client_sock, recvbuffer, sizeof(recvbuffer), 0) == SOCKET_ERROR) {// Receive data from the client
			MessageBoxW(NULL, L"��������ʧ�ܣ������������ã�", L"���մ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			return FALSE;
		}
		return TRUE;
	}

private:
	//������ʼ��
	static CClientSocket* m_instance;// ��̬��Ա���������ڴ洢����ʵ��
	SOCKET serv_sock;
	SOCKET client_sock;
	SOCKADDR_IN serv_adr, client_adr;
	//�ͻ���buffer
	char sendbuffer[1024] = { 0 };
	char recvbuffer[1024] = { 0 };
	CClientSocket() {
		if (InitSocket() == FALSE) {
			MessageBoxW(NULL, L"�޷���ʼ�������������������ã�", L"��ʼ������", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��	
			exit(1); // 0�����˳� 1�쳣�˳�
		}
	}

	~CClientSocket() {
		WSACleanup();
	}
	BOOL InitSocket() {
		WSADATA Data;// Initialize Winsock
		if (WSAStartup(MAKEWORD(2, 2), &Data) != 0) { //Winsock version 2.2 ���������ַ
			return FALSE; // Return FALSE if initialization fails
		}
		return TRUE; // Return TRUE if successful, FALSE otherwise	
	}

};

#pragma once
