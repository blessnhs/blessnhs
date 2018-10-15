/**
 *  file    Synchronization.h
 *  date    2009/03/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   동기화를 처리하는 함수에 관련된 파일
 */

#ifndef __SYNCHRONIZATION_H__
#define __SYNCHRONIZATION_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 뮤텍스 자료구조
typedef struct kMutexStruct
{
    // 태스크 ID와 잠금을 수행한 횟수
    volatile QWORD qwTaskID;
    volatile DWORD dwLockCount;

    // 잠금 플래그
    volatile BOOL bLockFlag;
    
    // 자료구조의 크기를 8바이트 단위로 맞추려고 추가한 필드
    BYTE vbPadding[ 3 ];
} MUTEX;

// 스핀락 자료구조
typedef struct kSpinLockStruct
{
    // 로컬 APIC ID와 잠금을 수행한 횟수
    volatile DWORD dwLockCount;
    volatile BYTE bAPICID;

    // 잠금 플래그
    volatile BOOL bLockFlag;
    
    // 인터럽트 플래그
    volatile BOOL bInterruptFlag;
    
    // 자료구조의 크기를 8바이트 단위로 맞추려고 추가한 필드
    BYTE vbPadding[ 1 ];
} SPINLOCK;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////
#if 0
BOOL LockForSystemData( void );
void UnlockForSystemData( BOOL bInterruptFlag );
#endif

void InitializeSpinLock( SPINLOCK* pstSpinLock );
void LockForSpinLock( SPINLOCK* pstSpinLock );
void UnlockForSpinLock( SPINLOCK* pstSpinLock );

void InitializeMutex( MUTEX* pstMutex );
void Lock( MUTEX* pstMutex );
void Unlock( MUTEX* pstMutex );

#define MUTEX_CREATE(name) MUTEX name = { .bLockFlag = 0,.dwLockCount = 0,.qwTaskID = 0xFFFFFFFFFFFFFFFF }

#endif /*__SYNCHRONIZATION_H__*/
