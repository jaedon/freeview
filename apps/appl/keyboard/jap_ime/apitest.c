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
	apitest.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
 *
 * apiask_ÇÔ¼öÀÇ µ¿ÀÛ Å×½ºÆ® ¿ë 	
 *
 */

#include "../apiask.h"

#include <stdio.h>
#include <string.h> /* strcpy µî */

/* »çÀü ÆÄÀÏ ÀÌ¸§ */
#define MAIN_DIC "c:\\netfront\\ask\\ask3.dic"

/* ¹®ÀÚ¿­ Ãâ·Â ÇÔ¼ö±º.  */
static FILE *outfp = NULL;
static void outstr(char *s);
static void outstr2(char *s, char *t);
static void outnum(char *s, int n);
static void outresult(int result);

/* Å×½ºÆ® ÀÌ°ÍÀú°Í */
static void test1(void);
static void test2(void);
static void test3(void);
static void test4(void);

/* apiask.c ÀÇ µð¹ö±×¸¦ À§ÇÑ Å×½ºÆ® ÇÔ¼ö. */
void apiask_test(int nTestNumber)
{
	outfp = fopen("\\asktest.txt", "w");
	outnum("apiask_test start : TestNumber", nTestNumber);
	switch (nTestNumber) {
	case 0:
		test1();
		test2();
		test3();
		test4();
		break;
		
	default:
		outstr("bad test number");
	}
	outstr("apiask_test end");
	fclose(outfp);
}

/* ·Î¸¶ÀÚ º¯È¯  */
static void test1(void)
{
	char src[80];
	char dst[80];
	int ret;

	{
		outstr("----------");
		outstr("·Î¸¶ÀÚ -> Ä«Å¸Ä«³ª º¯È¯ Å×½ºÆ® ");
		
		strcpy(src, "[vaiorinwo,katakananisitekudasai.]");
		ret = apiask_15_RomaToKata(src, dst);
		
		outstr("apiask_15_RomaToKata");
		outnum("ret", ret);
		outstr2("src", src);
		outstr2("dst", dst);
		
//		outresult(strcmp(dst, "¡¸«ô«¡«¤«ª«ê«ó«ò¡¢«««¿«««Ê«Ë«·«Æ«¯«À«µ«¤¡£¡¹") == 0);
		outresult(strcmp(dst, "uƒ”ƒ@ƒCƒIƒŠƒ“ƒ’AƒJƒ^ƒJƒiƒjƒVƒeƒNƒ_ƒTƒCBv") == 0);
	}

	{
		outstr("----------");
		outstr("·Î¸¶ÀÚ -> È÷¶ó°¡³ª º¯È¯ Å×½ºÆ®");
		
		strcpy(src, "[vaiorinwo,hiragananisitekudasai.]");
		ret = apiask_16_RomaToHira(src, dst);
		
		outstr("apiask_16_RomaToHira");
		outnum("ret", ret);
		outstr2("src", src);
		outstr2("dst", dst);
		
//		outresult(strcmp(dst, "¡¸«ôª¡ª¤ªªªêªóªò¡¢ªÒªéª¬ªÊªËª·ªÆª¯ªÀªµª¤¡£¡¹") == 0);
		outresult(strcmp(dst, "uƒ”‚Ÿ‚¢‚¨‚è‚ñ‚ðA‚Ð‚ç‚ª‚È‚É‚µ‚Ä‚­‚¾‚³‚¢Bv") == 0);
	}
}

/* ±âº»ÀûÀÎ ÀÏ°ýº¯È¯ */
static void test2(void)
{
	char buf[1024];	/* Ugh!: Å©±â¸¦ ³Ê¸ð Å©°Ô Àâ¾Ò³ª  */
	char kouho[1024];
	char kouzoku[1024];
	int ret;
	char *maindic, *subdic;
//	char *src = "ª«ªóª¸ªÎªØªóª«ªóªòª¹ªëªÎªÏªÈªÆªâªàªºª«ª·ª¤ªÈª¤ªïªìªÆª¤ªÞª¹¡£";
	char *src = "‚©‚ñ‚¶‚Ì‚Ö‚ñ‚©‚ñ‚ð‚·‚é‚Ì‚Í‚Æ‚Ä‚à‚Þ‚¸‚©‚µ‚¢‚Æ‚¢‚í‚ê‚Ä‚¢‚Ü‚·B";

	outstr("----------");
	outstr("ÀÏ°ýº¯È¯ Å×½ºÆ® ");

	{
		outstr("> »çÀü ÆÄÀÏÀÇ ¼³Á¤");
		maindic = MAIN_DIC;
		subdic = "";
		ret = apiask_32_SetDicName(maindic, subdic);
		outstr("apiask_32_SetDicName");
		outnum("ret", ret);
		outstr2("maindic", maindic);
		outstr2("subdic", subdic);
	}

	{
		outstr("> ÀÏ°ýº¯È¯ ");
		/* »çÀü ÆÄÀÏÀÇ ¿ÀÇÂ  */
		ret = apiask_28_OpenDic();
		/* º¯È¯ */
		strcpy(buf, src);
		ret = apiask_19_Henkan(buf, kouho, kouzoku);
		outstr("apiask_19_Henkan");
		outnum("ret", ret);
		outstr2("src", src);
		outstr2("buf", buf);
		outstr2("kouho", kouho);
		outstr2("kouzoku", kouzoku);
		/* »çÀüÆÄÀÏ Å¬·Î¿ìÁî  */
		ret = apiask_29_CloseDic();
	}
}

/* ±âº»ÀûÀÎ º¯È¯°ú ÀüºÎ È®Á¤ */
static void test3(void)
{
	char buf[1024];	/* Ugh!: Å©±â°¡ ³Ê¹« Å«°¡  */
	char kouho[1024];
	char kouzoku[1024];
	int ret;
	char *maindic, *subdic;
//	char *src = "ª«ªóª¸ªÎªØªóª«ªóªòª¹ªëªÎªÏªÈªÆªâªàªºª«ª·ª¤ªÈª¤ªïªìªÆª¤ªÞª¹¡£";
	char *src = "‚©‚ñ‚¶‚Ì‚Ö‚ñ‚©‚ñ‚ð‚·‚é‚Ì‚Í‚Æ‚Ä‚à‚Þ‚¸‚©‚µ‚¢‚Æ‚¢‚í‚ê‚Ä‚¢‚Ü‚·B";

	outstr("----------");
	outstr("º¯È¯°ú ÀüÃ¼È®Á¤ÀÇ Å×½ºÆ® ");

	/* »çÀü ÆÄÀÏÀÇ ¼³Á¤ */
	maindic = MAIN_DIC;
	subdic = "";
	ret = apiask_32_SetDicName(maindic, subdic);

	/* »çÀüÀÇ ¿ÀÇÂ */
	ret = apiask_28_OpenDic();

	outstr("> º¯È¯");
	strcpy(buf, src);
	ret = apiask_19_Henkan(buf, kouho, kouzoku);
	outstr("apiask_19_Henkan");
	outnum("ret", ret);
	outstr2("src", src);
	outstr2("buf", buf);
	outstr2("kouho", kouho);
	outstr2("kouzoku", kouzoku);

	outstr("> ÀüºÎ È®Á¤");
	buf[0] = '\0';
	ret = apiask_24_AllKakutei(buf);
	outstr("apiask_24_AllKakutei");
	outnum("ret", ret);
	outstr2("buf", buf);
	
	/* »çÀüÆÄÀÏ Å¬·ÎÁî */
	ret = apiask_29_CloseDic();
}

/* ÈÄº¸±º ºí·ÏÀÇ ÀüÃ¼ ÈÄº¸¸¦ Ç¥½Ã */
static void test4(void)
{
	char buf[1024];	/* Ugh!: Å©±â¸¦ ³Ê¹« Å©°Ô Àâ¾Ò³ª  */
	char kouho[1024];
	char kouzoku[1024];
	int ret;
	int kouhonum;
//	char *src = "ª«ªóª¸ªÎªØªóª«ªóªòª¹ªëªÎªÏªÈªÆªâªàªºª«ª·ª¤ªÈª¤ªïªìªÆª¤ªÞª¹¡£";
	char *src = "‚©‚ñ‚¶‚Ì‚Ö‚ñ‚©‚ñ‚ð‚·‚é‚Ì‚Í‚Æ‚Ä‚à‚Þ‚¸‚©‚µ‚¢‚Æ‚¢‚í‚ê‚Ä‚¢‚Ü‚·B";

	outstr("----------");
	outstr("º¯È¯°ú ÀüÃ¼ ÈÄº¸ Ç¥½ÃÀÇ Å×½ºÆ®");

	/* »çÀü ÆÄÀÏÀÇ ¼³Á¤ */
	ret = apiask_32_SetDicName(MAIN_DIC, "");

	/* »çÀü ¿ÀÇÂ */
	ret = apiask_28_OpenDic();

	outstr("> º¯È¯");
	strcpy(buf, src);
	ret = apiask_19_Henkan(buf, kouho, kouzoku);
	outstr("apiask_19_Henkan");
	outnum("ret", ret);
	outstr2("src", src);
	outstr2("buf", buf);
	outstr2("kouho", kouho);
	outstr2("kouzoku", kouzoku);
	kouhonum = ret;
	outnum("<> ¼±µÎ¹®ÀýÀÇ ÈÄº¸ ¼ö ", kouhonum);

	outstr("> ÀüÃ¼ ÈÄº¸ÀÇ Ç¥½Ã");
	while (ret > 0) {
		ret = apiask_21_NextKouho(kouho, kouzoku);
		outnum("<>ÈÄº¸¹øÈ£", ret);
		outstr("apiask_21_NextKouho");
		outstr2("kouho", kouho);
		outstr2("kouzoku", kouzoku);
	}
	
	/* »çÀüÀÇ Å¬·ÎÁî */
	ret = apiask_29_CloseDic();
}

/***********************************************/
/* ÀÌÇÏ´Â ÇÏÃ» ÇÔ¼ö. */

static void outstr(char *s)
{
	if (outfp) {
		fprintf(outfp, "ask:%s\n", s);
	}
}

static void outstr2(char *s, char *t)
{
	if (outfp) {
		fprintf(outfp, "ask:%s=%s\n", s, t);
	}
}

static void outnum(char *s, int n)
{
	if (outfp) {
		fprintf(outfp, "ask:%s=%d\n", s, n);
	}
}

static void outresult(int result)
{
	if (result) {
		outstr("OK : ¼º°øÀÔ´Ï´Ù.");
	} else {
		outstr("NG : ½ÇÆÐÀÔ´Ï´Ù.");
	}
}
