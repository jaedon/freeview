/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory System Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_CI_H_
#define _FDI_CI_H_

/******************************************************************************
* header and definitions
******************************************************************************/

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef struct 
{
	unsigned char *next;					/* point the next saved LPDU	*/
	unsigned short length;					/* length of saved LPDU 		*/
	unsigned char pLPDU[1]; 				/* start of saved LPDU buffer	*/
} FDI_CI_SLPDU_t;

typedef struct 
{
	unsigned short Slot_id; 			/* slot identifier				*/
	unsigned short Ci_plus; 			/* CI+ CAM detected 			*/
	unsigned short Negociated_size; 	/* physical buffer size 		*/
	short State;							/* physical state				*/
	FDI_CI_SLPDU_t *firstSLPDU;						/* head of the linked list of received LPDUs */
} FDI_CI_Phys_t;


typedef enum
{
	FDI_CI_EXTRACTED = 0,	/* ķ�� ���ŵ� ��� �Ǵ� ķ ����� power off �� ��� */
	FDI_CI_INSERTED, 		/* ķ�� ���Ե� ��� */
	FDI_CI_ENABLED			/* ķ�� ���ԵǾ� reset �� access �����ϰ� �� ��� (DI ���������θ� ���) */
} FDI_CI_STATE;

typedef void (*FDI_CI_CallbackFunc)(unsigned short usSlotId, unsigned short usFlag);

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */


/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FDI_ERR_CODE FDI_CI_Open(unsigned long ulSlotId);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FDI_CI_STATE FDI_CI_CheckModule(unsigned long ulSlotId);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FDI_ERR_CODE FDI_CI_ReadMem(unsigned long ulSlotId, 
							unsigned long ulAddr, 
							unsigned long ulLen, 
							unsigned char *pucBuf);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FDI_ERR_CODE FDI_CI_WriteMem(unsigned long ulSlotId, 
								unsigned long ulAddr, 
								unsigned long ulLen, 
								unsigned char *pucBuf);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FDI_ERR_CODE FDI_CI_ReadIo(unsigned long ulSlotId, unsigned long ulReg, unsigned char *pucValue);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FDI_ERR_CODE FDI_CI_WriteIo(unsigned long ulSlotId, unsigned long ulReg, unsigned char *pucValue);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FDI_ERR_CODE FDI_CI_Close(unsigned long ulSlotId);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* ��Ÿ �����ڷ� �� ����
*******************************************************************************/
FDI_ERR_CODE FDI_CI_RegisterCallback(FDI_CI_CallbackFunc pfnCallback);

#endif

