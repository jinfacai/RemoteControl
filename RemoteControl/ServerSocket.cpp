#include "pch.h"
#include "ServerSocket.h"

CServerSocket* CServerSocket::m_instance = nullptr; // ��̬��Ա������ʼ��
CServerSocket* pserver = CServerSocket:: GetInstance(); // ��ȡ�������׽���ʵ����ȷ��ֻ����һ��ʵ�� GetInstance(); // ��ȡ�������׽���ʵ����ȷ��ֻ����һ��ʵ��