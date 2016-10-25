#pragma once

#include <string>

class CCriteria
{
public:
	std::string field;
	std::string val;
	std::string releation;
	std::string compare;

public:
	CCriteria(){
		field = "";
		val = "";
		releation = "";
		compare = "";
	}
};