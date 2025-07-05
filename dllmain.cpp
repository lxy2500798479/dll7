// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include <windows.h>
#include "WebServer.hpp"

// 全局变量，用于保存我们DLL自己的模块句柄
HMODULE g_hModule = NULL;

// 启动Web服务的线程函数
DWORD WINAPI MainThread(LPVOID lpParam) {
    StartWebService(8080); // 假设我们使用8080端口
    return 0;
}

// “自毁”线程的实现，这是整个方案的核心
// 它在一个安全的环境中执行清理和卸载
DWORD WINAPI UnloadProcedureThread(LPVOID lpParam) {
    // 第1步：调用清理函数，停止服务并释放所有资源
    StopWebServiceAndCleanup();

    // 第2步：调用Windows API，将自己从进程中卸载，并结束当前线程
    FreeLibraryAndExitThread(g_hModule, 0);

    return 0;
}

// DLL 的主入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // DLL加载时，保存自己的句柄
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);

        // 创建一个线程来启动服务，然后立刻返回，不阻塞DllMain
        if (HANDLE hThread = CreateThread(NULL, 0, MainThread, NULL, 0, NULL)) {
            CloseHandle(hThread);
        }
        break;
    case DLL_PROCESS_DETACH:
        // 在卸载时，我们不执行任何操作，以避免加载器锁死锁
        // 所有的清理工作都已转移到 UnloadProcedureThread 中
        break;
    }
    return TRUE;
}