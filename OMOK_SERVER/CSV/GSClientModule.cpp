_commandEntries
#include "StdAfx.h"
#include "GSClientModule.h"
#include "GSClientSocket.h"
#include "../GSPacket/Front.h"
#include "../GSPacket/auth.h"
#include "../GSPacket/Relay.h"

#include "CSV.h"
#include "CSVDlg.h"
#include "LOBBY.h"
#include "ROOM.h"
#include "GSSerialize.h"
#include "UDPSession.h"

#include "lib_json/json/reader.h"
#include "lib_json/json/writer.h"

GSClientSocket m_SocketLogin;
GSClientSocket m_SocketLobby;
GSClientSocket m_SocketRelay;

DWORD MyIndex = 0;

extern std::vector<UDP_Client*> udp_client;

// 구현입니다.
LOBBY *m_lobby;
ROOM  *m_Room = NULL;

/////////////////////////////READ//////////////////////////////////////////////////////
#define DECLARE_JSON_READ(Data)	\
	Json::Value		root;	\
	Json::Reader	reader;	\
	bool bParse = reader.parse((char *)Data, root);	\
	if(!bParse) {	\
	printf("Json String Parse Error...\r\n");	\
	return;	\
	}	\

#define GET_JSON_WSTR_MEMBER(MEMBER,DATA) \
	std::string _##DATA = root.get(MEMBER,MEMBER).asString();	\
	std::wstring DATA;	\
	DATA.assign(_##DATA.begin(),_##DATA.end());	\

#define GET_JSON_INT_MEMBER(MEMBER,DATA) \
	int DATA = root.get(MEMBER,MEMBER).asInt();	\


#define GET_JSON_UINT_MEMBER(MEMBER,DATA) \
	unsigned int DATA = root.get(MEMBER,MEMBER).asUInt();	\

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////WRITE///////////////////////////////////////////////////
#define DECLARE_JSON_WRITER	\
	Json::Value root2;	\

#define ADD_JSON_MEMBER(MEMBER,DATA) \
	root2[MEMBER] = DATA;	\

#define ADD_JSON_WSTR_MEMBER(MEMBER,DATA) \
{\
	std::string VAR;	\
	VAR.assign(DATA.begin(),DATA.end());\
	root2[MEMBER] = VAR;	\
}\

#define END_JSON_MEMBER(OBJ,MAINID)	\
	Json::FastWriter writer;	\
	std::string outputConfig = writer.write( root2 );	\
	OBJ->WritePacket(MAINID,0,(BYTE*)outputConfig.c_str(),outputConfig.size()); \
	////////////////////////////////////////////////////////////////////////////////////////



	GSClientModule::GSClientModule(VOID)
	{
		IsUdp = FALSE;
	}

	GSClientModule::~GSClientModule(VOID)
	{
	}

	BOOL GSClientModule::BeginTcp(LPSTR remoteAddress, USHORT remotePort)
	{
		if (!remoteAddress || remotePort <= 0)
			return FALSE;

		if (!mTCPSession.Initialize())
		{
			End();

			return FALSE;
		}

		if (!mTCPSession.CreateTcpSocket())
		{
			End();

			return FALSE;
		}

		if (!GSEventSelect::Initialize(mTCPSession.GetSocket()))
		{
			End();

			return FALSE;
		}

		if (!mTCPSession.Connect(remoteAddress, remotePort))
		{
			End();

			return FALSE;
		}

		return TRUE;
	}

	BOOL GSClientModule::End(VOID)
	{
		mTCPSession.Termination();

		GSEventSelect::Termination();

		return TRUE;
	}

	BOOL GSClientModule::GetLocalIP(WCHAR* pIP)
	{
		return mTCPSession.GetLocalIP(pIP);
	}

	USHORT GSClientModule::GetLocalPort(VOID)
	{
		return mTCPSession.GetLocalPort();
	}

	BOOL GSClientModule::ReadPacket(WORD &protocol, BYTE *packet, WORD &packetLength)
	{
		if(m_ReadPacketQueue.size() == 0) return FALSE;

		std::shared_ptr<READ_PACKET_INFO> pData = m_ReadPacketQueue.front();

		CopyMemory(packet,pData->Data,sizeof(pData->Data));
		protocol = (WORD)pData->Protocol;
		packetLength = (WORD)pData->DataLength;

		if(pData != NULL)
		{
			m_ReadPacketQueue.pop();

			return TRUE;
		}

		return FALSE;
	}


	BOOL GSClientModule::WritePacket(WORD protocol, WORD Subprotocol,const BYTE *packet, WORD packetLength)
	{
		if (!mTCPSession.WritePacket(protocol,Subprotocol, packet, packetLength))
			return FALSE;

		if (!mTCPSession.WriteComplete())
			return FALSE;

		return TRUE;
	}

	VOID GSClientModule::OnIoRead(VOID)
	{
		BYTE	Packet[MAX_BUFFER_LENGTH]	= {0,};
		DWORD	PacketLength				= 0;

		WORD	MainProtocol				= 0;
		WORD	SubProtocol					= 0;

		if (IsUdp)
		{
			CHAR	RemoteAddress[32]	= {0,};
			USHORT	RemotePort			= 0;

			
		}
		else
		{
			if (mTCPSession.ReadPacketForEventSelect())
			{
				while (mTCPSession.GetPacket(MainProtocol,SubProtocol, Packet, PacketLength))
				{
					std::shared_ptr<READ_PACKET_INFO> pReadData(new READ_PACKET_INFO());

					CopyMemory(pReadData->Data,Packet,sizeof(pReadData->Data));
					pReadData->DataLength = PacketLength;
					pReadData->Object = this;
					pReadData->Protocol = MainProtocol;

					m_ReadPacketQueue.push(pReadData); 
				}
			}
		}
	}

	VOID GSClientModule::Proc()
	{
		WORD MainProtocol;
		BYTE Packet[1024*10];
		WORD Length;


		while(ReadPacket(MainProtocol,Packet,Length) == TRUE)
		{
			switch(MainProtocol)
			{
			case ID_FC_PKT_REGIST_RES:
				{
					DECLARE_JSON_WRITER
					ADD_JSON_MEMBER("ChannelId",0)
					END_JSON_MEMBER(this,ID_CF_PKT_ENTER_CHANNEL_REQ)

					{
						DECLARE_JSON_READ(Packet)
						GET_JSON_INT_MEMBER("Index",Index)

						MyIndex = Index;
					}
				}
				break;

			case ID_AUTH_LOGIN_RES:
				{
	
					Json::Value		root;	
					Json::Reader	reader;	
					bool bParse = reader.parse((char *)Packet, root);	
					if(!bParse) {	
						printf("Json String Parse Error...\r\n");	
						return;	
					}	

					bool _v = root.isMember("Result2");
					bool Id2 = root.isMember("Id2");

					std::string Id3 = root.get("Id2","fh").asString();

					int Result = root.get("Result",0).asInt();
					std::string Id = root.get("Id","fh").asString();
					std::string SessionKey = root.get("SessionKey","SessionKey").asString();

					if(Result == 0)
					{

						Termination();

						m_SocketLobby.Position = 1;
						m_SocketLobby.id.assign(Id.begin(),Id.end());
						m_SocketLobby.sessionkey.assign(SessionKey.begin(),SessionKey.end());

						m_SocketLobby.BeginTcp("127.0.0.1",8590);

				
						m_lobby = new LOBBY(AfxGetApp()->m_pMainWnd);
						m_lobby->m_char.Name = m_SocketLobby.id;
						m_lobby->DoModal();
					}
					else 
						AfxMessageBox(_T("Login failed"));

				}
				break;

			case ID_FC_PKT_ROOM_LIST_RES:				//방 리스트 요청
				{
					Json::Value		root;	
					Json::Value		value;	
					Json::Value		array;	

					Json::Reader	reader;	
					bool bParse = reader.parse((char *)Packet, root);	
					if(!bParse) {	
						printf("Json String Parse Error...%s\r\n",Packet);	
						return;	
					}	

					CListBox *pList=(CListBox*)m_lobby->GetDlgItem(IDC_LIST_ROOM); 
					if (pList) 
					{
						pList->ResetContent();
						const Json::Value& rooms = root["room"];
						for (Json::ValueConstIterator it = rooms.begin(); it != rooms.end(); ++it)
						{
							const Json::Value& room = *it;

							int Index = room["Index"].asInt();
							string name = room["Name"].asString();
					
							wstring _name;
							WCHAR BUFF[256];

							wsprintf(BUFF,_T("%d_%S"),Index,name.c_str());

							_name = BUFF;

							if(_name.size() > 0)
							{
								pList->AddString(_name.c_str());
							}
					
						}
					}
			
				}
				break;
			case ID_FC_PKT_USER_LIST_RES:				//유저 리스트 요청
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_INT_MEMBER("Result",Result)
					GET_JSON_WSTR_MEMBER("Id",Id)
					GET_JSON_WSTR_MEMBER("SessionKey",SessionKey)

				}
				break;

			case ID_FC_PKT_CREATE_ROOM_RES:
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_INT_MEMBER("RoomNumber",RoomNumber)

					m_Room = new ROOM(m_lobby);
					m_Room->RoomNumber = RoomNumber;
					m_Room->DoModal();
				}
				break;
			case ID_FC_PKT_NEW_USER_IN_ROOM:
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_WSTR_MEMBER("PlayerName",PlayerName)

					if(m_Room == NULL)
					{
						AfxMessageBox(_T("Room is null"));
					}

					CListBox *pList=(CListBox*)m_Room->GetDlgItem(IDC_LIST_USER);
					if(pList)
					{
						pList->AddString(PlayerName.c_str());
					}

				}
				break;
			case ID_CF_PKT_LOBBY_CHAT_NTY:
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_WSTR_MEMBER("Msg",Msg)

					CListBox *pList=(CListBox*)m_lobby->GetDlgItem(IDC_LIST_CHAT);
					if(pList)
					{
						pList->InsertString(pList->GetCount(),Msg.c_str());
					}
				}
				break;
			case ID_FC_PKT_ENTER_ROOM_RES:
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_INT_MEMBER("Result",Result)
					int index = root["roominfo"]["Index"].asInt() ;


					if(FC_PKT_ENTER_ROOM_RES::SUCCESS == Result)
					{
						if(m_Room == NULL)
							m_Room = new ROOM(m_lobby);

						m_Room->RoomNumber = index;
						m_Room->DoModal();
					}
					else
						AfxMessageBox(_T("Room Enter Failed"));

				}
				break;
			case ID_FC_PKT_LEAVE_ROOM_RES:
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_INT_MEMBER("Result",Result)
					GET_JSON_WSTR_MEMBER("PlayerName",PlayerName_)

					wstring PlayerName	=  PlayerName_;

					if(FC_PKT_LEAVE_ROOM_RES::SUCCESS == Result)
					{
						CListBox *pList=(CListBox*)m_Room->GetDlgItem(IDC_LIST_USER);
						if(pList)
						{
							INT RESULT = pList->FindString(0,PlayerName.c_str());
							if(RESULT != -1)
							{
								pList->DeleteString(RESULT);
							}
						}

						if(m_lobby->m_char.Name == PlayerName)
							m_Room->SendMessage(WM_SYSCOMMAND,9000);

					}
					else
						AfxMessageBox(_T("Room leave Failed"));

				}
				break;
			case ID_FC_PKT_CHAT_ROOM_RES:
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_WSTR_MEMBER("Chat",Chat)

					CListBox *pList=(CListBox*)m_Room->GetDlgItem(IDC_LIST_CHAT); 
					if (pList) 
					{
						pList->InsertString(pList->GetCount(),Chat.c_str());
					}

					
				}
				break;

			case ID_RC_REQUEST_STUN:
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_WSTR_MEMBER("Ip",Ip)
					GET_JSON_INT_MEMBER("Port",Port)
					GET_JSON_INT_MEMBER("GroupID",GroupID)
					GET_JSON_INT_MEMBER("Index",Index)

					std::string _DATA;
					std::wstring DATA = Ip;
					_DATA.assign(DATA.begin(),DATA.end());	

					{
						for(int i=0;i<udp_client.size();i++)
						{
							if(udp_client[i]->MyId == Index)
							{
								udp_client[i]->ToAddr.sin_addr.S_un.S_addr = inet_addr(_DATA.c_str());
								udp_client[i]->ToAddr.sin_port = (Port);
								udp_client[i]->m_state = UDP_Client::P2P_TUNNELING;
							}
						}
					}
				}
				break;

			case ID_RC_ENTER_ROOM_RES:
				{
					DECLARE_JSON_READ(Packet)
					GET_JSON_INT_MEMBER("Id",Id)
					GET_JSON_INT_MEMBER("Port",Port)
					GET_JSON_INT_MEMBER("Result",Result)
					GET_JSON_INT_MEMBER("GroupID",GroupID)

					{
						UDP_Client *pClient = new UDP_Client();
						udp_client.push_back( pClient );

						if(MyIndex == Id)
							pClient->Connect(RELAY_IP,Port,Id,1);

						pClient->MyId = Id;

						printf("add memeber %d \n",Id);
					}
				}
				break;

		
			}
		}
	}
	 