#include "stdafx.h"
#include "extern.h"
DispatchOperate::DispatchOperate()
{
	mnisIP = "";
	isUdpConnect = false;
	isTcpConnect = false;
	pDs = new CDataScheduling();
	pTs = new CTcpScheduling();
}

DispatchOperate::~DispatchOperate()
{
	if (pDs)
	{
		delete pDs;
	}
	if (pTs)
	{
		delete pTs;
	}
}

int DispatchOperate::getLic(const char* licPath)
{
	

	//char* strmac;
	//strmac = pXnlConnection->readmac;
	//CString  filename(licPath);
	////char buffer[256];
	//unsigned char temp[48];
	//memset(temp, 0, 48);
	////GetCurrentDirectory(256, (LPWSTR)buffer);
	////filename.Format(_T("%s"), licPath);
	////filename += "\\serial.lic";
	//CStdioFile file;
	//CString strText;
	//CString szLine;
	//if (file.Open(filename, CFile::modeRead))
	//{

	//	file.Read(temp, 48);
	//	file.Close();
	//}


	////解密
	//unsigned char key[16];
	//unsigned char input[16];
	//unsigned char output[16];

	//char serial[11];                          //Master  Serial
	//unsigned char date[11];
	//unsigned char peerNum[5];
	//memset(key, 0, 16);
	//memset(input, 0, 16);
	//memset(output, 0, 16);

	//memset(serial, 0, 11);
	//memset(date, 0, 11);
	//memset(peerNum, 0, 5);
	//memcpy(key, "ji1hua!@#567ADef", 16);
	//Aes1 m_Aes1(16, key);
	//for (int i = 0; i <strText.GetLength(); i++)
	//{
	//	temp[i] = strText.GetAt(i);
	//}
	//for (int j = 0; j <= 2; j++)
	//{
	//	memcpy(input, temp + j * 16, 16);
	//	m_Aes1.InvCipher(input, output);
	//	if (j == 0)
	//	{
	//		memcpy(serial, output, 8);
	//	}
	//	if (j == 1)
	//	{
	//		memcpy(serial + 8, output, 2);           //    Master Serial   
	//		memcpy(date, output + 2, 6);
	//	}

	//	if (j == 2)
	//	{
	//		memcpy(date + 6, output, 4);
	//		memcpy(peerNum, output + 4, 4);
	//	}

	//}
	//CString cstr_serial, cstr_date, cstr_peer, strDate, cstr_serial1;

	//cstr_serial = (unsigned char *)serial;              //序列号
	//if (pXnlConnection == NULL)
	//{
	//	return 1;
	//}
	//cstr_serial1 = pXnlConnection->readmac;


	//cstr_date = (unsigned char *)date;                  //使用期限 
	//cstr_peer = (unsigned char *)peerNum;                //中继台数

	//int year = 0, month = 0, day = 0;
	//sscanf_s((char *)date, "%4d-%2d-%2d", &year, &month, &day);
	//year = (year > 2000) ? year - 2000 : 0;

	//if (memcmp(cstr_serial, cstr_serial1, 10))
	//{
	//	return 1;
	//}
	//else
	//{
	//	if (cstr_date == _T("0000-00-00"))
	//	{
	//		return  0;
	//	}
	//	else
	//	{
	//		SYSTEMTIME st;
	//		GetLocalTime(&st);
	//		strDate.Format(_T("%4d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);
	//		if (cstr_date.Compare(strDate) < 0)
	//		{
	//			return (year << 24) + (month << 16) + (day << 8) + 1;                             //超出授权时间
	//		}
	//		else
	//		{
	//			return (year << 24) + (month << 16) + (day << 8);
	//		}
	//	}

	//}
return 1;
}

void DispatchOperate::sendConnectStatusToClient()
{
	ArgumentType args;
	if (isUdpConnect && isTcpConnect)
	{
		args["getType"] = CONNECT_STATUS;
		args["info"] = FieldValue(DATA_SUCESS_DISPATCH_SUCESS);
		dis.send2Client("status", args);
	}
	else if (isUdpConnect && !isTcpConnect)
	{
		args["getType"] = CONNECT_STATUS;
		args["info"] = FieldValue(DATA_SUCESS_DISPATCH_FAILED);
		dis.send2Client("status", args);
	}
	else if (!isUdpConnect && isTcpConnect)
	{
		args["getType"] = CONNECT_STATUS;
		args["info"] = FieldValue(DATA_FAILED_DISPATCH_SUCESS);
		dis.send2Client("status", args);
	}
	else if (!isUdpConnect && !isTcpConnect)
	{
		args["getType"] = CONNECT_STATUS;
		args["info"] = FieldValue(DATA_FAILED_DISPATCH_FAILED);
		dis.send2Client("status", args);
	}
}
//void DispatchOperate::sendRadioStatusToClient(CRemotePeer* pRemote)
//{
//	if (pRemote != NULL)
//	{
//		map<string, status>::iterator it;
//		ArgumentType args;
//		args["getType"] = RADIO_STATUS;
//		FieldValue info(FieldValue::TArray);
//		for (it = radioStatus.begin(); it != radioStatus.end(); it++)
//		{
//			FieldValue element(FieldValue::TObject);
//			
//			element.setKeyVal("radioId", FieldValue(it->second.id));
//			bool isGps = false;
//			if (it->second.gpsQueryMode != 0)
//			{
//				isGps = true;
//			}
//			bool isArs = false;
//			if (it->second.status != 0)
//			{
//				isArs = true;
//			}
//			element.setKeyVal("IsInGps", FieldValue(isGps));
//			element.setKeyVal("IsOnline", FieldValue(isArs));
//
//			info.push(element);
//			//info.setKeyVal(it->first.c_str(),element);
//		}
//		
//		args["info"] = info;
//		std::string callJsonStr = CRpcJsonParser::buildCall("status", ++seq, args, "radio");
//		if (pRemotePeer != NULL)
//		{
//			pRemotePeer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
//		}
//	}
//}
void DispatchOperate::sendCallStatusToClient()
{

}


void DispatchOperate::addPeer(CRemotePeer* peer)
{
	TcpClient *client = new TcpClient();
	client->addr = ((TcpClient *)peer)->addr;
	client->s = ((TcpClient *)peer)->s;
	bool isHave = false;
	for (auto i = rmtPeerList.begin(); i != rmtPeerList.end(); i++)
	{
		TcpClient *p = *i;
		if (p->s == client->s)
		{
			isHave = true;
			break;
		}
	}
	if (!isHave)
	{
		rmtPeerList.push_back(client);
	}
	else
	{
		delete client;
		client = NULL;
	}
}
void DispatchOperate::delPeer(CRemotePeer* peer)
{
	TcpClient *client = new TcpClient();
	client->addr = ((TcpClient *)peer)->addr;
	client->s = ((TcpClient *)peer)->s;
	for (auto i = rmtPeerList.begin(); i != rmtPeerList.end(); i++)
	{
		TcpClient *p = *i;
		if (p->s == client->s)
		{
			delete (*i);
			rmtPeerList.erase(i);
			break;
		}
	}
	delete client;
	client = NULL;
}
void DispatchOperate::OnConnect(CRemotePeer* pRemotePeer)
{
	
	if (pRemotePeer)
	{

	

		dis.addPeer(pRemotePeer);
		std::string strRequest = CRpcJsonParser::buildCall("getRadioConfig", ++g_sn, ArgumentType(), "radio");
		pRemotePeer->sendResponse((const char *)strRequest.c_str(), strRequest.size());

		//发送序列号到tserver
		ArgumentType args;
		FieldValue fSerial(FieldValue::TString);
		fSerial.setString(serial.c_str());
		args["serial"] = fSerial;
		dis.send2Client("readSerial", args);
		//发送mode到tserver
		ArgumentType args1;
		FieldValue fSerial1(FieldValue::TString);
		fSerial1.setString(radiomode.c_str());
		args1["serial"] = fSerial;
		dis.send2Client("readSerial", args1);
		
	}
}

void DispatchOperate::OnDisConnect(CRemotePeer* pRemotePeer)
{
	if (pRemotePeer)
	{
		dis.delPeer(pRemotePeer);
		dis.disConnect();

	}
}
void DispatchOperate::OnData(  int call, Respone data)
{
	ArgumentType args;
	FieldValue Gps(FieldValue::TObject);
	switch (call)
	{
	case MNIS_CONNECT:
		try
		{
			if (SUCESS == data.connectStatus)
			{
				dis.isUdpConnect = true;
			}
			else
			{
				dis.isUdpConnect = false;
			}
			
		}
		catch (std::exception e)
		{

		}
		break;
	case CONNECT_STATUS:
		try
		{
			if (SUCESS == data.connectStatus)
			{
				dis.isUdpConnect = true;
			}
			else
			{
				dis.isUdpConnect = false;
			}
			//dis.sendConnectStatusToClient();
		}
		catch (std::exception e)
		{

		}
		break;
	case SEND_PRIVATE_MSG:
		args["type"] = FieldValue(PRIVATE);
		try
		{
			args["Source"] = FieldValue(NULL);
			args["Target"] = FieldValue(data.target);
			args["contents"] = FieldValue("");
			args["status"] = FieldValue(data.msgStatus); 
			args["type"] = FieldValue(data.msgType);
			dis.send2Client("messageStatus", args);
		}
		catch (std::exception e)
		{

		}
		break;
	case SEND_GROUP_MSG:
		args["type"] = FieldValue(GROUP);
		try
		{
			args["Source"] = FieldValue(NULL);
			args["Target"] = FieldValue(data.target);
			args["contents"] = FieldValue("");
			args["status"] = FieldValue(data.msgStatus);
			args["type"] = FieldValue(data.msgType);
			dis.send2Client("messageStatus", args);
		}
		catch (std::exception e)
		{

		}
		break;
	case RECV_MSG:
		args["Target"] = FieldValue(data.target);
		args["Source"] = FieldValue(data.source);
		args["Contents"] = FieldValue(data.msg.c_str());
		args["type"] = FieldValue(PRIVATE);
		dis.send2Client("message", args);
		break;
	case  GPS_IMME_COMM:	
	case GPS_TRIGG_COMM:	
	case GPS_IMME_CSBK:	
	case GPS_TRIGG_CSBK:	
	case GPS_IMME_CSBK_EGPS:
	case GPS_TRIGG_CSBK_EGPS:
	case STOP_QUERY_GPS:
		args["Target"] = FieldValue(data.target);
		args["Type"] = FieldValue(data.querymode);
		args["Cycle"] = FieldValue(data.cycle);
		args["Operate"] = FieldValue(data.operate);
		args["Status"] = FieldValue(data.gpsStatus);
		args["Operate"] = FieldValue(data.gpsType);
		dis.send2Client("sendGpsStatus", args);
		break;
	case RECV_GPS:
		
		Gps.setKeyVal("Lon", FieldValue(data.lon));
		Gps.setKeyVal("Lat", FieldValue(data.lat));
		Gps.setKeyVal("Valid", FieldValue(data.valid));
		Gps.setKeyVal("Speed", FieldValue(data.speed));
		//gps.setKeyVal("date", FieldValue(strTime.c_str()));
		//FieldValue result(FieldValue::TObject);
		//result.setKeyVal("Source", FieldValue(radioID));
		//result.setKeyVal("Gps",Gps);
		args["Target"] = FieldValue(data.target);
		args["Source"] = FieldValue(data.source);
		args["Altutude"] = FieldValue(data.altitude);
		args["Gps"] = Gps;
		dis.send2Client("sendGps", args);
		break;
	case RADIO_ARS:
		args["Target"] = FieldValue(data.source);
		if (data.arsStatus == SUCESS)
		{
			args["IsOnline"] = FieldValue("True");
		}
		else
		{
			args["IsOnline"] = FieldValue("False");
		}
		dis.send2Client("sendArs", args);
		break;
	case RADIO_STATUS:
		std::map<std::string,RadioStatus>::iterator it;
		args["getType"] = RADIO_STATUS;
		FieldValue info(FieldValue::TArray);
		for (it = data.rs.begin(); it != data.rs.end(); it++)
		{
			FieldValue element(FieldValue::TObject);
			element.setKeyVal("radioId", FieldValue(it->second.id));
			bool isGps = false;
			if (it->second.gpsQueryMode != 0)
			{
				isGps = true;
			}
			bool isArs = false;
			if (it->second.status != 0)
			{
				isArs = true;
			}
			element.setKeyVal("IsInGps", FieldValue(isGps));
			element.setKeyVal("IsOnline", FieldValue(isArs));
			info.push(element);
		}
		args["info"] = info;
		dis.send2Client("status", args);
		break;
	}
}
void DispatchOperate::OnTcpData(int call, TcpRespone data)
{
	ArgumentType args;
	switch (call)
	{
	case RADIO_CONNECT:
		try
		{
			if(SUCESS == data.result)
			{
				dis.isTcpConnect = true;
			}
			else
			{
				dis.isTcpConnect = false;
			}
			if ("" == dis.mnisIP && !dis.isTcpConnect)
			{
				dis.isUdpConnect = false;
			}
			else if ("" == dis.mnisIP && dis.isTcpConnect && !dis.isUdpConnect)
			{
				dis.isUdpConnect = true;
			}
			dis.sendConnectStatusToClient();
		}
		catch (std::exception e)
		{

		}
		break;
	case PRIVATE_CALL:
	case GROUP_CALL:
	case ALL_CALL:
		args["Status"] = FieldValue(data.result);
		args["Target"] = FieldValue(data.id);
		args["Operate"] = FieldValue(START);
		args["Type"] = FieldValue(data.callType);
		dis.send2Client("callStatus", args);
		break;
	case STOP_CALL:
		args["Status"] = FieldValue(data.result);
		args["Target"] = FieldValue(data.id);
		args["Operate"] = FieldValue(STOP);
		args["Type"] = FieldValue(data.callType);
		dis.send2Client("callStatus",args);
		break;
	case  REMOTE_CLOSE :
	case REMOTE_OPEN  :
	case CHECK_RADIO_ONLINE :
	case REMOTE_MONITOR  :
		args["Status"] = FieldValue(data.result);
		args["Target"] = FieldValue(data.id);
		args["Type"] = FieldValue(data.controlType);
		dis.send2Client("controlStatus", args);
		break;
	case RADIO_ARS:
		break;
	case RADIO_SERIAL:
		FieldValue fSerial (FieldValue::TString);
		fSerial.setString((data.radioSerial).c_str());
		args["serial"] = fSerial;
		dis.send2Client("readSerial", args);
		break;
	}

}
void DispatchOperate::send2Client( char* name, ArgumentType args)
{
	std::lock_guard<std::mutex> locker(m_locker);
	try
	{
		std::string callJsonStr = CRpcJsonParser::buildCall(name, ++g_sn, args, "radio");
		for (auto i = rmtPeerList.begin(); i != rmtPeerList.end(); i++)
		{
			TcpClient *peer = *i;
			if (peer != NULL)
			{

				peer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
			}
		}
	}
	catch (std::exception e)
	{

	}
	
	
}



void DispatchOperate::connect(const char * ip ,const char* mIp,const char* gpsIP )
{
	if (INADDR_NONE != inet_addr(mIp))
	{
		mnisIP = mIp;
		pDs->radioConnect(mIp);
		pTs->radioConnect(ip);
	}
	else
	{
		if (INADDR_NONE != inet_addr(gpsIP))
		{
			pDs->InitGPSOverturnSocket(inet_addr(gpsIP));
		}
		pDs->radioConnect(ip);
		pTs->radioConnect(ip);
	}
	
	
}
void DispatchOperate::call( int type,int op, int id)
{
	pTs->call(type, id, op);
}
void DispatchOperate::control( int type,  int id)
{
	pTs->control(type, id);
}

void DispatchOperate::setCallBack()
{
	pDs->setCallBackFunc(DispatchOperate::OnData);
	pTs->setCallBackFunc(DispatchOperate::OnTcpData);
}
bool DispatchOperate::getGps( int id, int querymode, double cycle)
{
	return pDs->radioGetGps(id, querymode, cycle);
}
bool DispatchOperate::stopGps( int id, int querymode)
{
	return pDs->radioStopGps(id, querymode);
}
bool DispatchOperate::sendMsg(std::string text, int id,  int opterateType)
{
	return pDs->radioSendMsg(text, id, opterateType);
}
void DispatchOperate::getStatus( int type)
{
	ArgumentType args;
	switch (type)
	{
	case RADIO_CONNECT:
		
		if (isUdpConnect && isTcpConnect)
		{
			args["getType"] = CONNECT_STATUS;
			args["info"] = FieldValue(DATA_SUCESS_DISPATCH_SUCESS);
			dis.send2Client("status", args);
		}
		else if (isUdpConnect && !isTcpConnect)
		{
			args["getType"] = CONNECT_STATUS;
			args["info"] = FieldValue(DATA_SUCESS_DISPATCH_FAILED);
			dis.send2Client("status", args);
		}
		else if (!isUdpConnect && isTcpConnect)
		{
			args["getType"] = CONNECT_STATUS;
			args["info"] = FieldValue(DATA_FAILED_DISPATCH_SUCESS);
			dis.send2Client("status", args);
		}
		else if (!isUdpConnect && !isTcpConnect)
		{
			args["getType"] = CONNECT_STATUS;
			args["info"] = FieldValue(DATA_FAILED_DISPATCH_FAILED);
			dis.send2Client("status", args);
		}
		break;
	case RADIO_STATUS:
		pDs->getRadioStatus(type);
		break;
	}
}
void DispatchOperate::disConnect()
{
	pDs->radioDisConnect();
	pTs->disConnect();
	isTcpConnect = false;
	isUdpConnect = false;
	
}
void DispatchOperate::OnRadioUsb(bool isConnected)
{
	dis.isTcpConnect = isConnected;
	dis.pTs->setUsb(isConnected);
}
