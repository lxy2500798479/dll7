#pragma once

// ===================================================================
//                      ����ͷ�ļ�������
// ===================================================================

// 1. ���� WIN32_LEAN_AND_MEAN ������ windows.h ������
#define WIN32_LEAN_AND_MEAN

// 2. �ڰ��� windows.h ֮ǰ���Ȱ����°汾�� winsock2.h
#include <winsock2.h>
#include <windows.h>

// 3. ����������Ҫ�ı�׼��ͷ�ļ�
#include <cstdint>
#include <string>
#include <vector>

// ===================================================================
//                      ���������궨��
// ===================================================================
#ifdef WECHATSPY_EXPORTS
#define WCF_API __declspec(dllexport)
#else
#define WCF_API __declspec(dllimport)
#endif

typedef uint64_t QWORD;

class WxString {
public:
    const wchar_t* wptr;
    DWORD size;
    DWORD length;
    char* ptr;
    DWORD clen;
    WxString(const std::wstring& ws) : wptr(ws.c_str()), size(static_cast<DWORD>(ws.size())), length(static_cast<DWORD>(ws.length())), ptr(nullptr), clen(0) {}
};

// �����Լ�����Ϣ�ṹ��
struct WxMsg {
    bool is_self;
    bool is_group;
    uint32_t type;
    uint32_t ts;
    uint64_t id;
    std::string sender;
    std::string roomid;
    std::string content;
    std::string xml;
};

namespace Offsets {
    constexpr QWORD SendMsgMgr = 0x1B5B210;
    constexpr QWORD SendText = 0x22D4A90;
    constexpr QWORD FreeChatMsg = 0x1B5CAC0;
}

using SendMsgMgr_t = QWORD(*)();
using SendText_t = QWORD(*)(QWORD, WxString*, WxString*, QWORD, int, int, QWORD, QWORD);
using FreeChatMsg_t = QWORD(*)(QWORD);

void SendTextMessage(const std::wstring& wxid, const std::wstring& msg);
// ��ʵ����/Σ�ա�������ʾ�����ڴ��ͷŵĺ���
void SendTextMessage_And_Crash_With_Free(const std::wstring& wxid, const std::wstring& msg);






