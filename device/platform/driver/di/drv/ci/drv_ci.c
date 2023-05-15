//#define	CI_HW_TEST
#ifdef	CI_HW_TEST
#else
/* $Header$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: drv_ci.c
// Original Author:
// File Description:
// Module:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.								*/
/* All rights reserved. 												*/
/*******************************************************************/


/*******************************************************************/
/* INCLUDE FILES													*/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<string.h>
#endif
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "nexus_types.h"
#include "nexus_pid_channel.h"

/* humax di layer */
#include "linden_trace.h"

#include "bstd_defs.h"
#include "vkernel.h"
#include "taskdef.h"
#include "di_err.h"
#include "drv_ci.h"
#include "di_ci.h"
#include "di_uart.h"
#include "pdriver.h"
#include "drv_gpio.h"

#define CONFIG_BCM_KERNEL_MODE

#if !defined(CONFIG_BCM_KERNEL_MODE)
#include "breg_mem.h"
#endif

#if defined(CONFIG_REMUX_EXT_CLOCK)
#include "nexus_platform.h"
#if (NEXUS_VERSION < 1401)
#include "bchp.h"
#include "bchp_aud_fmm_pll0.h"
#include "bchp_aud_fmm_pll1.h"
#include "bchp_aud_fmm_op_ctrl.h"
#endif
#endif

#include "bchp_ebi.h"
#include "nexus_platform_features.h"

/*******************************************************************/
/* LOCAL DEFINE 													*/
/*******************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_CI

#define UNREFENCED_PARAM(x) (x=x)
#define DRV_CI_USING_MMAP		(1)

#define CI_TASK_SLEEP_TIME		500

#define DI_CI_TASK_STACKSIZE		(1024 * 4)
#define MAX_NUM_CIMSG			16

#define CAM_RESET_TIME				20

#if 0
#define CAM_INT_WAIT_TIME			300
#define CAM_POWER_ON_WAIT_TIME	500
#define CAM_RESET_WAIT_TIME 		450
#define CAM_RESET_TIME				20
#define CAM_READY_WAIT_TIME 		600
#define RETRY_COUNT 				3
#endif

#define MAX_RESET_COUNT			300 // org is 100

#define GPIO_PIN_5_CI_PWR		5

#define CI_PWR_OFF					(0)
#define CI_PWR_ON					(1)

#define CAM_RESET_OFF				(0)
#define CAM_RESET_ON				(1)

#define OFFSET_BCHP_EBI_CS_BASE_2		BCHP_EBI_CS_BASE_2			/*0x00419810*/
#define	OFFSET_BCHP_EBI_CS_CONFIG_2		BCHP_EBI_CS_CONFIG_2		/*0x00419814*/
#define	PHYS_EBI_CS2_BASE  				NEXUS_DVB_CI_MEMORY_BASE	/*0x1A000000*/	// 0x19800000
#define	PHYS_EBI_CS2_SIZE				NEXUS_DVB_CI_MEMORY_LENGTH	/*0x100000*/
#define	PHYS_EBI_CS2_SIZE_1MB			BCHP_EBI_CS_BASE_2_size_SIZE_1MB	/*7*/
#define	PHYS_EBI_CS2_SIZE_8kB			BCHP_EBI_CS_BASE_2_size_SIZE_8KB	/*0*/
#define	EBI_mem_io_MASK					BCHP_EBI_CS_CONFIG_2_mem_io_MASK	/*0x80000000*/
#define	EBI_cs_hold_MASK				BCHP_EBI_CS_CONFIG_2_cs_hold_MASK	/*0x00008000*/
#define	EBI_split_cs_MASK				BCHP_EBI_CS_CONFIG_2_split_cs_MASK	/*0x00004000*/
#define	EBI_enable_MASK					BCHP_EBI_CS_CONFIG_2_enable_MASK	/*0x00000001*/
#define	EBI_we_ctl_MASK					BCHP_EBI_CS_CONFIG_2_we_ctl_MASK	/*0x00000020*/
#define	EBI_little_endian_MASK			BCHP_EBI_CS_CONFIG_2_le_MASK		/*0x00000400*/

#ifdef CONFIG_USE_CI_BUS_BIG_ENDIAN_MODE
#define	CI_BUS_ENDIAN	0 /*BIG*/
#else
#define	CI_BUS_ENDIAN	EBI_little_endian_MASK /*LITTLE*/
#endif

/* kernel driver 관련 임시 코드..   */
//#include "autoconf.h"
#ifdef CONFIG_BCM_KERNEL_MODE
#define CI_REG_ACCESS	0
#define CI_MEM_ACCESS	1
#define CI_RD_MODE		0
#define CI_WR_MODE		1

void (*_pfnCiCallbackFuncforDemux)(unsigned short, unsigned short);

unsigned long CIREG_Read(void *RegHandle, unsigned long ofs)
{
	UNREFENCED_PARAM(RegHandle);
	unsigned long rdValue;

#if defined(CONFIG_CI_NULL)
	rdValue = 0;
#elif defined(CONFIG_DIRECTFB_MASTER)
	NEXUS_CI_Read(CI_REG_ACCESS, (unsigned long)ofs, (unsigned long)&rdValue);
#else
	NEXUS_CI_Access(CI_REG_ACCESS, CI_RD_MODE, (unsigned long)ofs, (unsigned long)&rdValue);
#endif
	return rdValue;
}

void CIREG_Write(void *RegHandle, unsigned int ofs, unsigned int data)
{
	UNREFENCED_PARAM(RegHandle);
	PrintDebug("[%s] write at 0x%x value=0x%x\n", __FUNCTION__, ofs, data);
#if defined(CONFIG_CI_NULL)
#elif defined(CONFIG_DIRECTFB_MASTER)
	NEXUS_CI_Write(CI_REG_ACCESS, (unsigned long)ofs, (unsigned long)data);
#else
	NEXUS_CI_Access(CI_REG_ACCESS, CI_WR_MODE, (unsigned long)ofs, (unsigned long)data);
#endif
}

unsigned char CIMEM_Read(void *RegHandle, unsigned int ofs)
{
	UNREFENCED_PARAM(RegHandle);
	unsigned long rdValue;
#if defined(CONFIG_CI_NULL)
	rdValue = 0;
#elif defined(CONFIG_DIRECTFB_MASTER)
	NEXUS_CI_Read(CI_MEM_ACCESS, (unsigned long)ofs, (unsigned long)&rdValue);
#else
	NEXUS_CI_Access(CI_MEM_ACCESS, CI_RD_MODE, (unsigned long)ofs, (unsigned long)&rdValue);
#endif
	return (rdValue & 0x000000FF);
}

void CIMEM_Write(void *RegHandle, unsigned int ofs, unsigned char data)
{
	UNREFENCED_PARAM(RegHandle);
	unsigned long wrValue;
	wrValue = (unsigned long)data;

#if defined(CONFIG_CI_NULL)
#elif defined(CONFIG_DIRECTFB_MASTER)
	NEXUS_CI_Write(CI_MEM_ACCESS, ofs, wrValue);
#else
	NEXUS_CI_Access(CI_MEM_ACCESS, CI_WR_MODE, ofs, wrValue);
#endif
}
#else
#define CIREG_Read	BREG_Read32
#define CIREG_Write	BREG_Write32
#define CIMEM_Read	BREG_Read8
#define CIMEM_Write	BREG_Write8
#endif

#if defined(CONFIG_REMUX_EXT_CLOCK)
#if (NEXUS_VERSION < 1401)
static unsigned long REG_Read32(void *RegHandle, unsigned long ofs)
{
	UNREFENCED_PARAM(RegHandle);
	unsigned long rdValue;
	NEXUS_Platform_Registers_Read_Access(ofs, (unsigned long *)&rdValue);
	return rdValue;
}

static void REG_Write32(void *RegHandle, unsigned int ofs, unsigned int data)
{
	UNREFENCED_PARAM(RegHandle);
	NEXUS_Platform_Registers_Write_Access(ofs, data);
}
#endif
DRV_Error DRV_CI_SetRemuxExtinalClock(void);
#endif
/*******************************************************************/
/* TYPE DEFINE													*/
/*******************************************************************/
typedef enum CI_TASK_STATE
{
	CI_TASK_IDLE,
	CI_TASK_CAM_EXTRACTED,
	CI_TASK_CAM_INSERTED,
	CI_TASK_CAM_POWER_ON,
	CI_TASK_CAM_RESET,
	CI_TASK_CAM_ACCESSIBLE
}CI_TASK_STATE_t;

typedef enum
{
	DRV_CI_MODE_MEMORY		=0x0,
	DRV_CI_MODE_IO			=0x1
}DRV_CI_MODE;

#ifdef CONFIG_BCM_KERNEL_MODE
void *g_hReg = NULL;
void *g_CI_MMAP_Handle = NULL;
#else
extern BREG_Handle g_hReg;
extern BREG_Handle g_CI_MMAP_Handle;
#endif

/*******************************************************************/
/* GLOBAL VARIABLES 												*/
/*******************************************************************/
CAMINFO_t caminfo;


/*******************************************************************/
/* STATIC VARIABLES 												*/
/*******************************************************************/
static unsigned long 	s_ulCiTaskId;
static DRV_CI_MODE		s_nEbiMode;
static HULONG s_ebiLock;
/*******************************************************************/
/* STATIC FUNCTIONS 												*/
/*******************************************************************/
static void P_CI_Task (void *param);
static void P_CI_EBI_CS2_SelectMode(DRV_CI_MODE nMode);

/*******************************************************************/
/* FUNCTION DESCRIPTION 										*/
/*******************************************************************/

int acquire_ebi_sem(void)
{
	VK_SEM_Get(s_ebiLock);
	return 0;
}

void release_ebi_sem(void)
{
	VK_SEM_Release(s_ebiLock);
	return;
}

/*****************************************************************
 * Function	: DRV_CI_Init
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DRV_Error DRV_CI_Init (void)
{
	int err;

	PrintEnter();
#if defined(CONFIG_REMUX_EXT_CLOCK)
	DRV_CI_SetRemuxExtinalClock();
#endif

#if	defined(DRV_CI_USING_MMAP)
	P_CI_EBI_CS2_SelectMode(DRV_CI_MODE_MEMORY);
#endif

	caminfo.ucOpen	= FALSE;
	caminfo.nDevId	= 0;
	caminfo.usSlotId	= 0;
	caminfo.ucDetect	= CAM_NO_DETECT;
	caminfo.usStatus	= DI_CI_EXTRACTED;
	DRV_CI_PowerOff_CAM (caminfo.nDevId); /*Power down to ensure the intial power status.*/

	err = VK_SEM_Create(&s_ebiLock, "EBI_SEM", VK_SUSPENDTYPE_FIFO);
	if(err){
		PrintError("VK_SEM_Create() error: EBI_LOCK\n");
		return DRV_ERR;
	}

#if defined(CONFIG_CI_NULL)
#else
	/* Create Task */
	if (VK_TASK_Create ( P_CI_Task,
						(unsigned long)CI_TASK_PRIORITY,
						(unsigned long)DI_CI_TASK_STACKSIZE,
						(char *)"DRV_CI",
						(void *)NULL,
						&s_ulCiTaskId,
						FALSE) != VK_OK)
	{
		PrintError("[%s] Error::ci_init - VK_TASK_Create \n", __func__);
		return DRV_ERR;
	}

	if (VK_TASK_Start (s_ulCiTaskId) != VK_OK)
	{
		PrintError("[%s] s_ulCiTaskId task start error \n\r", __func__);
		return DRV_ERR;
	}
#endif

	PrintExit();

	return DRV_OK;
}

void DRV_CI_RegisterCallback (void (*CiCallbackFuncforDemux)(unsigned short usSlotId, unsigned short flag))
{
	_pfnCiCallbackFuncforDemux = CiCallbackFuncforDemux;
	return;
}

/*****************************************************************
 * Function	: DRV_CI_Task
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
static void P_CI_Task (void *param)
{
	int retval;
	static CI_TASK_STATE_t s_CiTaskState = CI_TASK_IDLE;

	UNREFENCED_PARAM(param);
	while (1)
	{
		if (_pfnCiCallbackFunc != NULL)
		{
			retval = DRV_CI_GetCAMDetect (caminfo.nDevId); // PHYS_DRVOpen() -> DRV_CI_Open() 되기 전에 caminfo.nDevId 값은 무효할텐데 이건 좀 이상하다... 어쨌든 현재로선 돈다 -.-

			if (retval != caminfo.ucDetect)
			{
				caminfo.ucDetect = retval;

				if (retval == CAM_DETECT)
					s_CiTaskState = CI_TASK_CAM_INSERTED;
				else
					s_CiTaskState = CI_TASK_CAM_EXTRACTED;
			}

			switch (s_CiTaskState)
			{
				case CI_TASK_IDLE:
					/* idle state : nothing to do */
					break;

				case CI_TASK_CAM_EXTRACTED:
					PrintDebug("[%s] CI_TASK_CAM_EXTRACTED\n", __func__);
					DRV_CI_PowerOff_CAM (caminfo.nDevId);
					caminfo.usStatus = DI_CI_EXTRACTED;
					_pfnCiCallbackFunc (caminfo.usSlotId, caminfo.usStatus);
					if(_pfnCiCallbackFuncforDemux != NULL)
					{
						_pfnCiCallbackFuncforDemux(caminfo.usSlotId, caminfo.usStatus);
					}
					s_CiTaskState = CI_TASK_IDLE;
					break;

				case CI_TASK_CAM_INSERTED:
					PrintDebug("[%s] CI_TASK_CAM_INSERTED\n", __func__);
					caminfo.usStatus = DI_CI_INSERTED;
					_pfnCiCallbackFunc (caminfo.usSlotId, caminfo.usStatus);
					s_CiTaskState = CI_TASK_CAM_POWER_ON;
					break;

				case CI_TASK_CAM_POWER_ON:
					PrintDebug("[%s] CI_TASK_CAM_POWER_ON\n", __func__);
					DRV_CI_PowerOn_CAM (caminfo.nDevId);
					s_CiTaskState = CI_TASK_CAM_RESET;
					break;

				case CI_TASK_CAM_RESET:
					PrintDebug("[%s] CI_TASK_CAM_RESET\n", __func__);
					DRV_CI_Reset_CAM (caminfo.nDevId);
					s_CiTaskState = CI_TASK_CAM_ACCESSIBLE;
					break;

				case CI_TASK_CAM_ACCESSIBLE:
					PrintDebug("[%s] CI_TASK_CAM_ACCESSIBLE\n", __func__);
					caminfo.usStatus = DI_CI_ENABLED;
					_pfnCiCallbackFunc (caminfo.usSlotId, caminfo.usStatus);
					if(_pfnCiCallbackFuncforDemux != NULL)
					{
						_pfnCiCallbackFuncforDemux(caminfo.usSlotId, caminfo.usStatus);
					}
					s_CiTaskState = CI_TASK_IDLE;
					break;

				default:
					PrintError("[%s] Unknown CI TASK STATE!!!\n", __func__);
					break;
			}

		}

		VK_TASK_Sleep (CI_TASK_SLEEP_TIME);
	}
}


/*****************************************************************
 * Function	: DRV_CI_CheckSlotId
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
int DRV_CI_CheckSlotId (unsigned short usSlotId)
{
#if defined(CONFIG_CI_NULL)
#else
	// 일단 슬롯 하나인 경우만 고려하여 코딩함.
	if ((caminfo.ucOpen == TRUE) && (caminfo.usSlotId == usSlotId))
	{
		return caminfo.nDevId;
	}
	else
#endif
	{
		return -1;
	}
}


/*****************************************************************
 * Function	: DRV_CI_GetCAMDetect
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
unsigned char CamEnabled = 1;
unsigned char DRV_CI_GetCAMDetect (int nDevId)
{
	DI_ERR_CODE 	Ret = DI_ERR_OK;
	GPIO_STATE_t		nCI_CD1 = 1;
	GPIO_STATE_t		nCI_CD2 = 1;

	UNREFENCED_PARAM(nDevId);
	PrintEnter();

	Ret = DRV_GPIO_Read(GPIO_ID_nCI_CD1, &nCI_CD1);
	if (Ret != DI_ERR_OK)
	{
		PrintError("[%s] ERROR(%#lx) In DDI_GPIO_Read() : GPIO_nCI_CD1\r\n", __func__, Ret);
		return	CAM_NO_DETECT;
	}

	Ret = DRV_GPIO_Read(GPIO_ID_nCI_CD2, &nCI_CD2);
	if (Ret != DI_ERR_OK)
	{
		PrintError("[%s] ERROR(%#lx) In DDI_GPIO_Read() : GPIO_nCI_CD2\r\n", __func__, Ret);
		return	CAM_NO_DETECT;
	}

	PrintDebug("[%s]nCI_CD1(%d), nCI_CD2(%d) \n", __func__, nCI_CD1, nCI_CD2);

	if ((nCI_CD1==0) && (nCI_CD2==0) && (CamEnabled == 1))	// nCI_CD1, nCI_CD2 모두 '0' 이어야 cam 이 올바르게 detect 된 것임 ~ !!!
		return CAM_DETECT;
	else
		return CAM_NO_DETECT;

	PrintExit();

}

/*****************************************************************
 * Function	: DRV_CI_GetCAMStatus
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
unsigned short DRV_CI_GetCAMStatus(int nDevId)
{
	unsigned short retVal = 0;

	UNREFENCED_PARAM(nDevId);

	retVal = caminfo.usStatus;

	return retVal;
}

void DRV_CI_DisconnectCAM(void)
{
	CamEnabled = 0;
}

void DRV_CI_ConnectCAM(void)
{
	CamEnabled = 1;
}


/*****************************************************************
 * Function	: DRV_CI_Reset_CAM
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DRV_CI_Reset_CAM (int nDevId)
{
	UNREFENCED_PARAM(nDevId);
	PrintEnter();

	/* CAM ReSeT Signal ACTIVE */
	DRV_GPIO_Write(GPIO_ID_CI_RST, CAM_RESET_ON);

	VK_TASK_Sleep(CAM_RESET_TIME);

	/* CAM ReSeT Signal INACTIVE */
	DRV_GPIO_Write(GPIO_ID_CI_RST, CAM_RESET_OFF);

	PrintExit();

	return DI_ERR_OK;
}


/*****************************************************************
 * Function	: DRV_CI_PowerOff_CAM
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DRV_CI_PowerOff_CAM (int nDevId)
{
	UNREFENCED_PARAM(nDevId);
	/* CAM Module POWER OFF */
	DRV_GPIO_Write(GPIO_ID_CI_PWR, CAM_POWER_OFF);

	return DI_ERR_OK;
}


/*****************************************************************
 * Function	: DRV_CI_PowerOn_CAM
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DRV_CI_PowerOn_CAM (int nDevId)
{
	UNREFENCED_PARAM(nDevId);
	/* CAM Module POWER OFF */
	DRV_GPIO_Write(GPIO_ID_CI_PWR, CAM_POWER_ON);

	return DI_ERR_OK;
}

/*****************************************************************
 * Function		: DI_CI_ReadIo
 * Description	: CAM I/O Read
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DRV_CI_ReadIo(int nDevId, unsigned long ulOffset, unsigned char *pucData)
{
	PrintEnter();

	if(pucData == NULL)
	{
		PrintError("[%s]pucData is NULL~!!\r\n", __func__);
		return DI_ERR_ERROR;
	}

	if(DRV_CI_GetCAMDetect(nDevId)!=CAM_DETECT)
	{
		PrintError("[%s]CAM is Extracted~!!\r\n", __func__);
		return DI_ERR_ERROR;
	}
#if	defined(DRV_CI_USING_MMAP)
	if (!acquire_ebi_sem())
	{

	if (s_nEbiMode != DRV_CI_MODE_IO)
	{
		P_CI_EBI_CS2_SelectMode(DRV_CI_MODE_IO);
		PrintDebug("[%s] Change EBI_CS2 Mode : MEMORY -> IO\n", __func__);
	}

	ulOffset &= 0xfff;	//[11:0]


		//DRV_GPIO_Write(GPIO_ID_nCI_OUTEN, CAM_ENABLE);

		// IO Read~!
		*pucData = CIMEM_Read(g_CI_MMAP_Handle, ulOffset);

		//DRV_GPIO_Write(GPIO_ID_nCI_OUTEN, CAM_DISABLE);

		release_ebi_sem();
	}
#endif

	PrintExit();

	return DI_ERR_OK;
}

/*****************************************************************
 * Function		: DI_CI_WriteIo
 * Description	: CAM I/O Write
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DRV_CI_WriteIo(int nDevId, unsigned long ulOffset, unsigned char ucValue)
{
	PrintEnter();

	UNREFENCED_PARAM(ucValue);
	if(DRV_CI_GetCAMDetect(nDevId)!=CAM_DETECT)
	{
		PrintError("[%s] CAM is Extracted~!!\r\n", __func__);
		return DI_ERR_ERROR;
	}

#if	defined(DRV_CI_USING_MMAP)
	if (!acquire_ebi_sem())
	{
	if (s_nEbiMode != DRV_CI_MODE_IO)
	{
		P_CI_EBI_CS2_SelectMode(DRV_CI_MODE_IO);
		PrintDebug("[%s] Change EBI_CS2 Mode : MEMORY -> IO\n", __func__);
	}

	ulOffset &= 0xfff;	//[11:0]


		//DRV_GPIO_Write(GPIO_ID_nCI_OUTEN, CAM_ENABLE);

		// IO Write
		CIMEM_Write(g_CI_MMAP_Handle, ulOffset, ucValue);

		//DRV_GPIO_Write(GPIO_ID_nCI_OUTEN, CAM_DISABLE);

		release_ebi_sem();
	}
#endif
	PrintExit();

	return DI_ERR_OK;
}

/*****************************************************************
 * Function		: DI_CI_ReadMem
 * Description	: CAM (Attribute) Memory Read
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DRV_CI_ReadMem(int nDevId, unsigned long ulOffset, unsigned char *pucData)
{
	unsigned short 	usMemAddr;

	PrintEnter();
	if(pucData == NULL)
	{
		PrintError("[%s]pucData is NULL~!!\r\n", __func__);
		return DI_ERR_ERROR;
	}

	if(DRV_CI_GetCAMDetect(nDevId)!=CAM_DETECT)
	{
		PrintError("[%s]CAM is Extracted~!!\r\n", __func__);
		return DI_ERR_ERROR;
	}

	/* Attribute memory(CIS etc..) is located only in even bytes consecutively. */
	usMemAddr = ((ulOffset << 1) & 0xffe);
	//usMemAddr = ((ulOffset << 1) & 0xfffe);
	//usMemAddr = ulOffset<<1;

#if	defined(DRV_CI_USING_MMAP)
	if (!acquire_ebi_sem())
	{

		if (s_nEbiMode != DRV_CI_MODE_MEMORY)
		{
			P_CI_EBI_CS2_SelectMode(DRV_CI_MODE_MEMORY);
			PrintDebug("[%s] Change EBI_CS2 Mode : IO ->  MEMORY\n",  __func__);
		}

		//DRV_GPIO_Write(GPIO_ID_nCI_OUTEN, CAM_ENABLE);

		// MEMORY Read~!
		*pucData = CIMEM_Read(g_CI_MMAP_Handle, (uint32_t)usMemAddr);

		//DRV_GPIO_Write(GPIO_ID_nCI_OUTEN, CAM_DISABLE);

		release_ebi_sem();
	}
#endif
	PrintExit();
	return DI_ERR_OK;
}

/*****************************************************************
 * Function		: DI_CI_WriteMem
 * Description	: CAM (Attribute) Memory Write
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
DI_ERR_CODE DRV_CI_WriteMem(int nDevId, unsigned long ulOffset, unsigned char ucValue)
{
	unsigned short 	usMemAddr;

	PrintEnter();

	UNREFENCED_PARAM(ucValue);
	if(DRV_CI_GetCAMDetect(nDevId)!=CAM_DETECT)
	{
		PrintError("[%s] CAM is Extracted~!!\r\n", __func__);
		return DI_ERR_ERROR;
	}

	/* Attribute memory(CIS etc..) is located only in even bytes consecutively. */
	usMemAddr = ((ulOffset << 1) & 0xffe);
	//usMemAddr = ((ulOffset << 1) & 0xfffe);
	//usMemAddr = ulOffset<<1;

#if	defined(DRV_CI_USING_MMAP)
	if (!acquire_ebi_sem())
	{

	if (s_nEbiMode != DRV_CI_MODE_MEMORY)
	{
		P_CI_EBI_CS2_SelectMode(DRV_CI_MODE_MEMORY);
		PrintDebug("[%s] Change EBI_CS2 Mode : IO ->  MEMORY\n", __func__);
	}


		//DRV_GPIO_Write(GPIO_ID_nCI_OUTEN, CAM_ENABLE);

		// MEMORY Write
		CIMEM_Write(g_CI_MMAP_Handle, (uint32_t)usMemAddr, ucValue);

		//DRV_GPIO_Write(GPIO_ID_nCI_OUTEN, CAM_DISABLE);

		release_ebi_sem();
	}
#endif
	PrintExit();

	return DI_ERR_OK;
}


/*****************************************************************
 * Function		: P_CI_EBI_CS2_SelectMode
 * Description
 * Inputs
 * Outputs
 * Returns
 *****************************************************************/
static void P_CI_EBI_CS2_SelectMode(DRV_CI_MODE nMode)
{
	PrintEnter();

#if	defined(DRV_CI_USING_MMAP)
	PrintDebug("[%s]BCHP_EBI_CS_CONFIG_2 = %#lx\n", __func__, CIREG_Read(g_hReg, OFFSET_BCHP_EBI_CS_CONFIG_2));

	if (nMode == DRV_CI_MODE_MEMORY)
	{
		CIREG_Write(g_hReg,
				OFFSET_BCHP_EBI_CS_BASE_2,
				PHYS_EBI_CS2_BASE|PHYS_EBI_CS2_SIZE_1MB);

		CIREG_Write(g_hReg, OFFSET_BCHP_EBI_CS_CONFIG_2,
							( EBI_mem_io_MASK
							// TTL 로 구현된 CI 에서는 WAIT 핀이 CAM insert/extract 시 bus hold 로 작용하여, 문제가 됨. 따라서, 사용하지 않음.
							//| BCHP_EBI_CS_CONFIG_2_ta_wait_MASK
							| 0x18740000
							| CI_BUS_ENDIAN
							| EBI_cs_hold_MASK
							| EBI_split_cs_MASK
							| EBI_enable_MASK ) );
	}
	else	//DDI_CI_MODE_IO
	{
		CIREG_Write(g_hReg,
				OFFSET_BCHP_EBI_CS_BASE_2,
				PHYS_EBI_CS2_BASE|PHYS_EBI_CS2_SIZE_8kB);

		CIREG_Write(g_hReg, OFFSET_BCHP_EBI_CS_CONFIG_2,
							( EBI_mem_io_MASK
							// TTL 로 구현된 CI 에서는 WAIT 핀이 CAM insert/extract 시 bus hold 로 작용하여, 문제가 됨. 따라서, 사용하지 않음.
							//| BCHP_EBI_CS_CONFIG_2_ta_wait_MASK
							| 0x18740000
							| CI_BUS_ENDIAN
							| EBI_cs_hold_MASK
							| EBI_split_cs_MASK
							| EBI_enable_MASK
							| EBI_we_ctl_MASK ) );
	}
	PrintDebug("[%s]BCHP_EBI_CS_CONFIG_2 = %#lx\n", __func__, CIREG_Read(g_hReg, OFFSET_BCHP_EBI_CS_CONFIG_2));

#endif

	s_nEbiMode = nMode;

	PrintExit();

	return;
}


////////////////////CI TEST CODE//////////////////////////////////
#define TPL_LINKTARGET 0x13
#define TPL_DEVICE_0A 0x1d
#define TPL_DEVICE_0C 0x1c
#define TPL_VERS_1 0x15
#define TPL_CONFIG 0x1a
#define CCST_CIF 0xc0
#define TPL_CFTABLE_ENTRY 0x1b
#define TPL_END 0xff
#define TPL_NO_LINK 0x14

#define NEXUS_DVB_CI_MANUFACTURER_NAME_MAX 128
#define NEXUS_DVB_CI_PRODUCT_NAME_MAX 128
#define NEXUS_DVB_CI_ADDITIONAL_INFO_MAX 128

#define INCREMENT_INDEX(i,val,next) do { if ( (i)+(val) >= (1024*1024) || (i)+(val)>(next) ) goto overflow; (i) += (val); } while (0)

char manufacturerName[NEXUS_DVB_CI_MANUFACTURER_NAME_MAX];
char productName[NEXUS_DVB_CI_PRODUCT_NAME_MAX];
char additionalInfo[NEXUS_DVB_CI_ADDITIONAL_INFO_MAX];
unsigned char corValue;
unsigned int  corAddr;
HBOOL validCis;
HBOOL vpp5v;

int DRV_CI_ParseCis(unsigned char *paramCIS)
{
    unsigned char tuple, cislink;
    unsigned char noLinkFlag=0;
    int i, j, k, nextLink, cc=0;
    unsigned char byte;
    bool foundCor=false, foundCen=false;

#if 1
    DI_UART_Print("CIS DUMP\n");
	for ( i = 16; i < 256; i+=16 )
    {
    	k=0;
	DI_UART_Print("[0x%03x] ", (i-16));
	while(k<2)
	{
		if(k==0)
		{
			for(j=(i-16); j<i;j++)
			{
				DI_UART_Print("0x%02x ",paramCIS[j]);
			}
			k++;
		}
		else if(k==1)
		{
			DI_UART_Print("| ");
			for(j=(i-16); j<i;j++)
			{
				if(paramCIS[j] >= 0x20)
					DI_UART_Print("%c",paramCIS[j]);
				else
					DI_UART_Print(".");
			}
			k++;
		}
	}
	DI_UART_Print("\n");
    }
#endif

    for ( i = 0;; )
    {
        DI_UART_Print("CIS Parsing - offset %d\n", i);
        tuple = paramCIS[i];
        i++;
        cislink = paramCIS[i];
        i++;
        nextLink = i + cislink;

        switch ( tuple )
        {
        case TPL_VERS_1:
            DI_UART_Print("TPL_VERS_1: major %x, minor %x.", paramCIS[i], paramCIS[i+1]);
            INCREMENT_INDEX(i,2,nextLink);
            j=0;
            while ( (byte = paramCIS[i]) )
            {
                if (byte == 0xff)
                    break;
                if ( j < NEXUS_DVB_CI_MANUFACTURER_NAME_MAX-1 )
                {
                    manufacturerName[j++] = byte;
                }
                INCREMENT_INDEX(i,1,nextLink);
            }
            /* Skip NUL byte */
            INCREMENT_INDEX(i,1,nextLink);
            DI_UART_Print("Manufacturer Name: '%s'", manufacturerName);
            j=0;
            while ( (byte = paramCIS[i]) )
            {
                if (byte == 0xff)
                    break;
                if ( j < NEXUS_DVB_CI_PRODUCT_NAME_MAX-1 )
                {
                    productName[j++] = byte;
                }
                INCREMENT_INDEX(i,1,nextLink);
            }
            /* Skip NUL byte */
            INCREMENT_INDEX(i,1,nextLink);
            DI_UART_Print("Product Name: '%s'", productName);
            j=0;
            while ( (byte = paramCIS[i]) )
            {
                if (byte == 0xff)
                    break;
                if ( j < NEXUS_DVB_CI_ADDITIONAL_INFO_MAX-1 )
                {
                    additionalInfo[j++] = byte;
                }
                INCREMENT_INDEX(i,1,nextLink);
            }
            DI_UART_Print("Additional Info: '%s'", additionalInfo);
            break;

        case TPL_CONFIG:
        {
            unsigned char rmsz, rasz, last;
            unsigned int  corVal;

            rmsz = ((paramCIS[i]&0x3c)>>2);
            rasz = (paramCIS[i]&0x03);
            INCREMENT_INDEX(i,1,nextLink);
            last = (paramCIS[i]&0x3f);
            INCREMENT_INDEX(i,1,nextLink);

            DI_UART_Print("rmsz = %x, rasz = %x, last = %x ", rmsz, rasz, last);

            corVal = 0;
            for (j=0; j<=rasz; j++)
                corVal |= ((paramCIS[i+j])<<(j*8));
            corVal &= 0x03ffffff;
            DI_UART_Print("cor address offset 0x%x ", corVal);
            corAddr = corVal;
            INCREMENT_INDEX(i,j,nextLink);

            if ( (paramCIS[i] & 0x01) == 0 )
            {
                DI_UART_Print(("COR not present "));
            }
            foundCor = true;
            INCREMENT_INDEX(i,(rmsz+1),nextLink);
            if ( paramCIS[i] != CCST_CIF )
            {
                DI_UART_Print("CCST_CIF not present ");
                return -1;
            }
            INCREMENT_INDEX(i,2,nextLink);
            #if 0
            /* CableCard - useful for HPNx testing */
            if ( handle->pBaseAddress[i] != 0x41 || handle->pBaseAddress[i+2] != 0x03 )
            #else
            /* DVB-CI */
            if ( paramCIS[i] != 0x41 || paramCIS[i+1] != 0x02 )
            #endif
            {
                DI_UART_Print("CCST_CIF IFN not correct %x %x ", paramCIS[i], paramCIS[i+1]);
                return -1;
            }

            #if 0
            /* Useful for debug */
            INCREMENT_INDEX(i,4,nextLink);
            BDBG_MSG(("CCST_CIF: Interface Description: "));
            while ( handle->pBaseAddress[i] && i < nextLink)
            {
                BDBG_MSG(("   %c", handle->pBaseAddress[i]));
                INCREMENT_INDEX(i,2,nextLink);
            }
            #endif
        }
        break;
        case TPL_CFTABLE_ENTRY:
        {
            unsigned char fs, pdParSel, normv;

            corValue = 0x40 | (paramCIS[i] & 0x3f);
            foundCen = true;
            DI_UART_Print("CISTPL_CFTABLE_ENTRY: TPCE_INDX %x ", corValue);
            INCREMENT_INDEX(i,1,nextLink);  /* Points to TPCE_IF */
            DI_UART_Print("CISTPL_CFTABLE_ENTRY: TPCE_IF %x ", paramCIS[i]);
            INCREMENT_INDEX(i,1,nextLink);  /* Points to TPCE_FS */
            fs = paramCIS[i];
            DI_UART_Print("CISTPL_CFTABLE_ENTRY: TPCE_FS %x ", fs);

            fs &= 0x03; /* extract PD. */
            switch (fs)
            {
            case 0x02: /* VCC and VPP PD. */
                INCREMENT_INDEX(i,1,nextLink);  /* points to VCC PD par sel */
                pdParSel = paramCIS[i];
                DI_UART_Print("CISTPL_CFTABLE_ENTRY: VCC TPCE_PD Par Sel %x ", pdParSel);
                INCREMENT_INDEX(i,1,nextLink);  /* points to VCC PD par */
                for (j=0; j<8; j++)
                    if ((1<<j) & pdParSel)
                    {
                        while (paramCIS[i] & 0x80)
                            INCREMENT_INDEX(i,1,nextLink);
                        INCREMENT_INDEX(i,1,nextLink);
                    }
                pdParSel = paramCIS[i];
                INCREMENT_INDEX(i,1,nextLink);
                DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP TPCE_PD Par Sel %x ", pdParSel);
                if (pdParSel & 1)
                {
                    normv = paramCIS[i];
                    DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP TPCE_PD normV %x ", normv);
                    if ((normv & 0x07) == 5)
                    {
                        /* exponent is 1V. */
                        normv >>= 3;
                        normv &= 0x0f;
                        if (normv >= 6 && normv < 8) /* 6:3, 7:3.5. */
                        {
                            DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP norminal 3.3V ");
                            vpp5v = FALSE;
                        }
                        else if(normv >= 0xa && normv < 0xc) /* a:5, b:5.5 */
                        {
                            DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP norminal 5V ");
                        }
                        else
                            DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP TPCE_PD normV Mantissa out of range! use 5V for Vpp by default. ");
                    }
                    else
                        DI_UART_Print("CISTPL_CFTABLE_ENTRY: VPP TPCE_PD normV exponent not 1V! use 5V for Vpp by default. ");
                }
                else
                    DI_UART_Print("CISTPL_CFTABLE_ENTRY: NO VPP TPCE_PD normV, use 5V for Vpp by default. ");

                break;

            default:
                DI_UART_Print("CISTPL_CFTABLE_ENTRY: NO TPCE_PD for VPP, use 5V for Vpp by default. ");
                break;
            }
        }
		break;
        case TPL_NO_LINK:
            noLinkFlag = 1;
            break;
        case TPL_END:
            if ( noLinkFlag == 0 )
                DI_UART_Print("have not rcved NO_LINK before END ");
            goto done;
        default:
            if (cc++ < 50)
            {
                DI_UART_Print("CIS Tuple %x received. ", tuple);
            }
            else
            {
                DI_UART_Print("Too many tuples ");
                return -1;
            }
            break;
        }

        i = nextLink;
    }

done:
    if ( !foundCor )
    {
        DI_UART_Print("Did not find CISTPL_CONFIG ");
        return -1;
    }
    if ( !foundCen )
    {
        DI_UART_Print("Did not find CISTPL_CFTABLE_ENTRY ");
        return -1;
    }

    validCis = true;

    return 0;

overflow:
    DI_UART_Print("Overflow processing CIS ");
    return -1;
}


#if defined(EBI_CONFLICT_TEST)
//#define  EBI_READ_WRITE_TEST
#define RD_BLK_SIZE (32*1024)
static void P_CI_EBITrafficGeneratingTask (void *param)
{
	int mtd_fd;
	char *pBuffer;
	int ret = 0;
#if defined(EBI_READ_WRITE_TEST)
	int i;
	char bWrite = 1;
#endif
	BSTD_UNUSED(param);
	pBuffer = DD_MEM_Alloc(RD_BLK_SIZE);
	while(1)
	{
#if defined(EBI_READ_WRITE_TEST)
		if(bWrite != 0) //Write 1Block
		{
			//DI_UART_Print("Generate EBI traffic (WRITE MODE)......\n");
			bWrite = 0;
			for(i=0;i<RD_BLK_SIZE;i++)
				pBuffer[i] = (i%255);

			mtd_fd = open("/dev/mtdblock5", O_RDWR | O_SYNC );
			if( mtd_fd < 0 )
			{
				DI_UART_Print( "Failed opening mtdblock to write!\n" );
				continue;
			}
			lseek(mtd_fd,0,SEEK_SET);
			ret = write(mtd_fd,pBuffer,RD_BLK_SIZE);
			close(mtd_fd);
		}
		else //Read 1Block
		{
			bWrite = 1;
#endif
			//DI_UART_Print("Generate EBI traffic (READ MODE)......\n");
			mtd_fd = open("/dev/mtdblock5", O_RDWR | O_SYNC );
			if( mtd_fd < 0 )
			{
				DI_UART_Print( "Failed opening mtdblock to read!\n" );
				continue;
			}
			lseek(mtd_fd,0,SEEK_SET);
			ret = read(mtd_fd,pBuffer,RD_BLK_SIZE);
			close(mtd_fd);
#if defined(EBI_READ_WRITE_TEST)
			for(i=0;i<RD_BLK_SIZE;i++)
			{
				if((unsigned char)pBuffer[i] != (unsigned char)(i%255))
				{
					DI_UART_Print("DATA COMPARE ERROR!(%dth data %x %x)\n", i,(unsigned char)pBuffer[i] ,(unsigned char)(i%255));
					while(1);
				}
			}
		}
#endif
		VK_TASK_Sleep(100);
	}
}

DRV_Error DRV_CI_StartEBITrafficTask(void)
{
	HUINT32 err;
	HULONG taskId;

	err = VK_TASK_Create(P_CI_EBITrafficGeneratingTask,
						(unsigned long)CI_TASK_PRIORITY,
						(unsigned long)DI_CI_TASK_STACKSIZE,
						"EbiTrafficTask",
						NULL,
						&taskId, FALSE);
	if( err != VK_OK ){
		PrintError("DRV_CI_StartEBITrafficTask() Error(0x%x) in VK_TASK_Create()\n\r", err);
		return DRV_ERR;
	}

	err = VK_TASK_Start(taskId);
	if(err != VK_OK){
		VK_TASK_Destroy(taskId);
		PrintError("DRV_CI_StartEBITrafficTask() Error(0x%x) in VK_TASK_Start()\n\r", err);
		return DRV_ERR;
	}
	return DRV_OK;
}

#endif //EBI_CONFLICT_TEST


#if defined(CONFIG_REMUX_EXT_CLOCK)
DRV_Error DRV_CI_SetRemuxExtinalClock(void)
{
#if (NEXUS_VERSION < 1401)
	unsigned long regAddr, regVal;

	PrintEnter();
	/* Select source for MCLK to PLL1_ch1(=AUD_FMM_PLL1 channel 1) */
	regAddr = (BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_BASE + (1 * BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_ARRAY_ELEMENT_SIZE / 8));
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~(BCHP_MASK(AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, PLLCLKSEL));
	regVal |= BCHP_FIELD_DATA(AUD_FMM_OP_CTRL_MCLK_CFG_EXTi, PLLCLKSEL, BCHP_AUD_FMM_OP_CTRL_MCLK_CFG_EXTi_PLLCLKSEL_PLL1_ch1);
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL1_MACRO.MACRO_SELECT = User */
	regAddr = BCHP_AUD_FMM_PLL1_MACRO;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~(BCHP_MASK(AUD_FMM_PLL0_MACRO, MACRO_SELECT));
	regVal |= BCHP_FIELD_ENUM(AUD_FMM_PLL0_MACRO, MACRO_SELECT, User);
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_CONTROL_0.USER_UPDATE_DIVIDERS = 0 */
	regAddr = BCHP_AUD_FMM_PLL1_CONTROL_0;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~(BCHP_MASK(AUD_FMM_PLL0_CONTROL_0, USER_UPDATE_DIVIDERS));
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_NDIV.NDIV_INT = ndivInt */
	regAddr = BCHP_AUD_FMM_PLL1_USER_NDIV;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~(BCHP_MASK(AUD_FMM_PLL0_USER_NDIV, NDIV_INT));
	regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_NDIV, NDIV_INT, 59);
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_CONTROL_0.USER_UPDATE_DIVIDERS = 1 */
	regAddr = BCHP_AUD_FMM_PLL1_CONTROL_0;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~(BCHP_MASK(AUD_FMM_PLL0_CONTROL_0, USER_UPDATE_DIVIDERS));
	regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_CONTROL_0, USER_UPDATE_DIVIDERS, BCHP_AUD_FMM_PLL0_CONTROL_0_USER_UPDATE_DIVIDERS_Update_now);
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch0.MDIV = MdivCh0 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch0;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~(BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch0, MDIV));
	regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch0, MDIV, 10);
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch0.LOAD_EN = 1 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch0;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch0, LOAD_EN, 1 );
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch0.LOAD_EN = 0 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch0;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~(BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch0, LOAD_EN));
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch1.MDIV = MdivCh1 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch1;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch1, MDIV);
	regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch1, MDIV, 10);
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch1.LOAD_EN = 1 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch1;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch1, LOAD_EN);
	regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch1, LOAD_EN, 1 );
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch1.LOAD_EN = 0 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch1;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch1, LOAD_EN);
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch2.MDIV = MdivCh2 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch2;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch2, MDIV);
	regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch2, MDIV, 20);
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch2.LOAD_EN = 1 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch2;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch2, LOAD_EN);
	regVal |= BCHP_FIELD_DATA(AUD_FMM_PLL0_USER_MDIV_Ch2, LOAD_EN, 1 );
	REG_Write32(g_hReg, regAddr, regVal);

	/* AUD_FMM_PLL0_USER_MDIV_Ch2.LOAD_EN = 0 */
	regAddr = BCHP_AUD_FMM_PLL1_USER_MDIV_Ch2;
	regVal = REG_Read32(g_hReg, regAddr);
	regVal &= ~BCHP_MASK(AUD_FMM_PLL0_USER_MDIV_Ch2, LOAD_EN);
	REG_Write32(g_hReg, regAddr, regVal);
#else
	NEXUS_AudioOutputPllSettings audioOutputPllSettings;
	PrintEnter();

	NEXUS_AudioModule_EnableExternalMclk(1, NEXUS_AudioOutputPll_e1, NEXUS_ExternalMclkRate_e128Fs);
	NEXUS_AudioOutputPll_GetSettings(NEXUS_AudioOutputPll_e1, &audioOutputPllSettings);

	audioOutputPllSettings.mode = NEXUS_AudioOutputPllMode_eCustom;
	audioOutputPllSettings.modeSettings.vcxo.vcxo = NEXUS_Vcxo_e0;
	audioOutputPllSettings.modeSettings.custom.value[0] = 59;
	audioOutputPllSettings.modeSettings.custom.value[1] = 10;
	NEXUS_AudioOutputPll_SetSettings(NEXUS_AudioOutputPll_e1, &audioOutputPllSettings);
#endif

	PrintExit();
	return DRV_OK;
}
#endif

/*********************** End of File **********************************/
#endif

