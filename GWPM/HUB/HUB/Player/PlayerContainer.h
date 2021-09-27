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

	bool Add2(PlayerPtr& pObj);
	bool Del2(PlayerPtr& pObj);

	PlayerPtr Search(INT64 Id);
	PlayerPtr Search(string flatformid);
	PlayerPtr SearchBySocketId(DWORD Id);


	PlayerPtr Search(DWORD FrontId,DWORD FrontSid);
	PlayerPtr SearchByFrontSid(DWORD FrontSid);

	void Print();

	VOID CheckUserList();

	int Count();

	template<class TYPE>
	VOID BroadCast(TYPE MSG);

	VOID BroadCast(DWORD MainId,DWORD SudbId,byte *Message,WORD Length);

	VOID OnResponse(LPVOID Data);

	VOID Disconnect(GSCLIENT_PTR pSession);
	VOID Disconnect(PlayerPtr pPlayer);

protected:

	//원래 concurrent_unordered_map 자료 구조는 캐싱용도의 쓰레드 세이프한 자료 구조이기 때문에
	//루프같은 순회할때를 위해 루프용을 하나 만든다.
	//m_PlayerMap erase 대신 key value에 null을 넣었기 때문이다.
	concurrency::concurrent_unordered_map<INT64, PlayerPtr> m_PlayerMap;
	concurrency::concurrent_unordered_map<INT64, PlayerPtr> m_PlayerMapForLoop;

	CRITICAL_SECTION										  m_PublicLock;
};


extern PlayerContainer<GSCreator> &GetPlayerContainer();

#define PLAYERMGR GetPlayerContainer()

#include "PlayerContainer.hpp"


