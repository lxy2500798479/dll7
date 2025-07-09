#include "WeChatSpy.h"
#include "MinHook.h"
#include <cstdio>
#include <winnt.h> // Ϊ�� __fastcall

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
// --- ȫ�ֱ��� ---
static GetValueFunc_t g_pfnOriginalGetValueFunc = nullptr;
// **ע��**: �����ַ��Ҫ���Լ�ͨ��x64dbg��IDAȷ�ϡ�
// ���磬���WeChatWin.dll����ַ��0x7FFF0C400000, ���Ŀ�꺯����ַ��0x7FFF0C90F9B0
// ��ôƫ�ƾ��� 0x7FFF0C90F9B0 - 0x7FFF0C400000 = 0x50F9B0
constexpr QWORD g_ReturnValueFuncOffset = 0x2286DA9; // <--- ������滻������������ȷƫ��!


// ���������̰߳�ȫ�Ķ��к�ͬ������
static std::queue<CapturedReturnValue> g_ret_val_queue;
static std::mutex g_queue_mutex;
static std::condition_variable g_queue_cv;
static bool g_stop_thread = false;
static std::thread g_processing_thread;
// --- ���ǵ��°�ص����� (������) ---
// �����ڿ������磬ֻ�������£����á��������ӡ�
__int64 __fastcall MyReturnValueHookCallback(void* a1, int a2)
{
    // 1. ���̵���ԭʼ�������õ�����ֵ
    __int64 return_value = g_pfnOriginalGetValueFunc(a1, a2);

    // 2. ���ٴ��������Ϣ�������ͷ���ֵ��
    CapturedReturnValue captured_data = { a1, a2, return_value };

    // 3. �������������������
    {
        std::lock_guard<std::mutex> lock(g_queue_mutex);
        g_ret_val_queue.push(captured_data);
    }

    // 4. ֪ͨ��̨�̣߳�Ȼ�����̷���ԭʼֵ
    g_queue_cv.notify_one();
    return return_value;
}


// --- ��̨�����߳� (������) ---
void ProcessingThreadR() {
    printf("[INFO] ����ֵ�����߳����������ȴ�����...\n");

    while (!g_stop_thread) {
        CapturedReturnValue captured_data;

        {
            std::unique_lock<std::mutex> lock(g_queue_mutex);
            g_queue_cv.wait(lock, [] { return !g_ret_val_queue.empty() || g_stop_thread; });

            if (g_stop_thread && g_ret_val_queue.empty()) break;

            captured_data = g_ret_val_queue.front();
            g_ret_val_queue.pop();
        }

        // --- �������Լ��İ�ȫ�߳���ִ�����к�ʱ���� ---
        printf("--- ReturnValueHook Triggered (Worker Thread) ---\n");
        printf("  ������� a1 (RCX): 0x%p\n", captured_data.p1);
        printf("  ������� a2 (RDX): %d\n", captured_data.p2);
        printf("  >>>>> ���񵽷���ֵ (RAX): 0x%llX <<<<<\n", captured_data.ret_val);
        printf("----------------------------------------------\n\n");
    }
    printf("[INFO] ����ֵ�����߳��Ѱ�ȫ�˳���\n");
}


// --- ����/ֹͣHook�ĺ��� ---
void StartReturnValueHook() {
    if (MH_Initialize() != MH_OK) {
        printf("[ERROR] MinHook ��ʼ��ʧ�ܡ�\n");
        return;
    }

    HMODULE hWeChatWin = GetModuleHandle("WeChatWin.dll");
    if (!hWeChatWin) {
        printf("[ERROR] δ�ܻ�ȡ WeChatWin.dll �����\n");
        return;
    }

    LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_ReturnValueFuncOffset);

    if (MH_CreateHook(pTarget, &MyReturnValueHookCallback, reinterpret_cast<LPVOID*>(&g_pfnOriginalGetValueFunc)) != MH_OK) {
        printf("[ERROR] ��������ֵHookʧ�ܡ�\n");
        return;
    }

    if (MH_EnableHook(pTarget) != MH_OK) {
        printf("[ERROR] ���÷���ֵHookʧ�ܡ�\n");
        return;
    }

    printf("[INFO] ����ֵHook�ѳɹ�������\n");
    // ������̨�����߳�
    g_stop_thread = false;
    g_processing_thread = std::thread(ProcessingThreadR);
}

void StopReturnValueHook() {
    printf("[INFO] ׼��ֹͣHook�ͺ�̨�߳�...\n");
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
    printf("[INFO] ����ֵHook��ֹͣ��\n");
}