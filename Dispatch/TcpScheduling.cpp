#include "stdafx.h"
#include "TcpScheduling.h"
#include "../lib/service/service.h"

std::list <TcpCommand>tcpCommandTimeOutList;
std::mutex m_allCommandListLocker;

std::string m_radioIP = "0.0.0.0";
std::string m_mnisIP = "0.0.0.0";
int num;
void(*myTcpCallBackFunc)( int, TcpRespone);
void onTcpData(void(*func)( int, TcpRespone),  int call, TcpRespone data);
static HDEVNOTIFY hDeviceNotify;
CTcpScheduling::CTcpScheduling()
{
	pXnlConnection = NULL;
	dwip = 0;
	m_workThread = true;
	m_timeThread = true;
	m_usbThread = true;
	m_connectThread = true;
	isTcpConnect = false;
	
}
CTcpScheduling::~CTcpScheduling()
{
	if (pXnlConnection)
	{
		delete pXnlConnection;
	}
}
int CTcpScheduling::radioConnect(const char* ip)
{
	// Connection
	dwip = inet_addr(ip);
	unsigned char iptemp[4];
	memcpy(iptemp, (void *)&dwip, 4);
	iptemp[3] = 1;
	dwip = *((DWORD *)iptemp);
	connect();
	//addTcpCommand( RADIO_CONNECT,ip,0,0);
	
	return 0;

}
int CTcpScheduling::privateCall( int id)
{
#if DEBUG_LOG
	LOG(INFO) << "单呼";
#endif
	unsigned long target_radio_id = id;
	unsigned char call_type = 0x04;     //0x06 单呼


	try
	{
		if (pXnlConnection != NULL)
		{
			if (target_radio_id != 0)
			{
				
				pXnlConnection->send_xcmp_call_ctrl_request(0x01, call_type, 0x01, target_radio_id, 0);
				pXnlConnection->send_xcmp_tx_ctrl_request(0x01, 0x00);


				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "单呼失败";
#endif
	}

	return 1;
}
int CTcpScheduling::groupCall(int id)
{
#if DEBUG_LOG
	LOG(INFO) << "组呼";
#endif
	unsigned long target_radio_id = id;
	unsigned char call_type = 0x06;     //0x06 组呼

	try
	{

		if ((pXnlConnection != NULL) && (pXnlConnection->m_bConnected == TRUE))
		{
			if (target_radio_id != 0)
			{
				//pXnlConnection->setRemotePeer(pRemote);
				pXnlConnection->send_xcmp_call_ctrl_request(0x01, call_type, 0x01, 0, target_radio_id);
				pXnlConnection->send_xcmp_tx_ctrl_request(0x01, 0x00);

				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "组呼失败";
#endif
	}

	return 1;
}
int CTcpScheduling::allCall()
{
#if DEBUG_LOG
	LOG(INFO) << "全呼";
#endif
	unsigned long target_radio_id = 255;   //全呼
	unsigned char call_type = 0x06;

	try
	{
		if ((pXnlConnection != NULL) && (pXnlConnection->m_bConnected == TRUE))
		{
			if (target_radio_id != 0)
			{
			
				pXnlConnection->send_xcmp_call_ctrl_request(0x01, call_type, 0x01, 0, target_radio_id);
				pXnlConnection->send_xcmp_tx_ctrl_request(0x01, 0x00);

				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "全呼失败";
#endif
	}
	return 1;

}
int CTcpScheduling::stopCall()
{
#if DEBUG_LOG
	LOG(INFO) << "停止呼叫";
#endif
	unsigned char call_type = 0x04;

	try
	{
		if (pXnlConnection != NULL)
		{
			pXnlConnection->send_xcmp_tx_ctrl_request(0x02, 0x00);

			return 0;

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "停止呼叫失败";
#endif
	}

	return 1;
}
int CTcpScheduling::remotePowerOn(int id)
{
#if DEBUG_LOG
	LOG(INFO) << "遥开";
#endif
	unsigned long target_radio_id = 0;
	target_radio_id = id;

	try
	{
		if (pXnlConnection != NULL)
		{
			if (target_radio_id != 0)
			{
				pXnlConnection->send_xcmp_rmt_radio_ctrl_request(0x02, 0x01, 0x01, target_radio_id);

				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "遥开失败";
#endif
	}

	return 1;
}
int CTcpScheduling::remotePowerOff( int id)
{
#if DEBUG_LOG
	LOG(INFO) << "遥闭";
#endif
	unsigned long target_radio_id = id;

	try
	{
		if (pXnlConnection != NULL)
		{

			if (target_radio_id != 0)
			{
				pXnlConnection->send_xcmp_rmt_radio_ctrl_request(0x01, 0x01, 0x01, target_radio_id);

			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "遥闭失败";
#endif
	}
	return 1;
}
int CTcpScheduling::radioCheck( int id)
{
#if DEBUG_LOG
	LOG(INFO) << "在线检测";
#endif
	unsigned long target_radio_id = 0;
	target_radio_id = id;

	try
	{
		if (pXnlConnection != NULL)
		{

			if (target_radio_id != 0)
			{
				pXnlConnection->send_xcmp_rmt_radio_ctrl_request(0x00, 0x01, 0x01, target_radio_id);

				return 0;
			}
			else
			{
				return 1;
			}

		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "在线检测失败";
#endif
	}
	return 1;

}
int CTcpScheduling::wiretap( int id)
{
#if DEBUG_LOG
	LOG(INFO) << "远程监听";
#endif
	unsigned long target_radio_id = 0;
	target_radio_id = id;

	try
	{
		if ((pXnlConnection != NULL) && (pXnlConnection->m_bConnected == TRUE))
		{

			if (target_radio_id != 0)
			{
				
				pXnlConnection->send_xcmp_rmt_radio_ctrl_request(0x03, 0x01, 0x01, target_radio_id);

				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
	catch (double)
	{
#if DEBUG_LOG
		LOG(ERROR) << "远程监听失败";
#endif
	}
	return 1;

}
void CTcpScheduling::workThreadFunc()
{
	while (m_workThread)
	{
		std::lock_guard <std::mutex> locker(m_workLocker);
		std::list<TcpCommand>::iterator it;
		for (it = workList.begin(); it != workList.end(); ++it)
		{
			switch (it->command)
			{
			case  RADIO_CONNECT:
				/*if (it->radioIP != "")
				{
					connect();
				}*/
				break;
			case PRIVATE_CALL:
				privateCall(it->radioId);
				break;
			case GROUP_CALL:
				groupCall(it->radioId);
				break;
			case ALL_CALL:
				allCall();
				break;
			case STOP_CALL:
				stopCall();
				break;
			case REMOTE_CLOSE:
				remotePowerOff(it->radioId);
				break;
			case REMOTE_OPEN:
				remotePowerOn(it->radioId);
				break;
			case CHECK_RADIO_ONLINE:
				radioCheck(it->radioId);
				break;
			case REMOTE_MONITOR:
				wiretap(it->radioId);
				break;
			default:
				break;
			}
			it = workList.erase(it);
			break;
		}
		Sleep(10);
	}	
}
void CTcpScheduling::addTcpCommand( int command, std::string radioIP, int id, int callType)
{
	std::lock_guard<std::mutex> locker(m_addCommandLocker);
	TcpCommand      m_allCommand;
	m_allCommand.command = command;
	m_allCommand.timeOut = 60000;     //ms
	m_allCommand.timeCount = 0;
	m_allCommand.radioIP = radioIP;
	m_allCommand.radioId = id;
	//std::lock_guard <std::mutex> locker(m_allCommandListLocker);
	tcpCommandTimeOutList.push_back(m_allCommand);
	//std::lock_guard <std::mutex> wlocker(m_workLocker);
	workList.push_back(m_allCommand);
	//seq = callId +1;
}
DWORD WINAPI CTcpScheduling::timeOutThread(LPVOID lpParam)
{
	CTcpScheduling * p = (CTcpScheduling *)(lpParam);
	if ( p!=NULL)
	{
		p->timeOut();
	}
	return 1;
}
void CTcpScheduling::timeOut()
{
	while (m_timeThread)
	{
		m_allCommandListLocker.lock();
		std::list<TcpCommand>::iterator it;
		for (it = tcpCommandTimeOutList.begin(); it != tcpCommandTimeOutList.end(); ++it)
		{
			it->timeCount++;
			if (it->timeCount % (it->timeOut / 100) == 0)
			{

			//	if (it->peer != NULL)
				{
					int operate = -1;
					int type = -1;
					switch (it->command)
					{
					case  RADIO_CONNECT:
						break;
					case PRIVATE_CALL:
						if (myTcpCallBackFunc != NULL)
						{
							TcpRespone tr;
							tr.id = it->radioId;
							tr.callType = PRIVATE;
							tr.result = REMOTE_FAILED;
							onTcpData(myTcpCallBackFunc, ALL_CALL, tr);
							it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
							LOG(INFO) << "单呼失败！";
#endif
							break;
						}
						break;
					case GROUP_CALL:
						if (myTcpCallBackFunc != NULL)
						{
							TcpRespone tr;
							tr.id = it->radioId;
							tr.callType = GROUP;
							tr.result = REMOTE_FAILED;
							onTcpData(myTcpCallBackFunc, ALL_CALL, tr);
							it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
							LOG(INFO) << "组呼失败！";
#endif
							break;
						}
						break;
					case ALL_CALL:
						if (myTcpCallBackFunc != NULL)
						{
							TcpRespone tr;
							tr.id = it->radioId;
							tr.callType = ALL;
							tr.result = REMOTE_FAILED;
							onTcpData(myTcpCallBackFunc, ALL_CALL, tr);
							it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
							LOG(INFO) << "全呼失败！";
#endif
							break;
						}

						/*try
						{
						ArgumentType args;
						args["Status"] = FieldValue(REMOTE_FAILED);
						args["Target"] = FieldValue(it->radioId);
						args["Operate"] = FieldValue(operate);
						args["Type"] = FieldValue(START);
						std::string callJsonStr = CRpcJsonParser::buildCall("callStatus", num, args, "radio");
						if (it->peer != NULL)
						{
						it->peer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
						}

						}
						catch (std::exception e)
						{

						}*/
						break;
					case STOP_CALL:
						try
						{
							if (myTcpCallBackFunc != NULL)
							{
								TcpRespone tr;
								tr.id = it->radioId;
								tr.callType = STOP;
								tr.result = REMOTE_FAILED;
								onTcpData(myTcpCallBackFunc, ALL_CALL, tr);
								it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
								LOG(INFO) << "停止呼叫失败！";
#endif
								break;
							}
							//ArgumentType args;
							//args["Status"] = FieldValue(REMOTE_FAILED);
							//args["Target"] = FieldValue(it->radioId);
							//args["Operate"] = FieldValue(STOP);
							////args["Type"] = FieldValue(it->querymode);
							//std::string callJsonStr = CRpcJsonParser::buildCall("callStatus", num, args, "radio");
							//if (it->peer != NULL)
							//{
							//	it->peer->sendResponse((const char *)callJsonStr.c_str(), callJsonStr.size());
							//}

						}
						catch (std::exception e)
						{

						}
						break;
					case REMOTE_CLOSE:
						if (myTcpCallBackFunc != NULL)
						{
							TcpRespone tr;
							tr.id = it->radioId;
							tr.controlType = OFF;
							tr.result = REMOTE_FAILED;
							onTcpData(myTcpCallBackFunc, REMOTE_CLOSE, tr);
							it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
							LOG(INFO) << "遥闭失败！";
#endif
							break;
						}
						break;
					case REMOTE_OPEN:
						try
						{
							if (myTcpCallBackFunc != NULL)
							{
								TcpRespone tr;
								tr.id = it->radioId;
								tr.controlType = ON;
								tr.result = REMOTE_FAILED;
								onTcpData(myTcpCallBackFunc, REMOTE_CLOSE, tr);
								it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
								LOG(INFO) << "遥开失败！";
#endif
								break;
							}

						}
						catch (std::exception e)
						{

						}
						break;
					case CHECK_RADIO_ONLINE:
						try
						{
							if (myTcpCallBackFunc != NULL)
							{
								TcpRespone tr;
								tr.id = it->radioId;
								tr.controlType = RADIOCHECK;
								tr.result = REMOTE_FAILED;
								onTcpData(myTcpCallBackFunc, REMOTE_CLOSE, tr);
								it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
								LOG(INFO) << "在线检测失败！";
#endif
								break;
							}

						}
						catch (std::exception e)
						{

						}
						break;
					case REMOTE_MONITOR:
						try
						{
							if (myTcpCallBackFunc != NULL)
							{
								TcpRespone tr;
								tr.id = it->radioId;
								tr.controlType = MONITOR;
								tr.result = REMOTE_FAILED;  
								onTcpData(myTcpCallBackFunc, REMOTE_CLOSE, tr);
								it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
								LOG(INFO) << "远程监听失败！";
#endif
								break;
							}

						}
						catch (std::exception e)
						{

						}
						break;
					default:
						break;
					}
					if (tcpCommandTimeOutList.size()>0)
					if(it->command != RADIO_CONNECT)
					{
						it = tcpCommandTimeOutList.erase(it);
						break;
					}

				}

			}
			break;
		}
		m_allCommandListLocker.unlock();
		Sleep(100);
	}
}

int CTcpScheduling::tcpConnect()
{
	while (m_connectThread)
	{
		if (pXnlConnection == NULL || !isTcpConnect)
		{
			pXnlConnection = CXNLConnection::CreatConnection(dwip, 8002, "0x152C7E9D0x38BE41C70x71E96CA40x6CAC1AFC",
				strtoul("0x9E3779B9", NULL, 16));

			std::list<TcpCommand>::iterator it;
			//for (it = tcpCommandTimeOutList.begin(); it != tcpCommandTimeOutList.end(); it++)
			{
			//	if (it->command == RADIO_CONNECT)
				{
					if (pXnlConnection == NULL)    //0:调度业务和数据业务都连接失败 1： 调度业务连接成功，数据业务连接失败 2：调度业务连接失败，数据业务连接成功 3. 调度业务和数据业务都连接成功
					{
						isTcpConnect = false;
						TcpRespone tr;
						tr.result = 1;
						onTcpData(myTcpCallBackFunc, RADIO_CONNECT, tr);
						//it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
						LOG(ERROR) << "调度连接失败！";
#endif
						//break;
					}
					else
					{
						isTcpConnect = true;
						TcpRespone tr;
						tr.result = 0;
						onTcpData(myTcpCallBackFunc, RADIO_CONNECT, tr);
						//it = tcpCommandTimeOutList.erase(it);
#if DEBUG_LOG
						LOG(ERROR) << "调度连接成功！";
#endif
						//break;
					}

				}

			}
		}
		Sleep(30000);
	}
	return 0;
}
DWORD WINAPI CTcpScheduling::tcpConnectionThread(LPVOID lpParam)
{
	CTcpScheduling * p = (CTcpScheduling *)lpParam; 
	if (p!=NULL)
	{
		p->tcpConnect();
		
	}
	
	return 1;
}
DWORD WINAPI CTcpScheduling::workThread(LPVOID lpParam)
{
	CTcpScheduling * p = (CTcpScheduling *)(lpParam);
	if (p!=NULL)
	{
		p->workThreadFunc();
	}
	return 1;
}
DWORD WINAPI CTcpScheduling::radioUsbStatusThread(LPVOID lpParam)
{
	CTcpScheduling * p = (CTcpScheduling *)(lpParam);
	if (p!=NULL)
	{
		p->radioUsbStatus();
	}
	return 1;
}
//LRESULT message_handler(HWND__* hwnd, UINT uint, WPARAM wparam, LPARAM lparam)
//{
//	switch (uint)
//	{
//	case WM_NCCREATE: // before window creation
//		return true;
//		break;
//
//	case WM_CREATE: // the actual creation of the window
//	{
//						// you can get your creation params here..like GUID..
//						LPCREATESTRUCT params = (LPCREATESTRUCT)lparam;
//						GUID InterfaceClassGuid = *((GUID*)params->lpCreateParams);
//						DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
//						ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
//						NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
//						NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
//						NotificationFilter.dbcc_classguid = InterfaceClassGuid;
//						HDEVNOTIFY dev_notify = RegisterDeviceNotification(hwnd, &NotificationFilter,
//							DEVICE_NOTIFY_WINDOW_HANDLE);
//						if (dev_notify == NULL)
//						{
//							throw std::runtime_error("Could not register for devicenotifications!");
//						}
//						break;
//	}
//
//	case WM_DEVICECHANGE:
//	{
//							PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)lparam;
//							PDEV_BROADCAST_DEVICEINTERFACE lpdbv = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
//							std::string path;
//							if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
//							{
//								switch (wparam)
//								{
//								case DBT_DEVICEARRIVAL:
//									break;
//								case DBT_DEVICEREMOVECOMPLETE:
//									isTcpConnect = false;
//									break;
//								}
//							}
//							break;
//	}
//
//	}
//	return 0L;
//}
//unsigned long __stdcall DeviceEventNotify(DWORD evtype, PVOID evdata)
//{
//	switch (evtype)
//	{
//		case DBT_DEVICEREMOVECOMPLETE:
//		{			
//			isTcpConnect = false;
//		}
//			break;
//		case DBT_DEVICEARRIVAL:
//		{
//			isTcpConnect = true;
//		}
//		break;
//	}
//	return 0;
//}
//DWORD WINAPI CtrlHandler(_In_ DWORD dwCtrl, _In_ DWORD dwEventType, _In_ LPVOID lpEventData, _In_ LPVOID lpContext)
//{
//	switch (dwCtrl)
//	{
//	case SERVICE_CONTROL_DEVICEEVENT:
//	{
//		DeviceEventNotify(dwEventType, lpEventData);
//	}
//		break;
//	default:
//		break;
//	}
//	return 1;
//}
void CTcpScheduling::radioUsbStatus()
{

	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_NET;
	while (hDeviceNotify == NULL)
	{
		hDeviceNotify = RegisterDeviceNotification(CService::instance()->m_StatusHandle,	// events recipient
			&NotificationFilter,	// type of device 
			DEVICE_NOTIFY_SERVICE_HANDLE	// type of recipient handle 
			);
		Sleep(100);
	}

	//if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	//{
		//while (m_usbThread)
		//{
			/*HWND hWnd = NULL;
			WNDCLASSEX wx;
			ZeroMemory(&wx, sizeof(wx));

			wx.cbSize = sizeof(WNDCLASSEX);
			wx.lpfnWndProc = reinterpret_cast<WNDPROC>(message_handler);
			wx.hInstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(0));
			wx.style = CS_HREDRAW | CS_VREDRAW;
			wx.hInstance = GetModuleHandle(0);
			wx.hbrBackground = (HBRUSH)(COLOR_WINDOW);
			wx.lpszClassName = CLS_NAME;

			GUID guid = GUID_DEVINTERFACE_NET;

			if (RegisterClassEx(&wx))
			{
			hWnd = CreateWindow(CLS_NAME, _T("DevNotifWnd"), WS_ICONIC,
			0, 0, CW_USEDEFAULT, 0, HWND_MESSAGE,
			NULL, GetModuleHandle(0), (void*)&guid);
			}

			if (hWnd == NULL)
			{
			throw std::runtime_error("Could not create message window!");
			}

			MSG msg;
			BOOL bRet;


			while (((bRet = GetMessage(&msg, NULL, 0, 0) > 0)) != 0)
			{
			try
			{
			if (bRet == -1)
			{

			}
			else
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}

			}
			catch (std::exception e)
			{

			}
			}*/


			/*while (CService::instance()->m_StatusHandle)
			{

			}*/
		//	Sleep(100);
		//}
//	}
	

}
void  CTcpScheduling::setCallBackFunc(void(*callBackFunc)( int, TcpRespone))
{
	myTcpCallBackFunc = callBackFunc;
}

void CTcpScheduling::connect()
{
	m_connectThread = true;
	m_workThread = true;
	m_timeThread = true;
	m_usbThread = true;
	m_cMt = CreateThread(NULL, 0, tcpConnectionThread, this, THREAD_PRIORITY_NORMAL, NULL);
	m_wMt = CreateThread(NULL, 0, workThread, this, THREAD_PRIORITY_NORMAL, NULL);
	m_tMt = CreateThread(NULL, 0, timeOutThread, this, THREAD_PRIORITY_NORMAL, NULL);
	m_uMt = CreateThread(NULL, 0, radioUsbStatusThread, this, THREAD_PRIORITY_NORMAL, NULL);
}
void CTcpScheduling::call( int type, int op, int id )
{
	if (START == op)
	{

		switch (type)
		{
		case PRIVATE:
			addTcpCommand( PRIVATE_CALL, "", id, PRIVATE);
			break;
		case GROUP:
			addTcpCommand( GROUP_CALL, "", id, GROUP);
			break;
		case ALL:
			addTcpCommand( ALL_CALL, "", id, ALL);
			break;
		}
	}
	else
	{
		addTcpCommand(STOP_CALL,"",id,type);
	}

}
void CTcpScheduling::control( int type, int id)
{
	switch (type)
	{
	case  RADIOCHECK :
		addTcpCommand(CHECK_RADIO_ONLINE, "", id,0);
		break;
	case  MONITOR :
		addTcpCommand( REMOTE_MONITOR, "", id,0);
		break;
	case  OFF :
		addTcpCommand( REMOTE_CLOSE, "", id, 0);
		break;
	case  ON :
		addTcpCommand( REMOTE_OPEN, "", id, 0);
		break;
	default:
		break;
	}
}
void CTcpScheduling::disConnect()
{
	if (m_wMt && isTcpConnect)
	{
		m_workThread = false;
		WaitForSingleObject(m_wMt, 1000);
		CloseHandle(m_wMt);
	}
	if (m_cMt && isTcpConnect)
	{
		m_connectThread = false;
		WaitForSingleObject(m_cMt, 1000);
		CloseHandle(m_cMt);
	}
	if (m_tMt && isTcpConnect)
	{
		m_timeThread = false;
		WaitForSingleObject(m_tMt, 1000);
		CloseHandle(m_tMt);
	}
	if (m_uMt && isTcpConnect)
	{
		m_usbThread = false;
		WaitForSingleObject(m_uMt, 1000);
		CloseHandle(m_uMt);
	}
	workList.clear();
	tcpCommandTimeOutList.clear();
	UnregisterDeviceNotification(hDeviceNotify);
	isTcpConnect = false;
	pXnlConnection = NULL;
}
void onTcpData(void(*func)(int, TcpRespone),  int call, TcpRespone data)
{

	try
	{
		func( call, data);
	}
	catch (double)
	{

	}

}
void CTcpScheduling::setUsb(bool result)
{
	isTcpConnect = result;
}

