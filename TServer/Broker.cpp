#include "stdafx.h"
#include "../lib/rpc/include/RpcClient.h"
#include "../lib/rpc/include/RpcServer.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "Broker.h"
#include "Settings.h"

std::auto_ptr<CBroker> CBroker::m_instance;

CBroker::CBroker()
	: m_wirelanClient(NULL)
	, m_radioClient(NULL)
	, m_rpcServer(NULL)
	, m_logClient(NULL)
	, callId(1)
{

}


CBroker::~CBroker()
{
	if (m_radioClient)
	{
		m_radioClient->stop();
		delete m_radioClient;
	}

	if (m_logClient)
	{
		m_logClient->stop();
		delete m_logClient;
	}

	if (m_rpcServer)
	{
		m_rpcServer->stop();
		delete m_rpcServer;
	}
}


void CBroker::startRpcServer(std::map<std::string, ACTION> serverActions)
{
	if (NULL == m_rpcServer)
	{
		m_rpcServer = new CRpcServer();

		for (auto action = serverActions.begin(); action!=serverActions.end(); action++)
		{
			m_rpcServer->addActionHandler(action->first.c_str(), action->second);
		}

		m_rpcServer->start(9000, CRpcServer::TCP);
	}
}

void CBroker::startRadioClient(std::map<std::string, ACTION> clientActions)
{
	if (NULL == m_radioClient)
	{
		m_radioClient = new CRpcClient();

		for (auto action = clientActions.begin(); action != clientActions.end(); action++)
		{
			m_radioClient->addActionHandler(action->first.c_str(), action->second);
		}

		m_radioClient->start("tcp://127.0.0.1:9001");

		// send radio hardware connect command
		ArgumentType args;
		FieldValue radioP(FieldValue::TObject);
		radioP.setKeyVal("Ip", FieldValue("192.168.10.2"));
		radioP.setKeyVal("Port", FieldValue(0));

		FieldValue mnisP(FieldValue::TObject);
		mnisP.setKeyVal("Ip", FieldValue("192.168.11.2"));
		mnisP.setKeyVal("Port", FieldValue(0));

		FieldValue gpsP(FieldValue::TObject);
		gpsP.setKeyVal("Ip", FieldValue("192.168.12.2"));
		gpsP.setKeyVal("Port", FieldValue(0));

		args["Radio"] = radioP;
		args["Mnis"] = mnisP;
		args["Gps"] = gpsP;

		uint64_t callId = m_radioClient->getCallId();
		std::string connectCommand = CRpcJsonParser::buildCall("connect", callId, args);
		m_radioClient->sendRequest(connectCommand.c_str(), callId, NULL, [](const char* pResponse, void* data){
			printf("recevied response:%s\r\n",pResponse);
		}, nullptr, 60);
	}
}
void CBroker::startLogClient()
{
	if (NULL == m_logClient)
	{
		m_logClient = new CRpcClient();
		m_logClient->start("tcp://127.0.0.1:9003");
	}
}

void CBroker::startWireLanClient(std::map<std::string, ACTION> clientActions)
{

	/*wire lan*/
	if (NULL == m_wirelanClient)
	{
		m_wirelanClient = new CRpcClient();

		for (auto action = clientActions.begin(); action != clientActions.end(); action++)
		{
			m_wirelanClient->addActionHandler(action->first.c_str(), action->second);
		}

		m_wirelanClient->start("tcp://127.0.0.1:9002");

		// send repeater hardware connect command by fixed parameter
		//ArgumentType args;
		//args["Type"] = "CPC";
		//FieldValue Master(FieldValue::TObject);
		//Master.setKeyVal("Ip", FieldValue("192.168.2.121"));
		//Master.setKeyVal("Port", FieldValue(50000));
		//args["Master"] = Master;
		//args["DefaultGroupId"] = 9;
		//args["DefaultChannel"] = 1;
		//args["MinHungTime"] = 4;
		//args["MaxSiteAliveTime"] = 59;
		//args["MaxPeerAliveTime"] = 59;
		//args["LocalPeerId"] = 120;
		//args["LocalRadioId"] = 5;
		//FieldValue Dongle(FieldValue::TObject);
		//Dongle.setKeyVal("Com", FieldValue(7));
		//args["Dongle"] = Dongle;

		//uint64_t callId = m_wirelanClient->getCallId();
		//std::string connectCommand = CRpcJsonParser::buildCall("wlConnect", callId, args);
		//m_wirelanClient->sendRequest(connectCommand.c_str(), callId, NULL, [](const char* pResponse, void* data){
		//	printf("recevied response:%s\r\n", pResponse);
		//}, nullptr, 60);

		// send repeater hardware connect command by setting file*/
		std::string strConnect = CSettings::instance()->getRequest("wlConnect", "wl", m_wirelanClient->getCallId(), CSettings::instance()->getValue("repeater"));
		m_wirelanClient->send(strConnect.c_str(), strConnect.size());
	}
}
