#include "stdafx.h"
#include "clientobj.h"


ClientObj::ClientObj(ClientParams_t params, MultiCallBackFuncs_t callbacks)
{
	InitProtocolData(params, callbacks);

	CreatProtocolParseThread();

	CreateProcessClientReqThread();


}

void ClientObj::InitProtocolData(ClientParams_t params, MultiCallBackFuncs_t callbacks)
{
	
	RequestCallBackFunc = callbacks.RequestCallBackFunc;//回调设置

	NotifyDeleteCallBackFunc = callbacks.ExitNotifyCallBackFunc;//回调设置

	channel1RTP = new MyRTP;
	channel1RTP->RtpParamsInit(params.channel1.rtp_portbase, params.channel1.rtp_destport, params.channel1.rtp_ownssrc);
	channel1RTP->SetCallBackFunc(callbacks.channel1.RecvVoiceDataFunc);

	channel2RTP = new MyRTP;
	channel2RTP->RtpParamsInit(params.channel2.rtp_portbase, params.channel2.rtp_destport, params.channel2.rtp_ownssrc);
	channel2RTP->SetCallBackFunc(callbacks.channel2.RecvVoiceDataFunc);

	clientparams = params;


	memset(recvbuff, 0x00, BUFLENGTH);
	memset(&temp_option, 0x00, sizeof(StickDismantleOptions_t)); 
	
	set_thread_exit_flag = false;

	socketoption.lingertimeout = TIMEOUT_VALUE;
	socketoption.recvtimeout = TIMEOUT_VALUE;
	socketoption.sendtimeout = TIMEOUT_VALUE;
	
	process_client_thread_p = NULL;
	parse_thread_p = NULL;

	ClientPRO_State = PROTOCOL_UNCONNECTEDWAITINGSTATUS; 

	//inset states
	statemap.insert(std::pair<string, int>("Connect", CONNECT));
	statemap.insert(std::pair<string, int>("Listening", LISTENING));
	statemap.insert(std::pair<string, int>("Query", QUERY));
	statemap.insert(std::pair<string, int>("CallRequest", CALLREQUEST));
	statemap.insert(std::pair<string, int>("CallRelease", CALLRELEASE));
	statemap.insert(std::pair<string, int>("CallStart", CALLSTART));
	statemap.insert(std::pair<string, int>("CallEND", CALLEND));

	//init default protocol data
	thePROTOCOL_Ctrlr.clientfd = INVALID_SOCKET;
	//thePROTOCOL_Ctrlr.MASTER_State = CONNECT;
	thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.identifier = "";
	thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.type = "Request";
	thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name = "Connect";
	thePROTOCOL_Ctrlr.PROTOCOL_params.key = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.reason = "";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id = 0;


	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.reason = "timeout";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.listening_group_id = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.RTPportbase = params.channel1.rtp_portbase;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.RTPdestport = params.channel1.rtp_destport;

	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.reason = "timeout";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.status = "fail";
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.listening_group_id = 0;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.RTPportbase = params.channel2.rtp_portbase;
	thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.RTPdestport = params.channel2.rtp_destport;

	thePROTOCOL_Ctrlr.PROTOCOL_params.src = 9;
	thePROTOCOL_Ctrlr.PROTOCOL_params.dst = 65536;
	thePROTOCOL_Ctrlr.PROTOCOL_params.channel = "";

}

ClientObj::~ClientObj()
{
	if (clientparams.socket_fd != INVALID_SOCKET)
	{
		SocketClose(clientparams.socket_fd);
	}

	SetThreadExitFlag();//通知线程退出

	if (parse_thread_p != NULL)
	{
		delete parse_thread_p;
		parse_thread_p = NULL;

	}
	if (process_client_thread_p != NULL)
	{
		delete process_client_thread_p;
		process_client_thread_p = NULL;

	}


	jqueue.ClearQueue();
	statemap.clear();

	if (channel1RTP != NULL)
	{
		delete channel1RTP;
		channel1RTP = NULL;
	}
	if (channel2RTP != NULL)
	{
		delete channel2RTP;
		channel2RTP = NULL;
	}

	//if (ondata_locker != NULL)
	//{
	//	delete ondata_locker;
	//	ondata_locker = NULL;
	//}
	//if (deobj_locker != NULL)
	//{
	//	delete deobj_locker;
	//	deobj_locker = NULL;
	//}

	std::cout << "Destory: ClientObj \n" << std::endl;

}
void ClientObj::CreatProtocolParseThread()
{
	parse_thread_p = new MyCreateThread(ProtocolParseThread, this);
}

void ClientObj::CreateProcessClientReqThread()
{
	process_client_thread_p = new MyCreateThread(ProcessClientReqThread, this);
}
int ClientObj::ProtocolParseThread(void *p)
{
	ClientObj *arg = (ClientObj*)p;
	int return_value = 0;
	if (arg != NULL)
	{
		return_value = arg->ProtocolParseThreadFunc();
	}
	return return_value;


}

int ClientObj::ProtocolParseThreadFunc()
{
	Json::Value val;
	Json::Reader reader;
	char queue_data[1024];
	char json_data[512];
	int len = 0;
	int return_value = SYN_ABANDONED;
	HSocket client_fd = INVALID_SOCKET;
	memset(queue_data, 0x00, 1024);
	memset(json_data, 0x00, 512);

	while ((return_value = jqueue.TakeFromQueue(queue_data, (int&)len, 18)) >= 0)//18ms
	{
		if (set_thread_exit_flag)break;
		if (SYN_OBJECT_o == return_value)
		{
			sscanf(queue_data, "%4D", &client_fd);
			memcpy(json_data, &queue_data[sizeof(HSocket)], sizeof(json_data));
			if (reader.parse(json_data, val))//Parse JSON buff
			{
				thePROTOCOL_Ctrlr.clientfd = client_fd;

				thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.identifier = val["identifier"].asString();
				thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.type = val["type"].asString();
				thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name = val["name"].asString();

				if (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "Connect")
					thePROTOCOL_Ctrlr.PROTOCOL_params.key = val["key"].asString();
				else if (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "Listening")
				{
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id = val["param"]["listening"]["channel1"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.listening_group_id = thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id;

					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id = val["param"]["listening"]["channel2"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.listening_group_id = thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id;

				}
				else if (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "Query")
				{
					//no param data
				}
				else if ((thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "CallRequest") || (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "CallRelease")
					|| (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "CallStart") || (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name == "CallEnd"))
				{
					thePROTOCOL_Ctrlr.PROTOCOL_params.src = val["param"]["src"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.dst = val["param"]["dst"].asInt();
					thePROTOCOL_Ctrlr.PROTOCOL_params.channel = val["param"]["channel"].asString();

				}
				else
				{
					//other data

				}

				if (thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.type == "Request")
					DataProcessFunc();//process thePROTOCOL_Ctrlr data
			}
			else
			{
				std::cout << "reader.parse err!!!\n" << std::endl;
			}
			val.clear();
			client_fd = INVALID_SOCKET;
			memset(queue_data, 0x00, 1024);
			memset(json_data, 0x00, 512);
		}
		else//timeout
		{
			//Sleep(200);//200ms
		}

	}

	//TRACE((" exit ProtocolParseThreadFunc : 0x%x\n"), GetCurrentThreadId());
	std::cout << "exit ProtocolParseThreadFunc : 0x" << hex << GetCurrentThreadId()<< std::endl;
	return return_value;
}
void ClientObj::DataProcessFunc()
{

	switch (statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second)
	{
	case CONNECT:
		std::cout << "the ThirdParty Request Connect\n" << std::endl;
		break;

	case LISTENING:
		std::cout << "the ThirdParty Request Listening\n" << std::endl;
		break;

	case QUERY:
		std::cout << "the ThirdParty Request Query\n" << std::endl;
		break;

	case CALLREQUEST:
		std::cout << "the ThirdParty Request Call Start\n" << std::endl;
		break;
	case CALLRELEASE:
		std::cout << "the ThirdParty Request Call Release\n" << std::endl;
		break;
	case CALLSTART:
	case CALLEND:
		std::cout << "no support cmd\n" << std::endl;
		break;
	default:
		break;
	}

	ProCallBackFunc(ClientPRO_State);

	switch (ClientPRO_State)
	{
		case PROTOCOL_UNCONNECTEDWAITINGSTATUS:

				if ((statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second) == CONNECT)
				{
					ClientPRO_State = PROTOCOL_UNCONNECTEDWAITINGSETLISTENING;
				}

				break;
		case PROTOCOL_UNCONNECTEDWAITINGSETLISTENING:

				if ((statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second) == LISTENING)
				{
					ClientPRO_State = PROTOCOL_CONNECTED;		
				}

				break;
		case PROTOCOL_CONNECTED:
						
				if ((statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second) == DISCONNECT)
				{
					ClientPRO_State = PROTOCOL_UNCONNECTEDWAITINGSTATUS;
				}
				break;

			default:
				break;
	}

	//if (RequestCallBackFunc != NULL)
	//{
	//	ResponeData r = { 
	//		ClientPRO_State,
	//		thePROTOCOL_Ctrlr.clientfd, 
	//		thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.identifier,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.key,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.channel,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.RTPportbase,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.RTPdestport,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.RTPportbase,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.RTPdestport,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.src,
	//		thePROTOCOL_Ctrlr.PROTOCOL_params.dst,
	//		"", 
	//		"" 
	//	};
	//	RequestCallBackFunc(statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second, r);
	//	//onData(RequestCallBackFunc, statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second, r);//callback (*func)
	//}


}
void ClientObj::ProCallBackFunc(ClientPRO_States_t State)
{
	if (RequestCallBackFunc != NULL)
	{
		ResponeData r = {
			State,
			thePROTOCOL_Ctrlr.clientfd,
			thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.identifier,
			thePROTOCOL_Ctrlr.PROTOCOL_params.key,
			thePROTOCOL_Ctrlr.PROTOCOL_params.channel,
			thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel1_group_id,
			thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.RTPportbase,
			thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel1.RTPdestport,
			thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Channels_Group.channel2_group_id,
			thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.RTPportbase,
			thePROTOCOL_Ctrlr.PROTOCOL_params.Listening_Params_Channels_Params.channel2.RTPdestport,
			thePROTOCOL_Ctrlr.PROTOCOL_params.src,
			thePROTOCOL_Ctrlr.PROTOCOL_params.dst,
			"",
			""
		};
		RequestCallBackFunc(statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second, r);
		//onData(RequestCallBackFunc, statemap.find(thePROTOCOL_Ctrlr.PROTOCOL_Fixed_Header.name)->second, r);//callback (*func)
	}

}
void ClientObj::onData(void(*func)(int, ResponeData), int command, ResponeData data)
{
	//ondata_locker->Lock();
	try
	{
		func(command, data);
	}
	catch (double)
	{
		std::cout << "onData func error...\n" << std::endl;

	}
	//ondata_locker->Unlock();

}
void ClientObj::deOBJ(void(*func)(ClientParams_t), ClientParams_t clientsocket)
{
	//deobj_locker->Lock();
	try
	{
		func(clientsocket);
	}
	catch (double)
	{
		std::cout << "deOBJ func error...\n" << std::endl;

	}
	//deobj_locker->Unlock();
}


int ClientObj::ProcessClientReqThread(void *p)
{
	ClientObj *arg = (ClientObj*)p;
	int return_value = 0;
	if (arg != NULL)
	{
		return_value = arg->ProcessClientReqThreadFunc();
	}
	std::cout << "Notifying the upper App and exit ProcessClientReqThreadFunc : 0x" << hex << GetCurrentThreadId() << std::endl;

	arg->deOBJ(arg->NotifyDeleteCallBackFunc, arg->clientparams);//通知上层应用删除clientObj对象

	return return_value;
}
int ClientObj::ProcessClientReqThreadFunc()
{
	
	int return_value = 0;
	//创建并初始化select需要的参数(这里仅监视read)，并把sock添加到fd_set中
	fd_set readfds;
#if WIN32

	struct timeval timeout;
	timeout.tv_usec = 0;

#else
	struct timespec timeout;
	timeout.tv_nsec = 0;

#endif

	timeout.tv_sec = SELECT_TIMEOUT;
	//设置clientfd超时
	//SocketTimeOut(clientparams.socket_fd, socketoption.recvtimeout, socketoption.sendtimeout, socketoption.lingertimeout);

	//设置socket_fd为non-blocking
	SocketBlock(clientparams.socket_fd, 0);

	std::cout << "Connected\n" << std::endl;
	while (!set_thread_exit_flag)
	{
		FD_ZERO(&readfds);
		FD_SET(clientparams.socket_fd, &readfds);
#if WIN32
		return_value = select(0, &readfds, NULL, NULL, &timeout);//注意linux/windows下次函数的功能有巨大差异，不同平台运行时，请注意：如linux下timeout需要重复初始化，函数返回后重置为0。
#else
		return_value = pselect(0, &readfds, NULL, NULL, &timeout, NULL);
#endif
		if (return_value < 0)
		{
			std::cout << "select Client fail\n" << std::endl;
			return_value = -1;
			break;
		}
		else if (return_value == 0)
		{
			//std::cout << "select Client timeout\n" << std::endl;
			continue;//timeout
		}
		else
		{

			SocketRecv(clientparams.socket_fd, &recvbuff[0], 512, rt);
			if (rt.nbytes > 0)
			{
				if ((recvbuff[0] == 'Q') && rt.nbytes == 1)//测试用
				{
					return_value = 0;
					break;
				}

				return_value = StickDismantleProtocol(clientparams.socket_fd, &recvbuff[0], rt.nbytes, temp_option);
				memset(&recvbuff[0], 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];

			}
			else if ((rt.nbytes == -1) && (rt.nresult == 1))
			{
				std::cout << "SocketRecv Timeout\n" << std::endl;
			}
			else if ((rt.nresult == -1))
			{
				std::cout << "Client close socket\n" << std::endl;
				return_value = -1;
				break;
			}
			else
			{
			}
		}

	}

	return return_value;

}
int ClientObj::StickDismantleProtocol(HSocket clientfd, char *ptr, int ptr_len, StickDismantleOptions_t &option)
{
	int return_value = 0;
	int32_t recv_length = ptr_len;
	std::string len_str;
	len_str.clear();

	if (!ptr || ptr_len>BUFLENGTH) return -1;
	memcpy(&option.data[option.count], ptr, ptr_len);

Start:

	if ((option.data[0] == PROTOCOL_HEAD) && (recv_length >= 5) && (option.bytes_remained == 0))//protocol start
	{
		memcpy((void*)len_str.c_str(), &option.data[1], PROTOCOL_PACKAGE_LENGTH_SIZE);
		sscanf(len_str.c_str(), "%D", &option.pro_length);//string->int

		option.bytes_remained = option.pro_length - (recv_length - 5);

		if (option.bytes_remained == 0)
		{
			std::cout << "recv_okay\n" << std::endl;
			PushRecvBuffToQueue(clientfd, &option.data[5], option.pro_length);
			//clear temp
			option.pro_length = 0;
			option.count = 0;
			memset(option.data, 0, BUFLENGTH);//clear data;
		}
		else if (option.bytes_remained > 0)//need  to stick the buff
		{
			std::cout << "need to stick the buff\n" << std::endl;
			option.count += recv_length;

		}
		else//recv_length >(pro_length+5) (bytes_remained < 0)//need  to dismantle the buff
		{
			std::cout << "need  to dismantle the buff\n" << std::endl;
			PushRecvBuffToQueue(clientfd, &option.data[5], option.pro_length);

			memcpy(&option.data[0], &option.data[option.pro_length + 5], recv_length - 5 - option.pro_length);
			memset(&option.data[recv_length - option.pro_length - 5], 0x00, BUFLENGTH - (recv_length - option.pro_length - 5));


			recv_length = recv_length - 5 - option.pro_length;
			option.bytes_remained = 0;
			option.count = 0;
			goto Start;

		}

	}
	else if (option.bytes_remained >0)
	{
		if (option.bytes_remained == recv_length)
		{
			std::cout << "recv_okay\n" << std::endl;
			PushRecvBuffToQueue(clientfd, &option.data[5], option.pro_length);
			//clear temp
			option.pro_length = 0;
			option.count = 0;
			option.bytes_remained = 0;
			memset(option.data, 0, BUFLENGTH);//clear recvbuf[BUFLENGTH];
		}
		else if (option.bytes_remained > recv_length)
		{
			std::cout << "need to stick the buff-2\n" << std::endl;
			option.bytes_remained -= recv_length;
			option.count += recv_length;
			//continue;//wait the 
		}
		else//bytes_remained < recv_length
		{
			if ((option.data[option.pro_length + PROTOCOL_PACKAGE_LENGTH_SIZE] == '}'))
			{
				std::cout << "need  to dismantle the buff-2\n" << std::endl;
				PushRecvBuffToQueue(clientfd, &option.data[5], option.pro_length);

				memcpy(&option.data[0], &option.data[option.pro_length + 5], (option.count + recv_length - option.pro_length - 5));
				memset(&option.data[option.count + recv_length - option.pro_length - 5], 0x00, BUFLENGTH - (option.count + recv_length - option.pro_length - 5));

				recv_length = option.count + recv_length - option.pro_length - 5;
				option.bytes_remained = 0;
				option.count = 0;
				//goto Start;
			}
			else
			{
				std::cout << "packet loss and wait to stick buff-3\n" << std::endl;
				uint32_t i = 0;
				int index = 0;
				for (i = (option.count); i < (option.count + recv_length); i++)//遍历数据包并找到下一包的头
				{
					if (option.data[i] == PROTOCOL_HEAD)
					{
						index = i;
						break;
					}
				}
				if (i <(option.count + recv_length))//找到
				{
					memcpy(&option.data[0], &option.data[index], recv_length + option.count - index);
					memset(&option.data[recv_length + option.count - index], 0x00, BUFLENGTH - (recv_length + option.count - index));
					recv_length = recv_length + option.count - index;
					option.bytes_remained = 0;
					option.count = 0;

				}
				else//没找到
				{
					option.bytes_remained = -1;
				}
			}
			goto Start;

		}

	}
	else if ((recv_length < 5) && (option.bytes_remained == 0))
	{
		///if (recv_length< 5)
		{
			option.count += recv_length;
		}
	}
	else////bytes_remained < 0
	{
		option.pro_length = 0;
		option.count = 0;
		option.bytes_remained = 0;
		memset(option.data, 0, BUFLENGTH);
		SocketClearRecvBuffer(clientfd);
		std::cout << "Recv err data and clear temp!!!\n" << std::endl;
	}


	return return_value;

}
int ClientObj::PushRecvBuffToQueue(HSocket clientfd, char *buff, int buff_len)
{
	if ((buff_len > 512) || (clientfd == INVALID_SOCKET))return -1;
	RTPTime delay(0.020);//20ms
	bool ret = false;
	char data[600];
	memset(data, 0x00, sizeof(data));
	/*GOSPRINTF(data, sizeof(HSocket)+1, "%d", clientfd);*/
	sprintf(data, "%d", clientfd);
	memcpy(&data[sizeof(HSocket)], buff, buff_len);
	//do
	//{
		ret = jqueue.PushToQueue(data, buff_len + sizeof(HSocket));//push to fifo-buff
		if (ret == false)
		{
			std::cout << "queue full!!!\n" << std::endl;
			RTPTime::Wait(delay);
		}

	//} while (!ret);

	return 0;

}
