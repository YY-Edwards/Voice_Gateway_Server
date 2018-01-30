
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
	TCP_CMD Э�������ʼ
	*/
	void Start();

	void Stop();
	
	/*
	�ص��ӿ�
	���ûص�����
	*/
	//void SetCallBackFunc(void(*callBackFunc)(int, ResponeData));
	void SetCallBackFunc(ClientsCallBackFuncs_t funcs);

	/*
	���ӻظ�:
	*/
	void ConnectReply(HSocket fd, std::string status, std::string reason = "");

	/*
	���ûظ�
	*/
	void ConfigReply(HSocket fd, Listening_Reply_Params_Channels_t channel_params);

	/*
	��ѯ�ظ�
	*/

	void QueryReply(HSocket fd, int channel1_value, int channel2_value);

	/*
	����������ظ�
	*/
	void CallRequestReply(HSocket fd, std::string status, std::string reason);

	/*
	��������ظ�
	*/
	void CallReleaseReply(HSocket fd, std::string status, std::string reason);

	/*
	�����ʼ֪ͨ
	*/
	void CallStartNotify(HSocket fd, int src, int dst, std::string channel);

	/*
	�������֪ͨ
	*/
	void CallEndNotify(HSocket fd, int src, int dst, std::string channel);

	/*
	�رշ�������
	*/
	void CloseMater();

	/*
	��ȡMaster��ʼ���Ƿ����
	*/
	bool IsMaterInitComplete()
	{
		return startfunc_is_finished;
	}

private:

	HSocket serversoc;
	struct sockaddr_in my_addr; /* loacl */
	/*
	����socket�ӿ���
	*/
	CSockWrap *mytcp_server;

	MultiCallBackFuncs_t multicallbackfuncs;
	RTPCallBackFunc_t RTPcallBackfuncs[MAX_CLIENT_COUNT*MAX_SPECIAL_GROUP];

	//�ص��ӿ�
	//void(*RequestCallBackFunc)(int, ResponeData);//������ص�
	//void(*NotifyCallBackFunc)(int, ResponeData);//֪ͨ��ص�
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
	���json���ݰ�
	*/
	int SendDataToTheThirdParty(HSocket fd, std::string buff);

	/*
	socket��ʼ��
	*/
	bool listenIsopen;;
	bool InitSocket();
	bool CloseSocket(HSocket sockfd);

	/*
	��ʼ����Ա����
	*/
	void InitProtocolData();

	ILock *clientmap_locker;

	std::map <ClientParams_t , ClientObj *>  clientmap;//save client-info

	bool set_thread_exit_flag;
	int client_numb;//�ͻ��˼���ֵ

	/*
	�����߳��˳���־
	*/
	void SetThreadExitFlag()   { set_thread_exit_flag = true; }
	bool startfunc_is_finished;

	/*
	��ȡUUID
	*/
	std::string CreateGuid();
	

	/*
	����㷢��Э�����ݰ�
	*/
	int PhySocketSendData(HSocket fd, char *buff, int buff_len);

	bool ConfigClientParams(ClientParams_t &clientparams);



};










#endif