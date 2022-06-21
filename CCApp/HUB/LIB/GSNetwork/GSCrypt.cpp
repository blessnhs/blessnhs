#include "StdAfx.h"
#include "GSCrypt.h"

namespace GSNetwork	{	namespace GSCrypt	{	

const INT		C1				= 52845;
const INT		C2				= 22719;
const INT		KEY				= 72957;

BOOL GSCrypt::Encrypt(BYTE *source, BYTE *destination, DWORD length)
{
	DWORD	i;
	INT		Key = KEY;

	if (!source || !destination || length <= 0)
		return FALSE;

	for (i=0;i<length;i++)
	{
		destination[i] = source[i]^Key >> 8;
		Key = (destination[i] + Key) * C1 + C2;
	}

	return TRUE;
}

BOOL GSCrypt::Decrypt(BYTE *source, BYTE *destination, DWORD length)
{
	DWORD	i;
	BYTE	PreviousBlock;
	INT		Key	= KEY;

	if (!source || !destination || length <= 0)
		return FALSE;

	for (i=0;i<length;i++)
	{
		PreviousBlock = source[i];
		destination[i] = source[i]^Key >> 8;
		Key = (PreviousBlock + Key) * C1 + C2;
	}

	return TRUE;
}

}	}