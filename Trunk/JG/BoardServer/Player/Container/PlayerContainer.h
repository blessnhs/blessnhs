#pragma once

#include "IGSContainer.h"
#include "./Player.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>
#include "TemplateStrategyPattern.h"

#include "GSFactory.h"

#include "../GSPacket/Front.h"
#include "../GSPacket/Protocol.h"
#include "../../DB/DBJob/DBContext.h"

#include "GSSerialize.h"
#include "../../LOG/Log.h"
#include "../../Ini/Ini.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

typedef boost::shared_ptr<Player> PlayerPtr;

template<template<class T> class CreationPolicy>
class PlayerContainer : public IContainer<PlayerPtr>, public CreationPolicy<Player>
{
public:
	PlayerContainer();
	~PlayerContainer();

	PlayerPtr Create();

	bool Add(PlayerPtr &pObj);
	bool Del(PlayerPtr &pObj);
	PlayerPtr Search(DWORD Id);
	PlayerPtr Search(wstring Account);
	void DeleteAllByServerId(WORD ServerId);

	void Print();

	VOID GetUserList(std::vector<UserInfo> &List);
	VOID CheckUserList();

	template<class TYPE>
	VOID BroadCast(TYPE MSG);

	VOID BroadCast(DWORD MainId,DWORD SudbId,byte *Message,WORD Length);

	VOID OnResponse(LPVOID Data);

protected:

	map<DWORD, PlayerPtr> m_PlayerMap;

	CRITICAL_SECTION											  m_PublicLock;
};


extern PlayerContainer<GSCreator> &GetPlayerContainer();

#define PLAYERMGR GetPlayerContainer()

#include "PlayerContainer.hpp"


