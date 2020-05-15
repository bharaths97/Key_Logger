/*
	#This is the helper header where all necessary utility and functions are declared and defined
*/
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

namespace KeyLogger
{
	template <class T>
	string ToString(const T&);
	
	struct DateTime
	{
		int day, month, year, minute, hour, seconds;
		//Constructor for the Current Local date and time
		DateTime()
		{
			time_t sec;
			time(&sec);

			struct tm* info = localtime(&sec);

			day = info->tm_mday,
			month = info->tm_mon + 1, // Adding one as months index starts fom 0
			year = 1900 + info->tm_year, //Adding 1900 since tm_year is relative to 1900 on all POSIX compliant platforms
			minute = info->tm_min,
			hour = info->tm_hour,
			seconds = info->tm_sec;
		}

		//Used when all parameters are passed for the constructor
		DateTime(int d, int m, int y, int h, int M, int s) : day(d), month(m), year(h), hour(h), minute(M), seconds(s) {}
		//Used when only date, month and year are passed and time is set to 0
		DateTime(int d, int m, int y) : day(d), month(m), year(y), hour(0), minute(0), seconds(0) {}

		//Getting current date and time
		DateTime current() const //readonly
		{
			return DateTime();
		}

		//Formatting Date into Human Readable format DD/MM/YY
		string GetDateString() const
		{
			return	string(day < 10 ? "0" : "") + ToString(day) +
					string(month < 10 ? ".0" : ".") + ToString(month) + "." +
					ToString(year);
		}

		//Formatting Time into Human ReadableFormat HH:MM:SS
		string GetTimeString(const string &separator =  ":") const
		{
			return	string(hour < 10 ? "0" : "") + ToString(hour) + separator +
				string(minute < 10 ? "0" : "") + ToString(minute) + separator +
				string(seconds < 10 ? "0" : "") + ToString(seconds);
		}

		//Joining Formatted date and time
		string GetDateTimeString(const string &separator = ":") const
		{
			return GetDateString() + " " + GetTimeString(separator);
		}
	};

	template <class T>
	string ToString(const T &inp)
	{
		ostringstream OutStr;
		OutStr << inp;
		return OutStr.str();
	}

	//Function to log the functioning of the KeyLogger for easier Debugging. # not the log for logging the keys
	void InfoLogger(const string& logline)
	{
		ofstream filelog("KeyLoggerApplication.log", ios::app);
		filelog << "[" << KeyLogger::DateTime().GetDateTimeString() << "] [" << ToString(GetCurrentProcessId()) << "] [INFO] " <<logline << endl;
		filelog.close();
	}
	void ErrorLogger(const string& logline)
	{
		ofstream filelog("KeyLoggerApplication.log", ios::app);
		filelog << "[" << KeyLogger::DateTime().GetDateTimeString() << "] [" << ToString(GetCurrentProcessId()) << "] [ERROR] " << logline << endl;
		filelog.close();
	}
}