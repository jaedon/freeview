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
	henkan.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include	"askdef.h"
#include	"pword.h"
#include	"imode.h"
#include	"hwork.h"
#include	"dic.h"
#include	"henkan.h"
#include	"specsyn.h"
#include	"mean.h"
#include	"dictypes.h"
#include	"dicdef.h"
#include	"dic_main.h"
#include	"memory.h"
#include	"prefer.h"
#include	"kouho.h"
#include	"rmtype.h"
#include	"connect.h"


#define ERROR			(-1)
#define NOTYET			0
#define DONE			1
#define FL_NOTYET		(PWORD *)(-1)
#define PWMEM_NOTYET	(PWMEM *)(-1)

int		henkan_mode = KYOUSEI_HENKAN;
int		kd_mode = KD_CHIKUJI;
int		ikkatsu_mode = OFF;

extern int		kana_len;
extern u_char	kana_ustr[];
extern JSYMBOL	kana_buf[];

HCHAR	hchar[HCHAR_SIZE];


void init_hchar(void)
{
	register HCHAR	*hp;

	for (hp = hchar; hp < &hchar[HCHAR_SIZE]; hp++)
		clear_hchar(hp);
}


void clear_hchar(register HCHAR	*hp)
{
	hp->f_list = FL_NOTYET;
	hp->ch_mem = NULL;
	hp->bs_mem = NULL;
	hp->kh_mem = NULL;
	hp->kl_mem = NULL;
	hp->merged = 0;
	hp->bnstlen = 0;
	hp->fst_bslen = 0;
	hp->w_usedlen = -1;
	hp->f_usedlen = -1;
	hp->page_ulen = -1;
}


void free_hchar(int spos, int epos, int level)
{
	register HCHAR	*hp;
	HCHAR	*ehp = &hchar[epos];

	for (hp = &hchar[spos]; hp < ehp; hp++) {
		free_hchar_one(hp,level);
	}
}


void free_hchar_one(register HCHAR *hp, int level)
{
	if (level == ALL_FREE) {
		hp->f_list = FL_NOTYET;
		hp->f_usedlen = -1;
		if (hp->ch_mem != (CHMEM *)-1) {
			free_chmemlist(hp->ch_mem);
		}
		hp->ch_mem = NULL;
		hp->w_usedlen = -1;
		hp->page_ulen = -1;
		hp->fst_bslen = 0;
	}
	if (hp->bs_mem != NULL && hp->bs_mem != (BSMEM *)-1) {
		free_bsmemlist(hp->bs_mem);
	}
	hp->bs_mem = NULL;
	if (hp->kh_mem != NULL) {
		free_khmemlist(hp->kh_mem);
	}
	hp->kh_mem = NULL;
	hp->kl_mem = NULL;
	hp->merged = 0;
	hp->bnstlen = 0;
}

/*
**		º¯È¯ ¸ÞÀÎ ÇÔ¼ö,
*/

ENTRYS	entbuf;

#define CSMEM_NOTYET	(CHMEM *)(-1)
#define BSMEM_NOTYET	(BSMEM *)(-1)

/*
**		º¯È¯ ºÎ 
*/

void henkan(int spos, int epos, int mode, int fstlen)
{
	extern int henkan_mode;
	register int fstpos;
	int sndpos, len;
	HCHAR *hp;
	BSMEM *bsm;

	henkan_mode = mode;
	fl_check(spos,epos);
	if (henkan_mode == FREE_HENKAN && ikkatsu_mode)
		return;
	for (fstpos = spos; fstpos < epos; ) {
		hp = &hchar[fstpos];
		if (hp->bnstlen > 0) {
			fstpos += hp->bnstlen;
			continue;
		}
		if (!hp->ch_mem) {
			if (ch_check(fstpos,epos) == NOTYET)
				break;
		}
		if (!hp->bs_mem) {
			if (bs_check(fstpos,epos) == NOTYET)
				break;
		}
		for (bsm = hp->bs_mem; bsm; bsm = bsm->bs_next) {
			sndpos = fstpos + bsm->bs_len;
			if (sndpos >= epos) {
				if (henkan_mode >= FREE_HENKAN) {
					if (bsm->bs_syn == SYN_KUTEN)
						henkan_mode = KYOUSEI_HENKAN;
					else
						break;
				}
				continue;
			}
			hp = &hchar[sndpos];
			if (!hp->ch_mem) {
				if (ch_check(sndpos,epos) == NOTYET)
					break;
			}
			if (!hp->bs_mem) {
				if (bs_check(sndpos,epos) == NOTYET)
					break;
			}
		}
		if (fstpos == spos) {
			if (henkan_mode == FIXED_HENKAN) {
				bs_prefer(fstpos,epos,fstlen);
				hchar[spos].merged = 1;
				len = fstlen;
			} else if (henkan_mode == FIXEDRAW_HENKAN) {
				hchar[spos].merged = 1;
				len = fstlen;
			} else
				goto normal;
		} else if (fstpos == spos + fstlen &&
					(henkan_mode == FIXEDRAW_HENKAN)) {
			bs_prefix(fstpos);				/* fstpos ¹®ÀÚÁ¾·ù º¯È¯ÇÑ ´ÙÀ½ ¹®Àý */
			goto normal;
		} else {
normal:
			if ((len = bs_prefer(fstpos,epos,-1)) == -1)
				break;
		}
		unify_bsmem(fstpos,len);
		first_kouho(fstpos,len);
		hchar[fstpos].bnstlen = (char)len;
		fstpos += len;
	}

	/* ¹«ÀÚ ¹®ÀýÀÇ º´ÇÕ */
	for (fstpos = spos; fstpos < epos; fstpos += len) {
		BSMEM *bsmem;
		int l;

		hp = &hchar[fstpos];
		l = hp->bnstlen;
		if (hp->merged) {
			len = l;
			continue;
		}
		if ((len = concat_bslen(fstpos,epos)) < 0)
			return;
		if (len == 0)
			len = l;
		if (len == l) {
			hp->merged = 1;
			continue;
		}
		while (l < len) {
			if ((bsm = hp->bs_mem) != (BSMEM *)-1)
				free_bsmemlist(bsm);
			hp->bs_mem = NULL;
			hp->bnstlen = 0;
			free_khmemlist(hp->kh_mem);
			hp->kh_mem = NULL;
			hp = &hchar[fstpos + l];
			l += hp->bnstlen;
		}
		if ((bsm = hp->bs_mem) != (BSMEM *)-1)
			free_bsmemlist(bsm);
		hp->bs_mem = NULL;
		hp->bnstlen = 0;
		free_khmemlist(hp->kh_mem);
		hp->kh_mem = NULL;
		hp = &hchar[fstpos];
		hp->bnstlen = (char)len;
		if ((bsmem = alloc_bsmem()) == NULL)
			continue;
		bsmem->bs_len = (short)len;
		bsmem->bs_syn = SYN_TANKANJI;
		bsmem->bs_ktu = 0;
		bsmem->wd_mem = NULL;
		bsmem->itsprior = 0;
		bsmem->ctdprior = 0;
		bsmem->bs_next = NULL;
		hp->bs_mem = bsmem;
		hp->merged = 1;
		first_kouho(fstpos,len);
	}
}

int concat_bslen(int pos, int epos)
{
	register HCHAR *hp;
	BSMEM *bsm;
	int l, len;
		
	l = 0;
	hp = &hchar[pos];
	len = 0;
	while (pos < epos) {
		if ((l = hp->bnstlen) == 0)
			return(-1);
		bsm = hp->bs_mem;
		if (bsm != NULL && bsm != (BSMEM *)-1 &&
			bsm->bs_syn != SYN_TANKANJI)
			break;
		len += l;
		pos += l;
		hp += l;
	}
	if (len == 0 || pos >= epos)
		return(len);
	return(len + l);
}

int muhenkan(int spos, int epos)
{
	register int fpos;

	for (fpos = spos + 1; fpos < epos; fpos++) {
		if (!not_bhead(kana_ustr[fpos]))
			break;
	}
	return(fpos - spos);
}

void fl_check(int spos, int epos)
{
	HCHAR *hp;
	int ul;
	int used;
	char *up;
	PWORD *pwd;

	/* °¢ À§Ä¡¿¡ åÀÖ¾î¼­ ºÎ°¡¾îÀÇ ¼­Ä¡ */
	for (hp = &hchar[spos], up = (char *)&kana_ustr[spos], ul = epos - spos;
								ul > 0; up++, ul--, hp++) {
		if (hp->f_list == FL_NOTYET) {
			pwd = srch_pword((unsigned char *)up,ul,&used);
			if (pwd == FL_NOTYET)
				break;
			hp->f_usedlen = (char)used;
			hp->f_list = pwd;
		}
	}
}


int ch_check(int spos, int epos)
{
	HCHAR *hp;
	int pos;
	u_int u_len;
	char *u_str, *p;
	SYNTAX xsyn;
	u_int lmin, lmax;

	if ((u_len = epos - spos) <= 0)
		return(NOTYET);
	u_str = (char*)&kana_ustr[spos];
	hp = &hchar[spos];

	xsyn = 0;
	lmax = 0;

	/* Ä«³ªÄ«³ª ¶Ç´Â ¿µ¹® */
	if (is_num(*u_str)) {												/* ¼ýÀÚ */
		xsyn = SYN_SUUJI;
		for (lmin = 1; lmin < u_len; lmin++) {
			if (!is_num(u_str[lmin]))
				break;
		}
		if (henkan_mode >= FREE_HENKAN && lmin >= u_len)
			return(NOTYET);
	} else if (is_alpha(*u_str)) {										/* ¾ËÆÄºª */
		xsyn = SYN_EXMEISHI;
		for (lmin = 1; lmin < u_len; lmin++) {
			if (!is_alphanum(u_str[lmin]))
				break;
		}
		if (henkan_mode >= FREE_HENKAN && lmin >= u_len)
			return(NOTYET);
	} else if (is_paren(*u_str)) {										/* °ýÈ£ */
		if (henkan_mode >= FREE_HENKAN && u_len <= 1)
			return(NOTYET);
		lmin = lmax = 1;
		xsyn = SYN_EXMEISHI;
		goto dic_get;
	} else if (*u_str && (*(unsigned char *)u_str < 0xa0)) {			/* ±âÈ£ */
		if (henkan_mode >= FREE_HENKAN && u_len <= 1)
			return(NOTYET);
		lmin = lmax = 1;
		xsyn = SYN_KIGOU;
		goto dic_get;
	} else if (*u_str == 0 && (kana_buf[spos].symbol & 0xff00) == 0) {	/* ¹Ý°¢ */
		for (pos = spos + 1; pos < epos; pos++) {
			if (kana_buf[pos].symbol & 0xff00)
				break;
		}
		xsyn = SYN_EXMEISHI;
		goto check_and_create;
	} else if (*u_str == 0 && is_kata(kana_buf[spos].symbol,0)) {		/* Ä«Å¸Ä«³ª */
		for (pos = spos + 1; pos < epos; pos++) {
			if (!is_kata(kana_buf[pos].symbol,1))
				break;
		}
		xsyn = SYN_EXMEISHI;
		check_and_create:
		if (henkan_mode >= FREE_HENKAN && pos == epos)
			return(NOTYET);
		add_dummy_chmem(spos,pos - spos,xsyn);
		return(DONE);
	} else if (*u_str == 0) {									/* ±× ¿ÜÀÇ 2¹ÙÀÌÆ® °è */
		for (pos = spos + 1, u_str++; pos < epos; pos++, u_str++) {
			if (*u_str)
				break;
		}
		if (is_jisknj(kana_buf[pos - 1].symbol))
			xsyn = SYN_TANKANJI;
		else if (is_jiskig(kana_buf[pos - 1].symbol))
			xsyn = SYN_KIGOU;
		else
			xsyn = SYN_EXMEISHI;
			goto check_and_create;
	} else {															/* Åë»ó */
			lmin = 1;
	}

	for (p = u_str, lmax = 0; lmax < u_len; lmax++) {
		if (*p++ == '\0')
			break;
	}
	u_len = lmax;
	switch (dic_snssyn((UCODE *)u_str,(ULEN)u_len,&entbuf)) {
	case DIC_HASLONGER:
		if (henkan_mode < FREE_HENKAN) {
			lmax = u_len;
			break;
		}
		return(NOTYET);
	case DIC_HASMATCH:
		if (xsyn && entbuf.yomilen <= (int)lmin) {
			dic_freeentid(entbuf.entid);
			hp->w_usedlen = (char)entbuf.used_ulen;
			lmax = lmin;
			break;
		}
		hchar_addchmem(spos,make_chmem(spos,&entbuf));
		hp->w_usedlen = (char)entbuf.used_ulen;
		lmax = entbuf.yomilen - 1;
		break;
	case DIC_HASNONE:
	case DIC_BADARG:
	case DIC_ISCLOSED:
	case DIC_FATAL:
	default:
		if (xsyn) {
			lmax = lmin;
			break;
		}
		hp->ch_mem = (CHMEM *)-1;
		return(ERROR);
	}
dic_get:
	while (lmax >= lmin) {
		if (xsyn && lmax == lmin) {
			add_dummy_chmem(spos,lmin,xsyn);
		}
		if (dic_getsyn((UCODE*)u_str,(ULEN)lmax,&entbuf) == DIC_HASMATCH) {
			if (hp->w_usedlen == (char)(-1))
				hp->w_usedlen = (char)entbuf.used_ulen;
			hchar_addchmem(spos,make_chmem(spos,&entbuf));
		}
		lmax--;
	}
	return(DONE);
}


CHMEM *make_chmem(int pos, ENTRYS *pent)
{
	int ylen;
	ENTID id;
	int num;
	SYNREF *pref;
	CHMEM *t, *smem = NULL, *emem = NULL;
	HCHAR *hp;

	hp = &hchar[pos];
	ylen = pent->yomilen;
	id = pent->entid;
	num = pent->synreflen;
	for (pref = pent->synref; num--; pref++) {
		if ((t = alloc_chmem()) == NULL)
			break;
		if (smem == NULL)
			smem = t;
		t->yomilen = (short)ylen;
		t->ch_syn = pref->syntax;
		t->ch_ktu = pref->syntaxinfo.katuyo;
		t->entid = id;
		t->maxprior = pref->rtime;
		t->ch_next = NULL;
		if (emem != NULL)
			emem->ch_next = t;
		emem = t;
	}
	return(smem);
}

void hchar_addchmem(int pos, CHMEM *chmem)
{
	register CHMEM **m;

	m = &hchar[pos].ch_mem;
	while (*m) {
		m = &(*m)->ch_next;
	}
	*m = chmem;
}


void add_dummy_chmem(int pos, int ylen, SYNTAX syn)
{
	CHMEM *t;

	if ((t = alloc_chmem()) == NULL)
		return;
	t->yomilen = (short)ylen;
	t->ch_syn = syn;
	t->ch_ktu = 0;
	t->entid = (unsigned long)-1L;
	t->maxprior = 0;
	t->ch_next = NULL;
	hchar[pos].w_usedlen = (char)(ylen + 1);
	hchar_addchmem(pos,t);
}

static CHMEM null_chmem = {		/* ±æÀÌ 0ÀÇ ÀÚ¸³¾î */
	0,							/* yomilen */
	SYN_EXMEISHI,				/* ch_syn */
	0,							/* ch_ktu */
	(unsigned long)-1L,				/* entid */
	0L,							/* maxprior */
	NULL						/* ch_next */
};


int bs_check(int spos, int epos)
{
	HCHAR *hp;
	CHMEM *chm;
	BSMEM *bsm;

	hp = &hchar[spos];
	if ((chm = hp->ch_mem) == (CHMEM *)-1)
		goto no_chmem;
	for (chm = hp->ch_mem; chm; chm = chm->ch_next) {
		if ((bsm = make_bsmem(spos,epos,chm)) == BSMEM_NOTYET) {
			if (hp->bs_mem) {
				free_bsmemlist(hp->bs_mem);
				hp->bs_mem = NULL;
			}
			return(NOTYET);
		}
		if (bsm != NULL) {
			hchar_addbsmem(spos,bsm);
		}
	}
no_chmem:
	if ((bsm = make_bsmem(spos,epos,&null_chmem)) == BSMEM_NOTYET) {
		if (hp->bs_mem) {
			free_bsmemlist(hp->bs_mem);
			hp->bs_mem = NULL;
		}
		return(NOTYET);
	}
	if (bsm != NULL)
		hchar_addbsmem(spos,bsm);

	if (hp->bs_mem == NULL) {
		if ((bsm = alloc_bsmem()) == NULL)
			return(ERROR);

		bsm->bs_len = (short)muhenkan(spos,epos);
		bsm->bs_syn = SYN_TANKANJI;
		bsm->bs_ktu = 0;
		bsm->wd_mem = NULL;
		bsm->itsprior = 0;
		bsm->ctdprior = 0;
		bsm->bs_next = NULL;
		hp->bs_mem = bsm;
		return(DONE);
	}
	return(DONE);
}

BSMEM *make_bsmem(int spos, int epos, CHMEM	*chmem)
{
	int fpos, ylen;
	PWMEM *pwm, *pwm_bak;
	BSMEM *t, *smem = NULL, *emem = NULL;

	ylen = chmem->yomilen;
	fpos = spos + ylen;
	pwm = pword_list(fpos,epos,chmem->ch_syn,chmem->ch_ktu);
	if (pwm == PWMEM_NOTYET)
		return(BSMEM_NOTYET);
	pwm_bak = pwm;
	while (pwm) {
		if ((t = alloc_bsmem()) == NULL)
			break;
		if (smem == NULL)
			smem = t;
		t->bs_len = (short)(ylen + pwm->pw_len);
		t->bs_syn = pwm->lst_sn;
		t->bs_ktu = pwm->lst_kt;
		t->wd_mem = chmem;
		t->itsprior = 0;
		t->ctdprior = 0;
		t->bs_next = NULL;
		if (emem != NULL)
			emem->bs_next = t;
		emem = t;
		pwm = pwm->fnext;
	}

	free_pwmemlist(pwm_bak);		/* pwmÀ» ÇØ¹æÇÑ´Ù.  */

	if (ylen > 0 && setu_end(chmem->ch_syn,chmem->ch_ktu)
				&& (fpos >= epos || !not_bhead(kana_ustr[fpos]))) {
		if ((t = alloc_bsmem()) != NULL) {
			if (smem == NULL)
				smem = t;

			t->bs_len = (short)ylen;
			t->bs_syn = chmem->ch_syn;
			t->bs_ktu = chmem->ch_ktu;
			t->wd_mem = chmem;
			t->itsprior = 0;
			t->ctdprior = 0;
			t->bs_next = NULL;
			if (emem != NULL)
				emem->bs_next = t;
		}
	}
	return(smem);
}

static BSMEM *ins_bsmem(BSMEM *bsl, BSMEM *bsm)
{
	register BSMEM *m;
	BSMEM *pre;
	int len;

	len = bsm->bs_len;
	if (bsl == NULL || bsl->bs_len < len) {
		bsm->bs_next = bsl;
		return(bsm);
	}
	for (pre = bsl, m = bsl->bs_next; m; pre = m, m = m->bs_next) {
		if (m->bs_len < len)
			break;
	}
	pre->bs_next = bsm;
	bsm->bs_next = m;
	return(bsl);
}

void hchar_addbsmem(int pos, BSMEM *bsmem)
{
	register BSMEM *m;
	BSMEM *next, *bsl;

	bsl = hchar[pos].bs_mem;
	for (m = bsmem; m; ) {
		next = m->bs_next;
		bsl = ins_bsmem(bsl,m);
		m = next;
	}
	hchar[pos].bs_mem = bsl;
}

void unify_bsmem(int pos, int len)
{
	register BSMEM *bsm, **pre;

	pre = &hchar[pos].bs_mem;
	for (bsm = *pre; bsm; bsm = *pre) {
		if (bsm->bs_len == len) {
			pre = &bsm->bs_next;
		} else {
			*pre = bsm->bs_next;
			free_bsmem(bsm);
		}
	}
}

int bunsetu_len(int pos)
{
	extern int im_state;
	register HCHAR *hp;

	if (pos >= kana_len)
		return(0);
	hp = &hchar[pos];

	if (im_state == IM_EDITING && !hp->merged)
		return(0);

	return(hp->bnstlen);
}

void save_fst_bslen(int pos)
{
	register HCHAR *hp;

	hp = &hchar[pos];
	if (hp->fst_bslen == 0 && hp->merged)
		hp->fst_bslen = hp->bnstlen;
}

/*
**		¾ðµÎ (¹®ÀýÁ¤º¸)
*/

HCHAR ud_hchar[HCHAR_SIZE];
int ud_hchar_len;

extern int undo_prepared;

/*
**		ºÎ¼Ó¾î °ü·Ã ÇÔ¼ö 
*/

PWORD *srch_pword(u_char *ustr, int ulen, int *pused)
{
	register u_char *p, *u;
	int lp, lu;
	PWORD *ppwd, *end;
	u_char key;
	int usemax = 1, use;

	if ((key = *ustr) < 0xa0) {
		*pused = 1;
		return(NULL);
	}
	key -= 0xa0;
	ppwd = &pword[pwordmap[key + 1]] - 1;
	end = &pword[pwordmap[key]];
	ustr++;
	ulen--;
	while (ppwd >= end) {
		p = &pluswd[ppwd->point];
		lp = *p++ - 1;
		u = ustr;
		lu = ulen;
		for (use = 1; ; u++, p++, lu--, lp--) {
			if (lp == 0) {
				if (use > usemax)
					usemax = use;
				*pused = usemax;
				return(ppwd);
			}
			if (lu == 0) {
				if (henkan_mode >= FREE_HENKAN)
					return(FL_NOTYET);
				else
					break;
			}
			use++;
			if (*p != *u) {
				if (use > usemax)
					usemax = use;
				break;
			}
		}
		ppwd--;
	}
	*pused = usemax;
	return(NULL);
}


PWMEM *pword_list(int spos, int epos, u_char sn, u_char kt)
{
	register PWORD *ppwd;
	register PWMEM *s;
	PWMEM *t, *umem = NULL;
	int l;

	if (spos >= epos) {
		if (henkan_mode < FREE_HENKAN)
			return(NULL);
		else
			return(PWMEM_NOTYET);
	}
	if ((ppwd = hchar[spos].f_list) == FL_NOTYET)
		return(PWMEM_NOTYET);
	if (ppwd == NULL && is_kuten(kana_ustr[spos])) {
		if (sn == 0 || setu_end(sn,kt)) {
			t = alloc_pwmem();
			if (t) {
				t->pw_len = 1;
				t->fst_sn = t->lst_sn = SYN_KUTEN;
				t->fst_kt = t->lst_kt = 0;
				t->fnext = NULL;
			}
			return(t);
		}
	}
	for ( ; ppwd; ppwd -= ppwd->next) {
		if (setu_end(ppwd->sn,ppwd->kt) &&
				(sn == 0 || goku_connect(sn,kt,ppwd->sn))) {
			l = pluswd[ppwd->point];
			if (spos + l < epos && not_bhead(kana_ustr[spos + l]))
				goto next_pwd;

			t = alloc_pwmem();
			if (t) {
				t->pw_len = (short)l;
				t->fst_sn = t->lst_sn = ppwd->sn;
				t->fst_kt = t->lst_kt = ppwd->kt;
				t->fnext = umem;
				umem = t;
			}
		}
		next_pwd:
		if (ppwd->next == 0)
			break;
	}
	for (ppwd = hchar[spos].f_list; ppwd; ppwd -= ppwd->next) {
		if (sn == 0 || goku_connect(sn,kt,ppwd->sn)) {
			l = pluswd[ppwd->point];
			t = pword_list(spos + l,epos,ppwd->sn,ppwd->kt);
			if (t == PWMEM_NOTYET) {
				free_pwmemlist(umem);
				return(PWMEM_NOTYET);
			}
			if (t) {
				for (s = t; ; ) {
					s->pw_len = (short)(s->pw_len + l);
					s->fst_sn = ppwd->sn;
					s->fst_kt = ppwd->kt;
					if (s->fnext == NULL)
						break;
					s = s->fnext;
				}
				s->fnext = umem;
				umem = t;
			}
		}
		if (ppwd->next == 0)
			break;
	}
	return(umem);
}

int not_bhead(u_char uc)
{
	if (henkan_mode == FIXED_HENKAN || henkan_mode == FIXEDRAW_HENKAN)
		return(0);
	switch (uc) {
	case 0x9d:					/* [ */
	case 0xa1:					/* ‚Ÿ */
	case 0xa3:					/* ‚¡ */
	case 0xa5:					/* ‚£ */
	case 0xa7:					/* ‚¥ */
	case 0xa9:					/* ‚§ */
	case 0xc3:					/* ‚Á */
	case 0xe3:					/* ‚á */
	case 0xe5:					/* ‚ã */
	case 0xe7:					/* ‚å */
	case 0xee:					/* ‚ì */
	case 0xf2:					/* ‚ð */
	case 0xf3:					/* ‚ñ */
		return(1);
	}
	return(0);
}

int is_kuten(u_char uc)
{
	if (uc >= 0xa0)
		return(0);
	switch (uc) {
	case 0x20:					/*	 */
	case 0x21:					/* ! */
	case 0x2c:					/* , */
	case 0x2e:					/* . */
	case 0x3a:					/* : */
	case 0x3b:					/* ; */
	case 0x3f:					/* ? */
	case 0x98:					/* ¡ */
	case 0x9b:					/* ¤ */
	case 0x9c:					/* ¥ */
		return(1);
#ifdef	The_followings_are_deleted
	case 0x22:					/* " */
	case 0x27:					/* ' */
	case 0x28:					/* ( */
	case 0x29:					/* ) */
	case 0x2f:					/* / */
	case 0x3c:					/* < */
	case 0x3e:					/* > */
	case 0x5b:					/* [ */
	case 0x5d:					/* ] */
	case 0x7b:					/* { */
	case 0x7d:					/* } */
	case 0x99:					/* ¢ */
	case 0x9a:					/* £ */
		return(0);
#endif
	}
	return(0);
}
