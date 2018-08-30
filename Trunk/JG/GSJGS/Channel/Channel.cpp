#include "StdAfx.h"
#include "./Channel.h"
#include "../GSPacket/Front.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"
#include "GSSerialize.h"

namespace GSFrontServer	{	namespace	Channel	{


VOID Channel::SendRoomList(GSCLIENT *pSession)
{
	FC_PKT_ROOM_LIST_RES Res;

	Json::Value root2;	
	Json::Value array;
	Json::Value Value;

	std::vector<RoomInfo> List;

	m_RoomContainer.GetRoomList(List);

	if(List.size() == 0)
	{

		root2["room"] = array;
		Json::FastWriter writer;	
		std::string outputConfig = writer.write( root2 );	
		pSession->GetTCPSocket()->WritePacket(ID_FC_PKT_ROOM_LIST_RES,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); 
	}

	int Pos = 0;

	std::vector<RoomInfo>::iterator iter = List.begin();
	while(iter != List.end())
	{
		std::wstring Name = iter->Name;
		WTOM(Name)

		Value.clear();
		Value["Name"]  = _Name;
		Value["Index"] =  (WORD)iter->Index;

		array.append( Value );


		if((Pos%MAX_TRNASMISSION_COUNT) == MAX_TRNASMISSION_COUNT)
		{
			root2["room"] = array;

			Json::FastWriter writer;	\
			std::string outputConfig = writer.write( root2 );	\
			pSession->GetTCPSocket()->WritePacket(ID_FC_PKT_ROOM_LIST_RES,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); \

			root2.clear();
			array.clear();
		}

		++Pos ;
		iter++;
	}

	if(Pos != 0 )
	{
		root2["room"] = array;
		Json::FastWriter writer;	
		std::string outputConfig = writer.write( root2 );	
		pSession->GetTCPSocket()->WritePacket(ID_FC_PKT_ROOM_LIST_RES,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); 
	}

}

VOID Channel::SendUserList(GSCLIENT *pSession)
{
/*	FC_PKT_USER_LIST_RES Res;

	std::vector<UserInfo> List;
	m_PlayerContainer.GetUserList(List);

	std::vector<UserInfo>::iterator iter = List.begin();
	while(iter != List.end())
	{
		Res.info.Name = iter->Name;

		Sender::Send(pSession,Res);

		iter++;
	}*/

	Json::Value root2;	
	Json::Value array;
	Json::Value Value;
	int Pos = 0;

	std::vector<UserInfo> List;
	m_PlayerContainer.GetUserList(List);

	std::vector<UserInfo>::iterator iter = List.begin();
	while(iter != List.end())
	{
		std::wstring Name = iter->Name;
		WTOM(Name)
		
		Value.clear();
		Value["Name"] = _Name;
		array.append(Value);

		if((Pos%MAX_TRNASMISSION_COUNT) == MAX_TRNASMISSION_COUNT)
		{
			root2["userlist"] = array;

			Json::FastWriter writer;	\
				std::string outputConfig = writer.write( root2 );	\
				pSession->GetTCPSocket()->WritePacket(ID_FC_PKT_ROOM_LIST_RES,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); \

			root2.clear();
			array.clear();
			//	Sender::Send(pSession,Res);
		}


		Pos++;
		iter++;
	}


	if(Pos != 0)
	{
		Json::FastWriter writer;	
		std::string outputConfig = writer.write( root2 );	
		pSession->GetTCPSocket()->WritePacket(ID_FC_PKT_ROOM_LIST_RES,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); 
	}
}

}	}