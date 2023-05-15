/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ir_util.c \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#include "octo_common.h"
#include "s3_status.h"

#include "ir_util.h"



/********************************************************************
*	Definitions (for this source file)
********************************************************************/









/********************************************************************
*	Functions
********************************************************************/

HBOOL Is_D29_4 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_INFO_DND) && (stStatus.wStatus == 29) && (stStatus.eSource == 4))
		return TRUE;
	else
		return FALSE;
}

HBOOL Is_D100_8 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_INFO_DND) && (stStatus.wStatus == 100) && (stStatus.eSource == 8))
		return TRUE;

	return FALSE;
}

HBOOL Is_I34_9 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_INFO) && (stStatus.wStatus == 34) && (stStatus.eSource == 9))
		return TRUE;
	else
		return FALSE;
}

HBOOL Is_I07_4 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_INFO) && (stStatus.wStatus == 7) && (stStatus.eSource == 4))
		return TRUE;
	else
		return FALSE;
}

HBOOL Is_E04_4 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_ERROR) && (stStatus.wStatus == 4) && (stStatus.eSource == 4))
		return TRUE;
	else
		return FALSE;
}

HBOOL Is_E05_4 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_ERROR) && (stStatus.wStatus == 5) && (stStatus.eSource == 4))
		return TRUE;
	else
		return FALSE;
}

HBOOL Is_E06_4 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_ERROR) && (stStatus.wStatus == 6) && (stStatus.eSource == 4))
		return TRUE;
	else
		return FALSE;
}

HBOOL Is_D00_4 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_INFO_DND) && (stStatus.wStatus == 0) && (stStatus.eSource == 4))
		return TRUE;
	else
		return FALSE;
}

HBOOL Is_E136orE137_4 (ia_status_st stStatus)
{
	if ((stStatus.eSeverity == STATUS_ERROR) &&  (stStatus.eSource == SOURCE_SMARTCARD) && \
		((stStatus.wStatus == 136) ||(stStatus.wStatus == 137 )) )
		return TRUE;
	else
		return FALSE;
}

char SEVERITY_TO_CHAR (ia_status_severity_e eSeverity)
{
	switch (eSeverity)
	{
		case STATUS_FATAL:		return 'F';
		case STATUS_ERROR:		return 'E';
		case STATUS_WARNING:	return 'W';
		case STATUS_INFO:		return 'I';
		case STATUS_INFO_DND:	return 'D';
		default:				return 'U';
	}
}


