#include "WeChatSpy.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <locale.h> // **��������** ���� locale.h ͷ�ļ�

void CreateDebugConsole()
{
    if (AllocConsole()) {
        FILE* pFile;

        freopen_s(&pFile, "CONOUT$", "w", stdout);
        freopen_s(&pFile, "CONOUT$", "w", stderr);
        freopen_s(&pFile, "CONIN$", "r", stdin);

        SetConsoleTitle((LPCSTR)"WeChatSpy Debug Console");

        // **���������� 1��** ���ÿ���̨���������ҳΪUTF-8
        // ��һ�����߿���̨���ڣ�������UTF-8�ġ����塱����ʾ�ַ���
        SetConsoleOutputCP(CP_UTF8);

        // **���������� 2��** ����C/C++��׼�����������ΪUTF-8
        // ��һ������ printf / std::cout������ѽ��յ����ֽ�������UTF-8��������
        // ".UTF8" ��һ����׼��locale�ַ���
        setlocale(LC_ALL, ".UTF8");

        printf("--- Debug Console Initialized ---\n");
        printf("����������ΪUTF-8������Ӧ����ȷ��ʾ���ġ�\n");
        printf("----------------------------------\n\n");
    }
}