/*
	#This is used for Asynchronous Execution of methods
*/
#include <thread>
#include <chrono>
#include <functional>
using namespace std;

class Timer
{
	thread Thread;
	bool isAlive = false; //State if the timer
	long CallNumber = -1L, //Used to indicate how any times we execute a particular function. The '-1' is arbitrary
		Count = -1L; //To count how many times a function has been called
	
	chrono::microseconds interval = chrono::microseconds(0); //Default interval between function interval = 0
	function<void(void)> funct = nullptr;
	
	void Exec()
	{
		this_thread::sleep_for(interval);
		if (isAlive)
			Function()();
	}

	void ThreadFunction() //Function that will be passed to the thread
	{
		if (CallNumber == Infinite)
			while (isAlive)
				Exec();
		else
			while (Count--)
				Exec();
	}

public:
	static const long Infinite = -1L;
	
	Timer() {} //Empty constructor to create a Timer instance as a global variable

	Timer(const function<void(void)>& f) : funct(f) {}  //Accepts a pointer to a function. (Reference to a constant function object)

	Timer(const function<void(void)>& f,
		const unsigned long& i,
		const long repeat_count = Timer::Infinite) : funct(f), interval(chrono::milliseconds(i)), CallNumber(repeat_count) {}

	void Timer_Start(bool Async = true)
	{
		if (IsAlive())
			return;
		isAlive = true;
		Count = CallNumber;
		if (Async)
			Thread = thread(&Timer::ThreadFunction, this);
		else
			this->ThreadFunction();
	}

	void Timer_Stop()
	{
		isAlive = false;
		Thread.join();
	}

	bool IsAlive() const
	{
		return isAlive;
	}

	void CountRepeat(const long r)
	{
		if (isAlive)
			return;
		CallNumber = r;
	}

	void SetFunction(const function<void(void)> &f)
	{
		funct = f;
	}

	long GetRemainingCount() const
	{
		return Count;
	}

	long RepeatCount() const
	{
		return CallNumber;
	}

	void SetInterval( const unsigned long &i)
	{
		if (isAlive)
			return;
		interval = chrono::milliseconds(i);
	}

	unsigned long Interval() const
	{
		return interval.count();
	}

	//This function returns a function
	const function <void(void)>& Function() const
	{
		return funct;
	}
};
