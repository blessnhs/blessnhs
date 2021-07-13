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
class RoomContainer  : public CreationPolicy<ROOM>
{
public:
	RoomContainer(void) { InitializeCriticalSection(&m_PublicLock); }
	~RoomContainer(void) {	DeleteCriticalSection(&m_PublicLock);	}

	ROOM_PTR Create();

	bool Add(ROOM_PTR &pObj, PLAYER_PTR player);
	bool Del(ROOM_PTR &pObj, PLAYER_PTR player);

	bool Add2(ROOM_PTR& pObj);
	bool Del2(ROOM_PTR& pObj);


	ROOM_PTR Search(INT64 Id);

	ROOM_PTR SearchByGameType(WORD Type);

	VOID GetRoomList(google::protobuf::RepeatedPtrField<RoomInfo2> *List);

	int Count();

	const concurrency::concurrent_unordered_map<DWORD, ROOM_PTR>& GetRoomMap();

	void LeaveRoomPlayer(PLAYER_PTR player,int room_number);


protected:

	concurrency::concurrent_unordered_map<DWORD,ROOM_PTR>		m_RoomMap;
	concurrency::concurrent_unordered_map<DWORD, ROOM_PTR>		m_RoomMapForLoop;

	CRITICAL_SECTION											m_PublicLock;
};

//채널 도입으로 전역 룸 객체는 주석
extern RoomContainer<GSCreator> &GetRoomContainer();
#define ROOMMGR GetRoomContainer()

#include "RoomContainer.hpp"

