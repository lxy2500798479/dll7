#pragma once
#include "WeChatSpy.h" // Ϊ��ʹ�� WxMsg �ṹ��

// ���Ƽ�����������URL����Ϊһ���꣬���ø���
#define WEBHOOK_URL "http://localhost:3000/webhook"

// ������̨�����߳�
void StartAsyncSender();

// ֹͣ��̨�����̣߳���DLLж��ʱ���ã�
void StopAsyncSender();

// ��Hook�ص��������õĺ���������Ϣ�������
void QueueMessageForSending(const WxMsg& msg);