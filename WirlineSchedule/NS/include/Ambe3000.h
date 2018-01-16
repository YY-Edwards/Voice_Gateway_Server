/*******************************************************************************
*
*                         C++  HEADER  F I L E
*
*            COPYRIGHT 2010 MOTOROLA, INC. ALL RIGHTS RESERVED.
*                    MOTOROLA CONFIDENTIAL RESTRICTED
*
********************************************************************************/
#ifndef AMBE3000_H_
#define AMBE3000_H_
//#include "config.h"

const    char AMBE3000_SYNC_BYTE           = (const char)0x61;
const    char AMBE3000_PARITYTYPE_BYTE     = (const char)0x2F;

const    char AMBE3000_PCM_LENGTH_HBYTE    = (const char)0x01;
const    char AMBE3000_PCM_LENGTH_LBYTE    = (const char)0x44;
const    char AMBE3000_PCM_TYPE_BYTE       = (const char)0x02;
const    char AMBE3000_PCM_SPEECHID_BYTE   = (const char)0x00;
const    char AMBE3000_PCM_NUMSAMPLES_BYTE = (const char)0xA0;
const    int  AMBE3000_PCM_INTSAMPLES_BYTE = (const int )0xA0;

const    char AMBE3000_AMBE_LENGTH_HBYTE   = (const char)0x00;
const    char AMBE3000_AMBE_LENGTH_LBYTE   = (const char)0x0B;
const    char AMBE3000_AMBE_TYPE_BYTE      = (const char)0x01;
const    char AMBE3000_AMBE_CHANDID_BYTE   = (const char)0x01;
const    char AMBE3000_AMBE_NUMBITS_BYTE   = (const char)0x31;
const    char AMBE3000_AMBE_NUMBYTES_BYTE  = (const char)0x07;

const    char AMBE3000_CCP_TYPE_BYTE       = (const char)0x00;
const    char AMBE3000_CCP_ECMODE	       = (const char)0x05;
const    char AMBE3000_CCP_DCMODE          = (const char)0x06;
const    char AMBE3000_CCP_MODE_LENGTHH    = (const char)0x00;
const    char AMBE3000_CCP_MODE_LENGTHL    = (const char)0x05;
const    char AMBE3000_CCP_MODE_NOISEH     = (const char)0x00;
const    char AMBE3000_CCP_MODE_NOISEL     = (const char)0x40;

const    int  AMBE3000_PCM_BYTESINFRAME    = 328;
const    int  AMBE3000_AMBE_BYTESINFRAME    = 15;




#pragma pack(1)
typedef union
{
	struct
	{
		unsigned __int8             Sync;
		unsigned __int8             LengthH;
		unsigned __int8             LengthL;
		unsigned __int8             Type;
		unsigned __int8             ID;
		unsigned __int8             Num;
		unsigned __int8             Samples[320]; //Bytes!
		unsigned __int8             PT;
		unsigned __int8             PP;
	}fld;
	unsigned __int8                 All[328];
}tPCMFrame;

typedef union
{
	struct
	{
		unsigned __int8             Sync;
		unsigned __int8             LengthH;
		unsigned __int8             LengthL;
		unsigned __int8             Type;
		unsigned __int8             ID;
		unsigned __int8             Num;
		unsigned __int8             ChannelBits[7];
		unsigned __int8             PT;
		unsigned __int8             PP;
		unsigned __int8				reserved;
	}fld;
	unsigned __int8                 All[16];
}tAMBEFrame;

typedef union
{
	struct BASE
	{
		unsigned __int8             Sync;
		unsigned __int8             LengthH;
		unsigned __int8             LengthL;
		unsigned __int8             Type;
		unsigned __int8				empty[324];
	}base;

	//PCM Frame; Type = 0x02
	struct PCMTYPE
	{
		tPCMFrame        thePCMFrame;
	}PCMType;

	//AMBE Frame; Type = 0x01
	struct AMBETYPE
	{
		tAMBEFrame      theAMBEFrame;
		unsigned __int8 empty[312];
	}AMBEType;

	unsigned __int8     All[328];

}DVSI3000struct;
#pragma pack()

#endif /*AMBE3000_H_*/
