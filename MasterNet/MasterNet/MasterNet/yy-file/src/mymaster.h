#ifndef MYMASTER_H
#define MYMASTER_H

#include "myinclude.h"

class MyMasterServer
{
public:
	MyMasterServer();
	~MyMasterServer();

	void Start();
	void Stop();

private:

	static MyMasterServer *pThis;

	ClientsCallBackFuncs_t clients_callback_funcs;

	JProtocol *mastergate;

	//protocol callbackfunc
	static void  MasterOnData(int command, ResponeData data);
	void  MasterOnDataFunc(int command, ResponeData data);

	FILE *fp;

	//channel_1/2
	static void  SpecialGroup1Voice(ResponeRTPData data);
	void  SpecialGroup1VoiceFunc(ResponeRTPData data);
	static void  SpecialGroup2Voice(ResponeRTPData data);
	void  SpecialGroup2VoiceFunc(ResponeRTPData data);

	//channel_3/4
	static void  SpecialGroup3Voice(ResponeRTPData data);
	void  SpecialGroup3VoiceFunc(ResponeRTPData data);
	static void  SpecialGroup4Voice(ResponeRTPData data);
	void  SpecialGroup4VoiceFunc(ResponeRTPData data);

	//channel_5/6
	static void  SpecialGroup5Voice(ResponeRTPData data);
	void  SpecialGroup5VoiceFunc(ResponeRTPData data);
	static void  SpecialGroup6Voice(ResponeRTPData data);
	void  SpecialGroup6VoiceFunc(ResponeRTPData data);


};





#endif