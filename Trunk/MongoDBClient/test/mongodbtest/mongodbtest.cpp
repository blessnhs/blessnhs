// mongodbtest.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

