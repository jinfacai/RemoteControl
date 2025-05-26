// RemoteControl.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "pch.h"              // 预编译头文件，包含常用库，提升编译效率
#include "framework.h"        // 框架头文件，可能包含应用程序相关的宏定义、全局声明等
#include "RemoteControl.h"    // 项目主头文件，定义了应用程序类等
#include "ServerSocket.h" //socket

#ifdef _DEBUG
#define new DEBUG_NEW         // 在 Debug 模式下启用内存调试支持
#endif

// 唯一的应用程序对象（CWinApp 是 MFC 应用程序的核心类）
// 实例化一个全局的 CWinApp 对象，MFC 使用此对象来管理应用生命周期
CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;  // 用于存储程序返回值，0 表示正常退出, 1表示失败

    // 获取当前模块（可执行文件）的句柄
    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)  // 如果句柄获取成功
    {
        // 初始化 MFC 库
        // 参数依次为：模块句柄，保留参数（nullptr），命令行字符串，显示命令（0 = SW_HIDE）
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // 初始化失败，输出错误信息到控制台
            wprintf(L"错误: MFC 初始化失败\n");//wprintf wchar_t 类型
            nRetCode = 1;
        }
        else
        {
            //CServerSocket server;//只能访问public，所以考虑全局静态变量
            CServerSocket *pserver = CServerSocket::GetInstance(); // 获取服务器套接字实例，确保只创建一个实例
            ///pserver->InitSocket();
            // TODO: 在此处为应用程序的行为编写代码
            // 程序初始化成功，可以在这里编写主程序逻辑
        }
    }
    else
    {
        // 如果获取模块句柄失败，输出错误信息
        wprintf(L"错误: GetModuleHandle 失败\n");
        nRetCode = 1;
    }

    return nRetCode;  // 返回程序执行结果
}
