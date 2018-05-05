#pragma  once

#include <Windows.h>


//////////////////////////////////////////////////////////////////////////
/*     CriticalSection operator*/
//////////////////////////////////////////////////////////////////////////
#define  CubeInitializeCriticalSection(x)	InitializeCriticalSection(x)
#define  CubeEnterCriticalSection(x)		EnterCriticalSection(x)
#define  CubeLeaveCriticalSection(x)		LeaveCriticalSection(x)
typedef  CRITICAL_SECTION            CubeCriticalSection; 

class Cube_Thread
{
public:
	Cube_Thread();
	virtual ~Cube_Thread();

	void start();
	void terminate();
	bool IsRunning();
	void SetRunningFlag();
	virtual void run(){};
	virtual void stop(){};
	virtual void free(){};

	void  __ThreadFlagRemove();

	void  WaitForExit();
private:
	bool   m_IsRunning;
	HANDLE m_ThreadHandle;
	DWORD  m_ThreadID;
};