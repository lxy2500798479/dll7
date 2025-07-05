#include "WeChatSpy.h"
#include <debugapi.h>
#include <cstdlib> // Ϊ�� free()

// =============================================================
// ��ȷ�ġ��ȶ���ʵ�� (������Ϊ�Ա�)
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

    // *** ��ȷ�ķ�ʽ������΢���Լ��ĺ������ͷ� ***
    pFreeChatMsg(reinterpret_cast<QWORD>(&buffer));
}


// =============================================================
// ��ʵ��һ�� ����ʾ��������ʹ�� free() �ͷ�
// =============================================================
void SendTextMessage_And_Crash_With_Free(const std::wstring& wxid, const std::wstring& msg)
{
    HMODULE hWeChatWin = GetModuleHandle(L"WeChatWin.dll");
    if (!hWeChatWin) return;
    auto pSendText = (SendText_t)((QWORD)hWeChatWin + Offsets::SendText);
    if (!pSendText) return;

    WxString wxid_str(wxid);
    WxString msg_str(msg);

    // ע�⣺�������ǹ����ڶ��Ϸ��� buffer����Ϊ free() ��������ջ�ڴ�
    // ����ӽ���ʵ������ģ�����ǲ�֪�� buffer ����α��ڲ�ʹ�õ�
    void* buffer = malloc(1104);
    if (!buffer) return;
    memset(buffer, 0, 1104);

    pSendText(reinterpret_cast<QWORD>(buffer), &wxid_str, &msg_str, 0, 1, 1, 0, 0);

    // *** ����Ĳ����������������Լ�DLL�� free() ���ͷ� ***
    // �⽫���¶��𻵣���Ϊ������ͷ��ڲ�ͬ��ģ��/CRT�С�
    // ����ܿ��ܻ�����������������
    OutputDebugString(L"�������� free()��׼������...");
    free(buffer);
}

