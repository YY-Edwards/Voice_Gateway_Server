#include "stdafx.h"
#include "allCommon.h"
std::map<std::string, RadioStatus> g_radioStatus;
std::mutex g_radioStatusLocker;