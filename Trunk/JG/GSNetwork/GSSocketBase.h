#pragma once

#include "common.h"
#include <boost/shared_ptr.hpp>
class GSClient;

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
	int			ObjectId;
} OVERLAPPED_EX;

class WRITE_PACKET_INFO
{
public:
	WRITE_PACKET_INFO() { Data = NULL;  DataLength = 0;  }
	virtual ~WRITE_PACKET_INFO() { }

	BYTE	*Data;
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
