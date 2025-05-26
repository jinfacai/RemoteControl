#pragma once
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define MAXCONN 1
class CServerSocket
{
public:
	//�����ͨ��ʵ�ֵ���ģʽ����װ����࣬ȷ��ȫ��ֻ��һ��ʵ�����ṩ�������ʵ�
	static CServerSocket* GetInstance() {//��̬�ֲ�������ȷ��ֻ����һ��ʵ��
		if (m_instance == NULL) {//��̬����û��thisָ���޷�ֱ�ӷ��ʳ�Ա����
			m_instance = new CServerSocket(); // ����һ���µ�ʵ��
		}
		return m_instance; // ���ظ�ʵ��������
	}
	//static CServerSocket* GetInstance()
private:
	//���캯��
	CServerSocket() {
		if (InitSocket() == FALSE) {
			MessageBoxW(NULL, L"�޷���ʼ�������������������ã�", L"��ʼ������", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��	
			exit(1); // 0�����˳� 1�쳣�˳�
		}
	}
	//�������캯��
	CServerSocket(const CServerSocket& ss) {
		//ִ�п�������
		//�����ָ���Ա��������Ҫ���
		//�������Դ��Ҫ�ͷţ���Ҫ�������������ͷ�
		//������Ը�����Ҫʵ������߼�
		if (InitSocket() == FALSE) {
			MessageBoxW(NULL, L"�޷���ʼ�������������������ã�", L"��ʼ������", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��	
			exit(1); // 0�����˳� 1�쳣�˳�
		}
	}
	CServerSocket& operator=(const CServerSocket& ss) {
		if (this != &ss) {
			//ִ�п�������
			//�����ָ���Ա��������Ҫ���
			//�������Դ��Ҫ�ͷţ���Ҫ�������������ͷ�
		}
		return *this; //���ص�ǰ���������
	}
	//��������
	~CServerSocket() {
		WSACleanup(); 
	}
	BOOL InitSocket() {
		WSADATA Data;// Initialize Winsock
		if (WSAStartup(MAKEWORD(2, 2), &Data) != 0) { //Winsock version 2.2 ���������ַ
			return FALSE; // Return FALSE if initialization fails
		}
		SOCKET serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a TCP socket	
		if (serv_sock == INVALID_SOCKET) {//windowsʧ�ܷ��� INVALID_SOCKET linuxʧ�ܷ��� -1
			WSAGetLastError(); // Get the last error code
			return FALSE; // Return FALSE if socket creation fails
		}
		else {
			// Set up the server address structure
			//socket bind listen accpet recv send close 
			SOCKADDR_IN serv_adr, client_adr; // Windows
			//sockaddr_in serv_adr; linux
			//serv_adr��ʼ��
			memset(&serv_adr, 0, sizeof(serv_adr));//serv_adr���нṹ���Ա��ʼ��Ϊ0
			serv_adr.sin_family = AF_INET; // Set address family to IPv4
			//����ͨ�Ź涨����ʹ�ô���������ֽ���
			serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // Set the IP address to any available address 32λת��
			serv_adr.sin_port = htons(8888); // Set the port number  16λת��
			//bind
			//bind��Ҫ����const sockaddr*���͵ĵ�ַ��������Ҫǿ��ת��
			//Winsock ���ʱ��bind ����ʧ�ܵķ���ֵ�� SOCKET_ERROR
			if (bind(serv_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) == SOCKET_ERROR) {
				MessageBoxW(NULL, L"��ʧ�ܣ�����˿��Ƿ�ռ�ã�", L"�󶨴���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
				exit(1);
			}
			if (listen(serv_sock, MAXCONN) == SOCKET_ERROR) {// Listen for incoming connections, MAXCONN is the maximum number of pending connections
				MessageBoxW(NULL, L"����˿��Ƿ�ռ�ã�", L"�󶨴���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
				exit(1);
			}
			//�����buffer
			char sendbuffer[1024] = { 0 };
			char recvbuffer[1024] = { 0 };
			// Accept a client connection
			//�ɹ�ʱ����һ���µ� SOCKET��������ͻ��˵����Ӿ�������ں������շ����ݡ�
			//ʧ��ʱ���� INVALID_SOCKET��ͨ����(SOCKET)(~0)��ֵΪ - 1����
			SOCKET client_sock = accept(serv_sock, (sockaddr*)&client_adr, (int*)sizeof(client_adr));
			if (client_sock == INVALID_SOCKET) {
				MessageBoxW(NULL, L"����ʧ�ܣ������������ã�", L"���Ӵ���", MB_OK | MB_ICONERROR);
				exit(1);
			}
			//recv���� 0����ʾ�ɹ����յ����ֽ��������� 0����ʾ���ӱ��Է��رգ�TCP���������رգ���
			//���� SOCKET_ERROR��ͨ���� -1������ʾ����ʧ�ܣ�
			if (recv(client_sock, recvbuffer, sizeof(recvbuffer), 0) == SOCKET_ERROR) {// Receive data from the client
				MessageBox(NULL, L"��������ʧ�ܣ������������ã�", L"���մ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			}
			if (send(client_sock, sendbuffer, sizeof(sendbuffer), 0) == SOCKET_ERROR)// Echo the received data back to the client)
			{
				MessageBox(NULL, L"��������ʧ�ܣ������������ã�", L"���ʹ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			}
			closesocket(serv_sock);
		}


		return TRUE; // Return TRUE if successful, FALSE otherwise	
	}
	static CServerSocket *m_instance;// ��̬��Ա���������ڴ洢����ʵ��

}server;

