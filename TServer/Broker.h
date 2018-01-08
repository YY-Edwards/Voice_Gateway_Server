#pragma once

#include <memory>
#include <mutex>
#include <map>

#define System_WorkMode 0x01
#define System_ServerStatus 0x02
#define System_DeviceStatus 0x03
#define System_MnisStatus 0x04
#define System_DatabaseStatus 0x05
#define System_DongleCount 0x06
#define System_MicphoneStatus 0x07
#define System_SpeakerStatus 0x08
#define System_LEStatus 0x09
#define System_WireLanStatus 0x0A
#define System_DeviceInfoStatus 0x0B

struct SerialInformation{
	char licType[12];
	int deviceType;
	char pcMac[16];
	char radioSerial[16];
	char repeaterSerial[16];
	char time[16];
	short isEver;
	char expiration[14];
	int funcList[16];
	char operate[16];
	char res[16];
	char radioMode[13];
	char repeaterMode[13];

};
struct SystemStatus
{
	 /*
	 "WorkMode":0:offline,1:Only Vechtion Stations, 2:Only Repeater,3:Vechtion Stations With MNIS, 4:repeater with mnis
	 "ServerStatus":0:connected, 1:disconnect //vechion station status OR repeater status
	 "DeviceStatus":0:connected, 1:disconnect //vechion station status OR repeater status
	 "MnisStatus":0:connected, 1:disconnected
	 "DatabaseStatus":0:connected, 1:disconnected
	 "DongleCount":1
	 "MicphoneStatus":0:connected, 1:disconnected
	 "SpeakerStatus":0:connected, 1:disconnected
	 "LEStatus":0:connected, 1:disconnected
	 "WireLanStatus":0:registed, 1:UnRegisted
	 "DeviceInfoStatus":0:Updated, 1:UnKnow
		*/
	int workMode;
	int serverStatus;
	int deviceStatus;
	int mnisStatus;
	int dongleCount;
	int micphoneStatus;
	int speakerStatus;
	int leStatus;
	int wireLanStatus;
	int deviceInfoStatus;
	int databaseStatus;
};
class CRpcClient;
class CRpcServer;

class CBroker
{
public:
	static CBroker* instance(){
		static std::mutex locker;

		if (NULL == m_instance.get())
		{
			std::lock_guard<std::mutex> lock(locker);

			if (NULL == m_instance.get())
			{
				m_instance.reset(new CBroker());
			}
		}

		return m_instance.get();
	}

	CRpcClient* getRadioClient(){
		return m_radioClient;
	}

	CRpcClient* getWireLanClient()
	{
		return m_wirelanClient;
	}

	CRpcClient* getLogClient()
	{
		return m_logClient;
	}

	CRpcServer* getRpcServer()
	{
		return m_rpcServer;
	}

	int getCallId(){
		return callId++;
	}
	void setSerialInformation(SerialInformation serialInformation);
	SerialInformation getSerialInformation();
	void setLicenseInformation(SerialInformation serialInformation);
	SerialInformation getLicenseInformation();
	void setLicenseStatus(int status);
	int getLicenseStatus();
	void startRpcServer(std::map<std::string, ACTION> serverActions);
	void startRadioClient(std::map<std::string, ACTION> clientActions);
	void startWireLanClient(std::map<std::string, ACTION> clientActions);
	void sendWirelanConfig();
	void sendRadioConfig();
	void sendSettingConfig();
	void startLogClient();
	void startMonitorClient(std::map<std::string, ACTION> clientActions);
	bool getLic(std::string license);
	void sendLoactionIndoorConfig();
	//void sendLoactionIndoorConfigToWl();
	void getSystemstatus(std::string type);
	void stop();

	void setDeviceStatus(bool device, bool mnis);
	//SystemStatus getSystemStatus();
	void sendSystemStatusToClient(std::string  sessionId, CRemotePeer* pRemote, uint64_t callId);
	static DWORD WINAPI clientConnectStatusThread(LPVOID lpParam);
	void setDeviceStatusByType(int type, int value);
	
protected:
	CBroker();
	~CBroker();

private:
	void setSystemStatus();
	friend class std::auto_ptr<CBroker>;
	static std::auto_ptr<CBroker> m_instance;

	int callId;

	CRpcClient* m_radioClient;
	CRpcClient* m_wirelanClient;
	CRpcClient* m_logClient;
	CRpcClient* m_monitorClient;
	CRpcServer* m_rpcServer;
	
	SerialInformation m_serialInformation ;
	SerialInformation m_licenseInformation;
	int licenseStatus;
	SystemStatus  systemStatus;
	bool isRadio;
	bool isRepeater;
	bool isMnis;
	bool isDeviceConnect;
	bool isMnisConenct;
	bool isRecvSerial;
	bool isStart;
	bool isRadioStart;
	bool isRepeaterStart;
	int isLastDispatchStatus;
	int isLastWlStatus;
	int isLastSerialStatus;
	int isLastRadioConnectStatus;
	int isLastMnisConnectStatus;
	int  lastDongleCount;
	int isLastMicphoneStatus;
	int isLastSpeakerStatus;
	int isLastLEStatus;
	int isLastWireLanStatus;
	void clientConnectStatus();
	std::mutex    sendLock;
	std::mutex    DeviceStatusLock;
	std::mutex    threadLock;
	
};

