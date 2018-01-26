
/*
* myprotocol.h
*
* Created: 2018/01/03
* Author: EDWARDS
*/


#ifndef MYPROTOCOL_H
#define MYPROTOCOL_H

#include "common.h"
#include "myrtp.h"
#include "json.h"
#include "fifoqueue.h"
#include "socketwrap.h"
#include "clientobj.h"
#include <time.h>


class JProtocol
{
public:
	JProtocol();
	~JProtocol();


	/*
	TCP_CMD 协议解析开始
	*/
	void Start();
	
	/*
	回调接口
	设置回调函数
	*/
	//void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));
	void SetCallBackFunc(ClientsCallBackFuncs_t funcs);

	/*
	连接回复:
	*/
	void ConnectReply(HSocket fd, std::string status, std::string reason = "");

	/*
	配置回复
	*/
	void ConfigReply(HSocket fd, Listening_Reply_Params_Channels_t channel_params);

	/*
	查询回复
	*/

	void QueryReply(HSocket fd, int channel1_value, int channel2_value);

	/*
	请求发起组呼回复
	*/
	void CallRequestReply(HSocket fd, std::string status, std::string reason);

	/*
	结束组呼回复
	*/
	void CallReleaseReply(HSocket fd, std::string status, std::string reason);

	/*
	组呼开始通知
	*/
	void CallStartNotify(HSocket fd, int src, int dst, std::string channel);

	/*
	组呼结束通知
	*/
	void CallEndNotify(HSocket fd, int src, int dst, std::string channel);

	/*
	关闭服务器端
	*/
	void CloseMater();

	/*
	获取Master初始化是否完成
	*/
	bool IsMaterInitComplete()
	{
		return startfunc_is_finished;
	}

private:

	HSocket serversoc;
	struct sockaddr_in my_addr; /* loacl */
	/*
	声明socket接口类
	*/
	CSockWrap *mytcp_server;

	MultiCallBackFuncs_t multicallbackfuncs;
	RTPCallBackFunc_t RTPcallBackfuncs[MAX_CLIENT_COUNT*MAX_SPECIAL_GROUP];

	//回调接口
	//void(*RequestCallBackFunc)(int, ResponeData);//请求类回调
	//void(*NotifyCallBackFunc)(int, ResponeData);//通知类回调
	//void onData(void(*func)(int, ResponeData), int command, ResponeData data);

	MyCreateThread *listen_thread_p;
	MyCreateThread *recovery_thread_p;

	void CreateListenThread();
	static int ListenThread(void* p);
	int ListenThreadFunc();

	void CreateRecoveryClientObjThread();
	static int RecoveryClientObjThread(void* p);
	int RecoveryClientObjThreadFunc();


	static JProtocol * pThis;
	static void NotifyRecoveryClienObj(ClientParams_t clientparams);
	void NotifyDeleleClienObjFunc(ClientParams_t clientparams);
	FifoQueue clientqueue;
	
	/*
	打包json数据包
	*/
	int SendDataToTheThirdParty(HSocket fd, std::string buff);

	/*
	socket初始化
	*/
	bool listenIsopen;;
	bool InitSocket();
	bool CloseSocket(HSocket sockfd);

	/*
	初始化成员变量
	*/
	void InitProtocolData();

	ILock *clientmap_locker;

	std::map <ClientParams_t , ClientObj *>  clientmap;//save client-info

	bool set_thread_exit_flag;
	int client_numb;//客户端计数值

	/*
	设置线程退出标志
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }
	bool startfunc_is_finished;

	/*
	获取UUID
	*/
	std::string CreateGuid();
	

	/*
	物理层发送协议数据包
	*/
	int PhySocketSendData(HSocket fd, char *buff, int buff_len);

	bool ConfigClientParams(ClientParams_t &clientparams);



};










#endif