#ifndef WLNSRECORDFILE_H
#define WLNSRECORDFILE_H

#define PATH_FILE_MAXSIZE 1024

class NSLog;
class NSAmbe;
class NSManager;
class NSWLPeer;

class  NSRecordFile
{
public:
	NSRecordFile(NSWLPeer* peer);
	~NSRecordFile();
public:
	void WriteVoiceFrame(const char* pAmbe, int size, bool needDongle = true);
	bool TimeOut();
	SYSTEMTIME* StartTime();
	unsigned int Length();;
	void setCallStatus(int value);
	int CallStatus();
	void WriteToDb();
	void setPeer(NSWLPeer* value);

	unsigned long src_radio;
	unsigned long target_radio;
	unsigned char call_type;
	unsigned long src_peer_id;
	int src_rssi;
	int src_slot;
	unsigned long call_id;
	//char SessionId[SESSION_SIZE];
private:
	SYSTEMTIME time;//this time is voice start time
	unsigned int length;
	unsigned long timeout;
	char* buffer;
	int call_status;
	NSLog* m_pLog;
	unsigned int id;
	HANDLE m_hOpenFile;
	wchar_t m_strCurrentFilePath[PATH_FILE_MAXSIZE];
	NSAmbe* m_pAmbe;
	NSWLPeer* m_peer;//此条语音记录的产生者
	//NSManager* m_pManager;
	//char* m_fullBuffer;

	BOOL CreateNewFileByYearMonth();
};
#endif