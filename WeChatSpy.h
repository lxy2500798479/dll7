#pragma once

// ===================================================================
//                      核心头文件修正区
// ===================================================================

// 1. 定义 WIN32_LEAN_AND_MEAN 来精简 windows.h 的内容
#define WIN32_LEAN_AND_MEAN

// 2. 在包含 windows.h 之前，先包含新版本的 winsock2.h
#include <winsock2.h>
#include <windows.h>

// 3. 包含其他必要的标准库头文件
#include <cstdint>
#include <string>
#include <vector>

// ===================================================================
//                      导出函数宏定义
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

// 我们自己的消息结构体
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
// 【实验性/危险】用于演示错误内存释放的函数
void SendTextMessage_And_Crash_With_Free(const std::wstring& wxid, const std::wstring& msg);






