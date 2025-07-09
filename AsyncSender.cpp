#include "AsyncSender.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <string>

#include "nlohmann/json.hpp"
#include "cpr/cpr.h" // �����ġ����� cpr ����ͷ�ļ�

// ================= ȫ�ֱ���������������-������ģ�� (���ֲ���) =================
static std::queue<WxMsg>       g_msgQueue;
static std::mutex              g_queueMutex;
static std::condition_variable g_cv;
static std::thread             g_workerThread;
static std::atomic<bool>       g_stopWorker(false);
// ===========================================================================


// �������̵߳�������
void SenderWorkerLoop() {
    while (!g_stopWorker) {
        WxMsg msg_to_send;
        {
            // --- �ȴ����Ӷ����л�ȡ���� (�˲����߼�����) ---
            std::unique_lock<std::mutex> lock(g_queueMutex);
            g_cv.wait(lock, [] { return !g_msgQueue.empty() || g_stopWorker; });

            if (g_stopWorker && g_msgQueue.empty()) {
                return; // �յ�ֹͣ�ź��Ҷ����ѿգ��˳��߳�
            }

            msg_to_send = g_msgQueue.front();
            g_msgQueue.pop();
        } // unique_lock�������Զ��ͷ���

        try {
            // --- ����JSON (�˲����߼�����) ---
            nlohmann::json j;
            j["is_self"] = msg_to_send.is_self;
            j["is_group"] = msg_to_send.is_group;
            j["type"] = msg_to_send.type;
            j["id"] = msg_to_send.id;
            j["ts"] = msg_to_send.ts;
            j["sender"] = msg_to_send.sender;
            j["roomid"] = msg_to_send.roomid;
            j["content"] = msg_to_send.content;
            j["xml"] = msg_to_send.xml;

            // ====================================================================
            // �����ġ�ʹ�� cpr ����POST���󣬴��뼫���ࡢ�ɿ�
            cpr::Response r = cpr::Post(cpr::Url{ WEBHOOK_URL },
                cpr::Body{ j.dump() },
                cpr::Header{ {"Content-Type", "application/json"} });
            // ====================================================================

#ifdef _DEBUG
            if (r.status_code == 200) {
                printf(u8"[AsyncSender] �ɹ�������Ϣ�� %s\n", WEBHOOK_URL);
            }
            else {
                // cpr �ṩ�˷ḻ�Ĵ�����Ϣ
                printf(u8"[AsyncSender] ����ʧ��, Status: %ld, URL: %s, Error: %s\n",
                    r.status_code, r.url.c_str(), r.error.message.c_str());
            }
#endif
        }
        catch (const std::exception& e) {
#ifdef _DEBUG
            printf("[AsyncSender] �����������ʱ�����쳣: %s\n", e.what());
#endif
        }
    }
}

// StartAsyncSender, StopAsyncSender, QueueMessageForSending ������ȫ���ֲ���
void StartAsyncSender() {
    g_stopWorker = false;
    g_workerThread = std::thread(SenderWorkerLoop);
    printf("[AsyncSender] ��̨���ͷ��������� (ʹ�� cpr)��\n");
}

void StopAsyncSender() {
    if (g_workerThread.joinable()) {
        g_stopWorker = true;
        g_cv.notify_one();
        g_workerThread.join();
        printf("[AsyncSender] ��̨���ͷ�����ֹͣ��\n");
    }
}

void QueueMessageForSending(const WxMsg& msg) {
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        g_msgQueue.push(msg);
    }
    g_cv.notify_one();
}