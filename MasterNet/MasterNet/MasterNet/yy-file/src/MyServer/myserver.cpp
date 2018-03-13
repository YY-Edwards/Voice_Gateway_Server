/*
* myserver.cpp
*
* Created: 2018/01/03
* Author: EDWARDS
*/
#include "stdafx.h"
#include "myserver.h"

MyServer *MyServer::pThis = NULL;

MyServer::MyServer()
{
	 InitProtocolData();
}

MyServer::~MyServer()
{

	std::map<ClientParams_t, ClientObj *> ::iterator it;
	while (clientmap.size()>0)
	{
		it = clientmap.begin();
		if (it->second != NULL)
		{
			delete it->second;
			it->second = NULL;
		}
		clientmap.erase(it);
	}

	if (clientmap_locker != NULL)
	{
		delete clientmap_locker;
		clientmap_locker = NULL;
	}

	FreeSocketEnvironment();
	pThis = NULL;
	std::cout<<"Destory: MyServer \n"<<std::endl;

}

void MyServer::Stop()
{
	CloseMater();
	SetThreadExitFlag();//֪ͨ�߳��˳�
	if (recovery_thread_p != NULL)
	{
		delete recovery_thread_p;
		recovery_thread_p = NULL;
	}

	{
		if (listen_thread_p != NULL)
		{
			delete listen_thread_p;
			listen_thread_p = NULL;
		}

	}
	listenIsopen = false;
	startfunc_is_finished = false;
	set_thread_exit_flag = false;

	std::cout << "Stop: MyServer-thread\n" << std::endl;
}

void MyServer::InitProtocolData()
{
	pThis = this;

	InitializeSocketEnvironment();

	mytcp_server = NULL;
	listenIsopen = false;

	startfunc_is_finished = false;
	set_thread_exit_flag = false;

	serversoc = INVALID_SOCKET;
	client_numb = 0;
	recovery_thread_p = NULL;
	listen_thread_p = NULL;

	multicallbackfuncs.RequestCallBackFunc = NULL;//���ϲ��ȡ
	multicallbackfuncs.ExitNotifyCallBackFunc = NotifyRecoveryClienObj;//֪ͨ���ص��ӿ�
	multicallbackfuncs.channel1.RecvVoiceDataFunc = NULL;//���ϲ��ȡ
	multicallbackfuncs.channel2.RecvVoiceDataFunc = NULL;//���ϲ��ȡ

	for (int i = 0; i < (MAX_CLIENT_COUNT*MAX_SPECIAL_GROUP); i++)
	{
		RTPcallBackfuncs[i].RecvVoiceDataFunc = NULL;
	}

	clientmap_locker = new CriSection();

}
void MyServer::CloseMater()
{
	if (serversoc != INVALID_SOCKET)
	{
		serversoc = INVALID_SOCKET;
		delete mytcp_server;
		mytcp_server = NULL;
	}

	std::cout<<"Close Server\n"<<std::endl;

}
void MyServer::SetCallBackFunc(ClientsCallBackFuncs_t funcs)
{
	//�ص�����
	multicallbackfuncs.RequestCallBackFunc = funcs.RequestCallBackFunc;
	memcpy(RTPcallBackfuncs, funcs.RTPcallbackfuncs, sizeof(funcs.RTPcallbackfuncs));
}
void MyServer::Start()
{
	bool status = false;
	status = InitSocket();
	if (status != true)
	{
		std::cout<<"InitSocket fail...\n"<<std::endl;
	}
	else
	{
		//��ʼ�����
		CreateListenThread();
		CreateRecoveryClientObjThread();
		listenIsopen = true;
		startfunc_is_finished = status;
	}

}
bool MyServer::InitSocket()
{
	if (mytcp_server != NULL)
	{
		mytcp_server->Reopen(TRUE);
		std::cout << "# mytcp_server Reopen # \n" << std::endl;
	}
	else
	{
		mytcp_server = new CSockWrap(SOCK_STREAM);
	}
	serversoc = mytcp_server->GetHandle();//��ȡ�����������
	GetAddressFrom(&my_addr, 0, TCPPORT);//��������IP

	if (mytcp_server->SetReuseAddr() != 0)
	{
		std::cout << "The server SetReuseAddr fail!\n" << std::endl;
	}

	//����sockΪnon-blocking
	mytcp_server->SetBlock(0);

	if (SocketBind(serversoc, &my_addr) == SOCKET_ERROR)
	{
		std::cout << "SocketBind fail!\n" << std::endl;
		delete mytcp_server;
		mytcp_server = NULL;
		serversoc = INVALID_SOCKET;
		return false;
	}
	if (SocketListen(serversoc, MAX_CLIENT_COUNT) != 0)
	{
		std::cout << "SocketListen fail!\n" << std::endl;
		delete mytcp_server;
		mytcp_server = NULL;
		serversoc = INVALID_SOCKET;
		return false;
	}


	/*
	if (socketopen)
	{
		CloseSocket(serversoc);
	}

	if ((serversoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) //create a tcp socket  
	{
		std::cout<<"Create socket fail!\n"<<std::endl;
		//printf("Create socket fail!\n");
		return false;
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(TCPPORT);
	my_addr.sin_addr.S_un.S_addr = INADDR_ANY;

	//�����ص�ַ�󶨵����������׽�����
	if (bind(serversoc, (LPSOCKADDR)&my_addr, sizeof(my_addr)) == SOCKET_ERROR)
	{
		CloseSocket(serversoc);
		return false;
	}

	//start listen
	if (listen(serversoc, 5) != 0)
	{
		CloseSocket(serversoc);
		return false;
	}
	socketopen = true;

	*/


	return true;

}
void MyServer::CreateRecoveryClientObjThread()
{
	recovery_thread_p = new MyCreateThread(RecoveryClientObjThread, this);

}
void MyServer::CreateListenThread()
{
	{
		listen_thread_p = new MyCreateThread(ListenThread, this);
	}

}
int MyServer::ListenThread(void* p)
{
	MyServer *arg = (MyServer*)p;
	int return_value = 0;
	if (arg != NULL)
	{
		return_value = arg->ListenThreadFunc();
	}
	return return_value;
}

int MyServer::updateMaxfd(fd_set fds, int maxfd)
{
	int i;
	int new_maxfd = 0;
	for (i = 0; i <= maxfd; i++) {
		if (FD_ISSET(i, &fds) && i > new_maxfd) {
			new_maxfd = i;
		}
	}
	return new_maxfd;
}


int MyServer::ListenThreadFunc()
{

#if 1//������select,non-blockingģ��

	//��������ʼ��select��Ҫ�Ĳ���(���������read)������sock��ӵ�fd_set��

	fd_set readfds;
	fd_set readfds_bak; //backup for readfds(����ÿ��select֮������readfds�������Ҫbackup)
	struct timeval timeout;
	int maxfd;
	maxfd = serversoc;
	FD_ZERO(&readfds);
	FD_ZERO(&readfds_bak);
	FD_SET(serversoc, &readfds_bak);

	//ѭ������client����
	int return_value = 0;
	ClientParams_t clientparams;
	clientparams.socket_fd = INVALID_SOCKET;
	//socklen_t client_addr_len;
	//char client_ip_str[INET_ADDRSTRLEN];
	int res;
	std::cout << "The server is waiting for a newest connection \n" << std::endl;
	while (!set_thread_exit_flag) {

		//ע��select֮��readfds��timeout��ֵ���ᱻ�޸ģ����ÿ�ζ���������
		readfds = readfds_bak;
		maxfd = updateMaxfd(readfds, maxfd);        //����maxfd
		timeout.tv_sec = SELECT_TIMEOUT;
		timeout.tv_usec = 0;
		//printf("selecting maxfd=%d\n", maxfd);

		//select(����û������writefds��errorfds��������Ҫ��������)
		res = select(maxfd + 1, &readfds, NULL, NULL, &timeout);
		if (res == -1) 
		{
			return_value = -1;
			std::cout << "Server select fail!\n"<< std::endl;
			break;
			//perror("select failed");
			//exit(EXIT_FAILURE);
		} else if (res == 0) 
		{
			//fprintf(stderr, "no socket ready for read within %d secs\n", SELECT_TIMEOUT);
			continue;
		}

		//���ÿ��socket�������ж�(�����serversoc��accept)
		//for (i = 0; i <= maxfd; i++) 
		{	//���serversoc, ����accept

			if (!FD_ISSET(serversoc, &readfds))
			{
				continue;
			}
			//�ɶ���socket
			//if (i == serversoc) //��ǰ��server��socket�������ж�д����accept������
			{
				clientparams.socket_fd = SocketAccept(serversoc, (sockaddr_in*)&(clientparams.remote_addr));
				if (clientparams.socket_fd == INVALID_SOCKET) 
				{
					return_value = -1;
					std::cout << "Accept fail! and exit ListenThreadFunc: 0x" << hex << listen_thread_p->GetPthreadID() << std::endl;
					break;
					//perror("accept failed");
				}

				//if (!inet_ntop(AF_INET, &(clientparams.remote_addr.sin_addr), client_ip_str, sizeof(client_ip_str)))
				//{
				//	//perror("inet_ntop failed");
				//	std::cout << "inet_ntop failed!\n" << std::endl;
				//	break;
				//}
				//printf("accept a client from: %s\n", client_ip_str);

				if (clientmap.size() < MAX_CLIENT_COUNT)
				{
					client_numb++;

					ConfigClientParams(clientparams);//��̬����RTP�˿ڣ��ص���ַ

					ClientObj *clientobj_p = new ClientObj(clientparams, multicallbackfuncs);

					clientmap_locker->Lock();
					clientmap[clientparams] = clientobj_p;
					clientmap_locker->Unlock();
					std::cout << "Create a ClientObj finished\n" << std::endl;
				}

				//��new_sock��ӵ�select��������
				if (clientparams.socket_fd > maxfd) {
					maxfd = clientparams.socket_fd;
				}
				//FD_SET(clientparams.socket_fd, &readfds_bak);
			}
			//else {
			//	//��ǰ��client���ӵ�socket������д(read from client)
			//	printf("close new_sock=%d done\n", i);
			//	//����ǰ��socket��select���������Ƴ�
			//	//FD_CLR(i, &readfds_bak);
			//}
		}
	}

	std::cout << "exit ListenThreadFunc: 0x" << hex << listen_thread_p->GetPthreadID() << std::endl;
	return 0;

#endif

#if 0//���߳�ģ�ͣ���Ҫ����zoom�߳�

	int return_value = 0;
	ClientParams_t clientparams;
	clientparams.socket_fd = INVALID_SOCKET;

	std::cout<<"The server is waiting for a newest connection \n"<<std::endl;
	clientparams.socket_fd = SocketAccept(serversoc, (sockaddr_in*)&(clientparams.remote_addr));

	if (serversoc != INVALID_SOCKET)
	{
		CreateListenThread();//���������������������
	}

	if (clientparams.socket_fd == INVALID_SOCKET)
	{
		return_value = -1;
		std::cout << "Accept fail! and exit ListenThreadFunc: 0x" << hex << GetCurrentThreadId ()<< std::endl;
		//TRACE(("Accept fail! and exit listenthread: 0x%x\n"), GetCurrentThreadId());
	}
	else
	{
		if (clientmap.size() < MAX_CLIENT_COUNT)
		{
			client_numb++;
			ConfigClientParams(clientparams);//��̬����RTP�˿ڣ��ص���ַ

			ClientObj *clientobj_p = new ClientObj(clientparams, multicallbackfuncs);

			clientmap_locker->Lock();
			clientmap[clientparams] = clientobj_p;
			//clientmap.insert(std::pair<ClientParams_t, ClientObj *>(clientparams, clientobj_p));//save client info(socketfd,ip,port...) to map
			clientmap_locker->Unlock();
			std::cout << "Create ClientObj finished and exit ListenThreadFunc: 0x" << hex << GetCurrentThreadId() << std::endl;
		}
		else
		{
			SocketClose(clientparams.socket_fd);
			std::cout << "client_numb is full ,close client and exit ListenThreadFunc: 0x" << hex << GetCurrentThreadId() << std::endl;
		}

	}

	return return_value;


#endif
}
bool MyServer::ConfigClientParams(ClientParams_t &clientparams)
{
	std::map<ClientParams_t, ClientObj *> ::iterator it;
	int numb = 0;

	clientmap_locker->Lock();

	it = clientmap.begin();
	while (it!=clientmap.end())
	{
		for (; it != clientmap.end(); ++it)
		{
			if ((it->first.channel1.rtp_portbase) == (CHANNEL1RTPBASEPORT_START + (numb * 8)))//����portbase�Ƿ��ѱ�ʹ��
			{
				numb++;//��һ��
				break;
			}
		}

	}

	clientmap_locker->Unlock();
	if (numb > (MAX_CLIENT_COUNT*MAX_SPECIAL_GROUP))
	{
		std::cout << "numb is full!!!\n"<< std::endl;
		return false;
	}

	multicallbackfuncs.channel1.RecvVoiceDataFunc = RTPcallBackfuncs[numb*2].RecvVoiceDataFunc;
	multicallbackfuncs.channel2.RecvVoiceDataFunc = RTPcallBackfuncs[numb*2+1].RecvVoiceDataFunc;
	
	clientparams.channel1.rtp_portbase = CHANNEL1RTPBASEPORT_START + (numb * 8);
	clientparams.channel1.rtp_destport = CHANNEL1RTPDESTPORT_START + (numb * 8 + 2);
	clientparams.channel1.rtp_ownssrc = CHANNEL1RTPSSRC_START + numb * 2;

	clientparams.channel2.rtp_portbase = CHANNEL2RTPBASEPORT_START + numb * 8 + 4;
	clientparams.channel2.rtp_destport = CHANNEL2RTPDESTPORT_START + numb * 8 + 6;
	clientparams.channel2.rtp_ownssrc = CHANNEL2RTPSSRC_START + numb * 2 + 1;

	return true;

}
int MyServer::RecoveryClientObjThread(void* p)
{
	MyServer *arg = (MyServer*)p;
	int return_value = 0;
	if (arg != NULL)
	{
		return_value = arg->RecoveryClientObjThreadFunc();
	}
	return return_value;
}
int MyServer::RecoveryClientObjThreadFunc()
{
	int return_value = 0;
	int len = 0;
	char temp[512];
	memset(temp, 0x00, 512);
	ClientParams_t clientparams_temp;
	std::map<ClientParams_t, ClientObj *> ::iterator it;
	int ret = 0;

	while ((ret = clientqueue.TakeFromQueue(temp, (int&)len, 20)) >= 0)
	{
		if (set_thread_exit_flag)break;
		if (ret == SYN_OBJECT_o)
		{
			memcpy(&clientparams_temp, temp, sizeof(ClientParams_t));
			clientmap_locker->Lock();

			it = clientmap.find(clientparams_temp);
			if (it != clientmap.end())
			{
				delete it->second;
				it->second = NULL;
				it = clientmap.erase(it);
				std::cout << "#clientmap.erase a client# \n" << std::endl;
			}
			else
			{
				std::cout << "#clientmap no find# \n" << std::endl;
			}
			memset(temp, 0x00, 512);
			clientmap_locker->Unlock();

			if (clientmap.size() < MAX_CLIENT_COUNT)
			{
				if (!listenIsopen)
				{
					InitSocket();//���´򿪼���socket
					CreateListenThread();//���������������������
					listenIsopen = true;
				}

			}
		}
		else
		{
			if (clientmap.size() >= MAX_CLIENT_COUNT)
			{
				if (listenIsopen)
				{
					std::cout << "# clientmap.size() >= MAX_CLIENT_COUNT, Close listen # \n" << std::endl;
					serversoc = INVALID_SOCKET;
					mytcp_server->Close();//�رռ���socket
					listenIsopen = false;
				}
			}
			//timeout
		}
	}

	std::cout << "exit RecoveryClientThreadFunc: 0x" << hex << recovery_thread_p->GetPthreadID() << std::endl;
	return return_value;

}
void MyServer::NotifyRecoveryClienObj(ClientParams_t clientparams)
{
	if (pThis == NULL)exit(-1);
	pThis->NotifyDeleleClienObjFunc(clientparams);

}
void MyServer::NotifyDeleleClienObjFunc(ClientParams_t clientparams)
{

	clientqueue.PushToQueue(&clientparams, sizeof(ClientParams_t));

	std::cout << "#push clientqueue# \n" << std::endl;
}
bool MyServer::CloseSocket(HSocket sockfd)
{
	closesocket(sockfd);
	return true;

}
int MyServer::PhySocketSendData(HSocket Objsoc, char *buff, int send_len)
{
	int count = 0;
	transresult_t rt; 
	rt.nbytes = 0;
	rt.nresult = 0;

	//��������ʼ��select��Ҫ�Ĳ���(���������write)������Objsoc��ӵ�fd_set��
	fd_set writefds;
	struct timeval timeout;
	timeout.tv_sec = SELECT_TIMEOUT;
	timeout.tv_usec = 0;
	int return_value = 0;

	FD_ZERO(&writefds);
	FD_SET(Objsoc, &writefds);
	while((return_value = select(0, NULL, &writefds, NULL, &timeout)) == 0);//ע�⣺δ�����˳��߼�
	if (return_value < 0)
	{
		std::cout << "select Client fail\n" << std::endl;
		return_value = -1;
	}
	else
	{

		do
		{
			SocketSend(Objsoc, &buff[count], (send_len - count), rt);
			if ((rt.nbytes > 0))
			{
				//TRACE(("send length is %d\n"), rt.nbytes);
				count += rt.nbytes;
			}
			else if ((rt.nbytes == -1) && (rt.nresult == 1))
			{
				std::cout << "SocketSend Timeout\n" << std::endl;

			}
			else if ((rt.nbytes == -1) && (rt.nresult == -1))
			{
				std::cout << "Client close socket\n" << std::endl;
				count = -1;
				return count;
			}


		} while ((send_len - count) != 0);
	}

	count = 0;
	return count;


}
int MyServer::SendDataToTheThirdParty(HSocket fd, std::string buff)
{
	int return_value = 0;
	int send_len = 0;
	HSocket Objsoc = 0;
	//std::map<HSocket, struct sockaddr_in> ::iterator it;
	std::map<ClientParams_t, ClientObj *> ::iterator it;

	stringstream ss;
	ss<< buff.size();
	Objsoc = fd;

	phy_fragment_t phy_fragment;
	memset(phy_fragment.fragment_element, 0x00, sizeof(phy_fragment));

	phy_fragment.transport_protocol_fragment.head = PROTOCOL_HEAD;

	if (buff.size() < 1000)
	{
		phy_fragment.transport_protocol_fragment.payload_len[0] = '0';
		memcpy(&(phy_fragment.transport_protocol_fragment.payload_len[1]), ss.str().c_str(), PROTOCOL_PACKAGE_LENGTH_SIZE - 1);
		//sprintf_s���²�������
		//sprintf_s(&(phy_fragment.transport_protocol_fragment.payload_len[1]), 10,  "%d", buff.size());
	}
	else
	{
		memcpy(&(phy_fragment.transport_protocol_fragment.payload_len[0]), ss.str().c_str(), PROTOCOL_PACKAGE_LENGTH_SIZE);
		//sprintf_s(&(phy_fragment.transport_protocol_fragment.payload_len[0]), 10,  "%d", buff.size());

	}
	
	memcpy(phy_fragment.transport_protocol_fragment.json_payload, buff.c_str(), buff.size());
	send_len = strlen(phy_fragment.fragment_element);

	if (Objsoc != INVALID_SOCKET)
	{
		if (Objsoc == 0)
		{
			clientmap_locker->Lock();
			for (it = clientmap.begin(); it != clientmap.end(); ++it)
			{
				Objsoc = (it->first).socket_fd;
				return_value = PhySocketSendData(Objsoc, phy_fragment.fragment_element, send_len);
			}
			clientmap_locker->Unlock();

		}
		else
		{
			return_value = PhySocketSendData(Objsoc, phy_fragment.fragment_element, send_len);
		}

	}
	else
	{
		std::cout<<"socket is empty!!!\n"<<std::endl;
	}

	Objsoc = INVALID_SOCKET;
	return return_value;
}
void MyServer::ConnectReply(HSocket dst_fd, std::string status, std::string reason)
{
	
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["status"] = status;
	send_item["reason"] = reason;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Reply";
	send_root["name"] = "Connect";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	//SendBuf.clear();
	//send_root.clear();
	//send_arrayObj.clear();
	//send_item.clear();
	std::cout<<"Send ConnectReply\n"<<std::endl;

}
void MyServer::ConfigReply(HSocket dst_fd, Listening_Reply_Params_Channels_t channel_params)
{
	Json::Value send_root;
	Json::Value send_arrayObj1;
	Json::Value send_arrayObj2;
	Json::Value send_arrayObj3;
	Json::Value send_item1;
	Json::Value send_item2;
	Json::Value send_item3;
	Json::StyledWriter style_write;


	send_item1["status"] = channel_params.channel1.status;
	send_item1["reason"] = channel_params.channel1.reason;
	send_item1["RTPportbase"] = channel_params.channel1.RTPportbase;
	send_item1["RTPdestport"] = channel_params.channel1.RTPdestport;
	send_item1["value"] = channel_params.channel1.listening_group_id;
	send_arrayObj1.append(send_item1);

	send_item2["status"] = channel_params.channel2.status;
	send_item2["reason"] = channel_params.channel2.reason;
	send_item2["RTPportbase"] = channel_params.channel2.RTPportbase;
	send_item2["RTPdestport"] = channel_params.channel2.RTPdestport;
	send_item2["value"] = channel_params.channel2.listening_group_id;;
	send_arrayObj2.append(send_item2);

	send_item3["channel1"] = send_arrayObj1;
	send_item3["channel2"] = send_arrayObj2;
	send_arrayObj3.append(send_item3);


	send_root["identifier"] = CreateGuid();
	send_root["type"] = "Reply";
	send_root["name"] = "Listening";
	send_root["param"] = send_arrayObj3;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);

	std::cout<<"Send ConfigReply\n"<<std::endl;


}
void MyServer::QueryReply(HSocket dst_fd, int channel1_value, int channel2_value)
{
	Json::Value send_root;
	Json::Value send_arrayObj1;
	Json::Value send_arrayObj2;
	Json::Value send_item1;
	Json::Value send_item2;
	Json::StyledWriter style_write;


	send_item2["channel1"] = channel1_value;
	send_item2["channel2"] = channel2_value;
	send_arrayObj2.append(send_item2);
	send_item1["listening"] = send_arrayObj2;
	if ((channel1_value != 0) && (channel2_value != 0))
	{
		send_item1["status"] = "success";
		send_item1["reason"] = "";
	}
	else
	{
		send_item1["status"] = "fail";
		if (channel1_value ==0 && channel2_value ==0)
			send_item1["reason"] = "channel1,2 is unset";
		else if (channel1_value == 0 && channel2_value != 0)
			send_item1["reason"] = "channel1 is unset";
		else
			send_item1["reason"] = "channel2 is unset";
	}

	send_arrayObj1.append(send_item1);

	send_root["identifier"] = CreateGuid();
	send_root["type"] = "Reply";
	send_root["name"] = "Query";

	send_root["param"] = send_arrayObj1;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	std::cout<<"Send QueryReply\n"<<std::endl;

}
void MyServer::CallRequestReply(HSocket dst_fd, std::string status, std::string reason)
{
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["status"] = status;
	send_item["reason"] = reason;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Reply";
	send_root["name"] = "CallRequest";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	std::cout<<"Send CallRequestReply\n"<<std::endl;

}
void MyServer::CallReleaseReply(HSocket dst_fd, std::string status, std::string reason)
{
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["status"] = status;
	send_item["reason"] = reason;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Reply";
	send_root["name"] = "CallRelease";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	std::cout<<"Send CallReleaseReply\n"<<std::endl;

}
void MyServer::CallStartNotify(HSocket dst_fd, int src, int dst, std::string channel)
{
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["src"] = src;
	send_item["dst"] = dst;
	send_item["channel"] = channel;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Noitify";
	send_root["name"] = "CallStart";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);

	std::cout<<"Send CallStartNotify\n"<<std::endl;

}
void MyServer::CallEndNotify(HSocket dst_fd, int src, int dst, std::string channel)
{
	Json::Value send_root;
	Json::Value send_arrayObj;
	Json::Value send_item;
	Json::StyledWriter style_write;

	send_item["src"] = src;
	send_item["dst"] = dst;
	send_item["channel"] = channel;
	send_arrayObj.append(send_item);

	send_root["identifier"] = CreateGuid();//"2017010915420322";
	send_root["type"] = "Noitify";
	send_root["name"] = "CallEnd";

	send_root["param"] = send_arrayObj;

	send_root.toStyledString();//build json data

	std::string SendBuf = style_write.write(send_root);

	SendDataToTheThirdParty(dst_fd, SendBuf);
	std::cout<<"Send CallEndNotify\n"<<std::endl;

}
std::string MyServer::CreateGuid()
{
	std::string strGuid = "", strValue;
	srand((unsigned)clock()); /*������*/
	for (int i = 0; i < 32; i++)
	{
		float Num = (float)(rand() % 16);
		int nValue = (int)floor(Num);
		/*GOSPRINTF((char *)strValue.c_str(), 2, "%0x", nValue);*/
		sprintf((char *)strValue.c_str(),"%0x", nValue);
		strGuid.insert(i, strValue.c_str());
	}
	return strGuid;
	

}