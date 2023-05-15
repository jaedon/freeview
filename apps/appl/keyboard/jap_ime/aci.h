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
	aci.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
**		A C I (액세사리/인터페이스)
*/

typedef int 			(*FUNCPTR)(void*);
typedef unsigned char	*ACCSTR;
typedef unsigned short	*ACCMEAN;

typedef struct {
		ACCSTR	outstr; 				/* 확정 문자열 */
		ACCMEAN kwinstr;				/* 후보 윈도우 표시용 문자열 */
		ACCMEAN mwinstr;				/* 모드 윈도우 표시 문자열 */
} ACC_STRS;

typedef struct {
		BIT16	invoke_stt;
		BIT16	invoke_key;
		FUNCPTR 		acc_func;
		ACC_STRS		acc_mes;
} ACC_DEF;
typedef ACC_DEF *U_ACC_DEF;

/* for 'invoke_stt' */
#define KS_EDIT0		0x0001			/* 가입력 중 (가입력 없음) */
#define KS_EDITING		0x0002			/* 가입력 중 (가입력 있음) */
#define KS_SELECT0		0x0004			/* 후보선택중 (후보군 표시 없음) */
#define KS_SELECT		0x0008			/* 후보선택중 (후보군 표시 있음) */

/* for Return Value of 'acc_func()' */
#define CACI_NORMAL 	0
#define CACI_ANYWAY 	1
#define CACI_URGENT 	2
#define CACI_SUSPEND	3
#define CACI_END		0x00ff

#define DF_OUTSTR		0x8000			/* 'outstr' 이 유효 */
#define DF_KWINSTR		0x4000			/* 'kwinstr' 이 유효 */
#define DF_MWINSTR		0x2000			/* 'mwinstr' 이 유효 */

#define KEY_AGAIN		0x0100			/* 입력키를 버리지 않고 한번 더 사용 한다. */


#define IACI_ERROR		(-1)
#define IACI_OK 		(0)

extern ACC_DEF	aci_def[];
extern int		aci_num;
extern ACC_DEF	*cur_acc;
