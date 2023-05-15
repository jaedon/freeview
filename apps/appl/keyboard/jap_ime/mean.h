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
	mean.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
**
**		MEAN 코드 / 정의 파일 
**
**
**		+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
**		|  Kind field	|  Type field	|		   Code field			|
**		+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
**
**				Kind	: 표시 형식 (문자의 수절 방법을 나타냄) 
**				Type	: 코드 형식 (code의 의미를 결정한다. )
**				Code	: 문자코드
**
*/

typedef unsigned short	MEAN;

#define ToKind(kind)	((kind) << 12)
#define ToType(type)	((type & 0x000f) << 8)
#define ToCode(code)	((code) & 0x00ff)
#define To2type(mean)	((mean) + 0x0100)

#define out_mtype(mean) (((mean) >> 8) & 0x000f)

/*
**		Type field 정의 
*/
#define JIS1HALF		1
#define JIS2HALF		2
#define SJIS1HALF		3
#define SJIS2HALF		4
#define JIS1FULL		5
#define JIS2FULL		6
#define SJIS1FULL		7
#define SJIS2FULL		8
#define ASCII_CODE		0
/*
**		Kind field 의 정의 
*/
#define NOT_MEAN		0				/* 후보 표시 */
#define FKEYCOLOR		1
#define FKEY1MODE		2				/* 모드 용 */
#define FKEY2MODE		3
#define UNCERTAIN		4				/* 가입력 */
#define NEXTONE			5				/* 술차 선두 문절 */
#define CERTAIN			6				/* 변환후 문자열 */
#define PREOUTPUT		7				/* 주목문절/주목 후보 */
#define CURSOR			8

int		is_jis(BIT16 code);
int		is_kanji(u_char c);
int		is_shift(BIT16 code);
int		setmean(MEAN *mbuf, BIT16 code, u_short kind);
void	change_kind(register MEAN *mbuf, int len, u_short kind);
int		shiftlen(BIT16 code);
int		sstrtom(MEAN *mbuf, register u_char *str, u_short kind);
u_char	to_ucode(BIT16 code);
int		is_kata(BIT16 cd, int chou_on);
int		is_jisknj(BIT16 cd);
int		is_jiskig(BIT16 cd);
int		meantosjis(MEAN *sour, char *dest, int len);
BIT16	ask_sjis2jis(BIT16 c);
BIT16	ask_jis2sjis(BIT16 jis);
