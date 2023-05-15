/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory System Driver Interface
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <string.h>
#include <ctype.h>

/* chip manufacture's  header files */

/* humax header files */
#include "ftype.h"
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_app.h"
#include "fai_tmd.h"
#include "fdi_err.h"
#include "fdi_ci.h"
#include "fdi_panel.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_CI_MAX_NUM 	2

#define	DRV_IDLE		1
#define	DRV_CONNECT		2
#define	DRV_DISCONNECT	3

#define MAXLNSIZE		1024 

#define	OK				0x00
#define	W_SEACH			0x01
#define	M_BUSY			0x02
#define	M_DATA			0x03
#define	T_ERR			0x04
#define	RS_ERR			0x05
#define	MEM_ERR			0x06
#define KO				0x07
#define	R_IIR			0x08	// Reset request using the IIR bit from CI+ CAM

#define DRV_MEMADDR		0							  /* Base Memory Address	*/
#define DRV_CISLEN		512							  /* CIS size in bytes		*/

/* Special commands for DRV_Ioctl */
#define DRV_ADDR		1						  /* Set I/O Base Address   */
#define DRV_READMEM		2						  /* Read memory			*/
#define DRV_WRITEMEM	3						  /* Write memory			*/
#define DRV_READIO		4						  /* Read a I/O Register	*/
#define DRV_WRITEIO		5						  /* Write a I/O Register	*/
#define DRV_TSIGNAL		6						  /* Check a Signal			*/
#define DRV_SSIGNAL		7						  /* Set / Clear a Signal	*/

/* Signal number for DRV_TSIGNAL command */
#define DRV_CARD_DETECT	1
#define DRV_READY_BUSY	2

/* Signal number for DRV_SSIGNAL command */
#define DRV_EMSTREAM	0
#define DRV_ENSLOT		1
#define DRV_RSTSLOT		2
#define DRV_IOMEM		6
#define DRV_SLOTSEL		7

/* physical timeout and delay ***********************************************/
#define	PHYS_TIMEOUT	1000	/* physical timeout count */
#define	PHYS_DELAY		20		/* physical delay (in ms) : 10 -> 20 (캠이 busy한 상태에서 빨리 retry하는 것이 안좋을 것 같다. 위의 PHYS_TIMEOUT 값은 그대로 유지하므로 대기 시간은 2배가 된다) */

/* BIT MASKS ****************************************************************/
#define	BIT0	0x01
#define	BIT1	0x02
#define	BIT2	0x04
#define	BIT3	0x08
#define	BIT4	0x10
#define BIT5	0x20
#define	BIT6	0x40
#define	BIT7	0x80

/* Command Register *********************************************************/
#define	HC		BIT0
#define	SW		BIT1
#define	SR		BIT2
#define	RS		BIT3
#define FRIE	BIT6
#define DAIE	BIT7

/* Status Register and masks ************************************************/
#define	RE		BIT0
#define	WE		BIT1
#define	FR		BIT6
#define	DA		BIT7
	
#define	STATUS_MASK		0xC3
#define	MS_SIZE_MASK	0x07

/* I/O REGISTERS ************************************************************/
#define	OFF_DATA_REG	0
#define	OFF_COM_REG		1
#define	OFF_STA_REG		1
#define	OFF_SIZE_LS_REG	2
#define	OFF_SIZE_MS_REG	3

#define INVALID_CI_SLOT_ID	(0xFFFF)

#define PH_INSERT		0x01
#define PH_DATA			0x02
#define PH_ERROR		0x03
#define PH_EXTRACT		0x04

/* Layer 1 - Basic Compatability Tuples */
#define	CISTPL_NULL				0x00	/* null tuple - ignore */
#define	CISTPL_DEVICE				0x01	/* device information */
#define	CISTPL_LONGLINK_CB		0x02	/* longlink to next tuple chain */
#define	CISTPL_CONFIG_CB			0x04	/* configuration tuple */
#define	CISTPL_CFTABLE_ENTRY_CB	0x05	/* configuration table entry */
#define	CISTPL_LONGLINK_MFC		0x06	/* multi-function tuple */
#define	CISTPL_BAR					0x07	/* Base Address Register definition */
#define	CISTPL_CHECKSUM			0x10	/* checksum control */
#define	CISTPL_LONGLINK_A			0x11	/* long-link to AM */
#define	CISTPL_LONGLINK_C			0x12	/* long-link to CM */
#define	CISTPL_LINKTARGET			0x13	/* link-target control */
#define	CISTPL_NO_LINK			0x14	/* no-link control */
#define CISTPL_VERS_1				0x15	/* level 1 version information */
#define	CISTPL_ALTSTR				0x16	/* alternate language string */
#define	CISTPL_DEVICE_A			0x17	/* AM device information */
#define	CISTPL_JEDEC_C			0x18	/* JEDEC programming info for CM */
#define	CISTPL_JEDEC_A			0x19	/* JEDEC programming info for AM */
#define CISTPL_CONFIG				0x1a	/* configuration */
#define CISTPL_CFTABLE_ENTRY		0x1b	/* configuration-table-entry */
#define CISTPL_DEVICE_0C			0x1c	/* other op conditions CM device info */
#define CISTPL_DEVICE_0A			0x1d	/* other op conditions AM device info */
#define	CISTPL_DEVICEGEO			0x1e	/* Common Memory device geometry */
#define	CISTPL_DEVICEGEO_A		0x1f	/* Attribute Memory device geometry */
#define CISTPL_MANFID				0x20	/* manufacturer identification */
#define	CISTPL_FUNCID				0x21	/* function identification */
#define	CISTPL_FUNCE				0x22	/* function extension */
/* Layer 2 - Data Recording Format Tuples */
#define	CISTPL_SWIL				0x23	/* software interleave */
#define	CISTPL_VERS_2				0x40	/* level 2 version information */
#define	CISTPL_FORMAT				0x41	/* Common Memory recording format */
#define	CISTPL_GEOMETRY			0x42	/* geometry */
#define	CISTPL_BYTEORDER			0x43	/* byte order */
#define	CISTPL_DATE				0x44	/* card initialization date */
#define	CISTPL_BATTERY			0x45	/* battery replacement date */
#define	CISTPL_FORMAT_A			0x47	/* Attribute Memory recording format */
/* Layer 3 - Data Organization Tuples */
#define	CISTPL_ORG					0x46	/* organization */
/* Layer 4 - System Specific Standard Tuples */
#define	CISTPL_SPCL				0x90	/* special-purpose tuple */
#define CISTPL_END					0xff	/* end-of-list tuple */

/*--- subtuples ---*/

#define	CCST_CIF			0xc0
#define	STCE_EV			0xc0
#define	STCE_PD			0xc1

/*--- offsets in tuple parameters (without the 2 first bytes tuple and link) ---*/

/* CISTPL_VERS_1 */
#define TPLLV1_MAJOR		0
#define TPLLV1_MINOR		1
#define TPLLV1_INFO		2

/* CISTPL_CONFIG */
#define TPCC_SZ			0
#define TPCC_LAST			1
#define TPCC_RADR			2

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_INDX			0
#define TPCE_IF			1
#define TPCE_FS			2
#define TPCE_PD			3

/*--- bit masks ---*/

/* CISTPL_CONFIG */
#define TPCC_RASZ			0x03
#define TPCC_RMSZ			0x3c

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_INDX_MASK	0xc0
#define TPCE_INTFACE		0x80
#define TPCE_FS_MASK		0x0b
#define TPCE_FS_POWER		0x03
#define TPCE_PD_MASK		0x01
#define TPCE_EXT			0x80
#define TPCE_TIMING		0x04
#define TPCE_TIMING_WAIT	0x03
#define TPCE_TIMING_READY	0x1c
#define TPCE_IO			0x08
#define TPCE_IO_RANGE		0x80
#define TPCE_IO_SLN		0xc0
#define TPCE_IO_SLNR		0x06
#define TPCE_IO_SAN		0x30
#define TPCE_IO_SANR		0x04
#define TPCE_IR			0x10
#define TPCE_IR_MASK		0x10
#define TPCE_MEM			0x60
#define TPCE_MEM_SL		0x18
#define TPCE_MEM_SLR		0x03
#define TPCE_MEM_SCA		0x60
#define TPCE_MEM_SCAR		0x05
#define TPCE_MEM_HPA		0x80

/*--- values ---*/

/* CISTPL_VERS_1 */
#define TPLLV1_MAJOR_VAL	0x05
#define TPLLV1_MINOR_VAL	0x00

/* CISTPL_CONFIG */
#define TPCC_RMSK			0x01
#define NBRBYTES			0x01
#define MAXADDR			0xffe

/* CISTPL_CFTABLE_ENTRY */
#define TPCE_IF_VAL		0x04
#define DVB_INDEX			0x0f

/* CISTPL_NO_LINK */
#define	CISTPL_NOLINKLEN	0x00

/* CCST_CIF */
#define CCST_CIFLEN		0x0e
#define CCST_CIF1			0x41
#define CCST_CIF2			0x02
#define	STCI_STR			"DVB_CI_V"
#define	STCI_STRP			0x2e

/* STCE_EV */
#define STCE_EV_VAL		"DVB_HOST"

/* STCE_PD */
#define STCE_PD_VAL		"DVB_CI_MODULE"

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static FDI_CI_Phys_t tCiPhysTbl[FAI_CI_MAX_NUM];
static volatile unsigned char  Command_Register=0;
static volatile unsigned char  Status_Register=0;
static volatile unsigned char  Size_LS_Register=0;
static volatile unsigned char  Size_MS_Register=0;

BOOL g_bCiTestPass[FAI_CI_MAX_NUM];

static void INTRN_FAI_CI_NotifyCamStatus(unsigned short usSlotId, unsigned short usState);
FAI_ERR_CODE INTRN_FAI_CI_CamTestStart(unsigned long ulSlotId);
FAI_ERR_CODE INTRN_FAI_CI_CamTestStop(unsigned long ulSlotId);
FAI_ERR_CODE INTRN_FAI_CI_CamMemTest(unsigned long ulSlotId);
FAI_ERR_CODE INTRN_FAI_CI_CamIOTest(unsigned long ulSlotId);
FAI_ERR_CODE INTRN_FAI_CI_GetCisLen(unsigned long ulSlotId, unsigned short *addrCcrs);
FAI_ERR_CODE INTRN_FDI_CI_SearchTuple(unsigned mtuple, unsigned char tuple, unsigned char **pcis, int *len, int *first, int *itpcc_radr, int *dvb, int *last);
FAI_ERR_CODE INTRN_FDI_CI_CheckCIS(unsigned char *cis, int len, unsigned short *addrcor, unsigned char *cor, unsigned char *ciplus);
FAI_ERR_CODE INTRN_FAI_CI_ReadByte(unsigned long ulSlotId, int reg, unsigned char *pucValue);
FAI_ERR_CODE INTRN_FAI_CI_WriteByte(unsigned long ulSlotId, int reg, unsigned char *pucValue);
FAI_ERR_CODE INTRN_FAI_CI_ReceiveData(unsigned long ulSlotId, unsigned char *pucData, 
										unsigned short *pusSizeOut, unsigned char *pucStatus);
FAI_ERR_CODE INTRN_FAI_CI_SendData(unsigned long ulSlotId, unsigned char *pucData, 
									unsigned short usSizeIn, unsigned char *pucStatus);

static unsigned char uc_Expected_Tsb[6] = {0x01, 0x00, 0x80, 0x02, 0x01, 0x00};

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_CI_Init(void)
{
	int i;
	
	FDI_CI_RegisterCallback(INTRN_FAI_CI_NotifyCamStatus);

	for( i = 0; i < FAI_CI_MAX_NUM; i++ )
	{
		g_bCiTestPass[i] = FALSE;
	}
	
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (AVPLAY_CI_ENABLE==YES)
FAI_ERR_CODE FAI_CI_Test(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FDI_ERR_CODE eFdiErr;
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	unsigned char ucTmdErrCode[2];
	unsigned char ucSlotId;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CI_Test] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}

	FAI_TMD_GetParam(&ucSlotId, pucArgList, 0);

	if( ucSlotId > FAI_CI_MAX_NUM )
	{
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_PARAMETER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}
	
	if( g_bCiTestPass[ucSlotId] == FALSE )
	{
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		goto END;
	}
END:    
	tTMDResult.eResult = eTmdErr;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
	
}
#endif

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_Test(unsigned char ucSlotId, FDI_CI_STATE eState)
{
	FAI_ERR_CODE eFaiErr;
	FDI_ERR_CODE eFdiErr;
	FAI_TESTMENU_H hMenu;
	
	FAI_APP_GetTestMenuHandle(CI_DETECTION_UI_SHEET, CI_DETECTION_UI_CELL, &hMenu);
	if( eState == FDI_CI_EXTRACTED )
	{
		INTRN_FAI_CI_CamTestStop(ucSlotId);
		FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "CI OUT", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_CI_OUT_STR);
	}
	else if( eState == FDI_CI_ENABLED )
	{
		FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "CI Detection", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "Test", RGB_COLOR_YELLOW);
		FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);

		eFaiErr = INTRN_FAI_CI_CamTestStart(ucSlotId);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_Test] : INTRN_FAI_CI_CamTestStart!!\n")); 
			goto TEST_FAIL;
		}
		eFaiErr = INTRN_FAI_CI_CamMemTest(ucSlotId);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_Test] : INTRN_FAI_CI_CamMemTest!!\n")); 
			goto TEST_FAIL;
		}
		eFaiErr = INTRN_FAI_CI_CamIOTest(ucSlotId);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_Test] : INTRN_FAI_CI_CamIOTest!!\n")); 
			goto TEST_FAIL;
		}
		
		FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "CI Detection", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
		FDI_PANEL_Display(FAI_CI_PASS_STR);
		g_bCiTestPass[ucSlotId] = TRUE;
	}

	return FAI_NO_ERR;
TEST_FAIL:
	FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_RED);
	FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "CI Detection", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
	FDI_PANEL_Display(FAI_CI_FAIL_STR);
	g_bCiTestPass[ucSlotId] = FALSE;
	return FAI_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void INTRN_FAI_CI_NotifyCamStatus(unsigned short usSlotId, unsigned short usState)
{
	int Ret = 0;
	
	switch (usState)
	{
		case FDI_CI_EXTRACTED:
	        FAI_PRINT(FAI_PRT_ERROR, ("[CI] CAM EXTRACTED !!\n")); 
			INTRN_FAI_CI_Test(usSlotId, FDI_CI_EXTRACTED);			
	        break;
		case FDI_CI_INSERTED:
	        FAI_PRINT(FAI_PRT_ERROR, ("[CI] CAM INSERTED !!\n"));
	        break;

		case FDI_CI_ENABLED:
			INTRN_FAI_CI_Test(usSlotId, FDI_CI_ENABLED);
	        FAI_PRINT(FAI_PRT_ERROR, ("[CI] CAM ENABLED !!\n")); 
	        break;
		default:
	        FAI_PRINT(FAI_PRT_ERROR, ("[CI] Unknown Status !!\n"));
	        break;
	}
	return;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_CamTestStart(unsigned long ulSlotId)
{
	FDI_ERR_CODE eFdiErr;

	eFdiErr = FDI_CI_Open(ulSlotId);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamTestStart] : FDI_CI_Open Error!!\n"));
		tCiPhysTbl[ulSlotId].Slot_id = INVALID_CI_SLOT_ID;
		return FAI_ERR;
	}

	tCiPhysTbl[ulSlotId].State 	 = DRV_IDLE;
	tCiPhysTbl[ulSlotId].firstSLPDU = NULL;
	tCiPhysTbl[ulSlotId].Slot_id = ulSlotId;
	tCiPhysTbl[ulSlotId].Ci_plus = 0;

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_CamTestStop(unsigned long ulSlotId)
{
	unsigned char *p;
	unsigned char *free_p;

	if (tCiPhysTbl[ulSlotId].Slot_id != INVALID_CI_SLOT_ID)
	{
		FDI_CI_Close(tCiPhysTbl[ulSlotId].Slot_id);
		tCiPhysTbl[ulSlotId].Slot_id = INVALID_CI_SLOT_ID;
	}
	if (tCiPhysTbl[ulSlotId].firstSLPDU)
	{
		p = (unsigned char *)tCiPhysTbl[ulSlotId].firstSLPDU;
		while (((FDI_CI_SLPDU_t*)p)->next)
		{
			free_p = p;
			p = ((FDI_CI_SLPDU_t*)p)->next;
			FVK_MEM_Free(free_p);
		}
		FVK_MEM_Free(p);
	}
	tCiPhysTbl[ulSlotId].State = DRV_DISCONNECT;

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_CamMemTest(unsigned long ulSlotId)
{
	FDI_ERR_CODE eFdiErr;
	unsigned char *cis; 			/* CIS buffer		 */
	unsigned short addrcor; 		/* COR Address		 */
	unsigned char cor;				/* COR value		 */
	unsigned char ciplus;			/* CI+ CAM detected  */
	int iRet;						/* return code		 */
	unsigned short cisLen;
	unsigned short addrccrs;

	tCiPhysTbl[ulSlotId].State = DRV_DISCONNECT;

	if( FDI_CI_CheckModule(ulSlotId) != FDI_CI_INSERTED)
	{
		tCiPhysTbl[ulSlotId].State = DRV_IDLE;
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamMemTest] : FDI_CI_IsCamAvailable Error!!\n"));
		return FAI_ERR;	
	}
 	FVK_TASK_Sleep(1000);

	if( INTRN_FAI_CI_GetCisLen(ulSlotId, &addrccrs) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamMemTest] : INTRN_FAI_CI_GetCisLen Error!!\n"));
		return FAI_ERR;		/* wrong CIS */
	}
	cisLen = addrccrs >> 1;
	FAI_PRINT(0, ("[INTRN_FAI_CI_CamMemTest] : cisLen=%x\n", cisLen));

	cis = (unsigned char *)FVK_MEM_Alloc(cisLen);
	if(!cis)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamMemTest] : FVK_MEM_Alloc Error!!\n"));
		return FAI_ERR;
	}

	eFdiErr = FDI_CI_ReadMem(ulSlotId, DRV_MEMADDR, cisLen, cis);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamMemTest] : FDI_CI_ReadMem Error!!\n"));
		FVK_MEM_Free(cis);
		return FAI_ERR; 	/* error while reading CIS */
	}

	if( INTRN_FDI_CI_CheckCIS(cis, cisLen, &addrcor, &cor, &ciplus) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CIS : PHYS_CheckCIS reports wrong CIS !!!\n"));
		FVK_MEM_Free(cis);
		return FAI_ERR;		/* wrong CIS */
	}
	FVK_MEM_Free(cis);

	/* the CIS is OK, addrcor contains the COR Address and cor the COR value */
	eFdiErr = FDI_CI_WriteMem(ulSlotId, addrcor>>1, 1, &cor);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamMemTest] : FDI_CI_WriteMem Error!!\n"));
		return FAI_ERR; /* error while writing the COR */
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_CamIOTest(unsigned long ulSlotId)
{
	FDI_ERR_CODE eFdiErr;
	FAI_ERR_CODE eFaiErr;
	unsigned char	buff[MAXLNSIZE];
	unsigned short	Size;
	unsigned char	status;
	int 			count;
	int 			Ret;
	unsigned int Host_Size = MAXLNSIZE;
	unsigned short NegociatedBufferSize;
	unsigned short *Negociate_Size = &NegociatedBufferSize;

	if( FDI_CI_CheckModule(ulSlotId) != FDI_CI_INSERTED )
	{
		tCiPhysTbl[ulSlotId].State = DRV_IDLE;
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : FDI_CI_Open Error!!\n"));
		return FAI_ERR;		
	}

	/* CLEAR the EMSTREAM pin */
#if 0	/* all right? */
	eFdiErr = FDI_CI_SetSignal(ulSlotId, DRV_EMSTREAM, 0);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : FDI_CI_SetSignal Error!!\n"));
		return FAI_ERR;
	}
#endif
	Command_Register = RS;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}

	Command_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}

	count=0;
	while(1)
	{
		Status_Register = 0x00;
		eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_STA_REG, (unsigned char *)&Status_Register);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : INTRN_FAI_CI_ReadByte Error!!\n"));
			return FAI_ERR;
		}
				
		if(Status_Register & FR)
			break;

		FVK_TASK_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return FAI_ERR;
		}
	}

	Command_Register = SR;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}
	
	count=0;
	while(1)
	{
		status = 0;
		buff[0] = buff[1] = 0; // VK_memset(buff, 0, MAXLNSIZE);
		eFaiErr = INTRN_FAI_CI_ReceiveData(ulSlotId, buff, &Size, &status);
		if( eFaiErr == FAI_NO_ERR )
		{
			break;
		}
		
		if( status != M_DATA )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : Error at %d lines !!!\n", __LINE__));
			return FAI_ERR;
		}

		FVK_TASK_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return FAI_ERR;
		}
	}

	(*Negociate_Size) = buff[0] * 256 + buff[1];

	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] PHYS_ResetModule : Size = %d, Negociate_Size = %d, Host_Size = %d\n", Size, *Negociate_Size, Host_Size));

	if (Size != 2)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] PHYS_ResetModule : Wrong Size %d !!!\n", Size));
	}

	if((*Negociate_Size) > Host_Size)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] PHYS_ResetModule : Negociate_Size = %d > Host_Size = %d !!!\n", *Negociate_Size, Host_Size)); // 이 경우 지원 가능하다면 Host_Size (MAXLNSIZE) 를 늘려주는 것이 좋다.
		buff[0] = Host_Size >> 8;
		buff[1] = Host_Size;
		(*Negociate_Size) = Host_Size;
	}

	Command_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}

	Command_Register = SW;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}

	/* send the buffer size to the module */
	count=0;
	do
	{
		status = 0;
		eFaiErr = INTRN_FAI_CI_SendData(ulSlotId, buff, 2, &status);
		if( eFaiErr == FAI_NO_ERR )
		{
			break;
		}

		if( status != M_BUSY )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : INTRN_FAI_CI_SendData Error!!\n"));
			return FAI_ERR;
		}
		FVK_TASK_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return FAI_ERR;
		}

	} while(status == M_BUSY);	
	
	Command_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}
	
	count=0;
	while(1)
	{
		Status_Register = 0x00;
		eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_STA_REG, (unsigned char *)&Status_Register);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : FDI_CI_WriteIo Error!!\n"));
			return FAI_ERR;
		}
				
		if(Status_Register & FR)
			break;

		FVK_TASK_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return FAI_ERR;
		}
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_GetCisLen(unsigned long ulSlotId, unsigned short *addrCcrs)
{
	FDI_ERR_CODE eFdiErr;
	int i;
	int iRet;							/* return code	*/
	unsigned char cis[8];
	unsigned char radrSize;

	for(i = DRV_MEMADDR; i < MAXADDR; )
	{
		eFdiErr = FDI_CI_ReadMem(ulSlotId, i, 8, cis);
		if( eFdiErr != FDI_NO_ERR )
		{
			return FAI_ERR;		/* error while reading CIS */
		}
		if(cis[0] == CISTPL_CONFIG)
		{
			radrSize = cis[2] & TPCC_RASZ;			/* number of bytes of Configuration Register Base Address */
			if (radrSize > NBRBYTES)				/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
			{
				return FAI_ERR;						/* ======= wrong Configuration Register Base Address */
			}
			if (radrSize == 0)
				*addrCcrs = cis[4];
			else
				*addrCcrs = cis[4] | (cis[5] << 8);
			if (*addrCcrs > MAXADDR)				/* Control */
			{
				return FAI_ERR;						/* ======= wrong Configuration Register Base Address */
			}
			return FAI_NO_ERR;
		}
		else
		{
			if(cis[0] == 0xff || !cis[1] || cis[1] == 0xff)
				return -1;
			i += cis[1] + 2;
		}

	}

	return FAI_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FDI_CI_SearchTuple(unsigned mtuple, unsigned char tuple, unsigned char **pcis, int *len, int *first, int *itpcc_radr, int *dvb, int *last)
{
	int ocis;						/* current offset in the CIS */
	int link;						/* tuple link				 */
	int found;						/* 1 if the current tuple is found, else 0 */
	unsigned char ctuple;			/* current tuple			 */
	int sublen; 					/* intermediate length		 */
	unsigned char *subpcis; 		/* pointer to subtuple		 */
	int nb, nbt0 = 0, nbt1 = 0, i;	/* intermediate variables	 */
	int ctpce_number = 0;			/* Configuration Index Value */
	char ctpcc_sz;					/* tuple's elementary item   */
	char lastentry; 				/* 1 if the last entry is found */
	char ctpcc_rasz;
	char ctpcc_rmsz;
	char cpce_pd;
	int ciplus = 0;

	ocis  = 0;
	found = 0;

	do
	{
		ctuple = **pcis;
		link   = *(*pcis+1);
		*pcis += 2;
		ocis  += 2;

		if (tuple == ctuple)
		{
			found = 1;
			/* tuple found ; control it */

			switch (tuple) {
			case CISTPL_VERS_1 :
				if (*(*pcis + TPLLV1_MAJOR) != TPLLV1_MAJOR_VAL)	/* Control */
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong TPLLV1_MAJOR\n"));
					return -1;	/* wrong TPLLV1_MAJOR */			/* ======= */
				}
				if (*(*pcis + TPLLV1_MINOR) != TPLLV1_MINOR_VAL)	/* Control */
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong TPLLV1_MINOR\n"));
					return -1;	/* wrong TPLLV1_MINOR */			/* ======= */
				}
				{
					/* check TPLLV1_INFO's "Additional Product Information" includes "ciplus=1" with case insensitive parsing */

					unsigned char info_str[256]; // maybe max tuple data length is 254
					int info_len;

					info_len = link - TPLLV1_INFO; // info_len 가 254 를 넘을 일이 없으므로 이에 대한 에러 핸들링은 할 필요가 없지...
					if (info_len > 0)
					{
						for (i = 0; i < info_len; i++)
						{
							if (*(*pcis + TPLLV1_INFO + i) == CISTPL_NULL)
								info_str[i] = ' ';
							else if (*(*pcis + TPLLV1_INFO + i) == CISTPL_END)
								info_str[i] = 0x00;
							else
								info_str[i] = *(*pcis + TPLLV1_INFO + i);
						}
						for (i = 0; i < info_len; i++)
						{
							info_str[i] = tolower(info_str[i]);
						}
						if (strstr(info_str, "ciplus=1"))
						{
							ciplus = 1;
						}
					}
				}
				break;

			case CISTPL_CONFIG :
				ctpcc_sz = *(*pcis + TPCC_SZ);
				ctpcc_rasz = ctpcc_sz & TPCC_RASZ;		/* number of bytes of Configuration Register Base Address */
				if (ctpcc_rasz > NBRBYTES)				/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : wrong Configuration Register Base Address (ctpcc_rasz = 0x%x)\n", ctpcc_rasz));
					return -1;							/* ======= wrong Configuration Register Base Address */
				}

				ctpcc_rmsz = ctpcc_sz & TPCC_RMSZ;
				*last  = *(*pcis + TPCC_LAST);			/* index number of last entry in Card configuration Table */
				if (ctpcc_rasz == 0)
					*itpcc_radr = *(*pcis + TPCC_RADR);
				else
					*itpcc_radr = *(*pcis + TPCC_RADR) + (*(*pcis + TPCC_RADR + 1) << 8);
				if (*itpcc_radr > MAXADDR)				/* Control */
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : wrong Configuration Register Base Address (*itpcc_radr = 0x%x)\n", *itpcc_radr));
					return -1;							/* ======= wrong Configuration Register Base Address */
				}
				if (! ((*(*pcis + TPCC_RADR + ctpcc_rasz + 1)) & TPCC_RMSK))	/* Control */
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : COR not present\n"));
					return -1;							/* COR not present		   ======= */
				}

				nb = TPCC_RADR + ctpcc_rasz + 1 + ctpcc_rmsz + 1;
				subpcis = *pcis + nb;
				sublen = *len - ocis - (link - nb);

				/* control CCST_CIF subtuple */
				if (INTRN_FDI_CI_SearchTuple(tuple, CCST_CIF, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong CCST_CIF\n"));
					return -1;		/* Control not found or wrong CCST_CIF */
				}
				break;				/* =======							   */

			case CISTPL_CFTABLE_ENTRY :
				if (*first & !((*(*pcis + TPCE_INDX)) & TPCE_INDX_MASK))	/* Intface bit and Default bit must be present in the first entry */
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong CISTPL_CFTABLE_ENTRY\n"));
					return -1;		/* Control wrong first CISTPL_CFTABLE_ENTRY tuple */
				}
									/* =======										  */
				ctpce_number = *(*pcis + TPCE_INDX) & ~TPCE_INDX_MASK;	/* configuration Entry Number to write to the COR */

				*first = 0;

				if (*(*pcis + TPCE_INDX) & TPCE_INTFACE)
				{
					if (*(*pcis + TPCE_IF) != TPCE_IF_VAL)				/* Control : TPCE_IF must be equal to TPCE_IF_VAL */
					{													/* =======										  */
						lastentry = (char)-2; 							/* wrong CISTPL_CFTABLE_ENTRY tuple */
						break;
					}
				}
				else
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong TPCE_IF\n"));
					lastentry = (char)-2; 	/* Control : wrong CISTPL_CFTABLE_ENTRY tuple */
					break;					/* =======									  */
				}

				if (!(*(*pcis + TPCE_FS) & TPCE_FS_MASK))	/* Control : I/O Space and power must be present */
				{											/* =======										 */
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong TPCE_FS_MASK\n"));
					lastentry = (char)-2; 					/* wrong CISTPL_CFTABLE_ENTRY tuple */
					break;
				}

				nb = 0;

				if (*(*pcis + TPCE_FS) & TPCE_FS_POWER)
				{
					cpce_pd = *(*pcis + TPCE_PD);
					/* find the number of bytes coding the TPCE_PD (power)	*/
					for (i=0; i < 8; i++)
					{
						if (cpce_pd & TPCE_PD_MASK) /* one set bit means one following byte */
							nb++;
						cpce_pd = cpce_pd >> 1;
					}
					/* skip the TPCE_PD ; if the bit 7 of one power byte is set then a additional byte is present */
					for (i=TPCE_PD + 1; i <= TPCE_PD + nb; i++)
					{
						if (*(*pcis + i) & TPCE_EXT)
							nb++;
					}	/* end of power configuration skipping */
					nb++;		/* count the Power Description Structure Parameter Selection Byte */
				}

				if (*(*pcis + TPCE_FS) & TPCE_TIMING)
				{
					/* skip the timing configuration */
					if ((*(*pcis + TPCE_PD + nb) & TPCE_TIMING_WAIT) != TPCE_TIMING_WAIT)
					{
						nbt0 = 0;
						while (*(*pcis + TPCE_PD + nb + nbt0 + 1) & TPCE_EXT)
							nbt0++;
						nbt0++;
					}
					if ((*(*pcis + TPCE_PD + nb) & TPCE_TIMING_READY) != TPCE_TIMING_READY)
					{
						nbt1 = 0;
						while (*(*pcis + TPCE_PD + nb + nbt0 + nbt1 + 1) & TPCE_EXT)
							nbt1++;
						nbt1++;
					}
					nb = nb + nbt0 + nbt1 + 1;
				}	/* end of timing configuration skipping */

				/* skip the I/O Space */
				if (*(*pcis + TPCE_FS) & TPCE_IO)
				{
					if (*(*pcis + TPCE_PD + nb) & TPCE_IO_RANGE)
					{
						nbt0 = (*(*pcis + TPCE_PD + nb + 1) & TPCE_IO_SLN) >> TPCE_IO_SLNR;
						nbt1 = (*(*pcis + TPCE_PD + nb + 1) & TPCE_IO_SAN) >> TPCE_IO_SANR;
						nb = nb + nbt0 + nbt1 + 1;	/* + 1 to skip the I/O Range Descriptor */
					}
					nb++;	/* skip the I/O Space descriptor */
				}	/* end of I/O space configuration skipping */

				/* skip the IR Configuration */
				if (*(*pcis + TPCE_FS) & TPCE_IR)
				{
					if (*(*pcis + TPCE_PD + nb) & TPCE_IR_MASK)
						nb += 2;
					nb++;
				}	/* end of IR configuration skipping */

				/* skip the Mem space Configuration */
				if (*(*pcis + TPCE_FS) & TPCE_MEM)
				{
					nbt0 = (*(*pcis + TPCE_PD + nb) & TPCE_MEM_SL) >> TPCE_MEM_SLR;
					nbt1 = (*(*pcis + TPCE_PD + nb) & TPCE_MEM_SCA) >> TPCE_MEM_SCAR;
					if (*(*pcis + TPCE_PD + nb) & TPCE_MEM_HPA)
						nbt1 *= 2;
					nb = nb + nbt0 + nbt1 + 1;
				}	/* end of Mem space configuration skipping */

				/* Control the STCE_EV subtuple */
				nb = TPCE_PD + nb;
				subpcis = *pcis + nb;
				sublen = *len - ocis - nb;
				if (INTRN_FDI_CI_SearchTuple(tuple, STCE_EV, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{						/* Control */
					lastentry = (char)-2; 	/* ======= not found or wrong STCE_EV */
					break;
				}

				/* Control the STCE_PD subtuple */
				if (INTRN_FDI_CI_SearchTuple(tuple, STCE_PD, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{						/* Control */
					lastentry = (char)-2; 	/* ======= not found or wrong STCE_PD */
					break;
				}

				*dvb = ctpce_number;		/* the DVB entry is present */

				break;

			case CISTPL_NO_LINK :
				if (link != CISTPL_NOLINKLEN)	/* Control */
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong CISTPL_NOLINKLEN\n"));
					return -1;					/* ======= wrong CISTPL_NO_LINK tuple */
				}
				break;

			/* subtuples */
			case CCST_CIF : 		/* if mtuple = CISTPL_CFTABLE_ENTRY then this tuple is STCE_EV */
				if (mtuple == CISTPL_CFTABLE_ENTRY)
				{	/* STCE_EV */
					if (strcmp((char*)*pcis, STCE_EV_VAL)) 	/* Control */
					{
						FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong STCE_EV_VAL\n"));
						return -1;						/* ======= wrong STCE_EV subtuple */
					}
				}
				else
				{	/* CCST_CIF */
					if (**pcis != CCST_CIF1)	/* Control */
					{
						FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong CCST_CIF1\n"));
						return -1;				/* ======= wrong CCST_CIF subtuple */
					}
					if (*(*pcis + 1) != CCST_CIF2)	/* Control */
					{
						FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong CCST_CIF2\n"));
						return -1;					/* ======= wrong CCST_CIF subtuple */
					}

					/* STCI_STR must be "DVB_CI_Vx.xx" where x is digit */
					nb = strlen(STCI_STR);
					if (strncmp((char*)(*pcis + 2), STCI_STR, nb))	/* Control */
					{
						FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong STCI_STR\n"));
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!isdigit(*(*pcis + 2 + nb)))		/* Control */
					{
						FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__));
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (*(*pcis + 2 + nb + 1) != STCI_STRP) /* Control */
					{
						FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong STCI_STRP\n"));
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!isdigit(*(*pcis + 2 + nb + 2)))	/* Control */
					{
						FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__));
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!isdigit(*(*pcis + 2 + nb + 3)))	/* Control */
					{
						FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__));
						return -1;							/* ====== wrong CCST_CIF subtuple */
					}
				}
				break;

			case STCE_PD :
				if (strcmp((char*)(*pcis), STCE_PD_VAL)) /* Control */ 
				{
					FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : not found or wrong STCE_PD_VAL\n"));
					return -1;					/* ======= wrong STCE_PD subtuple */
				}
				break;

			/* end-of-list tuple */
			case CISTPL_END :
				return 0;			/* good CISTPL_END tuple */

			default :
				/* other tuples : no control */
				break;
			}
		}

		/* skip the current tuple */
		*pcis += link;
		ocis += link;
		*len -= link;
		if (*len <= 0)	/* Control */
		{
			FAI_PRINT(FAI_PRT_ERROR, ("PHYS_Tuple : tuple not found (ocis=%d, len=%d)\n", ocis, *len));
			return -1;		/* ======= tuple not found */
		}
	}
	while (!found);

	if (tuple == CISTPL_CFTABLE_ENTRY)
	{
		if (ctpce_number == *last || **pcis == CISTPL_NO_LINK) // if (ctpce_number == *last)
		{
			lastentry = 1;		/* end of one entry analysis, it's the last */
		}
		else
		{
			lastentry = 0;		/* not the last entry */
		}
		return lastentry;
	}
	else if (tuple == CISTPL_VERS_1)
	{
		if (ciplus == 1)
			return 2;
		else
			return 1;
	}
	else
		return 1;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FDI_CI_CheckCIS(unsigned char *cis, int len, unsigned short *addrcor, unsigned char *cor, unsigned char *ciplus)
{
	unsigned char *pcis;	/* pointer to the remainding CIS	*/
	int lcis;				/* length of the remainding CIS 	*/
	int first;				/* 1 for the first configuration table entry, else 0 */
	int addr;				/* configuration registers base address */
	int dvb;				/* index number of the DVB entry	*/
	int last;				/* index number of the last entry	*/
	int iRet;				/* return code						*/

	dvb = 0;

	pcis = cis;
	lcis = len;

	if (INTRN_FDI_CI_SearchTuple(0, CISTPL_DEVICE_0A, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0A\n"));
		return FAI_ERR;		/* not found or wrong CISTPL_DEVICE_0A		*/
	}

	if (INTRN_FDI_CI_SearchTuple(0, CISTPL_DEVICE_0C, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0C\n"));
		return FAI_ERR;		/* not found or wrong CISTPL_DEVICE_0C		*/
	}

	if ((iRet = INTRN_FDI_CI_SearchTuple(0, CISTPL_VERS_1, &pcis, &lcis, &first, &addr, &dvb, &last)) < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : not found or wrong CISTPL_VERS_1\n"));
		return FAI_ERR;		/* not found or wrong CISTPL_VERS_1 		*/
	}

	if (iRet == 2)
		*ciplus = 1;
	else
		*ciplus = 0;

	if (INTRN_FDI_CI_SearchTuple(0, CISTPL_MANFID, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : not found or wrong CISTPL_MANFID\n"));
		return FAI_ERR;		/* not found or wrong CISTPL_MANFID 		*/
	}

	if (INTRN_FDI_CI_SearchTuple(0, CISTPL_CONFIG, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : not found or wrong CISTPL_CONFIG\n"));
		return FAI_ERR;		/* not found or wrong CISTPL_CONFIG 		*/
	}

	*addrcor = addr;
	first = 1;
	do
	{
		iRet = INTRN_FDI_CI_SearchTuple(0, CISTPL_CFTABLE_ENTRY, &pcis, &lcis, &first, &addr, &dvb, &last);
	}
	while ((iRet == 0) || (iRet == -2));		/* one more entry */
	if (iRet < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : not found or wrong CISTPL_CFTABLE_ENTRY\n"));
		return FAI_ERR;		/* not found or wrong CISTPL_CFTABLE_ENTRY	*/
	}

	if (INTRN_FDI_CI_SearchTuple(0, CISTPL_NO_LINK, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : not found or wrong CISTPL_NO_LINK\n"));
		return FAI_ERR;		/* not found or wrong CISTPL_NO_LINK 		*/
	}

	if (INTRN_FDI_CI_SearchTuple(0, CISTPL_END, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : not found or wrong CISTPL_END\n"));
		return FAI_ERR;		/* not found or wrong CISTPL_END			*/
	}

	if (dvb != last)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_CheckCIS : DVB entry found but... (dvb=%x, last=%x)\n", dvb, last));
	}

	*cor = dvb;

	return FAI_NO_ERR;			/* good CIS */
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_ReadByte(unsigned long ulSlotId, int reg, unsigned char *pucValue)
{
	FDI_ERR_CODE eFdiErr;
	unsigned char ucValue;
#if 0	
	eFdiErr = FDI_CI_CheckSignal(ulSlotId, DRV_CARD_DETECT, &ucValue);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : FDI_CI_CheckSignal Error!!\n"));
		return FAI_ERR;
	}
	if( ucValue == 0 )
	{
		return FAI_ERR;
	}
#endif
	eFdiErr = FDI_CI_ReadIo(ulSlotId, reg, (unsigned char *)pucValue);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : FDI_CI_WriteIo Error!!\n"));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_WriteByte(unsigned long ulSlotId, int reg, unsigned char *pucValue)
{
	FDI_ERR_CODE eFdiErr;
	unsigned char ucValue;
#if 0	
	eFdiErr = FDI_CI_CheckSignal(ulSlotId, DRV_CARD_DETECT, &ucValue);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : FDI_CI_CheckSignal Error!!\n"));
		return FAI_ERR;
	}
	if( ucValue == 0 )
	{
		return FAI_ERR;
	}
#endif	
	eFdiErr = FDI_CI_WriteIo(ulSlotId, reg, pucValue);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_CamIOTest] : FDI_CI_WriteIo Error!!\n"));
		return FAI_ERR;
	}
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_ReceiveData(unsigned long ulSlotId, unsigned char *pucData, 
										unsigned short *pusSizeOut, unsigned char *pucStatus)
{
	FAI_ERR_CODE eFaiErr;
	FDI_ERR_CODE eFdiErr;
	int i;
	
	if(tCiPhysTbl[ulSlotId].State == DRV_IDLE)
	{
		return FAI_ERR;
	}

	Status_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_STA_REG, (unsigned char *)&Status_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : INTRN_FAI_CI_ReadByte Error!!\n"));
		return FAI_ERR;
	}

	if(!(Status_Register & DA))
	{
		*pucStatus = M_DATA;
		return FAI_ERR; /* if no data available */
	}

	/* data size */
	Size_LS_Register = Size_MS_Register = 0;
	eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_SIZE_LS_REG, (unsigned char *)&Size_LS_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : INTRN_FAI_CI_ReadByte Error!!\n"));
		return FAI_ERR;
	}
	eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_SIZE_MS_REG, (unsigned char *)&Size_MS_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : INTRN_FAI_CI_ReadByte Error!!\n"));
		return FAI_ERR;
	}
	*pusSizeOut = ((unsigned short)Size_MS_Register<<8) & 0xff00;
	*pusSizeOut |= (unsigned short)(Size_LS_Register & 0x00ff);

	if((tCiPhysTbl[ulSlotId].State == DRV_CONNECT) && (*pusSizeOut > tCiPhysTbl[ulSlotId].Negociated_size))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : read size invalid %d > %d -> PAL_CI_ResetModule(%d)...\n",
			*pusSizeOut, tCiPhysTbl[ulSlotId].Negociated_size, tCiPhysTbl[ulSlotId].Slot_id));

#if 0
		if (DI_CI_ResetModule(DRVTab[index].Slot_id) != 0x0)
		{
			CMD_Printf("[CI_RW_ERROR] PHYS_ReceiveData : fail to reset slot %d !!!\n", DRVTab[index].Slot_id);
		}
#endif		
		return FAI_ERR;
	}
	if (*pusSizeOut == 0xffff)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : read size invalid 0xffff !!!\n"));
		return FAI_ERR;
	}
	if (*pusSizeOut > MAXLNSIZE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : read size invalid %d > %d !!!\n", *pusSizeOut, MAXLNSIZE));
		return FAI_ERR;
	}

	/* read data */
	/* ========= */
	for( i = 0; i < *pusSizeOut; i++ )
	{
		eFdiErr = FDI_CI_ReadIo(ulSlotId, OFF_DATA_REG, &pucData[i]);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : FDI_CI_ReadIo(%d)\n", i));
			return FAI_ERR;
		}
	}

	Status_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_STA_REG, (unsigned char *)&Status_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_ReceiveData] : INTRN_FAI_CI_ReadByte Error!!\n"));
		return FAI_ERR;
	}

	if (Status_Register & DA)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("PHYS_ReceiveData : DA\n"));
//		return(M_DATA); /* <TODO_CI> : 이런 경우에 이 플로우에서 데이터를 다시 가져오도록 하는 것이 좋을 것 같은데... 좀더 고려 필요... */
	}

	if (Status_Register & RE)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[CI_RW_ERROR] PHYS_ReceiveData : RE !!!\n"));
//		return(T_ERR); /* <TODO_CI> : 이런 경우에 대한 처리에 대해선 좀더 고려 필요... */
	}

	if (*pusSizeOut == 6 // length 가 6 으로써 캠에서 보내는 TPDU 는 따져보면 Tsb 밖에 없다.
		&&
			((pucData[0] != 0x01 && pucData[0] != 0x02)
			|| (pucData[1] != 0x00 && pucData[1] != 0x01)
			|| (pucData[2] != 0x80 /* && pData[2] != 0x83 && pData[2] != 0x84 && pData[2] != 0x85 && pData[2] != 0x86 && pData[2] != 0xA0 && pData[2] != 0xA1 */)
			|| (pucData[3] != 0x02)
			|| (pucData[4] != pucData[0])
			|| (pucData[5] != 0x00 && pucData[5] != 0x80)
			)
		)
	{
		// 일단 에러 찍어 주고...
		FAI_PRINT(FAI_PRT_ERROR, ("[CI_RW_ERROR] Read I/O mem failed : wrong Tsb = [ %02x %02x %02x %02x %02x %02x ] -> expected [ %02x %02x %02x %02x %02x (00 or 80) ] !!!\n",
			pucData[0], pucData[1], pucData[2], pucData[3], pucData[4], pucData[5],
			uc_Expected_Tsb[0], uc_Expected_Tsb[1], uc_Expected_Tsb[2], uc_Expected_Tsb[3], uc_Expected_Tsb[4]));

		// 기대되는 T_SB 값으로 강제 변경
		FVK_MEM_Copy(pucData, uc_Expected_Tsb, 6);
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_CI_SendData(unsigned long ulSlotId, unsigned char *pucData, 
									unsigned short usSizeIn, unsigned char *pucStatus)
{
	FAI_ERR_CODE eFaiErr;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucSize;
	int i;

	if(tCiPhysTbl[ulSlotId].State == DRV_IDLE)
	{
		return FAI_ERR;
	}

	Status_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_STA_REG, (unsigned char *)&Status_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_ReadByte Error!!\n"));
		return FAI_ERR;
	}
	
#if 0
	if (Status_Register & DA)
	{
		PHYS_ReceiveIdle(index);
	}
#endif
	Command_Register = HC;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}

	/* Test FR */
	Status_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_STA_REG, (unsigned char *)&Status_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		Command_Register = 0x00;
		eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_ReadByte Error!!\n"));
			return FAI_ERR;
		}
		return FAI_ERR;
	}

	if(!(Status_Register & FR)) /* if dvb module is busy */
	{
		Command_Register = 0x00;
		eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_WriteByte Error!!\n"));
			return FAI_ERR;
		}
		*pucStatus = M_BUSY;
		return FAI_ERR;
	}


	/* write data size */
	ucSize = (unsigned char)usSizeIn;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_SIZE_LS_REG, &ucSize);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}
	
	ucSize = (unsigned char)(usSizeIn>>8);	
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_SIZE_MS_REG, &ucSize);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}

	/* write data */
	/* ========== */
	for( i = 0; i < usSizeIn; i++ )
	{
		eFdiErr = FDI_CI_WriteIo(ulSlotId, OFF_DATA_REG, &pucData[i]);
		if( eFdiErr != FDI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : FDI_CI_WriteIo Error!!\n"));
			return FAI_ERR;			
		}
	}

	/* Verify if WE is down */
	Status_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_ReadByte(ulSlotId, OFF_STA_REG, (unsigned char *)&Status_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_ReadByte Error!!\n"));
		return FAI_ERR;
	}

	if(Status_Register & WE)
	{
		Command_Register = 0x00;
		eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_WriteByte Error!!\n"));
			return FAI_ERR;
		}
		return FAI_ERR;
	}

	Command_Register = 0x00;
	eFaiErr = INTRN_FAI_CI_WriteByte(ulSlotId, OFF_COM_REG, (unsigned char *)&Command_Register);
	if( eFaiErr != FAI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_CI_SendData] : INTRN_FAI_CI_WriteByte Error!!\n"));
		return FAI_ERR;
	}
	uc_Expected_Tsb[0] = uc_Expected_Tsb[4] = pucData[0];

	return FAI_NO_ERR;
}

/* end of file */

