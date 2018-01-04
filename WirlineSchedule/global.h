#ifndef _XQTT_GLOBAL_H
#define _XQTT_GLOBAL_H

#include <stddef.h>

#ifdef _WIN32
#include <stdbool.h>
#endif // _WIN32

#define			SafeCall(pFunc, ...)	do{if(((void*)0)!=pFunc){pFunc(__VA_ARGS__);}}while(0)
#define			SafeCallNoArg(pFunc)	do{if(((void*)0)!=pFunc){pFunc();}}while(0)
#define			SafeDelete(p)			do{if((void*)0!=p){free(p);p=(void*)0;}}while(0)

#ifndef NULL
#define NULL ((void*)0)
#endif // !NULL


#endif // !_XQTT_GLOBAL_H
