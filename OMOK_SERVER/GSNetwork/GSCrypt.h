#pragma once

namespace GSNetwork	{	namespace GSCrypt	{	

class GSCrypt
{
public:
	static BOOL Encrypt(BYTE *source, BYTE *destination, DWORD length);
	static BOOL Decrypt(BYTE *source, BYTE *destination, DWORD length);
};

}	}