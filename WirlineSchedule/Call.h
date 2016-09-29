#ifndef CALL_H
#define CALL_H


const int  MAXCALLPARAMS = 28;
const unsigned __int8 DATA_TYPE_PI_HEADER = (const unsigned __int8)0x00;
const unsigned __int8 DATA_TYPE_VOICE_HEADER = (const unsigned __int8)0x01;
const unsigned __int8 DATA_TYPE_VOICE_TERMINATOR = (const unsigned __int8)0x02;
const unsigned __int8 DATA_TYPE_CSBK = (const unsigned __int8)0x03;
const unsigned __int8 DATA_TYPE_DATA_HEADER = (const unsigned __int8)0x06;
const unsigned __int8 DATA_TYPE_UNCONFIRM_DATA_CONT = (const unsigned __int8)0x07;
const unsigned __int8 DATA_TYPE_CONFIRM_DATA_CONT = (const unsigned __int8)0x08;
const unsigned __int8 DATA_TYPE_VOICE = (const unsigned __int8)0x0A;
const unsigned __int8 DATA_TYPE_SYNC_UNDETECT = (const unsigned __int8)0x13;

typedef union
{
	struct
	{
		char                        CallOpcode;                               // 0
		unsigned __int8             CallSequenceNumber;                       // 1
		unsigned __int8             CallPriority;                             // 2
		unsigned __int8             CallControlInformation;                   // 3
		unsigned __int8             RepeaterBurstDataType;                    // 4
		unsigned __int8             ESNLIEHB;                                 // 5
		unsigned __int16            RTPSequenceNumber;     //Host Order       // 6
		unsigned __int32            CallOriginatingPeerID; //Host Order       // 8
		unsigned __int32            CallFloorControlTag;   //Host Order       //12
		unsigned __int32            CallSrcID;             //Host Order 0x3210//16
		unsigned __int32            CallTgtID;             //Host Order 0x3210//20
		unsigned __int32            RTPTimeStamp;          //Host order 0x4321//24
	}fld;
	char                 All[28];
}tCallParams;

class CIPSCCall
{
public:
	CIPSCCall();
	virtual ~CIPSCCall();
	bool     m_ValidInfo;

	unsigned __int32 GetOriginatingPeerID(void);
	unsigned __int8  GetSequenceNumber(void);
	unsigned __int32 GetSrcID(void);
	unsigned __int32 GetTgtID(void);
	unsigned __int8  GetCallOpcode(void);
	void             SetParams(tCallParams* pCall);

private:
	bool        m_CallInProgress;
	tCallParams m_CallParams;
};

#endif