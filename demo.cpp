

void demo() {

//#include "WeChatSpy.h"
//#include "MinHook.h"
//#include <cstdio>
//#include <string>

    //// --- 全局变量 ---
    //static TopLevelFunc_t g_pfnOriginalTopLevelFunc = nullptr;
    //// **注意**: 这个地址需要你自己通过x64dbg或IDA确认。
    //// `sub_1825B90E0` 中的 `1825B90E0` 是一个加载后的绝对地址，我们需要的是相对偏移。
    //// 假设 WeChatWin.dll 基地址是 0x180000000, 那么偏移就是 0x25B90E0。
    //// 请务必替换成你找到的正确偏移！
    //constexpr QWORD g_TopLevelFuncOffset = 0x25B90E0; // <--- 请替换此地址!

    //// --- 辅助函数 ---
    //// 从WeChatFerry借鉴的、用于解析内存中宽字符串的函数
    //std::string GetStringFromWx(QWORD addr) {
    //    if (!addr) return "";
    //    wchar_t** p = (wchar_t**)addr;
    //    if (!p || !*p) return "";

    //    std::wstring wstr(*p);
    //    // wstring转string (UTF-8)
    //    if (wstr.empty()) return std::string();
    //    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    //    std::string strTo(size_needed, 0);
    //    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    //    return strTo;
    //}

    //// --- 我们的回调函数 ---
    //char __fastcall MyTopLevelHookCallback(void* a1, void* a2, void* a3)
    //{
    //    printf("--- TopLevelHook 触发！---\n");
    //    printf("  已成功拦截到顶层函数调用。\n");
    //    printf("  参数 a2 (消息对象指针): 0x%p\n", a2);

    //    // a2 就是包含了所有信息的完整结构体指针！
    //    // 现在我们可以像WeChatFerry一样，通过偏移量来解析它。
    //    // 以下偏移量来自WeChatFerry，用于演示
    //    QWORD base = (QWORD)a2;
    //    try {
    //        uint64_t msgId = *(QWORD*)(base + 0x30);
    //        uint32_t msgType = *(DWORD*)(base + 0x38);
    //        std::string roomid = GetStringFromWx(base + 0x48);
    //        std::string content = GetStringFromWx(base + 0x88);
    //        std::string sender = GetStringFromWx(base + 0x240);

    //        printf("\n  --- 解析 a2 成功 ---\n");
    //        printf("    消息ID: %llu\n", msgId);
    //        printf("    消息类型: %u\n", msgType);
    //        printf("    原始群ID/发送者: %s\n", roomid.c_str());
    //        printf("    消息内容: %s\n", content.c_str());
    //        printf("    (群内)发送者: %s\n", sender.c_str());
    //        printf("  ---------------------\n\n");
    //    }
    //    catch (...) {
    //        printf("[错误] 解析 a2 结构时发生异常。\n");
    //    }

    //    // 调用原始函数，让微信继续运行
    //    return g_pfnOriginalTopLevelFunc(a1, a2, a3);
    //}

    //// --- 启动/停止Hook的函数 ---
    //void StartTopLevelHook() {
    //    if (MH_Initialize() != MH_OK) {
    //        printf("[ERROR] MinHook 初始化失败。\n");
    //        return;
    //    }

    //    HMODULE hWeChatWin = GetModuleHandle(L"WeChatWin.dll");
    //    if (!hWeChatWin) {
    //        printf("[ERROR] 未能获取 WeChatWin.dll 句柄。\n");
    //        return;
    //    }

    //    LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_TopLevelFuncOffset);

    //    if (MH_CreateHook(pTarget, &MyTopLevelHookCallback, reinterpret_cast<LPVOID*>(&g_pfnOriginalTopLevelFunc)) != MH_OK) {
    //        printf("[ERROR] 创建顶层Hook失败。\n");
    //        return;
    //    }

    //    if (MH_EnableHook(pTarget) != MH_OK) {
    //        printf("[ERROR] 启用顶层Hook失败。\n");
    //        return;
    //    }

    //    printf("[INFO] 顶层Hook已成功启动！\n");
    //}

    //void StopTopLevelHook() {
    //    if (g_pfnOriginalTopLevelFunc) {
    //        HMODULE hWeChatWin = GetModuleHandle(L"WeChatWin.dll");
    //        if (hWeChatWin) {
    //            LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_TopLevelFuncOffset);
    //            MH_DisableHook(pTarget);
    //        }
    //    }
    //    MH_Uninitialize();
    //    printf("[INFO] 顶层Hook已停止。\n");
    //}

}