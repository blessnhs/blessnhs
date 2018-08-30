#pragma once

#include "IGSContainer.h"
#include "./Room.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>

#include "TemplateStrategyPattern.h"
#include "GSFactory.h"

namespace GSFrontServer	{	namespace	RoomContainer	{

template<template<class T> class CreationPolicy>
class RoomContainer  : public IContainer<ROOM_PTR>, public CreationPolicy<ROOM>
{
public:
	RoomContainer(void) {}
	~RoomContainer(void){}

	ROOM_PTR Create();

	bool Add(ROOM_PTR &pObj);
	bool Del(ROOM_PTR &pObj);
	ROOM_PTR Search(DWORD Id);

	ROOM_PTR SearchByGameType(WORD Type);

	VOID GetRoomList(std::vector<RoomInfo> &List);

protected:

	std::map<DWORD,ROOM_PTR>											m_RoomMap;
};

#include "RoomContainer.hpp"

}	}

