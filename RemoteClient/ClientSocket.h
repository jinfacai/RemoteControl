#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
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
	//���캯����ʼ�����ݳ�Ա���������������ָ���Լ����ݴ�С
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
			memcpy((void*)sData.c_str(), pData, nSize); // ��ԭʼ���ݸ��Ƶ� sData ��
		}
		else {
			sData.clear(); // ���û�����ݣ���� sData
		}
		//��У�� �����������ֽڼ������������ΪУ��ֵ
		//for (size_t i = 0; i < nSize; ++i) {
		//	sSum += pData[i] & 0xFFFF; // �ۼ����ݵ�ÿ���ֽڣ�&0xFFFFȷ��ֻ������16λ
		//}	
		//sSum += sHead + sLength + sCmd; // ����ͷ�����Ⱥ�����Ҳ�ӵ�У�����
	}
	//��װ���ṹ
	bool FzPacket(BYTE* pBuffer, size_t nSize) {//ָ�򻺴���
		if (pBuffer == nullptr) {//����ڴ�ָ���Ƿ�Ϊ��
			return FALSE;
		}
		size_t totalSize = 2 + 4 + 2 + sData.size() + 2; // �����ܴ�С����ͷ(2) + ���ݳ���(4) + ����(2) + �������� + У���(2)
		//����
		if (nSize < totalSize) {//��鴫��Ļ�������С�Ƿ��㹻
			MessageBoxW(NULL, L"��������С���㣬�޷���װ���ݰ���", L"��װ����", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		nSize = totalSize; // ���´���� nSize ����Ϊ�ܴ�С
		//�����ݰ���װ����������
		*(WORD*)pBuffer = sHead; // ��ͷ
		*(DWORD*)(pBuffer + 2) = sLength; // ���ݳ���
		*(WORD*)(pBuffer + 6) = sCmd; // ����
		if (sData.size() > 0) {
			memcpy(pBuffer + 8, sData.c_str(), sData.size()); // ��sData���ݷ��͵�������
		}
		else {
			memset(pBuffer + 8, 0, 0); // ���û�����ݣ����0
		}
		// ����У��Ͳ��洢��������
		sSum = 0; // ����У���
		for (size_t i = 0; i < nSize - 2; ++i) { // ����������У���
			sSum += pBuffer[i] & 0xFFFF; // �ۼ����ݵ�ÿ���ֽڣ�&0xFFFFȷ��ֻ������16λ
		}
		*(WORD*)(pBuffer + 8 + sData.size()) = sSum; // �洢У��͵�������

	}
	//�������ṹ���жϳ����Ƿ�Ϸ�������ͷ���ֶΣ�sHead��sLength��sCmd
	//�������ݶΣ�sData,��ȡУ��ͣ�sSum,���¼���У��ͣ���֤�Ƿ�һ��
	bool ParsePacket(const BYTE* pData, size_t nSize) {
		if (nSize < 10) { //sHead(2)+ sLength��4�� + sCmd��2�� + sData + sSum��2��������Ҫ10�ֽ�
			// ���ݰ�����С��10�ֽڣ��޷�����
			MessageBoxW(NULL, L"���ݰ�����С��10�ֽڣ��޷�������", L"��������", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		sHead = *(WORD*)pData;// ��ȡ��ͷ
		// ����ͷ�Ƿ���ȷ
		if (sHead != 0xFEFF) {
			MessageBoxW(NULL, L"��ͷ����ȷ���������ݰ���ʽ��", L"��������", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		sLength = *(DWORD*)(pData + 2); // ��ȡ���ݳ���
		//���ݳ����Ƿ��ں���Χ��
		//nSize = sLength + 6
		if (sLength != nSize - 6) {
			return FALSE;
		}
		sCmd = *(WORD*)(pData + 6); // ��ȡ����
		// ��������Ƿ��ں���Χ��
		if (sCmd < 0 || sCmd > 10) { // �������Χ��0��10
			MessageBoxW(NULL, L"����ں���Χ�ڣ��������ݰ���ʽ��", L"��������", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		// ���У��Ͷ�ȡ�Ƿ�Խ��
		if (nSize < 8 + sLength + 2) {
			MessageBoxW(NULL, L"���ݰ����Ȳ��㣬�޷���ȡУ��ͣ�", L"��������", MB_OK | MB_ICONERROR);
			return false;
		}
		sSum = *(WORD*)(pData + 8 + sLength); // ��ȡУ���
		//���¼���У��ͣ���֤�Ƿ�һ��
		if (sSum != 0) { // ���У��Ͳ�Ϊ0����ʾ�д���
			WORD calculatedSum = 0;
			for (size_t i = 0; i < nSize - 2; ++i) { // ����������У���
				calculatedSum += pData[i] & 0xFFFF; // �ۼ����ݵ�ÿ���ֽڣ�&0xFFFFȷ��ֻ������16λ
			}
			if (calculatedSum != sSum) {
				MessageBoxW(NULL, L"У��Ͳ�ƥ�䣬�������ݰ���ʽ��", L"��������", MB_OK | MB_ICONERROR);
				return FALSE;
			}
		}
		return TRUE;
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
