/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/*
	Miscellaneous functions for Viaccess
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "va_util.h"
#include "va_def.h"

#include "htype.h"
#include "vkernel.h"

#include <hlib.h>

#include "va_main.h"

#include <string.h>

#ifdef CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG
#include "va_op_msg.h"
#endif
#include "pal_sys.h"
#include "pal_param.h"

//#include "pal_eeprom.h"
#include "va_nvm_api.h"

#include "pal_video.h"
#include "pal_audio.h"
#include <_svc_cas_mgr_pal_port.h>
#include <stb_params.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
HUINT32 g_VaUtil_Level = DBG_ASSERT; // | DBG_TRACE; /* DBG_TRACE, DBG_PRINT, DBG_ASSERT 만 사용! */
#endif

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
#define VaPrint(level, message)		(((level) & g_VaUtil_Level) ? ((HxLOG_Print("[VaUtil][%d][%s], ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#define VaError(level, message)		(((level) & g_VaUtil_Level) ? ((HxLOG_Error("[VaUtil][%d][%s] ERROR, ", __LINE__, __FUNCTION__)), (VK_Print message)) : 0)
#else
#define VaPrint(level, message)
#define VaError(level, message)
#endif

#ifdef CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG

// 이미 EEPROM을 쓰도록 출하된 제품을 upgrade할 경우, BBX관련 메시지를 다시 표시 하지 않도록 막기 위한 조치
// 최초 변경시 OTA용으로 한번만 쓰고 다음부턴 쓰지마세요.
// EEPROM을 다른용도로 쓰게되면 아래 define은 이상한 짓을 할 수 있습니다...
//#define DATA_MOVE_FROM_EEPROM_TO_DB

#define VA_DATA_FORMAT_VERSION			1
#define VA_DATA_PATH						OBAMA_DIR_DATA "/cas"
#define VA_DATA_FILENAME					"va.dat"

#define VA_FLASH_PIN_CODE_SIZE			(4+1)

typedef struct {
	HUINT8 ucVersion;		// structure의 version (혹시 나중에 field 추가시 생길 수 있는 문제 대비)
	HUINT8 aucPinCodeData[VA_FLASH_PIN_CODE_SIZE];
#ifdef VA_BBX_INIT_CHECK
	HUINT8 ucBbxInit;
	HUINT8 ucBbxInitDisplayed;
#endif
} VA_DataFile;

/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/

static HUINT32 s_ulAudioMuteFlag;
static HUINT32 s_ulVideoMuteFlag;

static VA_DataFile s_stVaData;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
static HERROR local_VaUtil_UpdateDataFile();
static HERROR local_VaUtil_ReadDataFile();

void VA_UTIL_Init()
{
	s_ulAudioMuteFlag = VA_MUTE_NONE;
	s_ulVideoMuteFlag = VA_MUTE_NONE;

	VA_InitOpMessage();

	local_VaUtil_ReadDataFile();
}

HERROR local_VaUtil_ReadDataFile()
{
	HUINT32 ulFileHandle;
	HUINT32 ulReadSize, ulWriteSize, ulOpMsgNum;
	HCHAR szFilename[64];

	HxFILE_t	hFile;

	if (HLIB_DIR_IsExist(VA_DATA_PATH) == FALSE)
	{
		// folder가 없으면 생성
		HLIB_DIR_Make(VA_DATA_PATH);
	}

	// data 파일을 찾는다.
	snprintf(szFilename, 64, "%s/%s", VA_DATA_PATH, VA_DATA_FILENAME);
	if (HLIB_FILE_Exist(szFilename) == FALSE)
	{
		// 파일이 없는경우
		VA_memset(&s_stVaData, 0, sizeof(VA_DataFile));
		s_stVaData.ucVersion = VA_DATA_FORMAT_VERSION;

#ifdef DATA_MOVE_FROM_EEPROM_TO_DB

	// pin code
//	#define VA_FLASH_PIN_CODE_OFFSET				4
//	#define VA_FLASH_PIN_CODE_SIZE				5	// 0: validity (1=true), 1-4: pin code
	// bbx init check
//	#define VA_FLASH_BBX_INIT_OFFSET				(4+5)
//	#define VA_FLASH_BBX_INIT_SIZE					1	// true/false
//	#define VA_FLASH_BBX_INIT_DISPLAY_OFFSET		(9+1)
//	#define VA_FLASH_BBX_INIT_DISPLAY_SIZE		1	// true/false

		// ucVersion때문에 하나 더 읽는다..
		// PAL_NVRAM_Read 함수로 변경하세요..
		if (MW_EEP_ExtZoneReadData(VA_NVM_EXT_DATA_OFFSET + 3, 8, &s_stVaData) == ERR_OK)
		{
			if (VA_UTIL_GetBbxInit() == TRUE && VA_UTIL_GetBbxInitDisplay() == TRUE)
			{
				// 정상 출하된 제품이라면 bbx init도 되고, 정보도 표시하였을 것임. 그러한 경우에만 EEPROM의 정보를 DB로 옮김.
				s_stVaData.ucVersion = VA_DATA_FORMAT_VERSION;
				local_VaUtil_UpdateDataFile();
			}
		}
#endif
	}
	else
	{
		hFile - HLIB_FILE_Open(ulFileHandle, "r");
		if (hFile == NULL)
		{
			VaError(DBG_TRACE, ("fail to open file(%s)\n", szFilename));
			return ERR_FAIL;
		}

		ulReadSize = HLIB_FILE_Read(hFile, &s_stVaData, sizeof(VA_DataFile), 1);
		if (ulReadSize <= 0)
		{
			VaError(DBG_TRACE, ("fail to read file(%s)\n", szFilename));
			HLIB_FILE_Close(ulFileHandle);
			return ERR_FAIL;
		}

		HLIB_FILE_Close(hFile);

		// 만약 version이 다르다면, 문제가 있을 수 있으므로 파일 내용을 무시한다.
		if (s_stVaData.ucVersion != VA_DATA_FORMAT_VERSION)
		{
			VA_memset(&s_stVaData, 0, sizeof(VA_DataFile));
			s_stVaData.ucVersion = VA_DATA_FORMAT_VERSION;
		}
	}

	return ERR_OK;
}

HERROR local_VaUtil_UpdateDataFile()
{
	HUINT32 		ulFileHandle;
	HUINT32 		ulWriteSize;
	PAL_FS_INT64	nPosOffset;
	HCHAR 			szFilename[64];

	HxFILE_t	hFile;

	snprintf(szFilename, 64, "%s/%s", VA_DATA_PATH, VA_DATA_FILENAME);

	hFile = HLIB_FILE_Open(szFilename, "w");
	if (hFile == NULL)
	{
		VaError(DBG_TRACE, ("fail to open file(%s)\n", szFilename));
		return ERR_FAIL;
	}

	ulWriteSize = HLIB_FILE_Write(hFile, &s_stVaData, 1, sizeof(VA_DataFile));
	if (ulWriteSize <= 0)
	{
		VaError(DBG_TRACE, ("fail to write file(%s)\n", szFilename));
		HLIB_FILE_Close(hFile);
		return VA_ERR;
	}
	HLIB_FILE_Close(hFile);

	return ERR_OK;
}


int VA_UTIL_InvalidatePincode()
{
	int nRet;

	HxSTD_memset(s_stVaData.aucPinCodeData, 0, VA_FLASH_PIN_CODE_SIZE);
	nRet = local_VaUtil_UpdateDataFile();

	return nRet;
}

int VA_UTIL_SetPinCode(HUINT8 *pucPin)
{
	int nRet = VA_ERR_OK;
	HUINT8 aucPinCodeData[VA_FLASH_PIN_CODE_SIZE];

	VaPrint(DBG_PRINT, ("[Enter]\n"));

	// pin code를 XOR하여 최소한의 보호를 한다.
	aucPinCodeData[1] = pucPin[0] ^ 0x3F;
	aucPinCodeData[2] = pucPin[1] ^ 0xF5;
	aucPinCodeData[3] = pucPin[2] ^ 0x77;
	aucPinCodeData[4] = pucPin[3] ^ 0xE3;

	if (s_stVaData.aucPinCodeData[0] == FALSE || VA_memcmp(s_stVaData.aucPinCodeData + 1, aucPinCodeData, 4) != 0)
	{
		s_stVaData.aucPinCodeData[0] = TRUE;
		HxSTD_memcpy(s_stVaData.aucPinCodeData + 1, aucPinCodeData, 4);

		nRet = local_VaUtil_UpdateDataFile();
	}

	VaPrint(DBG_PRINT, ("[Exit]\n"));
	return nRet;
}

int VA_UTIL_GetPinCode(HUINT8 *pucPin)
{
	int nRet = VA_ERR_OK;

	VaPrint(DBG_PRINT, ("[Enter]\n"));

	if (s_stVaData.aucPinCodeData[0] == FALSE)
	{
		// 저장된 pin이 없음
		return VA_ERR;
	}

	VA_memcpy(pucPin, s_stVaData.aucPinCodeData, 4);

	// pin code를 XOR하여 최소한의 보호를 한다.
	pucPin[0] ^= 0x3F;
	pucPin[1] ^= 0xF5;
	pucPin[2] ^= 0x77;
	pucPin[3] ^= 0xE3;

	VaPrint(DBG_PRINT, ("[Exit(%x %x %x %x)]\n", pucPin[0], pucPin[1], pucPin[2], pucPin[3]));
	return VA_ERR_OK;
}

#ifdef VA_BBX_INIT_CHECK

static HBOOL s_bBbxUpdated = FALSE;

#define BBX_DATA_UPDATED		0x3A
#define BBX_DATA_DISPLAYED		0xC5

void VA_UTIL_SetBbxUpdated()
{
	s_bBbxUpdated = TRUE;
}

void VA_UTIL_SetBbxInit()
{
	if (s_bBbxUpdated)
	{
		s_stVaData.ucBbxInit = BBX_DATA_UPDATED;
		s_stVaData.ucBbxInitDisplayed = 0;

		local_VaUtil_UpdateDataFile();

		s_bBbxUpdated = FALSE;
	}
}

void VA_UTIL_SetBbxInitDisplay()
{
	s_stVaData.ucBbxInitDisplayed = BBX_DATA_DISPLAYED;

	local_VaUtil_UpdateDataFile();
}

HBOOL VA_UTIL_GetBbxInit()
{
	if (s_stVaData.ucBbxInit != BBX_DATA_UPDATED)
		return FALSE;

	return TRUE;
}

HBOOL VA_UTIL_GetBbxInitDisplay()
{
	if (s_stVaData.ucBbxInitDisplayed != BBX_DATA_DISPLAYED)
		return FALSE;

	return TRUE;
}
#endif

// 아래 함수가 호출되면 BBX data도 날라가 버리므로 주의 요망.
void VA_EraseAllExtArea()
{
	int nRet;
	HUINT8 *pucAcsBuf;
	HUINT8 *pucExtBuf;

	pucAcsBuf = (HUINT8 *)VA_MEM_Alloc(VA_NVM_ACS_DATA_SIZE);
	if (pucAcsBuf == NULL)
		return;

	pucExtBuf = (HUINT8 *)VA_MEM_Alloc(VA_NVM_EXT_DATA_SIZE);
	if (pucExtBuf == NULL)
	{
		VA_MEM_Free(pucAcsBuf);
		return;
	}

	VK_memset32(pucAcsBuf, 0, VA_NVM_ACS_DATA_SIZE);
	VK_memset32(pucExtBuf, 0, VA_NVM_EXT_DATA_SIZE);

	// PAL_NVRAM_Write 함수로 변경하세요..
	nRet = MW_EEP_ExtZoneWriteData(VA_NVM_ACS_DATA_OFFSET, VA_NVM_ACS_DATA_SIZE, pucAcsBuf);
	VaPrint(DBG_PRINT, ("ACS EEP Init %s\n", nRet == ERR_OK? "Success":"Fail"));

	// PAL_NVRAM_Write 함수로 변경하세요..
	nRet = MW_EEP_ExtZoneWriteData(VA_NVM_EXT_DATA_OFFSET, VA_NVM_EXT_DATA_SIZE, pucExtBuf);
	VaPrint(DBG_PRINT, ("EXT EEP Init %s\n", nRet == ERR_OK? "Success":"Fail"));

	VA_MEM_Free(pucAcsBuf);
	VA_MEM_Free(pucExtBuf);
}

#else //CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG

#define CRC_VALID	0x5ac3a53c

// Moved from pal_eeprom.h
#define E2P_SIZE								0x2000				/*  8k byte  	 	*/

// eeprom이 남으면 flash대신 eeprom을 쓰세요..
#if (E2P_SIZE >= 0x8000) // if eeprom size is over 32K then...
#define VA_EMULATE_FLASH_BY_EEPROM
#endif

#ifdef VA_EMULATE_FLASH_BY_EEPROM
#define VA_FLASH_SIZE				VA_NVM_EXT_DATA_SIZE
#else
#define VA_FLASH_SIZE				0x20000
#endif
#define VA_FLASH_OFFSET			0

// VA FLASH MAP
// User Data       : 2K
// OP MSG info   : 256 byte
// OP MSG data  : 2K
// OP DATA info  : 256 byte
// OP DATA data : 2K
// total               : 6.5K

// User Data : 2K (나중에 추가되는 기능이 있다면 쓰시라..)
// user data CRC
#define VA_FLASH_USER_DATA_CRC_OFFSET		VA_FLASH_OFFSET
#define VA_FLASH_USER_DATA_CRC_SIZE			4
	// user data area
	#define VA_FLASH_USER_DATA_OFFSET			(VA_FLASH_USER_DATA_CRC_OFFSET + VA_FLASH_USER_DATA_CRC_SIZE)
	#define VA_FLASH_USER_DATA_SIZE				(2048 - VA_FLASH_USER_DATA_CRC_SIZE)
	// pin code
	#define VA_FLASH_PIN_CODE_OFFSET				VA_FLASH_USER_DATA_OFFSET
	#define VA_FLASH_PIN_CODE_SIZE				5	// 0: validity (1=true), 1-4: pin code
#ifdef VA_BBX_INIT_CHECK
	// bbx init check
	#define VA_FLASH_BBX_INIT_OFFSET				(VA_FLASH_PIN_CODE_OFFSET+VA_FLASH_PIN_CODE_SIZE)
	#define VA_FLASH_BBX_INIT_SIZE					1	// true/false
	#define VA_FLASH_BBX_INIT_DISPLAY_OFFSET		(VA_FLASH_BBX_INIT_OFFSET+VA_FLASH_BBX_INIT_SIZE)
	#define VA_FLASH_BBX_INIT_DISPLAY_SIZE		1	// true/false
#endif
	// va_schip_api.c 에서 테스트 용도로 VA_SCHIP_TEST_MODE 가 define 되어 있을 때 FusingInfo(CWE) 를 임시 저장하기 위한 용도
	#define VA_FLASH_SCHIP_TEST_OFFSET					(VA_FLASH_BBX_INIT_DISPLAY_OFFSET + VA_FLASH_BBX_INIT_DISPLAY_SIZE)
	#define VA_FLASH_SCHIP_TEST_SIZE					1

// OP Message와 OP Data를 각각 4개씩 저장한다... (실제로는 1개씩만 써도 됨..)
// information about stored message for future use
#define VA_FLASH_OP_MESSAGE_INFO_OFFSET		(VA_FLASH_USER_DATA_OFFSET + VA_FLASH_USER_DATA_SIZE)
#define VA_FLASH_OP_MESSAGE_INFO_SIZE		256
	#define VA_FLASH_OP_MESSAGE_CRC_OFFSET		(VA_FLASH_OP_MESSAGE_INFO_OFFSET)
	#define VA_FLASH_OP_MESSAGE_CRC_SIZE			4
	#define VA_FLASH_OP_MESSAGE_NUM_OFFSET		(VA_FLASH_OP_MESSAGE_CRC_OFFSET + VA_FLASH_OP_MESSAGE_CRC_SIZE)
	#define VA_FLASH_OP_MESSAGE_NUM_SIZE		4

// message slot
#define VA_FLASH_OP_MESSAGE_OFFSET			(VA_FLASH_OP_MESSAGE_INFO_OFFSET + VA_FLASH_OP_MESSAGE_INFO_SIZE)
#define VA_FLASH_OP_MESSAGE_SLOT_SIZE		512	// 1 item: 256 + 4 + 4 + 4 + extra,,,
#define VA_FLASH_OP_MESSAGE_SLOT_NUM		4	// actually only one messages are used. others are reserved for future use
#define VA_FLASH_OP_MESSAGE_SIZE				(VA_FLASH_OP_MESSAGE_SLOT_SIZE*VA_FLASH_OP_MESSAGE_SLOT_NUM)

// information about stored data for future use
#define VA_FLASH_OP_DATA_INFO_OFFSET			(VA_FLASH_OP_MESSAGE_OFFSET + VA_FLASH_OP_MESSAGE_SIZE)
#define VA_FLASH_OP_DATA_INFO_SIZE			256
	#define VA_FLASH_OP_DATA_CRC_OFFSET		(VA_FLASH_OP_DATA_INFO_OFFSET)
	#define VA_FLASH_OP_DATA_CRC_SIZE		4
	#define VA_FLASH_OP_DATA_NUM_OFFSET		(VA_FLASH_OP_DATA_CRC_OFFSET + VA_FLASH_OP_DATA_CRC_SIZE)
	#define VA_FLASH_OP_DATA_NUM_SIZE		4

// data slot
#define VA_FLASH_OP_DATA_OFFSET				(VA_FLASH_OP_DATA_INFO_OFFSET + VA_FLASH_OP_DATA_INFO_SIZE)
#define VA_FLASH_OP_DATA_SLOT_SIZE			512		// 1 item: 256 + 4 + 4 + extra,,,
#define VA_FLASH_OP_DATA_SLOT_NUM			4	// actually only one messages are used. others are reserved for future use
#define VA_FLASH_OP_DATA_SIZE					(VA_FLASH_OP_DATA_SLOT_SIZE*VA_FLASH_OP_DATA_SLOT_NUM)

/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/

static HUINT8 *s_pucFlashBuffer;

static HUINT32 s_ulAudioMuteFlag;
static HUINT32 s_ulVideoMuteFlag;

static HUINT8 s_aucPinCodeData[VA_FLASH_PIN_CODE_SIZE];

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/

void VA_UTIL_Init()
{
	int bFlagModified;

	s_ulAudioMuteFlag = VA_MUTE_NONE;
	s_ulVideoMuteFlag = VA_MUTE_NONE;

	VA_memset(s_aucPinCodeData, 0, VA_FLASH_PIN_CODE_SIZE);

	VA_FlashInit();

	bFlagModified = FALSE;

	// check CRC of va data/message
	if (VA_CheckOpMessageCrc(FALSE) == FALSE)
	{
		HxLOG_Error("EraseAllOpMessage by wrong CRC\n");
		VA_EraseAllOpMessage();
		bFlagModified = TRUE;
	}

	if (VA_CheckOpDataCrc(FALSE) == FALSE)
	{
		HxLOG_Error("EraseAllOpData by wrong CRC\n");
		VA_EraseAllOpData();
		bFlagModified = TRUE;
	}

// user data는 나중에 막 바뀔 수도 있으니 지우지는 맙시다..
#if 0
	if (VA_CheckUserDataCrc(FALSE) == FALSE)
	{
		HxLOG_Error("EraseAllUserData by wrong CRC\n");
		VA_EraseAllUserData();
		bFlagModified = TRUE;
	}
#endif

	if (bFlagModified)
		VA_FlashFlush();
}

///////////////////////////////////
// read/write data from/to flash
///////////////////////////////////

// create a buffer and copy data from flash to the buffer
// data will be written and read to/from the buffer.
// to store data in the flash, VA_FlashFlush() must be invoked
int VA_FlashInit()
{
	HxLOG_Print("[Enter]\n");

	s_pucFlashBuffer = (HUINT8 *)VA_MEM_Alloc(VA_FLASH_SIZE);
	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

#ifdef VA_EMULATE_FLASH_BY_EEPROM
	if (svc_cas_DevPortNVRAMReadData(eDxCAS_GROUPID_VA, VA_NVM_EXT_DATA_OFFSET, VA_NVM_EXT_DATA_SIZE, s_pucFlashBuffer) != ERR_OK)
		return VA_ERR;
#endif

	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

// copy data from the buffer to the flash
int VA_FlashFlush()
{
	HxLOG_Print("[Enter]\n");

	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

#ifdef VA_EMULATE_FLASH_BY_EEPROM
	if (svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM_EXT_DATA_OFFSET, VA_NVM_EXT_DATA_SIZE, s_pucFlashBuffer) != ERR_OK)
		return VA_ERR;
#endif

	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

HUINT32	VA_INT_ReadVAData(HUINT32 ulOffset, HUINT8 *pucData, HUINT32 ulSize)
{
HxLOG_Print("[Enter]\n");

	if ( (pucData == NULL) || (ulOffset + ulSize > VA_FLASH_SIZE) )
		return VA_ERR;

	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

	VA_memcpy(pucData, s_pucFlashBuffer + ulOffset, ulSize);

HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

HUINT32	VA_INT_WriteVAData(HUINT32 ulOffset, HUINT8 *pucData, HUINT32 ulSize)
{
HxLOG_Print("[Enter]\n");

	if ( (pucData == NULL) || (ulOffset + ulSize > VA_FLASH_SIZE) )
		return VA_ERR;

	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

	VA_memcpy(s_pucFlashBuffer + ulOffset, pucData, ulSize);

#ifdef VA_EMULATE_FLASH_BY_EEPROM
	if (svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, ulOffset + VA_NVM_EXT_DATA_OFFSET, ulSize, pucData) != ERR_OK)
		return VA_ERR;
#endif

HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_EraseAllOpMessage()
{
	HxLOG_Print("[Enter]\n");

	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

	VA_memset(s_pucFlashBuffer + VA_FLASH_OP_MESSAGE_INFO_OFFSET, 0, VA_FLASH_OP_MESSAGE_INFO_SIZE + VA_FLASH_OP_MESSAGE_SIZE);
	VA_INT_WriteVAData(VA_FLASH_OP_MESSAGE_INFO_OFFSET, s_aucPinCodeData, VA_FLASH_PIN_CODE_SIZE);
	VA_CheckOpMessageCrc(TRUE);

	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_CheckOpMessageCrc(int bUpdateCrc)
{
	HUINT32 ulCrcStored, ulCrcCalculated, ulSize;
	HUINT8	aucCrc[4];

	HxLOG_Print("[Enter][%d]\n", bUpdateCrc);

	ulSize = VA_FLASH_OP_MESSAGE_INFO_SIZE + VA_FLASH_OP_MESSAGE_SIZE;

	ulCrcStored = get32bit(s_pucFlashBuffer + VA_FLASH_OP_MESSAGE_CRC_OFFSET);
	ulCrcCalculated = HLIB_MATH_GetCrc32(s_pucFlashBuffer + VA_FLASH_OP_MESSAGE_CRC_OFFSET + VA_FLASH_OP_MESSAGE_CRC_SIZE, ulSize - VA_FLASH_OP_MESSAGE_CRC_SIZE);
	ulCrcCalculated += CRC_VALID;

	if (bUpdateCrc)
	{
		if (ulCrcStored != ulCrcCalculated)
		{
			put32bit(aucCrc, ulCrcCalculated);
			VA_INT_WriteVAData(VA_FLASH_OP_MESSAGE_CRC_OFFSET, aucCrc, VA_FLASH_OP_MESSAGE_CRC_SIZE);
		}
	}

	HxLOG_Print("[Exit][%x,%x]\n", ulCrcStored, ulCrcCalculated);
	return (ulCrcStored == ulCrcCalculated);
}

int VA_SetOpMessage(VA_UI_OP_MESSAGE *pOpMsg, unsigned char ucSlotNum)
{
	int nRet;
	int nLength;

	HxLOG_Print("[Enter][%d, %d, %x, %s]\n", pOpMsg->ulCount, pOpMsg->ulEncode, pOpMsg->ulSoid, pOpMsg->szMsg);

	if (ucSlotNum >= VA_FLASH_OP_MESSAGE_SLOT_NUM)
		return VA_ERR;

	//nLength = strlen(pOpMsg->szMsg) + 12 + 1; //unicode로 된 message의 경우 strlen값이 잘못 계산됨.
	nLength = pOpMsg->ulCount + 12 + 1;
	if (nLength > sizeof(VA_UI_OP_MESSAGE))
		nLength = sizeof(VA_UI_OP_MESSAGE);

	nRet = VA_INT_WriteVAData(VA_FLASH_OP_MESSAGE_OFFSET + VA_FLASH_OP_MESSAGE_SLOT_SIZE * ucSlotNum, (HUINT8 *)pOpMsg, nLength);
	VA_CheckOpMessageCrc(TRUE);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_GetOpMessage(VA_UI_OP_MESSAGE *pOpMsg, unsigned char ucSlotNum)
{
	int nRet;

	HxLOG_Print("[Enter][%d]\n", ucSlotNum);

	if (ucSlotNum >= VA_FLASH_OP_MESSAGE_SLOT_NUM)
		return VA_ERR;

	nRet = VA_INT_ReadVAData(VA_FLASH_OP_MESSAGE_OFFSET + VA_FLASH_OP_MESSAGE_SLOT_SIZE * ucSlotNum, (HUINT8 *)pOpMsg, sizeof(VA_UI_OP_MESSAGE));

	HxLOG_Print("[Exit][%d, %d, %x, %s]\n", pOpMsg->ulCount, pOpMsg->ulEncode, pOpMsg->ulSoid, pOpMsg->szMsg);
	return nRet;
}

int VA_EraseOpMessage(unsigned char ucSlotNum)
{
	VA_UI_OP_MESSAGE msg;
	int nRet;

	HxLOG_Print("[Enter]\n");

	if (ucSlotNum >= VA_FLASH_OP_MESSAGE_SLOT_NUM)
		return VA_ERR;

	VA_memset(&msg, 0, sizeof(VA_UI_OP_MESSAGE));
	nRet = VA_INT_WriteVAData(VA_FLASH_OP_MESSAGE_OFFSET + VA_FLASH_OP_MESSAGE_SLOT_SIZE * ucSlotNum, (HUINT8 *)&msg, sizeof(VA_UI_OP_MESSAGE));
	VA_CheckOpMessageCrc(TRUE);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_EraseAllOpData()
{
	HxLOG_Print("[Enter]\n");

	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

	VA_memset(s_pucFlashBuffer + VA_FLASH_OP_DATA_INFO_OFFSET, 0, VA_FLASH_OP_DATA_INFO_SIZE + VA_FLASH_OP_DATA_SIZE);
	VA_INT_WriteVAData(VA_FLASH_OP_DATA_INFO_OFFSET, s_aucPinCodeData, VA_FLASH_PIN_CODE_SIZE);
	VA_CheckOpDataCrc(TRUE);

	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

int VA_CheckOpDataCrc(int bUpdateCrc)
{
	HUINT32 ulCrcStored, ulCrcCalculated, ulSize;
	HUINT8	aucCrc[4];

	HxLOG_Print("[Enter][%d]\n", bUpdateCrc);

	ulSize = VA_FLASH_OP_DATA_INFO_SIZE + VA_FLASH_OP_DATA_SIZE;

	ulCrcStored = get32bit(s_pucFlashBuffer + VA_FLASH_OP_DATA_CRC_OFFSET);
	ulCrcCalculated = HLIB_MATH_GetCrc32 (s_pucFlashBuffer + VA_FLASH_OP_DATA_CRC_OFFSET + VA_FLASH_OP_DATA_CRC_SIZE, ulSize - VA_FLASH_OP_DATA_CRC_SIZE);
	ulCrcCalculated += CRC_VALID;

	if (bUpdateCrc)
	{
		if (ulCrcStored != ulCrcCalculated)
		{
			put32bit(aucCrc, ulCrcCalculated);
			VA_INT_WriteVAData(VA_FLASH_OP_DATA_CRC_OFFSET, aucCrc, VA_FLASH_OP_DATA_CRC_SIZE);
		}
	}

	HxLOG_Print("[Exit][%x,%x]\n", ulCrcStored, ulCrcCalculated);
	return (ulCrcStored == ulCrcCalculated);
}

int VA_SetOpData(VA_UI_OP_DATA *pOpMsg, unsigned char ucSlotNum)
{
	int nRet;

	HxLOG_Print("[Enter]\n");
	if (ucSlotNum >= VA_FLASH_OP_DATA_SLOT_NUM)
		return VA_ERR;

	nRet = VA_INT_WriteVAData(VA_FLASH_OP_DATA_OFFSET + VA_FLASH_OP_DATA_SLOT_SIZE * ucSlotNum, (HUINT8 *)pOpMsg, sizeof(VA_UI_OP_DATA));
	VA_CheckOpDataCrc(TRUE);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_GetOpData(VA_UI_OP_DATA *pOpMsg, unsigned char ucSlotNum)
{
	int nRet;

	HxLOG_Print("[Enter]\n");
	if (ucSlotNum >= VA_FLASH_OP_DATA_SLOT_NUM)
		return VA_ERR;

	nRet = VA_INT_ReadVAData(VA_FLASH_OP_DATA_OFFSET + VA_FLASH_OP_DATA_SLOT_SIZE * ucSlotNum, (HUINT8 *)pOpMsg, sizeof(VA_UI_OP_DATA));

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_EraseOpData(unsigned char ucSlotNum)
{
	VA_UI_OP_MESSAGE msg;
	int nRet;

	HxLOG_Print("[Enter]\n");
	if (ucSlotNum >= VA_FLASH_OP_DATA_SLOT_NUM)
		return VA_ERR;

	VA_memset(&msg, 0, sizeof(VA_UI_OP_DATA));
	nRet = VA_INT_WriteVAData(VA_FLASH_OP_DATA_OFFSET + VA_FLASH_OP_DATA_SLOT_SIZE * ucSlotNum, (HUINT8 *)&msg, sizeof(VA_UI_OP_DATA));
	VA_CheckOpDataCrc(TRUE);

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_CheckUserDataCrc(int bUpdateCrc)
{
	HUINT32 ulCrcStored, ulCrcCalculated;
	HUINT8	aucCrc[4];

	HxLOG_Print("[Enter][%d]\n", bUpdateCrc);

	ulCrcStored = get32bit(s_pucFlashBuffer + VA_FLASH_USER_DATA_CRC_OFFSET);
	ulCrcCalculated = HLIB_MATH_GetCrc32(s_pucFlashBuffer + VA_FLASH_USER_DATA_OFFSET, VA_FLASH_USER_DATA_SIZE);
	ulCrcCalculated += CRC_VALID;

	if (bUpdateCrc)
	{
		if (ulCrcStored != ulCrcCalculated)
		{
			put32bit(aucCrc, ulCrcCalculated);
			VA_INT_WriteVAData(VA_FLASH_USER_DATA_CRC_OFFSET, aucCrc, VA_FLASH_USER_DATA_CRC_SIZE);
		}
	}

	HxLOG_Print("[Exit][%x,%x]\n", ulCrcStored, ulCrcCalculated);
	return (ulCrcStored == ulCrcCalculated);
}

int VA_EraseAllUserData()
{
	HxLOG_Print("[Enter]\n");

	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

	VA_memset(s_pucFlashBuffer + VA_FLASH_USER_DATA_OFFSET, 0, VA_FLASH_USER_DATA_SIZE);
	VA_INT_WriteVAData(VA_FLASH_USER_DATA_OFFSET, s_aucPinCodeData, VA_FLASH_PIN_CODE_SIZE);
	VA_CheckUserDataCrc(TRUE);

	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
int VA_UTIL_InvalidatePincode()
{
	int nRet;

	VA_memset(s_aucPinCodeData, 0, VA_FLASH_PIN_CODE_SIZE);

	nRet = VA_INT_WriteVAData(VA_FLASH_PIN_CODE_OFFSET, s_aucPinCodeData, VA_FLASH_PIN_CODE_SIZE);
	VA_CheckUserDataCrc(TRUE);

	return nRet;
}

int VA_UTIL_SetPinCode(HUINT8 *pucPin)
{
	int nRet;
	HUINT8 aucPinCodeData[VA_FLASH_PIN_CODE_SIZE];

	HxLOG_Print("[Enter]\n");

	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

	if (s_aucPinCodeData[0] == FALSE || VA_memcmp(s_aucPinCodeData + 1, pucPin, 4) != 0)
	{
		s_aucPinCodeData[0] = TRUE;
		VA_memcpy(s_aucPinCodeData + 1, pucPin, 4);
		VA_memcpy(aucPinCodeData, s_aucPinCodeData, VA_FLASH_PIN_CODE_SIZE);

		// pin code를 XOR하여 최소한의 보호를 한다.
		aucPinCodeData[1] ^= 0x3F;
		aucPinCodeData[2] ^= 0xF5;
		aucPinCodeData[3] ^= 0x77;
		aucPinCodeData[4] ^= 0xE3;

		nRet = VA_INT_WriteVAData(VA_FLASH_PIN_CODE_OFFSET, aucPinCodeData, VA_FLASH_PIN_CODE_SIZE);
		VA_CheckUserDataCrc(TRUE);
	}

	HxLOG_Print("[Exit]\n");
	return nRet;
}

int VA_UTIL_GetPinCode(HUINT8 *pucPin)
{
	int nRet;
	HUINT8 aucPinCodeData[VA_FLASH_PIN_CODE_SIZE];

	HxLOG_Print("[Enter]\n");

	if (s_pucFlashBuffer == NULL)
		return VA_ERR;

	if (s_aucPinCodeData[0] == FALSE)
	{
		nRet = VA_INT_ReadVAData(VA_FLASH_PIN_CODE_OFFSET, aucPinCodeData, VA_FLASH_PIN_CODE_SIZE);
		if (nRet != ERR_OK || aucPinCodeData[0] == FALSE)
			return VA_ERR;

		// pin code를 XOR하여 최소한의 보호를 한다.
		aucPinCodeData[1] ^= 0x3F;
		aucPinCodeData[2] ^= 0xF5;
		aucPinCodeData[3] ^= 0x77;
		aucPinCodeData[4] ^= 0xE3;

		VA_memcpy(s_aucPinCodeData, aucPinCodeData, 5);
	}

	VA_memcpy(pucPin, s_aucPinCodeData + 1, 4);

	HxLOG_Print("[Exit]\n");
	return VA_ERR_OK;
}
#endif

#ifdef VA_BBX_INIT_CHECK

static HBOOL s_bBbxUpdated = FALSE;

#define BBX_DATA_UPDATED		0x3A
#define BBX_DATA_DISPLAYED		0xC5

void VA_UTIL_SetBbxUpdated()
{
	s_bBbxUpdated = TRUE;
}

void VA_UTIL_SetBbxInit()
{
	HUINT8 ucInit;

	if (s_bBbxUpdated)
	{
		ucInit = BBX_DATA_UPDATED;
		VA_INT_WriteVAData(VA_FLASH_BBX_INIT_OFFSET, &ucInit, VA_FLASH_BBX_INIT_SIZE);

		ucInit = 0;
		VA_INT_WriteVAData(VA_FLASH_BBX_INIT_DISPLAY_OFFSET, &ucInit, VA_FLASH_BBX_INIT_DISPLAY_SIZE);

		VA_CheckUserDataCrc(TRUE);
		s_bBbxUpdated = FALSE;
	}
}

void VA_UTIL_SetBbxInitDisplay()
{
	HUINT8 ucInit;

	ucInit = BBX_DATA_DISPLAYED;
	VA_INT_WriteVAData(VA_FLASH_BBX_INIT_DISPLAY_OFFSET, &ucInit, VA_FLASH_BBX_INIT_DISPLAY_SIZE);
	VA_CheckUserDataCrc(TRUE);
}

HBOOL VA_UTIL_GetBbxInit()
{
	int nRet;
	HUINT8 ucInit;

	nRet = VA_INT_ReadVAData(VA_FLASH_BBX_INIT_OFFSET, &ucInit, VA_FLASH_BBX_INIT_SIZE);

	if (nRet != ERR_OK || ucInit != BBX_DATA_UPDATED)
		return FALSE;

	return TRUE;
}

HBOOL VA_UTIL_GetBbxInitDisplay()
{
	int nRet;
	HUINT8 ucInit;

	nRet = VA_INT_ReadVAData(VA_FLASH_BBX_INIT_DISPLAY_OFFSET, &ucInit, VA_FLASH_BBX_INIT_DISPLAY_SIZE);

	if (nRet != ERR_OK || ucInit != BBX_DATA_DISPLAYED)
		return FALSE;

	return TRUE;
}
#endif

HUINT8 VA_GetCWE(void) // va_schip_api.c 에서 테스트 용도로 VA_SCHIP_TEST_MODE 가 define 되어 있을 때만 사용!
{
	HUINT8 ucCWE = 0;

	if (VA_INT_ReadVAData(VA_FLASH_SCHIP_TEST_OFFSET, &ucCWE, VA_FLASH_SCHIP_TEST_SIZE))
		VaError(DBG_ASSERT, ("VA_INT_ReadVAData Fail\n"));

	return ucCWE;
}

void VA_SetCWE(void) // va_schip_api.c 에서 테스트 용도로 VA_SCHIP_TEST_MODE 가 define 되어 있을 때만 사용!
{
	HUINT8 ucCWE = 1;

	if (VA_INT_WriteVAData(VA_FLASH_SCHIP_TEST_OFFSET, &ucCWE, VA_FLASH_SCHIP_TEST_SIZE))
	{
		VaError(DBG_ASSERT, ("VA_INT_WriteVAData Fail\n"));
		return;
	}

	VA_CheckUserDataCrc(TRUE);
}

void VA_ClearCWE(void) // va_schip_api.c 에서 테스트 용도로 VA_SCHIP_TEST_MODE 가 define 되어 있을 때만 사용!
{
	HUINT8 ucCWE = 0;

	if (VA_INT_WriteVAData(VA_FLASH_SCHIP_TEST_OFFSET, &ucCWE, VA_FLASH_SCHIP_TEST_SIZE))
	{
		VaError(DBG_ASSERT, ("VA_INT_WriteVAData Fail\n"));
		return;
	}

	VA_CheckUserDataCrc(TRUE);
}

#endif //CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG

/*
	get STB serial number (probably from EEPROM)

	- VA STB Serial number: 12 bytes BCD format
*/
int VA_GetSTBSerialNumber (unsigned char *pucSerial)
{
	HUINT8 aucStbSerial[HUMAX_SERIAL_NUMBER_SIZE];
	int i, nIdx;

	VaPrint(DBG_PRINT, ("[Enter]\n"));

#if 0 //defined(CONFIG_DEBUG)
	{
		// 생산 SN (14자리) : 19 1920034 00006 -> VA SN (12자리) : 0x00 0x00 0x00 0x00 0x00 0x19 0x19 0x20 0x03 0x40 0x00 0x06
//		HUINT8 aucTestSerial[VA_STB_SERIAL_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x19, 0x20, 0x03, 0x40, 0x00, 0x06};
//		VA_memcpy(pucSerial, aucTestSerial, VA_STB_SERIAL_SIZE);

		// 생산 SN (14자리) : 191 9200610 00211 -> VA SN (12자리) : 0x00 0x00 0x00 0x00 0x01 0x91 0x92 0x00 0x61 0x00 0x02 0x11
		HUINT8 aucTestSerial[VA_STB_SERIAL_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x91, 0x92, 0x00, 0x61, 0x00, 0x02, 0x11};
		VA_memcpy(pucSerial, aucTestSerial, VA_STB_SERIAL_SIZE);

		// chip id : 0000627485
//		HUINT8 aucTestSerial[VA_STB_SERIAL_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x01, 0x91, 0x92, 0x00, 0x61, 0x00, 0x02, 0x12};
//		VA_memcpy(pucSerial, aucTestSerial, VA_STB_SERIAL_SIZE);
/*
		// chip id : 0000624558
		HUINT8 aucTestSerial[VA_STB_SERIAL_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x19, 0x20, 0x03, 0x40, 0x00, 0x06};
		VA_memcpy(pucSerial, aucTestSerial, VA_STB_SERIAL_SIZE);

		// chip id : 000062455F
		HUINT8 aucTestSerial[VA_STB_SERIAL_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x19, 0x20, 0x06, 0x10, 0x01, 0x47};
		VA_memcpy(pucSerial, aucTestSerial, VA_STB_SERIAL_SIZE);
*/
	}
#else
	VA_memset(aucStbSerial, 0x00, HUMAX_SERIAL_NUMBER_SIZE);
	if (PalParam_Activated())
	{
		if (PAL_PARAM_GetField(ePAL_PARAM_SERIAL_NO, (void *)aucStbSerial, HUMAX_SERIAL_NUMBER_SIZE) != ERR_OK)
			return VA_ERR;
	}
	else
	{
		if (PAL_SYS_GetField(PAL_FLASH_FIELD_SERIAL_NO, (void*)aucStbSerial, HUMAX_SERIAL_NUMBER_SIZE) != ERR_OK)
			return VA_ERR;
	}

	VA_memset(pucSerial, 0, VA_STB_SERIAL_SIZE);

	nIdx = VA_STB_SERIAL_SIZE - 1;

	for (i = HUMAX_SERIAL_NUMBER_SIZE - 1; i >= 0; i--)
	{
		// % 10까지는 할 필요없는데 serial number가 없는 경우 이상한 값이 올 수 있어서 10보다 작은 수로 만들어준다.
		pucSerial[nIdx] = ((aucStbSerial[i--] & 0xF) % 10);
		pucSerial[nIdx] |= (((aucStbSerial[i] & 0x0F) % 10) << 4);
		nIdx--;
	}
#endif

	VaPrint(DBG_PRINT, ("[Exit]\n"));
	return VA_ERR_OK;
}

/*
	return manufacturer ID given by Viaccess (va_main.h)
*/
int VA_GetManufacturerId()
{
	return VA_MANUFACTURER_IDENTIFIER;
}

/*
	return model ID given by Viaccess (va_main.h)
*/
int VA_GetModelId()
{
	return VA_MODEL_IDENTIFIER;
}

const char *VA_GetHardwareVer()
{
	return VA_MODEL_HARDWARE_VERSION;
}


#ifdef CONFIG_DEBUG

// 아래 함수가 호출되면 BBX data도 날라가 버리므로 주의 요망.
int VA_EraseAllAcsData()
{
	int nRet = ERR_FAIL;
	HUINT8 * pucAcsBuf = NULL;

	pucAcsBuf = (HUINT8 *)VA_MEM_Alloc(VA_NVM_ACS_DATA_SIZE);
	if (pucAcsBuf == NULL)
	{
		HxLOG_Error("VA_MEM_Alloc Fail\n");
		return nRet;
	}

	VK_memset32(pucAcsBuf, 0, VA_NVM_ACS_DATA_SIZE);

	nRet = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM_ACS_DATA_OFFSET, VA_NVM_ACS_DATA_SIZE, pucAcsBuf);
	if (nRet == ERR_OK)
	{
		HxLOG_Print("ACS EEP Init Success\n");
	}
	else
	{
		HxLOG_Error("ACS EEP Init Fail\n");
	}

	VA_MEM_Free(pucAcsBuf);

	return nRet;
}

int VA_EraseAllExtData()
{
	int nRet = ERR_FAIL;
	HUINT8 * pucExtBuf = NULL;

	pucExtBuf = (HUINT8 *)VA_MEM_Alloc(VA_NVM_EXT_DATA_SIZE);
	if (pucExtBuf == NULL)
	{
		HxLOG_Error("VA_MEM_Alloc Fail\n");
		return nRet;
	}

	VK_memset32(pucExtBuf, 0, VA_NVM_EXT_DATA_SIZE);

	nRet = svc_cas_DevPortNVRAMWriteData(eDxCAS_GROUPID_VA, VA_NVM_EXT_DATA_OFFSET, VA_NVM_EXT_DATA_SIZE, pucExtBuf);
	if (nRet == ERR_OK)
	{
		HxLOG_Print("EXT EEP Init Success\n");
	}
	else
	{
		HxLOG_Error("EXT EEP Init Fail\n");
	}

	VA_MEM_Free(pucExtBuf);

	return nRet;
}

#endif

// 좀 복잡하게 되버렸는데,,,
// camlock은 av를 다 막는거고
// pb는 a,v 권한이 다를 수가 있어서.. 아흑..
void VA_OutputMute(HBOOL bMute, HUINT32 ulType)
{
	HUINT32 ulContextId = 0;

	if (bMute)
	{
		// mute 시킨다
		if (ulType & VA_MUTE_CAMLOCK)
		{
			s_ulVideoMuteFlag |= VA_MUTE_CAMLOCK;
			s_ulAudioMuteFlag |= VA_MUTE_CAMLOCK;
		}

		if (ulType & VA_MUTE_PB_VIDEO)
		{
			s_ulVideoMuteFlag |= VA_MUTE_PB_VIDEO;
		}

		if (ulType & VA_MUTE_PB_AUDIO)
		{
			s_ulAudioMuteFlag |= VA_MUTE_PB_AUDIO;
		}
	}
	else
	{
		// unmute 시킨다
		if (ulType & VA_MUTE_CAMLOCK)
		{
			s_ulVideoMuteFlag &= ~VA_MUTE_CAMLOCK;
			s_ulAudioMuteFlag &= ~VA_MUTE_CAMLOCK;
		}

		if (ulType & VA_MUTE_PB_VIDEO)
		{
			s_ulVideoMuteFlag &= ~VA_MUTE_PB_VIDEO;
		}

		if (ulType & VA_MUTE_PB_AUDIO)
		{
			s_ulAudioMuteFlag &= ~VA_MUTE_PB_AUDIO;
		}
	}

	HxLOG_Info("bMute = %x, ulType = %x -> V = %x, A = %x\n", bMute, ulType, s_ulVideoMuteFlag, s_ulAudioMuteFlag);

#if 0 // TODO: 이거 TV manger 로 올려서 처리하도록 해주세요....
	if (s_ulVideoMuteFlag)
	{
		PAL_VIDEO_SetHide(0, eAvHideClient_Cas, TRUE);
	}
	else
	{
		PAL_VIDEO_SetHide(0, eAvHideClient_Cas, FALSE);
	}

	if (s_ulAudioMuteFlag)
	{
		PAL_AUDIO_SetHide(eAvHideClient_Cas, TRUE);
	}
	else
	{
		PAL_AUDIO_SetHide(eAvHideClient_Cas, FALSE);
	}
#endif
}

