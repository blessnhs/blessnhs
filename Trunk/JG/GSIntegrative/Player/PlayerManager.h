#pragma once

#include "IManager.h"
#include "IGSContainer.h"
#include "IProcess.h"

#include "Player.h"
#include "IResponse.h"
#include <boost/pool/object_pool.hpp>
#include "GSfactory.h"
#include "SingleTonHolder.h"

typedef Player Player;
typedef boost::shared_ptr<Player> PlayerPtr;

using namespace std;

class PlayerManager : /*public IManager<PlayerPtr> ,*/ public IContainer<PlayerPtr>, 
	public IProcess<PlayerManager>, public IResponse,  public Singleton<PlayerManager>
{
public:
	PlayerManager(void);
	~PlayerManager(void);

	void Destroy(PlayerPtr &obj)  {};
	PlayerPtr &Create(WORD Type) { return PlayerPtr(0);  };
	void OnEvt(LPVOID Arg);
	PlayerPtr &Get(DWORD Id){return PlayerPtr(0); };

	bool Add(PlayerPtr &pObj);
	bool Del(PlayerPtr &pObj);
	PlayerPtr Search(DWORD Id);
	PlayerPtr Search(wstring Account);
	PlayerPtr SearchBySession(DWORD id);

	VOID Process(LPVOID Data,DWORD Length,WORD MainProtocol,WORD SubProtocol);

	virtual VOID OnResponse(LPVOID Data);

	VOID CheckCloseJob();

	void DestoryAll();

	VOID Print();

	GSFactory<Player> &GetPlayerPool();

protected:
public:
	std::map<DWORD,PlayerPtr>										m_PlayerMap;


	GSFactory<Player>												m_PlayerPool;
};

#define PLAYERMGR Singleton<PlayerManager>::Instance()

