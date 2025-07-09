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


// ===================================================================
//                      【新增】顶层Hook数据结构
// ===================================================================
// 这个结构体仅用于在线程间传递消息对象的指针
struct QueuedMsg {
    void* msg_1;
    void* msg_obj_ptr; // 就是我们拦截到的参数 a2
	void* msg_2; // 可能是其他参数
};



// ===================================================================
//              【新增】微信消息结构体内存偏移量
// ===================================================================
// 描述了从顶层Hook函数参数a2指向的内存地址开始，各个消息字段的偏移
// 注意：这些偏移量需要针对特定版本的WeChatWin.dll进行验证
namespace MsgStructOffsets {
    constexpr QWORD id = 0x30;   // 消息ID (uint64_t)
    constexpr QWORD type = 0x38;   // 消息类型 (uint32_t)
    constexpr QWORD roomId = 0x48;   // 原始房间ID/私聊wxid (WxString*)
    constexpr QWORD content = 0x88;   // 消息内容 (WxString*)
    constexpr QWORD sender = 0x240;  // (群聊中)发送者wxid (WxString*)
    constexpr QWORD clientMsgId = 0x260;  // 客户端生成的消息ID (WxString*)
    constexpr QWORD xml = 0x308;  // XML内容，用于卡片、系统消息等 (WxString*)
}
// ===================================================================

using TopLevelFunc_t = char(__fastcall*)(void* a1, void* a2, void* a3);

// 【新增】顶层Hook实验的启动和停止函数
void StartTopLevelHook();
void StopTopLevelHook();


// 用于在线程间传递拦截到的参数
struct CapturedParams {
    void* p1;
    unsigned int p2;
    QWORD p3;
    DWORD p4;
    QWORD p5;
};

void CreateDebugConsole();





// 我们要Hook的目标函数的指针类型 (5个参数)
using LowLevelFunc_t = char(__fastcall*)(void* a1, unsigned int a2, QWORD a3, DWORD a4, QWORD a5);








using GetValueFunc_t = __int64(__fastcall*)(void* a1, int a2);

// 【新增】返回值Hook的启动和停止函数
void StartReturnValueHook();
void StopReturnValueHook();


struct CapturedReturnValue {
    void* p1;          // 参数 a1
    int p2;            // 参数 a2
    __int64 ret_val;   // 函数的返回值
};


