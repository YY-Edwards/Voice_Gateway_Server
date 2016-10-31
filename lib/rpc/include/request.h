#pragma once

#include <string>
#include <functional>

#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

#define				PingTime				30			// 30 seconds

class CRequest{
public:
	std::string m_strRequest;
	uint64_t m_nCallId;
	bool m_bNeedResponse;

	std::function<void(const char*, void*)> success;
	std::function<void(const char*, void*)> failed;

	int nTimeoutSeconds;

	void* data;
};