

void demo() {

//#include "WeChatSpy.h"
//#include "MinHook.h"
//#include <cstdio>
//#include <string>

    //// --- ȫ�ֱ��� ---
    //static TopLevelFunc_t g_pfnOriginalTopLevelFunc = nullptr;
    //// **ע��**: �����ַ��Ҫ���Լ�ͨ��x64dbg��IDAȷ�ϡ�
    //// `sub_1825B90E0` �е� `1825B90E0` ��һ�����غ�ľ��Ե�ַ��������Ҫ�������ƫ�ơ�
    //// ���� WeChatWin.dll ����ַ�� 0x180000000, ��ôƫ�ƾ��� 0x25B90E0��
    //// ������滻�����ҵ�����ȷƫ�ƣ�
    //constexpr QWORD g_TopLevelFuncOffset = 0x25B90E0; // <--- ���滻�˵�ַ!

    //// --- �������� ---
    //// ��WeChatFerry����ġ����ڽ����ڴ��п��ַ����ĺ���
    //std::string GetStringFromWx(QWORD addr) {
    //    if (!addr) return "";
    //    wchar_t** p = (wchar_t**)addr;
    //    if (!p || !*p) return "";

    //    std::wstring wstr(*p);
    //    // wstringתstring (UTF-8)
    //    if (wstr.empty()) return std::string();
    //    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    //    std::string strTo(size_needed, 0);
    //    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    //    return strTo;
    //}

    //// --- ���ǵĻص����� ---
    //char __fastcall MyTopLevelHookCallback(void* a1, void* a2, void* a3)
    //{
    //    printf("--- TopLevelHook ������---\n");
    //    printf("  �ѳɹ����ص����㺯�����á�\n");
    //    printf("  ���� a2 (��Ϣ����ָ��): 0x%p\n", a2);

    //    // a2 ���ǰ�����������Ϣ�������ṹ��ָ�룡
    //    // �������ǿ�����WeChatFerryһ����ͨ��ƫ��������������
    //    // ����ƫ��������WeChatFerry��������ʾ
    //    QWORD base = (QWORD)a2;
    //    try {
    //        uint64_t msgId = *(QWORD*)(base + 0x30);
    //        uint32_t msgType = *(DWORD*)(base + 0x38);
    //        std::string roomid = GetStringFromWx(base + 0x48);
    //        std::string content = GetStringFromWx(base + 0x88);
    //        std::string sender = GetStringFromWx(base + 0x240);

    //        printf("\n  --- ���� a2 �ɹ� ---\n");
    //        printf("    ��ϢID: %llu\n", msgId);
    //        printf("    ��Ϣ����: %u\n", msgType);
    //        printf("    ԭʼȺID/������: %s\n", roomid.c_str());
    //        printf("    ��Ϣ����: %s\n", content.c_str());
    //        printf("    (Ⱥ��)������: %s\n", sender.c_str());
    //        printf("  ---------------------\n\n");
    //    }
    //    catch (...) {
    //        printf("[����] ���� a2 �ṹʱ�����쳣��\n");
    //    }

    //    // ����ԭʼ��������΢�ż�������
    //    return g_pfnOriginalTopLevelFunc(a1, a2, a3);
    //}

    //// --- ����/ֹͣHook�ĺ��� ---
    //void StartTopLevelHook() {
    //    if (MH_Initialize() != MH_OK) {
    //        printf("[ERROR] MinHook ��ʼ��ʧ�ܡ�\n");
    //        return;
    //    }

    //    HMODULE hWeChatWin = GetModuleHandle(L"WeChatWin.dll");
    //    if (!hWeChatWin) {
    //        printf("[ERROR] δ�ܻ�ȡ WeChatWin.dll �����\n");
    //        return;
    //    }

    //    LPVOID pTarget = (LPVOID)((QWORD)hWeChatWin + g_TopLevelFuncOffset);

    //    if (MH_CreateHook(pTarget, &MyTopLevelHookCallback, reinterpret_cast<LPVOID*>(&g_pfnOriginalTopLevelFunc)) != MH_OK) {
    //        printf("[ERROR] ��������Hookʧ�ܡ�\n");
    //        return;
    //    }

    //    if (MH_EnableHook(pTarget) != MH_OK) {
    //        printf("[ERROR] ���ö���Hookʧ�ܡ�\n");
    //        return;
    //    }

    //    printf("[INFO] ����Hook�ѳɹ�������\n");
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
    //    printf("[INFO] ����Hook��ֹͣ��\n");
    //}

}