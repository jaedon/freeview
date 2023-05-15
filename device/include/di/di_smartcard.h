#ifndef _DI_SMARTCARD_H_
#define _DI_SMARTCARD_H_

/********************************************************************
 Header Files
 ********************************************************************/
#include "di_err.h"

/***************************************************************
  Macro definition
***************************************************************/
/***************************************************************
  Type definition
***************************************************************/

typedef enum 
{
	DI_SC_STD_ISO,			//ISO7816 -> T0 or T1
	DI_SC_STD_CONAX,
	DI_SC_STD_IRDETO,		// T14
	DI_SC_STD_NDS, 		// NDS
	DI_SC_STD_EMV2000
} DI_SC_STD;

typedef enum 
{
	DI_T_0 = 0,	//T0 default is IR
	DI_T_1,		//T1 default is NA

	DI_T_0_IR,
	DI_T_0_CX,
	DI_T_1_NA,
	DI_T_1_ARIB,

	DI_T_14 = 14
} DI_SC_PROTOCOL;

typedef enum 
{
	DI_SC_NOT_PRESENT = 0,
	DI_SC_PRESENT,
	DI_SC_UNKNOWN
} DI_SC_STATUS;

typedef enum 
{
	DI_SC_REMOVED = 0, 
	DI_SC_INSERTED,
	DI_SC_EVENT_MAX
} DI_SC_EVENT;

typedef enum DI_SC_VccLevel {
	DI_SC_VccLevel_e5V = 0,   /* 5v is default value */
	DI_SC_VccLevel_e3V = 1    /* 3v  */  
} DI_SC_VccLevel;

typedef void (*pfnDI_SC_EVT_CALLBACK)(  HUINT32 unDeviceId, DI_SC_STATUS nStatus );


DI_ERR_CODE DI_SC_GetCapability(HUINT32  *pNumDevice);

DI_ERR_CODE DI_SC_NegotiatePTS( HUINT32 unDeviceId,
						unsigned char *pucWriteBuf, int ucNumToWrite,
						unsigned char *pucReadBuf, int *pucNumRead );

DI_ERR_CODE DI_SC_SetParams( HUINT32 unDeviceId, DI_SC_PROTOCOL nProtocol,
						unsigned long ulMinClock, unsigned long ulSrcBaudrate,
						unsigned char ucFI, unsigned char ucDI, unsigned char ucN,
						unsigned char ucCWI, unsigned char ucBWI );

DI_ERR_CODE DI_SC_GetParams( HUINT32 unDeviceId, DI_SC_PROTOCOL *pnProtocol,
						unsigned long *pulClock, unsigned long *pulBaudrate,
						unsigned char *pucFI, unsigned char *pucDI, unsigned char *pucN,
						unsigned char *pucCWI, unsigned char *pucBWI );

DI_ERR_CODE DI_SC_GetCardStatus( HUINT32 unDeviceId, DI_SC_STATUS *pnStatus );

DI_ERR_CODE DI_SC_Reset (HUINT32 unDeviceId, HUINT8 *pucAtr, HUINT32 *pucAtrlen);

DI_ERR_CODE DI_SC_TransferData( HUINT32 unDeviceId,
						unsigned char *pucWriteBuf, int ucNumToWrite,
						unsigned char *pucReadBuf, unsigned int *pulNumRead );

DI_ERR_CODE DI_SC_ReadData ( HUINT32 unDeviceId,
						unsigned char *pucWriteBuf, int ucNumToWrite,
						unsigned char *pucReadBuf, int *pucNumRead );

DI_ERR_CODE DI_SC_WriteData( HUINT32 unDeviceId,
				unsigned char *pucWriteBuf, int ucNumToWrite,
				unsigned char *pucReadBuf, int *pucNumRead );

DI_ERR_CODE DI_SC_RegisterCallback (HUINT32 unDeviceId, pfnDI_SC_EVT_CALLBACK pfnSCCallback);

DI_ERR_CODE DI_SC_SetVccLevel(HUINT32 unDeviceId,DI_SC_VccLevel in_vccLevel);

DI_ERR_CODE DI_SC_DownUpVCC(HUINT32 unDeviceId, unsigned int unDownTime /* ms*/);

DI_ERR_CODE DI_SC_Activate( HUINT32 unDeviceId );
DI_ERR_CODE DI_SC_Deactivate( HUINT32 unDeviceId );

#endif /* _DI_SMARTCARD_H_ */

