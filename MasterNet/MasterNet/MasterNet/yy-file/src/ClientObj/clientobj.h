
/*
* myprotocol.h
*
* Created: 2018/01/03
* Author: EDWARDS
*/


#ifndef CLIENTOBJ_H
#define CLIENTOBJ_H

#include "..\Common\common.h"
#include "..\MyRTP\myrtp.h"
#include "..\..\lib\JsonWrap\include\json\json.h"
#include "..\Fifoqueue\fifoqueue.h"
#include "..\Socketwrap\socketwrap.h"
#include "..\SynInterface\syninterface.h"
#include <time.h>

#pragma pack(push, 1)
typedef struct{

	int32_t		bytes_remained;
	uint32_t	count;
	int32_t		pro_length;
	char		data[BUFLENGTH];

}StickDismantleOptions_t;

#pragma pack(pop)



class ClientObj
{
public:
	ClientObj(ClientParams_t params, MultiCallBackFuncs_t callbacks);
	~ClientObj();

private:

	/*
	初始化成员变量
	*/
	void InitProtocolData(ClientParams_t params, MultiCallBackFuncs_t callbacks);

	void DataProcessFunc();

	void(*RequestCallBackFunc)(int, ResponeData);//请求类回调
	void(*NotifyDeleteCallBackFunc)(ClientParams_t);//对象摧毁通知类回调
	void onData(void(*func)(int, ResponeData), int command, ResponeData data);
	void deOBJ(void(*func)(ClientParams_t), ClientParams_t clientparams);
	
	
	/*
	协议数据粘包/拆包函数
	*/
	int StickDismantleProtocol(HSocket fd, char *buff, int len, StickDismantleOptions_t &ptr);

	/*
	打包协议数据
	*/
	int PushRecvBuffToQueue(HSocket clientfd, char *buff, int buff_len);

	/*
	设置线程退出标志
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }
	bool set_thread_exit_flag;


	void CreateProcessClientReqThread();
	static int ProcessClientReqThread(void* p);
	int ProcessClientReqThreadFunc();

	void CreatProtocolParseThread();
	static int ProtocolParseThread(void *p);
	int ProtocolParseThreadFunc();

	void ProCallBackFunc(ClientPRO_States_t State);


	FifoQueue jqueue;//JSON data queue
	//socket初始化变量结构体
	socketoption_t socketoption;

	PROTOCOL_Ctrlr thePROTOCOL_Ctrlr;//协议结构
	std::map <std::string, int>  statemap;//状态机
	ClientPRO_States_t ClientPRO_State;
	//ILock *ondata_locker;
	//ILock *deobj_locker;
	MyCreateThread * process_client_thread_p;
	MyCreateThread * parse_thread_p;
	ClientParams_t clientparams;

	char recvbuff[BUFLENGTH];
	transresult_t rt;
	StickDismantleOptions_t temp_option;

	MyRTP *channel1RTP;
	MyRTP *channel2RTP;

};




#endif