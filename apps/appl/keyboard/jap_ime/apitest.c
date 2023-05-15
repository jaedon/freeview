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
 * apiask_�Լ��� ���� �׽�Ʈ �� 	
 *
 */

#include "../apiask.h"

#include <stdio.h>
#include <string.h> /* strcpy �� */

/* ���� ���� �̸� */
#define MAIN_DIC "c:\\netfront\\ask\\ask3.dic"

/* ���ڿ� ��� �Լ���.  */
static FILE *outfp = NULL;
static void outstr(char *s);
static void outstr2(char *s, char *t);
static void outnum(char *s, int n);
static void outresult(int result);

/* �׽�Ʈ �̰����� */
static void test1(void);
static void test2(void);
static void test3(void);
static void test4(void);

/* apiask.c �� ����׸� ���� �׽�Ʈ �Լ�. */
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

/* �θ��� ��ȯ  */
static void test1(void)
{
	char src[80];
	char dst[80];
	int ret;

	{
		outstr("----------");
		outstr("�θ��� -> īŸī�� ��ȯ �׽�Ʈ ");
		
		strcpy(src, "[vaiorinwo,katakananisitekudasai.]");
		ret = apiask_15_RomaToKata(src, dst);
		
		outstr("apiask_15_RomaToKata");
		outnum("ret", ret);
		outstr2("src", src);
		outstr2("dst", dst);
		
//		outresult(strcmp(dst, "�������������򡢫������ʫ˫��ƫ�����������") == 0);
		outresult(strcmp(dst, "�u���@�C�I�������A�J�^�J�i�j�V�e�N�_�T�C�B�v") == 0);
	}

	{
		outstr("----------");
		outstr("�θ��� -> ���󰡳� ��ȯ �׽�Ʈ");
		
		strcpy(src, "[vaiorinwo,hiragananisitekudasai.]");
		ret = apiask_16_RomaToHira(src, dst);
		
		outstr("apiask_16_RomaToHira");
		outnum("ret", ret);
		outstr2("src", src);
		outstr2("dst", dst);
		
//		outresult(strcmp(dst, "�������������򡢪Ҫ骬�ʪ˪��ƪ�����������") == 0);
		outresult(strcmp(dst, "�u�������������A�Ђ炪�Ȃɂ��Ă��������B�v") == 0);
	}
}

/* �⺻���� �ϰ���ȯ */
static void test2(void)
{
	char buf[1024];	/* Ugh!: ũ�⸦ �ʸ� ũ�� ��ҳ�  */
	char kouho[1024];
	char kouzoku[1024];
	int ret;
	char *maindic, *subdic;
//	char *src = "���󪸪Ϊت󪫪�򪹪�ΪϪȪƪ�઺�������Ȫ����ƪ��ު���";
	char *src = "���񂶂̂ւ񂩂������̂͂ƂĂ��ނ��������Ƃ����Ă��܂��B";

	outstr("----------");
	outstr("�ϰ���ȯ �׽�Ʈ ");

	{
		outstr("> ���� ������ ����");
		maindic = MAIN_DIC;
		subdic = "";
		ret = apiask_32_SetDicName(maindic, subdic);
		outstr("apiask_32_SetDicName");
		outnum("ret", ret);
		outstr2("maindic", maindic);
		outstr2("subdic", subdic);
	}

	{
		outstr("> �ϰ���ȯ ");
		/* ���� ������ ����  */
		ret = apiask_28_OpenDic();
		/* ��ȯ */
		strcpy(buf, src);
		ret = apiask_19_Henkan(buf, kouho, kouzoku);
		outstr("apiask_19_Henkan");
		outnum("ret", ret);
		outstr2("src", src);
		outstr2("buf", buf);
		outstr2("kouho", kouho);
		outstr2("kouzoku", kouzoku);
		/* �������� Ŭ�ο���  */
		ret = apiask_29_CloseDic();
	}
}

/* �⺻���� ��ȯ�� ���� Ȯ�� */
static void test3(void)
{
	char buf[1024];	/* Ugh!: ũ�Ⱑ �ʹ� ū��  */
	char kouho[1024];
	char kouzoku[1024];
	int ret;
	char *maindic, *subdic;
//	char *src = "���󪸪Ϊت󪫪�򪹪�ΪϪȪƪ�઺�������Ȫ����ƪ��ު���";
	char *src = "���񂶂̂ւ񂩂������̂͂ƂĂ��ނ��������Ƃ����Ă��܂��B";

	outstr("----------");
	outstr("��ȯ�� ��üȮ���� �׽�Ʈ ");

	/* ���� ������ ���� */
	maindic = MAIN_DIC;
	subdic = "";
	ret = apiask_32_SetDicName(maindic, subdic);

	/* ������ ���� */
	ret = apiask_28_OpenDic();

	outstr("> ��ȯ");
	strcpy(buf, src);
	ret = apiask_19_Henkan(buf, kouho, kouzoku);
	outstr("apiask_19_Henkan");
	outnum("ret", ret);
	outstr2("src", src);
	outstr2("buf", buf);
	outstr2("kouho", kouho);
	outstr2("kouzoku", kouzoku);

	outstr("> ���� Ȯ��");
	buf[0] = '\0';
	ret = apiask_24_AllKakutei(buf);
	outstr("apiask_24_AllKakutei");
	outnum("ret", ret);
	outstr2("buf", buf);
	
	/* �������� Ŭ���� */
	ret = apiask_29_CloseDic();
}

/* �ĺ��� ����� ��ü �ĺ��� ǥ�� */
static void test4(void)
{
	char buf[1024];	/* Ugh!: ũ�⸦ �ʹ� ũ�� ��ҳ�  */
	char kouho[1024];
	char kouzoku[1024];
	int ret;
	int kouhonum;
//	char *src = "���󪸪Ϊت󪫪�򪹪�ΪϪȪƪ�઺�������Ȫ����ƪ��ު���";
	char *src = "���񂶂̂ւ񂩂������̂͂ƂĂ��ނ��������Ƃ����Ă��܂��B";

	outstr("----------");
	outstr("��ȯ�� ��ü �ĺ� ǥ���� �׽�Ʈ");

	/* ���� ������ ���� */
	ret = apiask_32_SetDicName(MAIN_DIC, "");

	/* ���� ���� */
	ret = apiask_28_OpenDic();

	outstr("> ��ȯ");
	strcpy(buf, src);
	ret = apiask_19_Henkan(buf, kouho, kouzoku);
	outstr("apiask_19_Henkan");
	outnum("ret", ret);
	outstr2("src", src);
	outstr2("buf", buf);
	outstr2("kouho", kouho);
	outstr2("kouzoku", kouzoku);
	kouhonum = ret;
	outnum("<> ���ι����� �ĺ� �� ", kouhonum);

	outstr("> ��ü �ĺ��� ǥ��");
	while (ret > 0) {
		ret = apiask_21_NextKouho(kouho, kouzoku);
		outnum("<>�ĺ���ȣ", ret);
		outstr("apiask_21_NextKouho");
		outstr2("kouho", kouho);
		outstr2("kouzoku", kouzoku);
	}
	
	/* ������ Ŭ���� */
	ret = apiask_29_CloseDic();
}

/***********************************************/
/* ���ϴ� ��û �Լ�. */

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
		outstr("OK : �����Դϴ�.");
	} else {
		outstr("NG : �����Դϴ�.");
	}
}
