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
	Miscellaneous fuctions for Viaccess
*/

#ifndef VA_UTIL_H
#define VA_UTIL_H

#include "va_def.h"
#include "va_ui_adapt.h"

#define VA_STB_SERIAL_SIZE				12
#define HUMAX_SERIAL_NUMBER_SIZE		14

#define VA_MUTE_NONE			0
#define VA_MUTE_CAMLOCK		1
#define VA_MUTE_PB_VIDEO		2
#define VA_MUTE_PB_AUDIO		4

void VA_UTIL_Init();

int VA_GetSTBSerialNumber (unsigned char *pucSerial);
int VA_GetManufacturerId();
int VA_GetModelId();

#ifdef VA_BBX_INIT_CHECK
void VA_UTIL_SetBbxUpdated();
void VA_UTIL_SetBbxInit();
void VA_UTIL_SetBbxInitDisplay();
HBOOL VA_UTIL_GetBbxInit();
HBOOL VA_UTIL_GetBbxInitDisplay();
#endif

// 아래 함수가 호출되면 BBX data도 날라가 버리므로 주의 요망.
void VA_EraseAllExtArea();

#ifndef CONFIG_MW_CAS_VIACCESS_MULTI_OPMSG
int VA_FlashInit();
int VA_FlashFlush();

int VA_EraseAllOpMessage();
int VA_CheckOpMessageCrc(int bUpdateCrc);
int VA_SetOpMessage(VA_UI_OP_MESSAGE *pOpMsg, unsigned char  ucSlotNum);
int VA_GetOpMessage(VA_UI_OP_MESSAGE *pOpMsg, unsigned char ucSlotNum);
int VA_EraseOpMessage(unsigned char ucSlotNum);

int VA_EraseAllOpData();
int VA_CheckOpDataCrc(int bUpdateCrc);
int VA_SetOpData(VA_UI_OP_DATA *pOpMsg, unsigned char ucSlotNum);
int VA_GetOpData(VA_UI_OP_DATA *pOpMsg, unsigned char ucSlotNum);
int VA_EraseOpData(unsigned char ucSlotNum);

int VA_CheckUserDataCrc(int bUpdateCrc);
int VA_EraseAllUserData();
#endif

#ifdef CONFIG_MW_CAS_VIACCESS_PVR
int VA_UTIL_InvalidatePincode();
int VA_UTIL_SetPinCode(HUINT8 *pucPin);
int VA_UTIL_GetPinCode(HUINT8 *pucPin);
#endif

#ifdef CONFIG_DEBUG
int VA_EraseAllAcsData();
int VA_EraseAllExtData();
HUINT8 VA_GetCWE(void); // va_schip_api.c 에서 테스트 용도로 VA_SCHIP_TEST_MODE 가 define 되어 있을 때만 사용!
void VA_SetCWE(void); // va_schip_api.c 에서 테스트 용도로 VA_SCHIP_TEST_MODE 가 define 되어 있을 때만 사용!
void VA_ClearCWE(void); // va_schip_api.c 에서 테스트 용도로 VA_SCHIP_TEST_MODE 가 define 되어 있을 때만 사용!
#endif

void VA_OutputMute(HBOOL bMute, HUINT32 ulType);
const char *VA_GetHardwareVer();

#endif // VA_UTIL_H


