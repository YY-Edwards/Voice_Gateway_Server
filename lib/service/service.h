#pragma once

#include <mutex>
#include <functional>
#include <memory>

class CService{
private:
	CService();
	~CService();

	friend class std::auto_ptr<CService>;
	static std::auto_ptr<CService> m_instance;
public:
	static CService* instance(){
		static std::mutex locker;

		if (NULL == m_instance.get())
		{
			std::lock_guard<std::mutex> lock(locker);

			if (NULL == m_instance.get())
			{
				m_instance.reset(new CService());
			}
		}

		return m_instance.get();
	}

public:
	SERVICE_STATUS_HANDLE m_StatusHandle;
	SERVICE_STATUS m_ServiceStatus;
	HANDLE m_ServiceStopEvent;
	HANDLE m_ServiceStoppedEvent;
	TCHAR m_szServiceName[300];
	TCHAR m_szServiceDescription[1024];
	TCHAR m_szServiceRun[300];
	std::function<void(void)> m_fnServiceCode;
	std::function<void(bool)> m_radioUsb;
	BOOL m_bServiceStopped;

public:
	void SetServiceNameAndDescription(LPCTSTR pServiceName, LPCTSTR pServiceDescription);
	void InstallService();
	void UninstallService();
	void RunService();
	void StartWindowsService();
	void StopService();
	void SetServiceCode(std::function<void(void)> fn);
	void SetRadioUsb(std::function < void(bool)>radioUsb);
	static DWORD WINAPI ServiceCtrlHandler(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext);
	static DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);
	static VOID WINAPI ServiceMain(DWORD argc, LPTSTR *argv);
};