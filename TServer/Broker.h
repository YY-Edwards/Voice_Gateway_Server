#pragma once

#include <memory>
#include <mutex>

class CRpcClient;

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

	int getCallId(){
		return callId++;
	}

protected:
	CBroker();
	~CBroker();

private:
	friend class std::auto_ptr<CBroker>;
	static std::auto_ptr<CBroker> m_instance;

	int callId;

	CRpcClient* m_radioClient;
	CRpcClient* m_wirelanClient;
};

