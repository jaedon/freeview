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
	dictypes.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/****************************************************************/
/*													*/
/*		ASK 3										*/
/*		사전 매니저 + 커널								*/
/*		공통 헤더 파일.								*/
/*													*/
/*						Made by K.O. 1991/11/19 		*/
/*													*/
/****************************************************************/

/*		범용 형선언		*/
#ifndef __dic_types_h

#define __dic_types_h

typedef unsigned char	UCODE;	/* U 코드 */
typedef unsigned long	ENTID;	/* 단어 레코드 엔트리 ID */
typedef unsigned short	WDID;	/* 레코드 중의 단어 ID  */
/*typedef		int 			ULEN;	* 읽은 길이  */
typedef short			ULEN;	/* 읽은 길이 */
typedef unsigned char	SYNTAX; /* 품사 정보 */
typedef unsigned char	SETU;	/* 부속어의 접속 정보 */
typedef unsigned char	KATUYO; /* 활용 부의 정보 */
typedef unsigned char	LEVEL;	/* 단어 레벨 */
typedef unsigned short	RTIME;	/* 참조 시각 */

#endif
