#ifndef ALLCOMMON_H
#define ALLCOMMON_H
#include <map>
#include<mutex>
//¼ì²âÄÚ´æÐ¹Â©
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif  // _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif  // _DEBUG

typedef  struct tagRadioStatus{
	int    id;
	int	   status = 0;
	int    gpsQueryMode = 0;
} RadioStatus;
extern std::map<std::string, RadioStatus> g_radioStatus;
extern std::mutex g_radioStatusLocker;
#endif