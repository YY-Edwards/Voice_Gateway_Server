#include "stdafx.h"
#include "../lib/rpc/include/RpcClient.h"
#include "../lib/rpc/include/RpcServer.h"
#include "../lib/rpc/include/RpcJsonParser.h"
#include "../lib//AES/Aes.h"
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
	licenseStatus = 2;
	m_serialInformation.deviceType = 0;
	memset(m_serialInformation.expiration, 0, 14);
	m_serialInformation.isEver = false;
	memset(m_serialInformation.funcList, 0, 16);
	memset(m_serialInformation.licType, 0, 12);
	memset(m_serialInformation.operate, 0, 16);
	memset(m_serialInformation.pcMac, 0, 16);
	memset(m_serialInformation.radioMode, 0, 13);
	memset(m_serialInformation.radioSerial, 0, 16);
	memset(m_serialInformation.repeaterMode, 0, 13);
	memset(m_serialInformation.repeaterSerial, 0, 16);
	memset(m_serialInformation.res, 0, 16);
	memset(m_serialInformation.time, 0, 16);
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
	/*	ArgumentType args;
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
		}, nullptr, 60);*/
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
void CBroker::startMonitorClient(std::map<std::string, ACTION> clientActions)
{
	if (NULL == m_monitorClient)
	{
		m_monitorClient = new CRpcClient();

		for (auto action = clientActions.begin(); action != clientActions.end(); action++)
		{
			m_monitorClient->addActionHandler(action->first.c_str(), action->second);
		}

		m_monitorClient->start("tcp://127.0.0.1:9004");
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
		//sendWirelanConfig();
	}
}

void CBroker::sendWirelanConfig()
{
	//std::string strConnect = CSettings::instance()->getRequest("wlConnect", "wl", m_wirelanClient->getCallId(), CSettings::instance()->getValue("repeater"));
	//m_wirelanClient->send(strConnect.c_str(), strConnect.size());

	//std::string content = 
		//"{\"repeater\":" + CSettings::instance()->getValue("repeater") +
		//",\"mnis\":" + CSettings::instance()->getValue("mnis") +
		//",\"location\":" + CSettings::instance()->getValue("location") +
		//",\"locationIndoor\":" + CSettings::instance()->getValue("locationIndoor") + "}";
	//std::string strConnect = CSettings::instance()->getRequest("connect", "radio", m_radioClient->getCallId(), content);
	//m_radioClient->send(strConnect.c_str(), strConnect.size());
	//std::string strConnect = CSettings::instance()->getRequest("wlConnect", "wl", m_wirelanClient->getCallId(), content);
	//m_wirelanClient->send(strConnect.c_str(), strConnect.size());


	std::string repeater = CSettings::instance()->getValue("repeater");
	std::string mnis = CSettings::instance()->getValue("mnis");
	std::string location = CSettings::instance()->getValue("location");
	std::string locationIndoor = CSettings::instance()->getValue("locationIndoor");
	if (repeater != "")
	{
		repeater = "{\"repeater\":" + repeater + ",";
	}
	else
	{
		repeater = "\"repeater\":null ,";
	}
	if (mnis != "")
	{
		mnis = "\"mnis\":" + mnis + ",";
	}
	else
	{
		mnis = "\"mnis\":null ,";
	}
	if (location != "")
	{
		location = "\"location\":" + location + ",";
	}
	else
	{
		location = "\"location\":null ,";
	}
	if (locationIndoor != "")
	{
		locationIndoor = "\"locationIndoor\":" + locationIndoor + "}";
	}
	else
	{
		locationIndoor = "\"locationIndoor\":null }";
	}
	std::string content = repeater + mnis + location + locationIndoor;
	std::string strConnect = CSettings::instance()->getRequest("wlConnect", "wl", m_wirelanClient->getCallId(), content);
	m_wirelanClient->send(strConnect.c_str(), strConnect.size());

}
void CBroker::sendRadioConfig()
{
	std::string radio = CSettings::instance()->getValue("radio");
	std::string mnis = CSettings::instance()->getValue("mnis");
	std::string location = CSettings::instance()->getValue("location");
	std::string locationIndoor = CSettings::instance()->getValue("locationIndoor");
	if (radio != "")
	{
		radio = "{\"radio\":" + radio + ",";
	}
	else
	{
		radio = "\"radio\":null ,";
	}
	if (mnis != "")
	{
		mnis = "\"mnis\":" + mnis + ",";
	}
	else
	{
		mnis = "\"mnis\":null ,";
	}
	if (location != "")
	{
		location = "\"location\":" + location + ",";
	}
	else
	{
		location = "\"location\":null ,";
	}
	if (locationIndoor != "")
	{
		locationIndoor = "\"locationIndoor\":" + locationIndoor + "}";
	}
	else
	{
		locationIndoor = "\"locationIndoor\":null }";
	}
	std::string content1 = "{\"radio\":" + CSettings::instance()->getValue("radio") + ",\"mnis\":"+
		CSettings::instance()->getValue("mnis") +",\"location\":"+
		CSettings::instance()->getValue("location") +",\"locationIndoor\":"+
		CSettings::instance()->getValue("locationIndoor")+"}";
	std::string content = radio + mnis + location + locationIndoor;
	std::string strConnect = CSettings::instance()->getRequest("connect", "radio", m_radioClient->getCallId(), content);
	m_radioClient->send(strConnect.c_str(), strConnect.size());
}
void CBroker::sendSettingConfig()
{
	std::string strConnect = CSettings::instance()->getRequest("connect", "radio", m_monitorClient->getCallId(), CSettings::instance()->getValue("radio"));
	m_monitorClient->send(strConnect.c_str(), strConnect.size());
}
bool CBroker::getLic(std::string license)
{
	unsigned char key[16];
	unsigned char input[16];
	unsigned char output[16];
	SerialInformation lic = { 0 };
	memset(key, 0, 16);
	memset(input, 0, 16);
	memset(output, 0, 16);
	int lenth = 16;
	char temp[145];
	char tempLic[289];
	strcpy_s(tempLic, license.c_str());
	for (int j = 0; j < 145; j++)
	{
		const char * p = tempLic + j * 2;
		sscanf_s(p, "%02x", &temp[j]);
	}
	memcpy(key, "#^.Lic/.cd@JH.^#", lenth);
	Aes1 m_Aes1(16, key);
	for (int i = 0; i < 9; i++)
	{
		m_Aes1.InvCipher((unsigned char*)temp + i * 16, (unsigned char*)&lic + i * 16);
	}

	/*for (int i = 0; i < 9; i++)
	{
		memcpy(input, temp + i * 16, 16);
		m_Aes1.InvCipher(input, output);
		switch (i)
		{
		case 1:
			memcpy(lic.licType, output, 12);
			lic.deviceType = *((int*)output + 12);
			break;
		case 2:
			memcpy(lic.pcMac, output, 16);
			break;
		case 3:
			memcpy(lic.radioSerial, output, 16);
			break;
		case 4:
			memcpy(lic.repeaterSerial, output, 16);
			break;
		case 5:
			memcpy(lic.time, output, 16);
			break;
		case 6:
			lic.isEver = *((short *)(output));
			break;
		case 7:
			memcpy(lic.expiration, output + 2, 14);
			break;
		case 8:
			memcpy(lic.funcList, output, 16);
			break;
		case 9:
			memcpy(lic.res, output, 16);
			break;
		default:
			break;
		}

	}*/
	SerialInformation s = getSerialInformation();
	memcpy(lic.repeaterMode, s.repeaterMode,13);
	memcpy(lic.radioMode, s.radioMode, 13);

	setLicenseInformation(lic);
	//对比授权文件


	if (strcmp(s.licType, lic.licType)==0)
	if (s.deviceType == lic.deviceType)
	{
		std::string temp = lic.radioSerial;
		if (strcmp(s.radioSerial, lic.radioSerial) == 0 && temp.length() == 10)
		{
			if (lic.isEver == 1)      //IsEver:是否永久，1:永久，0：试用
			{
				licenseStatus = true;    // 授权成功
				return licenseStatus;
			}
			else if (lic.isEver == 0)
			{
				//试用版 ，对比时间，是否过期
				SYSTEMTIME sys;
				GetLocalTime(&sys);
				char tmp[64] = { NULL };
				//sprintf_s(tmp, "%4d%02d%02d", sys.wYear, sys.wMonth, sys.wDay);
				int year2, month2, day2, hour2, min2, sec2;
				sscanf_s(lic.expiration, "%4d%02d%02d ", &year2, &month2, &day2, &hour2, &min2, &sec2);
				int tm1 = sys.wYear * 10000 + sys.wMonth * 100 + sys.wDay;
				int tm2 = year2 * 10000 + month2 * 100 + day2;
				if (tm1 != tm2)
				{
					int a = (tm1>tm2) ? 0 : 1;//如果相等，大返回1，小返回0
					//CBroker::instance()->setLicenseStatus(a);
					
					return a;
				}
			
			}
		}
		else if (strcmp(s.repeaterSerial, lic.repeaterSerial)==0)
		{
			std::string temp1 = lic.repeaterSerial;
			if (temp1.length() == 10)
			{
				if (lic.isEver == 1)      //IsEver:是否永久，1:永久，0：试用
				{
					licenseStatus = true;    // 授权成功
					return licenseStatus;
				}
				else if (lic.isEver == 0)
				{
					//试用版 ，对比时间，是否过期
					SYSTEMTIME sys;
					GetLocalTime(&sys);
					char tmp[64] = { NULL };
					int year2, month2, day2, hour2, min2, sec2;
					sscanf_s(lic.expiration, "%4d%02d%02d ", &year2, &month2, &day2, &hour2, &min2, &sec2);
					int tm1 = sys.wYear * 10000 + sys.wMonth * 100 + sys.wDay;
					int tm2 = year2 * 10000 + month2 * 100 + day2;
					if (tm1 != tm2)
					{
						int a = (tm1>tm2) ? 0 : 1;//如果相等，大返回1，小返回0
						//CBroker::instance()->setLicenseStatus(a);
						return a;
					}


				}
			}
	
		}
		else
		{
			//licenseStatus = false;    授权失败
		}
	}
	return false;
}
void CBroker::setSerialInformation(SerialInformation serialInformation)
{
	m_serialInformation = serialInformation;
}
SerialInformation CBroker::getSerialInformation()
{
	return m_serialInformation;
}
void CBroker::setLicenseInformation(SerialInformation serialInformation)
{
	m_licenseInformation = serialInformation;
}
SerialInformation CBroker::getLicenseInformation()
{
	return m_licenseInformation;
}
void CBroker::setLicenseStatus(int status)
{
	licenseStatus = status;
}
int CBroker::getLicenseStatus()
{
	return licenseStatus;
}
void CBroker::sendLoactionIndoorConfig()
{
	std::string strConnect = CSettings::instance()->getRequest("locationIndoor", "radio", m_radioClient->getCallId(), CSettings::instance()->getValue("locationIndoor"));
	m_radioClient->send(strConnect.c_str(), strConnect.size());
}
//void CBroker::sendLoactionIndoorConfigToWl()
//{
	//std::string strConnect = CSettings::instance()->getRequest("locationIndoor", "radio", m_wirelanClient->getCallId(), CSettings::instance()->getValue("locIndoor"));
	//m_wirelanClient->send(strConnect.c_str(), strConnect.size());
//}
