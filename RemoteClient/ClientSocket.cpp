#include "pch.h"
#include "ClientSocket.h"

CClientSocket* CClientSocket::m_instance = nullptr; // ��̬��Ա������ʼ��
CClientSocket* pclient = CClientSocket::GetInstance(); // ��ȡ�ͻ����׽���ʵ����ȷ��ֻ����һ��ʵ��
