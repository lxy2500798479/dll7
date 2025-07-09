#include "WebServer.hpp"
#include "crow.h"           // ȷ��·����ȷ
#include "WeChatSpy.h"      // 1. ��������������Ϣ���������ͷ�ļ�
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
    CROW_ROUTE((*g_app), "/hello")([] {
        return "Hello from Crow, trying to unload remotely.";
        });

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


    // =======================================================================
    // ===================    ������������Ϣ��POST�ӿ�    ===================
    // =======================================================================
    CROW_ROUTE((*g_app), "/send_message").methods("POST"_method)
        ([](const crow::request& req) {

        // 1. ���������е�JSON����
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, "Invalid JSON"); // Bad Request
        }

        // 2. ��JSON�л�ȡ wxid �� msg
        if (!body.has("wxid") || !body.has("msg")) {
            return crow::response(400, "Missing 'wxid' or 'msg' in JSON body");
        }

        std::string wxid_str = body["wxid"].s();
        std::string msg_str = body["msg"].s();

        if (wxid_str.empty() || msg_str.empty())
        {
            return crow::response(400, "'wxid' or 'msg' cannot be empty");
        }

        // 3. �� std::string ת��Ϊ std::wstring
        //    ��Ϊ SendTextMessage ������Ҫ���ַ�����
        int w_wxid_len = MultiByteToWideChar(CP_UTF8, 0, wxid_str.c_str(), -1, NULL, 0);
        std::wstring w_wxid(w_wxid_len, 0);
        MultiByteToWideChar(CP_UTF8, 0, wxid_str.c_str(), -1, &w_wxid[0], w_wxid_len);

        int w_msg_len = MultiByteToWideChar(CP_UTF8, 0, msg_str.c_str(), -1, NULL, 0);
        std::wstring w_msg(w_msg_len, 0);
        MultiByteToWideChar(CP_UTF8, 0, msg_str.c_str(), -1, &w_msg[0], w_msg_len);

        // 4. ���÷�����Ϣ�ĺ���
        SendTextMessage_And_Crash_With_Free(w_wxid, w_msg); //

        // 5. ���سɹ���Ӧ
        return crow::response(200, "Message sent successfully.");
            });
    // =======================================================================


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