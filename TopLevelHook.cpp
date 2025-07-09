
#include "WeChatSpy.h"
#include "MinHook.h"
#include <cstdio>
#include <string>
#include "AsyncSender.h" // ��1. �����������첽����ģ���ͷ�ļ�



//// --- ȫ�ֱ��� ---
static TopLevelFunc_t g_pfnOriginalTopLevelFunc = nullptr;
//// **ע��**: �����ַ��Ҫ���Լ�ͨ��x64dbg��IDAȷ�ϡ�
//// `sub_1825B90E0` �е� `1825B90E0` ��һ�����غ�ľ��Ե�ַ��������Ҫ�������ƫ�ơ�
//// ���� WeChatWin.dll ����ַ�� 0x180000000, ��ôƫ�ƾ��� 0x25B90E0��
//// ������滻�����ҵ�����ȷƫ�ƣ�
constexpr QWORD g_TopLevelFuncOffset = 0x25B90E0; // <--- ���滻�˵�ַ!

//// --- �������� ---
//// ��WeChatFerry����ġ����ڽ����ڴ��п��ַ����ĺ���
std::string GetStringFromWx(QWORD addr) {
    if (!addr) return "";
    wchar_t** p = (wchar_t**)addr;
    if (!p || !*p) return "";

    std::wstring wstr(*p);
    // wstringתstring (UTF-8)
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

// =======================================================================================
// ========================= �������޸ġ��ص�������Ϊ������������ =========================
// =======================================================================================
char __fastcall MyTopLevelHookCallback(void* a1, void* a2, void* a3)
{
    // ����ص��������ڼ��������Ϳ���
    try {
        QWORD base = (QWORD)a2;

        // ��2. ���ṹ�塿����һ�� WxMsg ���󣬲��ý����������������
        WxMsg msg;
        msg.id = *(QWORD*)(base + MsgStructOffsets::id);
        msg.type = *(DWORD*)(base + MsgStructOffsets::type);
        msg.roomid = GetStringFromWx(base + MsgStructOffsets::roomId);
        msg.content = GetStringFromWx(base + MsgStructOffsets::content);
        msg.sender = GetStringFromWx(base + MsgStructOffsets::sender);
        msg.xml = GetStringFromWx(base + MsgStructOffsets::xml);

        // ��������������飬���������ֶ�
        msg.is_group = msg.roomid.find("@chatroom") != std::string::npos;

        // TODO: �ж��Ƿ�Ϊ�Լ����͵���Ϣ����Ҫ��ȡ����wxid���жԱȣ��˴��ݴ�Ĭ��ֵ
        msg.is_self = false;

        // TODO: ��Ϣʱ�����Ҫ�ҵ����Ӧ��ƫ�������˴��ݴ�Ĭ��ֵ
        msg.ts = 0;

        // ��3. �������ݡ������õ���Ϣ�ṹ�����뵽�첽���Ͷ���
        QueueMessageForSending(msg);

    }
    catch (...) {
#ifdef _DEBUG
        // ֻ��Debugģʽ�´�ӡ�ؼ����󣬱���Ӱ�췢������
        printf(u8"[����] MyTopLevelHookCallback �ڲ����������ʱ�����쳣��\n");
#endif
    }

    // ��4. ����ԭʼ������ȷ��΢���������̲����ж�
    return g_pfnOriginalTopLevelFunc(a1, a2, a3);
}


//// --- ����/ֹͣHook�ĺ��� ---
void StartTopLevelHook() {
    if (MH_Initialize() != MH_OK) {
        printf("[ERROR] MinHook ��ʼ��ʧ�ܡ�\n");
        return;
    }

    HMODULE hWeChatWin = GetModuleHandle("WeChatWin.dll");
    if (!hWeChatWin) {
        printf("[ERROR] δ�ܻ�ȡ WeChatWin.dll �����\n");
        return;
    }

    LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_TopLevelFuncOffset);

    if (MH_CreateHook(pTarget, &MyTopLevelHookCallback, reinterpret_cast<LPVOID*>(&g_pfnOriginalTopLevelFunc)) != MH_OK) {
        printf("[ERROR] ��������Hookʧ�ܡ�\n");
        return;
    }

    if (MH_EnableHook(pTarget) != MH_OK) {
        printf("[ERROR] ���ö���Hookʧ�ܡ�\n");
        return;
    }

    printf("[INFO] ����Hook�ѳɹ�������\n");
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
    printf("[INFO] ����Hook��ֹͣ��\n");
}
