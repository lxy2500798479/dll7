#include "WeChatSpy.h"
#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <locale.h> // **【新增】** 包含 locale.h 头文件

void CreateDebugConsole()
{
    if (AllocConsole()) {
        FILE* pFile;

        freopen_s(&pFile, "CONOUT$", "w", stdout);
        freopen_s(&pFile, "CONOUT$", "w", stderr);
        freopen_s(&pFile, "CONIN$", "r", stdin);

        SetConsoleTitle((LPCSTR)"WeChatSpy Debug Console");

        // **【核心修正 1】** 设置控制台的输出代码页为UTF-8
        // 这一步告诉控制台窗口：“请用UTF-8的“字体”来显示字符”
        SetConsoleOutputCP(CP_UTF8);

        // **【核心修正 2】** 设置C/C++标准库的区域设置为UTF-8
        // 这一步告诉 printf / std::cout：“请把接收到的字节流当做UTF-8来解析”
        // ".UTF8" 是一个标准的locale字符串
        setlocale(LC_ALL, ".UTF8");

        printf("--- Debug Console Initialized ---\n");
        printf("编码已设置为UTF-8，现在应能正确显示中文。\n");
        printf("----------------------------------\n\n");
    }
}