#include "stdafx.h"
#include <string>
#include <iostream>
//#include <pthread.h>
#include "../lib/mongoose/mongoose.h"
#include <shlobj.h> 
#include <Shlwapi.h>
#include "schexception.h"
#include "Http.h"

std::shared_ptr<CHttp> CHttp::m_instance;

void CHttp::handle_upload(struct mg_connection *nc, int ev, void *p)
{
	CHttp* pThis = (CHttp*)nc->user_data;
	struct mg_http_multipart_part *mp = (struct mg_http_multipart_part *) p;
	struct http_message *hm = (struct http_message *) p;
	
	std::string addrKey = inet_ntoa(nc->sa.sin.sin_addr);
	addrKey += ":";
	addrKey += std::to_string(nc->sa.sin.sin_port);

	try
	{
		switch (ev) {
		case MG_EV_HTTP_REQUEST: {    //add by wx 
									 char var_name[100], file_name[100];
									 const char *chunk;
									 size_t chunk_len, n1, n2;								
									 n1 = n2 = 0;
									 while ((n2 = mg_parse_multipart(hm->body.p + n1,hm->body.len - n1, var_name, sizeof(var_name),
										 file_name, sizeof(file_name), &chunk, &chunk_len)) > 0) {
										 printf("var: %s, file_name: %s, size: %d, chunk: [%.*s]\n",
											 var_name, file_name, (int)chunk_len,
											 (int)chunk_len, chunk);
										 n1 += n2;
									 }
									 if (pThis == NULL) {
										 CUploadFile* pUpload = new CUploadFile();
										 pThis = CHttp::getInstance();
										 std::string fileName(file_name);
										 //std::string filePath = "/tmp/" + fileName;
										 std::string filePath=saveFile(fileName);
										 fopen_s(&(pUpload->m_pfTmpFile), filePath.c_str(), "wb+");
										 pUpload->m_nWriteLen = 0;
										 pUpload->m_strFileName = fileName;
										
										 if (0 == pUpload->m_pfTmpFile) {
											 throw SchException("Failed to open file");
											 return;
										 }

										 if (pThis->m_mpClientUploadFiles.count(addrKey) > 0);
										 {
											 delete pThis->m_mpClientUploadFiles[addrKey];
											 pThis->m_mpClientUploadFiles.erase(addrKey);
										 }
										 pThis->m_mpClientUploadFiles[addrKey] = pUpload;
										 nc->user_data = (void *)pThis;
										 //±£´æÊý¾Ý
										 if (fwrite(chunk, 1, chunk_len, pThis->m_mpClientUploadFiles[addrKey]->m_pfTmpFile) !=chunk_len) {
											 throw SchException("Failed to write to a file");
										 }
										 pThis->m_mpClientUploadFiles[addrKey]->m_nWriteLen += chunk_len;
										 mg_printf(nc,
											 "HTTP/1.1 200 OK\r\n"
											 "Content-Type: text/plain\r\n"
											 "Connection: close\r\n\r\n"
											 "Written %ld of POST data to a temp file\n\n",
											 (long)ftell(pThis->m_mpClientUploadFiles[addrKey]->m_pfTmpFile));
										 nc->flags |= MG_F_SEND_AND_CLOSE;
										 fclose(pThis->m_mpClientUploadFiles[addrKey]->m_pfTmpFile);
										 nc->user_data = NULL;

										 // TODO: set event that new image incoming

										 // Release object
										 delete pThis->m_mpClientUploadFiles[addrKey];
										 pThis->m_mpClientUploadFiles.erase(addrKey);
									 }
									 }
			break;
		case MG_EV_HTTP_PART_BEGIN: {
			if (pThis == NULL) {
				CUploadFile* pUpload = new CUploadFile();

				pThis = CHttp::getInstance();
				std::string fileName(mp->file_name);
				std::string filePath = "/tmp/" + fileName;
				// dump header
#if 0			
	std::cout<<"var name:"<<mp->var_name<<std::endl;
				std::string strHead;
				for (int ll = 0; ll < mp->data.len; ll++)
				{
					char buf[10];
					sprintf(buf, "0x%2x-", mp->data.p[ll]);
					strHead += buf;
				}
				std::cout << "first Head:" << strHead << std::endl;
#endif
				// end dump header
				fopen_s(&(pUpload->m_pfTmpFile),filePath.c_str(), "wb+");
				pUpload->m_nWriteLen = 0;
				pUpload->m_strFileName = fileName;
				if (0 == pUpload->m_pfTmpFile) {
					throw SchException("Failed to open file");
					return;
				}
				
				if (pThis->m_mpClientUploadFiles.count(addrKey) > 0);
				{
					delete pThis->m_mpClientUploadFiles[addrKey];
					pThis->m_mpClientUploadFiles.erase(addrKey);
				}
				pThis->m_mpClientUploadFiles[addrKey] = pUpload;

				nc->user_data = (void *)pThis;
			}
			break;
		}
		case MG_EV_HTTP_PART_DATA: {
			if (fwrite(mp->data.p, 1, mp->data.len, pThis->m_mpClientUploadFiles[addrKey]->m_pfTmpFile) != mp->data.len) {
				throw SchException("Failed to write to a file");
			}
			pThis->m_mpClientUploadFiles[addrKey]->m_nWriteLen += mp->data.len;
			break;
		}
		case MG_EV_HTTP_PART_END: {
			mg_printf(nc,
				"HTTP/1.1 200 OK\r\n"
				"Content-Type: text/plain\r\n"
				"Connection: close\r\n\r\n"
				"Written %ld of POST data to a temp file\n\n",
				(long)ftell(pThis->m_mpClientUploadFiles[addrKey]->m_pfTmpFile));
			nc->flags |= MG_F_SEND_AND_CLOSE;
			fclose(pThis->m_mpClientUploadFiles[addrKey]->m_pfTmpFile);
			nc->user_data = NULL;

			// TODO: set event that new image incoming

			// Release object
			delete pThis->m_mpClientUploadFiles[addrKey];
			pThis->m_mpClientUploadFiles.erase(addrKey);
			break;
		}
		}
	}
	catch (SchException& e)
	{
		mg_printf(nc,
			"HTTP/1.1 500 %s\r\n"
			"Content-Length: 0\r\n\r\n", e.what());
		nc->flags |= MG_F_SEND_AND_CLOSE;
	}
	catch (...)
	{
		mg_printf(nc, "%s",
			"HTTP/1.1 500 Unknown error\r\n"
			"Content-Length: 0\r\n\r\n");
		nc->flags |= MG_F_SEND_AND_CLOSE;
	}
}
std::wstring CHttp::getAppdataPath(){
	TCHAR szBuffer[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szBuffer, CSIDL_APPDATA, FALSE);

	return std::wstring(szBuffer);
}
std::string  CHttp::saveFile(std::string fileName)
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

	std::wstring tmpFolder = appFolder + _T("\\tmp");
	if (!PathFileExists(tmpFolder.c_str()))
	{
		createFileRlt = _wmkdir(tmpFolder.c_str());
	}
	std::string temp = wstringToString(tmpFolder) + "\\" + fileName;
	return temp;
}
std::string CHttp::wstringToString(const std::wstring& wstr)
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
void CHttp::handle_request(struct mg_connection *nc)
{
	// This handler gets for all endpoints but /upload
	mg_printf(nc, "%s",
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Connection: close\r\n"
		"\r\n"
		"<html><body>Upload example."
		"<form method=\"POST\" action=\"/upload\" "
		"  enctype=\"multipart/form-data\">"
		"<input type=\"file\" name=\"file\" /> <br/>"
		"<input type=\"submit\" value=\"Upload\" />"
		"</form></body></html>");
	nc->flags |= MG_F_SEND_AND_CLOSE;
}

CHttp::CHttp()
	: m_bQuit(false)
{
}


void* CHttp::webThread(void* p)
{
	CHttp* pThis = (CHttp*)p;

	struct mg_mgr mgr;
	struct mg_connection *nc;

	mg_mgr_init(&mgr, NULL);
	nc = mg_bind(&mgr, std::to_string(pThis->m_nPort).c_str(), webEvhandler);
	mg_register_http_endpoint(nc, "/upload", handle_upload);
	mg_set_protocol_http_websocket(nc);
	while (!pThis->m_bQuit)
	{
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return 0;
}

static struct mg_str cb(struct mg_connection *c, struct mg_str file_name) {
	// Return the same filename. Do not actually do this except in test!
	// fname is user-controlled and needs to be sanitized.
	return file_name;
}

void CHttp::split(const std::string& s, const std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		std::string tt = s.substr(last, index - last);
		if (!tt.empty())
		{
			ret->push_back(tt);
		}
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}

void CHttp::webEvhandler(struct mg_connection *nc, int ev, void *ev_data)
{
	struct http_message *hm = (struct http_message *) ev_data;
	try
	{
		switch (ev) {
		case MG_EV_HTTP_REQUEST:{
			mg_str scheme, user_info, host, path, query, fragment;
			unsigned int port;

			int ret = mg_parse_uri(hm->uri, &scheme, &user_info, &host, &port, &path, &query, &fragment);
			if (0 != ret)
			{
				throw SchException("parse uri failed");
			}
			std::vector<std::string> pathSections;
			split(std::string(path.p, path.len), "/", &pathSections);

			if (pathSections[0].empty() || (0==pathSections[0].compare("/")))
			{
				handle_request(nc);
				return;
			}
			else if (pathSections[0].compare("images") == 0)
			{
				std::string imagePath=saveFile(pathSections[1]);
				//std::string imagePath = "/tmp/" + pathSections[1];

				std::cout << "in get images section, name:" << imagePath << std::endl;
				mg_http_serve_file(nc, hm, imagePath.c_str(),
					mg_mk_str("image/jpeg"), mg_mk_str(""));
			}
			else
			{
				mg_printf(nc,
					"HTTP/1.1 404 Not Found\r\n");
				nc->flags |= MG_F_SEND_AND_CLOSE;
			}

		}
		break;
		}
	}
	catch (SchException& e)
	{
		mg_printf(nc,
			"HTTP/1.1 500 %s\r\n"
			"Content-Length: 0\r\n\r\n", e.what());
		nc->flags |= MG_F_SEND_AND_CLOSE;
	}
	catch (...)
	{
		mg_printf(nc, "%s",
			"HTTP/1.1 500 Unknown error\r\n"
			"Content-Length: 0\r\n\r\n");
		nc->flags |= MG_F_SEND_AND_CLOSE;
	}
}


CHttp::~CHttp()
{
}

int CHttp::start(int port)
{
	m_nPort = port;

	/*if (pthread_create(&m_hThread, NULL, webThread, this))
	{
		return -1;
	}*/
	CreateThread(NULL, 0, workThread, this, THREAD_PRIORITY_NORMAL, NULL);
	return 0;
}
DWORD WINAPI CHttp::workThread(LPVOID lpParam)
{
	CHttp * p = (CHttp *)(lpParam);
	if (p != NULL)
	{
		p->webThread(lpParam);
	}
	return 1;
}

void CHttp::stop()
{
	m_bQuit = true;
}
