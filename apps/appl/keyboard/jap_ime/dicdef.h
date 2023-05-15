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
	dicdef.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*		범용 선언			*/
#ifndef NULL
#define NULL	((void*)0)
#endif

#define YES		1
#define NO		0

#define OK		0
#define ERR		-1

#define ON		1
#define OFF		0

/*		사전 파일 핸들 	*/
typedef int		HAND;			/* 파일 핸들 */
#define 	CLOSE	-1				/* 클로즈 되어 있다. */
extern	HAND	dic_fd;			/* 사전용 파일 포인터  */
extern	int		dic_rw;			/* READ or RDWR */

/*		읽기(u코드)의 길이	*/
#define ULENMAX			80

/*		레코드 참조 매크로		*/
#define RECORD_LEN(p)	((ushort)(*p + (*(p+1))*256))
#define UCODE_LEN(p)	(*(p+2))
#define UCODE_STR(p)	(p+3)
#define WD_STR(p)		(p + 3 + *(p+2))
#define IS_SYNTAX(c)			(c <= 56)
#define IS_SYNTAXINFO(c)		(c <= 11)

/*		WDS 의 사이즈		*/
#define SIZEWDS			(short)(sizeof(WDID)+sizeof(RTIME)+sizeof(LEVEL))

/*		최대 RTIME				*/
#define RTIMEMAX		255

/*		단어 ID 					*/
#define MAIN	0x0000
#define TEMP	0x8000

/*		학습 모드 				*/
extern	int		gaku_mode;

/*		숫자 변환 모드				*/
extern	int		suji_mode;

/*		커런트 임시 단어 페이지 	*/
extern	int		temp_page;

/*		레코드 검색 구조체			*/
#define NONE	-1
typedef struct recinfo_entry {
/*		int		page;			* 레코드 페이지  */
/*		int		offset;			* 레코드의 오프셋 (없을 땐 NONE) */
		short		page;			/* 레코드 페이지  */
		short		offset;			/* 레코드의 오프셋 (없을 땐 NONE) */
		ULEN	yomilen;			/* ENTRYS 와 같은 의미 */
		ULEN	used_ulen;		/* ENTRYS 와 같은 의미 */
		ULEN	page_ulen;		/* ENTRYS 와 같은 의미 */
		uchar	*ptr;			/* 레코드 에의 포인터 */
} RECINFO;

/*		페이지 관계 				*/
#define PAGEMAX			1024
#define HEADERSIZE		1024
#define INDEXLEN		4
#define INDEXSIZE		INDEXLEN*PAGEMAX
#define TEMPPAGES		8
#define TEMPSIZE		TEMPPAGES*PAGEMAX
#define INDEXOFFSET		HEADERSIZE
#define TEMPOFFSET		INDEXOFFSET+INDEXSIZE
#define PAGEOFFSET		TEMPOFFSET+TEMPSIZE

/*		일시 단어 오프셋		*/
/*		PPPP OOOO OOOO OOOO		*/
#define TEMP_PAGE(x)	((short)(x >> 12))
#define TEMP_OFFSET(x)	((short)(x & 0x0FFF))

/*		파일 액세스 			*/
#ifdef	X68K
#define READ			0x0000
#define WRITE			0x0001
#define RDWR			0x0002
#else	/* PC98 */
#define READ			0x8000
#define WRITE			0x8001
#define RDWR			0x8002
#endif
