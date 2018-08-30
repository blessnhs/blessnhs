/**
 *  file    Keyboard.h
 *  date    2017/09/20
 *  author  blessnhs
 *
 *  brief   키보드 디바이스 드라이버 함수
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "Types.h"
#include "Synchronization.h"

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// Pause 키를 처리하기 위해 무시해야 하는 나머지 스캔 코드의 수
#define KEY_SKIPCOUNTFORPAUSE       2

// 키 상태에 대한 플래그
#define KEY_FLAGS_UP             0x00
#define KEY_FLAGS_DOWN           0x01
#define KEY_FLAGS_EXTENDEDKEY    0x02

// 스캔 코드 매핑 테이블에 대한 매크로
#define KEY_MAPPINGTABLEMAXCOUNT    89

#define KEY_NONE        0x00
#define KEY_ENTER       '\n'
#define KEY_TAB         '\t'
#define KEY_ESC         0x1B
#define KEY_BACKSPACE   0x08

#define KEY_CTRL        0x81
#define KEY_LSHIFT      0x82
#define KEY_RSHIFT      0x83
#define KEY_PRINTSCREEN 0x84
#define KEY_LALT        0x85
#define KEY_CAPSLOCK    0x86
#define KEY_F1          0x87
#define KEY_F2          0x88
#define KEY_F3          0x89
#define KEY_F4          0x8A
#define KEY_F5          0x8B
#define KEY_F6          0x8C
#define KEY_F7          0x8D
#define KEY_F8          0x8E
#define KEY_F9          0x8F
#define KEY_F10         0x90
#define KEY_NUMLOCK     0x91
#define KEY_SCROLLLOCK  0x92
#define KEY_HOME        0x93
#define KEY_UP          0x94
#define KEY_PAGEUP      0x95
#define KEY_LEFT        0x96
#define KEY_CENTER      0x97
#define KEY_RIGHT       0x98
#define KEY_END         0x99
#define KEY_DOWN        0x9A
#define KEY_PAGEDOWN    0x9B
#define KEY_INS         0x9C
#define KEY_DEL         0x9D
#define KEY_F11         0x9E
#define KEY_F12         0x9F
#define KEY_PAUSE       0xA0

// 키 큐에 대한 매크로
// 키 큐의 최대 크기
#define KEY_MAXQUEUECOUNT	100

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 스캔 코드 테이블을 구성하는 항목
typedef struct KeyMappingTable
{
    // Shift 키나 Caps Lock 키와 조합되지 않는 ASCII 코드
    BYTE bNormalCode;
    
    // Shift 키나 Caps Lock 키와 조합된 ASCII 코드
    BYTE bCombinedCode;
} KEY_MAPPING_TABLE;

// 키보드의 상태를 관리하는 자료구조
typedef struct Keyboard
{
    // 자료구조 동기화를 위한 스핀락
    SPINLOCK stSpinLock;
    
    // 조합 키 정보
    BOOL bShiftDown;
    BOOL bCapsLockOn;
    BOOL bNumLockOn;
    BOOL bScrollLockOn;
    
    // 확장 키를 처리하기 위한 정보
    BOOL bExtendedCodeIn;
    int iSkipCountForPause;
} KEYBOARD_DEVICE;

// 키 큐에 삽입할 데이터 구조체
typedef struct KeyData
{
    // 키보드에서 전달된 스캔 코드
	BYTE bScanCode;
    // 스캔 코드를 변환한 ASCII 코드
	BYTE bASCIICode;
    // 키 상태를 저장하는 플래그(눌림/떨어짐/확장 키 여부)
	BYTE bFlags;
} KEY_DATA;

#pragma pack( pop )

// ------------------------------------------------------------------------------------------------
//usb keyboard code

typedef enum
{
    _USB_KEY_INVALID,
    USB_KEY_A, USB_KEY_B, USB_KEY_C, USB_KEY_D, USB_KEY_E, USB_KEY_F, USB_KEY_G, USB_KEY_H, USB_KEY_I, USB_KEY_J, USB_KEY_K, USB_KEY_L, USB_KEY_M, USB_KEY_N, USB_KEY_O, USB_KEY_P, USB_KEY_Q, USB_KEY_R, USB_KEY_S, USB_KEY_T, USB_KEY_U, USB_KEY_V, USB_KEY_W, USB_KEY_X, USB_KEY_Y, USB_KEY_Z,
    USB_KEY_0, USB_KEY_1, USB_KEY_2, USB_KEY_3, USB_KEY_4, USB_KEY_5, USB_KEY_6, USB_KEY_7, USB_KEY_8, USB_KEY_9,
    USB_KEY_ACC, USB_KEY_MINUS, USB_KEY_EQUAL, USB_KEY_BACKSL, USB_KEY_BACK, USB_KEY_SPACE, USB_KEY_CAPS, USB_KEY_TAB, USB_KEY_OSQBRA, USB_KEY_CSQBRA,
    USB_KEY_LSHIFT, USB_KEY_LCTRL, USB_KEY_LGUI, USB_KEY_LALT, USB_KEY_RSHIFT, USB_KEY_RCTRL, USB_KEY_RGUI, USB_KEY_ALTGR, USB_KEY_MENU, USB_KEY_ENTER, USB_KEY_ESC,
    USB_KEY_F1, USB_KEY_F2, USB_KEY_F3, USB_KEY_F4, USB_KEY_F5, USB_KEY_F6, USB_KEY_F7, USB_KEY_F8, USB_KEY_F9, USB_KEY_F10, USB_KEY_F11, USB_KEY_F12,
    USB_KEY_PRINT, USB_KEY_SCROLL, USB_KEY_PAUSE, USB_KEY_NUM, USB_KEY_INS, USB_KEY_DEL,
    USB_KEY_HOME, USB_KEY_PGUP, USB_KEY_END, USB_KEY_PGDWN, USB_KEY_ARRU, USB_KEY_ARRL, USB_KEY_ARRD, USB_KEY_ARRR,
    USB_KEY_KPSLASH, USB_KEY_KPMULT, USB_KEY_KPMIN, USB_KEY_KPPLUS, USB_KEY_KPEN, USB_KEY_KPDOT,
    USB_KEY_KP0, USB_KEY_KP1, USB_KEY_KP2, USB_KEY_KP3, USB_KEY_KP4, USB_KEY_KP5, USB_KEY_KP6, USB_KEY_KP7, USB_KEY_KP8, USB_KEY_KP9,
    USB_KEY_SEMI, USB_KEY_APPOS, USB_KEY_COMMA, USB_KEY_DOT, USB_KEY_SLASH,
    USB_KEY_GER_ABRA, // German USB_KEYboard has one USB_KEY more than the international one. TODO: Find a better name.
    _USB_KEY_LAST
} KEY_t;

static bool pressedKeys[_USB_KEY_LAST] = {false}; // for monitoring pressed keys
static uint8_t LED = 0x00;
bool keyPressed(KEY_t key);
void keyboard_keyPressedEvent(KEY_t key, bool make);
char keyToASCII(KEY_t key);
//usb keyboard end
//--------------------------------------------------------------------------------------------------

extern KEY_MAPPING_TABLE gs_vstKeyMappingTable[ KEY_MAPPINGTABLEMAXCOUNT ];

////////////////////////////////////////////////////////////////////////////////
//
//  함수
//
////////////////////////////////////////////////////////////////////////////////
BOOL IsOutputBufferFull( void );
BOOL IsInputBufferFull( void );
BOOL ActivateKeyboard( void );
BYTE GetKeyboardScanCode( void );
BOOL ChangeKeyboardLED( BOOL bCapsLockOn, BOOL bNumLockOn, BOOL bScrollLockOn );
void EnableA20Gate( void );
void Reboot( void );
BOOL IsAlphabetScanCode( BYTE bScanCode );
BOOL IsNumberOrSymbolScanCode( BYTE bScanCode );
BOOL IsNumberPadScanCode( BYTE bScanCode );
BOOL IsUseCombinedCode( BOOL bScanCode );
void UpdateCombinationKeyStatusAndLED( BYTE bScanCode );
BOOL ConvertScanCodeToASCIICode( BYTE bScanCode, BYTE* pbASCIICode, BOOL* pbFlags );
BOOL InitializeKeyboard( void );
BOOL ConvertScanCodeAndPutQueue( BYTE bScanCode );
BOOL GetKeyFromKeyQueue( KEY_DATA* pstData );
BOOL WaitForACKAndPutOtherScanCode( void );

#endif /*__KEYBOARD_H__*/

