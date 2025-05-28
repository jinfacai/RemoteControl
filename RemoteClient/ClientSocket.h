#pragma once
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#define MAXCONN 1
// CPacket 类用于封装数据包的结构和处理逻辑
class CPacket {
public:
	//构造函数初始化数据成员
	CPacket() {
		sHead = 0;
		sLength = 0;
		sCmd = 0;
		sSum = 0;
	}
	//构造函数初始化数据成员，接收命令和数据指针以及数据大小
	CPacket(WORD nCmd, const BYTE* pData, size_t nSize) {//用指针指向网络数据缓冲区或接收到的数据包起始位置。
		sHead = 0xFEFF; // 设置包头 \\网络数据必须使用大端序 
		sCmd = nCmd; // 设置命令
		sLength = nSize + 4;// 2字节标识sHead|2字节数据长度sLength
		if (nSize > 0) {
			sData.resize(nSize);//把sData的大小设置为nSize，确保有足够的空间存储数据
			//memcpy参数  输出：目标内存地址 输入：源内存地址 输入：复制的字节数
			//c_str()用于返回一个指向其内部字符数组的 C 风格字符串指针
			// （即以空字符 '\0' 结尾的 const char*）。
			//const char* dataPtr = sData.data();  
			memcpy((void*)sData.c_str(), pData, nSize); // 将原始数据复制到 sData 中
		}
		else {
			sData.clear(); // 如果没有数据，清空 sData
		}
		//和校验 把所有数据字节加起来，结果作为校验值
		//for (size_t i = 0; i < nSize; ++i) {
		//	sSum += pData[i] & 0xFFFF; // 累加数据的每个字节，&0xFFFF确保只保留低16位
		//}	
		//sSum += sHead + sLength + sCmd; // 将包头、长度和命令也加到校验和中
	}
	//封装包结构
	bool FzPacket(BYTE* pBuffer, size_t nSize) {//指向缓存区
		if (pBuffer == nullptr) {//检查内存指针是否为空
			return FALSE;
		}
		size_t totalSize = 2 + 4 + 2 + sData.size() + 2; // 计算总大小：包头(2) + 数据长度(4) + 命令(2) + 数据内容 + 校验和(2)
		//报错
		if (nSize < totalSize) {//检查传入的缓冲区大小是否足够
			MessageBoxW(NULL, L"缓冲区大小不足，无法封装数据包！", L"封装错误", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		nSize = totalSize; // 更新传入的 nSize 参数为总大小
		//将数据包封装到缓冲区中
		*(WORD*)pBuffer = sHead; // 包头
		*(DWORD*)(pBuffer + 2) = sLength; // 数据长度
		*(WORD*)(pBuffer + 6) = sCmd; // 命令
		if (sData.size() > 0) {
			memcpy(pBuffer + 8, sData.c_str(), sData.size()); // 将sData数据发送到缓冲区
		}
		else {
			memset(pBuffer + 8, 0, 0); // 如果没有数据，填充0
		}
		// 计算校验和并存储到缓冲区
		sSum = 0; // 重置校验和
		for (size_t i = 0; i < nSize - 2; ++i) { // 不包括最后的校验和
			sSum += pBuffer[i] & 0xFFFF; // 累加数据的每个字节，&0xFFFF确保只保留低16位
		}
		*(WORD*)(pBuffer + 8 + sData.size()) = sSum; // 存储校验和到缓冲区

	}
	//解析包结构：判断长度是否合法，解析头部字段：sHead、sLength、sCmd
	//解析数据段：sData,读取校验和：sSum,重新计算校验和，验证是否一致
	bool ParsePacket(const BYTE* pData, size_t nSize) {
		if (nSize < 10) { //sHead(2)+ sLength（4） + sCmd（2） + sData + sSum（2）至少需要10字节
			// 数据包长度小于10字节，无法解析
			MessageBoxW(NULL, L"数据包长度小于10字节，无法解析！", L"解析错误", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		sHead = *(WORD*)pData;// 读取包头
		// 检查包头是否正确
		if (sHead != 0xFEFF) {
			MessageBoxW(NULL, L"包头不正确，请检查数据包格式！", L"解析错误", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		sLength = *(DWORD*)(pData + 2); // 读取数据长度
		//数据长度是否在合理范围内
		//nSize = sLength + 6
		if (sLength != nSize - 6) {
			return FALSE;
		}
		sCmd = *(WORD*)(pData + 6); // 读取命令
		// 检查命令是否在合理范围内
		if (sCmd < 0 || sCmd > 10) { // 假设命令范围是0到10
			MessageBoxW(NULL, L"命令不在合理范围内，请检查数据包格式！", L"解析错误", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		// 检查校验和读取是否越界
		if (nSize < 8 + sLength + 2) {
			MessageBoxW(NULL, L"数据包长度不足，无法读取校验和！", L"解析错误", MB_OK | MB_ICONERROR);
			return false;
		}
		sSum = *(WORD*)(pData + 8 + sLength); // 读取校验和
		//重新计算校验和，验证是否一致
		if (sSum != 0) { // 如果校验和不为0，表示有错误
			WORD calculatedSum = 0;
			for (size_t i = 0; i < nSize - 2; ++i) { // 不包括最后的校验和
				calculatedSum += pData[i] & 0xFFFF; // 累加数据的每个字节，&0xFFFF确保只保留低16位
			}
			if (calculatedSum != sSum) {
				MessageBoxW(NULL, L"校验和不匹配，请检查数据包格式！", L"解析错误", MB_OK | MB_ICONERROR);
				return FALSE;
			}
		}
		return TRUE;
	}
	~CPacket() {
		//析构函数
		//释放资源
	}
private:
	//WORD16位，DWORD32位二进制传输，更快
	WORD sHead;
	DWORD sLength;
	WORD sCmd;
	std::string sData; // 使用 std::string 存储数据内容
	WORD sSum;//和校验

};
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
