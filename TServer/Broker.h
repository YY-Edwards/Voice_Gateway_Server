#pragma once

#include <memory>
#include <mutex>
#include <map>

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
protected:
	CBroker();
	~CBroker();

private:
	friend class std::auto_ptr<CBroker>;
	static std::auto_ptr<CBroker> m_instance;

	int callId;

	CRpcClient* m_radioClient;
	CRpcClient* m_wirelanClient;
	CRpcClient* m_logClient;
	CRpcServer* m_rpcServer;
	CRpcClient* m_monitorClient;
	SerialInformation m_serialInformation;
	SerialInformation m_licenseInformation;
	int licenseStatus;
	
};

