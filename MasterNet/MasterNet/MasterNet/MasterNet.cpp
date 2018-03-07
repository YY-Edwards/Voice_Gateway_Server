// MasterNet.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
//#include "mymaster.h"
#include "yy-file\src\mymaster.h"


int _tmain(int argc, _TCHAR* argv[])
{
	char operat='\0';
	MyMasterGate master;

	std::cout << "Please input control cmd:\n" << endl;

	while ((operat=getchar()) != '0')
	{
		switch (operat)
		{
		case '1'://do something by yangyi
		{
			std::cout << "yy is debugging...\n" << endl;
			master.Start();

		}
			break;
		default:
			break;
		}
	}

	master.Stop();
	std::cout << "Master Gate Over!" << endl;

	return 0;
}

