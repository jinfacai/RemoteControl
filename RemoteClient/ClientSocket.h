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
		SOCKET client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a TCP socket	
		if (client_sock == INVALID_SOCKET) {//windowsʧ�ܷ��� INVALID_SOCKET linuxʧ�ܷ��� -1
			WSAGetLastError(); // Get the last error code
			return FALSE; // Return FALSE if socket creation fails
		}
		else {
			// Set up the server address structure
			//socket connect send recv close
			SOCKADDR_IN serv_adr; // Windows
			//sockaddr_in serv_adr; linux
			//serv_adr��ʼ��
			memset(&serv_adr, 0, sizeof(serv_adr));//serv_adr���нṹ���Ա��ʼ��Ϊ0
			serv_adr.sin_family = AF_INET; // Set address family to IPv4
			//����ͨ�Ź涨����ʹ�ô���������ֽ���
			//ת��Ϊһ�� in_addr_t ���ͣ�32 λ�������ĺ���
			//serv_adr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Set the IP address to any available address 32λת��
			if (inet_pton(AF_INET, "127.0.0.1", &serv_adr.sin_addr) <= 0) {
				MessageBoxW(NULL, L"��Ч��IP��ַ�������������ã�", L"IP��ַ����", MB_OK | MB_ICONERROR);
				exit(1);
			}
			serv_adr.sin_port = htons(8888); // Set the port number  16λת��
			//connect
			if (connect(client_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR) {
				MessageBoxW(NULL, L"����ʧ�ܣ������������ã�", L"���Ӵ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
				exit(1);
			}

			//�ͻ���buffer
			char sendbuffer[1024] = { 0 };
			char recvbuffer[1024] = { 0 };
			//recv���� 0����ʾ�ɹ����յ����ֽ��������� 0����ʾ���ӱ��Է��رգ�TCP���������رգ���
			//���� SOCKET_ERROR��ͨ���� -1������ʾ����ʧ�ܣ�
			//�����recv send,�ͻ���send ��recv
			if (send(client_sock, sendbuffer, sizeof(sendbuffer), 0) == SOCKET_ERROR)// Echo the received data back to the client)
			{
				MessageBox(NULL, L"��������ʧ�ܣ������������ã�", L"���ʹ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			}
			if (recv(client_sock, recvbuffer, sizeof(recvbuffer), 0) == SOCKET_ERROR) {// Receive data from the client
				MessageBox(NULL, L"��������ʧ�ܣ������������ã�", L"���մ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			}
			closesocket(client_sock);
		}


		return TRUE; // Return TRUE if successful, FALSE otherwise	
	}

};

#pragma once
