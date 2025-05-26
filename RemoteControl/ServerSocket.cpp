#include "pch.h"
#include "ServerSocket.h"

CServerSocket* CServerSocket::m_instance = nullptr; // 静态成员变量初始化
CServerSocket* pserver = CServerSocket:: GetInstance(); // 获取服务器套接字实例，确保只创建一个实例 GetInstance(); // 获取服务器套接字实例，确保只创建一个实例