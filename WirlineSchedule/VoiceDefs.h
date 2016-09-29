#ifndef VOICEDEFS_H
#define VOICEDEFS_H


const int IDX_CALLOPCODE                  =  4;
const int IDX_CALLORIGINATINGPEERID       =  8;
const int IDX_CALLSEQUENCENUMBER          =  9;
const int IDX_SRCID                       = 12;
const int IDX_TGTID                       = 15;
const int IDX_CALLPRIORITY                = 16;
const int IDX_CALLFLOORCONTROLTAG         = 20;
const int IDX_CALLFLOORCONTROLINFORMATION = 21;
const int IDX_RTP_VPXCC                   = 22;
const int IDX_RTP_MPT                     = 23;
const int IDX_RTPSEQUENCE                 = 25;
const int IDX_RTPTIMESTAMP                = 29;
const int IDX_RTPSSRC                     = 33;
const int IDX_REPEATERBURSTDATATYPE       = 34;

const int IDX_REPEATERBURSTDATASTATUS     = 35;
const int IDX_LENGTHINWORDS               = 37;
const int IDX_REPEATERBURSTEMBSIGBITS     = 39;
const int IDX_REPEATERBURSTDATASIZE       = 41;
const int IDX_LC_PFFLCO                   = 42;
const int IDX_LC_FID                      = 43;
const int IDX_LC_SERVICEOPTIONS           = 44;
const int IDX_LC_TGTID                    = 47;
const int IDX_LC_SRCID                    = 50;
const int IDX_LC_CRC                      = 53;
const int IDX_RESERVED1                   = 54;
const int IDX_SLOTTYPE                    = 55;
const int IDX_RESERVED2                   = 69;

const int IDX_BURSTLENGTHINBYTES          = 35;
const int IDX_ESNEIEHB                    = 37;
const int IDX_BURSTVOICE                  = 55;

const int IDX_HARDBITS                    = 59;
const int IDX_BCDF_EMB7                   = 60;

const int IDX_LC_COPY                     = 68;
const int IDX_E_EMB7                      = 69;

const unsigned __int8 VOICEHEADER_REPEATER_BURST_DATA_SIZE = 96;


enum IPSCBurstType {  VOICEBURST_A     = 0,
                      VOICEBURST_B     = 1,
					  VOICEBURST_C     = 2,
					  VOICEBURST_D     = 3,
                      VOICEBURST_E     = 4,
                      VOICEBURST_F     = 5,
	                  VOICEHEADER1     = 6,
					  VOICEHEADER2     = 7,
                      VOICETERMINATOR  = 8,
                      PREAMBLECBSK     = 9,
						VOICEHEADER0   = 10 };
 
const int SIZEOFVOICEHEADER              = 52;
const int SIZEOFVOICEBURSTA              = 52;
const int SIZEOFVOICEBURSTBCDF           = 57;
const int SIZEOFVOICEBURSTE              = 66;


typedef union
{
	struct
	{
		        __int32             Length;
		char                        CallOpcode;
		unsigned __int8             CallOriginatingPeerID[4];   //Host order 3210.
		unsigned __int8             CallSequenceNumber;
		unsigned __int8             srcID[3];                   //Host order  210.
		unsigned __int8             tgtID[3];                   //Host order  210.
		unsigned __int8             CallPriority;
		unsigned __int8             CallFloorControlTag[4];     //Host order 3210
		unsigned __int8             CallControlInformation;
		unsigned __int8             RTP_VPXCC;
		unsigned __int8             RTP_MPT;
		unsigned __int8             RTPSequence[2];             //Host order   10
        unsigned __int8             RTPTimeStamp[4];            //Host order 3210
        unsigned __int8             RTPSSRC[4];                 //[Always0x0000]
		unsigned __int8             RepeaterBurstDataType;
	}fld;
	char                            All[35];
}IPSCVoiceCommonField;


typedef union 
{
	struct
	{
		        __int32             Length;
		char                        CallOpcode;
		unsigned __int8             CallOriginatingPeerID[4];   //Host order 3210.
		unsigned __int8             CallSequenceNumber;
		unsigned __int8             srcID[3];                   //Host order  210.
		unsigned __int8             tgtID[3];                   //Host order  210.
		unsigned __int8             CallPriority;
		unsigned __int8             CallFloorControlTag[4];     //Host order 3210
		unsigned __int8             CallControlInformation;
		unsigned __int8             RTP_VPXCC;
		unsigned __int8             RTP_MPT;
		unsigned __int8             RTPSequence[2];             //Host order   10
        unsigned __int8             RTPTimeStamp[4];            //Host order 3210
        unsigned __int8             RTPSSRC[4];                 //[Always0x0000]
		unsigned __int8             RepeaterBurstDataType;

        unsigned __int8             RepeaterBurstDataStatus;
		unsigned __int8             LengthInWords[2];            //Host order   10.
		unsigned __int8             RepeterBurstEmbSigBits[2];   //Host order   10
		unsigned __int8             RepeterBurstDataSize[2];     //Host order   10
		unsigned __int8             LC_PFFLCO;
		unsigned __int8             LC_FID;
		unsigned __int8             LC_ServiceOptions;
		unsigned __int8             LC_tgtID[3];                 //Host order  210.
		unsigned __int8             LC_srcID[3];                 //Host order  210.
		unsigned __int8             LC_CRC[3];                   //Host order  210.
		unsigned __int8             reserved1;
		unsigned __int8             SlotType;
		unsigned __int8             reserved2[14];
	}fld;
	char                            All[70];
}IPSCVoiceControl;


typedef union 
{
	struct
	{
		        __int32             Length;
		char                        CallOpcode;
		unsigned __int8             CallOriginatingPeerID[4];   //Host order 3210.
		unsigned __int8             CallSequenceNumber;
		unsigned __int8             srcID[3];                   //Host order  210.
		unsigned __int8             tgtID[3];                   //Host order  210.
		unsigned __int8             CallPriority;
		unsigned __int8             CallFloorControlTag[4];     //Host order 3210
		unsigned __int8             CallControlInformation;
		unsigned __int8             RTP_VPXCC;
		unsigned __int8             RTP_MPT;
		unsigned __int8             RTPSequence[2];             //Host order   10
        unsigned __int8             RTPTimeStamp[4];            //Host order 3210
        unsigned __int8             RTPSSRC[4];                 //[Always0x0000]
		unsigned __int8             RepeaterBurstDataType;

        unsigned __int8             LengthInBytes;
		unsigned __int8             ESNEIEHB;
		unsigned __int8             Voice[19];
		unsigned __int8             reserved2[14];
	}fld;
	char                            All[70];
}IPSCVoiceBurstA;

typedef union 
{
	struct
	{
		        __int32             Length;//0-4
		char                        CallOpcode;//5
		unsigned __int8             CallOriginatingPeerID[4];   //Host order 3210.//6
		unsigned __int8             CallSequenceNumber;//7
		unsigned __int8             srcID[3];                   //Host order  210.//8-10
		unsigned __int8             tgtID[3];                   //Host order  210.//10-12
		unsigned __int8             CallPriority;//13
		unsigned __int8             CallFloorControlTag[4];     //Host order 3210//14-17
		unsigned __int8             CallControlInformation;//18
		unsigned __int8             RTP_VPXCC;//19
		unsigned __int8             RTP_MPT;
		unsigned __int8             RTPSequence[2];             //Host order   10
        unsigned __int8             RTPTimeStamp[4];            //Host order 3210
        unsigned __int8             RTPSSRC[4];                 //[Always0x0000]
		unsigned __int8             RepeaterBurstDataType;

        unsigned __int8             LengthInBytes;
		unsigned __int8             ESNEIEHB;
		unsigned __int8             Voice[19];

        unsigned __int8             HardBits[4];                 //[Always0x0000]
		unsigned __int8             EMB7;
		unsigned __int8             reserved2[9];
	}fld;
	char                            All[70];
}IPSCVoiceBurstBCDF;

typedef union 
{
	struct
	{
		        __int32             Length;
		char                        CallOpcode;
		unsigned __int8             CallOriginatingPeerID[4];   //Host order 3210.
		unsigned __int8             CallSequenceNumber;
		unsigned __int8             srcID[3];                   //Host order  210.
		unsigned __int8             tgtID[3];                   //Host order  210.
		unsigned __int8             CallPriority;
		unsigned __int8             CallFloorControlTag[4];     //Host order 3210
		unsigned __int8             CallControlInformation;
		unsigned __int8             RTP_VPXCC;
		unsigned __int8             RTP_MPT;
		unsigned __int8             RTPSequence[2];             //Host order   10
        unsigned __int8             RTPTimeStamp[4];            //Host order 3210
        unsigned __int8             RTPSSRC[4];                 //[Always0x0000]
		unsigned __int8             RepeaterBurstDataType;

        unsigned __int8             LengthInBytes;
		unsigned __int8             ESNEIEHB;
		unsigned __int8             Voice[19];

        unsigned __int8             HardBits[4];                 //[Always0x0000]
        unsigned __int8             LC_copy[9];
		unsigned __int8             EMB7;
	}fld;
	char                            All[70];
}IPSCVoiceBurstE;

typedef union
{
	IPSCVoiceControl Control;
	IPSCVoiceBurstA BurstA;
	IPSCVoiceBurstBCDF BurstBCDF;
	IPSCVoiceBurstE	BurstE;
	char RawChar[70];
}IPSCVoiceTemplate;


//SQZ Start ????????????????????
const int MAXCALLSEQUENCE   = 255;

const unsigned __int8 IPSC_HEADER_PRIORITY_VOICE = (const unsigned __int8) 0x02;
const unsigned __int8 IPSC_HEADER_PRIORITY_DATA = (const unsigned __int8) 0x01;
const unsigned __int8 IPSC_HEADER_PRIORITY_EMERGENCY = (const unsigned __int8) 0x03;

const unsigned __int8 IPSC_HEADER_CALL_CONTROL_LAST_PACKET = (const unsigned __int8) 0x40;
const unsigned __int8 IPSC_HEADER_CALL_CONTROL_SLOT2 = (const unsigned __int8) 0x20;

const unsigned __int8 RTP_VPXCC_VALUE = (const unsigned __int8)0x80;
const unsigned __int8 RTP_NON_LAST_PAYLOAD_TYPE = (const unsigned __int8)0x5D;
const unsigned __int8 RTP_LAST_PAYLOAD_TYPE = (const unsigned __int8)0x5E;
const unsigned __int32 RTP_TIMESTAMP_INTERVAL = (const unsigned __int32) 480; // 60ms * 8000 Hz sample rate

const unsigned __int8 RTP_PAYLOAD_VOICE_HEADER_OR_TERMINATOR_LENGTH_IN_WORD_NO_RSSI            = (const unsigned __int8)0x09;
const unsigned __int8 RTP_PAYLOAD_BURST_A_LEN_IN_BYTES = (const unsigned __int8) 0x14;
const unsigned __int8 RTP_PAYLOAD_BURST_BCDF_LEN_IN_BYTES = (const unsigned __int8) 0x19;
const unsigned __int8 RTP_PAYLOAD_BURST_E_LEN_IN_BYTES = (const unsigned __int8) 0x22;

const unsigned __int8 RTP_PAYLOADD_EMBSIGBIT_FIELD_BURST_SOURCE_REPEATER = (const unsigned __int8) 0x10;
const unsigned __int8 RTP_PAYLOADD_EMBSIGBIT_FIELD_SLOT_TYPE_PRESENT = (const unsigned __int8) 0x08;
const unsigned __int8 RTP_PAYLOADD_EMBSIGBIT_FIELD_DATA_SYNC_DETECTED = (const unsigned __int8) 0x02;

const unsigned __int8 RTP_PAYLOAD_VOICE_CONTROL_FIELD_SYNC_DETECTED = (const unsigned __int8) 0x40;
const unsigned __int8 RTP_PAYLOAD_VOICE_CONTROL_FIELD_72_EMB_LC_PRESENT = (const unsigned __int8) 0x10;
const unsigned __int8 RTP_PAYLOAD_VOICE_CONTROL_FIELD_EMB_LC_HARDBITS_PRESENT = (const unsigned __int8) 0x02;
const unsigned __int8 RTP_PAYLOAD_VOICE_CONTROL_FIELD_EMB_PRESENT = (const unsigned __int8) 0x04;
//const unsigned __int8 RTP_PAYLOAD_VOICE_CONTROL_FIELD_SYNC_DETECTED = (const unsigned __int8) 0x40;



const unsigned __int8 LC_STANDARD_FID = (const unsigned __int8) 0x00;
const unsigned __int8 LC_SERVICEOPTION_BROADCAST = (const unsigned __int8) 0x08;
const unsigned __int8 LC_SERVICEOPTION_OVCM      = (const unsigned __int8) 0x04;
const unsigned __int8 LC_SERVICEOPTION_EMERGENCY = (const unsigned __int8) 0x80;




const unsigned __int8 SLOT_TYPE_VOICE_HEADER = (const unsigned __int8) 0x01;
const unsigned __int8 SLOT_TYPE_VOICE_TERMINATOR = (const unsigned __int8) 0x02;

#define ITE_EMBLC_TABLE_SIZE            128



//Hamming (16, 11, 4) matrix that only contains the least 5 significant bits)
static const unsigned __int16 HammingMatrixGenerator[11] ={
	0x13,
	0x1A,
	0x1F,
	0x1C,
	0x0E,
	0x15,
	0x0B,
	0x16,
	0x19,
	0x0D,
	0x07};


typedef union
{
	struct
	{
		unsigned __int8				PRFLCO;					//bit 7: Protect Flag, bit 6: Reserved, bit 5-0: FLCO
		unsigned __int8				FID;					//Feature Set ID
		unsigned __int8				ServiceOptions;
		unsigned __int8				TargetAddress[3];
		unsigned __int8				SourceAddress[3];
	}fld;
	unsigned __int8							All[9];
}LC;



typedef union
{
	struct
	{
		unsigned __int8				BurstB[4];
		unsigned __int8				BurstC[4];
		unsigned __int8				BurstD[4];
		unsigned __int8				BurstE[4];
	}fld;
	unsigned __int8				All[16];
}FECEncodedEmbeddedLC;


typedef struct
{
	 IPSCVoiceCommonField		 CommonField;
	 unsigned __int32            SampleTimeStamp;             //for RTP TimeStamp
	 LC							 LCField;
	 unsigned __int16			 EmbeddedLCCRC;					//5 bit embedded LC CRC, store at the 5 least significant bits
	 FECEncodedEmbeddedLC		 EmbeddedHardBitLC;
} CallRecord;

#endif