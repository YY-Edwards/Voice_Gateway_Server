#ifndef ALLCOMMON_H
#define ALLCOMMON_H
#include <map>
#include<mutex>
typedef  struct tagRadioStatus{
	int    id;
	int	   status = 0;
	int    gpsQueryMode = 0;
} RadioStatus;
extern std::map<std::string, RadioStatus> g_radioStatus;
extern std::mutex g_radioStatusLocker;
#endif