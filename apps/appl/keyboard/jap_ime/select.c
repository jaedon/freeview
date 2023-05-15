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
	select.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
**	후보 표시 / 선택 부 
*/
#include <string.h>

#include	"askdef.h"
#include	"askkey.h"
#include	"mean.h"
#include	"imode.h"
#include	"envkey.h"
#include	"pword.h"
#include	"hwork.h"
#include	"dic.h"
#include	"henkan.h"
#include	"specsyn.h"
#include	"aci.h"
#include	"dictypes.h"
#include	"dicdef.h"
#include	"select.h"
#include	"apiroman.h"
#include	"memory.h"
#include	"kouho.h"
#include	"roman.h"
#include	"dic_main.h"


static void	add_prevwd(int pos, int len, KHMEM *kh, int init);


extern int kana_len;
extern KHMEM *top_kh;
extern int gun_nkh, gun_num;
extern HCHAR hchar[];
extern MEAN	meanbuf[];
extern int func_mode;

int	learn_on = OFF;			/* 학습의 ON/OFF */
int	cur_pos;				/* 현재의 문절의 개시위치 */
int	cur_len;				/* 현재의 문절의 길이 */
KHMEM *cur_kh;				/* 현재 문절의 후보 */

int	sel_job;				/* 변환 표시 상태 */
#define	SJ_CTRANS	(-1)	/* ... 문자종 변경 상태 (통상) 	*/
#define	SJ_CTRANS2	(-2)	/* ... 문자종 변경 상태 (CHLEN 으로 부터 ) */
#define	SJ_FIRST	(0)		/* ... 후보군  비 표시 상태		*/
#define	SJ_GUNDISP	(1)		/* ... 후보군 표시 상태		*/

/*static int once_raw;*/		/* 일단, RAW 변환을 했는가 ? */

extern int undo_length;
extern int undo_im_state;
extern int undo_cur_pos;

int	disp_select(void)
{
	extern MEAN	meanbuf[];
	MEAN	*mp;
	int	l, tlen;
	int	pos, blen;

	pos = 0;
	tlen = 0;
	mp = meanbuf;
	blen = bunsetu_len(pos);
	while (blen) {
		if (pos == cur_pos) {
			l = current_makedisp(pos,blen,mp);
			cur_len = blen;
			change_kind(mp,l,PREOUTPUT);
		} else {
			l = first_makedisp(pos,blen,mp);
			change_kind(mp,l,CERTAIN);
		}
		mp += l;
		pos += blen;
		tlen += l;
		blen = bunsetu_len(pos);
	}
	return(tlen);
}

int	current_makedisp(int start, int len, MEAN *mbuf)
{
	KHMEM *kh;

	if (sel_job == SJ_GUNDISP || func_mode >= 3)
		kh = take_kouho(top_kh,gun_nkh);
	else
		kh = take_kouho(hchar[start].kh_mem,0);
	return(kouho_makedisp(start,len,kh,mbuf));
}


int	first_makedisp(int start, int len, MEAN	*mbuf)
{
	return(kouho_makedisp(start,len,hchar[start].kh_mem,mbuf));
}


void unify_khmem(int pos, int sw)
{
	register KHMEM *kh, *khn;
	KHMEM *khc;

	if ((kh = hchar[pos].kh_mem) == NULL)
		return;
	if (sw == ON && (sel_job == SJ_GUNDISP || func_mode >= 3)) {
		khc = take_kouho(top_kh,gun_nkh);
		while (kh != NULL) {
			if (kh == khc)
				break;
			khn = kh_followed(kh);
			free_khmem(kh);
			kh = khn;
		}
		hchar[pos].kh_mem = kh;
	}

	if (kh != NULL)
	{
		kh = kh_skip(kh);
		free_khmemlist(kh);
	}
}


u_char *select_tostr()
{
	extern u_char outbuf[];
	extern MEAN	meanbuf[];
	int	l;

	l = disp_select();
	meantosjis(meanbuf,(char*)outbuf,l);
	return(outbuf);
}

u_char *select_subtostr(int pos, int len)
{
	extern u_char outbuf[];
	extern MEAN	meanbuf[];
	register MEAN *mp;
	int	l, blen;
	int	epos;

	epos = pos + len;
	mp = meanbuf;
	blen = bunsetu_len(pos);
	while (pos < epos && blen) {
		if (pos == cur_pos) {
			l = current_makedisp(pos,blen,mp);
		} else {
			l = first_makedisp(pos,blen,mp);
		}
		mp += l;
		pos += blen;
		blen = bunsetu_len(pos);
	}
	meantosjis(meanbuf,(char*)outbuf,(int)(mp - meanbuf));
	return(outbuf);
}

/*
**	학습 루틴 
*/

static u_char prev_yomi[80] = "";	/* 직전에 확정했던 단어를 정보 */
static u_char prev_tango[80] = "";	/* (2문절 학습용 ) */


void kh_learn(int pos, int len, KHMEM *kh, register BSMEM *bsl)
{
#define	EntidBuf	((ENTID *)wdsbuf)

	extern u_char wdsbuf[];
	register ENTID *entp;
	ENTID entid, pre_entid;
	CHMEM *chm;

	if (kh->wdid == (unsigned short)(-1))
		chm = NULL;
	else if ((chm = kh->wd_mem) == NULL)
		return;

	entp = EntidBuf;
	if (bsl == NULL || bsl == (BSMEM *)-1) {
		if (chm != NULL)
			*entp++ = chm->entid;
	} else {
		pre_entid = (ENTID)-1;
		do {
			if (bsl->wd_mem == NULL)
				continue;
			if ((entid = bsl->wd_mem->entid) == -1)
				continue;
			if (entid != pre_entid) {
				*entp++ = entid;
				pre_entid = entid;
			}
			bsl = bsl->bs_next;
		} while (bsl);
	}

	if (chm == NULL) {	/* 문자종 변경했던 단어의 학습 */
		NEWWD	*newp;
		newp = raw_newwd(pos,len,kh);
		dic_newlearn(newp,EntidBuf,(int)(entp - EntidBuf));
		return;
	}
	dic_learn(chm->entid,kh->wdid,EntidBuf,(int)(entp - EntidBuf));

#undef	EntidBuf
}


void prev_learn(KHMEM *kh)
{
	NEWWD newwd;
	CHMEM *ch;

	if (prev_yomi[0] == '\0')
		return;
	newwd.ustr = prev_yomi;
	newwd.ulen = (u_short)strlen((char*)prev_yomi);

	newwd.level = 0;
	if (kh == NULL || (ch = kh->wd_mem) == NULL) {
		newwd.syntax = SYN_TANKANJI;
		newwd.syntaxinfo.katuyo = 0;
	} else {
		newwd.syntax = ch->ch_syn;
		newwd.syntaxinfo.katuyo = ch->ch_ktu;
	}
	newwd.kbuf = prev_tango;
	dic_tmptouroku(&newwd);
}

void wd_learn(int pos, int epos)
{
	HCHAR *hp;
	KHMEM *kh;
	int	blen;

	blen = bunsetu_len(pos);
	while (pos < epos && blen) {
		hp = &hchar[pos];
		if (pos == cur_pos &&
			(sel_job == SJ_GUNDISP || func_mode >= 3)) {
			kh = take_kouho(top_kh,gun_nkh);
		} else {
			kh = take_kouho(hp->kh_mem,0);
		}

		if (im_state != IM_SELECT && func_mode <= 2) {
normal:
			kh_learn(pos,blen,kh,hp->bs_mem);
			pos += blen;
			continue;
		}

		if (hp->fst_bslen && hp->fst_bslen != hp->bnstlen) {
			/* 문절 길이의 변경이 일어 났다. */
			add_prevwd(pos,blen,kh,ON);
			kh_learn(pos,blen,kh,hp->bs_mem);
		} else if (prev_yomi[0]) {
			/* 문전 길이는 디폴트 */
			add_prevwd(pos,blen,kh,OFF);
			prev_learn(kh);
			clear_prevwd();
		} else {
			goto normal;
		}
		pos += blen;
		blen = bunsetu_len(pos);
	}
}


void clear_prevwd(void)
{
	prev_yomi[0] = '\0';
}

static void	add_prevwd(int pos, int len, KHMEM *kh, int init)
{
	extern u_char kana_ustr[];
	u_char *y, *t;
	int l;

	y = prev_yomi;
	t = prev_tango;
	if (init == OFF) {
		y += strlen((char*)prev_yomi);
		t += strlen((char*)prev_tango);
		len = kh->chlen;
	}
	strncpy((char*)y,(char*)&kana_ustr[pos],len);
	*(y + len) = '\0';
	if (strlen((char*)y) != (u_int)len) {		/* u-코드의 부정 */
		clear_prevwd();
		return;
	}
	l = kouho_makedisp(pos,len,kh,meanbuf);
	meantosjis(meanbuf,(char*)t,l);
}

NEWWD *raw_newwd(int pos, int len, KHMEM *kh)
{
	extern u_char yomi_buf[], tango_buf[];
	extern u_char kana_ustr[];
	static NEWWD newwd;
	int l;

	strncpy((char*)yomi_buf,(char*)&kana_ustr[pos],len);
	yomi_buf[len] = '\0';
	l = kouho_makedisp(pos,len,kh,meanbuf);
	meantosjis(meanbuf,(char*)tango_buf,l);
	newwd.ustr = yomi_buf;
	newwd.ulen = (short)len;
	newwd.level = 0;
	newwd.syntax = SYN_EXMEISHI;
	newwd.syntaxinfo.katuyo = 0;
	newwd.kbuf = tango_buf;
	return(&newwd);
}
