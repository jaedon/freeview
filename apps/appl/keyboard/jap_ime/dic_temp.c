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
	dic_temp.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include	<string.h>

#include	"askdef.h"
#include	"dic.h"
#include	"dicdef.h"
#include	"dic_temp.h"
#include	"dic_page.h"


#define NODES			512		/* ���R�[�h�̌� */
#define HASH_SIZE		80		/* ���R�[�h�̌��� */

NODE	node_tab[NODES];
NODE	*freelist;
NODE	*hash_tab[HASH_SIZE];
int		temp_page;				/* ���ݎg�p���̈ꎞ�w�K�y�[�W */
uchar	*temp_top;				/* �ꎞ�w�K�y�[�W�󂫃G���A�J�n */
uchar	*temp_end;				/* �ꎞ�w�K�y�[�W�󂫃G���A�I�� */
uchar	temp_updated;			/* �X�V���ꂽ���ǂ��� */
uchar	temp_pages[TEMPPAGES][PAGEMAX];


/*----------------------------------------------*/
/*		�ꎞ�P��֌W������						*/
/*----------------------------------------------*/
void init_temp(void)
{
	init_node_tab();
	init_hash_tab();
	if (read_temp() == ERR)
		reset_temp();
	temp_updated = NO;
}


void flush_temp(void)
{
	if (temp_updated == NO)
		return;
	write_temp();
}


void init_node_tab(void)
{
	register NODE *pn;

	for (pn=node_tab; pn < &node_tab[NODES-1]; pn++) {
		pn->nxt = pn + 1;
	}
	pn->nxt = NULL;
	freelist = node_tab;
}

void init_hash_tab(void)
{
	register int i;

	for (i=0; i<HASH_SIZE; i++)
		hash_tab[i] = (struct node_entry *)NULL;
}

void reset_temp()
{
	register int i;
	uchar *p;

	for (i=0; i<TEMPPAGES; i++) {
		p = temp_pages[i];				
		*p = *(p+1) = 0;
	}		
	temp_page = 0;
	temp_top = temp_pages[0];
	temp_end = temp_pages[0] + PAGEMAX;
}

/*----------------------------------------------*/
/*		�ꎞ�P��̈惁�����[�Ǘ�				*/
/*		(1) �y�[�W�Ǘ�							*/
/*		(2) Hash & Node �Ǘ�					*/
/*----------------------------------------------*/
int hashfunc(UCODE *yomi)
{
	/*		�ǂ݂���ꎞ�P�����������B			*/
	/*		�ǂ݁i���R�[�h�j����̃n�b�V��			*/
	register UCODE c = *yomi;

	if (c >= 0xa0 && c < 0xf0)
		return(c - 0xa0);
	else
		return(0);
}

NODE *alloc_node(void)
{
	NODE *p;
		
	if (freelist == NULL)
		page_gc();
	p = freelist;	
	freelist = freelist->nxt;
	return(p);
}		

void free_node(NODE	*p)
{
	p->nxt = freelist;
	freelist = p;
}

uchar *alloc_record(int len)
{
	uchar *p;

	if (temp_top > (temp_end - len - 2))
		page_gc();
	p = temp_top;	
	temp_top += len;
	/* �y�[�W�I�[�R�[�h */
	*temp_top = 0;
	*(temp_top+1) = 0;
	return(p);
}

NODE **srch_node(UCODE *yomi, ULEN ulen, NODE **npp)
{
	NODE *np, *prev;
	ULEN rulen;
	int idx, cmp;

	prev = NULL;
	np = hash_tab[idx=hashfunc(yomi)];
	for ( ; np != NULL; prev = np, np = np->nxt) {
		rulen = UCODE_LEN(np->ptr);
		if ((cmp=strncmp((char *)UCODE_STR(np->ptr), (char *)yomi, rulen)) == 0) {
			if (ulen == rulen)
				break;
		} else if (cmp > 0)
			break;
	}
	*npp = np;
	return((prev == NULL) ? &hash_tab[idx] : &prev->nxt);
}

/*----------------------------------------------*/
/*		�ꎞ�P��y�[�W�̉���i�f�b�j			*/
/*		�y�[�W���̒P������� NODE ���������B	*/
/*----------------------------------------------*/
void page_gc(void)
{
	uchar *p;
	ULEN ulen;
	UCODE yomi[ULENMAX];
	NODE *np;
	NODE **npp;

	/* ���݂̃y�[�W���I���ɂ���B */
	*temp_top++ = 0;
	*temp_top++ = 0;

	temp_page = (temp_page + 1) % TEMPPAGES;
	temp_top = temp_pages[temp_page];
	temp_end = temp_top + PAGEMAX;
	for (p = temp_top; RECORD_LEN(p); p += RECORD_LEN(p)) {
		if ((ulen=UCODE_LEN(p)) != 0) { 
			/* ulen == 0 �͍폜���ꂽ�ꎞ�P�� */
			strncpy((char *)yomi, (char *)UCODE_STR(p), ulen);
			yomi[ulen] = '\0';
			npp = srch_node(yomi, ulen, &np);
			if (np->ptr == p) {
				*npp = np->nxt;
				free_node(np);
			}
		}
	}
	/* ENTID �e�[�u���� tmp ���N���A���� */
	clear_entid_tmpall(temp_page);
}

/*----------------------------------------------*/
/*		ENTID �Ŏg�� tmp �̍쐬���Q��			*/
/*----------------------------------------------*/
int tmpoffset(uchar *p)
{
	int offset, page;

	offset = p - temp_pages[0];
	page = (offset/PAGEMAX) << 12;
	return(page | (offset % PAGEMAX));
}

uchar *load_temp(int tmp)
{
	uchar *p;

	p = temp_pages[TEMP_PAGE(tmp)] + TEMP_OFFSET(tmp);
	return(p);
}

/*----------------------------------------------*/
/*		�����t�@�C������̃Z�[�u�E���[�h		*/
/*		(1) �ꎞ�P���ǂݍ���� Hash �o�^		*/
/*		(2) �X�V�y�[�W��������������			*/
/*----------------------------------------------*/
int read_temp(void)
{
	uchar *p;
	UCODE yomi[ULENMAX];
	ULEN ulen;
	NODE *ap, *np;
	NODE **npp;
	int page;

	if (seek_dic((long)TEMPOFFSET) == ERR)
		return(ERR);

	if (d_read(dic_fd, temp_pages[0], TEMPSIZE) != TEMPSIZE)
		return(ERR);

	for (page=0; page < TEMPPAGES; page++) {
		for (p = temp_pages[page]; RECORD_LEN(p); p += RECORD_LEN(p)) {
			if ((ulen=UCODE_LEN(p)) != 0) { 
				/* ulen == 0 �͍폜���ꂽ�ꎞ�P�� */
				strncpy((char *)yomi, (char*)UCODE_STR(p), ulen);
				yomi[ulen] = '\0';
				npp = srch_node(yomi, ulen, &np);
				ap = alloc_node();
				ap->ptr = p;
				ap->nxt = np;
				*npp = ap;
			}
		}
		if (temp_page == page) {
			temp_top = p;
			temp_end = temp_pages[page] + PAGEMAX;
		}
	}
	return(OK);
}


int write_temp(void)
{
	if (seek_dic((long)TEMPOFFSET) == ERR)
		return(ERR);

	if (d_write(dic_fd, temp_pages[0], TEMPSIZE) != TEMPSIZE)
		return(ERR);

	return(OK);
}

/*----------------------------------------------*/
/*		�ꎞ�P��̓o�^							*/
/*		(1) �����ǂ݂̒P�ꂪ�Ȃ��Ƃ��͑}��		*/
/*		(2) �����ǂ݂̒P�ꂪ����Ƃ��͍폜���}��*/
/*----------------------------------------------*/
int newwdlen(NEWWD *newp)
{
	int atrlen;

	/* �V�������R�[�h����邽�߂ɕK�v�ȃo�C�g�� */
	if (newp->syntaxinfo.katuyo != 0 || newp->syntax >= 45)
		atrlen = 3;		/* �i��, RTIME, ���p */
	else
		atrlen = 2;		/* �i��, RTIME */
	return(3 + newp->ulen + atrlen + setkbuflen(newp->kbuf));
}

int updwdlen(NEWWD *newp)
{
	int atrlen;

	/* ���łɂ��郌�R�[�h�ɒP���ǉ����邽�߂ɕK�v�ȃo�C�g�� */
	if (newp->syntaxinfo.katuyo != 0 || newp->syntax >= 45)
		atrlen = 3;		/* �i��, RTIME, ���p */
	else
		atrlen = 2;		/* �i��, RTIME */
	return(atrlen + setkbuflen(newp->kbuf));
}

void set_record(uchar *s, int len, NEWWD *newp, int	rtime)
{
	/* RECORD LEN */
	*s++ = (u_char)(len % 256);
	*s++ = (u_char)(len / 256);
	/* ULEN */
	*s++ = (u_char)newp->ulen;
	/* UCODE */
	strncpy((char*)s, (char*)newp->ustr, newp->ulen);
	s += newp->ulen;
	/* SYN */
	*s++ = newp->syntax;
	/* RTIME */
	*s++ = (u_char)rtime;
	/* KATU */
	if (newp->syntaxinfo.katuyo != 0 || newp->syntax >= 45)
		*s++ = newp->syntaxinfo.katuyo;
	/* ���� */
	setkbuf(newp->kbuf, s);
}

void upd_record(uchar *s, int len, NEWWD *newp, int rtime)
{
	int oldlen;

	oldlen = RECORD_LEN(s);
	len += oldlen;
	/* RECORD LEN */
	*s = (u_char)(len % 256);
	*(s+1) = (u_char)(len / 256);
	s += oldlen;
	/* SYN */
	*s++ = newp->syntax;
	/* RTIME */
	*s++ = (u_char)rtime;
	/* KATU */
	if (newp->syntaxinfo.katuyo != 0 || newp->syntax >= 45)
		*s++ = newp->syntaxinfo.katuyo;
	/* ���� */
	setkbuf(newp->kbuf, s);
}

int tmp_touroku(UCODE *yomi, NEWWD *newp, int rtime)
{
	NODE *np, **npp, *ap;
	uchar *p;
	int len;

	temp_updated = YES;
	npp = srch_node(yomi, newp->ulen, &np);
	p = np->ptr;
	/* �����ǂ݂̈ꎞ�P�ꂪ����ꍇ�͍폜���� */
	if (UCODE_LEN(p) == newp->ulen &&
		strncmp((char*)yomi, (char*)UCODE_STR(p), newp->ulen) == 0) { 
		*(p+2) = 0;		/* UCODE_LEN ���O�ɂ��� */
		*npp = np->nxt;
		/* ENTID�e�[�u���ł��̈ꎞ�P��������Ă�����̂��N���A���� */
		clear_entid_tmp(tmpoffset(np->ptr));
		free_node(np);
	}
	ap = alloc_node();
	p = alloc_record(len=newwdlen(newp));
	set_record(p, len, newp, rtime);
	/* GC �̉\��������̂ŁA�����ł��炽�߂� srch_node ����B */
	npp = srch_node(yomi, newp->ulen, &np); 
	ap->ptr = p;
	ap->nxt = np;
	*npp = ap;
	return(OK);
}

/*----------------------------------------------*/
/*		�ꎞ�P��̎������� snssyn & getsyn		*/
/*		�@�\�I�ɂ� main_snssyn -> tmp_snssyn	*/
/*				   main_getsyn -> tmp_getsyn	*/
/*----------------------------------------------*/
int tmp_snssyn(UCODE *yomi, ULEN ulen, RECINFO *recp)
{
	NODE *np;
	uchar *p, *match_rec;
	ULEN usedlen, yomilen, rulen, len;
	int cmp;

	match_rec = 0;
	recp->yomilen = recp->used_ulen = 0;
	usedlen = yomilen = 0;
	np = hash_tab[hashfunc(yomi)];
	for ( ; np != NULL; np=np->nxt) {
		p = np->ptr;
		rulen = UCODE_LEN(p);
		if ((len = (short)prefix(UCODE_STR(p), yomi, rulen)) >= usedlen)
			usedlen = (short)(len + 1);

		if ((cmp=strncmp((char *)UCODE_STR(p), (char *)yomi, rulen)) == 0) {
			if (rulen == ulen)
				return(DIC_HASLONGER);
			yomilen = rulen;
			match_rec = p;	/* �}�b�`�������R�[�h */
		} else if (cmp > 0) {
			if (strncmp((char*)UCODE_STR(p), (char*)yomi, ulen) == 0)
				return(DIC_HASLONGER);
			else
				break;
		}
	}
	if (yomilen == 0) {
		recp->offset = NONE;
		return(DIC_HASNONE);
	}
	recp->page = 0;
	recp->offset = (short)tmpoffset(match_rec);
	recp->yomilen = yomilen;
	recp->used_ulen = usedlen;
	recp->ptr = match_rec;
	return(DIC_HASMATCH);
}

int tmp_getsyn(UCODE *yomi, ULEN ulen, RECINFO *recp)
{
	NODE *np;
	uchar *p;
	ULEN rulen;
	int cmp;

	p = 0;
	np = hash_tab[hashfunc(yomi)];
	for ( ; np != NULL; np=np->nxt) {
		p = np->ptr;
		rulen = UCODE_LEN(p);
		if ((cmp=strncmp((char*)UCODE_STR(p), (char*)yomi, rulen)) == 0) {
			if (rulen == ulen)
				break;	/* ���S�}�b�` */
		} else if (cmp > 0) {
			recp->offset = NONE;
			return(DIC_HASNONE);
		}
	}

	if (np == NULL) {
		recp->offset = NONE;
		return(DIC_HASNONE);
	}
	recp->page = 0;
	recp->offset = (short)tmpoffset(p);
	recp->ptr = p;
	return(DIC_HASMATCH);
}

/*----------------------------------------------*/
/*		�ꎞ�P��̊w�K�i�V�t�g�j				*/
/*		(1) temp_page �ɂ���Ƃ��͉������Ȃ�	*/
/*		(2) �V�t�g���悤�Ƃ��� GC ���N����		*/
/*			�Ȃ��Ȃ��Ă��܂��ꍇ�͒v�����Ȃ�	*/
/*----------------------------------------------*/
void shift_temp(int tmp)
{
	int oldpage, len;
	uchar *p, *q, *old;
	ULEN ulen;
	UCODE yomi[ULENMAX];
	NODE *np;

	if ((oldpage=TEMP_PAGE(tmp)) == temp_page)
		return;

	old = p = load_temp(tmp);
	q = alloc_record(len=RECORD_LEN(p));
	if (oldpage == temp_page)		/* GC �ɂ���ď����� */
		return;
		
	ulen = UCODE_LEN(p);
	strncpy((char*)yomi, (char*)UCODE_STR(p), ulen);
	yomi[ulen] = '\0';
	srch_node(yomi, ulen, &np);
	if (np->ptr == p) {
		np->ptr = q;
		while (len--)	/* ���R�[�h�R�s�[ */
			*q++ = *p++;
			*(old+2) = 0;	/* UCODE_LEN ���O�ɂ��� */
			clear_entid_tmp(tmp);
	}
}

void update_nowtmp(void)
{
	temp_updated = YES;
}

/*----------------------------------------------*/
/*		���p�P�� <-> �����\��					*/
/*----------------------------------------------*/
#define ASCII	0xff

int setkbuflen(uchar *a)
{
	int len = 0;

	while (*a) {
		if (iskanji(*a))
			a += 2;
		else
			a++;
		len += 2;
	}
	return(len);
}


void setkbuf(uchar *a, uchar *w)
{
	while (*a) {
		if (iskanji(*a))
			*w++ = *a++;
		else
			*w++ = ASCII;
		*w++ = *a++;
	}
}

int getkbuflen(uchar *w, uchar *wend)
{
	int len = 0;

	while (w < wend && !IS_SYNTAX(*w)) {
		if (*w == ASCII)
			len++;
		else
			len += 2;
		w += 2;
	}
	return(len);
}

void getkbuf(uchar *a, uchar *w, uchar *wend)
{
	while (w < wend && !IS_SYNTAX(*w)) {
		if (*w == ASCII)
			w++;
		else
			*a++ = *w++;
		*a++ = *w++;
	}
	*a = '\0';
}


/*
**		check kanji code
*/
int iskanji(uchar c)
{
	if (((c >= 0x80) && (c <= 0x9f)) || ((c >= 0xe0) && (c <= 0xfc)))
		return(YES);
	else
		return(NO);
}
