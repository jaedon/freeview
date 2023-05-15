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
**		Function Pointer 함수 ($FF22)  for X68000
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


/*** API 리턴 값 ***/
#define API_FNOERR		(-1)
#define API_FATAL		(-1)
#define API_OK			(0)
#define API_MODEERR		(2)

/*** FP 변환 모드의 상태 ***/
#define FP_NOKANJI		0
#define FP_EDIT0		1
#define FP_EDITING		2
#define FP_SELECT		3

extern u_char	dic_name[];

int		func_mode = FP_NOKANJI; /* FP 콜에 의해 변환을 실행하는 모드.  */
int		flush_mode = OFF;		/* FP키 버퍼도 플래쉬 한다.	*/
int		lock_mode = ON;			/* 키 입력에 의한 가나 한자를 불가능하게 함.	*/
int		echo_lock = OFF;		/* 에코모드의, 키에 의한 변경의 허가  */

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
/*		FP콜 메인 엔트리 									*/
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
/*		FP 콜 번호 : 15, 16 									*/
/*		함수		int		api_Rstrconv( av )					*/
/*		기능		문자열의 변환 (로마자 변환)					*/
/*		인수		FNO = av[-1];			변환 방법 			*/
/*						= 15	반각 --> 전각 카타카나 (로마자 변환) 	*/
/*						= 16	반각 --> 전각 히라가나 (로마자 변환)	*/
/*				SRC = av[0];			변환전 문자열의 포인터		*/
/*				DST = av[1];			변환결과 격납 장소의 포인터	*/
/*		리턴		0 또는 에러의 원인이 된 문자의 위치 	 		*/
/*					= 0		정상종료						*/
/*					> 0		변환에러가 생긴 문자의 버퍼의,	*/
/*							선두로 부터의 문자 수 (반각환산)	*/
/*		해설												*/
/*		주의		변환전후의 문자열의 길이는 79바이트 이하.		*/
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

	if (mode == 15)		/* 반각 --> 전각 카타카나 (로마자 변환)	*/
		kana_mask = (IC_KATAKANA | IC_ZENKAKU);
	else					/* 반각 --> 전각 히라가나 (로마자 변환)	*/
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
				/* +1 하지 않으면, 선두문자에서 에러의 경우, api_ok와 구별할 수 없다. 	*/
				/* 이로 인해 몇 문자째에서 에러인지를 리턴하게 된다. 				*/
				/* 원래의 사양은 에러일 때는 > 0 이 되는 것이므로 좋은 해답.			*/
				return((int)(p - (BIT16 *)wdsbuf) + 1);
#endif
			nt = 1;
		}
		p += nt;
		l -= nt;
		for (q = rtmp; rlen--; ) {
			code = ask_jis2sjis(*q++);
			if (code == 0x82F2)		/* V 발음- 히라가나 */
				code = 0x8394;		/* V 발음- 카타카나 */
			*dst++ = (u_char)(code >> 8);
			*dst++ = (u_char)code;
		}
	}
	*dst++ = '\0';
	return(API_OK);
}


/************************************************************************/
/*														*/
/*		FP 콜 번호 : 19									*/
/*		함수		int		api_henkan( av )					*/
/*		기능		일괄, 순차, 미리읽기 변환의, 최초의 후보군과 후속 문절의 작성.	*/
/*		인수		BUF = av[0];		변환할 전각 히라가나 문자열	*/
/*				KOUHO = av[1];	최초의 후보의 격납 버퍼		*/
/*				KOUZOKU = av[2];	후속 문절의 격납 버퍼		*/
/*		리턴		후보수 또는 에러 코드. 						*/
/*					>= 0		후보 수						*/
/*					< 0		에러							*/
/*		해설		BUF의 값이 0일 때, 순차 미리읽기라고 해석되어,	*/
/*				이미 보내져 있는 문자열을 변한한다.		 	*/
/*		주의												*/
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

	cur_pos = 0;	/* 이후의 각 함수에서, CUR_POS 를 참조 한다.  */
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
/*		FP 콜 번호 : 20, 21									*/
/*		함수		int		api_movekh( av )					*/
/*		기능		후보군 블록 내의 전후 후보를 취득한다.			*/
/*		인수		FNO = av[-1];			앞인지 뒤 인지 		*/
/*					= 20	앞 후보의 취득						*/
/*					= 21	뒷 후보의 취득						*/
/*				KOUHO = av[0];		후보의 격납 버퍼		*/
/*				KOUZOKU = av[1];		후속 문절의 격납 버퍼 	*/
/*		리턴		0 또는 후보 번호							*/
/*					= 0		후보 없음						*/
/*					> 0		후보 번호						*/
/*		해설		후보번호는, 현재의 후보군 안에서의 번호		*/
/*		주의												*/
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
/*		FP 콜 번호 : 22, 23 									*/
/*		함수		int		api_moveblk( av )					*/
/*		기능		전후 후보 블록의 작성						*/
/*		인수		FNO = av[-1];			앞인지 뒤인지 			*/
/*					= 22	앞쪽 후보 블록의 작성		 		*/
/*					= 23	뒷쪽 후보 블록의 작성				*/
/*				KOUHO = av[0];		최초의 후보의 격납 버퍼	*/
/*				KOUZOKU = av[1];		후속 문절의 격납 버퍼	*/
/*		리턴		0   또는  후보 수							*/
/*					= 0		후보 없음						*/
/*					> 0		후보 수						*/
/*		해설		리턴값의 후보수는, 취득한 후보 블록에 포함된 후보수		*/
/*		주의												*/
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
/*		FP 콜 번호 : 24 									*/
/*		함수		int		api_allfix( av )						*/
/*		기능		전체 확정									*/
/*		덙릶	BUF = av[0];		변환 후 문자열의 격납 버퍼 		*/
/*		리턴		0 또는 에러코드 							*/
/*					= 0		정상 종료 					*/
/*					<> 0		에러							*/
/*		해설												*/
/*		주의												*/
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
/*		FP 콜 번호 : 25									*/
/*		함수		int		api_select( av )					*/
/*		기능		선두문절의 확정							*/
/*		인수		NO = av[0];			후보 블록 내에서의 번호	*/
/*				BUF = av[1];			확정할 선두 문절의 격납 버퍼	*/
/*				KOUHO = av[2];		다음 문절의 최초의 후보 격납 버퍼	*/
/*				KOUZOKU = av[3];		다음 문절의 후속 문절 격납 버퍼	*/
/*				NKOUHO = av[4];		다음 문절의 후보수 격납 버퍼		*/
/*									다음 문절의 후보 수 격납 버퍼	*/
/*		리턴값	확정한 문절의 읽기 길이 					*/
/*					= 0		후보 변호가 부정확 하다.		*/
/*					> 0		확정 문절의 읽기 길이 (반각 환산)	*/
/*		해설		주목하고 있던 문절 까지를 확정 한다. 			*/
/*		주의												*/
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
/*		FP 콜 번호 : 28									*/
/*		함수		int		api_dicopen()						*/
/*		기능		사전의 오픈								*/
/*		인수		없음										*/
/*		리턴		0 또는 에러코드							*/
/*					= 0		정상종료						*/
/*					= 1		드라이브의 준비가 되어 있지 않음 	*/
/*					> 1		그 외의 에러.					*/
/*		해설												*/
/*		주의												*/
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
/*		FP 콜 번호 : 29									*/
/*		함수		int		api_dicclose()						*/
/*		기능		사전의 클로우즈							*/
/*		인수		없음										*/
/*		리턴		0 또는 에러코드 							*/
/*					= 0		정상 종료						*/
/*					<> 0		에러							*/
/*		해설												*/
/*		주의												*/
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
/*		FP 콜 번호 :	30, 31								*/
/*		함수		int		api_touroku( av )					*/
/*		기능		사전에의 단어 등록, 삭제					*/
/*		인수		FNO = av[-1]		등록 또는 삭제				*/
/*					= 30		등록							*/
/*					= 31		삭제							*/
/*				FD = av[0];	사전의 종류					*/
/*					= 0		메인 사전						*/
/*					= 1		서브 사전						*/
/*				YOMI = av[1];	등록할 단어의 읽기				*/
/*				TANGO = av[2];	등록할 단어의 문자열	 	*/
/*				SYN = av[3];	문법 코드						*/
/*		리턴		0 또는 에러코드							*/
/*					= 0		정상 종료						*/
/*					= 1		사전의 종류가 부적합			*/
/*					= 2		문법 코드가 부적당				*/
/*					= 3		읽기 방법이 부적당				*/
/*					= 4		등록할 페이지가 없음.			*/
/*		해설												*/
/*		주의		서브 사전이 지정되었을 때에는 아무것도 안함.	*/
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
	if (fd != 1 && fd != 0)			/* 어느 것이라도 메인 사전.  */
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
		1,		/* 동사 카행 5단 */
		2,		/* 동사 가행 5단 */
		3,		/* 동사 사행 5단 */
		4,		/* 동사 타행 5단 */
		5,		/*  */
		6,		/*  */
		7,		/*  */
		8,		/*  */
		9,		/*  */
		10,		/* 동사 사행 변격 */
		12,		/* 동사 카행 변격 */
		13,		/* 동사 상하1단 */
		14,		/* 형용사 */
		19,		/* 형용동사 */
		20,		/* 형용동사 복합 명사  */
		22,		/* 사행 변격 복합 명사  */
		24,		/* 명사 */
		40,		/* 단문 한자 */
		27,		/* 인명(성씨) */
		28,		/* 인명(이름) */
		26,		/* 지명 */
		29,		/* 단체명 */
		30,		/* 사물의 명칭 */
		31,		/* 수사 */
		31,		/* 숫자 */
		50,		/* 접미어 */
		32,		/* 감동사 - 감탄사 */
		33,		/* 접속사 */
		34,		/* 부사 */
		38		/* 연체사 */
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
/*		FP 콜 번호 : 32									*/
/*		함수		int		apis_dicname( av )					*/
/*		기능		사전 파일명의 등록							*/
/*		인수		MAINDIC = av[0];		메인 사전 명			*/
/*				SUBDIC = av[1];		서브 사전 명			*/
/*		리턴		0 또는 에러코드 							*/
/*					= 0		정상 종료						*/
/*					= 1		드라이브의 준비가 안되었다.		*/
/*		해설												*/
/*		주의												*/
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
/*		FP 콜 번호 : 33									*/
/*		함수		int		api_khlong( av )					*/
/*		기능		선두문절을 1문자 길게 해서 재변환			*/
/*		인수		KOUHO = av[0];		최초의 후보의 격납 버퍼	*/
/*				KOUZOKU = av[1];		후속 문절의 격납 버퍼	*/
/*		리턴		후보 수									*/
/*		해설												*/
/*		주의												*/
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
/*		FP 콜 번호 : 34									*/
/*		함수		int		api_khshort( av )					*/
/*		기능		선두 문절을 1문자 짧게 하여 재검색			*/
/*		인수		KOUHO = av[0];		최초의 후보의 격납 버퍼	*/
/*				KOUZOKU = av[1];		후속 문절의 격납 버퍼	*/
/*		리턴		후보수									*/
/*		해설												*/
/*		주의												*/
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
/*		FP 콜 번호 : 35									*/
/*		함수		int		api_backbunsetu( av )				*/
/*		기능		앞 문절에의 이동							*/
/*		인수		KOUHO = av[0];			최초의 후보의 격납 버퍼		*/
/*				KOUZOKU = av[1];		후속 문절의 격납 버퍼	*/
/*		리턴		0 또는 후보 수 							*/
/*					= 0			앞 문절이 없다.			*/
/*					> 0			앞 문절의 후보 수			*/
/*		해설												*/
/*		주의												*/
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
/*		FP 콜 번호 : 36									*/
/*		함수		int		api_nextbunsetu( av )				*/
/*		기능		다음 문절에의 이동							*/
/*		인수		KOUHO = av[0];		최초의 후보의 격납 버퍼		*/
/*				KOUZOKU = av[1];		후속 문절의 격납 버퍼		*/
/*		리턴		0 또는 후보 수								*/
/*					= 0		다음 문절이 없다.				*/
/*					> 0		다음 문절의 후보 수				*/
/*		해설												*/
/*		주의												*/
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
/*		FP 콜 번호 : 37, 38, 39, 40							*/
/*		함수		int		api_khhenkan( av )					*/
/*		기능		선두 문절의 문자종 변환을 한다.				*/
/*		인수		FNO = av[-1];		변환 방법 				*/
/*					= 37		카나카나						*/
/*					= 38		히라가나						*/
/*					= 39		반각							*/
/*					= 40		전각							*/
/*				KOUHO = av[2];		후보의 격납 버퍼		*/
/*				KOUZOKU = av[3];		후속 문절의 격납 버퍼	*/
/*		리턴		0 또는 에러코드 							*/
/*					= 0		정상 종료						*/
/*					<> 0		에러							*/
/*		해설												*/
/*		주의												*/
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
	case 37:			/* 카타카나 */
		type = CT_KATA;
		break;
	case 38:			/* 히라가나 */
		type = CT_HIRA;
		break;
	case 39:			/* 반각 */
		type = CT_HANKAKU;
		break;
	case 40:			/* 전각 */
		type = CT_ZENKAKU;
		break;
	default:			/* 전각  */
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
/*		FP 콜 번호 :  ??									*/
/*		함수		int		api_none()						*/
/*		기능		잘못된 FP 콜								*/
/*		인수		없음										*/
/*		리턴		에러코드 - 1								*/
/*		해설												*/
/*		주의												*/
/*														*/
/************************************************************************/
APIRET api_none(APIARG *argv)		//int api_none(APIARG *argv)
{
	return(API_FNOERR);
}

