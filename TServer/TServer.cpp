// TServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../lib/rpc/include/RpcServer.h"
#include "../lib/rpc/include/RpcClient.h"
#include "Settings.h"

#include "StartAction.h"
#include "SettingAction.h"



#include <shlobj.h> 
#include <Shlwapi.h>
#include "Tool.h"
#include "Http.h"

//
//#include"AllCallAction.h"
//#include"PrivateCallAction.h"
//#include "CancelGpsAction.h"
//#include "ConenctRadioAction.h"
//#include "GetGpsAction.h"
//#include"GetOverTurnGpsAction.h"
//#include"GroupCallAction.h"
//#include"RadioCheckAction.h"
//#include"RemotePowerOffAction.h"
//#include"RemotePowerOnAction.h"
//#include"WiretapAction.h"
//#include"SendGroupSmsAction.h"
//#include"SendSmsAction.h"
//#include"StopCallAction.h"

#include "StatusAction.h"
#include "CallAction.h"
#include "ControlAction.h"
#include "GpsAction.h"
#include "MsgAction.h"

#include "RegisterLicenseAction.h"
#include "QueryLicenseAction.h"
#include "QueryDeviceAction.h"

#include "RecvMessageAction.h"
#include "RecvMessageResultAction.h"
#include "RecvArsAction.h"
#include "RecvGpsAction.h"
#include "RecvConnectResultAction.h"
#include "RecvStatusAction.h"
#include "RecvCallStatusAction.h"
#include "RecvControlResultAction.h"
#include "RecvSendGpsStatus.h"
#include "RecvGetConfigAction.h"
#include "RecvSerialAction.h"
#include "RecvLocationIndoorAction.h"


#include "WireLanRecvAction.h"
#include "WireLanSendAction.h"

#include "MonitorServer.h"
#include "GetSettingAction.h"
#include "../lib/service\service.h"

#define SERVICE_CODE    TRUE
#define HTTP_PORT   8001
std::string getServerName()
{
	std::string serverName = "";
	std::string strConfig = CSettings::instance()->getValue("radio");
	//std::string strConfig = "{\"IsEnable\":true,\"IsOnlyRide\":true,\"Svr\":{\"Ip\":\"127.0.0.1\",\"Port\":9001},\"Ride\":{\"Ip\":\"192.168.10.2\",\"Port\":0},\"Mnis\":{\"Ip\":null,\"Port\":0},\"GpsC\":{\"Ip\":null,\"Port\":0},\"Ars\":{\"Ip\":null,\"Port\":4007},\"Message\":{\"Ip\":null,\"Port\":4005},\"Gps\":null,\"Xnl\":null}";
	Document doc;
	doc.Parse(strConfig.c_str());
	if (doc.IsObject() && doc.HasMember("IsEnable") && doc["IsEnable"].IsBool())
	{
		if (true == doc["IsEnable"].GetBool())
		{
			serverName = "Trbox.Dispatch";
		}
		else
		{
			serverName = "Trbox.Wirelan";
		}
		
	}
	return  serverName;
}
int _tmain(int argc, _TCHAR* argv[])
{
	system("chcp 65001");
	system("cls");

	//CSettings::instance()->getResponse("success", 1, 200, "", rapidjson::Value(NULL));
	//std::string str =  CSettings::instance()->getResponse("success", 1, 200, "", "");
	//CSettings::instance()->setValue("tst", rapidjson::Value(NULL));

	//std::string dsd = CRpcJsonParser::buildCall("dd", 3, ArgumentType(), "raio");
	//std::string ddddd = CRpcJsonParser::mergeCommand("c", 33, "d", "radio");

	//	CBroker::instance();
	//
	//	Sleep(1000);
	//	while (CBroker::instance()->getRadioClient() != nullptr){
	//
	//
	//
	//	std::string callJsonStr = CSettings::instance()->getRequest("connect", "radio", 0, CSettings::instance()->getValue("radio"));
	//
	//	std::string callCommand = CRpcJsonParser::mergeCommand("connect", 0, callJsonStr.c_str());
	//	int ret = CBroker::instance()->getRadioClient()->sendRequest(callCommand.c_str(),
	//		0,
	//		NULL,
	//		[&](const char* pResponse, void* data){
	//		//CRemotePeer* pCommandSender = (CRemotePeer*)data;
	//		//pCommandSender->sendResponse(pResponse, strlen(pResponse));
	//	}, nullptr);
	//
	//	if (-1 != ret)break;
	//	
	//	Sleep(50);
	//
	//}
	int createFileRlt = 0;
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);
	std::wstring appFolder = getAppdataPath() + _T("\\")+ ConmpanyName;
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\Trbox");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\3.0");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}
	appFolder = appFolder + _T("\\TServer");
	if (!PathFileExists(appFolder.c_str()))
	{
		createFileRlt = _wmkdir(appFolder.c_str());
	}

	std::wstring logFolder = appFolder + _T("\\log");
	if (!PathFileExists(logFolder.c_str()))
	{
		createFileRlt = _wmkdir(logFolder.c_str());
	}
	std::wstring tmpFolder = appFolder + _T("\\tmp");
	if (!PathFileExists(tmpFolder.c_str()))
	{
		createFileRlt = _wmkdir(tmpFolder.c_str());
	}
	std::wstring pathLogInfo = logFolder + _T("/info_");
	std::wstring pathLogError = logFolder + _T("/error_");
	std::wstring pathLogWarning = logFolder + _T("/warning_");

	//FLAGS_log_dir = "./";
	google::InitGoogleLogging("");
	google::SetLogDestination(google::GLOG_INFO, CTool::UnicodeToUTF8(pathLogInfo).c_str());
	google::SetLogDestination(google::GLOG_ERROR, CTool::UnicodeToUTF8(pathLogError).c_str());
	google::SetLogDestination(google::GLOG_WARNING, CTool::UnicodeToUTF8(pathLogWarning).c_str());
	google::SetLogFilenameExtension("log");

	std::map<std::string, ACTION> serverActions, clientActions, wlClientActions ,mclientActions ;
#if SERVICE_CODE
	CService::instance()->SetServiceNameAndDescription(_T("Trbox.TServer"), _T("Trbox TServer Server"));
	CService::instance()->SetServiceCode([&](){
		serverActions["start"] = startAction;

		serverActions["setBaseSetting"] = setBaseAction;
		serverActions["getBaseSetting"] = getBaseAction;
		serverActions["setRadioSetting"] = setRadioAction;
		serverActions["getRadioSetting"] = getRadioAction;
		serverActions["setRepeaterSetting"] = setRepeaterAction;
		serverActions["getRepeaterSetting"] = getRepeaterAction;
		serverActions["setMnisSetting"] = setMnisAction;
		serverActions["getMnisSetting"] = getMnisAction;
		serverActions["setLocationSetting"] = setLocationAction;
		serverActions["getLocationSetting"] = getLocationAction;
		serverActions["setLocationInDoorSetting"] = setLocationInDoorAction;
		serverActions["getLocationInDoorSetting"] = getLocationInDoorAction;

		serverActions["call"] = callAction;
		serverActions["control"] = controlAction;
		serverActions["location"] = gpsAction;
		serverActions["message"] = msgAction;
		serverActions["status"] = statusAction;
		serverActions["registerLicense"] = registerLicenseAction;
		serverActions["queryLicense"] = queryLicenseAction;
		serverActions["queryDevice"] = queryDeviceAction;

		/*wire lan recive*/
		serverActions["wlCall"] = wlCallActionHandler;
		serverActions["wlCallStatus"] = wlCallStatusActionHandler;
		serverActions["wlInfo"] = wlInfoActionHandler;
		serverActions["wlPlay"] = wlPlayActionHandler;

		clientActions["message"] = recvMsgAction;
		clientActions["messageStatus"] = recvMessageResultAction;
		clientActions["sendArs"] = recvArsAction;
		clientActions["sendGps"] = recvGpsAction;
		clientActions["connectStatus"] = recvConnetResultAction;
		clientActions["status"] = recvStatusAction;
		clientActions["callStatus"] = recvCallStatusAction;
		clientActions["controlStatus"] = recvControlStatusAction;
		clientActions["controlResult"] = recvControlResultAction;
		clientActions["locationStatus"] = recvSendGpsStatusAction;
		clientActions["getRadioConfig"] = recvGetConfigAction;
		clientActions["readSerial"] = readSerialAction;
		//clientActions["locationIndoor"] = recvLocationIndoorAction;
		clientActions["sendBeacons"] = recvLocationIndoorAction;
		/*wire lan send*/
		wlClientActions["wlCall"] = wlCallAction;
		wlClientActions["wlCallStatus"] = wlCallStatusAction;
		wlClientActions["wlInfo"] = wlInfoAction;
		wlClientActions["wlPlayStatus"] = wlPlayStatusAction;
		wlClientActions["wlGetConfig"] = wlGetConfigAction;
		wlClientActions["connectStatus"] = recvConnetResultAction;
		//wlClientActions["sendGpsStatus"] = recvSendGpsStatusAction;
		wlClientActions["locationStatus"] = recvSendGpsStatusAction;
		wlClientActions["sendGps"] = recvGpsAction;
		wlClientActions["messageStatus"] = recvMessageResultAction;
		wlClientActions["message"] = recvMsgAction;
		wlClientActions["status"] = recvStatusAction;
		wlClientActions["sendArs"] = recvArsAction;
		wlClientActions["wlReadSerial"] = wlReadSerialAction;
		wlClientActions["sendBeacons"] = recvLocationIndoorAction;
		mclientActions["getSettingConfig"] = GetSettingAction;
		CBroker::instance()->startLogClient();

		CBroker::instance()->startWireLanClient(wlClientActions);
		CBroker::instance()->startRpcServer(serverActions);
		CBroker::instance()->startRadioClient(clientActions);
		CBroker::instance()->startMonitorClient(mclientActions);
		CHttp::getInstance()->start(HTTP_PORT);
		/*CMonitorServer ms;
		std::string strName = getServerName();
		std::wstring wstr(strName.length(), L' ');
		std::copy(strName.begin(), strName.end(), wstr.begin());
		if (strName != "")
		{
			ms.startMonitor(wstr.c_str(), _T("Trbox.Log"));

		}*/
		/*ÊÍ·Å×ÊÔ´*/
		while (!CService::instance()->m_bServiceStopped)
		{
			Sleep(100);
		}
		CHttp::getInstance()->stop();
		
	});
	std::wstring strArg = argv[1];
	try{
		if (0 == strArg.compare(_T("install")))
		{
			CService::instance()->InstallService();
			//InstallService();
		}
		else if (0 == strArg.compare(_T("uninstall")))
		{
			CService::instance()->UninstallService();
			//LOG(INFO) << "UnInstall Service";
		}
		else if (0 == strArg.compare(_T("start")))
		{
			CService::instance()->StartWindowsService();
			//LOG(INFO) << "Start Service";
		}
		else if (0 == strArg.compare(_T("stop")))
		{
			CService::instance()->StopService();
			//LOG(INFO) << "Stop Service";
		}
		else if (0 == strArg.compare(_T("run")))
		{
			CService::instance()->RunService();
		}
	}
	catch (std::system_error syserr) {
		exit(1);
	}
	catch (std::runtime_error runerr) {
		exit(1);
	}
	catch (...) {
		exit(1);
	}

	wprintf(argv[1]);
#else

	

	serverActions["start"] = startAction;
	serverActions["setBaseSetting"] = setBaseAction;
	serverActions["getBaseSetting"] = getBaseAction;
	serverActions["setRadioSetting"] = setRadioAction;
	serverActions["getRadioSetting"] = getRadioAction;
	serverActions["setRepeaterSetting"] = setRepeaterAction;
	serverActions["getRepeaterSetting"] = getRepeaterAction;
	serverActions["setMnisSetting"] = setMnisAction;
	serverActions["getMnisSetting"] = getMnisAction;
	serverActions["setLocationSetting"] = setLocationAction;
	serverActions["getLocationSetting"] = getLocationAction;
	serverActions["setLocationInDoorSetting"] = setLocationInDoorAction;
	serverActions["getLocationInDoorSetting"] = getLocationInDoorAction;

	serverActions["call"] = callAction;
	serverActions["control"] = controlAction;
	serverActions["location"] = gpsAction;
	serverActions["message"] = msgAction;
	serverActions["status"] = statusAction;
	serverActions["registerLicense"] = registerLicenseAction;
	serverActions["queryLicense"] = queryLicenseAction;
	serverActions["queryDevice"] = queryDeviceAction;


	/*wire lan recive*/
	serverActions["wlCall"] = wlCallActionHandler;
	serverActions["wlCallStatus"] = wlCallStatusActionHandler;
	serverActions["wlInfo"] = wlInfoActionHandler;
	serverActions["wlPlay"] = wlPlayActionHandler;

	clientActions["message"] = recvMsgAction;
	clientActions["messageStatus"] = recvMessageResultAction;
	clientActions["sendArs"] = recvArsAction;
	clientActions["sendGps"] = recvGpsAction;
	clientActions["connectStatus"] = recvConnetResultAction;
	clientActions["status"] = recvStatusAction;
	clientActions["callStatus"] = recvCallStatusAction;
	clientActions["controlStatus"] = recvControlStatusAction;
	clientActions["controlResult"] = recvControlResultAction;
	clientActions["locationStatus"] = recvSendGpsStatusAction;
	clientActions["getRadioConfig"] = recvGetConfigAction;
	clientActions["readSerial"] = readSerialAction;
	clientActions["sendBeacons"] = recvLocationIndoorAction;
	/*wire lan send*/
	wlClientActions["wlCall"] = wlCallAction;
	wlClientActions["wlCallStatus"] = wlCallStatusAction;
	wlClientActions["wlInfo"] = wlInfoAction;
	wlClientActions["wlPlayStatus"] = wlPlayStatusAction;;
	wlClientActions["wlGetConfig"] = wlGetConfigAction;
	wlClientActions["connectStatus"] = recvConnetResultAction;
	wlClientActions["locationStatus"] = recvSendGpsStatusAction;
	wlClientActions["sendGps"] = recvGpsAction;
	wlClientActions["messageStatus"] = recvMessageResultAction;
	wlClientActions["message"] = recvMsgAction;
	wlClientActions["status"] = statusAction;
	wlClientActions["sendArs"] = recvArsAction;
	wlClientActions["wlReadSerial"] = wlReadSerialAction;
	wlClientActions["sendBeacons"] = recvLocationIndoorAction;

	mclientActions["getSettingConfig"] = GetSettingAction;
	CBroker::instance()->startLogClient();
	
	CBroker::instance()->startWireLanClient(wlClientActions);
	CBroker::instance()->startRpcServer(serverActions);
	CBroker::instance()->startRadioClient(clientActions);
	CBroker::instance()->startMonitorClient(mclientActions);
	CHttp::getInstance()->start(HTTP_PORT);
	
	
	//rpcServer.addActionHandler("call", callAction);
	//rpcServer.addActionHandler("control", controlAction);
	//rpcServer.addActionHandler("queryGps", gpsAction);
	//rpcServer.addActionHandler("message", msgAction);

	//rpcServer.addActionHandler("start", startAction);



	//rpcServer.addActionHandler("setBaseSetting", setBaseAction);
	//rpcServer.addActionHandler("getBaseSetting", getBaseAction);

	//rpcServer.addActionHandler("setRadioSetting", setRadioAction);
	//rpcServer.addActionHandler("getRadioSetting", getRadioAction);

	//rpcServer.addActionHandler("setRepeaterSetting", setRepeaterAction);
	//rpcServer.addActionHandler("getRepeaterSetting", getRepeaterAction);

	////radio
	//rpcServer.addActionHandler("allCall", allCallAction);
	//rpcServer.addActionHandler("call", callAction);
	//rpcServer.addActionHandler("cancelGps", cancelGpsAction);
	//rpcServer.addActionHandler("connect", connectRadioAction);
	//rpcServer.addActionHandler("getGps", getGpsAction);
	//rpcServer.addActionHandler("getOverTurnGps", getOverTurnGpsAction);
	//rpcServer.addActionHandler("groupCall", groupCallAction);
	//rpcServer.addActionHandler("radioCheck", radioCheckAction);
	//rpcServer.addActionHandler("remotePowerOn", remotePowerOnAction);
	//rpcServer.addActionHandler("remotePowerOff", remotePowerOffAction);
	//rpcServer.addActionHandler("sendGroupSms", sendGroupSmsAction);
	//rpcServer.addActionHandler("sendSms", sendSmsAction);
	//rpcServer.addActionHandler("stopCall", stopCallAction);
	//rpcServer.addActionHandler("wiretap", wiretapAction);

	//rpcServer.addActionHandler("call", callAction);
	//rpcServer.addActionHandler("control", controlAction);
	//rpcServer.addActionHandler("queryGps", gpsAction);
	//rpcServer.addActionHandler("message", msgAction);
	//
	//rpcServer.start();
	
	/*CMonitorServer ms;
	std::string strName = getServerName();
	std::wstring wstr(strName.length(), L' ');
	std::copy(strName.begin(), strName.end(), wstr.begin());
	if (strName != "")
	{
		ms.startMonitor(wstr.c_str(),_T("Trbox.Log"));
		
	}*/
	while (1){ Sleep(1000); };
	CHttp::getInstance()->stop();
#endif
	return 0;
}

