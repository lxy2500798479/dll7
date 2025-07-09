#pragma once
#include "WeChatSpy.h" // 为了使用 WxMsg 结构体

// 【推荐】将完整的URL定义为一个宏，配置更简单
#define WEBHOOK_URL "http://localhost:3000/webhook"

// 启动后台发送线程
void StartAsyncSender();

// 停止后台发送线程（在DLL卸载时调用）
void StopAsyncSender();

// 供Hook回调函数调用的函数，将消息放入队列
void QueueMessageForSending(const WxMsg& msg);