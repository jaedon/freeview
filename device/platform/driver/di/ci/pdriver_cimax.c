//#define	CI_HW_TEST
#ifdef	CI_HW_TEST
#else
/********************************************************************
 * $Workfile:	pdriver_cimax.c  $
 * Project 	:	HDPVR
 * Author		:
 * Description:	CIMAX
 *
 *                              Copyright (c) 2011 by HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/****************************************************************************/
/*                                                                          								*/
/*   FILE:    pdriver_cimax.c														*/
/*   DATE:    2011/12/13                                                    							*/
/*                                                                    	    								*/
/*--------------------------------------------------------------------------*/
/*   GENERAL COMMENTS                                                 	    						*/
/* This code is only for 2 slots													*/
/*		fd = 0 --> slot 0														*/
/*		fd = 1 --> slot 1														*/
/*									    										*/
/****************************************************************************/

/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/

/* humax di layer */
#include "linden_trace.h"

#include "pdriver_cimax.h"
#include "di_ci.h"
#include "drv_cimax.h"
#include "vkernel.h"


/*******************************************************************/
/* LOCAL DEFINE 													*/
/*******************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_CI

#define UNREFENCED_PARAM(x) (x=x)
#define	MAX_CI_SLOT		2


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
int DRV_CI_Open (unsigned char *device_name, unsigned short *slotId)
{
	unsigned char index = 0;
	UNREFENCED_PARAM(device_name);
	PrintEnter();

	for(index = 0; index < 2; index++)
	{
		if (caminfo.ucOpen[index] == FALSE)
		{
			break;
		}
	}

	if(index != 2)
	{
		caminfo.ucOpen[index] = TRUE;
		caminfo.nDevId[index] = 0;
		caminfo.usSlotId[index] = index;
		*slotId = index;

		return 0;
	}
	else
		return -1;

	PrintExit();

}

/* ======================================================================== */
/* NAME : DRV_CI_Each_Open						    								*/
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
int DRV_CI_Each_Open (unsigned short slotId)
{
	PrintEnter();

	if(slotId >= CIMAX_CAM_MAX_SLOT_NUM)
		return -1;

	if (caminfo.ucOpen[slotId] == FALSE)
	{
		caminfo.ucOpen[slotId] = TRUE;
		caminfo.nDevId[slotId] = 0;
		caminfo.usSlotId[slotId] = slotId;
		return 0;
	}
	else
		return -1;

	PrintExit();

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

	caminfo.ucOpen[usSlotId] = FALSE;
	caminfo.ucDetect[usSlotId]	= CAM_NO_DETECT;
	caminfo.usStatus[usSlotId]	= DI_CI_EXTRACTED;

	return 1;
}

/*****************************************************************
 * Function	: DRV_CI_ResetCAM
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
int DRV_CI_ResetCAM (unsigned short slotId)
{
	int ret = 0, nDevId = 0;

	if ((nDevId = DRV_CI_CheckSlotId (slotId)) < 0)
	{
		return -1;
	}
	
	ret = DRV_CIMAX_CAMReset(slotId);

	return ret;
}


/*****************************************************************
 * Function	: DRV_CI_Get_CIS
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
int DRV_CI_Get_CIS (unsigned short slotId, unsigned char *cisBuff, unsigned short *cisLen)
{
	int ret = 0, nDevId = 0;

	if ((nDevId = DRV_CI_CheckSlotId (slotId)) < 0)
	{
		return -1;
	}

	if((cisBuff == NULL) || (cisLen == NULL))
	{
		return -1;
	}
		
	ret = DRV_CIMAX_GetCIS(slotId,cisBuff,cisLen);

	return ret;
}


/*****************************************************************
 * Function	: DRV_CI_Write_COR
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
int DRV_CI_Write_COR (unsigned short slotId, unsigned short coraddr, unsigned char cordata)
{
	int ret = 0, nDevId = 0;

	if ((nDevId = DRV_CI_CheckSlotId (slotId)) < 0)
	{
		return -1;
	}

	ret = DRV_CIMAX_WriteCOR(slotId,coraddr,cordata);
	
	return ret;

}


/*****************************************************************
 * Function	: DRV_CI_Negociation
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
int DRV_CI_Negociation (unsigned short slotId, unsigned short bufferSize, unsigned short *negociatedbufferSize)
{
	int ret = 0, nDevId = 0;

	if ((nDevId = DRV_CI_CheckSlotId (slotId)) < 0)
	{
		return -1;
	}

	ret = DRV_CIMAX_Negotiate(slotId,bufferSize,negociatedbufferSize);

	return ret;
}


/*****************************************************************
 * Function	: DRV_CI_WRITE_LPDU
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
 
int DRV_CI_Write_LPDU (unsigned short slotId, unsigned char *data, unsigned short dataSize)
{
	int ret = 0, nDevId = 0;

	if ((nDevId = DRV_CI_CheckSlotId (slotId)) < 0)
	{
		return -1;
	}

	if((data == NULL) || (dataSize == 0))
	{
		return -1;
	}

	ret = DRV_CIMAX_WriteLPDU(slotId, data, dataSize);

	return ret;
}


/*****************************************************************
 * Function	: DRV_CI_READ_LPDU
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
int DRV_CI_Read_LPDU (unsigned short slotId, unsigned char *data, unsigned short *dataSize)
{
	int ret = 0, nDevId = 0;

	if ((nDevId = DRV_CI_CheckSlotId (slotId)) < 0)
	{
		return -1;
	}

	if((data == NULL) || (dataSize == NULL))
	{
		return -1;
	}

	ret = DRV_CIMAX_ReadLPDU(slotId, data, dataSize);


	return ret;
}

/*****************************************************************
 * Function	: DRV_CI_DataAvailable
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
int DRV_CI_DataAvailable(unsigned short slotId)
{
	int ret = 0,result  = 0;

	ret = DRV_CIMAX_DataAvailable(slotId,&result);
	if(ret != 0)
		return ret;
	else
	{
		if(result&0x80)
			return 1;
		else
			return 0;
	}

}



/**********************************************************************
* int DRV_TsSignal(int fd, DRV_stSignal * pSig)
*
* Desc. :
* Param :
* Rtn : 	1 : success, -1 : fail
* Err Code :
**********************************************************************/
int DRV_TsSignal( unsigned short slotId , unsigned int *result )
{
	int ret = 0, nDevId = 0;

	if ((nDevId = DRV_CI_CheckSlotId (slotId)) < 0)
	{
		return -1;
	}

	if(result == NULL)
	{
		return -1;
	}

	ret = DRV_CIMAX_CAMDetected(slotId,result);

	return ret;
}


/*End Of File*/

#endif




