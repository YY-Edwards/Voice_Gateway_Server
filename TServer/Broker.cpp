#include "stdafx.h"
#include "../lib/rpc/include/RpcClient.h"
#include "../lib/rpc/include/RpcServer.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "Broker.h"

std::auto_ptr<CBroker> CBroker::m_instance;

CBroker::CBroker()
	: m_wirelanClient(NULL)
	, m_radioClient(NULL)
	, m_rpcServer(NULL)
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
		}, nullptr, 60);
	}
}