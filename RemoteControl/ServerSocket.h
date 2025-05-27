#pragma once
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define MAXCONN 1
// CPacket �����ڷ�װ���ݰ��Ľṹ�ʹ����߼�
class CPacket {
public:
	//���캯����ʼ�����ݳ�Ա
	CPacket() {
		sHead = 0;
		sLength = 0;
		sCmd = 0;
		sSum = 0;
	}
	CPacket(WORD nCmd, const BYTE* pData, size_t nSize) {//��ָ��ָ���������ݻ���������յ������ݰ���ʼλ�á�
		sHead = 0xFEFF; // ���ð�ͷ \\�������ݱ���ʹ�ô���� 
		sCmd = nCmd; // ��������
		sLength = nSize + 4;// 2�ֽڱ�ʶsHead|2�ֽ����ݳ���sLength
		if (nSize > 0) {
			sData.resize(nSize);//��sData�Ĵ�С����ΪnSize��ȷ�����㹻�Ŀռ�洢����
			//memcpy����  �����Ŀ���ڴ��ַ ���룺Դ�ڴ��ַ ���룺���Ƶ��ֽ���
			//c_str()���ڷ���һ��ָ�����ڲ��ַ������ C ����ַ���ָ��
			// �����Կ��ַ� '\0' ��β�� const char*����
			//const char* dataPtr = sData.data();  
			memcpy((void*)sData.c_str(), pData, nSize); // �����ݸ��Ƶ� sData ��
		}
		else {
			sData.clear(); // ���û�����ݣ���� sData
		}
		//��У�� �����������ֽڼ������������ΪУ��ֵ
		for (size_t i = 0; i < nSize; ++i) {
			sSum += pData[i] & 0xFFFF; // �ۼ����ݵ�ÿ���ֽڣ�&0xFFFFȷ��ֻ������16λ
		}
		//sSum += sHead + sLength + sCmd; // ����ͷ�����Ⱥ�����Ҳ�ӵ�У�����
	}
	//�������ṹ���жϳ����Ƿ�Ϸ�������ͷ���ֶΣ�sHead��sLength��sCmd
	//�������ݶΣ�sData,��ȡУ��ͣ�sSum,���¼���У��ͣ���֤�Ƿ�һ��
	bool ParsePacket(const BYTE* pData, size_t nSize) {
		if (nSize < 10) { //sHead(2)+ sLength��4�� + sCmd��2�� + sData + sSum��2��������Ҫ8�ֽ�
			// ���ݰ�����С��8�ֽڣ��޷�����
			return FALSE;
		}
		sHead = *(WORD*)pData;// ��ȡ��ͷ
		sLength = *(DWORD*)(pData + 2); // ��ȡ���ݳ���
		//nSize = sLength + 6
		if (sLength != nSize - 6){
			return false;
		}
		sCmd = *(WORD*)(pData + 6); // ��ȡ����
		sSum = *(WORD*)(pData + 8 + sLength); // ��ȡУ���

	}

	~CPacket() {
		//��������
		//�ͷ���Դ
	}
private:
	//WORD16λ��DWORD32λ�����ƴ��䣬����
	WORD sHead;
	DWORD sLength;
	WORD sCmd;
	std::string sData; // ʹ�� std::string �洢��������
	WORD sSum;//��У��

};
class CServerSocket
{
public:
	//�����ͨ��ʵ�ֵ���ģʽ����װ����࣬ȷ��ȫ��ֻ��һ��ʵ�����ṩ�������ʵ�
	//һ������ֻ��һ��ʵ��
	//����private�����캯�����������캯������ֵ��������أ�����������
	// ��̬��Ա������˽�к���InitSocket()
	static CServerSocket* GetInstance() {//��̬�ֲ�������ȷ��ֻ����һ��ʵ��
		if (m_instance == NULL) {//��̬����û��thisָ���޷�ֱ�ӷ��ʳ�Ա����
			m_instance = new CServerSocket(); // ����һ���µ�ʵ��
		}
		return m_instance; // ���ظ�ʵ��������
	}
	bool StartServer() {// ����������
		serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Create a TCP socket	
		if (serv_sock == INVALID_SOCKET) {//windowsʧ�ܷ��� INVALID_SOCKET linuxʧ�ܷ��� -1
			WSAGetLastError(); // Get the last error code
			return FALSE; // Return FALSE if socket creation fails
		}
		// Set up the server address structure
		//socket bind listen accpet recv send close 
		//SOCKADDR_IN serv_adr, client_adr; // Windows
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
			return FALSE;
		}
		if (listen(serv_sock, MAXCONN) == SOCKET_ERROR) {// Listen for incoming connections, MAXCONN is the maximum number of pending connections
			MessageBoxW(NULL, L"����˿��Ƿ�ռ�ã�", L"�󶨴���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			return FALSE;
		}
		return TRUE; 
	}

	bool AcceptClient() {
		// Accept a client connection
		//�ɹ�ʱ����һ���µ� SOCKET��������ͻ��˵����Ӿ�������ں������շ����ݡ�
		//ʧ��ʱ���� INVALID_SOCKET��ͨ����(SOCKET)(~0)��ֵΪ - 1����
		client_sock = accept(serv_sock, (sockaddr*)&client_adr, (int*)sizeof(client_adr));
		if (client_sock == INVALID_SOCKET) {
			MessageBoxW(NULL, L"����ʧ�ܣ������������ã�", L"���Ӵ���", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		return TRUE;

	}
	bool SendtoClient() {
		//recv���� 0����ʾ�ɹ����յ����ֽ��������� 0����ʾ���ӱ��Է��رգ�TCP���������رգ���
		//���� SOCKET_ERROR��ͨ���� -1������ʾ����ʧ�ܣ�
		if (send(client_sock, sendbuffer, sizeof(sendbuffer), 0) == SOCKET_ERROR)// Echo the received data back to the client)
		{
			MessageBox(NULL, L"��������ʧ�ܣ������������ã�", L"���ʹ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			return FALSE;
		}
		return TRUE;
	}
	bool RecvfromClient() {
		if (recv(client_sock, recvbuffer, sizeof(recvbuffer), 0) == SOCKET_ERROR) {// Receive data from the client
			MessageBox(NULL, L"��������ʧ�ܣ������������ã�", L"���մ���", MB_OK | MB_ICONERROR);//ֻ�С�ȷ������ť ,MB_ICONERROR��Ϣͼ��
			return FALSE;	
		}
		return TRUE;
	}
	//static CServerSocket* GetInstance()
private:
	//������ʼ��
	static CServerSocket* m_instance;// ��̬��Ա���������ڴ洢����ʵ��
	SOCKET serv_sock;
	SOCKET client_sock;
	SOCKADDR_IN serv_adr, client_adr;
	//�����buffer
	char sendbuffer[1024] = { 0 };
	char recvbuffer[1024] = { 0 };
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
		closesocket(serv_sock);
		WSACleanup(); 
	}
	//��ʼ��Winsock����
	BOOL InitSocket() {
		WSADATA Data;// Initialize Winsock
		if (WSAStartup(MAKEWORD(2, 2), &Data) != 0) { //Winsock version 2.2 ���������ַ
			return FALSE; // Return FALSE if initialization fails
		}
		return TRUE; // Return TRUE if successful, FALSE otherwise	
	}


};

