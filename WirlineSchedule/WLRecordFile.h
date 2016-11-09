#ifndef WLRECORDFILE_H
#define WLRECORDFILE_H
#define MAX_RECORD_BUFFER_SIZE (100*1024)
class  CRecordFile
{
public:
	CRecordFile();
	~CRecordFile();
public:
	void WriteVoiceFrame(char* pFrame, int len = 7, bool bLocalRequest = false);
	void setBoolPlay(bool value);
	void setBoolReport(bool value);
	void setBoolLogicProcess(bool value);
	bool getBoolPlay();
	bool getBoolReport();
	bool getBoolLogicProcess();
	
public:
	char buffer[MAX_RECORD_BUFFER_SIZE];
	int lenght;
	unsigned long srcId;
	unsigned long tagetId;
	unsigned long originalPeerId;
	unsigned long callId;
	unsigned char callType;
	int srcSlot;
	int srcRssi;
	unsigned long prevTimestamp;
	int callStatus;
	SYSTEMTIME recordTime;
private:
	bool m_bPlay;
	bool m_bReport;
	bool m_bLogicProcess;
};
#endif