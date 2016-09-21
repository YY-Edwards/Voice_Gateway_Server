#pragma once

#ifndef _ABSCTRACT_ACTION
#define _ABSCTRACT_ACTION

#include <map>
#include <string>

class CRemotePeer;

class CAbstractAction{
public:
	virtual std::string getName() = 0;
	virtual int run(CRemotePeer* pRemote, std::map<std::string, std::string> args) = 0;
};
#endif