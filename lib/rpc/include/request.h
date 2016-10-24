#pragma once

#include <string>
#include <functional>

#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif


class CRequest{
public:
	std::string m_strRequest;
	uint64_t m_nCallId;

	std::function<void(const char*, void*)> success;
	std::function<void(const char*, void*)> failed;

	void* data;
};