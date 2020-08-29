#pragma once

#include "IGSContainer.h"
#include "./Room.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>

#include "TemplateStrategyPattern.h"
#include "GSFactory.h"

#include "CLI.GS.pb.h"
#include "GS.CLI.pb.h"
#include "Structure.pb.h"

template<template<class T> class CreationPolicy>
class RoomContainer  : public IContainer<ROOM_PTR>, public CreationPolicy<ROOM>
{
public:
	RoomContainer(void) { InitializeCriticalSection(&m_PublicLock); }
	~RoomContainer(void) {	DeleteCriticalSection(&m_PublicLock);	}

	ROOM_PTR Create();

	bool Add(ROOM_PTR &pObj);
	bool Del(ROOM_PTR &pObj);
	ROOM_PTR Search(INT64 Id);

	ROOM_PTR SearchByGameType(WORD Type);

	VOID GetRoomList(google::protobuf::RepeatedPtrField<RoomInfo2> *List);

	int RoomCount();

	const concurrency::concurrent_unordered_map<DWORD, ROOM_PTR>& GetRoomMap();

	concurrency::concurrent_unordered_map<INT64, PLAYER_PTR>& GetMatchMap();

	void AddMatchMap(PLAYER_PTR player);
	void DelMatchMap(PLAYER_PTR player);
	bool IsExistMatchMap(PLAYER_PTR player);

	BOOL CreateMatchRoom(PLAYER_PTR target1, PLAYER_PTR target2);

	void LeaveRoomPlayer(PLAYER_PTR player);


protected:

	concurrency::concurrent_unordered_map<DWORD,ROOM_PTR>		m_RoomMap;
	concurrency::concurrent_unordered_map<DWORD, ROOM_PTR>		m_RoomMapForLoop;

	concurrency::concurrent_unordered_map<INT64, PLAYER_PTR>	m_MatchMap;

	CRITICAL_SECTION											m_PublicLock;
};

extern RoomContainer<GSCreator> &GetRoomContainer();

#include "RoomContainer.hpp"

#define ROOMMGR GetRoomContainer()
