#include "WebServer.hpp"
#include "crow.h" // ȷ��·����ȷ
#include <thread>
#include <Windows.h>

// ����һ����ǰ�������������߱���������һ����Ϊ UnloadProcedureThread �ĺ����ڱ�� .cpp �ļ���
// ���������ܹ��ӵ�ǰ�ļ����� main.cpp �ж���ġ��Ի١��̵߳Ĺؼ�
extern DWORD WINAPI UnloadProcedureThread(LPVOID lpParam);

// ȫ��ָ�룬���ڳ��� crow ʵ���������߳�
static crow::SimpleApp* g_app = nullptr;
static std::thread* g_server_thread = nullptr;

// ����Web�����ʵ��
void StartWebService(int port) {
    if (g_app) return;

    g_app = new crow::SimpleApp();

    // ����һ������·��
    CROW_ROUTE((*g_app), "/hello")([] { return "Hello from Crow, trying to unload remotely."; });

    // ------------------- �������룺���Զ�̡��Ի١��ӿ� -------------------
    CROW_ROUTE((*g_app), "/shutdown")([] {

        // �����URL������ʱ������һ���µġ��Ի١��߳�
        // ���Ǵ�����ȫж�����̵�Ψһ���
        if (HANDLE hThread = CreateThread(NULL, 0, UnloadProcedureThread, NULL, 0, NULL)) {
            CloseHandle(hThread);
        }

        // ������ͻ��˷�����Ӧ
        return "Shutdown command received. Attempting to unload the DLL...";
        });
    // ----------------------------------------------------------------------

    g_app->loglevel(crow::LogLevel::Warning);

    // ��һ�����߳�����������
    g_server_thread = new std::thread([port]() {
        g_app->port(port).run();
        });
}

// ֹͣ��������Դ��ʵ��
void StopWebServiceAndCleanup() {
    if (!g_app) return;

    // ֹͣ������
    g_app->stop();

    // �ȴ��������߳̽���
    if (g_server_thread && g_server_thread->joinable()) {
        g_server_thread->join();
    }

    // �����ڴ�
    delete g_server_thread;
    g_server_thread = nullptr;
    delete g_app;
    g_app = nullptr;
}