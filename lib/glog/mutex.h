#pragma once

#include <Windows.h>

namespace XyLib{

	class CMutex{
	private:
		HANDLE		m_mutex;

	public:
		CMutex(){
			m_mutex = CreateMutex(NULL, FALSE, NULL);
		}

		~CMutex(){
			CloseHandle(m_mutex);
		}

		void Lock(){
			WaitForSingleObject(m_mutex, INFINITE);
		}

		void UnLock(){
			ReleaseMutex(m_mutex);
		}
	};

}