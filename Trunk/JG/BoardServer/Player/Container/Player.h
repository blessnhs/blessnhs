#pragma once

#include "GSObject.h"
#include "Character/Character.h"
					 
	
typedef GSNetwork::GSObject::GSObject GSObject;


class Player : public GSObject 
{
public:

	Player(void);
	~Player(void);

	class Account
	{
	public:
		Account() { RemoteUdpPort = 0; }
		~Account() {}

		std::string& GetName() { return m_Name;	}
		void SetName(const std::string& _Name) { m_Name = _Name; }

		std::string& GetFlatformId() { return m_FlatformId; }
		void SetFlatformId(const std::string& _FlatformId) { m_FlatformId = _FlatformId; }

		std::string& GetEMail() { return m_EMail; }
		void SetEMail(const std::string& _EMail) { m_EMail = _EMail; }

		std::string& GetPicture_url() { return m_Picture_url; }
		void SetPicture_url(const std::string& _Picture_url) { m_Picture_url = _Picture_url; }

	private:

		std::string m_Name;
		std::string m_FlatformId;
		std::string m_EMail;
		std::string m_Picture_url;
	
	public:

		std::string RemoteUdpAddress;
		int			RemoteUdpPort;

	};

	VOID SetChannel(DWORD _id);
	DWORD GetChannel();


	DWORD GetPair();
	void SetPair(DWORD pPair);

	void Initialize();


	Account		m_Account;
	Character	m_Char[MAX_CHAR_CNT];

private:

	DWORD       m_PairSessionId;
	DWORD		m_ChannelId;
};

typedef boost::shared_ptr<Player> PlayerPtr;


#define PLAYER		Player 
#define PLAYER_PTR	PlayerPtr 