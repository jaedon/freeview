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
	kouho.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
**		후보 관리 부
*/


#include	<string.h>

#include	"askdef.h"
#include	"mean.h"
#include	"pword.h"
#include	"hwork.h"
#include	"dic.h"
#include	"specsyn.h"
#include	"kouho.h"
#include	"dictypes.h"
#include	"dicdef.h"
#include	"dic_main.h"
#include	"aci.h"
#include	"apiroman.h"
#include	"memory.h"
#include	"prefer.h"

static KHMEM	*kh_merge_getone(KLMEM	*klist);
static void		kh_append_init(int pos, register KHMEM	*kh);
static void		kh_append(int pos, KHMEM **pkhs, KHMEM	**pkhe, KHMEM	*kh);
static int		exist_kouho(u_char *str, int len);

/* ??? set_num() 함수 명이 dic_main.h 과 중복되므로 kouho.c에 있는 것은 kouho_set_num() 로 변경 */
static void		kouho_set_num(int num, register u_char *s);


extern HCHAR	hchar[];
extern u_char	kana_ustr[];

u_char	wdsbuf[1024];

/* 최초의 후보를 취득한다. */
void first_kouho(int pos, int len)
{
	HCHAR *hp;
	register BSMEM *b;
	KHMEM *ks, *ke;
	BSMEM *top = NULL;
	PRIOR maxprior = 0, t;
	CHMEM *c;

	hp = &hchar[pos];
	if (hp->kh_mem != NULL)
		return;
	if ((b = hp->bs_mem) == (BSMEM *)-1) {
		rmkouho:
		hp->kh_mem = null_kouho(len);
		return;
	}
	for ( ; b; b = b->bs_next) {
		if (b->bs_len != len)
			continue;
		t = (u_short)((b->wd_mem == NULL) ? 0 : kh_priority(b,b->wd_mem->maxprior));
		if (t > maxprior || top == NULL) {
			maxprior = t;
			top = b;
		}
	}
	if (top == NULL)
		goto rmkouho;

	if ((c = top->wd_mem) == NULL || c->yomilen == 0)
		goto rmkouho;

	if (c->entid == -1) {
		if (c->ch_syn != SYN_SUUJI || dic_get1num(&kana_ustr[pos],
				c->yomilen,(WDS *)wdsbuf) <= 0)
			goto rmkouho;
	} else if (dic_get1wd(c->entid,c->ch_syn,c->ch_ktu,(WDS *)wdsbuf) <= 0)
		goto rmkouho;

	hp->kh_mem = create_kouho((WDS *)wdsbuf,c,c->yomilen,&ks,&ke);
}

extern int		cur_pos, cur_len;

int		kwin_len;				/* 후보 윈도우의 길이 */
KHMEM	*top_kh;				/* 현재의 후보군의 선두 */
int		gun_num;				/* 현재 후보군의 후보 수 */
int		gun_nkh;				/* 현재의 후보의 순번 */

/* 모든 후보를 표시하기 위한 초기화 (최초의 표시)  */
void init_kouho(int kh_pos, int mode)
{
	extern int func_mode;
	KHMEM *kh;
	int n;

	if (mode == ON)
		all_kouho();
	kh = hchar[cur_pos].kh_mem;
	if (func_mode > 0)
		kwin_len = 80;
	else
		kwin_len = kwin_length();
	n = inc_gun(kh);
	while (kh_pos >= n) {
		kh_pos -= n;
		while (n--)
			kh = kh_followed(kh);
		if ((n = inc_gun(kh)) == 0) {
			init_kouho(0,OFF);
			goto disp;
		}
	}
	top_kh = kh;
	gun_nkh = kh_pos;
	gun_num = n;
disp:
	if (mode == ON && func_mode == 0)
		disp_kouho();
}

/* 모든 후보를 취득한다. */
void all_kouho(void)
{
	HCHAR *hp;
	BSMEM *b;
	KHMEM *kh;
	KLMEM *kl, *kls, *kle;

	hp = &hchar[cur_pos];
	if ((b = hp->bs_mem) == (BSMEM *)-1)
		return;

	kle = kls = NULL;
	for ( ; b; b = b->bs_next) {
		if (b->bs_len != cur_len)
			continue;
		if ((kl = alloc_klmem()) == NULL) {
			free_kouholists(kls);
			kls = NULL;
			break;
		}
		kh = get_kouholist(b);
		kl->khlist = kh;
		kl->bnst = b;
		kl->maxpri = (u_short)((kh == NULL) ? 0 : kh_priority(b,kh->kh_rtime));
		kl->kl_next = NULL;
		if (kls == NULL)
			kls = kle = kl;
		else {
			kle->kl_next = kl;
			kle = kl;
		}
	}
	kh_mergesort(cur_pos,kls);
	free_klmemlist(kls);
}

/* 하나의 문절에 대응하는 후보 리스트의 취득 */
KHMEM *get_kouholist(BSMEM *bsm)
{
	CHMEM *c;
	int wdn;
	KHMEM *kstart, *kend, *ks, *ke;
	register u_char *p;

	c = bsm->wd_mem;
	if (c == NULL || c->yomilen == 0) {
		rmkouho:
		kstart = null_kouho(bsm->bs_len);
		return(kstart);
	}

	if (c->entid == -1) {
		if (c->ch_syn != SYN_SUUJI ||
				dic_getallnum((UCODE*)&kana_ustr[cur_pos],
				(ULEN)c->yomilen,(WDS *)wdsbuf,&wdn) <= 0)
			goto rmkouho;
	} else if (dic_getallwd(c->entid,c->ch_syn,c->ch_ktu,
								(WDS *)wdsbuf, &wdn) <= 0)
		goto rmkouho;

		kstart = kend = NULL;
		for (p = wdsbuf; wdn > 0; wdn--) {
			if (create_kouho((WDS *)p,c,c->yomilen,&ks,&ke) == NULL) {
				free_khmemlist(kstart);
				return(NULL);
		}
		if (kstart == NULL) {
			kstart = ks;
			kend = ke;
		} else {
			kend->kh_next = ks;
			kend = ke;
		}
		for (p = ((WDS *)p)->kbuf; *p++; )
			;
		if (((int)p) & 1)
			p++;
	}
	return(kstart);
}

/* 후보 리스트의 해방  */
void free_kouholists(register KLMEM	*kls)
{
	while (kls) {
		free_khmem(kls->khlist);
		kls = kls->kl_next;
	}
}

/* 입력 그대로의 후보 구조체의 작성 */
KHMEM *null_kouho(int ylen)
{
	KHMEM *k;

	if ((k = alloc_khmem()) != NULL) {
		k->khtype = KH_ROMAN;
		k->khbuf[0] = '\0';
		k->chlen = (u_char)ylen;
		k->level = 0;
		k->wd_mem = NULL;
		k->wdid = (u_short)-1;
		k->kh_next = NULL;
	}
	return(k);
}

/* 일반적인 후보 구조체의 작성 */
KHMEM *create_kouho(WDS *wdsp, CHMEM *chm, int ylen, KHMEM **pkhs, KHMEM **pkhe)
{
	register char *p;
	int l, d;
	KHMEM *k;

	p = (char*)wdsp->kbuf;
	l = strlen(p);
	if ((k = alloc_khmem()) == NULL)
		return(NULL);
	k->khtype = KH_NORMAL;
	d = kstrncpy((u_char *)k->khbuf,(u_char *)p,KHLEN);
	k->chlen = (u_char)ylen;
	k->level = wdsp->level;
	k->wd_mem = chm;
	k->wdid = wdsp->wdid;
	k->kh_rtime = wdsp->rtime;
	k->kh_next = NULL;
	*pkhs = k;
	l -= d;
	p += d;
	while (l > 0) {
		KHMEM *kt;

		if ((kt = alloc_khmem()) == NULL) {
			free_khmemlist(*pkhs);
			return(NULL);
		}
		k->khtype |= KH_CONTINUE;
		kt->khtype = KH_SECOND;
		d = kstrncpy((u_char*)kt->khbuf,(u_char*)p,KHLENX);
		kt->kh_next = NULL;
		k->kh_next = kt;
		l -= d;
		p += d;
		k = kt;
	}
	*pkhe = k;
	return(*pkhs);
}


int kstrncpy(register u_char *d, register u_char *s, int n)
{
	int l = n;

	while (*s && n > 0) {
		if (is_kanji(*s)) {
			if (n <= 1)
				break;
			*d++ = *s++;
			n--;
		}
		*d++ = *s++;
		n--;
	}
	*d = '\0';
	return(l - n);
}

/* 주어진 문자 종류의 후보구조체의 작성 */
KHMEM *raw_kouho(int spos, int ylen, int type)
{
	register WDS *w;
	KHMEM *dummy1, *dummy2;

	w = (WDS *)wdsbuf;
	w->wdid = (u_short)-1;
	w->rtime = 0;
	w->level = 0;
	trans_roman(w->kbuf,spos,ylen,type);
	return(create_kouho(w,NULL,ylen,&dummy1,&dummy2));
}

void khmem_raw(int type)
{
	HCHAR *hp;

	hp = &hchar[cur_pos];
	free_khmemlist(hp->kh_mem);
	hp->kh_mem = raw_kouho(cur_pos,cur_len,type);
}

/************************************************/
/*		문절 종류를 머지 하고 후보를 정렬한다. 	*/
/************************************************/

static u_char *e_khstr;		/* 후보 문자 면의 최후 */

void kh_mergesort(int pos, KLMEM *klist)
{
	KHMEM *kh, *khs, *khe;
	HCHAR *hp;

	hp = &hchar[pos];
	khs = hp->kh_mem;
	khe = kh_endof(khs);
	kh_append_init(pos,khs);
	while ((kh = kh_merge_getone(klist)) != NULL) {
		kh_append(pos,&khs,&khe,kh);
	}
	if ((kh = null_kouho(cur_len)) != NULL)
		kh_append(pos,&khs,&khe,kh);
	hp->kh_mem = khs;
}

static KHMEM *kh_merge_getone(KLMEM	*klist)
{
	register KLMEM *kl;
	KLMEM *bestkl = NULL;
	KHMEM *kh;
	PRIOR max;

	max = 0;
	/* 검색 */
	for (kl = klist; kl; kl = kl->kl_next) {
		if (kl->khlist == NULL)
			continue;
		if (bestkl == NULL || kl->maxpri > max) {
			bestkl = kl;
			max = kl->maxpri;
		}
	}
	if (bestkl == NULL)
		return(NULL);
	/* 갱신 */
	kh = bestkl->khlist;
	bestkl->khlist = kh_skip(kh);
	bestkl->maxpri = (u_short)((bestkl->khlist == NULL) ? 0
					: kh_priority(bestkl->bnst, bestkl->khlist->kh_rtime));
	return(kh);
}

static void kh_append_init(int pos, register KHMEM *kh)
{
	extern MEAN meanbuf[];
	int l;

	e_khstr = wdsbuf;
	while (kh != NULL) {
		l = kouho_makedisp(pos,cur_len,kh,meanbuf);
		l = meantosjis(meanbuf,(char*)(e_khstr + 1),l);
		*e_khstr++ = (u_char)l;
		e_khstr += l;
		kh = kh_followed(kh);
	}
}

static void kh_append(int pos, KHMEM **pkhs, KHMEM **pkhe, KHMEM *kh)
{
	extern MEAN meanbuf[];
	extern u_char outbuf[];
	int l;

	l = kouho_makedisp(pos,cur_len,kh,meanbuf);
	l = meantosjis(meanbuf,(char*)outbuf,l);
	if (exist_kouho(outbuf,l)) {
		free_khmemlist(kh);
		return;
	}

	if (e_khstr + l + 1 <= wdsbuf + sizeof(wdsbuf)) {
		*e_khstr++ = (u_char)l;
		strncpy((char*)e_khstr,(char*)outbuf,l);
		e_khstr += l;
	}
	if (*pkhs == NULL)
		*pkhs = *pkhe = kh;
	else
		(*pkhe)->kh_next = kh;
	*pkhe = kh_endof(kh);
}

static int exist_kouho(u_char *str, int len)
{
	register u_char *pre;
	int l;

	for (pre = wdsbuf; pre < e_khstr; ) {
		l = *pre++;
		if (l == len && strncmp((char*)pre,(char*)str,l) == 0)
			return(1);
		pre += l;
	}
	return(0);
}

KHMEM *kh_skip(register KHMEM *kh)
{
	KHMEM *khret;

	while (kh->khtype & KH_CONTINUE)
		kh = kh->kh_next;
	khret = kh->kh_next;
	kh->kh_next = NULL;
	return(khret);
}

KHMEM *kh_followed(register KHMEM *kh)
{
	while (kh->khtype & KH_CONTINUE)
		kh = kh->kh_next;
	return(kh->kh_next);
}

KHMEM *kh_endof(register KHMEM *kh)
{
	register KHMEM *k;

	if (kh == NULL)
		return(NULL);
	for ( ; ; ) {
		if ((k = kh->kh_next) == NULL)
			break;
		kh = k;
	}
	return(kh);
}

int kh_count(KHMEM *kh)
{
	register int n = 0;

	while (kh) {
		kh = kh_followed(kh);
		n++;
	}
	return(n);
}

int kh_length(register KHMEM *kh)
{
	int l;

	if (kh->khtype == KH_ROMAN)
		return(cur_len * 2);
	l = (cur_len - kh->chlen) * 2;
	while (kh->khtype & KH_CONTINUE) {
		l += strlen((char*)kh->khbuf);
		kh = kh->kh_next;
	}
	return(l + strlen((char*)kh->khbuf));
}

KHMEM *take_kouho(KHMEM *kp, int n)
{
	while (n--)
		kp = kh_followed(kp);
	return(kp);
}

/*
**
**		후보 표시용 루틴
**
*/

static u_char kouho_head[] = "  1.";
static u_char kouho_rest[] = "  럄      ";
#define KOUHO_HEAD_LEN	(short)(sizeof(kouho_head) - 1)
#define KOUHO_REST_LEN	(short)(sizeof(kouho_rest) - 1)

/* 지정 갯수의 후보를 표시한다. */
void disp_kouho(void)
{
	extern MEAN meanbuf[];
	register int i;
	register KHMEM *kp;
	int ofs, l;

	/*** display kouho zan ***/
	kouho_set_num(kh_count(top_kh) - gun_nkh - 1,&kouho_rest[5]);
	sstrtom(meanbuf,kouho_rest,NOT_MEAN);

	/*** KOUHO display ***/
	ofs = KOUHO_REST_LEN;
	for (i = 0, kp = top_kh; i < gun_num; i++) {
		kouho_head[2] = (u_char)('1' + i);
		ofs += sstrtom(meanbuf + ofs,kouho_head,NOT_MEAN);
		l = kouho_makedisp(cur_pos,cur_len,kp,meanbuf + ofs);
		if (i == gun_nkh)
			change_kind(meanbuf + ofs,l,PREOUTPUT);
		else
			change_kind(meanbuf + ofs,l,NOT_MEAN);
		ofs += l;
		if ((kp = kh_followed(kp)) == NULL)
			break;
	}
	/*** temporally ***/
	while (ofs < kwin_len) {
		sstrtom(meanbuf + ofs,(u_char*)" ",NOT_MEAN);
		ofs++;
	}
}

static void kouho_set_num(int num, register u_char *s)
{
	if (num < 0)
		num = 0;
	if (num / 100)
		*s++ = (u_char)('0' + (num / 100));
	else
		*s++ = ' ';
	num %= 100;
	if (num / 10 || *(s - 1) != ' ')
		*s++ = (u_char)('0' + (num / 10));
	else
		*s++ = ' ';
	num %= 10 ;
	*s++ = (u_char)('0' + num);
}

/* 1개의 후보를 표시용 MEAN에 변환한다. */
int kouho_makedisp(int pos, int len, KHMEM *kouho, MEAN	*mbuf)
{
	int l;

	if (kouho == NULL || (kouho->khtype & KH_ROMAN)) {
		/* 모두 다 입력그대로 표시 */
		return(roman_makedisp(pos,len,mbuf,0));
	}
	/* 자립어 부분의 표시 */
	l = jiritu_makedisp(kouho,mbuf);
	mbuf += l;
	/* 부속어의 표시 */
	l += roman_makedisp(pos + kouho->chlen,len - kouho->chlen,mbuf,0);
	return(l);
}

/* 1개의 후보의 자립어 부분을 표시용 MEAN에 변환한다. */
int jiritu_makedisp(KHMEM *kouho, MEAN *mbuf)
{
	int l;

	for (l = 0; ; ) {
		l += sstrtom(mbuf + l,kouho->khbuf,CERTAIN);
		if (kouho->khtype & KH_CONTINUE)
			kouho = kouho->kh_next;
		else
			break;
	}
	return(l);
}

/*
**		후보군 표시 처리 루틴
*/

int inc_gun(KHMEM *kh)
{
	int n, l;
	int maxlen;
	KHMEM *kp;

	if (kh == NULL)
		return(0);
	maxlen = kwin_len - KOUHO_REST_LEN;
	for (n = l = 0, kp = kh; n < 9 && kp; n++, kp = kh_followed(kp)) {
		if ((l += KOUHO_HEAD_LEN + kh_length(kp)) > maxlen)
			break;
	}
	if (n == 0)
		return(1);
	return(n);
}

/** 다음 후보군을 낸다 **/
int next_gun(int sw, int wrap)
{
	KHMEM *save_top_kh;
	int num;

	save_top_kh = top_kh;
	top_kh = take_kouho(top_kh,gun_num);
	if ((num = inc_gun(top_kh)) == 0) {
		if (wrap == OFF) {
			top_kh = save_top_kh;
			return(0);
		}
		init_kouho(0,OFF);
	} else {
		gun_num = num;
	}
	gun_nkh = 0;
	if (sw == ON)
		disp_kouho();
	return(1);
}

/** 이전 후보군을 낸다. **/
int back_gun(int sw, int wrap)
{
	KHMEM *save_top_kh;
	int save_gun_num;

	save_top_kh = top_kh;
	save_gun_num = gun_num;
	init_kouho(0,OFF);
	if (save_top_kh == top_kh) {
		if (wrap == OFF) {
			top_kh = save_top_kh;
			gun_num = save_gun_num;
			gun_nkh = 0;
			return(0);
		}
		save_top_kh = NULL;
	}
	for ( ; ; ) {
		if (take_kouho(top_kh,gun_num) == save_top_kh)
			break;
		next_gun(OFF,ON);
	}
	if (sw == ON)
		disp_kouho();
	return(1);
}

/** 다음 후보를 낸다. **/
int next_kouho(int sw, int wrap)
{
	if (gun_nkh + 1 >= gun_num) {
		return(next_gun(sw,wrap));
	} else {
		gun_nkh++;
	}
	if (sw == ON)
		disp_kouho();
	return(1);
}

/** 이전 후보를 낸다. **/
int back_kouho(int sw, int wrap)
{
	if (gun_nkh == 0) {
		if (back_gun(OFF,wrap) == 0)
			return(0);
		gun_nkh = gun_num - 1;
	} else {
		gun_nkh--;
	}
	if (sw == ON)
		disp_kouho();
	return(1);
}
