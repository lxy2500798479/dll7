#include "WeChatSpy.h"
#include <debugapi.h>
#include <cstdlib> // 为了 free()

// =============================================================
// 正确的、稳定的实现 (保留作为对比)
// =============================================================
void SendTextMessage(const std::wstring& wxid, const std::wstring& msg)
{
    HMODULE hWeChatWin = GetModuleHandle(L"WeChatWin.dll");
    if (!hWeChatWin) return;

    auto pSendMsgMgr = (SendMsgMgr_t)((QWORD)hWeChatWin + Offsets::SendMsgMgr);
    auto pSendText = (SendText_t)((QWORD)hWeChatWin + Offsets::SendText);
    auto pFreeChatMsg = (FreeChatMsg_t)((QWORD)hWeChatWin + Offsets::FreeChatMsg);

    if (!pSendMsgMgr || !pSendText || !pFreeChatMsg) return;

    WxString wxid_str(wxid);
    WxString msg_str(msg);

    char buffer[1104] = { 0 };
    pSendMsgMgr();
    pSendText(reinterpret_cast<QWORD>(&buffer), &wxid_str, &msg_str, 0, 1, 1, 0, 0);

    // *** 正确的方式：调用微信自己的函数来释放 ***
    pFreeChatMsg(reinterpret_cast<QWORD>(&buffer));
}


// =============================================================
// 【实验一】 错误示范：尝试使用 free() 释放
// =============================================================
void SendTextMessage_And_Crash_With_Free(const std::wstring& wxid, const std::wstring& msg)
{
    HMODULE hWeChatWin = GetModuleHandle(L"WeChatWin.dll");
    if (!hWeChatWin) return;
    auto pSendText = (SendText_t)((QWORD)hWeChatWin + Offsets::SendText);
    if (!pSendText) return;

    WxString wxid_str(wxid);
    WxString msg_str(msg);

    // 注意：这里我们故意在堆上分配 buffer，因为 free() 不能用于栈内存
    // 这更接近真实场景，模拟我们不知道 buffer 是如何被内部使用的
    void* buffer = malloc(1104);
    if (!buffer) return;
    memset(buffer, 0, 1104);

    pSendText(reinterpret_cast<QWORD>(buffer), &wxid_str, &msg_str, 0, 1, 1, 0, 0);

    // *** 错误的操作：尝试用我们自己DLL的 free() 来释放 ***
    // 这将导致堆损坏，因为分配和释放在不同的模块/CRT中。
    // 程序很可能会在这里立即崩溃。
    OutputDebugString(L"即将调用 free()，准备崩溃...");
    free(buffer);
}

