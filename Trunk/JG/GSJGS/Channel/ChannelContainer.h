#pragma once

#include "IGSContainer.h"
#include "./Channel.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>

#include "TemplateStrategyPattern.h"
#include "GSFactory.h"

namespace GSFrontServer	{	namespace	ChannelContainer	{

template<template<class T> class CreationPolicy>
class ChannelContainer  : public IContainer<CHANNEL_PTR>, public CreationPolicy<CHANNEL>
{
public:
	ChannelContainer(void) {}
	~ChannelContainer(void){}

	void Initialize();

	CHANNEL_PTR Create();

	bool Add(CHANNEL_PTR &pObj);
	bool Del(CHANNEL_PTR &pObj);
	CHANNEL_PTR Search(DWORD Id);

	VOID GetChannelList(std::vector<RoomInfo> &List);

protected:

	std::map<DWORD,CHANNEL_PTR>											m_ChannelMap;
};

#include "ChannelContainer.hpp"

extern ChannelContainer<GSCreator> &GetChannelContainer();

#define CHANNELMGR GSFrontServer::ChannelContainer::GetChannelContainer()

}	}

