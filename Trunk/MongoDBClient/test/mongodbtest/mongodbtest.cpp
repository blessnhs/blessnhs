// mongodbtest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		mongo::DBClientConnection c;
		c.connect("devdb.bspfp.myhome");
	}
	catch (mongo::DBException const & ex)
	{
		std::cout << "exception: " << ex.what() << std::endl;
	}
	return 0;
}

