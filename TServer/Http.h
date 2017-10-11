#pragma once
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <vector>


class CHttp
{
	class CUploadFile
	{
	public:
		CUploadFile() :m_pfTmpFile(0), m_nWriteLen(0){}
	public:
		FILE* m_pfTmpFile;
		int m_nWriteLen;
		std::string m_strFileName;
		int m_nState;
	};
private:
	std::map<std::string, CUploadFile*> m_mpClientUploadFiles;

	friend class std::shared_ptr<CHttp>;
	static std::shared_ptr<CHttp> m_instance;
	int m_nPort;
	bool m_bQuit;
	//pthread_t m_hThread;
	static std::string saveFile(std::string fileName);
	static std::wstring getAppdataPath();
	static std::string wstringToString(const std::wstring& wstr);
	static DWORD WINAPI workThread(LPVOID lpParam);
private:  // web actions
	static void handle_upload(struct mg_connection *nc, int ev, void *p);
	static void handle_request(struct mg_connection *nc);

private:
	CHttp();
	static void* webThread(void* p);
	static void webEvhandler(struct mg_connection *nc, int ev, void *ev_data);
	static void split(const std::string& s, const std::string& delim, std::vector< std::string >* ret);

public:
	~CHttp();
	static CHttp* getInstance(){
		static std::mutex locker;

		if (NULL == m_instance.get())
		{
			std::lock_guard<std::mutex> lock(locker);

			if (NULL == m_instance.get())
			{
				m_instance.reset(new CHttp());
			}
		}

		return m_instance.get();
	}

public:
	int start(int port=8001);
	void stop();
};

