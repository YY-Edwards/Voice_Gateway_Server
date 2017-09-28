#pragma once

#ifndef _ABSTRACT_CONNECTOR
#define _ABSTRACT_CONNECTOR

#include <string>
#include <functional>

class CRemotePeer{
public:
	virtual int sendResponse(const char* pData, int dataLen) = 0;
	virtual int sendCommand(const char* pData, int dataLen) = 0;
};

class OnConnectorData{
public:
	virtual int onReceive(CRemotePeer* pRemote, char* pData, int dataLen) = 0;
};

class JsonUtil{
public:
	enum _parse_type{
		IsJsonArray = 1,
		IsJsonObject,
		InvalidJson,
	};

	/*
	* Find if the pBuffer has a valid json string
	* @return pop buffer size >0 json string returned in strJson argument
	*/
	static int getJsonStr(const char* pBuffer, int len, std::string& strJson)
	{
		int nJsonType = InvalidJson;
		int idx = 0;

		while ((pBuffer[idx] != '{') && (pBuffer[idx] != '['))
		{
			idx++;
		}
		if (pBuffer[idx] == '{')
		{
			nJsonType = IsJsonObject;
		}
		else if ('[' == pBuffer[idx])
		{
			nJsonType = IsJsonArray;
		}
		else
		{
			// TODO: this should not happen
		}

		int nStartPosition = idx;		// json string start position

		int nSignStart = 1;
		while (nSignStart > 0 && idx < len)
		{
			idx++;
			if (IsJsonObject == nJsonType)
			{
				if (pBuffer[idx] == '}')
				{
					nSignStart--;
				}
				else if (pBuffer[idx] == '{')
				{
					nSignStart++;
				}
			}
			else
			{
				// json array
				if (pBuffer[idx] == ']')
				{
					nSignStart--;
				}
				else if (pBuffer[idx] == '[')
				{
					nSignStart++;
				}
			}
		}
		if (('}' == pBuffer[idx]) || (']' == pBuffer[idx]))
		{
			// find valid json string
			strJson = "";
			strJson.append(&pBuffer[nStartPosition], idx - nStartPosition + 1);
			return idx;
		}

		return 0;
	}
};

class CBaseConnector
{
public:
	virtual ~CBaseConnector(){}
	virtual int start(const char* connStr = NULL) = 0;
	virtual void stop() = 0;
	virtual int send(const char* pData, int dataLen) = 0;
	virtual int connect(const char* connStr) = 0;

	void setReceiveDataHandler(OnConnectorData* handler){
		m_hReceiveData = handler;
	}

	void setConnectEvent(std::function<void(CRemotePeer*)> fnEvent)
	{
		m_fnConnectEvent = fnEvent;
	}

	void setDisconnectEvent(std::function<void(CRemotePeer*)> fnEvent)
	{
		m_fnDisconnectEvent = fnEvent;
	}

	virtual bool isConnected() = 0;

protected:
	OnConnectorData* m_hReceiveData;
	std::function<void(CRemotePeer*)> m_fnDisconnectEvent;
	std::function<void(CRemotePeer*)> m_fnConnectEvent;
};


#ifndef uint64_t
typedef unsigned long long uint64_t;
#endif

#ifndef ACTION
typedef std::function<void(CRemotePeer*, const std::string&, uint64_t, const std::string&)> ACTION;
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif

#endif