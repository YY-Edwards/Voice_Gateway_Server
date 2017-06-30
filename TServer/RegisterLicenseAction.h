#include <string>
#include <mutex>
#include "../lib/type.h"
#include "../lib/rpc/include/BaseConnector.h"
#include "../lib/rpc/include/RpcJsonParser.h"

std::wstring getAppdataPath(){
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);

	return std::wstring(szBuffer);
}
std::string wstringToString(const std::wstring& wstr)
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
void writeLicense(std::string lic)
{
	int createFileRlt = 0;
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);
	std::wstring appFolder = getAppdataPath() + _T("\\Jihua Information");
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
	std::string temp = wstringToString(logFolder) + "\\serial.lic";
	FILE *fp;
	fopen_s(&fp,temp.c_str(), "w+"); // 已文件后缀名为.lic,"w"方式打开文件
	if (fp != NULL)
	{
		fprintf(fp, "%s", lic.c_str());
	}
	
	fclose(fp);
}

void registerLicenseAction(CRemotePeer* pRemote, const std::string& param, uint64_t callId, const std::string& type)
{
	static std::mutex lock;

	std::lock_guard<std::mutex> locker(lock);
	LOG(INFO) << "registerLicenseAction";
	try{
		Document d;
		d.Parse(param.c_str());
		if (d.HasMember("license"))
		{
			std::string lic = d["license"].GetString();
			//将lic存入本地文件
			writeLicense(lic);
			bool result = CBroker::instance()->getLic(lic);
			if (result)
			{
				SerialInformation license = CBroker::instance()->getLicenseInformation();
				ArgumentType args;
				args["Time"] = license.time;
				args["IsEver"] = license.isEver;
				args["Expiration"] = license.expiration;
				std::string strResp = CRpcJsonParser::buildResponse("success", callId, 200, "", args);
				pRemote->sendResponse(strResp.c_str(), strResp.size());
			}
			else
			{
				std::string strResp = CRpcJsonParser::buildResponse("faliure", callId, 201, "", ArgumentType());
				pRemote->sendResponse(strResp.c_str(), strResp.size());
			}
			CBroker::instance()->setLicenseStatus(result);
			

		}
		
	}
	catch (std::exception e){

	}
	catch (...)
	{

	}

}

