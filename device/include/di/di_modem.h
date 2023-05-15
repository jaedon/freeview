/*******************************************************************************
* File name : di_modem.h
* Author : 
* description :
*
* Copyright (c) 2007 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

#ifndef __DI_MODEM_H__
#define __DI_MODEM_H__

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* humax header files */
#include "htype.h"
#include "di_err.h"

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
typedef enum
{
	DI_MODEM_RX_MSG = 0x0,
	DI_MODEM_TX_DONE,
	DI_MODEM_GET_DCD,
	DI_MODEM_LAST,
}DI_MODEM_EVENT_e;


/*******************************************************************************
* Global variables and structures
*******************************************************************************/
typedef void (*pfnDI_MODEM_Notify)(DI_MODEM_EVENT_e eMdmEvent, HUINT32 ulParam);


/*******************************************************************************
* Function prototype
*******************************************************************************/
/*******************************************************************************
* function : DI_MODEM_Read
* description : read received data from modem
* input : pucData(data buffer), ulDataSize
* output : pucActualReadSize
* return : DI_ERR_CODE
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_MODEM_Read(HUINT8 *pucData, HUINT32 ulDataSize, HUINT32 *pulActualReadSize);

/*******************************************************************************
* function : DI_MODEM_Writes
* description : read received data from modem
* input : pucData(data buffer), ulDataSize
* output : pucActualReadSize
* return : DI_ERR_CODE
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_MODEM_Write(HUINT8 *pucData, HUINT32 ulDataSize, HUINT32 *pulActualWriteSize);

/*******************************************************************************
* function : DI_MODEM_Reset
* description : reset modem 
* input : 
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DI_MODEM_Reset(void);

/*******************************************************************************
* function : DI_Modem_Disconnect
* description : make disconnection 
* input : 
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DI_MODEM_Disconnect(void);

/*******************************************************************************
* function : DI_MODEM_CheckDCD
* description : check dcd from mw
* input : none
* output : pucDcdState
* return : DI_ERR_CODE
* 기타 참고자료 및 파일
*******************************************************************************/
void DI_MODEM_CheckDCD(HUINT8 *pucDcdState);

/********************************************************************
Function   : DI_MODEM_RegisterRxCallback
Description : callback function for Receive data from Modem or DCD status
Input      : none
Output     : none
Return	 : DDI_MICOM_OK

********************************************************************/
void DI_MODEM_RegisterRxCallback(pfnDI_MODEM_Notify pfnNotify);



#endif /* __DI_MODEM_H__ */
