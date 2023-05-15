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
	apiask.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
 *
 * ÇÔ¼ö±ºÀÇ »ç¾çÀº api68k.c ÀÇ ´ëÀÀ FP ÄÝÀ» ÂüÁ¶ÇÒ °Í 
 *
 */

#include	"askdef.h"
#include	"askkey.h"
#include	"mean.h"
#include	"imode.h"
#include	"pword.h"
#include	"hwork.h"
#include	"dic.h"
#include	"henkan.h"
#include	"aci.h"

#include	"../apiask.h"


/* api68k.c ³»ÀÇ ÇÔ¼ö */
extern int api_funcent(void *argv);

/* Á¤ÇüÃ³¸®¸¦ À§ÇÑ ¸ÅÅ©·Î (ÀÎ¼öÀÇ °¹¼ö º°·Î ºÐ·ù) */
#define CALLAPI0(funcno) \
	void *argv[1]; \
	argv[0] = (void *)funcno; \
	return api_funcent(argv)

#define CALLAPI1(funcno,a0) \
	void *argv[2]; \
	argv[0] = (void *)funcno; \
	argv[1] = (void *)a0; \
	return api_funcent(argv)

#define CALLAPI2(funcno,a0,a1) \
	void *argv[3]; \
	argv[0] = (void *)funcno; \
	argv[1] = (void *)a0; \
	argv[2] = (void *)a1; \
	return api_funcent(argv)

#define CALLAPI3(funcno,a0,a1,a2) \
	void *argv[4]; \
	argv[0] = (void *)funcno; \
	argv[1] = (void *)a0; \
	argv[2] = (void *)a1; \
	argv[3] = (void *)a2; \
	return api_funcent(argv)

#define CALLAPI4(funcno,a0,a1,a2,a3) \
	void *argv[5]; \
	argv[0] = (void *)funcno; \
	argv[1] = (void *)a0; \
	argv[2] = (void *)a1; \
	argv[3] = (void *)a2; \
	argv[4] = (void *)a3; \
	return api_funcent(argv)

#define CALLAPI5(funcno,a0,a1,a2,a3,a4) \
	void *argv[6]; \
	argv[0] = (void *)funcno; \
	argv[1] = (void *)a0; \
	argv[2] = (void *)a1; \
	argv[3] = (void *)a2; \
	argv[4] = (void *)a3; \
	argv[5] = (void *)a4; \
	return api_funcent(argv)

/* ·Î¸¶ÀÚ º¯È¯ (Ä«Å¸Ä«³ª) */
int apiask_15_RomaToKata(char *src, char *dst)
{
	CALLAPI2(15, src, dst);
}

/* ·Î¸¶ÀÚ º¯È¯ (È÷¶ó°¡³ª) */
int apiask_16_RomaToHira(char *src, char *dst)
{
	CALLAPI2(16, src, dst);
}

/* º¯È¯ (ÃÖÃÊÀÇ ÈÄº¸±º ÀÛ¼º)  */
int apiask_19_Henkan(char *buf, char *kouho, char *kouzoku)
{
	CALLAPI3(19, buf, kouho, kouzoku);
}

/* ÈÄº¸±º ºí·Ï ³»ÀÇ ÀÌÀü ÈÄº¸ */
int apiask_20_PrevKouho(char *kouho, char *kouzoku)
{
	CALLAPI2(20, kouho, kouzoku);
}

/* ÈÄº¸±º ºí·Ï ³»ÀÇ ´ÙÀ½ ÈÄº¸ */
int apiask_21_NextKouho(char *kouho, char *kouzoku)
{
	CALLAPI2(21, kouho, kouzoku);
}

/* ÀÌÀü ÈÄº¸ ºí·ÏÀÇ ÀÛ¼º */
int apiask_22_PrevKouhoBlock(char *kouho, char *kouzoku)
{
	CALLAPI2(22, kouho, kouzoku);
}

/* ÀÌÈÄ ÈÄº¸ ºí·ÏÀÇ ÀÛ¼º  */
int apiask_23_NextKouhoBlock(char *kouho, char *kouzoku)
{
	CALLAPI2(23, kouho, kouzoku);
}

/* ÀüÃ¼ È®Á¤ */
int apiask_24_AllKakutei(char *buf)
{
	CALLAPI1(24, buf);
}

/* ¼±µÎ ¹®Àý È®Á¤ */
int apiask_25_FirstKakutei(int no, char *buf, char *kouho, char *kouzoku, short *nkouho)
{
	CALLAPI5(25, no, buf, kouho, kouzoku, nkouho);
}

/* »çÀü ¿ÀÇÂ */
int apiask_28_OpenDic(void)
{
	CALLAPI0(28);
}

/* »çÀü Å¬·ÎÁî */
int apiask_29_CloseDic(void)
{
	CALLAPI0(29);
}

/* »çÀü ´Ü¾î µî·Ï */
int apiask_30_TourokuAdd(int ndic, char *yomi, char *tango, int syn)
{
	CALLAPI4(30, ndic, yomi, tango, syn);
}

/* »çÀü ´Ü¾î »èÁ¦ */
int apiask_31_TourokuDelete(int ndic, char *yomi, char *tango, int syn)
{
	CALLAPI4(31, ndic, yomi, tango, syn);
}

/* »çÀü ÆÄÀÏ ¼³Á¤ */
int apiask_32_SetDicName(char *maindic, char *subdic)
{
	CALLAPI2(32, maindic, subdic);
}

/* ¼±µÎ¹®ÀýÀ» 1ÀÚ ±æ°Ô ÇØ¼­ Àç º¯È¯ */
int apiask_33_LongerKouho(char *kouho, char *kouzoku)
{
	CALLAPI2(33, kouho, kouzoku);
}

/* ¼±µÎ ¹®ÀýÀ» 1ÀÚ åÂª°Ô ÇØ¼­ Àçº¯È¯ */
int apiask_34_ShorterKouho(char *kouho, char *kouzoku)
{
	CALLAPI2(34, kouho, kouzoku);
}

/* ÀÌÀü ¹®Àý·ÎÀÇ ÀÌµ¿ */
int apiask_35_PrevBunsetu(char *kouho, char *kouzoku)
{
	CALLAPI2(35, kouho, kouzoku);
}

/* ÀÌÈÄ ¹®Àý·ÎÀÇ ÀÌµ¿ */
int apiask_36_NextBunsetu(char *kouho, char *kouzoku)
{
	CALLAPI2(36, kouho, kouzoku);
}

/* ¼±µÎ¹®ÀýÀÇ ¹®ÀÚ Á¾ º¯È¯ (Ä«Å¸Ä«³ª) */
int apiask_37_FirstHenkanToKata(char *kouho, char *kouzoku)
{
	CALLAPI2(37, kouho, kouzoku);
}

/* ¼±µÎ¹®ÀýÀÇ ¹®ÀÚ Á¾ º¯È¯ (È÷¶ó°¡³ª) */
int apiask_38_FirstHenkanToHira(char *kouho, char *kouzoku)
{
	CALLAPI2(38, kouho, kouzoku);
}

/* ¼±µÎ¹®ÀýÀÇ ¹®ÀÚÁ¾ º¯È¯ (¹Ý°¢) */
int apiask_39_FirstHenkanToHan(char *kouho, char *kouzoku)
{
	CALLAPI2(39, kouho, kouzoku);
}

/* ¼±µÎ¹®ÀýÀÇ ¹®ÀÚÁ¾ º¯È¯ (Àü°¢) */
int apiask_40_FirstHenkanToZen(char *kouho, char *kouzoku)
{
	CALLAPI2(40, kouho, kouzoku);
}
