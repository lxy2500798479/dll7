#include "AsyncSender.h"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <string>

#include "nlohmann/json.hpp"
#include "cpr/cpr.h" // 【核心】包含 cpr 的主头文件

// ================= 全局变量，用于生产者-消费者模型 (保持不变) =================
static std::queue<WxMsg>       g_msgQueue;
static std::mutex              g_queueMutex;
static std::condition_variable g_cv;
static std::thread             g_workerThread;
static std::atomic<bool>       g_stopWorker(false);
// ===========================================================================


// 消费者线程的主函数
void SenderWorkerLoop() {
    while (!g_stopWorker) {
        WxMsg msg_to_send;
        {
            // --- 等待并从队列中获取数据 (此部分逻辑不变) ---
            std::unique_lock<std::mutex> lock(g_queueMutex);
            g_cv.wait(lock, [] { return !g_msgQueue.empty() || g_stopWorker; });

            if (g_stopWorker && g_msgQueue.empty()) {
                return; // 收到停止信号且队列已空，退出线程
            }

            msg_to_send = g_msgQueue.front();
            g_msgQueue.pop();
        } // unique_lock在这里自动释放锁

        try {
            // --- 构建JSON (此部分逻辑不变) ---
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
            // 【核心】使用 cpr 发起POST请求，代码极其简洁、可靠
            cpr::Response r = cpr::Post(cpr::Url{ WEBHOOK_URL },
                cpr::Body{ j.dump() },
                cpr::Header{ {"Content-Type", "application/json"} });
            // ====================================================================

#ifdef _DEBUG
            if (r.status_code == 200) {
                printf(u8"[AsyncSender] 成功推送消息到 %s\n", WEBHOOK_URL);
            }
            else {
                // cpr 提供了丰富的错误信息
                printf(u8"[AsyncSender] 推送失败, Status: %ld, URL: %s, Error: %s\n",
                    r.status_code, r.url.c_str(), r.error.message.c_str());
            }
#endif
        }
        catch (const std::exception& e) {
#ifdef _DEBUG
            printf("[AsyncSender] 构造或发送请求时发生异常: %s\n", e.what());
#endif
        }
    }
}

// StartAsyncSender, StopAsyncSender, QueueMessageForSending 函数完全保持不变
void StartAsyncSender() {
    g_stopWorker = false;
    g_workerThread = std::thread(SenderWorkerLoop);
    printf("[AsyncSender] 后台推送服务已启动 (使用 cpr)。\n");
}

void StopAsyncSender() {
    if (g_workerThread.joinable()) {
        g_stopWorker = true;
        g_cv.notify_one();
        g_workerThread.join();
        printf("[AsyncSender] 后台推送服务已停止。\n");
    }
}

void QueueMessageForSending(const WxMsg& msg) {
    {
        std::lock_guard<std::mutex> lock(g_queueMutex);
        g_msgQueue.push(msg);
    }
    g_cv.notify_one();
}