#include "stdafx.h"
#include "mymaster.h"

MyMasterServer *MyMasterServer::pThis = NULL;

MyMasterServer::MyMasterServer()
{
	pThis = this;
	fp = NULL;
	clients_callback_funcs.RequestCallBackFunc = MasterOnData;

	clients_callback_funcs.RTPcallbackfuncs[0].RecvVoiceDataFunc = SpecialGroup1Voice;
	clients_callback_funcs.RTPcallbackfuncs[1].RecvVoiceDataFunc = SpecialGroup2Voice;//OBJ1
	clients_callback_funcs.RTPcallbackfuncs[2].RecvVoiceDataFunc = SpecialGroup3Voice;
	clients_callback_funcs.RTPcallbackfuncs[3].RecvVoiceDataFunc = SpecialGroup4Voice;//OBJ2
	clients_callback_funcs.RTPcallbackfuncs[4].RecvVoiceDataFunc = SpecialGroup5Voice;
	clients_callback_funcs.RTPcallbackfuncs[5].RecvVoiceDataFunc = SpecialGroup6Voice;//OBJ3

	mastergate = new JProtocol;




}

MyMasterServer::~MyMasterServer()
{
	
	if (fp != NULL)
	{
		fclose(fp);
		fp = NULL;
	}
	if (mastergate != NULL)
	{
		delete mastergate;
		mastergate = NULL;
	}

	std::cout << "exit class: MyMasterServer \n" << endl;

}

void MyMasterServer::SpecialGroup1Voice(ResponeRTPData data)
{
	if (pThis == NULL)exit(-1);
	pThis->SpecialGroup1VoiceFunc(data);
}
void MyMasterServer::SpecialGroup1VoiceFunc(ResponeRTPData data)
{

	static int count = 0;
	count++;
	//fwrite(data.payloaddata, 1, data.payloaddatalength, fp);
	std::cout << "SpecialGroup - 1: Receive %d packet of voice data \n" << count << endl;
	//TRACE(("RTPChannel-1: Receive %d packet of voice data\n"), count);

}

void MyMasterServer::SpecialGroup2Voice(ResponeRTPData data)
{
	if (pThis == NULL)exit(-1);
	pThis->SpecialGroup2VoiceFunc(data);
}
void MyMasterServer::SpecialGroup2VoiceFunc(ResponeRTPData data)
{

	static int count = 0;
	count++;
	//fwrite(data.payloaddata, 1, data.payloaddatalength, fp);
	std::cout << "SpecialGroup - 2: Receive %d packet of voice data \n" << count << endl;
	//TRACE(("RTPChannel-1: Receive %d packet of voice data\n"), count);

}

void MyMasterServer::SpecialGroup3Voice(ResponeRTPData data)
{
	if (pThis == NULL)exit(-1);
	pThis->SpecialGroup3VoiceFunc(data);
}
void MyMasterServer::SpecialGroup3VoiceFunc(ResponeRTPData data)
{

	static int count = 0;
	count++;
	//fwrite(data.payloaddata, 1, data.payloaddatalength, fp);
	std::cout << "SpecialGroup - 3: Receive %d packet of voice data \n" << count << endl;
	//TRACE(("RTPChannel-1: Receive %d packet of voice data\n"), count);

}

void MyMasterServer::SpecialGroup4Voice(ResponeRTPData data)
{
	if (pThis == NULL)exit(-1);
	pThis->SpecialGroup4VoiceFunc(data);
}
void MyMasterServer::SpecialGroup4VoiceFunc(ResponeRTPData data)
{

	static int count = 0;
	count++;
	//fwrite(data.payloaddata, 1, data.payloaddatalength, fp);
	std::cout << "SpecialGroup - 4: Receive %d packet of voice data \n" << count << endl;
	//TRACE(("RTPChannel-1: Receive %d packet of voice data\n"), count);

}

void MyMasterServer::SpecialGroup5Voice(ResponeRTPData data)
{
	if (pThis == NULL)exit(-1);
	pThis->SpecialGroup5VoiceFunc(data);
}
void MyMasterServer::SpecialGroup5VoiceFunc(ResponeRTPData data)
{

	static int count = 0;
	count++;
	//fwrite(data.payloaddata, 1, data.payloaddatalength, fp);
	std::cout << "SpecialGroup - 5: Receive %d packet of voice data \n" << count << endl;
	//TRACE(("RTPChannel-1: Receive %d packet of voice data\n"), count);

}

void MyMasterServer::SpecialGroup6Voice(ResponeRTPData data)
{
	if (pThis == NULL)exit(-1);
	pThis->SpecialGroup6VoiceFunc(data);
}
void MyMasterServer::SpecialGroup6VoiceFunc(ResponeRTPData data)
{

	static int count = 0;
	count++;
	//fwrite(data.payloaddata, 1, data.payloaddatalength, fp);
	std::cout << "SpecialGroup - 6: Receive %d packet of voice data \n" << count << endl;
	//TRACE(("RTPChannel-1: Receive %d packet of voice data\n"), count);

}

void MyMasterServer::Start()
{

	mastergate->SetCallBackFunc(clients_callback_funcs);//设置回调函数

	mastergate->Start();//启动master

	int src = 0xffff;
	int dst = 0xabcd;
	std::string channel = "channel0";
	// TODO:  在此添加控件通知处理程序代码
	//if (mastergate != NULL)
	//{
	//	if (mastergate->IsMaterInitComplete())mastergate->CallStartNotify(0, src, dst, channel);

	//}

	//if (mastergate != NULL)
	//{
	//	if (mastergate->IsMaterInitComplete())mastergate->CallEndNotify(0, src, dst, channel);

	//}


}

void MyMasterServer::MasterOnData(int command, ResponeData data)
{

	if (pThis == NULL)exit(-1);
	pThis->MasterOnDataFunc(command, data);

}
void MyMasterServer::MasterOnDataFunc(int command, ResponeData data)
{
	std::string c_status = "success";
	Listening_Reply_Params_Channels_t channel_params;
	//char *sendVoiceBuff = {"abcdefghiojklmnopqrstuvwxyz"};
	char sendVoiceBuff[50] = "abcdefghiojklmnopqrstuvwxyz";

	PROTOCOL_Names pro_name = (PROTOCOL_Names)command;
	switch (pro_name)
	{
	case CONNECT:
		if (data.CPRO_State == PROTOCOL_UNCONNECTEDWAITINGSTATUS)
		{
			mastergate->ConnectReply(data.socket_fd, "success", "fine!");
			if (1)/*PTT Notice Enable*/
			{

			}
		}
		else
		{
			mastergate->ConnectReply(data.socket_fd, "fail", "Already connected ");
		}
		break;


	case LISTENING:

		channel_params.channel1.listening_group_id = data.channel1_group_id;
		channel_params.channel1.RTPportbase = data.channel1_RTPportbase;
		channel_params.channel1.RTPdestport = data.channel1_RTPdestport;

		channel_params.channel2.listening_group_id = data.channel2_group_id;
		channel_params.channel2.RTPportbase = data.channel2_RTPportbase;
		channel_params.channel2.RTPdestport = data.channel2_RTPdestport;


		if (data.CPRO_State != PROTOCOL_UNCONNECTEDWAITINGSTATUS)
		{

			if (data.channel1_group_id != 0)
			{
				//	fp = fopen("E:\\CloudMusic\\master_recv_voice.pcm", "wb+");
				//	if (fp == NULL){
				//		printf("fp  fail\n");
				//	}
				//	fseek(fp, 0, SEEK_SET);
				//}

				std::cout << "set channel1 is:" << dec << data.channel1_group_id << endl;
			}

			if (data.channel2_group_id != 0)
			{
				std::cout << "set channel2 is:" << dec << data.channel2_group_id << endl;
			}
			if (data.channel1_group_id == 0 && data.channel2_group_id == 0)
			{
				std::cout << "No channel is set\n" << endl;
			}

			channel_params.channel1.status = "success";
			channel_params.channel1.reason = "";

			channel_params.channel2.status = "success";
			channel_params.channel2.reason = "";

		}
		else
		{
			channel_params.channel1.status = "fail";
			channel_params.channel1.reason = "Unconnected";

			channel_params.channel2.status = "fail";
			channel_params.channel2.reason = "Unconnected";

		}

			mastergate->ConfigReply(data.socket_fd, channel_params);
		break;

	case QUERY:
		if (data.CPRO_State != PROTOCOL_UNCONNECTEDWAITINGSTATUS)
		{
			mastergate->QueryReply(data.socket_fd, data.channel1_group_id, data.channel2_group_id);
		}
		break;


	case CALLREQUEST:
		if (data.CPRO_State == PROTOCOL_CONNECTED)
		{

			mastergate->CallRequestReply(data.socket_fd, "success", "");
			if (c_status == "success")
			{
				std::cout << "Start to send RTP Voice\n" << endl;
				mastergate->CallStartNotify(data.socket_fd, data.src_id, data.dst_id, data.channel_id);

				/*if ((data.channel_id == "channel1") && channel1RTP != NULL)
					channel1RTP->SendRTPPayloadData(sendVoiceBuff, 24);

					if ((data.channel_id == "channel2") && channel2RTP != NULL)
					channel2RTP->SendRTPPayloadData(sendVoiceBuff, 24);*/

			}
		}
		break;

	case CALLRELEASE:
		if (data.CPRO_State == PROTOCOL_CONNECTED)
		{
			mastergate->CallReleaseReply(data.socket_fd, "success", "");
			if (c_status == "success")
			{
				std::cout << "Stop sending RTP Voice\n" << endl;
				mastergate->CallEndNotify(data.socket_fd, data.src_id, data.dst_id, data.channel_id);
			}
		}
		break;
	default:

		break;


	}

	std::cout << "MasterOnDataFunc finished\n" << endl;

}