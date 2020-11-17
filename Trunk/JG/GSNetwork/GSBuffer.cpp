#include "StdAfx.h"
#include "GSBuffer.h"

namespace GSNetwork	{	namespace GSMemPool	{	namespace GSBUffer	{

GSBuffer::GSBuffer(void)
{
	mLength			= 0;
	mPos			= 0;
	mBufferPointer = NULL;

}

GSBuffer::~GSBuffer(void)
{
	if (mBufferPointer != NULL)
		delete mBufferPointer;
}

BYTE* GSBuffer::AllocBuffer(INT Size)
{
	mBufferPointer = new BYTE[Size + 1];
	mLength = Size;
	return mBufferPointer;
}

BOOL GSBuffer::SetBuffer(BYTE *buffer,INT Size)
{

	mBufferPointer = new BYTE[Size+1];

	memcpy(mBufferPointer,buffer,Size);
	mLength			= Size;
	return TRUE;
}

BOOL GSBuffer::SetBuffer()
{
	if (!mBufferPointer)
		return FALSE;

	mLength			= 0;

	return TRUE;
}

BYTE *GSBuffer::GetBuffer()
{
	return mBufferPointer;
}

BOOL GSBuffer::ReadInt32(INT *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(INT));

	mPos += sizeof(INT);

	return TRUE;
}

BOOL GSBuffer::ReadDWORD(DWORD *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(DWORD));

	mPos += sizeof(DWORD);

	return TRUE;
}

BOOL GSBuffer::ReadDWORD_PTR(DWORD_PTR *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(DWORD_PTR));

	mPos += sizeof(DWORD_PTR);

	return TRUE;
}

BOOL GSBuffer::ReadByte(BYTE *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(BYTE));

	mPos += sizeof(BYTE);

	return TRUE;
}

BOOL GSBuffer::ReadBytes(BYTE *data, DWORD length)
{
	memcpy(data, mBufferPointer + mPos, length);

	mPos += length;

	return TRUE;
}

BOOL GSBuffer::ReadFloat(FLOAT *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(FLOAT));

	mPos += sizeof(FLOAT);

	return TRUE;
}

BOOL GSBuffer::ReadInt64(INT64 *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(INT64));

	mPos += sizeof(INT64);

	return TRUE;
}

BOOL GSBuffer::ReadSHORT(SHORT *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(SHORT));

	mPos += sizeof(SHORT);

	return TRUE;
}

BOOL GSBuffer::ReadUSHORT(USHORT *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(USHORT));

	mPos += sizeof(USHORT);

	return TRUE;
}

BOOL GSBuffer::ReadBOOL(BOOL *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(BOOL));

	mPos += sizeof(BOOL);

	return TRUE;
}

BOOL GSBuffer::ReadWCHAR(WCHAR *data)
{
	memcpy(data, mBufferPointer + mPos, sizeof(WCHAR));

	mPos += sizeof(WCHAR);

	return TRUE;
}

BOOL GSBuffer::ReadWCHARs(LPWSTR data, DWORD length)
{
	memcpy(data, mBufferPointer + mPos, length * sizeof(WCHAR));

	mPos += length * sizeof(WCHAR);

	return TRUE;
}

BOOL GSBuffer::WriteInt32(INT data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(INT));

	mLength += sizeof(INT);

	return TRUE;
}

BOOL GSBuffer::WriteDWORD(DWORD data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(DWORD));

	mLength += sizeof(DWORD);

	return TRUE;
}

BOOL GSBuffer::WriteDWORD_PTR(DWORD_PTR data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(DWORD_PTR));

	mLength += sizeof(DWORD_PTR);

	return TRUE;
}

BOOL GSBuffer::WriteByte(BYTE data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(BYTE));

	mLength += sizeof(BYTE);

	return TRUE;
}

BOOL GSBuffer::WriteBytes(BYTE *data, DWORD length)
{
	memcpy(mBufferPointer + mLength, data, length);

	mLength += length;

	return TRUE;
}

BOOL GSBuffer::WriteFloat(FLOAT data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(FLOAT));

	mLength += sizeof(FLOAT);

	return TRUE;
}

BOOL GSBuffer::WriteInt64(INT64 data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(INT64));

	mLength += sizeof(INT64);

	return TRUE;
}

BOOL GSBuffer::WriteSHORT(SHORT data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(SHORT));

	mLength += sizeof(SHORT);

	return TRUE;
}

BOOL GSBuffer::WriteUSHORT(USHORT data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(USHORT));

	mLength += sizeof(USHORT);

	return TRUE;
}

BOOL GSBuffer::WriteBOOL(BOOL data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(BOOL));

	mLength += sizeof(BOOL);

	return TRUE;
}

BOOL GSBuffer::WriteWCHAR(WCHAR data)
{
	memcpy(mBufferPointer + mLength, &data, sizeof(WCHAR));

	mLength += sizeof(WCHAR);

	return TRUE;
}

BOOL GSBuffer::WriteWCHARs(LPCWSTR data, DWORD length)
{
	memcpy(mBufferPointer + mLength, data, length * sizeof(WCHAR));

	mLength += length * sizeof(WCHAR);

	return TRUE;
}

DWORD GSBuffer::GetLength(VOID)
{
	return mLength;
}

}	}	}