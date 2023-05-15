/**
 * drv_remocon.c
*/

/**
 * @defgroup		DRV_RC DRV_RC : DRV_RC module
 * @author		Jin-hong Kim
 * @note			DRV_RC APIs
 * @brief			Define broadcom dependent DRV_RC
 * @file			drv_remocon.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils headers */
#include "linden_trace.h"

/* include  drv headers */
#include "drv_remocon.h"
#include "nvram_map.h"
#include "drv_nvram.h"
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#include "di_key.h"
#include "drv_remocon_multi_custom_code.h"
#endif

#include "nexus_platform.h"
#include "nexus_ir_input.h"
//#include <assert.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif


#include "vkernel.h"
#include "taskdef.h"

/* This is a temporaily code because nexus code is not exist */
#if defined(CONFIG_RCU_MULTI_DECODER)
#include "breg_mem.h"
#include "bchp_pm_aon.h"
#endif
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
#include "drv_sharedmem.h"
#endif


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DRV_REMOCON

#if defined(CONFIG_DEVICE_MODE_KERNEL) && defined(CONFIG_RCU_MULTI_DECODER)
#if defined(CONFIG_DI_SHARED_LIB)
typedef struct
{
	unsigned int addr;
	unsigned int value;
}__attribute__((packed)) ioctl_driver_info;

#define IOCTLDRIVER_MAGIC 'H'
#define IOCTLGPIO_READ						_IOR(IOCTLDRIVER_MAGIC, 1, ioctl_driver_info)
#define IOCTLGPIO_WRITE 					_IOW(IOCTLDRIVER_MAGIC, 2, ioctl_driver_info)
#endif	//defined(CONFIG_DI_SHARED_LIB)
#endif	//defined(CONFIG_DEVICE_MODE_KERNEL)

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
NEXUS_IrInputHandle gIrHandle;

#if defined(CONFIG_RCU_MULTI_DECODER)
NEXUS_IrInputHandle gIrHandle2;
#endif

#if defined(CONFIG_RM_301)
char RCU_NAME[] = "RM-301";
#elif defined(CONFIG_RM_F01)
char RCU_NAME[] = "RM-F01";
#elif defined(CONFIG_RM_F02)
char RCU_NAME[] = "RM-F02";
#elif defined(CONFIG_RM_F04)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RM_E06)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RM_E08)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RM_E09)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RM_E10)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RM_G01)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RM_G02)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RS_538)
char RCU_NAME[] = "RS-538";
#elif defined(CONFIG_RM_106) || defined(CONFIG_RM_108) || defined(CONFIG_RM_G01) || defined(CONFIG_RM_G07) || defined(CONFIG_RM_G08) || defined(CONFIG_R_838)
char RCU_NAME[] = "RM-106";
#elif defined(CONFIG_RS_636)
char RCU_NAME[] = "RS-636";
#elif defined(CONFIG_URC_39741RJ2_001) || defined(CONFIG_RC_05)
char RCU_NAME[] = "URC-39741RJ2-001";
#elif defined(CONFIG_RC_H101)
char RCU_NAME[] = "RM-F01";
#elif defined(CONFIG_R_J03)
char RCU_NAME[] = "RJ-03";
#elif defined(CONFIG_RM_H04S) || defined(CONFIG_RM_H05S) || defined(CONFIG_RM_H04S_E08)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RM_I02S)
char RCU_NAME[] = "RM-F04";
#elif defined(CONFIG_RM_L01U)
char RCU_NAME[] = "RM-L01U";
#elif defined(CONFIG_R_C01)
char RCU_NAME[] = "R-C01";
#elif defined(CONFIG_URC_178200)
char RCU_NAME[] = "URC-178200";
#elif defined(CONFIG_RM_M02)
char RCU_NAME[] = "RM-M02";
#elif defined(CONFIG_RM_M03)
char RCU_NAME[] = "RM-M03";
#elif defined(CONFIG_RM_M04)
char RCU_NAME[] = "RM-M04";
#elif defined(CONFIG_RM_M08)
char RCU_NAME[] = "RM-M08";
#elif defined(CONFIG_RM_M14)
char RCU_NAME[] = "RM-M14";
#elif defined(CONFIG_RM_H09U)
char RCU_NAME[] = "RM-H09U";
#elif defined(CONFIG_RUWIDO_R97)
char RCU_NAME[] = "RUWIDO_R97";
#elif defined(CONFIG_R_843)
char RCU_NAME[] = "R-843";
#elif defined(CONFIG_R_849)
char RCU_NAME[] = "R-849";
#elif defined(CONFIG_R_861)
char RCU_NAME[] = "R-861";
#elif defined(CONFIG_NOVA_CE)
char RCU_NAME[] = "NOVA-CE";
#elif defined(CONFIG_RM_N01)
char RCU_NAME[] = "RM-N01";
#else
#error
#endif

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static pfnDRV_REMOCON_Notify remoconCallback;
#if defined(CONFIG_RCU_MULTI_DECODER)
static pfnDRV_REMOCON_Notify remoconCallback2;

#if defined(CONFIG_DEVICE_MODE_KERNEL)
void *g_hReg = NULL;
#else
extern BREG_Handle g_hReg;
#endif

#endif
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
static void *sharedMem=NULL;
#endif

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if defined(CONFIG_RCU_MULTI_DECODER)
#if defined(CONFIG_DI_SHARED_LIB) && defined(CONFIG_DEVICE_MODE_KERNEL)
static unsigned long FSREG_Read(void *RegHandle, unsigned long ofs)
{
	HAPPY(RegHandle);
	ioctl_driver_info ctrl_info;

	ctrl_info.addr = ofs;
	ctrl_info.value = 0;
	ioctl(g_fd, IOCTLGPIO_READ, &ctrl_info);

	return ctrl_info.value;
}

static void FSREG_Write(void *RegHandle, unsigned int ofs, unsigned int data)
{
	HAPPY(RegHandle);
	ioctl_driver_info ctrl_info;

	ctrl_info.addr = ofs;
	ctrl_info.value = data;
	ioctl(g_fd, IOCTLGPIO_WRITE, &ctrl_info);
}

#else	//defined(CONFIG_DI_SHARED_LIB)

static unsigned long FSREG_Read(void *RegHandle, unsigned long ofs)
{
	HAPPY(RegHandle);
	unsigned long rdValue;
	NEXUS_Platform_Registers_Read_Access(ofs, (unsigned long *)&rdValue);
	return rdValue;
}

static void FSREG_Write(void *RegHandle, unsigned int ofs, unsigned int data)
{
	HAPPY(RegHandle);
	//VK_printf("[hee.usermode] write at 0x%lx value=0x%lx\n", ofs, data);
	NEXUS_Platform_Registers_Write_Access(ofs, data);
}
#endif	//defined(CONFIG_DI_SHARED_LIB)

#else	//defined(CONFIG_DEVICE_MODE_KERNEL)

#define FSREG_Read	BREG_Read32
#define FSREG_Write	BREG_Write32

#endif	//defined(CONFIG_RCU_MULTI_DECODER)


void DRV_P_REMOCON_Callback(void *pParam, int iParam)
{
	BSTD_UNUSED(iParam);
	BSTD_UNUSED(pParam);

	if(remoconCallback == NULL)
		PrintDebug("[DRV_P_irCallback] remocon Callback is empty\n");
	else
		remoconCallback();	//DI_KEY_TASK로 키값 넘겨줌
 }

#if defined(CONFIG_RCU_MULTI_DECODER)
void DRV_P_REMOCON_Callback2(void *pParam, int iParam)
{
	BSTD_UNUSED(pParam);
	BSTD_UNUSED(iParam);

	if(remoconCallback2 == NULL)
		PrintDebug("[DRV_P_irCallback2] remocon Callback is empty\n");
	else
		remoconCallback2();	//DI_KEY_TASK로 키값 넘겨줌
}
#endif

HUINT8 DRV_REMOCON_GetEvents(HINT8 *ucRepeatType, HUINT32 *pucKeyCode, HUINT8 *ulQueueNum)
{
	size_t numEvents = 0;	// 1
	size_t numQueue = 0;
	bool overflow;
	NEXUS_IrInputEvent irEvent[2];
#if defined(CONFIG_RCU_INPUTMODE_RC5)
	NEXUS_IrInputPreambleStatus pStatus;
#endif

	NEXUS_IrInput_GetEvents(gIrHandle, irEvent, 2, &numEvents, &overflow); //irEvent에 키정보를 얻어옴

	PrintDebug("[ DRV_P_REMOCON_GetEvents ] numEvents = %d \n\r",numEvents);

	numQueue = numEvents;

#if defined(CONFIG_RCU_INPUTMODE_RC5)
/*
	PreambleA == 1 인 경우가 S1S2 = "11"
	PreambleB == 1 인 경우가 S1S2 = "10"
*/
	NEXUS_IrInput_GetPreambleStatus(gIrHandle, &pStatus);

	PrintDebug("irCallback: rc: preambleA = %d, preambleB = %d.\n", pStatus.preambleA, pStatus.preambleB);

	/*
		기존 코드는 address code를 고려하지 않고 command code 앞에 field bit을 or 연산하여 DI에 넘겨주도록 구현되었다.
		그래서 address code가 다른 RCU를 사용해도 동작하는 문제가 발생한다.
		기존 코드를 그대로 둔채로 address code를 체크할 경우
		field bit을 or 연산 했기 때문에 address code가 달라진 상태이므로 문제가 발생할 소지가 있다.

		따라서 RC5 Spec에 맞게 field bit을 command code 앞에 끼워넣도록 수정한다.
		DI에 넘겨주는 key 값은 12bit에서 13bit로 변경된다.

		| Toggle Bit	| Address bit		| Field bit	| Command bit	|
		|	1bit		| 	5bits		| 	1bit		| 	6bits		|


		(수정전) irEvent[0].code = irEvent[0].code | 0x40;

		-->

		(수정후) irEvent[0].code = ((irEvent[0].code & 0xFC0)<<1) | (irEvent[0].code & 0x3F) | 0x40 ;
	*/

	PrintDebug("irEvent[0].code = 0x%04x ==> ", irEvent[0].code);

	if (( pStatus.preambleA == 0 ) && ( pStatus.preambleB == 1 ))
	{
		irEvent[0].code = ((irEvent[0].code & 0xFC0)<<1) | (irEvent[0].code & 0x3F) | 0x40 ;
		if(numEvents > 1)
		{
			irEvent[1].code = ((irEvent[1].code & 0xFC0)<<1) | (irEvent[1].code & 0x3F) | 0x40 ;
		}
	}else{
		irEvent[0].code = ((irEvent[0].code & 0xFC0)<<1) | (irEvent[0].code & 0x3F);
		if(numEvents > 1)
		{
			irEvent[1].code = ((irEvent[1].code & 0xFC0)<<1) | (irEvent[1].code & 0x3F);
	}
	}

	PrintDebug("0x%04x\n", irEvent[0].code);

#endif

#if defined(CONFIG_RCU_INPUTMODE_RSTEP)
/* Invert raw data */
	switch(numEvents)
	{
		case 2:
			irEvent[1].code = ~(irEvent[1].code);
			irEvent[0].code = ~(irEvent[0].code);
			if((irEvent[1].interval > 0) && (irEvent[1].interval < 130))
			{
				irEvent[1].repeat = 1;
			}
			else
			{
				irEvent[1].repeat = 0;
			}
			break;

		case 1:
			irEvent[0].code = ~(irEvent[0].code);
			if((irEvent[0].interval > 0) && (irEvent[0].interval < 130))
			{
				irEvent[0].repeat = 1;
			}
			else
			{
				irEvent[0].repeat = 0;
			}
			break;

		default:
			break;
	}
#endif

	switch(numEvents)
	{
		case 2:
			PrintDebug("1st - Remocon is pressed! :0x%x\n",irEvent[0].code);
			PrintDebug("1st - ucRepeatType :0x%x\n",irEvent[0].repeat);
			PrintDebug("2nd - Remocon is pressed! :0x%x\n",irEvent[1].code);
			PrintDebug("2nd - ucRepeatType :0x%x\n",irEvent[1].repeat);
			*ucRepeatType = irEvent[1].repeat? 1:0;
			*pucKeyCode = irEvent[1].code;
			*ulQueueNum = numQueue;
			break;
		case 1:
			PrintDebug("Remocon is pressed! :0x%x\n",irEvent[0].code);
			PrintDebug("ucRepeatType :0x%x\n",irEvent[0].repeat);
			*ucRepeatType = irEvent[0].repeat? 1:0;
			*pucKeyCode = irEvent[0].code;
			*ulQueueNum = numQueue;
			break;
		default:
			break;
	}

   	return (HUINT8)numEvents;
 }

#if defined(CONFIG_RCU_INPUTMODE_XMP) || defined(CONFIG_RCU_INPUTMODE_MULTI_XMP)
static HBOOL P_REMOCON_Xmp_IsRegistryData(HUINT16 usData)
{
	if (usData == DIGITURK_XMP_01_REG_NUM)
	{
		return TRUE;
	}
	else if (usData == URC_178200_XMP_01_REG_NUM)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static DRV_Error P_REMOCON_Xmp_CheckSum(HUINT32 ulCheckSumData, HUINT8 ucCheckSum)
{
	int i;
	HUINT32 ulSum;
	HUINT8 ucCalCheckSum;
	HUINT32 ulCheckSumDataBuffer;

	ulCheckSumDataBuffer = ulCheckSumData;

	/* Calculate Registry Packet Checksum */
	for (i = 0, ulSum = 0; i < 8; i++)
	{
		ulSum += ulCheckSumDataBuffer & 0xF;
		ulCheckSumDataBuffer >>= 4;
	}

	ucCalCheckSum = (HUINT8)(16 - ulSum % 16);
	if (16 == ucCalCheckSum)
	{
		ucCalCheckSum = 0;
	}

	/* Compare Checksums Between Decoded Checksum and Calculated Checksum  */
	if (ucCalCheckSum != ucCheckSum)
	{
		PrintError("[DRV_REMOCON_Xmp_CheckSum] Received Checksum : 0x%x, Calculated Checksum : 0x%x \n", ucCheckSum, ucCalCheckSum);
		return DRV_ERR;
	}
	else
	{
		return DRV_OK;
	}
}

DRV_Error DRV_REMOCON_Xmp_GetEvents(DRV_RCU_XMP_REGISTRY_PACKET *pRegPacket, DRV_RCU_XMP_DATA_PACKET *pDataPacket, HUINT8 *ulQueueNum)
{
	DRV_Error drvErrCode = 0;
	NEXUS_Error nexusErrCode = 0;
	size_t numEvents = 0;
	bool overflow;
	NEXUS_IrInputEvent irEvent[2];
	HUINT8 checkSum;
	HUINT32 checkSumData;
	HUINT16 registryNumberData;
	int i;

#if defined(CONFIG_RCU_INPUTMODE_MULTI_XMP)
	nexusErrCode = NEXUS_IrInput_GetEvents(gIrHandle2, irEvent, 2, &numEvents, &overflow); //irEvent에 키정보를 얻어옴
#else
	nexusErrCode = NEXUS_IrInput_GetEvents(gIrHandle, irEvent, 2, &numEvents, &overflow); //irEvent에 키정보를 얻어옴
#endif
	if(nexusErrCode != NEXUS_SUCCESS)
	{
		PrintError("%s(%d) : NEXUS_IrInput_GetEvents Error(%d)!!!\n", __FUNCTION__, __LINE__, nexusErrCode);
		return DRV_ERR;
	}

	PrintDebug("[%s] numEvents = %d, overflow = %d\n", __FUNCTION__, numEvents, overflow);

	for(i=0; i<(int)numEvents; i++)
	{
		PrintDebug("%d st - XMP Remocon is pressed! :0x%x\n",i, irEvent[i].code);
		checkSum = ((irEvent[i].code & 0x0F000000)>>24);	// Extract Checksum Packet

		checkSumData = (irEvent[i].code & 0xF0FFFFFF);		// Extract Data Without Checksum
		drvErrCode = P_REMOCON_Xmp_CheckSum(checkSumData, checkSum);
		if(drvErrCode == DRV_OK)
		{
			registryNumberData = (HUINT16)(irEvent[i].code & 0x00007FFF); // Extract registery data for checking whether registry data
			if(P_REMOCON_Xmp_IsRegistryData(registryNumberData) == TRUE)
			{
				pRegPacket->RegCode = irEvent[i].code;						// Save Full Packet
				pRegPacket->owner = ((irEvent[i].code & 0xF0000000) >> 28);
				pRegPacket->checksum = checkSum;
				pRegPacket->tag = ((irEvent[i].code & 0x00F00000) >> 20);
				pRegPacket->peripheralId = ((irEvent[i].code & 0x000F0000) >> 16);
				pRegPacket->RegNum = registryNumberData;

				pDataPacket->DataCode = 0;
				pDataPacket->owner = 0;
				pDataPacket->checksum = 0;
				pDataPacket->fRepeat = 0;
				pDataPacket->fmb = 0;
				pDataPacket->tag = 0;
				pDataPacket->KeyData = 0;
				pDataPacket->fCntl = 0;
				pDataPacket->fShift = 0;
				pDataPacket->fAlt = 0;
			}
			else if(pRegPacket->RegNum != 0)
			{
				pDataPacket->DataCode = irEvent[i].code;							// Save Full Packet
				pDataPacket->owner = ((irEvent[i].code & 0xF0000000) >> 28);
				pDataPacket->checksum = checkSum;
				pDataPacket->fRepeat = ((irEvent[i].code & 0x00800000) >> 20);
				pDataPacket->fmb = ((irEvent[i].code & 0x00100000) >> 20);
				pDataPacket->tag = ((irEvent[i].code & 0x000F0000) >> 16);
				pDataPacket->KeyData = ((irEvent[i].code & 0x0000FF00) >> 8);
				pDataPacket->fCntl = ((irEvent[i].code & 0x00000080) >> 4);
				pDataPacket->fShift = ((irEvent[i].code & 0x00000040) >> 4);
				pDataPacket->fAlt = ((irEvent[i].code & 0x00000020) >> 4);
			}
			else
			{
				return DRV_ERR;
			}
		}
		else
		{
			return DRV_ERR;
		}

	}

	*ulQueueNum = numEvents;

   	return DRV_OK;
}
#endif

DRV_Error DRV_REMOCON_Init(void)
{
	NEXUS_IrInputSettings irSettings;
#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
	DRV_IR_SHAREDMEM_t *sharedHandle;
#endif

	NEXUS_IrInput_GetDefaultSettings(&irSettings);	//기본 설정값 가져오기
#if defined(CONFIG_RCU_INPUTMODE_RC5)
	PrintDebug("[ DRV_REMOCON_Init ] NEXUS_IrInputMode_eCirRc5 \n\r");
	irSettings.mode = NEXUS_IrInputMode_eCirRc5;	//RC5 vendor 리모콘 설정

#elif defined(CONFIG_RCU_INPUTMODE_XMP)
	PrintDebug("[ DRV_REMOCON_Init ] NEXUS_IrInputMode_eCirXmp \n\r");
	irSettings.mode = NEXUS_IrInputMode_eCirXmp;	//XMP vendor

#elif defined(CONFIG_RCU_INPUTMODE_RSTEP)
	PrintDebug("[ DRV_REMOCON_Init ] NEXUS_IrInputMode_eCirRstep \n\r");
	irSettings.mode = NEXUS_IrInputMode_eCirRstep;	//Ruwido vendor
	irSettings.repeatFilterTime = 100;
   	irSettings.dataReady.context = NULL;

#else	//Default NEC type
	PrintDebug("[ DRV_REMOCON_Init ] NEXUS_IrInputMode_eCirNec \n\r");
#if defined(CONFIG_TEMP_ROCKYPOM) //CVC_DEMO
	irSettings.mode = NEXUS_IrInputMode_eRemoteB;
#elif defined(CONFIG_RCU_INPUTMODE_SEJIN_56KHZ) // TLF vendor
	irSettings.mode = NEXUS_IrInputMode_eSejin56KhzKbd;
#else
	irSettings.mode = NEXUS_IrInputMode_eCirNec;	//Nec vendor 리모콘 설정
#endif
	irSettings.repeatFilterTime = IR_MAX_REPEAT_PERIOD;
   	irSettings.dataReady.context = NULL;
#endif

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
#else
	irSettings.dataReady.callback = DRV_P_REMOCON_Callback;	//callback 함수 등록
#endif

   	gIrHandle = NEXUS_IrInput_Open(0, &irSettings);	//리모콘 핸들을 얻어옴

#if defined(CONFIG_RCU_MULTI_DECODER)
	NEXUS_IrInput_GetDefaultSettings(&irSettings);
#if defined(CONFIG_RCU_INPUTMODE_MULTI_XMP)
	PrintDebug("[ DRV_REMOCON_Init ] NEXUS_IrInputMode_eCirXmp \n\r");
	irSettings.mode = NEXUS_IrInputMode_eCirXmp;
#endif
	irSettings.channel_number = 1;
	irSettings.dataReady.callback = DRV_P_REMOCON_Callback2;
	gIrHandle2 = NEXUS_IrInput_Open(1, &irSettings);

	/* This is a temporaily code because nexus code is not exist */
{
	HUINT32 ulRegVal = 0;

	ulRegVal = FSREG_Read(g_hReg, BCHP_PM_AON_CONFIG);
	PrintDebug("%s(%d) : Get BCHP_PM_AON_CONFIG Reg = 0x%X \n\r", __FUNCTION__, __LINE__, ulRegVal);

	ulRegVal &= ~BCHP_PM_AON_CONFIG_irr2_in_MASK;
	ulRegVal |= (BCHP_PM_AON_CONFIG_irr2_in_IR_IN0 << BCHP_PM_AON_CONFIG_irr2_in_SHIFT);
	PrintDebug("%s(%d) : Set BCHP_PM_AON_CONFIG Reg = 0x%X \n\r", __FUNCTION__, __LINE__, ulRegVal);

	FSREG_Write(g_hReg, BCHP_PM_AON_CONFIG, ulRegVal);
	ulRegVal = FSREG_Read(g_hReg, BCHP_PM_AON_CONFIG);
	PrintDebug("%s(%d) : Get BCHP_PM_AON_CONFIG Reg = 0x%X \n\r", __FUNCTION__, __LINE__, ulRegVal);
}
#endif

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
	DRV_SHAREDMEM_Init(sizeof(DRV_IR_SHAREDMEM_t),DRV_IR_SHAREDMEM_ID);
	sharedMem=DRV_SHAREDMEM_Setup(sizeof(DRV_IR_SHAREDMEM_t),DRV_IR_SHAREDMEM_ID,true);
	sharedHandle=(DRV_IR_SHAREDMEM_t *)sharedMem;
	sharedHandle->gIrHandle = gIrHandle;
#endif

   	return DRV_OK;
}

#if defined(CONFIG_PRODUCT_MULTI_PROCESS)
DRV_Error DRV_REMOCON_Join(void)
{
	NEXUS_IrInputSettings irSettings;
	DRV_IR_SHAREDMEM_t *sharedHandle;

	sharedMem=DRV_SHAREDMEM_Setup(sizeof(DRV_IR_SHAREDMEM_t),DRV_IR_SHAREDMEM_ID,false);
	sharedHandle=(DRV_IR_SHAREDMEM_t *)sharedMem;
	gIrHandle=sharedHandle->gIrHandle;

	NEXUS_IrInput_GetSettings(gIrHandle, &irSettings);

#if defined(CONFIG_RCU_INPUTMODE_RSTEP)
	PrintDebug("[ DRV_REMOCON_Join ] NEXUS_IrInputMode_eCirRstep \n\r");
	irSettings.mode = NEXUS_IrInputMode_eCirRstep;	//Ruwido vendor
	irSettings.repeatFilterTime = 100;
#else
	irSettings.mode = NEXUS_IrInputMode_eCirNec;
	irSettings.repeatFilterTime = IR_MAX_REPEAT_PERIOD;
#endif
   	irSettings.dataReady.context = NULL;
	irSettings.dataReady.callback = DRV_P_REMOCON_Callback;

	NEXUS_IrInput_SetSettings(gIrHandle, &irSettings);
   	return DRV_OK;
}
#endif

DRV_Error DRV_REMOCON_UnInit(void)
{
#if defined(CONFIG_RCU_MULTI_DECODER)
	NEXUS_IrInput_Close(gIrHandle2);
#endif
	NEXUS_IrInput_Close(gIrHandle);
	NEXUS_Platform_Uninit();
	return DRV_OK;
}

void DRV_REMOCON_RegisterKeyEventCallback(pfnDRV_REMOCON_Notify pfnNotify)
{
	remoconCallback = pfnNotify;
}

#if defined(CONFIG_RCU_MULTI_DECODER)
void DRV_REMOCON_RegisterKeyEventCallback2(pfnDRV_REMOCON_Notify pfnNotify)
{
	remoconCallback2 = pfnNotify;
}
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
DRV_Error DRV_REMOCON_SetWakeupCode(HUINT8 ucKeyCode, HUINT32 *pulCode)
{
	NEXUS_Error nResult;
	NEXUS_IrInputDataFilter irPattern;
	HUINT32 patternWord = 0;
	HUINT8 ucInverseKeyCode = 0;	
#if defined(CONFIG_RCU_INPUTMODE_RSTEP)
	HBOOL  bIgnoreCustomCode = TRUE;
#else
	HBOOL  bIgnoreCustomCode = FALSE;
	HUINT16 usCustomCode = IR_MULTI_CUSTOM_CODE_CUSTOM_CODE_DEFAULT;
#endif
	NEXUS_IrInput_GetDefaultDataFilter(&irPattern);

	ucInverseKeyCode = ~(ucKeyCode);

	PrintDebug("%s(%d) : bIgnoreCustomCode = %d\n", __FUNCTION__, __LINE__, bIgnoreCustomCode);
	PrintDebug("%s(%d) : ucKeyCode = 0x%02X\n", __FUNCTION__, __LINE__, ucKeyCode);
	PrintDebug("%s(%d) : ucInverseKeyCode = 0x%02X\n", __FUNCTION__, __LINE__, ucInverseKeyCode);
#if defined(CONFIG_RCU_INPUTMODE_RSTEP)
	if(bIgnoreCustomCode)
	{
		patternWord = (HUINT32)(ucInverseKeyCode);
		irPattern.patternWord0	= patternWord;
		irPattern.mask0 		= 0xFFFFFF00;
	}

	if( pulCode != NULL )
	{
		*pulCode = (HUINT32)patternWord;
	}	
#else
	if(bIgnoreCustomCode)
	{
		patternWord = (HUINT32)(ucInverseKeyCode << 24 | ucKeyCode << 16);
		irPattern.patternWord0	= patternWord;
		irPattern.mask0 		= 0x0000FFFF;
	}
	else
	{
		(void)DRV_NVRAM_Read(DRV_NVRAM_PARTITION_RW, RCU_CUSTOM_CODE, 0, (HUINT8*)&usCustomCode, sizeof(usCustomCode));
		patternWord = (HUINT32)(ucInverseKeyCode << 24 | ucKeyCode << 16 | usCustomCode);
		irPattern.patternWord0	= patternWord;
		irPattern.mask0 		= 0x00000000;
	}

	if( pulCode != NULL )
	{
		*pulCode = (HUINT32)(ucInverseKeyCode << 24 | ucKeyCode << 16 | usCustomCode);
	}
#endif

	irPattern.patternWord1 = 0;
	irPattern.mask1	= 0;
	irPattern.filterWord[1].enabled = true;
	irPattern.filterWord[1].patternWord = 0;
	irPattern.filterWord[1].mask = 0;

	PrintDebug("%s(%d) : irPattern.patternWord0 = 0x%08X\n", __FUNCTION__, __LINE__, irPattern.patternWord0);
	PrintDebug("%s(%d) : irPattern.mask0 = 0x%08X\n", __FUNCTION__, __LINE__, irPattern.mask0);

	nResult = NEXUS_IrInput_EnableDataFilter(gIrHandle, &irPattern);

	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("Error(0x%x) In NEXUS_IrInput_EnableDataFilter() \n", nResult);
		return DRV_ERR;
	}
	return DRV_OK;
}

DRV_Error DRV_REMOCON_ClearWakeupCode(void)
{
	NEXUS_IrInput_DisableDataFilter(gIrHandle);

	return DRV_OK;
}
#endif

