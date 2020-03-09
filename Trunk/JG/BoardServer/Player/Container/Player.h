#pragma once

#include "GSObject.h"
#include "Character/Character.h"
					 
	
typedef GSNetwork::GSObject::GSObject GSObject;

class Player : public GSObject {
public:

	Player(void);
	~Player(void);

	class Account
	{
	public:
		Account() {}
		~Account() {}

		std::wstring& GetName() { return m_Name;	}
		void SetName(const std::wstring& _Name) { m_Name = _Name; }

	public:

		std::wstring m_Name;
	};

	VOID SetChannel(DWORD _id);
	DWORD GetChannel();


	DWORD GetPair();
	void SetPair(DWORD pPair);

	void Initialize();

	Account		m_Account;
	Character	m_Char[MAX_CHAR_CNT];


	DWORD       m_PairSessionId;

	DWORD		m_ChannelId;

	DWORD		m_RoomNumber;
};

typedef boost::shared_ptr<Player> PlayerPtr;


#define PLAYER		Player 
#define PLAYER_PTR	PlayerPtr 