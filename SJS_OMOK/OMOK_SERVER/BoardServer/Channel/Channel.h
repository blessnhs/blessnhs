#pragma once

#include "GSObject.h"

#include <boost/pool/object_pool.hpp>
#include <boost/shared_ptr.hpp>
#include "GSClient.h"

#include "../Player/container/Player.h"
#include "../Server/GSBoard.h"
#include "MessageMapDef.h"

#include "CLI.GS.pb.h"
#include "GS.CLI.pb.h"
#include "Structure.pb.h"

#include "../Room/RoomContainer.h"
#include "../Player/Container/PlayerContainer.h"

using namespace std;
typedef GSNetwork::GSObject::GSObject GSObject;

class Channel : public GSObject 
{
public:
	Channel();
	~Channel(void);

public:
	RoomContainer<GSCreator> RoomContainer;
	PlayerContainer<GSCreator> PlayerContainer;
};

typedef boost::shared_ptr<Channel> ChannelPtr;



#define CHANNEL		Channel 
#define CHANNEL_PTR	ChannelPtr 
