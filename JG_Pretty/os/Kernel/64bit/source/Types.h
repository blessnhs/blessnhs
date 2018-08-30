/**
 *  file    Types.h
 *  date    2008/12/14
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   커널에서 사용하는 각종 타입을 정의한 파일
 */

#ifndef __TYPES_H__
#define __TYPES_H__

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
// 기본 타입 관련 매크로
#define BYTE    unsigned char
#define WORD    unsigned short
#define DWORD   unsigned int
#define QWORD   unsigned long
#define BOOL    unsigned char
#define TRUE    1
#define FALSE   0
#define NULL    0

#define true    1
#define false   0

typedef unsigned short      size_t;
typedef unsigned long long  uint64_t;
typedef unsigned int        uint32_t;
typedef unsigned short      uint16_t;
typedef unsigned char       uint8_t;
typedef unsigned char       u8;
typedef signed long long    int64_t;
typedef signed int          int32_t;
typedef signed short        int16_t;
typedef signed char         int8_t;
typedef uint32_t            uintptr_t;
typedef unsigned long       off_t;
typedef unsigned long       time_t;
typedef unsigned int        uid_t;
typedef unsigned int        gid_t;
typedef unsigned int        ino_t;
typedef int			        dev_t;
typedef unsigned int        mode_t;
typedef unsigned int        blkcnt_t;
typedef unsigned int        blksize_t;
typedef unsigned int        nlink_t;
typedef unsigned int        ssize_t;
typedef char                wchar_t;

#ifndef __bool_true_false_are_defined
  typedef _Bool             bool;
  #define true  1
  #define false 0
  #define __bool_true_false_are_defined 1
#endif

// stddef.h 헤더에 포함된 offsetof() 매크로의 내용
//#define offsetof(TYPE, MEMBER) __builtin_offsetof (TYPE, MEMBER)
#define BIT(n) (1U<<(n))

#define ATTR_ALIGNED(Bytes)        __attribute__ ((aligned(Bytes)))
#define ATTR_PACKED                __attribute__ ((packed))


  #define isdigit(c) ((c) >= '0' && (c) <= '9')
  #define isalpha(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
  #define isalnum(c) (isdigit(c) || isalpha(c))
  #define isupper(c) ((c) >= 'A' && (c) <= 'Z')
  #define islower(c) ((c) >= 'a' && (c) <= 'z')

  #define BIT(n) (1U<<(n))
  #define IS_BIT_SET(value, pos) ((value)&BIT(pos))

  #define BYTE1(a) ( (a)      & 0xFF)
  #define BYTE2(a) (((a)>> 8) & 0xFF)
  #define BYTE3(a) (((a)>>16) & 0xFF)
  #define BYTE4(a) (((a)>>24) & 0xFF)

  #define WORD1(a) ( (a)      & 0xFFFF)
  #define WORD2(a) (((a)>>16) & 0xFFFF)

  #define max(a, b) ((a) >= (b) ? (a) : (b))
  #define min(a, b) ((a) <= (b) ? (a) : (b))
  #define clamp(minimum, x, maximum) (max(min(x, maximum), minimum))

  #define NULL 0

  #define offsetof(st, m) ((size_t) ( (char*)&((st *)(0))->m - (char*)0 ))
  #define getField(addr, byte, shift, len) ((((uint8_t*)(addr))[byte]>>(shift)) & (BIT(len)-1))

#pragma pack( push, 1 )
// 비디오 모드 중 텍스트 모드 화면을 구성하는 자료구조
typedef struct kCharactorStruct
{
    BYTE bCharactor;
    BYTE bAttribute;
} CHARACTER;

#pragma pack( pop )

typedef enum
{
    CE_GOOD = 0,                    // No error
    CE_ERASE_FAIL,                  // An erase failed
    CE_NOT_PRESENT,                 // No device was present
    CE_NOT_FORMATTED,               // The disk is of an unsupported format
    CE_BAD_PARTITION,               // The boot record is bad

    CE_UNSUPPORTED_FS,              // The file system type is unsupported
    CE_BAD_FORMAT_PARAM,            // Trying to format a disk with bad parameters
    CE_NOT_INIT,                    // An operation was performed on an uninitialized device
    CE_BAD_SECTOR_READ,             // A bad read of a sector occured
    CE_WRITE_ERROR,                 // Could not write to a sector

    CE_INVALID_CLUSTER,             // Invalid cluster value > maxcls
    CE_FILE_NOT_FOUND,              // Could not find the file on the device
    CE_DIR_NOT_FOUND,               // Could not find the directory
    CE_BAD_FILE,                    // File is corrupted
    CE_TIMEOUT,                     // Timout while trying to access

    CE_COULD_NOT_GET_CLUSTER,       // Could not load/allocate next cluster in file
    CE_FILENAME_2_LONG,             // A specified file name is too long to use
    CE_FILENAME_EXISTS,             // A specified filename already exists on the device
    CE_INVALID_FILENAME,            // Invalid file name
    CE_DELETE_DIR,                  // The user tried to delete a directory with FSremove

    CE_DIR_FULL,                    // All root dir entry are taken
    CE_DISK_FULL,                   // All clusters in partition are taken
    CE_DIR_NOT_EMPTY,               // This directory is not empty yet, remove files before deleting
    CE_NONSUPPORTED_SIZE,           // The disk is too big to format as FAT16
    CE_WRITE_PROTECTED,             // Card is write protected

    CE_FILENOTOPENED,               // File not opened for the write
    CE_SEEK_ERROR,                  // File location could not be changed successfully
    CE_BADCACHEREAD,                // Bad cache read
    CE_UNSUPPORTED_FUNCTION,        // Driver does not support this operation
    CE_READONLY,                    // The file is read-only

    CE_WRITEONLY,                   // The file is write-only
    CE_INVALID_ARGUMENT,            // Invalid argument
    CE_FOPEN_ON_DIR,                // Attempted to call fopen() on a directory
    CE_UNSUPPORTED_SECTOR_SIZE,     // Unsupported sector size

    CE_FAT_EOF = 60,                // Read try beyond FAT's EOF
    CE_EOF                          // EOF
} FS_ERROR;

#endif /*__TYPES_H__*/


