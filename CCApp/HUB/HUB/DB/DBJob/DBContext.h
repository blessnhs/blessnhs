#pragma once

class Player;

typedef GSNetwork::GSObject::GSObject GSObject;

class RequestPlayerAuth : public GSObject
{
public:
	RequestPlayerAuth(void) 
	{
	}

	~RequestPlayerAuth(void) {}

	std::string token;
	INT64 Index;
	int Result;
	int channel;

	DWORD ForntId;		//����Ʈ ���� ���̵�(�����)
	DWORD FrontSid;		//����Ʈ �����ȿ��� � ���� Ŭ���̾�Ʈ����
};
