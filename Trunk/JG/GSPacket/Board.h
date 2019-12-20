#pragma once
 #pragma pack(push, 1) 

#include "stdafx.h"
#include "proto_header.h"

typedef enum _BOARD_PROTOCOL
{
	
	ID_PKT_LOGIN_REQ,					//Ŭ���̾�Ʈ ���� ��û
	ID_PKT_LOGIN_RES,					//Ŭ���̾�Ʈ ���� ���

	ID_PKT_CREATE_ROOM_REQ,				//�� ���� ��û
	ID_PKT_CREATE_ROOM_RES,				//�� ���� ��û ���

	ID_PKT_LEAVE_ROOM_REQ,				//�� ���� ��û
	ID_PKT_LEAVE_ROOM_RES,				//�� ���� ��û ���

	ID_PKT_ENTER_ROOM_REQ,				//�� ���� ��û
	ID_PKT_ENTER_ROOM_RES,				//�� ���� ��û ���

	ID_PKT_NEW_USER_IN_ROOM,				//�� ���� ���� �˸�

	ID_PKT_CHAT_ROOM_REQ,				//�� ä�� ��ûz
	ID_PKT_CHAT_ROOM_RES,				//�� ä�� ��û ���

	ID_PKT_READY_ROOM_REQ,				//�÷��̾� ���� ���� ��û
	ID_PKT_READY_ROOM_RES,				//�÷��̾� ���� ���

	ID_PKT_START_ROOM_REQ,				//�� ���� ���� ���� ��û
	ID_PKT_START_ROOM_RES,				//�� ���� ����  ���

	ID_PKT_RESULT_ROOM,					//�� ����  ���
	
	ID_PKT_ROOM_LIST_REQ,				//�� ����Ʈ ��û
	ID_PKT_ROOM_LIST_RES,				//�� ����Ʈ ��û

	ID_PKT_USER_LIST_REQ,				//���� ����Ʈ ��û
	ID_PKT_USER_LIST_RES,				//���� ����Ʈ ��û

	ID_PKT_ALL_COMPLETE_NTY,				//�غ� �Ϸ�
	ID_PKT_ALL_COMPLETE,					//��� �غ�Ϸ�

	ID_PKT_AUTO_START_REQ,

	ID_HELLO_WORLD,						//�κ� ���� ����
};



#pragma pack(pop)
	
	

	
	

	
	

	
	

			
