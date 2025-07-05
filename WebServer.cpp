#include "WebServer.hpp"
#include "crow.h" // 确保路径正确
#include <thread>
#include <Windows.h>

// 这是一个“前向声明”，告诉编译器，有一个名为 UnloadProcedureThread 的函数在别的 .cpp 文件里
// 这是我们能够从当前文件调用 main.cpp 中定义的“自毁”线程的关键
extern DWORD WINAPI UnloadProcedureThread(LPVOID lpParam);

// 全局指针，用于持有 crow 实例和其主线程
static crow::SimpleApp* g_app = nullptr;
static std::thread* g_server_thread = nullptr;

// 启动Web服务的实现
void StartWebService(int port) {
    if (g_app) return;

    g_app = new crow::SimpleApp();

    // 设置一个测试路由
    CROW_ROUTE((*g_app), "/hello")([] { return "Hello from Crow, trying to unload remotely."; });

    // ------------------- 新增代码：添加远程“自毁”接口 -------------------
    CROW_ROUTE((*g_app), "/shutdown")([] {

        // 当这个URL被访问时，创建一个新的“自毁”线程
        // 这是触发安全卸载流程的唯一入口
        if (HANDLE hThread = CreateThread(NULL, 0, UnloadProcedureThread, NULL, 0, NULL)) {
            CloseHandle(hThread);
        }

        // 立刻向客户端返回响应
        return "Shutdown command received. Attempting to unload the DLL...";
        });
    // ----------------------------------------------------------------------

    g_app->loglevel(crow::LogLevel::Warning);

    // 在一个新线程中启动服务
    g_server_thread = new std::thread([port]() {
        g_app->port(port).run();
        });
}

// 停止并清理资源的实现
void StopWebServiceAndCleanup() {
    if (!g_app) return;

    // 停止服务器
    g_app->stop();

    // 等待服务器线程结束
    if (g_server_thread && g_server_thread->joinable()) {
        g_server_thread->join();
    }

    // 清理内存
    delete g_server_thread;
    g_server_thread = nullptr;
    delete g_app;
    g_app = nullptr;
}