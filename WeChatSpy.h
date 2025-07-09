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


// ===================================================================
//                      ������������Hook���ݽṹ
// ===================================================================
// ����ṹ����������̼߳䴫����Ϣ�����ָ��
struct QueuedMsg {
    void* msg_1;
    void* msg_obj_ptr; // �����������ص��Ĳ��� a2
	void* msg_2; // ��������������
};



// ===================================================================
//              ��������΢����Ϣ�ṹ���ڴ�ƫ����
// ===================================================================
// �����˴Ӷ���Hook��������a2ָ����ڴ��ַ��ʼ��������Ϣ�ֶε�ƫ��
// ע�⣺��Щƫ������Ҫ����ض��汾��WeChatWin.dll������֤
namespace MsgStructOffsets {
    constexpr QWORD id = 0x30;   // ��ϢID (uint64_t)
    constexpr QWORD type = 0x38;   // ��Ϣ���� (uint32_t)
    constexpr QWORD roomId = 0x48;   // ԭʼ����ID/˽��wxid (WxString*)
    constexpr QWORD content = 0x88;   // ��Ϣ���� (WxString*)
    constexpr QWORD sender = 0x240;  // (Ⱥ����)������wxid (WxString*)
    constexpr QWORD clientMsgId = 0x260;  // �ͻ������ɵ���ϢID (WxString*)
    constexpr QWORD xml = 0x308;  // XML���ݣ����ڿ�Ƭ��ϵͳ��Ϣ�� (WxString*)
}
// ===================================================================

using TopLevelFunc_t = char(__fastcall*)(void* a1, void* a2, void* a3);

// ������������Hookʵ���������ֹͣ����
void StartTopLevelHook();
void StopTopLevelHook();


// �������̼߳䴫�����ص��Ĳ���
struct CapturedParams {
    void* p1;
    unsigned int p2;
    QWORD p3;
    DWORD p4;
    QWORD p5;
};

void CreateDebugConsole();





// ����ҪHook��Ŀ�꺯����ָ������ (5������)
using LowLevelFunc_t = char(__fastcall*)(void* a1, unsigned int a2, QWORD a3, DWORD a4, QWORD a5);








using GetValueFunc_t = __int64(__fastcall*)(void* a1, int a2);

// ������������ֵHook��������ֹͣ����
void StartReturnValueHook();
void StopReturnValueHook();


struct CapturedReturnValue {
    void* p1;          // ���� a1
    int p2;            // ���� a2
    __int64 ret_val;   // �����ķ���ֵ
};


