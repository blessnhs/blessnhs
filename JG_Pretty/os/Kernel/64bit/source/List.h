
#ifndef __LIST_H__
#define __LIST_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 데이터를 연결하는 자료구조
// 반드시 데이터의 가장 앞부분에 위치해야 함
typedef struct kListLinkStruct
{
    // 다음 데이터의 어드레스와 데이터를 구분하기 위한 ID
    void* pvNext;
    QWORD qwID;
} LISTLINK;

// 리스트를 관리하는 자료구조
typedef struct kListManagerStruct
{
    // 리스트 데이터의 수
    int iItemCount;

    // 리스트의 첫 번째와 마지막 데이터의 어드레스
    void* pvHeader;
    void* pvTail;
} LIST;

#pragma pack( pop )


////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////
void InitializeList( LIST* pstList );
int GetListCount( const LIST* pstList );
void AddListToTail( LIST* pstList, void* pvItem );
void AddListToHeader( LIST* pstList, void* pvItem );
void* RemoveList( LIST* pstList, QWORD qwID );
void* RemoveListFromHeader( LIST* pstList );
void* RemoveListFromTail( LIST* pstList );
void* FindList( const LIST* pstList, QWORD qwID );
void* GetHeaderFromList( const LIST* pstList );
void* GetTailFromList( const LIST* pstList );
void* GetNextFromList( const LIST* pstList, void* pstCurrent );

#endif /*__LIST_H__*/
