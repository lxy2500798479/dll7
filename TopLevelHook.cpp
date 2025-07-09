
#include "WeChatSpy.h"
#include "MinHook.h"
#include <cstdio>
#include <string>
#include "AsyncSender.h" // 【1. 新增】包含异步发送模块的头文件



//// --- 全局变量 ---
static TopLevelFunc_t g_pfnOriginalTopLevelFunc = nullptr;
//// **注意**: 这个地址需要你自己通过x64dbg或IDA确认。
//// `sub_1825B90E0` 中的 `1825B90E0` 是一个加载后的绝对地址，我们需要的是相对偏移。
//// 假设 WeChatWin.dll 基地址是 0x180000000, 那么偏移就是 0x25B90E0。
//// 请务必替换成你找到的正确偏移！
constexpr QWORD g_TopLevelFuncOffset = 0x25B90E0; // <--- 请替换此地址!

//// --- 辅助函数 ---
//// 从WeChatFerry借鉴的、用于解析内存中宽字符串的函数
std::string GetStringFromWx(QWORD addr) {
    if (!addr) return "";
    wchar_t** p = (wchar_t**)addr;
    if (!p || !*p) return "";

    std::wstring wstr(*p);
    // wstring转string (UTF-8)
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// =======================================================================================
// ========================= 【核心修改】回调函数成为真正的生产者 =========================
// =======================================================================================
char __fastcall MyTopLevelHookCallback(void* a1, void* a2, void* a3)
{
    // 这个回调函数现在极其轻量和快速
    try {
        QWORD base = (QWORD)a2;

        // 【2. 填充结构体】创建一个 WxMsg 对象，并用解析出的数据填充它
        WxMsg msg;
        msg.id = *(QWORD*)(base + MsgStructOffsets::id);
        msg.type = *(DWORD*)(base + MsgStructOffsets::type);
        msg.roomid = GetStringFromWx(base + MsgStructOffsets::roomId);
        msg.content = GetStringFromWx(base + MsgStructOffsets::content);
        msg.sender = GetStringFromWx(base + MsgStructOffsets::sender);
        msg.xml = GetStringFromWx(base + MsgStructOffsets::xml);

        // 根据逆向分析经验，补充其他字段
        msg.is_group = msg.roomid.find("@chatroom") != std::string::npos;

        // TODO: 判断是否为自己发送的消息，需要获取个人wxid进行对比，此处暂存默认值
        msg.is_self = false;

        // TODO: 消息时间戳需要找到其对应的偏移量，此处暂存默认值
        msg.ts = 0;

        // 【3. 生产数据】将填充好的消息结构体推入到异步发送队列
        QueueMessageForSending(msg);

    }
    catch (...) {
#ifdef _DEBUG
        // 只在Debug模式下打印关键错误，避免影响发布性能
        printf(u8"[错误] MyTopLevelHookCallback 内部解析或入队时发生异常。\n");
#endif
    }

    // 【4. 调用原始函数】确保微信正常流程不被中断
    return g_pfnOriginalTopLevelFunc(a1, a2, a3);
}


//// --- 启动/停止Hook的函数 ---
void StartTopLevelHook() {
    if (MH_Initialize() != MH_OK) {
        printf("[ERROR] MinHook 初始化失败。\n");
        return;
    }

    HMODULE hWeChatWin = GetModuleHandle("WeChatWin.dll");
    if (!hWeChatWin) {
        printf("[ERROR] 未能获取 WeChatWin.dll 句柄。\n");
        return;
    }

    LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_TopLevelFuncOffset);

    if (MH_CreateHook(pTarget, &MyTopLevelHookCallback, reinterpret_cast<LPVOID*>(&g_pfnOriginalTopLevelFunc)) != MH_OK) {
        printf("[ERROR] 创建顶层Hook失败。\n");
        return;
    }

    if (MH_EnableHook(pTarget) != MH_OK) {
        printf("[ERROR] 启用顶层Hook失败。\n");
        return;
    }

    printf("[INFO] 顶层Hook已成功启动！\n");
}

void StopTopLevelHook() {
    if (g_pfnOriginalTopLevelFunc) {
        HMODULE hWeChatWin = GetModuleHandle("WeChatWin.dll");
        if (hWeChatWin) {
            LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_TopLevelFuncOffset);
            MH_DisableHook(pTarget);
        }
    }
    MH_Uninitialize();
    printf("[INFO] 顶层Hook已停止。\n");
}
