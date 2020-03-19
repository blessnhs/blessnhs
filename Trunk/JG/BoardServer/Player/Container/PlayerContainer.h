#pragma once

#include "IGSContainer.h"
#include "./Player.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>
#include <concurrent_unordered_map.h>

#include "TemplateStrategyPattern.h"


typedef boost::shared_ptr<Player> PlayerPtr;

using namespace std;

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
	PlayerPtr Search(string Account);
	PlayerPtr SearchBySocketId(DWORD Id);

	void Print();

	VOID CheckUserList();

	template<class TYPE>
	VOID BroadCast(TYPE MSG);

	VOID BroadCast(DWORD MainId,DWORD SudbId,byte *Message,WORD Length);

	VOID OnResponse(LPVOID Data);

protected:

	concurrency::concurrent_unordered_map<DWORD, PlayerPtr> m_PlayerMap;

	CRITICAL_SECTION										  m_PublicLock;
};


extern PlayerContainer<GSCreator> &GetPlayerContainer();

#define PLAYERMGR GetPlayerContainer()

#include "PlayerContainer.hpp"


