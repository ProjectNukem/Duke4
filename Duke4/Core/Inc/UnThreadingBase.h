/*=============================================================================
	UnThreadingBase.h: Unreal Tournament Threading Support
	Copyright 2021-2022 OldUnreal. All Rights Reserved.

	This header describes the interfaces for the subset of UE3's
	synchronization	primitives we implemented for Unreal Tournament 469.

	Most of Unreal Engine 1's subsystems are NOT thread-safe. This means there
	are very few cases where it makes sense to use threads!

	Revision history:
		* Trimmed down for DNF
		* Created by Stijn Volckaert
=============================================================================*/

#ifndef _UNTHREADINGBASE_H
#define _UNTHREADINGBASE_H

#include "FPlatformAtomics.h"

/*----------------------------------------------------------------------------
	Threading support classes. Where possible, we use the same names as UE3
----------------------------------------------------------------------------*/

// Simple atomic counter
template <typename INTTYPE> class TThreadSafeCounter
{
public:
	TThreadSafeCounter()                 { Value = 0; }
	TThreadSafeCounter(INTTYPE Def)      { Value = Def; }
	INTTYPE GetValue() const             { return FPlatformAtomics::AtomicRead(&Value); }
	INTTYPE Increment(INTTYPE Amount)    { return FPlatformAtomics::InterlockedIncrement(&Value); }
	INTTYPE Decrement(INTTYPE Amount)    { return FPlatformAtomics::InterlockedDecrement(&Value); }
private:	
	volatile INTTYPE Value;
};
using FThreadSafeCounter = TThreadSafeCounter<INT>;

// Base instance for a runnable object
class CORE_API FRunnable
{
public:
	virtual ~FRunnable() = default;
	
	// Initializes the runnable. Called in the context of the thread that runs the runnable.
	virtual UBOOL Init() = 0;

	// Does the actual work. Called in the context of the thread that runs the runnable. This is where your "thread function" should go
	virtual DWORD Run() = 0;

	// Called in the context of the main thread if a thread should terminate early
	virtual UBOOL Stop() = 0;

	// Called in the context of the thread that runs the runnable
	virtual UBOOL Exit() = 0;	
};

// Base instance for a thread
class CORE_API FRunnableThread
{
public:
	virtual ~FRunnableThread() = default;
	
	// If bShouldPause is TRUE, this suspends execution of the thread. Otherwise, it resumes execution of the thread
	virtual void Suspend(UBOOL bShouldPause) = 0;

	// Shut the thread down. We can do this gracefully by setting bShouldWait to TRUE and MaxWaitTime to INFINITE.
	// We can also do it forcefully by setting bShouldWait to FALSE or by setting MaxWaitTime to a value below infinite.
	// NOTE: If you force a thread shutdown, it will not get a chance to clean up its resources (e.g., thread stack, TLS, ...)
	// Returns TRUE if the thread was shut down gracefully
	virtual UBOOL Kill(UBOOL bShouldWait = FALSE, DWORD MaxWaitTime = 0) = 0;

	// Waits for the thread to finish its work
	virtual void WaitForCompletion() = 0;
};

// Creates FRunnableThreads that execute FRunnables
class CORE_API FThreadFactory
{
public:
	virtual ~FThreadFactory() = default;

	// InRunnable is the runnable we want to execute
	// If bAutoDeleteSelf is TRUE, the FRunnableThread object (and its underlying thread) will be cleaned up automatically when the thread has completed
	// If bAutoDeleteRunnable is TRUE, the FRunnable object will be cleaned up automatically when the work has completed
	// StackSize can be set to a value above 0 to specify a custom stack size
	virtual FRunnableThread* CreateThread(FRunnable* InRunnable, UBOOL bAutoDeleteSelf = FALSE, UBOOL bAutoDeleteRunnable = FALSE, DWORD StackSize = 0) = 0;

	// Destroys the FRunnableThread object
	virtual void Destroy(FRunnableThread* InThread) = 0;
};

#endif
