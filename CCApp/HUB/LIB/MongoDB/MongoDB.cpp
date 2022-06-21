// MongoDB.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "MongoDB.h"

#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "Rpcrt4.lib")

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

mongocxx::instance instance{};// don't put inside main 
mongocxx::pool pool{ mongocxx::uri("mongodb://localhost:27017/?minPoolSize=4&maxPoolSize=8") };

using namespace std;

#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string



void MongoDB::CreateIndex()
{
	//디비에서하기로함
	/*{
		mongocxx::collection collection = db["ACCOUNT"];

		auto index_specification1 = document{} << "Name" << 1 << finalize;
		collection.create_index(std::move(index_specification1));
	}

	{
		mongocxx::collection collection = db["CONCURRENTUSER"];

		auto index_specification1 = document{} << "INDEX" << 1 << finalize;
		collection.create_index(std::move(index_specification1));
	}*/
}


	//버전 정보를 가져온다.
double MongoDB::ProcedureVersion(int type)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		mongocxx::collection collection = db["VERSION"];
		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			collection.find_one(document{} << "type" << type << finalize);
		if (maybe_result)
		{
			auto view = maybe_result->view()["value"];

			auto strview = view.get_utf8().value;
			string version = strview.to_string();

			double lol = atof(version.c_str());

			return lol;
		}
	}
	catch (...)
	{
		printf("exception ProcedureVersion \n");
	}

	return 0;
}


bool MongoDB::IsExistNickName(string name, int64_t Index)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		auto collection = db["ACCOUNT"];

		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			collection.find_one(document{} << "Name" << name << finalize);
		if (!maybe_result)
		{
			//닉네임이 없다.

			auto serverQuery = db["ACCOUNT"].update_one(
				make_document(kvp("INDEX", Index)),
				make_document(
					kvp("$set", make_document(kvp("Name", name)))));

			if (!serverQuery)
			{
				printf("IsExistNickName  Index %I64d query failed \n", Index);
				return false;
			}

			return true;
		}

		return false;
	}
	catch (...)
	{
		printf("exception IsExistNickName \n");
	}
}



std::list<tuple<__int64, string, string>> MongoDB::NoticeInfoInfo()
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		std::list<tuple<__int64, string, string>> list;
		auto collection = db["NOTICE"];

		auto order = bsoncxx::builder::stream::document{} << "reg-time" << -1 << bsoncxx::builder::stream::finalize;

		auto opts = mongocxx::options::find{};
		opts.sort(order.view());
		opts.limit(100);

		auto cursor = collection.find({}, opts);

		int irank = 1;

		for (auto doc : cursor)
		{
			auto index = doc["index"].get_int64();
			auto content = doc["content"].get_utf8().value;
			auto time = doc["reg-time"].get_utf8().value;

			list.push_back(tuple<__int64, string, string>(index, content, time));
		}

		return list;
	}
	catch (...)
	{

		printf("exception NoticeInfoInfo \n");
		return std::list<tuple<__int64, string, string>>();
	}
}

int MongoDB::UpdaetQNS(int64_t Index, std::string contents)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		mongocxx::collection collection = db["QNS"];
		///////////////////////////////////////////////////////////
		/////insert 
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc_value = builder
			<< "INDEX" << Index
			<< "contents" << contents
			<< "reg-time" << bsoncxx::types::b_date(std::chrono::system_clock::now())
			<< bsoncxx::builder::stream::finalize;

		bsoncxx::document::view view = doc_value.view();

		bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
			collection.insert_one(view);

		if (!result)
		{
			printf("UpdaetQNS  Index %I64d query failed \n", Index);

			return -1;
		}

		return 0;
	}
	catch (...)
	{
		printf("exception UpdaetQNS \n");
	}
}

std::list<tuple<string, string, string>> MongoDB::PrayList(int count)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		std::list<tuple<string, string, string>> prayList;
		auto collection = db["PRAY"];

		auto order = bsoncxx::builder::stream::document{} << "reg-time" << -1 << bsoncxx::builder::stream::finalize;

		auto opts = mongocxx::options::find{};
		opts.sort(order.view());
		opts.limit(count);

		auto cursor = collection.find({}, opts);

		int irank = 1;

		for (auto doc : cursor)
		{
			auto name = doc["name"].get_utf8().value;
			auto content = doc["contents"].get_utf8().value;

			auto time_reg = doc["reg-time"].get_date().value;

			auto time = doc["time"].get_utf8().value;

			//tm local_tm;
			//localtime_s(&local_tm ,&timestamp);

			//char buff[256];
			//sprintf_s(buff,256, "%d-%d-%d %02d:%02d:%02d", local_tm.tm_year + 1900, local_tm.tm_mon + 1, local_tm.tm_mday, local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec);

			prayList.push_back(tuple<string, string, string>(name, content, time));
		}

		return prayList;
	}
	catch (...)
	{

		return std::list<tuple<string, string, string>>();
		printf("exception PrayList \n");
	}

}


int MongoDB::UpdaetPray(string name, string contents)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		std::time_t t = std::time(0);   // get time now
		std::tm now;

		localtime_s(&now, &t);

		string time;

		time.append(to_string((now.tm_year + 1900)));
		time.append("-");
		time.append(to_string((now.tm_mon + 1)));
		time.append("-");
		time.append(to_string((now.tm_mday)));
		time.append("-");
		time.append(to_string((now.tm_hour)));
		time.append("-");
		time.append(to_string((now.tm_min)));


		mongocxx::collection collection = db["PRAY"];
		///////////////////////////////////////////////////////////
		/////insert 
		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc_value = builder
			<< "name" << name
			<< "contents" << contents
			<< "reg-time" << bsoncxx::types::b_date(std::chrono::system_clock::now())
			<< "time" << time
			<< bsoncxx::builder::stream::finalize;

		bsoncxx::document::view view = doc_value.view();

		bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
			collection.insert_one(view);

		if (!result)
		{
			printf("UpdaetQNS  Index %s query failed \n", name);

			return -1;
		}

		return 0;
	}
	catch (...)
	{
		printf("exception UpdaetQNS \n");
	}
}


int	 MongoDB::ProcedureUserLogout(const int64_t id)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		mongocxx::collection collection = db["CONCURRENTUSER"];

		collection.delete_many(document{} << "INDEX" << id << finalize);
		return 0;
	}
	catch (...)
	{
		printf("exception ProcedureUserLogout \n");
	}
}

int  MongoDB::DeleteAllConcurrentUser()
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		mongocxx::collection collection = db["CONCURRENTUSER"];
		collection.delete_many(document{} << finalize);

		return 0;
	}
	catch (...)
	{
		printf("exception DeleteAllConcurrentUser \n");
	}
}


int64_t MongoDB::GetNextIndex()
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		auto collection = db["INDEX"];

		bsoncxx::builder::stream::document query{};
		bsoncxx::builder::stream::document update{};
		update << "$inc" << bsoncxx::builder::stream::open_document <<
			"INDEX" << 1 <<
			bsoncxx::builder::stream::close_document;

		auto serverQuery = collection.find_one_and_update(query.view(), update.view());

		if (serverQuery) {
			//Do something

			int64_t index = serverQuery.get().view()["INDEX"].get_int64();
			return index;
		}
		else
		{
			auto builder = bsoncxx::builder::stream::document{};
			bsoncxx::document::value doc_value = builder
				<< "INDEX" << bsoncxx::types::b_int64()
				<< bsoncxx::builder::stream::finalize;

			bsoncxx::document::view view = doc_value.view();

			bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
				collection.insert_one(view);
		}

		serverQuery = collection.find_one_and_update(query.view(), update.view());
		if (serverQuery) {
			//Do something

			int64_t index = serverQuery.get().view()["INDEX"].get_int64();
			return index;
		}

		return -1;
	}
	catch (...)
	{
		printf("exception GetNextIndex \n");
	}
}

int		MongoDB::ProcedureUserLogin(std::string id, std::string pwd, std::string& szKey, int& Score, INT64& Index, int& Level)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		mongocxx::collection collection = db["ACCOUNT"];
		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			collection.find_one(document{} << "Id" << id << finalize);
		if (maybe_result) //캐릭터가 이미 존재한다.
		{
			auto passwd = maybe_result->view()["Pwd"];
			if (passwd.get_utf8().value.data() != pwd)
			{
				printf("invalid passwd %s\n", id.c_str());
				return -1;
			}

			auto viewScore = maybe_result->view()["Score"];
			Score = viewScore.get_int32().value;

			auto viewLevel = maybe_result->view()["Level"];
			Level = viewLevel.get_int32().value;

			auto viewIndex = maybe_result->view()["INDEX"];
			Index = viewIndex.get_int64().value;

			//concurrent
			//접속 기록이 있다. 중복 접속
			{
				mongocxx::collection collection2 = db["CONCURRENTUSER"];
				bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
					collection2.find_one(document{} << "INDEX" << Index << finalize);
				if (maybe_result)
				{
					//printf("duplicate login ProcedureUserLogin CONCURRENTUSER  Index %I64d query failed \n", Index);
					return -2;
				}

				UUID id1;
				UuidCreate(&id1);
				RPC_STATUS s;
				char* tmp;
				UuidToStringA(&id1, (RPC_CSTR*)&tmp);

				szKey = (char*)tmp;

				auto builder2 = bsoncxx::builder::stream::document{};
				bsoncxx::document::value doc_value2 = builder2
					<< "Type" << 2
					<< "SessionKey" << szKey
					<< "INDEX" << Index
					<< bsoncxx::builder::stream::finalize;

				bsoncxx::document::view view2 = doc_value2.view();

				bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
					collection2.insert_one(view2);

				if (!result)
				{
					printf("fail insert CONCURRENTUSER  Index %I64d query failed \n", Index);

					return -3;
				}
			}

		}
		else
		{
			///////////////////////////////////////////////////////////
			///////insert 
			{
				int64_t index = GetNextIndex();

				auto builder = bsoncxx::builder::stream::document{};
				bsoncxx::document::value doc_value = builder
					<< "Id" << id
					<< "Pwd" << pwd
					<< "LastLoginTime" << bsoncxx::types::b_date(std::chrono::system_clock::now())
					<< "Score" << 0
					<< "Score" << 0
					<< "Level" << 0
					<< "INDEX" << index
					<< bsoncxx::builder::stream::finalize;

				bsoncxx::document::view view = doc_value.view();

				bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
					collection.insert_one(view);

				//캐릭터 생성 실패
				if (!result)
				{
					printf("fail insert ACCOUNT  Index %I64d query failed \n", Index);

					return -1;
				}

				//concurrent
				{
					UUID id1;
					UuidCreate(&id1);
					RPC_STATUS s;
					char* tmp;
					UuidToStringA(&id1, (RPC_CSTR*)&tmp);

					szKey = (char*)tmp;

					mongocxx::collection collection2 = db["CONCURRENTUSER"];

					auto builder2 = bsoncxx::builder::stream::document{};
					bsoncxx::document::value doc_value2 = builder2
						<< "Type" << 2
						<< "SessionKey" << szKey
						<< "INDEX" << index
						<< bsoncxx::builder::stream::finalize;

					bsoncxx::document::view view2 = doc_value2.view();

					bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
						collection2.insert_one(view2);

					if (!result)
					{
						printf("fail insert CONCURRENTUSER  Index %I64d query failed \n", Index);
						return -1;
					}
				}

				//auto index_specification = document{} << "INDEX" << 1 << finalize;
				//collection.create_index(std::move(index_specification));
				Index = index;
			}
		}
		return 0;
	}
	catch (...)
	{
		printf("exception ProcedureUserLogin \n");
	}
}


int MongoDB::CreateRoom(string room_name, INT64& user_id, string user_name)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		mongocxx::collection room_collection = db["ROOMS"];

		mongocxx::collection user_collection = db["ROOM_USERS"];

		int64_t rooom_index = GetNextIndex();

		auto builder = bsoncxx::builder::stream::document{};
		bsoncxx::document::value doc_value = builder
			<< "RoomId" << rooom_index
			<< "Pwd" << ""
			<< "LastLoginTime" << bsoncxx::types::b_date(std::chrono::system_clock::now())
			<< "Name" << room_name
			<< bsoncxx::builder::stream::finalize;

		bsoncxx::document::view view = doc_value.view();

		bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
			room_collection.insert_one(view);

		//방 생성 실패
		if (!result)
		{
			printf("fail insert ACCOUNT  Index %I64d query failed \n", user_id);

			return -1;
		}

		///////////////////////////////////////////////////////////
		///////user insert 
		{
			auto builder = bsoncxx::builder::stream::document{};
			bsoncxx::document::value doc_value = builder
				<< "UserId" << user_id
				<< "Name" << user_name
				<< "RoomId" << rooom_index
				<< bsoncxx::builder::stream::finalize;

			bsoncxx::document::view view = doc_value.view();

			bsoncxx::stdx::optional<mongocxx::result::insert_one> result = user_collection.insert_one(view);

			//유저 생성 실패
			if (!result)
			{
				printf("fail insert ACCOUNT  Index %I64d query failed \n", user_id);

				return -1;
			}
		}

		return rooom_index;
	}
	catch (...)
	{
		printf("exception ProcedureUserLogin \n");
	}
}

int MongoDB::EnterRoom(int room_id, INT64& user_id, string user_name)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		mongocxx::collection room_collection = db["ROOMS"];
		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			room_collection.find_one(document{} << "RoomId" << room_id << finalize);
		if (!maybe_result) //방이 없다.
		{
			return -1;
		}
		else
		{
			mongocxx::collection user_collection = db["ROOM_USERS"];
			bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
				user_collection.find_one(document{} << "UserId" << user_id << finalize);
			if (maybe_result) //이미 유저가 있다.
			{
				return 0;
			}

			///////////////////////////////////////////////////////////
			///////insert 
			{

				auto builder = bsoncxx::builder::stream::document{};
				bsoncxx::document::value doc_value = builder
					<< "UserId" << user_id
					<< "Name" << user_name
					<< "RoomId" << room_id
					<< bsoncxx::builder::stream::finalize;

				bsoncxx::document::view view = doc_value.view();

				bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
					user_collection.insert_one(view);

				//유저 생성 실패
				if (!result)
				{
					printf("fail insert ACCOUNT  Index %I64d query failed \n", user_id);

					return -1;
				}
			}
		}
		return 0;
	}
	catch (...)
	{
		printf("exception ProcedureUserLogin \n");
	}
}

int MongoDB::LeaveRoom(int room_id, INT64 user_id, string user_name)
{
	try
	{
		auto _client = pool.acquire();
		mongocxx::client& client = *_client;

		auto db = client[default_db_name];

		mongocxx::collection room_collection = db["ROOMS"];
		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			room_collection.find_one(document{} << "RoomId" << room_id << finalize);
		if (!maybe_result) //방이 없다.
		{
			return -1;
		}
		else
		{
			{
				mongocxx::collection user_collection = db["ROOM_USERS"];
				user_collection.delete_many(document{} << "RoomId" << room_id << "UserId" << user_id << finalize);
			}

			{
				mongocxx::collection user_collection = db["ROOM_USERS"];
				bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
					user_collection.find_one(document{} << "RoomId" << room_id << finalize);
				if (!maybe_result) //방에 더이상 유저가 없다.
				{
					mongocxx::collection user_collection = db["ROOMS"];
					user_collection.delete_many(document{} << "RoomId" << room_id << finalize);
					return 1;
				}
			}
		}
		return 0;
	}
	catch (...)
	{
		printf("exception ProcedureUserLogin \n");
	}
}

int MongoDB::AddRoomMessage(int room_id, INT64 user_id, string user_name, string msg, string time)
{
	///////////////////////////////////////////////////////////
	///////user insert 

	auto _client = pool.acquire();
	mongocxx::client& client = *_client;

	auto db = client[default_db_name];

	mongocxx::collection room_collection = db["ROOM_MSG"];

	auto builder = bsoncxx::builder::stream::document{};
	bsoncxx::document::value doc_value = builder
		<< "UserId" << user_id
		<< "Name" << user_name
		<< "RoomId" << room_id
		<< "Time" << time
		<< "Msg" << msg
		<< bsoncxx::builder::stream::finalize;

	bsoncxx::document::view view = doc_value.view();

	bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
		room_collection.insert_one(view);


	if (!result)
	{

		return -1;
	}
}


std::list<tuple<__int64, string, int, string, string>> MongoDB::LoadRoomMessage(int room_id, int count)
{
	std::list<tuple<__int64, string, int, string, string>> list;

	auto _client = pool.acquire();
	mongocxx::client& client = *_client;

	auto db = client[default_db_name];

	auto collection = db["ROOM_MSG"];

	auto order = bsoncxx::builder::stream::document{} << "Time" << -1 << bsoncxx::builder::stream::finalize;

	auto opts = mongocxx::options::find{};
	opts.sort(order.view());
	opts.limit(count);

	auto cursor = collection.find({}, opts);

	int irank = 1;

	for (auto doc : cursor)
	{
		auto userid = doc["UserId"].get_int64().value;
		auto name = doc["Name"].get_utf8().value;

		auto room_id = doc["RoomId"].get_int32().value;

		auto time = doc["Time"].get_utf8().value;
		auto msg = doc["Msg"].get_utf8().value;

		list.push_back(tuple<__int64, string, int, string, string>(userid, name, room_id, time, msg));
	}

	return list;
}

/*std::set<int> LoadRooms(INT64 user_id,int count = 100)
{
	std::set<int> list;

	auto collection = db["ROOM"];


	auto opts = mongocxx::options::find{};
	opts.limit(count);

	auto cursor = collection.find(document{} << "UserId" << user_id << finalize);

	int irank = 1;

	for (auto doc : cursor)
	{
		auto userid = doc["UserId"].get_int64().value;
		auto room_id = doc["RoomId"].get_int32().value;


		list.insert(room_id);
	}

	return list;
}*/

std::list<tuple<int, string, string>> MongoDB::LoadRooms(int count)
{
	std::list<tuple<int, string, string>> list;

	auto _client = pool.acquire();
	mongocxx::client& client = *_client;

	auto db = client[default_db_name];

	auto collection = db["ROOMS"];

	auto cursor = collection.find({});

	int irank = 1;

	for (auto doc : cursor)
	{
		auto name = doc["Name"].get_utf8().value;
		auto room_id = doc["RoomId"].get_int64().value;
		auto pwd = doc["Pwd"].get_utf8().value;

		list.push_back(tuple<int, string, string>(room_id, name, pwd));
	}

	return list;
}





#pragma comment(lib, "Dbghelp.lib")
