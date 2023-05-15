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
 * �Լ����� ����� api68k.c �� ���� FP ���� ������ �� 
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


/* api68k.c ���� �Լ� */
extern int api_funcent(void *argv);

/* ����ó���� ���� ��ũ�� (�μ��� ���� ���� �з�) */
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

/* �θ��� ��ȯ (īŸī��) */
int apiask_15_RomaToKata(char *src, char *dst)
{
	CALLAPI2(15, src, dst);
}

/* �θ��� ��ȯ (���󰡳�) */
int apiask_16_RomaToHira(char *src, char *dst)
{
	CALLAPI2(16, src, dst);
}

/* ��ȯ (������ �ĺ��� �ۼ�)  */
int apiask_19_Henkan(char *buf, char *kouho, char *kouzoku)
{
	CALLAPI3(19, buf, kouho, kouzoku);
}

/* �ĺ��� ��� ���� ���� �ĺ� */
int apiask_20_PrevKouho(char *kouho, char *kouzoku)
{
	CALLAPI2(20, kouho, kouzoku);
}

/* �ĺ��� ��� ���� ���� �ĺ� */
int apiask_21_NextKouho(char *kouho, char *kouzoku)
{
	CALLAPI2(21, kouho, kouzoku);
}

/* ���� �ĺ� ����� �ۼ� */
int apiask_22_PrevKouhoBlock(char *kouho, char *kouzoku)
{
	CALLAPI2(22, kouho, kouzoku);
}

/* ���� �ĺ� ����� �ۼ�  */
int apiask_23_NextKouhoBlock(char *kouho, char *kouzoku)
{
	CALLAPI2(23, kouho, kouzoku);
}

/* ��ü Ȯ�� */
int apiask_24_AllKakutei(char *buf)
{
	CALLAPI1(24, buf);
}

/* ���� ���� Ȯ�� */
int apiask_25_FirstKakutei(int no, char *buf, char *kouho, char *kouzoku, short *nkouho)
{
	CALLAPI5(25, no, buf, kouho, kouzoku, nkouho);
}

/* ���� ���� */
int apiask_28_OpenDic(void)
{
	CALLAPI0(28);
}

/* ���� Ŭ���� */
int apiask_29_CloseDic(void)
{
	CALLAPI0(29);
}

/* ���� �ܾ� ��� */
int apiask_30_TourokuAdd(int ndic, char *yomi, char *tango, int syn)
{
	CALLAPI4(30, ndic, yomi, tango, syn);
}

/* ���� �ܾ� ���� */
int apiask_31_TourokuDelete(int ndic, char *yomi, char *tango, int syn)
{
	CALLAPI4(31, ndic, yomi, tango, syn);
}

/* ���� ���� ���� */
int apiask_32_SetDicName(char *maindic, char *subdic)
{
	CALLAPI2(32, maindic, subdic);
}

/* ���ι����� 1�� ��� �ؼ� �� ��ȯ */
int apiask_33_LongerKouho(char *kouho, char *kouzoku)
{
	CALLAPI2(33, kouho, kouzoku);
}

/* ���� ������ 1�� �ª�� �ؼ� �纯ȯ */
int apiask_34_ShorterKouho(char *kouho, char *kouzoku)
{
	CALLAPI2(34, kouho, kouzoku);
}

/* ���� �������� �̵� */
int apiask_35_PrevBunsetu(char *kouho, char *kouzoku)
{
	CALLAPI2(35, kouho, kouzoku);
}

/* ���� �������� �̵� */
int apiask_36_NextBunsetu(char *kouho, char *kouzoku)
{
	CALLAPI2(36, kouho, kouzoku);
}

/* ���ι����� ���� �� ��ȯ (īŸī��) */
int apiask_37_FirstHenkanToKata(char *kouho, char *kouzoku)
{
	CALLAPI2(37, kouho, kouzoku);
}

/* ���ι����� ���� �� ��ȯ (���󰡳�) */
int apiask_38_FirstHenkanToHira(char *kouho, char *kouzoku)
{
	CALLAPI2(38, kouho, kouzoku);
}

/* ���ι����� ������ ��ȯ (�ݰ�) */
int apiask_39_FirstHenkanToHan(char *kouho, char *kouzoku)
{
	CALLAPI2(39, kouho, kouzoku);
}

/* ���ι����� ������ ��ȯ (����) */
int apiask_40_FirstHenkanToZen(char *kouho, char *kouzoku)
{
	CALLAPI2(40, kouho, kouzoku);
}
