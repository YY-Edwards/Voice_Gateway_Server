#include "stdafx.h"
#include "Call.h"

CIPSCCall::CIPSCCall()
{
	m_ValidInfo = FALSE;
	m_CallInProgress = FALSE;
}

CIPSCCall::~CIPSCCall()
{
	//m_report = NULL;
}

unsigned __int32 CIPSCCall::GetOriginatingPeerID(void)
{
	return m_CallParams.fld.CallOriginatingPeerID;
}

unsigned __int8 CIPSCCall::GetSequenceNumber(void)
{
	return m_CallParams.fld.CallSequenceNumber;
}

unsigned __int32 CIPSCCall::GetSrcID(void)
{
	return m_CallParams.fld.CallSrcID;
}

unsigned __int32 CIPSCCall::GetTgtID(void)
{
	return m_CallParams.fld.CallTgtID;
}

unsigned __int8 CIPSCCall::GetCallOpcode(void)
{
	return m_CallParams.fld.CallOpcode;
}

void CIPSCCall::SetParams(tCallParams* pCall)
{
	int i;

	m_ValidInfo = TRUE;
	for (i = 0; i < MAXCALLPARAMS; i++){
		m_CallParams.All[i] = pCall->All[i];
	}
}
