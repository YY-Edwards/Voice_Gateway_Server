#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

#define CRADIO   1
#define REPEATER 2
#define RADIO    3
#define PC       4
std::wstring getAppdataPath1(){
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);

	return std::wstring(szBuffer);
}
std::string wstringToString1(const std::wstring& wstr)
{
	LPCWSTR pwszSrc = wstr.c_str();
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
		return std::string("");

	char* pszDst = new char[nLen];
	if (!pszDst)
		return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	std::string str(pszDst);
	delete[] pszDst;
	pszDst = NULL;

	return str;
}
void readSerial()
{
	//打开本地文件获取serial
	int createFileRlt = 0;
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);
	std::wstring appFolder = getAppdataPath1() + _T("\\Jihua Information");
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

	std::wstring logFolder = appFolder + _T("\\license");
	if (!PathFileExists(logFolder.c_str()))
	{
		createFileRlt = _wmkdir(logFolder.c_str());
	}
	std::string temp = wstringToString1(logFolder) + "\\serial.lic";
	FILE *fp;
	fopen_s(&fp, temp.c_str(), "r"); // 已文件后缀名为.lic,"w"方式打开文件
	char szTest[1000] = { 0 };
	if (fp != NULL)
	{
		while (!feof(fp))
		{
			memset(szTest, 0, sizeof(szTest));
			fgets(szTest, sizeof(szTest)-1, fp); // 包含了\n  
		}

	}

	fclose(fp);
	CBroker::instance()->getLic(szTest);
}
void readSerialAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);

	try{

		static std::mutex lock;
		std::lock_guard<std::mutex> locker(lock);
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("serial") && d["serial"].IsString())
		{
			SerialInformation s = CBroker::instance()->getSerialInformation();
			memcpy(s.licType, "TrboX 3.0",12);
			std::string serial = d["serial"].GetString();
			if (type == "radio")
			{
				s.deviceType = CRADIO;    //DeviceType：设备类型(1:车载台，2：中继台，3：对讲机， 4：PC)
				if (serial.length() == 10)
				{
					memcpy(s.radioSerial, serial.c_str(), 16);
				}
				else if (serial.length() == 12)
				{
					memcpy(s.radioMode, serial.c_str(), 16);
				}
				
				CBroker::instance()->setSerialInformation(s);
			}
			else if (type == "wlire")
			{
				s.deviceType = REPEATER;
				if (serial.length() == 10)
				{
					memcpy(s.repeaterSerial, serial.c_str(), 16);
				}
				else if (serial.length() == 12)
				{
					memcpy(s.repeaterMode, serial.c_str(), 16);
				}
				CBroker::instance()->setSerialInformation(s);
			}
			
		  
			std::string strResp = CRpcJsonParser::buildResponse("sucess", callId, 200, "", ArgumentType());
			pRemote->sendResponse(strResp.c_str(), strResp.size());

			readSerial();
		}

	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}
