#pragma once

class IResponse
{
public:
	IResponse(void){}
	~IResponse(void){}

	virtual VOID OnResponse(LPVOID Data) = 0;
};

