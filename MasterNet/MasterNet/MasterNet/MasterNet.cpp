// MasterNet.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "mymaster.h"


int _tmain(int argc, _TCHAR* argv[])
{
	char operat='\0';
	MyMasterServer master_server;

	std::cout << "Please input control cmd:\n" << endl;

	while ((operat=getchar()) != '0')
	{
		switch (operat)
		{
		case '1'://do something by yangyi
		{
			std::cout << "yy is debugging...\n" << endl;
			master_server.Start();

		}
			break;
		default:
			break;
		}
	}

	master_server.Stop();
	std::cout << "Master Server Over!" << endl;

	return 0;
}

