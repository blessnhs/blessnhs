#pragma once

#include "common.h"

enum IO_TYPE
{
	IO_ACCEPT,
	IO_READ,
	IO_WRITE
};

typedef struct _OVERLAPPED_EX
{
	OVERLAPPED	Overlapped;
	IO_TYPE		IoType;
	VOID		*Object;

} OVERLAPPED_EX;

class WRITE_PACKET_INFO
{
public:
	WRITE_PACKET_INFO(){}
	virtual ~WRITE_PACKET_INFO() {}

	BYTE	Data[MAX_BUFFER_LENGTH];
	DWORD	DataLength;

	VOID	*Object;

	CHAR	RemoteAddress[14];
	USHORT	RemotePort;
	DWORD	PacketNumber;

};

class READ_PACKET_INFO
{
public:
	READ_PACKET_INFO(){}
	virtual ~READ_PACKET_INFO() {}

	BYTE	Data[MAX_BUFFER_LENGTH];
	DWORD	DataLength;
	DWORD   Protocol;

	VOID	*Object;

	CHAR	RemoteAddress[14];
	USHORT	RemotePort;
	DWORD	PacketNumber;

};
