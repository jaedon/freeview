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
#ifndef __HUMAX_PINMUX_H__
#define __HUMAX_PINMUX_H__

#if 0
#define HUMAX_NUM_GPIO_PINS NEXUS_NUM_GPIO_PINS
#else
#define HUMAX_NUM_GPIO_PINS 179
#endif

typedef enum
{
	HUMAX_GPIO_FUNCTION_NCTP = -1,
	HUMAX_GPIO_FUNCTION_GPIO = 0,
	HUMAX_GPIO_FUNCTION_ALT1,
	HUMAX_GPIO_FUNCTION_ALT2,
	HUMAX_GPIO_FUNCTION_ALT3,
	HUMAX_GPIO_FUNCTION_ALT4,
	HUMAX_GPIO_FUNCTION_ALT5,
	HUMAX_GPIO_FUNCTION_ALT6,
	HUMAX_GPIO_FUNCTION_ALT7,
	HUMAX_GPIO_FUNCTION_ALT8,
	HUMAX_GPIO_FUNCTION_ALT9,
	HUMAX_GPIO_FUNCTION_MAX
} HUMAX_GPIO_FUNCTION ;


typedef struct
{
	unsigned int addr;
	unsigned mask;
	unsigned shift;
} HUMAX_GpioTable;

#endif

