#include "WeChatSpy.h"
#include "MinHook.h"
#include <cstdio>
#include <winnt.h> // 为了 __fastcall

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
// --- 全局变量 ---
static GetValueFunc_t g_pfnOriginalGetValueFunc = nullptr;
// **注意**: 这个地址需要你自己通过x64dbg或IDA确认。
// 例如，如果WeChatWin.dll基地址是0x7FFF0C400000, 你的目标函数地址是0x7FFF0C90F9B0
// 那么偏移就是 0x7FFF0C90F9B0 - 0x7FFF0C400000 = 0x50F9B0
constexpr QWORD g_ReturnValueFuncOffset = 0x2286DA9; // <--- 请务必替换成你计算出的正确偏移!


// 【新增】线程安全的队列和同步对象
static std::queue<CapturedReturnValue> g_ret_val_queue;
static std::mutex g_queue_mutex;
static std::condition_variable g_queue_cv;
static bool g_stop_thread = false;
static std::thread g_processing_thread;
// --- 我们的新版回调函数 (生产者) ---
// 它现在快如闪电，只做三件事：调用、打包、入队。
__int64 __fastcall MyReturnValueHookCallback(void* a1, int a2)
{
    // 1. 立刻调用原始函数，拿到返回值
    __int64 return_value = g_pfnOriginalGetValueFunc(a1, a2);

    // 2. 快速打包所有信息（参数和返回值）
    CapturedReturnValue captured_data = { a1, a2, return_value };

    // 3. 加锁，将数据推入队列
    {
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        g_ret_val_queue.push(captured_data);
    }

    // 4. 通知后台线程，然后立刻返回原始值
    g_queue_cv.notify_one();
    return return_value;
}


// --- 后台处理线程 (消费者) ---
void ProcessingThreadR() {
    printf("[INFO] 返回值处理线程已启动，等待数据...\n");

    while (!g_stop_thread) {
        CapturedReturnValue captured_data;

        {
            std::unique_lock<std::mutex> lock(g_queue_mutex);
            g_queue_cv.wait(lock, [] { return !g_ret_val_queue.empty() || g_stop_thread; });

            if (g_stop_thread && g_ret_val_queue.empty()) break;

            captured_data = g_ret_val_queue.front();
            g_ret_val_queue.pop();
        }

        // --- 在我们自己的安全线程里执行所有耗时操作 ---
        printf("--- ReturnValueHook Triggered (Worker Thread) ---\n");
        printf("  传入参数 a1 (RCX): 0x%p\n", captured_data.p1);
        printf("  传入参数 a2 (RDX): %d\n", captured_data.p2);
        printf("  >>>>> 捕获到返回值 (RAX): 0x%llX <<<<<\n", captured_data.ret_val);
        printf("----------------------------------------------\n\n");
    }
    printf("[INFO] 返回值处理线程已安全退出。\n");
}


// --- 启动/停止Hook的函数 ---
void StartReturnValueHook() {
    if (MH_Initialize() != MH_OK) {
        printf("[ERROR] MinHook 初始化失败。\n");
        return;
    }

    HMODULE hWeChatWin = GetModuleHandle("WeChatWin.dll");
    if (!hWeChatWin) {
        printf("[ERROR] 未能获取 WeChatWin.dll 句柄。\n");
        return;
    }

    LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_ReturnValueFuncOffset);

    if (MH_CreateHook(pTarget, &MyReturnValueHookCallback, reinterpret_cast<LPVOID*>(&g_pfnOriginalGetValueFunc)) != MH_OK) {
        printf("[ERROR] 创建返回值Hook失败。\n");
        return;
    }

    if (MH_EnableHook(pTarget) != MH_OK) {
        printf("[ERROR] 启用返回值Hook失败。\n");
        return;
    }

    printf("[INFO] 返回值Hook已成功启动！\n");
    // 启动后台处理线程
    g_stop_thread = false;
    g_processing_thread = std::thread(ProcessingThreadR);
}

void StopReturnValueHook() {
    printf("[INFO] 准备停止Hook和后台线程...\n");
    g_stop_thread = true;
    g_queue_cv.notify_one();

    if (g_pfnOriginalGetValueFunc) {
        HMODULE hWeChatWin = GetModuleHandle("WeChatWin.dll");
        if (hWeChatWin) {
            LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_ReturnValueFuncOffset);
            MH_DisableHook(pTarget);
        }
    }
    MH_Uninitialize();
    printf("[INFO] 返回值Hook已停止。\n");
}