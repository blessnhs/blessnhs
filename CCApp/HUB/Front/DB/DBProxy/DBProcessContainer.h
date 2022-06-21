#pragma once



class DBProcessContainer : public IContainer<DBPROCESS_CER_PTR>, public Singleton<DBProcessContainer>
{
public:

	DBProcessContainer(void);
	~DBProcessContainer(void);

	virtual bool Add(DBPROCESS_CER_PTR &pObj);
	virtual bool Del(DBPROCESS_CER_PTR &pObj);
	virtual DBPROCESS_CER_PTR Search(INT64 Id);

	//로그인아웃 처리는 고정해야할 필요가 있다.
	//id에 의해 분할되면 멀티 쓰레드 동기화 문제가 발생할 가능성이 존재한다.
	DBPROCESS_CER_PTR GetFirstHandle();

	void TryConnect();

	void Initialize(INT Count);

	std::map<DWORD,DBPROCESS_CER_PTR> m_List;
};

#define DBPROCESSCONTAINER_CER Singleton<DBProcessContainer>::Instance()