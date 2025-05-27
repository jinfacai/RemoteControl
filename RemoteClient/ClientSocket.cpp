#include "pch.h"
#include "ClientSocket.h"

CClientSocket* CClientSocket::m_instance = nullptr; // 静态成员变量初始化
CClientSocket* pclient = CClientSocket::GetInstance(); // 获取客户端套接字实例，确保只创建一个实例
