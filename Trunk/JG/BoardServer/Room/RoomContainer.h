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
	ROOM_PTR Search(DWORD Id);

	ROOM_PTR SearchByGameType(WORD Type);

	VOID GetRoomList(google::protobuf::RepeatedPtrField<RoomInfo2> *List);

	int RoomCount()
	{
		int count = 0;
		for each (auto room in m_RoomMap)
		{
			if (room.second != NULL)
				count++;
		}

		return count;
	}

	const concurrency::concurrent_unordered_map<DWORD, ROOM_PTR>& GetRoomMap() 
	{
		return m_RoomMap;
	}


protected:

	concurrency::concurrent_unordered_map<DWORD,ROOM_PTR>		m_RoomMap;

	CRITICAL_SECTION											m_PublicLock;
};

extern RoomContainer<GSCreator> &GetRoomContainer();

#include "RoomContainer.hpp"

#define ROOMMGR GetRoomContainer()
