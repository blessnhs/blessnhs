#pragma once

#include "GSObject.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>


#include "../PLAYER/Container/PlayerContainer.h"
#include "../Room/RoomContainer.h"

namespace GSFrontServer	{	namespace	Channel	{


class Channel : public GSNetwork::GSObject::GSObject 
{
public:
	Channel(void)  {}
	~Channel(void) {}

	PlayerContainer::PlayerContainer<GSCreator> m_PlayerContainer;
	RoomContainer::RoomContainer<GSCreator>		m_RoomContainer;

	VOID SendRoomList(GSCLIENT *pSession);
	VOID SendUserList(GSCLIENT *pSession);
	
};

typedef boost::shared_ptr<GSFrontServer::Channel::Channel> ChannelPtr;

}	}


#define CHANNEL		GSFrontServer::Channel::Channel 
#define CHANNEL_PTR	GSFrontServer::Channel::ChannelPtr 
