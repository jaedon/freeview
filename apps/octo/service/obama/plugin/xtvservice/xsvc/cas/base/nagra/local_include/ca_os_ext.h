/**
@file				ca_os_ext.h
@brief				ca_os_ext.h (Nagra OS Service)

Description:  \n
				\n
Module: MW/CAS \n
Remarks :\n

Copyright (c) 2008 HUMAX Co., Ltd. \n
All rights reserved. \n
*/

#ifndef CA_OS_EXT_H
#define CA_OS_EXT_H

#include "ca_defs.h"

/******* Timeout value define ********/
#define NO_WAIT 				0
#define WAIT_FOREVER 			((TUnsignedInt32)-1)

/******* Event ********/
#define EVENT_DATA_SIZE		4
#define EVENT_BUF_SIZE			32
#define EVENT_Q_NAME			"EV_Q"

#define STORAGE_ID_0			0
#define STORAGE_ID_1			1
#define STORAGE_ID_2			2
#define STORAGE_ID_3			3
#define STORAGE_ID_4			4
#define STORAGE_ID_5			5

#define CAS_FILE_PERSISTENT_MEM 	"NagraData"

/*************************************************************
	Type definition
*************************************************************/
typedef TUnsignedInt32 QUEUE_HANDLE;


/****** Event ******/
typedef enum
{
	eWait_Any_Event 	= 0x01,
	eWait_All_Event 		= 0x02,
	eEvent_No_Wait		= 0x04,
	eEvent_Wait			= 0x08
} Event_Wait_Option_t;

typedef struct
{
	QUEUE_HANDLE 		hQueue;
	TUnsignedInt32 		ulEvent;
} Event_Instance_t, *Event_Handle_t;


extern Event_Handle_t		CA_NA_OsEventCreate(void);
extern HERROR			CA_NA_OsEventDelete(Event_Handle_t hEvent);
extern HERROR			CA_NA_OsEventSend(Event_Handle_t hEvent, TUnsignedInt32 ulEvent);
extern HERROR			CA_NA_OsEventReceive(Event_Handle_t hEvent, TUnsignedInt32 ulEventMask, TUnsignedInt32 *pulEvent, TUnsignedInt32 ulEventOpt, TUnsignedInt32 ulTimeout);

extern HERROR 			CA_NA_OsFlashRead(TSignedInt32 ulStorageId, TUnsignedInt32 ulOffset, TUnsignedInt32 ulSize, TUnsignedInt8 *pucData);
extern HERROR 			CA_NA_OsFlashWrite(TSignedInt32 ulStorageId, TUnsignedInt32 ulOffset, TUnsignedInt32 ulSize, TUnsignedInt8 *pucData);

extern HERROR 			CA_NA_OsEepromRead(TSignedInt32 ulStorageId, TUnsignedInt32 ulOffset, TUnsignedInt32 ulSize, TUnsignedInt8 *pucData);
extern HERROR 			CA_NA_OsEepromWrite(TSignedInt32 ulStorageId, TUnsignedInt32 ulOffset, TUnsignedInt32 ulSize, TUnsignedInt8 *pucData);

extern HERROR CA_NA_OsCheckEepromVersion(void);
#endif /* CA_OS_EXT_H */

