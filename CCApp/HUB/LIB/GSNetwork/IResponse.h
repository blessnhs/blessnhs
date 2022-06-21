#pragma once

class IResponse
{
public:
	IResponse(void){}
	virtual ~IResponse(void){}

	virtual VOID OnResponse(LPVOID Data) = 0;
};

