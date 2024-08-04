/*=============================================================================
	UnThreadingBaseWindows.h: Unreal Tournament Threading Support
	Copyright 2021-2022 OldUnreal. All Rights Reserved.

	Revision history:
		* Added function implementations for DNF
		* Created by Stijn Volckaert
=============================================================================*/

#ifndef _UNTHREADINGWINDOWS_H
#define _UNTHREADINGWINDOWS_H

/*----------------------------------------------------------------------------
	Windows-specific classes
----------------------------------------------------------------------------*/

class FRunnableThreadWin : public FRunnableThread
{
	HANDLE Thread{};
	FRunnable* Runnable{};
	UBOOL bShouldDeleteSelf{};
	UBOOL bShouldDeleteRunnable{};

	static DWORD STDCALL ThreadProc(LPVOID Params)
	{
		check(Params);
		return ((FRunnableThreadWin*)Params)->Run();
	}

	DWORD Run()
	{
		check(Runnable);
		DWORD Result = 0;

		if (Runnable->Init())
		{
			Result = Runnable->Run();
			Runnable->Exit();
		}

		if (bShouldDeleteSelf)
			Kill(0, 0);

		return Result;
	}

public:
	FRunnableThreadWin()
	{
	}

	~FRunnableThreadWin()
	{
		Kill(TRUE, INFINITE);
	}

	UBOOL Create(FRunnable* InRunnable, UBOOL bAutoDeleteSelf = FALSE, UBOOL bAutoDeleteRunnable = FALSE, DWORD StackSize = 0)
	{
		DWORD ThreadId;
		Runnable = InRunnable;
		bShouldDeleteSelf = bAutoDeleteSelf;
		bShouldDeleteRunnable = bAutoDeleteRunnable;
		Thread = CreateThread(NULL, StackSize, &ThreadProc, this, 0, &ThreadId);
		return Thread != NULL;
	}

	virtual void Suspend(UBOOL bShouldPause = TRUE)
	{
		check(Thread);

		if (bShouldPause)
			SuspendThread(Thread);
		else
			ResumeThread(Thread);
	}

	virtual UBOOL Kill(UBOOL bShouldWait = FALSE, DWORD MaxWaitTime = 0)
	{
		check(Thread && Runnable);

		UBOOL Result = TRUE;
		Runnable->Stop();

		DWORD ExitCode;
		if ((bShouldWait && WaitForSingleObject(Thread, MaxWaitTime) == WAIT_TIMEOUT) || GetExitCodeThread(Thread, &ExitCode) == STILL_ACTIVE)
		{
			TerminateThread(Thread, INFINITE);
			Result = FALSE;
		}

		CloseHandle(Thread);
		Thread = NULL;

		if (bShouldDeleteRunnable)
		{
			delete Runnable;
			Runnable = NULL;
		}

		if (bShouldDeleteSelf)
			GThreadFactory->Destroy(this);

		return Result;
	}

	virtual void WaitForCompletion()
	{
		check(Thread);
		WaitForSingleObject(Thread, INFINITE);
	}
};

class FThreadFactoryWin : public FThreadFactory
{
public:
	virtual FRunnableThread* CreateThread(FRunnable* InRunnable, UBOOL bAutoDeleteSelf = FALSE, UBOOL bAutoDeleteRunnable = FALSE, DWORD StackSize = 0)
	{
		auto Thread = new FRunnableThreadWin;

		if (!Thread->Create(InRunnable, bAutoDeleteSelf, bAutoDeleteRunnable, StackSize))
		{
			Destroy(Thread);
			return NULL;
		}

		return Thread;
	}

	virtual void Destroy(FRunnableThread* Thread)
	{
		if (Thread)
		{
			delete Thread;
		}
	}
};

#endif
