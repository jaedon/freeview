//#define	CI_HW_TEST
#ifdef	CI_HW_TEST
#else
/********************************************************************
 * $Workfile:   pdriver.c  $
 * Project    : HDPVR
 * Author     :
 * Description:	CI2005
 *
 *                              Copyright (c) 2006 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/****************************************************************************/
/*                                                                          */
/*   FILE:    pdriver.c													*/
/*   DATE:    1998/02/27                                                    */
/*   PURPOSE: DVB-CI PHYSICAL Driver (Host to Module Access)				*/
/*			- DRV_Open														*/
/*			- DRV_Read														*/
/*			- DRV_Write														*/
/*			- DRV_Ioctl														*/
/*			- DRV_Close														*/
/*																			*/
/*   TAB STOPS : 4															*/
/*   HISTORY:				                                    			*/
/*   Version	Date	  	    Written by	     Notes	    	   			*/
/*	 1.0	    06/08/1997      TGA, Teamlog     Initial implementation 	*/
/*	 1.02		15/09/1997		TGA, Teamlog	 I2C modifications			*/
/*		- DRV_Read() : (I2C define Version) If a length equal to 0 is		*/
/*						received, the I2CRead is not called again			*/
/*		- DRV_Read() : (I2C define Version) : cast the received length into	*/
/*						unsigned char										*/
/*		- DRV_Ioctl(): DRV_READ_BUSY check : bits 4 and 5 are tested to 0	*/
/*	 1.03		18/09/1997		TGA, Teamlog	 DRV_READY_BUSY management	*/
/*		- DRV_Ioctl() : DRV_READ_BUSY check : change the check due to new	*/
/*						embedded software									*/
/*						H2M define option for using Host to Module transfer	*/
/*						and Module to Host transfer							*/
/*		- DRV_Read() :  Test the module extraction							*/
/*	 1.04		24/11/1997		TGA, Teamlog	 DRV_CodeLen management		*/
/*						Delete the byte coding the length of the data length*/
/*						in case of data length > 255 bytes					*/
/*	 2.0		1998/02/27		Sang Young, Lee	 Host to Moudle convertion  */
/*						Humax F-1 project									*/
/*                                                                    	    */
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    */
/* This code is only for 2 slots											*/
/*		fd = 0 --> slot 0													*/
/*		fd = 1 --> slot 1													*/
/*									    									*/
/****************************************************************************/

/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/

/* humax di layer */
#include "linden_trace.h"

#include "pdriver.h"
#include "di_ci.h"
#include "drv_ci.h"
#include "vkernel.h"


/*******************************************************************/
/* LOCAL DEFINE 													*/
/*******************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_CI

#define UNREFENCED_PARAM(x) (x=x)
/* I/O REGISTER : see physdrv.h */
#define OFF_DATA_REG		0
#define OFF_COM_REG			1
#define OFF_STA_REG			1
#define OFF_SIZE_LS_REG		2
#define OFF_SIZE_MS_REG		3


/*******************************************************************/
/* EXTERN	 													*/
/*******************************************************************/
extern CAMINFO_t caminfo;


/*******************************************************************/
/* GLOBAL VARIABLES 												*/
/*******************************************************************/


/*******************************************************************/
/* STATIC VARIABLES 												*/
/*******************************************************************/


/*******************************************************************/
/* STATIC FUNCTIONS 												*/
/*******************************************************************/
static int DRV_ReadIo (int nDevId, DRV_stIO * pReg);
static int DRV_WriteIo (int nDevId, DRV_stIO * pReg);
#ifndef CONFIG_DOUGLAS_PRIMITIVE_FUNC
static int DRV_ReadMem (int nDevId, DRV_stMem * pMem);
static int DRV_WriteMem (int nDevId, DRV_stMem * pMem);
#endif
static int DRV_TsSignal (int nDevId, DRV_stSignal * pSig);
static int DRV_SsSignal (int nDevId, DRV_ssSignal * pSig);


/*******************************************************************/
/* FUNCTION DESCRIPTION 										*/
/*******************************************************************/

/* ======================================================================== */
/* NAME : DRV_CI_Open						    								*/
/* 									    									*/
/* DESCRIPTION : open the Driver :					    					*/
/*		There is one call to DRV_Open for each physical slot .				*/
/*		The file descriptor returned by the function identifies a slot		*/
/* 									 										*/
/* INPUT PARAMETERS :							    						*/
/*		device_name :	 Driver Device name									*/
/*									    									*/
/* OUTPUT PARAMETERS :							  							*/
/*									   										*/
/* RETURN CODE								   								*/
/*	    > 0	: file descriptor												*/
/*		 -1 : error															*/
/*									   										*/
/* ======================================================================== */
int DRV_CI_Open (unsigned char *device_name)
{
	UNREFENCED_PARAM(device_name);
	PrintEnter();

	if (caminfo.ucOpen == TRUE)
	{
		PrintError(("CI already opened !!!\n"));
		return -1;
	}

	// CAM 1 개를 전제로, 아래와 같이 운용한다.
	// 2개 이상이 되면, 달라져야 한다.
	caminfo.ucOpen = TRUE;
	caminfo.nDevId = 0;
	caminfo.usSlotId = 0;

	PrintExit();

	return caminfo.usSlotId;
}


/* ======================================================================== */
/* NAME : DRV_CI_Close														*/
/*																			*/
/* DESCRIPTION : close the Driver											*/
/*																			*/
/* INPUT PARAMETERS :														*/
/*		fd	   : file descriptor, ie slot identifier						*/
/*																			*/
/* OUTPUT PARAMETERS :														*/
/*																			*/
/* RETURN CODE																*/
/*	 1 : OK 																*/
/*	-1 : error																*/
/*																			*/
/* ======================================================================== */
int DRV_CI_Close (unsigned short usSlotId)
{
	// 이게 호출된다는 것은 이미 MW CI 스택 동작 중에 복구 불가능한 치명적인 문제가 발생했다는 것이다.
	// 따라서 이 함수의 내용은 크게 중요치 않다.

	int nDevId;

	if ((nDevId = DRV_CI_CheckSlotId (usSlotId)) < 0)
	{
		return -1;
	}

	caminfo.ucOpen 		= FALSE;
	caminfo.ucDetect	= CAM_NO_DETECT;
	caminfo.usStatus	= DI_CI_EXTRACTED;

	return 1;
}


/* ======================================================================== */
/* NAME : DRV_CI_Write														*/
/* 									    									*/
/* DESCRIPTION : Send bytes to the module by the I/O Window					*/
/* 									 										*/
/* INPUT PARAMETERS :							    						*/
/*		fd     : file descriptor, ie slot identifier						*/
/*		len    : number of bytes to send									*/
/*		pbytes : pointer to the bytes to send								*/
/*									    									*/
/* OUTPUT PARAMETERS :							  							*/
/*									   										*/
/* RETURN CODE								   								*/
/*		>= 0 : number of bytes sent											*/
/*		  -1 : error														*/
/*									   										*/
/* ======================================================================== */
int DRV_CI_Write (unsigned short usSlotId, unsigned short len, unsigned char *pbytes)
{
	int i;
	int nDevId;

	if ((nDevId = DRV_CI_CheckSlotId (usSlotId)) < 0)
	{
		return -1;
	}

	for (i=0; i<len; i++)
	{
		if (DRV_CI_WriteIo(nDevId, OFF_DATA_REG, *pbytes++) != 0)
		{
			PrintError(("DRV_CI_WriteIo failed \n"));
			return -1;
		}
	}

	return len;
}


/* ======================================================================== */
/* NAME : DRV_CI_Read						    								*/
/* 									    									*/
/* DESCRIPTION : Read bytes from the module by the I/O Window    			*/
/* 									 										*/
/* INPUT PARAMETERS :							    						*/
/*		fd     : file descriptor, ie slot identifier						*/
/*		len    : number of bytes to read									*/
/*									    									*/
/* OUTPUT PARAMETERS :							  							*/
/*		pbytes : pointer to the bytes to read								*/
/*									   										*/
/* RETURN CODE								   								*/
/*		  >0 : number of received bytes										*/
/*		   0 : nothing received												*/
/*		  -1 : error														*/
/*									   										*/
/* ======================================================================== */
int DRV_CI_Read (unsigned short usSlotId, unsigned short len, unsigned char *pbytes)
{
	int i;
	int nDevId;

	if ((nDevId = DRV_CI_CheckSlotId (usSlotId)) < 0)
	{
		return -1;
	}

	for (i=0; i<len; i++)
	{
		if (DRV_CI_ReadIo(nDevId, OFF_DATA_REG, pbytes) != 0)
		{
			PrintError(("DRV_CI_ReadIo failed \n"));
			return -1;
		}
		pbytes++;
	}

	return len;
}


/* ======================================================================== */
/* NAME : DRV_CI_Ioctl							    							*/
/* 									    									*/
/* DESCRIPTION : Execute special commands									*/
/* 									 										*/
/* INPUT PARAMETERS :							    						*/
/*		fd     : file descriptor, ie slot identifier						*/
/*		command: special command											*/
/*		parg   : pointer to parameters depending of the command				*/
/*									    									*/
/* OUTPUT PARAMETERS :							  							*/
/*									   										*/
/* RETURN CODE								   								*/
/*	 1 : OK																	*/
/*	-1 : error																*/
/*																			*/
/* ======================================================================== */
int DRV_CI_Ioctl (unsigned short usSlotId, int command, void *parg)
{
	int nDevId;

	if ((nDevId = DRV_CI_CheckSlotId (usSlotId)) < 0)
	{
		return -1;
	}

//	PrintDebug(("DRV_CI_Ioctl command %d \n", command));

	switch (command)
	{
		case DRV_READIO:
			return DRV_ReadIo (nDevId, (DRV_stIO *)parg);

		case DRV_WRITEIO:
			return DRV_WriteIo (nDevId, (DRV_stIO *)parg);

#ifndef CONFIG_DOUGLAS_PRIMITIVE_FUNC

		case DRV_READMEM:
			return DRV_ReadMem (nDevId, (DRV_stMem *)parg);

		case DRV_WRITEMEM:
			return DRV_WriteMem (nDevId, (DRV_stMem *)parg);

#endif

		case DRV_TSIGNAL:
			return DRV_TsSignal (nDevId, (DRV_stSignal *)parg);

		case DRV_SSIGNAL:
			return DRV_SsSignal (nDevId, (DRV_ssSignal *)parg);

		case DRV_ADDR:
			/* useless with Set-Top Box */
			return 1;

		default:
			PrintError(("DRV_CI_Ioctl : unknown !!! \n"));
			return -1;		/* error */
	}

	return -1;	/* error */
}


/*******************************************************************/
// Function Name:	DRV_ReadIo
// Description:
// Parameter:		nSlotId, * pReg
// Return Value:
/*******************************************************************/
static int DRV_ReadIo(int nDevId, DRV_stIO * pReg)
{

	if (DRV_CI_ReadIo(nDevId, pReg->registr, pReg->pvalue) != 0)
	{
		return -1;
	}

	return 1;		/* Success */
}


/*******************************************************************/
// Function Name:	DRV_WriteIo
// Description:
// Parameter:		nSlotId, * pReg
// Return Value:
/*******************************************************************/
static int DRV_WriteIo(int nDevId, DRV_stIO * pReg)
{
	if (DRV_CI_WriteIo(nDevId, pReg->registr, *pReg->pvalue) != 0)
	{
		return -1;
	}

	return 1;		/* Success */
}


#ifndef CONFIG_DOUGLAS_PRIMITIVE_FUNC

/*******************************************************************/
// Function Name:	DRV_ReadMem
// Description:
// Parameter:		nSlotId, * pMem
// Return Value:
/*******************************************************************/
static int DRV_ReadMem (int nDevId, DRV_stMem * pMem)
{
	unsigned char *pucAddr;
	unsigned char *pucBytes;
	int i;

	pucAddr = (unsigned char *)(unsigned long)pMem->addr;
	pucBytes = pMem->pbytes;

	for (i=0; i<pMem->len; i++)
	{
		DRV_CI_ReadMem(nDevId, (unsigned long)pucAddr, pucBytes);

		pucBytes++;
		pucAddr++;
	}

	pMem->rlen = pMem->len;

	return 1;		/* Success */
}


/*******************************************************************/
// Function Name:	DRV_WriteMem
// Description:
// Parameter:		nSlotId, * pMem
// Return Value:
/*******************************************************************/
static int DRV_WriteMem (int nDevId, DRV_stMem * pMem)
{
	unsigned char *pucAddr;
	unsigned char *pucBytes;
	//int nAccessMode;
	int i;

	pucAddr = (unsigned char *)(unsigned long)pMem->addr;
	pucBytes = pMem->pbytes;

	for (i=0; i<pMem->len; i++)
	{
		DRV_CI_WriteMem(nDevId, (unsigned long)pucAddr, *pucBytes);

		pucBytes++;
		pucAddr++;

	}

	pMem->rlen = pMem->len;

	return 1;			/* Success */
}

#else /* for douglas */

/*****************************************************************
 * Function : DRV_CI_ReadCIS
 * Description	: 캠의 CIS(Card Information Structure) 정보를 가져온다.
 * Inputs	: usSlotId=CI Slot ID
 * Outputs	: *pcis=CIS 정보
 * Returns	: 1=read success,	-1=read fail
 *****************************************************************/
int DRV_CI_ReadCIS (unsigned short usSlotId, unsigned char *pcis, unsigned short *plen)
{
	// BCM 에서는 지원하지 않는 함수임.
	return DI_ERR_ERROR;
}


/*****************************************************************
 * Function : DRV_CI_WriteCOR
 * Description	: 캠의 CIS(Card Information Structure) 를 읽어 DVB CI 인지 체크 후
 *				COR(Configuration Option Register) 를 초기화하여 CI Module 로써 동작하도록 한다.
 * Inputs	: usSlotId=CI Slot ID
 * Outputs
 * Returns	: 1=write success,	-1=write fail
 *****************************************************************/
int DRV_CI_WriteCOR (unsigned short usSlotId)
{
	// BCM 에서는 지원하지 않는 함수임.
	return DI_ERR_ERROR;
}

#endif


/**********************************************************************
* int DRV_TsSignal(int fd, DRV_stSignal * pSig)
*
* Desc. :
* Param :
* Rtn : 	1 : success, -1 : fail
* Err Code :
**********************************************************************/
static int DRV_TsSignal(int nDevId, DRV_stSignal * pSig)
{
	switch(pSig->sig)
	{
	case DRV_CARD_DETECT :
		if (DRV_CI_GetCAMDetect(nDevId))
		{
			//PrintDebug(("Card detect...\n"));
			pSig->value	= 1;
		}
		else
		{
			//PrintDebug(("No Card...\n"));
			pSig->value = 0;			/* No Card Detect */
		}
		return 1;		/* Success */

	case DRV_READY_BUSY :
		if ((DRV_CI_GetCAMDetect(nDevId) == CAM_DETECT) && (caminfo.usStatus == DI_CI_ENABLED))
		{
			//PrintDebug(("PC Card[%d] Ready\n", nDevId));
			pSig->value = 1;
		}
		else
		{
			//PrintDebug(("PC Card[%d] not Ready\n", nDevId));
			pSig->value = 0;
		}
		return 1;		/* Success */

	default :
		PrintError(("DRV_TsSignal : unknown !!! \n"));
		return -1;		/* unknown signal */
	}
}


/**********************************************************************
* int DRV_SsSignal(int fd, DRV_ssSignal * pSig)
*
* Desc. : 	write cam memory
* Param :
* Rtn : 	1 : success, -1 : fail
* Err Code :
**********************************************************************/
static int DRV_SsSignal(int nDevId, DRV_ssSignal * pSig)
{
	UNREFENCED_PARAM(nDevId);
	switch (pSig->pin)
	{
	case DRV_EMSTREAM:
		if (pSig->cs)	// case : set
		{
			// enable stream
			;//PrintDebug(("DRV_SsSignal()--->nDevId:%d, DRV_EMSTREAM, set...\n", nDevId));
		}
		else	// case : clear
		{
			// disable stream
			;//PrintDebug(("DRV_SsSignal()--->nDevId:%d, DRV_EMSTREAM, clear..\n", nDevId));
		}

		return 1;

	case DRV_RSTSLOT:
		if (pSig->cs)	// set reset
		{
			;//PrintDebug(("DRV_SsSignal()--->nDevId:%d, DRV_RSTSLOT, set...\n", nDevId));
		}
		else	// clear reset
		{
			;//PrintDebug(("DRV_SsSignal()--->nDevId:%d, DRV_RSTSLOT, clear...\n", nDevId));
		}

		return 1;

	case DRV_ADDR:
		/* useless with Set-Top Box */
		return 1;

	default:
		PrintError(("DRV_SsSignal : unknown !!! \n"));
		return -1;
	}

	return 1;	/* success */
}

/*End Of File*/

#endif



