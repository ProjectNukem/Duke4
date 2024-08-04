/*=============================================================================
	FPlatformAtomics.h: Unreal Tournament atomics.

	Adapted from the public Unreal Engine 5 sources.
=============================================================================*/

#ifndef _INCL_PLATFORM_ATOMICS_H_
#define _INCL_PLATFORM_ATOMICS_H_

static_assert(sizeof(BYTE)   == sizeof(char)      && alignof(BYTE)   == alignof(char),      "BYTE must be compatible with char");
static_assert(sizeof(SWORD)  == sizeof(short)     && alignof(SWORD)  == alignof(short),     "SWORD must be compatible with short");
#if _MSC_VER
static_assert(sizeof(INT)    == sizeof(long)      && alignof(INT)    == alignof(long),      "INT must be compatible with long");
#endif
static_assert(sizeof(SQWORD) == sizeof(long long) && alignof(SQWORD) == alignof(long long), "SQWORD must be compatible with long long");

// Undo winnt.h defines
#undef InterlockedIncrement
#undef InterlockedDecrement
#undef InterlockedAdd
#undef InterlockedExchange
#undef InterlockedCompareExchange
#undef InterlockedAnd
#undef InterlockedOr
#undef InterlockedXor
#undef InterlockedCompareExchangePointer

#ifdef _MSC_VER
struct FWindowsPlatformAtomics
{
	static FORCEINLINE BYTE  InterlockedIncrement( volatile BYTE* Value )                { return (BYTE)::_InterlockedExchangeAdd8((char*)Value, 1) + 1; }
	static FORCEINLINE SWORD InterlockedIncrement( volatile SWORD* Value )               { return (SWORD)::_InterlockedIncrement16((short*)Value); }
	static FORCEINLINE INT   InterlockedIncrement( volatile INT* Value )                 { return (INT)::_InterlockedIncrement((long*)Value); }
	static FORCEINLINE BYTE  InterlockedDecrement( volatile BYTE* Value )                { return (BYTE)::_InterlockedExchangeAdd8((char*)Value, -1) - 1; }
	static FORCEINLINE SWORD InterlockedDecrement( volatile SWORD* Value )               { return (SWORD)::_InterlockedDecrement16((short*)Value); }
	static FORCEINLINE INT   InterlockedDecrement( volatile INT* Value )                 { return (INT)::_InterlockedDecrement((long*)Value); }
	static FORCEINLINE BYTE  InterlockedAdd( volatile BYTE* Value, BYTE Amount )         { return (BYTE)::_InterlockedExchangeAdd8((char*)Value, (char)Amount); }
	static FORCEINLINE SWORD InterlockedAdd( volatile SWORD* Value, SWORD Amount )       { return (SWORD)::_InterlockedExchangeAdd16((short*)Value, (short)Amount); }
	static FORCEINLINE INT   InterlockedAdd( volatile INT* Value, INT Amount )           { return (INT)::_InterlockedExchangeAdd((long*)Value, (long)Amount); }
	static FORCEINLINE BYTE  InterlockedExchange( volatile BYTE* Value, BYTE Exchange )  { return (BYTE)::_InterlockedExchange8((char*)Value, (char)Exchange); }
	static FORCEINLINE SWORD InterlockedExchange( volatile SWORD* Value, SWORD Exchange ){ return (SWORD)::_InterlockedExchange16((short*)Value, (short)Exchange); }
	static FORCEINLINE INT   InterlockedExchange( volatile INT* Value, INT Exchange )    { return (INT)::_InterlockedExchange((long*)Value, (long)Exchange); }
	static FORCEINLINE BYTE  InterlockedCompareExchange( volatile BYTE* Dest, BYTE Exchange, BYTE Comparand )    { return (BYTE)::_InterlockedCompareExchange8((char*)Dest, (char)Exchange, (char)Comparand); }
	static FORCEINLINE SWORD InterlockedCompareExchange( volatile SWORD* Dest, SWORD Exchange, SWORD Comparand ) { return (SWORD)::_InterlockedCompareExchange16((short*)Dest, (short)Exchange, (short)Comparand); }
	static FORCEINLINE INT   InterlockedCompareExchange( volatile INT* Dest, INT Exchange, INT Comparand )       { return (INT)::_InterlockedCompareExchange((long*)Dest, (long)Exchange, (long)Comparand); }
	static FORCEINLINE BYTE  InterlockedAnd(volatile BYTE* Value, const BYTE AndValue)   { return (BYTE)::_InterlockedAnd8((volatile char*)Value, (char)AndValue); }
	static FORCEINLINE SWORD InterlockedAnd(volatile SWORD* Value, const SWORD AndValue) { return (SWORD)::_InterlockedAnd16((volatile short*)Value, (short)AndValue); }
	static FORCEINLINE INT   InterlockedAnd(volatile INT* Value, const INT AndValue)     { return (INT)::_InterlockedAnd((volatile long*)Value, (long)AndValue); }
	static FORCEINLINE BYTE  InterlockedOr(volatile BYTE* Value, const BYTE OrValue)     { return (BYTE)::_InterlockedOr8((volatile char*)Value, (char)OrValue); }
	static FORCEINLINE SWORD InterlockedOr(volatile SWORD* Value, const SWORD OrValue)   { return (SWORD)::_InterlockedOr16((volatile short*)Value, (short)OrValue); }
	static FORCEINLINE INT   InterlockedOr(volatile INT* Value, const INT OrValue)       { return (INT)::_InterlockedOr((volatile long*)Value, (long)OrValue); }
	static FORCEINLINE BYTE  InterlockedXor(volatile BYTE* Value, const BYTE XorValue)   { return (BYTE)::_InterlockedXor8((volatile char*)Value, (char)XorValue); }
	static FORCEINLINE SWORD InterlockedXor(volatile SWORD* Value, const SWORD XorValue) { return (SWORD)::_InterlockedXor16((volatile short*)Value, (short)XorValue); }
	static FORCEINLINE INT   InterlockedXor(volatile INT* Value, const INT XorValue)     { return (INT)::_InterlockedXor((volatile long*)Value, (INT)XorValue); }
	static FORCEINLINE BYTE  AtomicRead(volatile const BYTE* Src)                        { return InterlockedCompareExchange((BYTE*)Src, 0, 0); }
	static FORCEINLINE SWORD AtomicRead(volatile const SWORD* Src)                       { return InterlockedCompareExchange((SWORD*)Src, 0, 0); }
	static FORCEINLINE INT   AtomicRead(volatile const INT* Src)                         { return InterlockedCompareExchange((INT*)Src, 0, 0); }
	static FORCEINLINE BYTE  AtomicRead_Relaxed(volatile const BYTE* Src)                { return *Src; }
	static FORCEINLINE SWORD AtomicRead_Relaxed(volatile const SWORD* Src)               { return *Src; }
	static FORCEINLINE INT   AtomicRead_Relaxed(volatile const INT* Src)                 { return *Src; }
	static FORCEINLINE void  AtomicStore(volatile BYTE* Src, BYTE Val)                   { InterlockedExchange(Src, Val); }
	static FORCEINLINE void  AtomicStore(volatile SWORD* Src, SWORD Val)                 { InterlockedExchange(Src, Val); }
	static FORCEINLINE void  AtomicStore(volatile INT* Src, INT Val)                     { InterlockedExchange(Src, Val); }
	static FORCEINLINE void  AtomicStore(volatile SQWORD* Src, SQWORD Val)               { InterlockedExchange(Src, Val); }
	static FORCEINLINE void  AtomicStore_Relaxed(volatile BYTE* Src, BYTE Val)           { *Src = Val; }
	static FORCEINLINE void  AtomicStore_Relaxed(volatile SWORD* Src, SWORD Val)         { *Src = Val; }
	static FORCEINLINE void  AtomicStore_Relaxed(volatile INT* Src, INT Val)             { *Src = Val; }

	// Pointer atomics
	static FORCEINLINE void* InterlockedExchangePtr( void*volatile* Dest, void* Exchange )
	{
		return ::_InterlockedExchangePointer(Dest, Exchange);
	}

	static FORCEINLINE void* InterlockedCompareExchangePointer( void*volatile* Dest, void* Exchange, void* Comparand )
	{
		return ::_InterlockedCompareExchangePointer(Dest, Exchange, Comparand);
	}

	//
	// 64 bit workarounds for 32 bit targets
	//
	static FORCEINLINE SQWORD InterlockedDecrement( volatile SQWORD* Value )
	{
#if _WIN64
		return (SQWORD)::_InterlockedDecrement64((long long*)Value);
#else
		for (;;) // No explicit instruction for 64-bit atomic decrement on 32-bit processors; has to be implemented in terms of CMPXCHG8B
		{
			SQWORD OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue - 1, OldValue) == OldValue)
				return OldValue - 1;
		}
#endif
	}

	static FORCEINLINE SQWORD InterlockedIncrement( volatile SQWORD* Value )
	{
#if _WIN64
		return (SQWORD)::_InterlockedIncrement64((long long*)Value);
#else
		for (;;) // No explicit instruction for 64-bit atomic increment on 32-bit processors; has to be implemented in terms of CMPXCHG8B
		{
			SQWORD OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue + 1, OldValue) == OldValue)
				return OldValue + 1;
		}
#endif
	}

	static FORCEINLINE SQWORD InterlockedAdd( volatile SQWORD* Value, SQWORD Amount )
	{
#if _WIN64
		return (SQWORD)::_InterlockedExchangeAdd64((SQWORD*)Value, (SQWORD)Amount);
#else
		// No explicit instruction for 64-bit atomic add on 32-bit processors; has to be implemented in terms of CMPXCHG8B
		for (;;)
		{
			SQWORD OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue + Amount, OldValue) == OldValue)
				return OldValue;
		}
#endif
	}

	static FORCEINLINE SQWORD InterlockedExchange( volatile SQWORD* Value, SQWORD Exchange )
	{
#if _WIN64
		return (SQWORD)::_InterlockedExchange64((long long*)Value, (long long)Exchange);
#else
		// No explicit instruction for 64-bit atomic exchange on 32-bit processors; has to be implemented in terms of CMPXCHG8B
		for (;;)
		{
			SQWORD OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, Exchange, OldValue) == OldValue)
				return OldValue;
		}
#endif
	}

	static FORCEINLINE SQWORD InterlockedCompareExchange( volatile SQWORD* Dest, SQWORD Exchange, SQWORD Comparand )
	{
		return (SQWORD)::_InterlockedCompareExchange64(Dest, Exchange, Comparand);
	}

	static FORCEINLINE SQWORD InterlockedAnd(volatile SQWORD* Value, const SQWORD AndValue)
	{
#if _WIN64
		return (SQWORD)::_InterlockedAnd64((volatile long long*)Value, (long long)AndValue);
#else
		// No explicit instruction for 64-bit atomic and on 32-bit processors; has to be implemented in terms of CMPXCHG8B
		for (;;)
		{
			const SQWORD OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue & AndValue, OldValue) == OldValue)
				return OldValue;
		}
#endif
	}

	static FORCEINLINE SQWORD InterlockedOr(volatile SQWORD* Value, const SQWORD OrValue)
	{
#if _WIN64
		return (SQWORD)::_InterlockedOr64((volatile long long*)Value, (long long)OrValue);
#else
		// No explicit instruction for 64-bit atomic or on 32-bit processors; has to be implemented in terms of CMPXCHG8B
		for (;;)
		{
			const SQWORD OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue | OrValue, OldValue) == OldValue)
				return OldValue;
		}
#endif
	}

	static FORCEINLINE SQWORD InterlockedXor(volatile SQWORD* Value, const SQWORD XorValue)
	{
#if _WIN64
		return (SQWORD)::_InterlockedXor64((volatile long long*)Value, (long long)XorValue);
#else
		// No explicit instruction for 64-bit atomic xor on 32-bit processors; has to be implemented in terms of CMPXCHG8B
		for (;;)
		{
			const SQWORD OldValue = *Value;
			if (_InterlockedCompareExchange64(Value, OldValue ^ XorValue, OldValue) == OldValue)
				return OldValue;
		}
#endif
	}

	static FORCEINLINE SQWORD AtomicRead(volatile const SQWORD* Src)
	{
		return InterlockedCompareExchange((SQWORD*)Src, 0, 0);
	}

	static FORCEINLINE SQWORD AtomicRead_Relaxed(volatile const SQWORD* Src)
	{
#if _WIN64
		return *Src;
#else
		return InterlockedCompareExchange((volatile SQWORD*)Src, 0, 0);
#endif
	}

	static FORCEINLINE void  AtomicStore_Relaxed(volatile SQWORD* Src, SQWORD Val)
	{
#if _WIN64
		*Src = Val;
#else
		InterlockedExchange(Src, Val);
#endif
	}

};
typedef FWindowsPlatformAtomics FPlatformAtomics;

#else

struct FClangPlatformAtomics
{
	static FORCEINLINE BYTE   InterlockedIncrement(volatile BYTE* Value)                   { return __sync_fetch_and_add(Value, 1) + 1; }
	static FORCEINLINE SWORD  InterlockedIncrement(volatile SWORD* Value)                  { return __sync_fetch_and_add(Value, 1) + 1; }
	static FORCEINLINE INT    InterlockedIncrement(volatile INT* Value)                    { return __sync_fetch_and_add(Value, 1) + 1; }
	static FORCEINLINE SQWORD InterlockedIncrement(volatile SQWORD* Value)                 { return __sync_fetch_and_add(Value, 1) + 1; }
	static FORCEINLINE BYTE   InterlockedDecrement(volatile BYTE* Value)                   { return __sync_fetch_and_sub(Value, 1) - 1; }
	static FORCEINLINE SWORD  InterlockedDecrement(volatile SWORD* Value)                  { return __sync_fetch_and_sub(Value, 1) - 1; }
	static FORCEINLINE INT    InterlockedDecrement(volatile INT* Value)                    { return __sync_fetch_and_sub(Value, 1) - 1; }
	static FORCEINLINE SQWORD InterlockedDecrement(volatile SQWORD* Value)                 { return __sync_fetch_and_sub(Value, 1) - 1; }
	static FORCEINLINE BYTE   InterlockedAdd(volatile BYTE* Value, BYTE Amount)            { return __sync_fetch_and_add(Value, Amount); }
	static FORCEINLINE SWORD  InterlockedAdd(volatile SWORD* Value, SWORD Amount)          { return __sync_fetch_and_add(Value, Amount); }
	static FORCEINLINE INT    InterlockedAdd(volatile INT* Value, INT Amount)              { return __sync_fetch_and_add(Value, Amount); }
	static FORCEINLINE SQWORD InterlockedAdd(volatile SQWORD* Value, SQWORD Amount)        { return __sync_fetch_and_add(Value, Amount); }
	static FORCEINLINE BYTE   InterlockedExchange(volatile BYTE* Value, BYTE Exchange)     { return __sync_lock_test_and_set(Value, Exchange); }
	static FORCEINLINE SWORD  InterlockedExchange(volatile SWORD* Value, SWORD Exchange)   { return __sync_lock_test_and_set(Value, Exchange); }
	static FORCEINLINE INT    InterlockedExchange(volatile INT* Value, INT Exchange)       { return __sync_lock_test_and_set(Value, Exchange); }
	static FORCEINLINE SQWORD InterlockedExchange(volatile SQWORD* Value, SQWORD Exchange) { return __sync_lock_test_and_set(Value, Exchange); }
	static FORCEINLINE BYTE   InterlockedCompareExchange(volatile BYTE* Dest, BYTE Exchange, BYTE Comperand)      { return __sync_val_compare_and_swap(Dest, Comperand, Exchange); }
	static FORCEINLINE SWORD  InterlockedCompareExchange(volatile SWORD* Dest, SWORD Exchange, SWORD Comperand)   { return __sync_val_compare_and_swap(Dest, Comperand, Exchange); }
	static FORCEINLINE INT    InterlockedCompareExchange(volatile INT* Dest, INT Exchange, INT Comperand)         { return __sync_val_compare_and_swap(Dest, Comperand, Exchange); }
	static FORCEINLINE SQWORD InterlockedCompareExchange(volatile SQWORD* Dest, SQWORD Exchange, SQWORD Comperand){ return __sync_val_compare_and_swap(Dest, Comperand, Exchange); }
	static FORCEINLINE BYTE   InterlockedAnd(volatile BYTE* Value, const BYTE AndValue)    { return __sync_fetch_and_and(Value, AndValue); }
	static FORCEINLINE SWORD  InterlockedAnd(volatile SWORD* Value, const SWORD AndValue)  { return __sync_fetch_and_and(Value, AndValue); }
	static FORCEINLINE INT    InterlockedAnd(volatile INT* Value, const INT AndValue)      { return __sync_fetch_and_and(Value, AndValue); }
	static FORCEINLINE SQWORD InterlockedAnd(volatile SQWORD* Value, const SQWORD AndValue){ return __sync_fetch_and_and(Value, AndValue); }
	static FORCEINLINE BYTE   InterlockedOr(volatile BYTE* Value, const BYTE OrValue)      { return __sync_fetch_and_or(Value, OrValue); }
	static FORCEINLINE SWORD  InterlockedOr(volatile SWORD* Value, const SWORD OrValue)    { return __sync_fetch_and_or(Value, OrValue); }
	static FORCEINLINE INT    InterlockedOr(volatile INT* Value, const INT OrValue)        { return __sync_fetch_and_or(Value, OrValue); }
	static FORCEINLINE SQWORD InterlockedOr(volatile SQWORD* Value, const SQWORD OrValue)  { return __sync_fetch_and_or(Value, OrValue); }
	static FORCEINLINE BYTE   InterlockedXor(volatile BYTE* Value, const BYTE XorValue)    { return __sync_fetch_and_xor(Value, XorValue); }
	static FORCEINLINE SWORD  InterlockedXor(volatile SWORD* Value, const SWORD XorValue)  { return __sync_fetch_and_xor(Value, XorValue); }
	static FORCEINLINE INT    InterlockedXor(volatile INT* Value, const INT XorValue)      { return __sync_fetch_and_xor(Value, XorValue); }
	static FORCEINLINE SQWORD InterlockedXor(volatile SQWORD* Value, const SQWORD XorValue){ return __sync_fetch_and_xor(Value, XorValue); }
	static FORCEINLINE BYTE   AtomicRead(volatile const BYTE* Src)                         { BYTE   Result; __atomic_load((volatile BYTE*)  Src, &Result, __ATOMIC_SEQ_CST); return Result; }
	static FORCEINLINE SWORD  AtomicRead(volatile const SWORD* Src)                        { SWORD  Result; __atomic_load((volatile SWORD*) Src, &Result, __ATOMIC_SEQ_CST); return Result; }
	static FORCEINLINE INT    AtomicRead(volatile const INT* Src)                          { INT    Result; __atomic_load((volatile INT*)   Src, &Result, __ATOMIC_SEQ_CST); return Result; }
	static FORCEINLINE SQWORD AtomicRead(volatile const SQWORD* Src)                       { SQWORD Result; __atomic_load((volatile SQWORD*)Src, &Result, __ATOMIC_SEQ_CST); return Result; }
	static FORCEINLINE BYTE   AtomicRead_Relaxed(volatile const BYTE* Src)                 { BYTE   Result; __atomic_load((volatile BYTE*)  Src, &Result, __ATOMIC_RELAXED); return Result; }
	static FORCEINLINE SWORD  AtomicRead_Relaxed(volatile const SWORD* Src)                { SWORD  Result; __atomic_load((volatile SWORD*) Src, &Result, __ATOMIC_RELAXED); return Result; }
	static FORCEINLINE INT    AtomicRead_Relaxed(volatile const INT* Src)                  { INT    Result; __atomic_load((volatile INT*)   Src, &Result, __ATOMIC_RELAXED); return Result; }
	static FORCEINLINE SQWORD AtomicRead_Relaxed(volatile const SQWORD* Src)               { SQWORD Result; __atomic_load((volatile SQWORD*)Src, &Result, __ATOMIC_RELAXED); return Result; }
	static FORCEINLINE void   AtomicStore(volatile BYTE* Src, BYTE Val)                    { __atomic_store((volatile BYTE*)Src, &Val, __ATOMIC_SEQ_CST); }
	static FORCEINLINE void   AtomicStore(volatile SWORD* Src, SWORD Val)                  { __atomic_store((volatile SWORD*)Src, &Val, __ATOMIC_SEQ_CST); }
	static FORCEINLINE void   AtomicStore(volatile INT* Src, INT Val)                      { __atomic_store((volatile INT*)Src, &Val, __ATOMIC_SEQ_CST); }
	static FORCEINLINE void   AtomicStore(volatile SQWORD* Src, SQWORD Val)                { __atomic_store((volatile SQWORD*)Src, &Val, __ATOMIC_SEQ_CST); }
	static FORCEINLINE void   AtomicStore_Relaxed(volatile BYTE* Src, BYTE Val)            { __atomic_store((volatile BYTE*)Src, &Val, __ATOMIC_RELAXED); }
	static FORCEINLINE void   AtomicStore_Relaxed(volatile SWORD* Src, SWORD Val)          { __atomic_store((volatile SWORD*)Src, &Val, __ATOMIC_RELAXED); }
	static FORCEINLINE void   AtomicStore_Relaxed(volatile INT* Src, INT Val)              { __atomic_store((volatile INT*)Src, &Val, __ATOMIC_RELAXED); }
	static FORCEINLINE void   AtomicStore_Relaxed(volatile SQWORD* Src, SQWORD Val)        { __atomic_store((volatile SQWORD*)Src, &Val, __ATOMIC_RELAXED); }

	static FORCEINLINE void*  InterlockedExchangePtr(void*volatile* Dest, void* Exchange)
	{
		return __sync_lock_test_and_set(Dest, Exchange); 
	}

	static FORCEINLINE void* InterlockedCompareExchangePointer(void*volatile* Dest, void* Exchange, void* Comperand)
	{
		return __sync_val_compare_and_swap(Dest, Comperand, Exchange);
	}
};
typedef FClangPlatformAtomics FPlatformAtomics;
#endif

#endif
