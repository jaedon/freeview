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
	dic.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*------------------------------------------------------*/
/*		ASK3 사전 매니저 Ver. 2.00					*/
/*		헤더 파일									*/
/*		by Tomy Kamada, 1991-11-11				*/
/*------------------------------------------------------*/

/*		범용 형선언		*/
typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned long	ulong;

#include		"dictypes.h"

typedef union {
		SETU	setu;
		KATUYO	katuyo;
} SYNTAXINFO;			/* 활용/접속 정보 */

typedef struct {
		UCODE	*ustr;			/* 읽기의 u 코드 열 */
		ULEN	ulen;			/* 그 길이 */
		LEVEL	level;			/* 단어 레벨 */
		SYNTAX	syntax; 			/* 품사 정보 */
		SYNTAXINFO		syntaxinfo; 	/* 활용/접속 정보 */
		uchar	*kbuf;			/* '\0' 이 최후에 붙는 자면 정보 */
} NEWWD;				/* 등록용의 말 */

typedef struct {
		SYNTAX	syntax; 		/* 품사 정보 */
		SYNTAXINFO		syntaxinfo; 	/* 활용 / 접속 정보 */
		LEVEL	level;			/* 단어 레벨 */
		RTIME	rtime;			/* 이 품사의최대 참조 시각 */
} SYNREF;				/* 품사 정보와 참조 시각의 구조체 */

typedef short	SRLEN;	/* 품사 정보와 참조 시각의 구조체 수 */
#define SYNS	32		/* 같은 읽기로 품사의 종류에 상한 */

typedef struct {
		ULEN	yomilen;			/* 그 단어의 읽기 길이  */
		ENTID	entid;			/* 그 레코드의 식별 값 */
		ULEN	used_ulen;		/* 그 단어 군을 결정하는데 사용된 u 코드의 길이 */
		ULEN	page_ulen;		/* 이 길이 이상의 단어는 1페이지 이내에 있다.*/
		SRLEN	synreflen;		/* synref[] 의 요소 수 */
		SYNREF	synref[SYNS];		/* 존재하는 품사 마다의 최대 참조 시각 */
} ENTRYS;					/* dic_snssyn() 가 리턴하는 구조체 */

typedef struct {
		WDID	wdid;			/* 단어 ID */
		RTIME	rtime;			/* 참조 시각 */
		LEVEL	level;			/* 단어 레벨 */
		uchar	kbuf[1];		/* 단어의 자면 */
} WDS;

/*		dic_???? 함수의 리턴 값  		*/
#define DIC_OK			3
#define DIC_HASLONGER	2
#define DIC_HASMATCH	1
#define DIC_HASNONE 	0
#define DIC_BADARG		-1
#define DIC_FATAL		-2
#define DIC_ISCLOSED	-3
#define DIC_ISOPEN		-4
#define DIC_OVERFLOW	-5
#define DIC_READONLY	-6
#define DIC_NODRIVE 	-7
#define DIC_NOFILE		-8
