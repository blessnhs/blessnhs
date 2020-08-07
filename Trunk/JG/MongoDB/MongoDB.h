#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <list>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <rpc.h>
#include <windows.h>
#include <iostream>

#include <time.h>
#include <boost/make_shared.hpp>

#include "CLI.GS.pb.h"

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
using namespace std;

class MongoDB
{
public:
	MongoDB(string db_name)
	{
		_client = mongocxx::client(mongocxx::uri("mongodb://localhost:27017/?minPoolSize=2&maxPoolSize=4"));
		db = _client[default_db_name];

		default_db_name = db_name;

		CheckIndexCollection();
	}

	~MongoDB()
	{
	}

private:

	std::string default_db_name = "Board";

	mongocxx::client _client;
	mongocxx::database db;

	//인덱스가 생성이 안되었으면 여기서 생성한다.
	bool CheckIndexCollection()
	{
		return true;
	}

public:

	//버전 정보를 가져온다.
	double ProcedureVersion(int type)
	{
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

		return 0;
	}

	//승수로 정렬해서 상위1 10개만 가져오고 각 해당 유저에게 rank를 업데이트 한다.
	int CalcRank(int count = 10)
	{
		auto collection_acconut = db["ACCOUNT"];
		auto collection_rank = db["RANK"];

		collection_rank.drop();

		auto order = bsoncxx::builder::stream::document{} << "Win" << -1 << bsoncxx::builder::stream::finalize;

		auto opts = mongocxx::options::find{};
		opts.sort(order.view());
		opts.limit(count);

		auto cursor = collection_acconut.find({}, opts);

		int irank = 1;

		for (auto doc : cursor)
		{

			bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
				collection_rank.insert_one(doc);

			//각각 유저 업데이트
			{
				auto viewIndex = doc["INDEX"];
				int64_t Index = viewIndex.get_int64().value;

				bsoncxx::builder::stream::document query{};

				query << "INDEX" << Index;

				bsoncxx::builder::stream::document update{};
				update << "$set" << bsoncxx::builder::stream::open_document <<
					"Rank" << irank++ <<
					bsoncxx::builder::stream::close_document;

				auto serverQuery = collection_acconut.find_one_and_update(query.view(), update.view());
				if (!serverQuery)
				{
					printf("%I64d user rank %d query failed \n", Index, irank - 1);
				}
			}


		}

		return 0;
	}


	void  RequestRank(std::list<Rank>& list, int count = 10)
	{
		mongocxx::collection collection = db["RANK"];

		auto order = bsoncxx::builder::stream::document{} << "Rank" << 1 << bsoncxx::builder::stream::finalize;

		auto opts = mongocxx::options::find{};
		opts.sort(order.view());
		opts.limit(count);

		//find all
		mongocxx::cursor cursor = collection.find(
			{}, opts);

		for (bsoncxx::document::view doc : cursor)
		{
			Rank r;

			auto viewRank = doc["Rank"];
			r.set_var_rank(viewRank.get_int32().value);

			auto viewLevel = doc["Level"];
			r.set_var_level(viewLevel.get_int32().value);

			auto viewLose = doc["Lose"];
			r.set_var_lose(viewLose.get_int32().value);

			auto viewWin = doc["Win"];
			r.set_var_win(viewWin.get_int32().value);

			auto viewDraw = doc["Draw"];
			r.set_var_draw(viewDraw.get_int32().value);

			auto viewName = doc["Name"];
			r.set_var_name( viewName.get_utf8().value.data());

			auto picUrl = doc["PictureUrl"];
			r.set_var_pic_uri( picUrl.get_utf8().value.data());

			list.push_back(r);
		}

	}


	int UpdaetPlayerScore(int64_t Index, int Win, int Lose, int Draw, int Level, int Score)
	{
		auto collection = db["ACCOUNT"];

		bsoncxx::builder::stream::document query{};

		/*query << "INDEX" << Index;*/

		//bsoncxx::builder::stream::document update{};
		//update << "$inc" << bsoncxx::builder::stream::open_document <<
		//	"Win" << Win <<
		//	bsoncxx::builder::stream::close_document
		//	<< "$inc" << bsoncxx::builder::stream::open_document <<
		//	"Lose" << Lose <<
		//	bsoncxx::builder::stream::close_document
		//	<< "$inc" << bsoncxx::builder::stream::open_document <<
		//	"Draw" << Draw <<
		//	bsoncxx::builder::stream::close_document
		//	<< "$set" << bsoncxx::builder::stream::open_document <<
		//	"Level" << Level <<
		//	bsoncxx::builder::stream::close_document
		//	<< "$set" << bsoncxx::builder::stream::open_document <<
		//	"Score" << Score <<
		//	bsoncxx::builder::stream::close_document;


		auto serverQuery = db["ACCOUNT"].update_one(
			make_document(kvp("INDEX", Index)),
			make_document(
				kvp("$inc", make_document(kvp("Win", Win))),
				kvp("$inc", make_document(kvp("Lose", Lose))),
				kvp("$inc", make_document(kvp("Draw", Draw))),
				kvp("$set", make_document(kvp("Level", Level))),
				kvp("$set", make_document(kvp("Score", Score)))));

	/*	auto serverQuery = collection.update_many(query.view(), update.view());*/

		if (!serverQuery)
		{
			printf("UpdaetPlayerScore  Index %I64d query failed \n", Index);
			return -1;
		}

		return 0;
	}

	std::string ToAnsi(const char* pszIn)
	{
		std::string resultString;

		int nLenOfUni = 0, nLenOfANSI = 0;
		wchar_t* us = NULL;
		char* pszOut = NULL;

		if ((nLenOfUni = MultiByteToWideChar(CP_UTF8, 0, pszIn, strlen(pszIn), NULL, 0)) <= 0)
			return 0;

		us = new wchar_t[nLenOfUni + 1];
		memset(us, 0x00, sizeof(wchar_t) * (nLenOfUni + 1));

		// utf8 --> unicode
		nLenOfUni = MultiByteToWideChar(CP_UTF8, 0, pszIn, strlen(pszIn), us, nLenOfUni);

		if ((nLenOfANSI = WideCharToMultiByte(CP_ACP, 0, us, nLenOfUni, NULL, 0, NULL, NULL)) <= 0)
		{
			//free(us);
			delete[] us;
			return 0;
		}

		pszOut = new char[nLenOfANSI + 1];
		memset(pszOut, 0x00, sizeof(char) * (nLenOfANSI + 1));

		// unicode --> ansi
		nLenOfANSI = WideCharToMultiByte(CP_ACP, 0, us, nLenOfUni, pszOut, nLenOfANSI, NULL, NULL);
		pszOut[nLenOfANSI] = 0;
		resultString = pszOut;

		delete[] us;
		delete[] pszOut;

		return resultString;
	}

	int NoticeInfoInfo(string& notice)
	{
		mongocxx::collection collection = db["NOTICE"];
		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			collection.find_one(document{} << finalize);
		if (maybe_result)
		{
			auto view = maybe_result->view()["text"];

			auto strview = view.get_utf8().value;
			notice = strview.to_string();

			notice = ToAnsi(notice.c_str());
		}

		return 0;
	}

	int UpdaetQNS(int64_t Index, std::string contents)
	{
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

	int	 ProcedureUserLogout(const int64_t id)
	{
		mongocxx::collection collection = db["CONCURRENTUSER"];

		collection.delete_many(document{} << "INDEX" << id << finalize);
		return 0;
	}

	int  DeleteAllConcurrentUser()
	{
		mongocxx::collection collection = db["CONCURRENTUSER"];
		collection.delete_many(document{} << finalize);

		return 0;
	}


	int64_t GetNextIndex()
	{
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

	int		ProcedureUserLogin(const char* flatformid, const int flatformtype, const char* name, const char* picture_url, const char* email, std::string& szKey, int& Rank, int& Score, int& Win, int& Lose, int& Draw, INT64& Index, int& Level)
	{
		mongocxx::collection collection = db["ACCOUNT"];
		bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
			collection.find_one(document{} << "FlatformId" << flatformid << "FlatformType" << flatformtype << finalize);
		if (maybe_result) //캐릭터가 이미 존재한다.
		{
			auto viewScore = maybe_result->view()["Score"];
			Score = viewScore.get_int32().value;

			auto viewRank = maybe_result->view()["Rank"];
			Rank = viewRank.get_int32().value;

			auto viewWin = maybe_result->view()["Win"];
			Win = viewWin.get_int32().value;

			auto viewLose = maybe_result->view()["Lose"];
			Lose = viewLose.get_int32().value;

			auto viewDraw = maybe_result->view()["Draw"];
			Draw = viewDraw.get_int32().value;

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
					printf("duplicate login ProcedureUserLogin CONCURRENTUSER  Index %I64d query failed \n", Index);
					return -1;
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

					return -1;
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
					<< "FlatformId" << flatformid
					<< "FlatformType" << flatformtype
					<< "Name" << name
					<< "PictureUrl" << picture_url
					<< "Email" << email
					<< "LastLoginTime" << bsoncxx::types::b_date(std::chrono::system_clock::now())
					<< "Score" << 0
					<< "Rank" << 0
					<< "Score" << 0
					<< "Win" << 0
					<< "Lose" << 0
					<< "Draw" << 0
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
				Score = Rank = Win = Lose = Draw = Level = 0;
			}
		

		}


		return 0;
	}


};


#pragma comment(lib, "Dbghelp.lib")

std::pair<std::string, std::string> GenerateDumpFilePath()
{
	/*time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", timeinfo);
	std::string str(buffer);

	const string&& filename(buffer);

	boost::filesystem::path path = boost::filesystem::current_path();
	path.append("dumps");
	path.append(filename);

	path.replace_extension("dmp");
	const string&& dump = path.generic_string<std::string>();
	path.replace_extension("txt");
	const string&& cs = path.generic_string<std::string>();

	boost::filesystem::create_directory(path.branch_path().generic_string<std::string>());*/

	//return std::make_pair(dump, cs);

	return std::pair<std::string, std::string>();
}
