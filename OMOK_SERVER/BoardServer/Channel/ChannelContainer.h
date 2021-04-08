#pragma once

#include "IGSContainer.h"
#include "./Channel.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>

#include "TemplateStrategyPattern.h"
#include "GSFactory.h"

#include "CLI.GS.pb.h"
#include "GS.CLI.pb.h"
#include "Structure.pb.h"

template<template<class T> class CreationPolicy>
class ChannelContainer  : public IContainer<CHANNEL_PTR>, public CreationPolicy<CHANNEL>
{
public:
	ChannelContainer(void) { InitializeCriticalSection(&m_PublicLock); }
	~ChannelContainer(void) {	DeleteCriticalSection(&m_PublicLock);	}

	CHANNEL_PTR Create(INT64 Id);

	bool Add(CHANNEL_PTR&pObj);
	bool Del(CHANNEL_PTR&pObj);
	CHANNEL_PTR Search(INT64 Id);


	bool PlayerEnterChannel(PLAYER_PTR player);
	bool PlayerLeaveChannel(PLAYER_PTR player);

	bool RoomEnterChannel(INT64 Id, ROOM_PTR room);
	bool RoomLeaveChannel(PLAYER_PTR player);

	void Report();

protected:

	concurrency::concurrent_unordered_map<DWORD, CHANNEL_PTR>		m_ChannelMap;

	CRITICAL_SECTION												m_PublicLock;
};

extern ChannelContainer<GSCreator> &GetChannelContainer();

#include "ChannelContainer.hpp"

#define CHANNELMGR GetChannelContainer()
