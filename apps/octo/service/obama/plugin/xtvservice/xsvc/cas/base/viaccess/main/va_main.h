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
	Viaccess Initialization API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.23
*/

#ifndef VA_MAIN_API_H
#define VA_MAIN_API_H

#include "va_setup.h"
#include "va_ui_adapt.h"
#define CONFIG_OCTO2_0
#define VA_MANUFACTURER_IDENTIFIER	11 // Humax

#define __VIACCESS50__	1
//#define _VIACCESS50_DISABLECODE_BEFORE_RELEASE_ 1 // build tree 릴리즈시에 지워야 한다.
// VA_MODEL_IDENTIFIER : assigned by Viaccess
// VA_MODEL_HARDWARE_VERSION : HW 버전 변경시 반드시 수정되어야 할 값 !!
// 15 for Va-Fox-T
// 20 for VA-ACE+
// 25 for VA-4SD NTV+
// 26 for VA-4SD UA
#if defined(CONFIG_PROD_VHDR3000S)
#define VA_MODEL_IDENTIFIER 			29
#define VA_MODEL_HARDWARE_VERSION	"1.0"
#elif defined(CONFIG_PROD_VHD3100S)
#define VA_MODEL_IDENTIFIER 			30
#define VA_MODEL_HARDWARE_VERSION	"1.0"
#elif defined(CONFIG_PROD_VA5SD)
#define VA_MODEL_IDENTIFIER 			31
#define VA_MODEL_HARDWARE_VERSION	"1.0"
#elif defined(CONFIG_PROD_BXRHD)
#define VA_MODEL_IDENTIFIER 			32
#define VA_MODEL_HARDWARE_VERSION	"1.0"
#elif defined(CONFIG_PROD_BXRHDPLUS)
#define VA_MODEL_IDENTIFIER 			34
#define VA_MODEL_HARDWARE_VERSION	"1.0"
#elif defined(CONFIG_PROD_VAHD3100S)
#define VA_MODEL_IDENTIFIER 			35
#define VA_MODEL_HARDWARE_VERSION	"1.0"
#elif defined(CONFIG_PROD_ICORDHDPLUS)
#define VA_MODEL_IDENTIFIER 			36
#define VA_MODEL_HARDWARE_VERSION	"1.0"
#elif defined(CONFIG_OCTO2_0)
#define VA_MODEL_IDENTIFIER 			37
#define VA_MODEL_HARDWARE_VERSION	"1.0"
#else
// unknown model !!
// model id는 test process notification 문서 제목으로 알 수 있다.. (문서 안에도 있음)
// (예) 0768-011-029-001-HUMAX_VHDR3000S_DRIVER+CERTIF.pdf ==> 11은 manufacturer id, 29는 model identifier
#error Any Viaccess model must have its model identifier.
#endif

#define VA_MAIN_ACS_NUM	kVA_SETUP_NBMAX_ACS		// 2 for pvr, 1 for zapper
#define VA_MAIN_SC_NUM		kVA_SETUP_NBMAX_SC			// 1

HBOOL VA_MAIN_TaskStarted();

void VA_Start();
void VA_Init();

#endif // VA_MAIN_API_H

