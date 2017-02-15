#pragma once

#include <memory>
#include <mutex>
#include <map>

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

	void startRpcServer(std::map<std::string, ACTION> serverActions);
	void startRadioClient(std::map<std::string, ACTION> clientActions);
	void startWireLanClient(std::map<std::string, ACTION> clientActions);
	void sendWirelanConfig();
	void sendRadioConfig();
	void sendSettingConfig();
	void startLogClient();
	void startMonitorClient(std::map<std::string, ACTION> clientActions);
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
};

