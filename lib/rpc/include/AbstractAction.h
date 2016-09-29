#pragma once

#ifndef _ABSCTRACT_ACTION
#define _ABSCTRACT_ACTION

#include <map>
#include <string>
#include <list>

#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

class CRemotePeer;

class CAbstractAction{
public:
	virtual std::string getName() = 0;
	virtual int run(CRemotePeer* pRemote, const std::string& param, uint64_t callId) = 0;
};
#endif