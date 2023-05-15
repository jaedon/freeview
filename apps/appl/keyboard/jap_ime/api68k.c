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
	api68k.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/



/*
**
**		Function Pointer �Լ� ($FF22)  for X68000
**
**								by K.Ohta		1991/12/4
*/

#include	<string.h>

#include	"askdef.h"
#include	"askkey.h"
#include	"mean.h"
#include	"imode.h"
#include	"pword.h"
#include	"hwork.h"
#include	"dic.h"
#include	"henkan.h"
#include	"aci.h"
#include	"api68k.h"
#include	"dicdef.h"
#include	"dic_main.h"
#include	"autab.h"
#include	"roman.h"
#include	"apiroman.h"
#include	"kouho.h"
#include	"select.h"

static void get_kouhostr(APIPTR kouho, APIPTR kouzoku);


/*** API ���� �� ***/
#define API_FNOERR		(-1)
#define API_FATAL		(-1)
#define API_OK			(0)
#define API_MODEERR		(2)

/*** FP ��ȯ ����� ���� ***/
#define FP_NOKANJI		0
#define FP_EDIT0		1
#define FP_EDITING		2
#define FP_SELECT		3

extern u_char	dic_name[];

int		func_mode = FP_NOKANJI; /* FP �ݿ� ���� ��ȯ�� �����ϴ� ���.  */
int		flush_mode = OFF;		/* FPŰ ���۵� �÷��� �Ѵ�.	*/
int		lock_mode = ON;			/* Ű �Է¿� ���� ���� ���ڸ� �Ұ����ϰ� ��.	*/
int		echo_lock = OFF;		/* ���ڸ����, Ű�� ���� ������ �㰡  */

static int (*api_func[])(APIARG *argv) = {
	api_none,				/*	1 */
	api_none,				/*	2 */
	api_none,				/*	3 */
	api_none,				/*	4 */
	api_none,				/*	5 */
	api_none,				/*	6 */
	api_none,				/*	7 */
	api_none,				/*	8 */
	api_none,				/*	9 */
	api_none,				/* 10 */
	api_none,				/* 11 */
	api_none,				/* 12 */
	api_none,				/* 13 */
	api_none,				/* 14 */
	api_Rstrconv,			/* 15 */
	api_Rstrconv,			/* 16 */
	api_none,				/* 17 */
	api_none,				/* 18 */
	api_henkan,				/* 19 */
	api_movekh,				/* 20 */
	api_movekh,				/* 21 */
	api_moveblk,			/* 22 */
	api_moveblk,			/* 23 */
	api_allfix,				/* 24 */
	api_select,				/* 25 */
	api_none,				/* 26 */
	api_none,				/* 27 */
	api_dicopen,			/* 28 */
	api_dicclose,			/* 29 */
	api_touroku,			/* 30 */
	api_touroku,			/* 31 */
	apis_dicname,			/* 32 */
	api_khlong,				/* 33 */
	api_khshort,			/* 34 */
	api_backbunsetu,		/* 35 */
	api_nextbunsetu,		/* 36 */
	api_khhenkan,			/* 37 */
	api_khhenkan,			/* 38 */
	api_khhenkan,			/* 39 */
	api_khhenkan,			/* 40 */
	api_khhenkan,			/* 41 */
	api_khhenkan,			/* 42 */
	api_khhenkan,			/* 43 */
	api_khhenkan,			/* 44 */
	api_none,				/* 45 */
	api_none,				/* 46 */
	api_none,				/* 47 */
	api_none,				/* 48 */
	api_none,				/* 49 */
	api_none,				/* 50 */
	api_none,				/* 51 */
	api_none,				/* 52 */
	api_none,				/* 53 */
	api_none,				/* 54 */
	api_none,				/* 55 */
	api_none,				/* 56 */
	api_none,				/* 57 */
	api_none,				/* 58 */
	api_none,				/* 59 */
	api_none,				/* 60 */
	api_none,				/* 61 */
	api_none,				/* 62 */
	api_none				/* 63 */
};
#define API_MAXFUNC		(sizeof(api_func) / sizeof(api_func[0]))

/************************************************************************/
/*														*/
/*		FP�� ���� ��Ʈ�� 									*/
/*														*/
/************************************************************************/
APIRET api_funcent(APIARG *argv)
{
	int num;

	num = (int)(*argv++);
	if (num <= 0 || num > API_MAXFUNC)
		return(API_FNOERR);

	return((APIRET)(*api_func[num - 1])(argv));
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 15, 16 									*/
/*		�Լ�		int		api_Rstrconv( av )					*/
/*		���		���ڿ��� ��ȯ (�θ��� ��ȯ)					*/
/*		�μ�		FNO = av[-1];			��ȯ ��� 			*/
/*						= 15	�ݰ� --> ���� īŸī�� (�θ��� ��ȯ) 	*/
/*						= 16	�ݰ� --> ���� ���󰡳� (�θ��� ��ȯ)	*/
/*				SRC = av[0];			��ȯ�� ���ڿ��� ������		*/
/*				DST = av[1];			��ȯ��� �ݳ� ����� ������	*/
/*		����		0 �Ǵ� ������ ������ �� ������ ��ġ 	 		*/
/*					= 0		��������						*/
/*					> 0		��ȯ������ ���� ������ ������,	*/
/*							���η� ������ ���� �� (�ݰ�ȯ��)	*/
/*		�ؼ�												*/
/*		����		��ȯ������ ���ڿ��� ���̴� 79����Ʈ ����.		*/
/*														*/
/************************************************************************/
int api_Rstrconv(APIARG av[])
{
	extern u_char wdsbuf[];		
	int mode;
	register APIPTR src, dst;
	BIT16 kana_mask;
	register BIT16 *p, *q;
	BIT16 code, rtmp[2];
	int l;
	int rlen, nt;

	mode = (int)*(av - 1);
	src = (APIPTR)av[0];
	dst = (APIPTR)av[1];

	if (mode == 15)		/* �ݰ� --> ���� īŸī�� (�θ��� ��ȯ)	*/
		kana_mask = (IC_KATAKANA | IC_ZENKAKU);
	else					/* �ݰ� --> ���� ���󰡳� (�θ��� ��ȯ)	*/
		kana_mask = IC_ZENKAKU;

	p = (BIT16 *)wdsbuf;	/* use for work */
	l = 0;
	while (*src && p < (BIT16 *)&wdsbuf[1024]) {
		*p++ = (BIT16)(*src++ | kana_mask);
		l++;
	}

	p = (BIT16 *)wdsbuf;	/* use for work */
	while (l > 0) {
		nt = to_roman(p,l,rtmp,&rlen);
		if (nt <= 0) {
			if (nt != -1)
#if 0 /* Changed By J.Katoh.1997/10/24 */
				return((int)(p - (BIT16 *)wdsbuf));
#else
				/* +1 ���� ������, ���ι��ڿ��� ������ ���, api_ok�� ������ �� ����. 	*/
				/* �̷� ���� �� ����°���� ���������� �����ϰ� �ȴ�. 				*/
				/* ������ ����� ������ ���� > 0 �� �Ǵ� ���̹Ƿ� ���� �ش�.			*/
				return((int)(p - (BIT16 *)wdsbuf) + 1);
#endif
			nt = 1;
		}
		p += nt;
		l -= nt;
		for (q = rtmp; rlen--; ) {
			code = ask_jis2sjis(*q++);
			if (code == 0x82F2)		/* V ����- ���󰡳� */
				code = 0x8394;		/* V ����- īŸī�� */
			*dst++ = (u_char)(code >> 8);
			*dst++ = (u_char)code;
		}
	}
	*dst++ = '\0';
	return(API_OK);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 19									*/
/*		�Լ�		int		api_henkan( av )					*/
/*		���		�ϰ�, ����, �̸��б� ��ȯ��, ������ �ĺ����� �ļ� ������ �ۼ�.	*/
/*		�μ�		BUF = av[0];		��ȯ�� ���� ���󰡳� ���ڿ�	*/
/*				KOUHO = av[1];	������ �ĺ��� �ݳ� ����		*/
/*				KOUZOKU = av[2];	�ļ� ������ �ݳ� ����		*/
/*		����		�ĺ��� �Ǵ� ���� �ڵ�. 						*/
/*					>= 0		�ĺ� ��						*/
/*					< 0		����							*/
/*		�ؼ�		BUF�� ���� 0�� ��, ���� �̸��б��� �ؼ��Ǿ�,	*/
/*				�̹� ������ �ִ� ���ڿ��� �����Ѵ�.		 	*/
/*		����												*/
/*														*/
/************************************************************************/
int api_henkan(APIARG av[])
{
	extern HCHAR hchar[];
	extern int kana_len, cur_pos, cur_len;
	APIPTR buf, kouho, kouzoku;

	if (func_mode == FP_NOKANJI)
		return(API_FATAL);

	buf = (APIPTR)av[0];
	kouho = (APIPTR)av[1];
	kouzoku = (APIPTR)av[2];

	if (buf != 0) {
		if (func_mode >= FP_EDITING)
			init_edit0();
		if (add_kana_buf(buf) < 0)
			return(API_FATAL);
	}
	if (kana_len <= 0) {
		*kouho = '\0';
		*kouzoku = '\0';
		return(0);
	}
	henkan(0,kana_len,KYOUSEI_HENKAN, -1);

	cur_pos = 0;	/* ������ �� �Լ�����, CUR_POS �� ���� �Ѵ�.  */
	cur_len = bunsetu_len(0);
	init_kouho(0,ON);
	get_kouhostr(kouho,kouzoku);

	func_mode = FP_SELECT;
	return(kh_count(hchar[0].kh_mem));
}

static void get_kouhostr(APIPTR	kouho, APIPTR kouzoku)
{
	extern int kana_len, cur_pos, cur_len;
	u_char *p;

	p = select_subtostr(cur_pos,cur_len);
	strcpy((char*)kouho,(char*)p);

	p = select_subtostr(cur_pos + cur_len,kana_len - cur_pos - cur_len);
	strcpy((char*)kouzoku,(char*)p);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 20, 21									*/
/*		�Լ�		int		api_movekh( av )					*/
/*		���		�ĺ��� ��� ���� ���� �ĺ��� ����Ѵ�.			*/
/*		�μ�		FNO = av[-1];			������ �� ���� 		*/
/*					= 20	�� �ĺ��� ���						*/
/*					= 21	�� �ĺ��� ���						*/
/*				KOUHO = av[0];		�ĺ��� �ݳ� ����		*/
/*				KOUZOKU = av[1];		�ļ� ������ �ݳ� ���� 	*/
/*		����		0 �Ǵ� �ĺ� ��ȣ							*/
/*					= 0		�ĺ� ����						*/
/*					> 0		�ĺ� ��ȣ						*/
/*		�ؼ�		�ĺ���ȣ��, ������ �ĺ��� �ȿ����� ��ȣ		*/
/*		����												*/
/*														*/
/************************************************************************/
int api_movekh(APIARG av[])
{
	extern int gun_num, gun_nkh;
	int mode, error = 0;
	APIPTR kouho, kouzoku;
	int pos;

	if (func_mode != FP_SELECT)
		return(0);

	mode = (int)*(av - 1);
	kouho = (APIPTR)av[0];
	kouzoku = (APIPTR)av[1];

	switch (mode) {
	case 20:
		if (gun_nkh > 0) {
			gun_nkh--;
		} else {
			if (back_gun(OFF,OFF) == 0) {
				init_kouho(0, ON);
				for (pos = gun_num; next_gun(OFF,OFF) != 0; pos += gun_num)
					;
				init_kouho(pos - 1, ON);
			} else {
				gun_nkh = gun_num - 1;
			}
		}
		break;
	case 21:
		if (gun_nkh < gun_num - 1) {
			gun_nkh++;
		} else {
			if (next_gun(OFF,OFF) == 0)
			{	init_kouho(0, ON);
			}
		}
		break;
	}
	get_kouhostr(kouho,kouzoku);
	if (error)
		return(0);
	return(gun_nkh + 1);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 22, 23 									*/
/*		�Լ�		int		api_moveblk( av )					*/
/*		���		���� �ĺ� ����� �ۼ�						*/
/*		�μ�		FNO = av[-1];			������ ������ 			*/
/*					= 22	���� �ĺ� ����� �ۼ�		 		*/
/*					= 23	���� �ĺ� ����� �ۼ�				*/
/*				KOUHO = av[0];		������ �ĺ��� �ݳ� ����	*/
/*				KOUZOKU = av[1];		�ļ� ������ �ݳ� ����	*/
/*		����		0   �Ǵ�  �ĺ� ��							*/
/*					= 0		�ĺ� ����						*/
/*					> 0		�ĺ� ��						*/
/*		�ؼ�		���ϰ��� �ĺ�����, ����� �ĺ� ��Ͽ� ���Ե� �ĺ���		*/
/*		����												*/
/*														*/
/************************************************************************/
int api_moveblk(APIARG av[])
{
	extern int gun_num, gun_nkh;
	int mode, error = 0;
	APIPTR kouho, kouzoku;

	if (func_mode != FP_SELECT)
		return(0);

	mode = (int)*(av - 1);
	kouho = (APIPTR)av[0];
	kouzoku = (APIPTR)av[1];

	switch (mode) {
	case 22:
		if (back_gun(OFF,OFF) == 0)
			error = 1;
		break;
	case 23:
		if (next_gun(OFF,OFF) == 0)
			error = 1;
		break;
	}
	get_kouhostr(kouho,kouzoku);
	if (error)
		return(0);
	return(gun_num);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 24 									*/
/*		�Լ�		int		api_allfix( av )						*/
/*		���		��ü Ȯ��									*/
/*		����	BUF = av[0];		��ȯ �� ���ڿ��� �ݳ� ���� 		*/
/*		����		0 �Ǵ� �����ڵ� 							*/
/*					= 0		���� ���� 					*/
/*					<> 0		����							*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
int api_allfix(APIARG av[])
{
	extern int kana_len;
	u_char *p;
	APIPTR buf;

	if (func_mode != FP_SELECT)
		return(API_FATAL);

	buf = *((APIPTR *)av);
	wd_learn(0,kana_len);
	p = select_tostr();
	strcpy((char*)buf,(char*)p);
	init_edit0();
	func_mode = FP_EDIT0;
	return(API_OK);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 25									*/
/*		�Լ�		int		api_select( av )					*/
/*		���		���ι����� Ȯ��							*/
/*		�μ�		NO = av[0];			�ĺ� ��� �������� ��ȣ	*/
/*				BUF = av[1];			Ȯ���� ���� ������ �ݳ� ����	*/
/*				KOUHO = av[2];		���� ������ ������ �ĺ� �ݳ� ����	*/
/*				KOUZOKU = av[3];		���� ������ �ļ� ���� �ݳ� ����	*/
/*				NKOUHO = av[4];		���� ������ �ĺ��� �ݳ� ����		*/
/*									���� ������ �ĺ� �� �ݳ� ����	*/
/*		���ϰ�	Ȯ���� ������ �б� ���� 					*/
/*					= 0		�ĺ� ��ȣ�� ����Ȯ �ϴ�.		*/
/*					> 0		Ȯ�� ������ �б� ���� (�ݰ� ȯ��)	*/
/*		�ؼ�		�ָ��ϰ� �ִ� ���� ������ Ȯ�� �Ѵ�. 			*/
/*		����												*/
/*														*/
/************************************************************************/
int api_select(APIARG av[])
{
	extern int kana_len, gun_num, gun_nkh;
	extern int cur_pos, cur_len;
	APIARG no;
	APIPTR buf, kouho, kouzoku, nkouho;
	u_char *p;
	int l;

	if (func_mode != FP_SELECT)
		return(API_FATAL);

	no = av[0];
	if (no <= 0 || no > gun_num)
		return(0);
	no--;
	buf = (APIPTR)av[1];
	kouho = (APIPTR)av[2];
	kouzoku = (APIPTR)av[3];
	nkouho = (APIPTR)av[4];

	gun_nkh = (int)no;
	p = select_subtostr(cur_pos,cur_len);
	strcpy((char*)buf,(char*)p);
	unify_khmem(cur_pos,ON);
	l = count_byte_kana_buf(cur_pos,cur_len);
	cur_pos += cur_len;
	if (cur_pos >= kana_len) {
		*kouho = '\0';
		*kouzoku = '\0';
		*(APISHORTPTR)nkouho = 0;
		return(l);
	}

	cur_len = bunsetu_len(cur_pos);
	init_kouho(0,ON);
	get_kouhostr(kouho,kouzoku);
	*(APISHORTPTR)nkouho = (short)kh_count(hchar[cur_pos].kh_mem);
	return(l);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 28									*/
/*		�Լ�		int		api_dicopen()						*/
/*		���		������ ����								*/
/*		�μ�		����										*/
/*		����		0 �Ǵ� �����ڵ�							*/
/*					= 0		��������						*/
/*					= 1		����̺��� �غ� �Ǿ� ���� ���� 	*/
/*					> 1		�� ���� ����.					*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
APIRET	api_dicopen(APIARG *argv)	//int api_dicopen(void)
{
	extern u_char dic_name[];

	if (lock_mode == OFF)
		return(API_FATAL);
	if (dic_open((char*)dic_name) == DIC_NODRIVE)
		return(1);

	into_editing(OFF);
	func_mode = FP_EDIT0;
	return(API_OK);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 29									*/
/*		�Լ�		int		api_dicclose()						*/
/*		���		������ Ŭ�ο���							*/
/*		�μ�		����										*/
/*		����		0 �Ǵ� �����ڵ� 							*/
/*					= 0		���� ����						*/
/*					<> 0		����							*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
APIRET	api_dicclose(APIARG *argv)	//int api_dicclose(void)
{
	if (lock_mode == OFF)
		return(API_FATAL);

	init_edit0();
	dic_close();
	func_mode = FP_NOKANJI;
	return(API_OK);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ :	30, 31								*/
/*		�Լ�		int		api_touroku( av )					*/
/*		���		�������� �ܾ� ���, ����					*/
/*		�μ�		FNO = av[-1]		��� �Ǵ� ����				*/
/*					= 30		���							*/
/*					= 31		����							*/
/*				FD = av[0];	������ ����					*/
/*					= 0		���� ����						*/
/*					= 1		���� ����						*/
/*				YOMI = av[1];	����� �ܾ��� �б�				*/
/*				TANGO = av[2];	����� �ܾ��� ���ڿ�	 	*/
/*				SYN = av[3];	���� �ڵ�						*/
/*		����		0 �Ǵ� �����ڵ�							*/
/*					= 0		���� ����						*/
/*					= 1		������ ������ ������			*/
/*					= 2		���� �ڵ尡 ������				*/
/*					= 3		�б� ����� ������				*/
/*					= 4		����� �������� ����.			*/
/*		�ؼ�												*/
/*		����		���� ������ �����Ǿ��� ������ �ƹ��͵� ����.	*/
/*														*/
/************************************************************************/
int api_touroku(APIARG av[])
{
	extern u_char yomi_buf[];
	int fno;
	APIARG fd, syn;
	APIPTR yomi, tango;
	BIT16 code;
	u_char *p, uc;
	int l, sts;
	NEWWD newwd;

	if (im_state >= IM_EDITING || func_mode >= FP_EDITING)
		return(4);
	fno = (int)*(av - 1);

	fd = av[0];
	if (fd != 1 && fd != 0)			/* ��� ���̶� ���� ����.  */
		return(1);
	yomi = (APIPTR)av[1];
	tango = (APIPTR)av[2];
	syn = av[3];

	if ((syn = syn_2to3((int)syn)) < 0)
		return(2);

	p = yomi_buf;
	l = 0;
	while (*yomi) {
		code = (BIT16)((*yomi << 8) | *(yomi + 1));
		if (!is_shift(code))
			return(3);
		code = ask_sjis2jis(code);
		uc = to_ucode(code);
		if (!uc)
			return(3);
		*p++ = uc;
		l++;
		yomi += 2;
	}
	*p = '\0';

	newwd.ustr = yomi_buf;
	newwd.ulen = (short)l;
	newwd.level = 0;
	newwd.syntax = (u_char)syn;
	newwd.syntaxinfo.katuyo = 0;
	newwd.kbuf = tango;
	if (fno == 30) {
		if (dic_touroku(&newwd) == DIC_OK)
			return(API_OK);
	} else if (fno == 31) {
		if ((sts = dic_delete(&newwd)) == DIC_OK)
			return(API_OK);
		else if (sts == DIC_HASNONE)
			return(-1);
	}
	return(4);
}

static u_char tab_2to3[] = {
		1,		/* ���� ī�� 5�� */
		2,		/* ���� ���� 5�� */
		3,		/* ���� ���� 5�� */
		4,		/* ���� Ÿ�� 5�� */
		5,		/*  */
		6,		/*  */
		7,		/*  */
		8,		/*  */
		9,		/*  */
		10,		/* ���� ���� ���� */
		12,		/* ���� ī�� ���� */
		13,		/* ���� ����1�� */
		14,		/* ����� */
		19,		/* ���뵿�� */
		20,		/* ���뵿�� ���� ���  */
		22,		/* ���� ���� ���� ���  */
		24,		/* ��� */
		40,		/* �ܹ� ���� */
		27,		/* �θ�(����) */
		28,		/* �θ�(�̸�) */
		26,		/* ���� */
		29,		/* ��ü�� */
		30,		/* �繰�� ��Ī */
		31,		/* ���� */
		31,		/* ���� */
		50,		/* ���̾� */
		32,		/* ������ - ��ź�� */
		33,		/* ���ӻ� */
		34,		/* �λ� */
		38		/* ��ü�� */
};

int syn_2to3(int syn2)
{
	syn2--;
	if (syn2 < 0 || syn2 >= (int)(sizeof(tab_2to3) / sizeof(u_char)))
		return(-1);
	return((int)tab_2to3[syn2]);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 32									*/
/*		�Լ�		int		apis_dicname( av )					*/
/*		���		���� ���ϸ��� ���							*/
/*		�μ�		MAINDIC = av[0];		���� ���� ��			*/
/*				SUBDIC = av[1];		���� ���� ��			*/
/*		����		0 �Ǵ� �����ڵ� 							*/
/*					= 0		���� ����						*/
/*					= 1		����̺��� �غ� �ȵǾ���.		*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
int apis_dicname(APIARG av[])
{
	extern u_char dic_name[];
	APIPTR maindic;

	maindic = *((APIPTR *)av);
	strncpy((char*)dic_name,(char*)maindic,128);

	return(API_OK);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 33									*/
/*		�Լ�		int		api_khlong( av )					*/
/*		���		���ι����� 1���� ��� �ؼ� �纯ȯ			*/
/*		�μ�		KOUHO = av[0];		������ �ĺ��� �ݳ� ����	*/
/*				KOUZOKU = av[1];		�ļ� ������ �ݳ� ����	*/
/*		����		�ĺ� ��									*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
int api_khlong(APIARG av[])
{
	extern HCHAR hchar[];
	extern int kana_len, cur_pos, cur_len;
	APIPTR kouho, kouzoku;

	if (func_mode != FP_SELECT)
		return(API_FATAL);

	kouho = (APIPTR)av[0];
	kouzoku = (APIPTR)av[1];

	if (cur_pos + cur_len >= kana_len)
		return(0);

	save_fst_bslen(cur_pos);
	free_hchar(cur_pos,kana_len,HALF_FREE);
	cur_len++;
	henkan(cur_pos,kana_len,FIXED_HENKAN,cur_len);
	init_kouho(0,ON);
	get_kouhostr(kouho,kouzoku);
	return(kh_count(hchar[cur_pos].kh_mem));
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 34									*/
/*		�Լ�		int		api_khshort( av )					*/
/*		���		���� ������ 1���� ª�� �Ͽ� ��˻�			*/
/*		�μ�		KOUHO = av[0];		������ �ĺ��� �ݳ� ����	*/
/*				KOUZOKU = av[1];		�ļ� ������ �ݳ� ����	*/
/*		����		�ĺ���									*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
int api_khshort(APIARG av[])
{
	extern HCHAR hchar[];
	extern int kana_len, cur_pos, cur_len;
	APIPTR kouho, kouzoku;

	if (func_mode != FP_SELECT)
		return(API_FATAL);

	kouho = (APIPTR)av[0];
	kouzoku = (APIPTR)av[1];

	if (cur_len <= 1)
		return(0);

	save_fst_bslen(cur_pos);
	free_hchar(cur_pos,kana_len,HALF_FREE);
	cur_len--;
	henkan(cur_pos,kana_len,FIXED_HENKAN,cur_len);
	init_kouho(0,ON);
	get_kouhostr(kouho,kouzoku);
	return(kh_count(hchar[cur_pos].kh_mem));
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 35									*/
/*		�Լ�		int		api_backbunsetu( av )				*/
/*		���		�� �������� �̵�							*/
/*		�μ�		KOUHO = av[0];			������ �ĺ��� �ݳ� ����		*/
/*				KOUZOKU = av[1];		�ļ� ������ �ݳ� ����	*/
/*		����		0 �Ǵ� �ĺ� �� 							*/
/*					= 0			�� ������ ����.			*/
/*					> 0			�� ������ �ĺ� ��			*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
int api_backbunsetu(APIARG av[])
{
	extern HCHAR hchar[];
	extern int kana_len, cur_pos, cur_len;
	APIPTR kouho, kouzoku;
	int pos, l;

	l = 0;
	if (func_mode != FP_SELECT)
		return(API_FATAL);

	kouho = (APIPTR)av[0];
	kouzoku = (APIPTR)av[1];

	if (cur_pos == 0)
		return(0);

	unify_khmem(cur_pos,OFF);
	for (pos = 0; pos < cur_pos; pos += l) {
		l = bunsetu_len(pos);
		if (pos + l >= cur_pos)
			break;
	}
	cur_pos = pos;
	cur_len = l;

	init_kouho(0,ON);
	get_kouhostr(kouho,kouzoku);
	return(kh_count(hchar[cur_pos].kh_mem));
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 36									*/
/*		�Լ�		int		api_nextbunsetu( av )				*/
/*		���		���� �������� �̵�							*/
/*		�μ�		KOUHO = av[0];		������ �ĺ��� �ݳ� ����		*/
/*				KOUZOKU = av[1];		�ļ� ������ �ݳ� ����		*/
/*		����		0 �Ǵ� �ĺ� ��								*/
/*					= 0		���� ������ ����.				*/
/*					> 0		���� ������ �ĺ� ��				*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
int api_nextbunsetu(APIARG av[])
{
	extern HCHAR hchar[];
	extern int kana_len, cur_pos, cur_len;
	APIPTR kouho, kouzoku;

	if (func_mode != FP_SELECT)
		return(API_FATAL);

	kouho = (APIPTR)av[0];
	kouzoku = (APIPTR)av[1];

	if (cur_pos + cur_len >= kana_len)
		return(0);

	unify_khmem(cur_pos,OFF);
	cur_pos += cur_len;
	cur_len = bunsetu_len(cur_pos);

	init_kouho(0,ON);
	get_kouhostr(kouho,kouzoku);
	return(kh_count(hchar[cur_pos].kh_mem));
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ : 37, 38, 39, 40							*/
/*		�Լ�		int		api_khhenkan( av )					*/
/*		���		���� ������ ������ ��ȯ�� �Ѵ�.				*/
/*		�μ�		FNO = av[-1];		��ȯ ��� 				*/
/*					= 37		ī��ī��						*/
/*					= 38		���󰡳�						*/
/*					= 39		�ݰ�							*/
/*					= 40		����							*/
/*				KOUHO = av[2];		�ĺ��� �ݳ� ����		*/
/*				KOUZOKU = av[3];		�ļ� ������ �ݳ� ����	*/
/*		����		0 �Ǵ� �����ڵ� 							*/
/*					= 0		���� ����						*/
/*					<> 0		����							*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
int api_khhenkan(APIARG av[])
{
	extern HCHAR hchar[];
	extern int kana_len, cur_pos, cur_len;
	int fno;
	APIPTR kouho, kouzoku;
	int type;

	if (func_mode != FP_SELECT)
		return(API_FATAL);

	fno = (int)*(av - 1);
	kouho = (APIPTR)av[0];
	kouzoku = (APIPTR)av[1];

	switch (fno) {
	case 37:			/* īŸī�� */
		type = CT_KATA;
		break;
	case 38:			/* ���󰡳� */
		type = CT_HIRA;
		break;
	case 39:			/* �ݰ� */
		type = CT_HANKAKU;
		break;
	case 40:			/* ���� */
		type = CT_ZENKAKU;
		break;
	default:			/* ����  */
		type = CT_ZENKAKU;
		break;
	}

	khmem_raw(type);
	if (hchar[cur_pos].kh_mem == NULL)
		return(API_FATAL);
	init_kouho(0, ON);
	get_kouhostr(kouho,kouzoku);

	return(API_OK);
}


/************************************************************************/
/*														*/
/*		FP �� ��ȣ :  ??									*/
/*		�Լ�		int		api_none()						*/
/*		���		�߸��� FP ��								*/
/*		�μ�		����										*/
/*		����		�����ڵ� - 1								*/
/*		�ؼ�												*/
/*		����												*/
/*														*/
/************************************************************************/
APIRET api_none(APIARG *argv)		//int api_none(APIARG *argv)
{
	return(API_FNOERR);
}

