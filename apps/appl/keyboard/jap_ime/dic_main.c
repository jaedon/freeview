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
	dic_main.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include	<string.h>

#include	"askdef.h"
#include	"dic.h"
#include	"dicdef.h"
#include	"dic_main.h"
#include	"dic_page.h"
#include	"dic_temp.h"


HAND	dic_fd = CLOSE; /* �����p�̃t�@�C���|�C���^ */
int		gaku_mode = 0;	/* �w�K���[�h */
int		suji_mode = 0;	/* �����ϊ����[�h */
int		dic_rw = RDWR;	/* �����������݃��[�h */

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_open(dicname)										*/
/*		�@�\�F	�w�莫�����I�[�v������									*/
/*		�����F	char	*dicname;		������							*/
/*		�Ԓl�F	DIC_OK			����I�[�v������						*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISOPEN		���������łɃI�[�v������Ă���			*/
/*				DIC_FATAL		�v���I�Ȏ��Ԃ�����						*/
/*				DIC_NODRIVE		�h���C�u�̏������ł��Ă��Ȃ�			*/
/*				DIC_NOFILE		�����t�@�C�����Ȃ�						*/
/*				DIC_READONLY	�������������݋֎~						*/		
/*		����F	�t�@�C���n���h���͐ÓI�ɕۑ�����						*/
/*		���ӁF	���������w�肳��Ă��Ȃ����� DIC_BADARG ��Ԃ�			*/
/*				������ rw �ŃI�[�v������								*/
/*----------------------------------------------------------------------*/
int dic_open(char *dicname)
{
	int ret;

	if (dic_fd != CLOSE)
		return(DIC_ISOPEN); /* ���łɎ������I�[�v������Ă��� */

	if (*dicname == '\0')
		return(DIC_BADARG); /* ���������w�肳��Ă��Ȃ� */

	/* �����t�@�C�����I�[�v������ */
	set_dicname((uchar *)dicname);
	if ((ret=open_dic()) != OK) {
		/* �����t�@�C���I�[�v���̎��s */
		/* -1 -> DIC_NODRIVE */
		/* -2 -> DIC_NOFILE	 */
		return((ret == -1) ? DIC_NODRIVE : DIC_NOFILE);
	}

	/* �w�b�_��ǂݍ��� */
	if (read_head() == ERR) {
		close_dic();
		return(DIC_FATAL);
	}

	/* �C���f�b�N�X��ǂݍ��� */
	if (read_index() == ERR) {
		close_dic();
		return(DIC_FATAL);
	}

	init_page();
	init_temp();
		
	if (dic_rw == READ)
		return(DIC_READONLY);
	else	/* RDWR */
		return(DIC_OK);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_close()												*/
/*		�@�\�F�@�I�[�v�����̎������N���[�Y����							*/
/*		�����F�@�Ȃ�													*/
/*		�Ԓl�F	DIC_OK			����N���[�Y����						*/
/*				DIC_ISCLOSED	���������łɃN���[�Y����Ă���			*/
/*				DIC_FATAL		�v���I�Ȏ��Ԃ�����						*/
/*----------------------------------------------------------------------*/
int dic_close(void)
{
	if (dic_fd == CLOSE) {
#if 0
		printf("���łɎ������N���[�Y����Ă��� \n");
#endif
		return(DIC_ISCLOSED); /* ���łɎ������N���[�Y����Ă��� */
	}
		
	flush_head();
	flush_temp();
	flush_pages();

	if (close_dic() == ERR) {
#if 0
		printf("�������N���[�Y�ł��Ȃ�����\n");
#endif
		return(DIC_FATAL);
	}

#if 0
		printf("����ɃN���[�Y�ł���\n");
#endif
	/* ����ɃN���[�Y�ł��� */
	return(DIC_OK);
}

/*----------------------------------------------------------------------*/
/*		�֐��F�@dic_snssyn(ustr, ulen, buf)								*/
/*		�@�\�F	�w��ǂ݂ɍŒ��}�b�`�����Q�𓾂�						*/
/*		�����F�@UCODE	ustr[];			�w��ǂ݂̂��R�[�h��			*/
/*				ULEN	ulen;			���̒���						*/
/*				ENTRYS	*buf;			����������Q����Ԃ��o�b�t�@*/
/*		�Ԓl�F�@DIC_HASLONGER	�^����ꂽ�ǂ݂� prefix �Ƃ���ꂪ����	*/
/*				DIC_HASMATCH	�w��ǂ݂ɍŒ��}�b�`�����Q������		*/
/*				DIC_HASNONE		�w��ǂ݂Ƀ}�b�`�����͈���Ȃ�		*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*		���ӁF	ustr[ulen] == 0 �Ƃ͌���Ȃ�							*/
/*----------------------------------------------------------------------*/
int dic_snssyn(UCODE ustr[], ULEN ulen, ENTRYS *buf)
{
	UCODE yomi[ULENMAX];
	RECINFO rec, rec2;
	int ret, ret2;
	int n, maxtime;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	strncpy((char *)yomi, (char *)ustr, ulen);		/* ���͕�������R�s�[ */
	yomi[ulen] = '\0';
	if ((ret=main_snssyn(yomi, ulen, &rec)) == DIC_HASLONGER)
		return(DIC_HASLONGER);

	strncpy((char *)yomi, (char *)ustr, ulen);		/* ���͕�������R�s�[ */
	yomi[ulen] = '\0';
	if ((ret2=tmp_snssyn(yomi, ulen, &rec2)) == DIC_HASLONGER)
		return(DIC_HASLONGER);

	if (ret == DIC_HASNONE && ret2 == DIC_HASNONE)
		return(DIC_HASNONE);

	buf->page_ulen = 1;		/* ���͎g���Ă��Ȃ� */
	maxtime = 0;
	buf->used_ulen = (short)((rec.used_ulen > rec2.used_ulen) ? 
				rec.used_ulen : rec2.used_ulen);
	if (rec.yomilen > rec2.yomilen) {
		buf->yomilen = rec.yomilen;
		buf->synreflen = (short)set_synref(rec.ptr, buf->synref, 0, &maxtime);
		rec2.offset = NONE;
	} else if (rec.yomilen < rec2.yomilen) {
		buf->yomilen = rec2.yomilen;
		buf->synreflen = (short)set_synref(rec2.ptr, buf->synref, 0, &maxtime);
		rec.offset = NONE;
	} else {	/* rec.yomilen == rec2.yomilen */
		buf->yomilen = rec.yomilen;
		n = set_synref(rec.ptr, buf->synref, 0, &maxtime);
		buf->synreflen = (short)set_synref(rec2.ptr, buf->synref, n, &maxtime);
	}

	if ((buf->entid=set_entid_tab(rec.page, rec.offset, 
								rec2.offset, maxtime)) == ERR)
		return(DIC_HASNONE);	/* ENTID�e�[�u���������ς� */

	return(DIC_HASMATCH);
}


int main_snssyn(UCODE *yomi, ULEN ulen, RECINFO *recp)
{
	ULEN yomilen, page_ulen, rulen, usedlen, len;
	int page, cmp;
	int firstpage = YES;
	uchar *top, *p, *match_rec;

	match_rec = 0;
	recp->yomilen = recp->used_ulen = 0;
	page = srch_page(yomi);
	if (page_fix(page, yomi) == NO) /* �y�[�W�m�肵�Ă��邩�H */
		return(DIC_HASLONGER);

	usedlen = 0;	/* ���̒P������肷��̂Ɏg�����ǂ݂̒��� */
	for ( ;; ) {
		p = top = load_page(page);
		page_ulen = (short)calc_pulen(page, yomi);
		yomilen = 0;
		for (; RECORD_LEN(p); p += RECORD_LEN(p)) {
			rulen = UCODE_LEN(p);
			if ((len=(short)prefix(UCODE_STR(p), yomi, rulen)) >= usedlen)
				usedlen = (short)(len+1);
			if ((cmp=strncmp((char *)UCODE_STR(p), (char *)yomi, rulen)) == 0) {
				if (rulen == ulen && firstpage)
					return(DIC_HASLONGER);
				yomilen = rulen;
				match_rec = p;	/* �}�b�`�������R�[�h */
			} else if (cmp > 0) {
				if (strncmp((char *)UCODE_STR(p), (char *)yomi, ulen) == 0 && firstpage)
					return(DIC_HASLONGER);
				else
					break;
			}
		}
		if (yomilen > 0)		/* �}�b�`���� */
			break;
		if (page_ulen <= 1) {
			recp->offset = NONE;
			return(DIC_HASNONE);
		}
		ulen = (short)(page_ulen - 1);
		yomi[ulen] = '\0';
		page = srch_page(yomi);
		firstpage = NO;
	}
	recp->page = (short)page;
	recp->offset = (short)(match_rec - top);
	recp->yomilen = yomilen;
	recp->used_ulen = usedlen;
	recp->ptr = match_rec;
	return(DIC_HASMATCH);
}

/* SYNREF �\���̂ɃZ�b�g	*/
int set_synref(register uchar *p, SYNREF *synp, int count, int *gakup)
{
	register SYNREF *syns;
	uchar *pend;

	pend = p + RECORD_LEN(p);
	p = WD_STR(p);
	syns = synp;
	synp += count;
	while (p < pend) {
		synp->syntax = *p++;
		if ((synp->rtime = *p++) > *gakup)
			*gakup = synp->rtime;
		if (IS_SYNTAXINFO(*p))
			synp->syntaxinfo.setu = *p++;
		else
			synp->syntaxinfo.setu = 0;
		synp->level = 0;		/* ���̂Ƃ���g���Ă��Ȃ� */

		/* �����i�������p�̂Ƃ��͏d�����Ȃ��悤�ɂ��� */
		if (exist_synref(syns, synp) == NO) {
			synp++;
			if (++count >= SYNS)
				break;
		}
		p = next_wd(p, pend);
	}
	return(count);	/* SYNREF �̑�����Ԃ� */
}						

int exist_synref(register SYNREF *s, register SYNREF *p)
{
	for ( ; s < p; s++) {
		if (s->syntax == p->syntax &&
			s->syntaxinfo.setu == p->syntaxinfo.setu) {
			/* RTIME ��傫�����ɂ��� */
			if (s->rtime < p->rtime)
				s->rtime = p->rtime;
			return(YES);
		}
	}
	return(NO);
}

uchar *next_wd(register uchar *p, register uchar *pend)
{
	while (p < pend)
		if (IS_SYNTAX(*p))
			break;
		else
			p += 2;

	return(p);
}

/*----------------------------------------------------------------------*/
/*		�֐��F�@dic_getsyn(ustr, ulen, buf)								*/
/*		�@�\�F	�w��ǂ݂Ɋ��S�}�b�`�����Q�𓾂�						*/
/*		�����F�@UCODE	ustr[];			�w��ǂ݂̂��R�[�h��			*/
/*				ULEN	ulen;			���̒���						*/
/*				ENTRYS	*buf;			����������Q����Ԃ��o�b�t�@*/
/*		�Ԓl�F�@DIC_HASMATCH	�w��ǂ݂ɍŒ��}�b�`�����Q������		*/
/*				DIC_HASNONE		�w��ǂ݂Ƀ}�b�`�����͈���Ȃ�		*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*		���ӁF	ustr[ulen] == 0 �Ƃ͌���Ȃ�							*/
/*				buf �ɒl���Ԃ����ꍇ�́Abuf->yomilen == ulen �ł���	*/
/*----------------------------------------------------------------------*/
int dic_getsyn(UCODE ustr[], ULEN ulen, ENTRYS *buf)
{
	UCODE yomi[ULENMAX];
	RECINFO rec, rec2;
	int ret, ret2;
	int maxtime;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	strncpy((char *)yomi, (char *)ustr, ulen);		/* ���͕�������R�s�[ */
	yomi[ulen] = '\0';

	ret = main_getsyn(yomi, ulen, &rec);
	ret2 = tmp_getsyn(yomi, ulen, &rec2);
	if (ret == DIC_HASNONE && ret2 == DIC_HASNONE)
		return(DIC_HASNONE);
	buf->yomilen = ulen;
	buf->used_ulen = ulen;
	buf->page_ulen = 1;		/* ���͎g���Ă��Ȃ� */
	buf->synreflen = 0;
	maxtime = 0;
	if (ret == DIC_HASMATCH) {
		buf->synreflen = (short)set_synref(rec.ptr, buf->synref, 0, &maxtime);
	}
	if (ret2 == DIC_HASMATCH) {
		buf->synreflen = (short)set_synref(rec2.ptr, buf->synref,
					buf->synreflen, &maxtime);
	}

	if ((buf->entid=set_entid_tab(rec.page, rec.offset, 
								rec2.offset, maxtime)) == ERR)
		return(DIC_HASNONE);	/* ENTID�e�[�u���������ς� */
	return(DIC_HASMATCH);
}

int main_getsyn(UCODE *yomi, ULEN ulen, RECINFO *recp)
{
	ULEN rulen;
	int page, cmp;
	uchar *top, *p;

	if (ulen <= 2 && chk_entry2(yomi, ulen)) {
		recp->offset = NONE;
		return(DIC_HASNONE);
	}
	page = srch_page(yomi);
	p = top = load_page(page);
	for (; RECORD_LEN(p); p += RECORD_LEN(p)) {
		rulen = UCODE_LEN(p);
		if ((cmp=strncmp((char *)UCODE_STR(p), (char *)yomi, rulen)) == 0) {
			if (rulen == ulen)
				break;	/* ���S�}�b�` */
		} else if (cmp > 0) {
			recp->offset = NONE;
			return(DIC_HASNONE);
		}
	}

	if (RECORD_LEN(p) == 0) {
		recp->offset = NONE;
		return(DIC_HASNONE);
	}
	recp->page = (short)page;
	recp->offset = (short)(p - top);
	recp->ptr = p;
	return(DIC_HASMATCH);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_get1wd(entid, syn, ktu, wdsp)						*/
/*		�@�\�F�@�w���Q����w��i���̍ő�D��x�̈��𓾂�			*/
/*		�����F�@ENTID	entid;			���R�[�h�h�c					*/
/*				SYNTAX	syn;			�w��i��						*/
/*				KATUYO	ktu;			�w�芈�p						*/
/*				WDS		*wdsp;			��������i�[����o�b�t�@		*/
/*		�Ԓl�F�@DIC_HASMATCH	�w��̌ꂪ����							*/
/*				DIC_HASNONE		�w��̌�͂Ȃ�							*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*		���ӁF�@wdsp->kbuf[] �̍Ō�ɂ� '\0' ���l�߂�					*/
/*----------------------------------------------------------------------*/

int dic_get1wd(ENTID	entid, SYNTAX	syn, KATUYO	ktu, WDS	*wdsp)
{
	int page, offset, tmp;
	int i, maxtime;
	uchar *p;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	if ((page=get_entid_tab(entid, &offset, &tmp, &i)) == ERR)
		return(DIC_HASNONE);	/* �ʏ�N���Ȃ��͂� */

	maxtime = -1;
	if (offset != NONE) {
		p = load_page(page) + offset;
		get1wd(p, syn, ktu, &maxtime, wdsp, MAIN);
	}
	if (tmp != NONE) {
		p = load_temp(tmp);
		get1wd(p, syn, ktu, &maxtime, wdsp, TEMP);
	}

	if (maxtime == -1)
		return(DIC_HASNONE);	/* �Y������i���̒P�ꂪ�Ȃ� */

	return(DIC_HASMATCH);
}

void get1wd(uchar *top_rec, int syn, int ktu, int *ptime, WDS *wdsp, int mode)
{
	uchar *p, *q, *pend, *wdp;
	int oldtime, wdid, ktu1, rtime;

	wdp = 0;
	pend = top_rec + RECORD_LEN(top_rec);
	p = WD_STR(top_rec);
	oldtime = *ptime;
	while (p < pend) {
		/* SYN GAKU (KATU) */
		q = p;
		rtime = *(p+1);
		p += 2;
		if (IS_SYNTAXINFO(*p)) {
			ktu1 = *p;
			p++;
		} else
			ktu1 = 0;
		/* �i�������p����v���Ă��邩�H */
		if (syn == *q && ktu == ktu1) {
			if (rtime > *ptime) {
				wdp = q;
				*ptime = rtime;
			}
		}
		p = next_wd(p, pend);
	}

	if (wdp != NULL)
	{
		if (*ptime > oldtime) {
			wdid = mode | (wdp - top_rec);
			set_wds(wdsp, wdp, pend, wdid);
		}
	}
}

void set_wds(WDS *wp, uchar *wdp, uchar	*end, int wdid)
{
	wp->wdid = (unsigned short)wdid;
	wp->rtime = *(wdp+1);
	wp->level = 0;			/* ���̂Ƃ���g���Ă��Ȃ� */

	wdp += 2;				/* SYN GAKU (KATU) */
	if (IS_SYNTAXINFO(*wdp))
		wdp++;
	getkbuf(wp->kbuf, wdp, end);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_getallwd(entid, syn, ktu, wdsp, wnp)				*/
/*		�@�\�F�@�w���Q����w��i���̑S����\�[�g���ē���				*/
/*		�����F�@ENTID	entid;			���R�[�h�h�c					*/
/*				SYNTAX	syn;			�w��i��						*/
/*				KATUYO	ktu;			�w�芈�p						*/
/*				WDS		*wdsp;			��������i�[����o�b�t�@		*/
/*				int		*wnp;			�i�[�����ꐔ���i�[����A�h���X	*/
/*		�Ԓl�F�@DIC_HASMATCH	�w��̌ꂪ����							*/
/*				DIC_HASNONE		�w��̌�͂Ȃ�							*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*		����F�@RTIME �Ń\�[�g���D��x�̍����ꂩ�珇�ɋl�߂�			*/
/*				��ƌ�̊Ԃ� '\0' �ŋ�؂���							*/
/*		���ӁF	dic_get1wd() �œ�����ő�D��x�̌���܂߂ĕԂ�		*/
/*				wdsp->kbuf[] �̍Ō�ɂ� '\0' ���l�߂�					*/
/*				�o�b�t�@�ɂ͏\���ȗ̈悪����Ă���Ƃ���				*/
/*----------------------------------------------------------------------*/
int dic_getallwd(ENTID entid, SYNTAX syn, KATUYO ktu, WDS *wdsp, int *wnp)
{
	int page, offset, tmp;
	int cnt, total;
	uchar *p;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	if ((page=get_entid_tab(entid, &offset, &tmp, &cnt)) == ERR)
		return(DIC_HASNONE);	/* �ʏ�N���Ȃ��͂� */

	cnt = 0;		/* WDS[] �̌� */
	total = 0;		/* WDS[] �̑��o�C�g�� */
	if (offset != NONE) {
		p = load_page(page) + offset;
		getallwd(p, syn, ktu, &cnt, &total, wdsp, MAIN);
	}
	if (tmp != NONE) {
		p = load_temp(tmp);
		getallwd(p, syn, ktu, &cnt, &total, wdsp, TEMP);
	}
		
	if ((*wnp = cnt) == 0)
		return(DIC_HASNONE);	/* �Y������i���̒P�ꂪ�Ȃ� */

	return(DIC_HASMATCH);
}

void getallwd(uchar	*top_rec, int syn, int ktu, int *pcnt, int *ptotal, WDS *wdsp, int mode)
{
	uchar *p, *pend, *wdp;
	int rtime, size, wdid, ktu1;
	WDS *w;

	pend = top_rec + RECORD_LEN(top_rec);
	p = WD_STR(top_rec);
	while (p < pend) {
		wdp = p;
		rtime = *(p+1);		/* RTIME */
		p += 2;				/* SYN GAKU (KATU) */
		if (IS_SYNTAXINFO(*p)) {
			ktu1 = *p;
			p++;
		} else
			ktu1 = 0;

		/* �i�������p����v���Ă��邩�H */
		if (syn == *wdp && ktu == ktu1) {
			size = getkbuflen(p, pend) + SIZEWDS + 1;
			if (size & 1)
				size++;
			w = ins_wds(wdsp, rtime, size, *ptotal);
			wdid = mode | (wdp - top_rec);
			set_wds(w, wdp, pend, wdid);
			(*ptotal) += size;
			(*pcnt)++;
		}
		p = next_wd(p, pend);
	}
}

/*		WDS[] �� rtime �ňʒu�𒲂ׂđ}�����邽�߂̋󂫂����	*/
WDS *ins_wds(WDS *wp, int rtime, int size, int total)
{
	WDS *wend;
	uchar *s;

	wend = (WDS*)(((char*)wp) + total);
	while (wp < wend) {
		if (wp->rtime < rtime)
			break;
		for (s=wp->kbuf; *s++; )
			;
		if (((int)s) & 1)
			s++;
		wp = (WDS*)(s);
	}

	if (wp < wend) {		/* �V�t�g����K�v���� */
		s = ((uchar*)wend) - 1;
		while (s >= (uchar*)wp) {
			*(s + size) = *s;
			s--;
		}
	}
	return(wp);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_learn(entid, wdid, entids, n)						*/
/*		�@�\�F	�w�����w�K����										*/
/*		�����F	ENTID	entid;			���R�[�h�h�c					*/
/*				WDID	wdid;			�P��h�c						*/
/*				ENTID	entids[];		�֘A���郌�R�[�h�h�c			*/
/*				int		n;				entids[] �̌�					*/
/*		�Ԓl�F	DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_HASNONE		�w�K���悤�Ƃ����P�ꂪ�Ȃ�����			*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*				DIC_OK			����I��								*/
/*		���ӁF	���R�[�h���ōő�� RTIME �ɐݒ肷��						*/
/*				�ꎞ�P��̊w�K�ł́A���̒P�ꂪ GC ���߂ɂ��łɂȂ�����	*/
/*				����Ȃ�												*/
/*----------------------------------------------------------------------*/
int dic_learn(ENTID	entid, WDID	wdid, ENTID entids[], int n)
{
	int page, offset, tmp;
	int maxtime;
	uchar *p, *top_rec;

	if (gaku_mode == OFF)
		return(DIC_OK);

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	/* �����ϊ��P�� */
	if ((entid == (unsigned long)(-1)) && (wdid <= 3)) {
		suji_mode = wdid;
		clear_allrtime(entids, n);
		update_entid_rtime(entids, n, 0);
		return(DIC_OK);
	}

	if ((page=get_entid_tab(entid, &offset, &tmp, &maxtime)) == ERR)
		return(DIC_HASNONE);	/* �ʏ�N���Ȃ��͂� */

	if (wdid & TEMP) {			/* �ꎞ�P�� */
		if (tmp == NONE)
			return(DIC_HASNONE);
		top_rec = load_temp(tmp);
	} else {					/* ���C������ */
		if (offset == NONE)
			return(DIC_HASNONE);
		top_rec = load_page(page) + offset;
	}
	maxtime = get_maxtime(entids, n);
	p = top_rec + (wdid & ~TEMP) + 1;		/* �w�肳�ꂽ WDID + 1 */
	if (*p && *p == maxtime) {				/* ���łɍő� RTIME �ɂȂ��Ă��� */
		if (wdid & TEMP) {					/* �ꎞ�P������R�[�h�V�t�g���� */
			shift_temp(tmp);
			update_nowtmp();
		}
		return(DIC_OK);
	}

	if (++maxtime == RTIMEMAX) {
		clear_allrtime(entids, n);
		maxtime = 1;
	}
	*p = (uchar)maxtime;
	if (wdid & TEMP) {			/* �ꎞ�P������R�[�h�V�t�g���� */
		shift_temp(tmp);
		update_nowtmp();
	} else {
		update_nowpage();
	}
	update_entid_rtime(entids, n, maxtime);
	return(DIC_OK);
}

/*		�w�肳�ꂽ ENTID �̂��ׂẴ��R�[�h�̂��ׂĂ̒P���		*/
/*		RTIME ���N�����[����									*/
void clear_rtime(uchar *p)
{
	uchar *pend;

	pend = p + RECORD_LEN(p);
	p = WD_STR(p);

	while (p < pend) {
		*(p+1) = 0;		/* RTIME */
		p += 2;			/* SYN GAKU (KATU) */
		if (IS_SYNTAXINFO(*p))
			p++;
		p = next_wd(p, pend);
	}
}


void clear_allrtime(ENTID entids[], int n)
{
	int page, offset, tmp;
	int maxtime;
	int i;

	for (i=0; i<n; i++) {
		if ((page=get_entid_tab(entids[i], &offset, &tmp, &maxtime)) == ERR)
			continue;

		if (offset != NONE) {
			clear_rtime(load_page(page) + offset);
			update_nowpage();
		}
		if (tmp != NONE) {
			clear_rtime(load_temp(tmp));
			update_nowtmp();
		}
	}
}

/*		�w�肳�ꂽ���R�[�h�̂��ׂĂ̒P��̍ő� RTIME ���擾���� */
int max_rtime(uchar *p)
{
	uchar *pend;
	int maxtime = 0;

	pend = p + RECORD_LEN(p);
	p = WD_STR(p);

	while (p < pend) {
		if (maxtime < *(p+1))	/* RTIME */
			maxtime = *(p+1);
		p += 2;			/* SYN GAKU (KATU) */
		if (IS_SYNTAXINFO(*p))
			p++;
		p = next_wd(p, pend);
	}
	return(maxtime);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_touroku(newwdp)										*/
/*		�@�\�F	�w����P��o�^����									*/
/*		�����F	NEWWD	*newwdp;		�o�^�����ɂ��Ă̏��		*/
/*		�Ԓl�F	DIC_OK			����I��								*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*				DIC_OVERFLOW	�y�[�W�̈���z���Ă��܂���				*/
/*				DIC_READONLY	�������݋֎~							*/
/*		���ӁF�@�o�^�����Ɠ����ǂ݂� ENTID, WDID ���擾���Ă��ꍇ		*/
/*				�����͖����ɂȂ�										*/
/*----------------------------------------------------------------------*/
int dic_touroku(NEWWD *newwdp)
{
	UCODE yomi[ULENMAX];
	ULEN rulen;
	int page, cmp;
	uchar *top, *p, *end;
	int len;
	int new=1;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */
	if (dic_rw == READ)
		return(DIC_READONLY);	/* �������������݋֎~ */

	strncpy((char *)yomi, (char *)newwdp->ustr, newwdp->ulen);
	yomi[newwdp->ulen] = '\0';
	page = srch_page(yomi);
	top = load_page(page);
	end = end_page(top);
	for (p=top; p < end; p += RECORD_LEN(p)) {
		rulen = UCODE_LEN(p);
		if ((cmp=strncmp((char *)UCODE_STR(p), (char *)yomi, rulen)) == 0) {
			if (rulen == newwdp->ulen) {	/* ���S�}�b�` */
				new = 0;
				break;
			}
		} else if (cmp > 0)
			break;	
	}
	len = (new) ? newwdlen(newwdp) : updwdlen(newwdp);
	if (end > (top + PAGEMAX - len - 2))
		return(DIC_OVERFLOW);
	if (new) {
		shiftpage(p, end+2, len);
		set_record(p, len, newwdp, 0);
		if (newwdp->ulen <= 2)
			set_entry2(yomi, newwdp->ulen);
	} else {
		shiftpage(p + RECORD_LEN(p), end+2, len);
		upd_record(p, len, newwdp, max_rtime(p)+1);
	}
	init_entid_tab();		/* ENTID�e�[�u���̏����� */
	update_nowpage();
	return(DIC_OK);
}


uchar *end_page(uchar *p)
{
	int len;

	len = RECORD_LEN(p);
	while (len) {
		p += len;
		len = RECORD_LEN(p);
	}
	return(p);
}


void shiftpage(uchar *s, uchar *t, int len)	/* len > 0 forward */
{
	if (len > 0) {
		for (t--; s <= t; t--)
			*(t+len) = *t;
	} else if (len < 0) {
		for (; s < t; s++)
			*(s+len) = *s;
	}
}
				
/*----------------------------------------------------------------------*/
/*		�֐��F	dic_delete(newwdp)										*/
/*		�@�\�F	�w����P��폜����									*/
/*		�����F	NEWWD	*newwdp;		�폜�����ɂ��Ă̏��		*/
/*		�Ԓl�F	DIC_OK			����I��								*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*				DIC_HASNONE		�폜���ׂ��P�ꂪ�Ȃ�����				*/
/*				DIC_READONLY	�������݋֎~							*/
/*		���ӁF�@�폜�����Ɠ����ǂ݂� ENTID, WDID ���擾���Ă��ꍇ		*/
/*				�����͖����ɂȂ�										*/
/*----------------------------------------------------------------------*/
int dic_delete(NEWWD *newwdp)
{
	UCODE yomi[ULENMAX];
	ULEN rulen;
	int page, cmp;
	uchar *p, *end;
	int len;
	int none=1;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */
	if (dic_rw == READ)
		return(DIC_READONLY);	/* �������������݋֎~ */

	strncpy((char *)yomi, (char *)newwdp->ustr, newwdp->ulen);

	yomi[newwdp->ulen] = '\0';
	page = srch_page(yomi);
	p = load_page(page);
	end = end_page(p);
	for ( ; p < end; p += RECORD_LEN(p)) {
		rulen = UCODE_LEN(p);
		if ((cmp=strncmp((char *)UCODE_STR(p), (char *)yomi, rulen)) == 0) {
			if (rulen == newwdp->ulen) {	/* ���S�}�b�` */
				none = 0;
				break;
			}
		} else if (cmp > 0)
			break;
	}
	if (none)
		return(DIC_HASNONE);
	if ((len=delwd(p, newwdp)) == 0)		/* �폜�������� */
		return(DIC_HASNONE);
	if (len == -1)	/* ���R�[�h���ƍ폜 */
		shiftpage(p + RECORD_LEN(p), end+2, -RECORD_LEN(p));
	else
		shiftpage(p + RECORD_LEN(p) + len, end+2, -len);
	init_entid_tab();		/* ENTID�e�[�u���̏����� */
	update_nowpage();
	return(DIC_OK);
}


int delwd(uchar *p, NEWWD *newp)
{
	uchar *top, *pend, *k, *wd;
	int oldlen, len, klen;
	uchar dumywd[128];

	setkbuf(newp->kbuf, dumywd);
	klen = setkbuflen(newp->kbuf);
	dumywd[klen] = '\0';
	top = p;
	oldlen = RECORD_LEN(p);
	pend = top + oldlen;
	p = WD_STR(top);
	while ((wd=p) < pend) {
		p += 2;			/* SYN GAKU (KATU) */
		if (IS_SYNTAXINFO(*p))
			p++;
		k = p;
		p = next_wd(p, pend);
		len = p - k;	/* �ϊ��敶����̒��� */
		if (*wd == newp->syntax &&
					len == klen && strncmp((char *)k, (char *)dumywd, len) == 0)
			break;
	}
	if ((len = p - wd) > 0) { /* �폜����P��̒��� */
		if (wd == WD_STR(top) && p == pend)
			return(-1);
		shiftpage(p, pend, -len);
		oldlen -= len;
		*top = (unsigned char)(oldlen % 256);
		*(top+1) = (unsigned char)(oldlen / 256);
	}
	return(len);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_tmptouroku(newwdp)									*/
/*		�@�\�F	�w�����ꎞ�P��o�^����								*/
/*		�����F	NEWWD	*newwdp;		�o�^�����ɂ��Ă̏��		*/
/*		�Ԓl�F	DIC_OK			����I��								*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*				DIC_OVERFLOW	�y�[�W�̈���z���Ă��܂���				*/
/*		���ӁF�@�o�^�����Ɠ����ǂ݂̈ꎞ�P�ꂪ���łɂ���ꍇ��		*/
/*				�폜�����												*/
/*----------------------------------------------------------------------*/
int dic_tmptouroku(NEWWD *newwdp)
{
	UCODE yomi[ULENMAX];

	if (gaku_mode == OFF)
		return(DIC_OK);

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	strncpy((char *)yomi, (char *)newwdp->ustr, newwdp->ulen);
	if (newwdp->syntax == 40 || newwdp->syntax == 41)
		newwdp->syntax = 39;	/* ���p�� */
	yomi[newwdp->ulen] = '\0';
	tmp_touroku(yomi, newwdp, 1);
	return(DIC_OK);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_newlearn(newwdp, entids, n)							*/
/*		�@�\�F	�w�����ꎞ�P��w�K����								*/
/*		�����F	NEWWD	*newwdp;		�o�^�����ɂ��Ă̏��		*/
/*				ENTID	entids[];		�֘A���郌�R�[�h�h�c			*/
/*				int		n;				entids[] �̌�					*/
/*		�Ԓl�F	DIC_OK			����I��								*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*		���ӁF�@�o�^�����Ɠ����ǂ݂̈ꎞ�P�ꂪ���łɂ���ꍇ��		*/
/*				�폜�����												*/
/*				�֘A���郌�R�[�h���ōő�� RTIME �ɐݒ肷��				*/
/*----------------------------------------------------------------------*/
int dic_newlearn(NEWWD *newwdp, ENTID entids[], int n)
{
	UCODE yomi[ULENMAX];
	int maxtime;

	if (gaku_mode == OFF)
		return(DIC_OK);

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	strncpy((char *)yomi, (char *)newwdp->ustr, newwdp->ulen);

	yomi[newwdp->ulen] = '\0';
	if (isnum(yomi))
		newwdp->syntax = 31;	/* �����E���\�� */
	maxtime = get_maxtime(entids, n);
	if (++maxtime == RTIMEMAX) {
		clear_allrtime(entids, n);
		maxtime = 1;
	}
	tmp_touroku(yomi, newwdp, maxtime);
	update_entid_rtime(entids, n, maxtime);
	return(DIC_OK);
}

int isnum(UCODE	*s)
{
	for ( ; *s; s++)
		if (*s < 0x30 || *s > 0x39)
			return(NO);
	return(YES);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_freeentid(entid)									*/
/*		�@�\�F	���R�[�h�h�c�̉��										*/
/*		�����F	ENTID	entid;			���R�[�h�h�c					*/
/*		�Ԓl�F	DIC_OK			����I��								*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*----------------------------------------------------------------------*/
int dic_freeentid(ENTID	entid)
{
	free_entid_tab(entid);
	return(DIC_OK);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_setgaku(mode)										*/
/*		�@�\�F	�w�K���[�h�̐ݒ�										*/
/*		�����F	int		mode;			�w�K���[�h�iON/OFF�j			*/
/*		�Ԓl�F	DIC_OK			����I��								*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*		���ӁF�@mode = 0/1 �Ńf�B�X�N�w�K�Ȃ��^����						*/
/*				������Ԃ̓f�B�X�N�w�K�Ȃ�								*/
/*----------------------------------------------------------------------*/
int dic_setgaku(int mode)
{
	/* �������I�[�v������Ă��Ȃ��Ă��ݒ�ł��� */
	if (mode == ON || mode == OFF) {
		gaku_mode = mode;
		return(DIC_OK);
	} else
		return(DIC_BADARG);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_getgaku()											*/
/*		�@�\�F	�w�K���[�h�̎擾										*/
/*		�����F	�Ȃ�													*/
/*		�Ԓl�F	�w�K���[�h												*/
/*		���ӁF�@mode = 0/1 �Ńf�B�X�N�w�K�Ȃ��^����						*/
/*----------------------------------------------------------------------*/
int dic_getgaku(void)
{
	return(gaku_mode);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_get1num(ustr, ulen, wdsp)							*/
/*		�@�\�F�@�w�肵����̐����ϊ���𓾂�							*/
/*		�����F�@UCODE	ustr[];			�w��ǂ݂̂��R�[�h��			*/
/*				ULEN	ulen;			���̒���						*/
/*				WDS		*wdsp;			��������i�[����o�b�t�@		*/
/*		�Ԓl�F�@DIC_HASMATCH	�w��̌ꂪ����							*/
/*				DIC_HASNONE		�w��̌�͂Ȃ�							*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*		���ӁF�@wdsp->kbuf[] �̍Ō�ɂ� '\0' ���l�߂�					*/
/*----------------------------------------------------------------------*/
int dic_get1num(UCODE ustr[], ULEN ulen, WDS *wdsp)
{
	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	if (set_num(ustr, ulen, wdsp, suji_mode) == (WDS*)NULL) {
		set_num(ustr, ulen, wdsp, 0);
	}
	return(DIC_HASMATCH);
}

#ifdef ASK_SJIS
static uchar num_chars3[] = {
/* �E���Q�\�S�疜���� */
	0x8F, 0x45, 0x88, 0xEB, 0x93, 0xF3, 0x8E, 0x51, 0x8F, 0x5C, 0x95, 0x53, 0x90, 0xE7, 0x96, 0x9C,
	0x89, 0xAD, 0x92, 0x9B,
};

/*
�@�R�����g���ɂ���ʂ�A�@��Ɉˑ����邽�߁A
�@warning�͋����I�ɍ폜���Ă���B
*/
static	uchar	*num_chars[] = {
		(uchar*)"�O�P�Q�R�S�T�U�V�W�X",		/* Hitachi��brain-damaged�ȃR���p�C�����G���[�ɂ���̂�... */
		(uchar*)"�Z���O�l�ܘZ������",		/* 1999/3/10 N. Kimura */
		/* (uchar*) */ num_chars3,
};

#else
static	uchar	*num_chars[] = {
		(uchar*)"�O�P�Q�R�S�T�U�V�W�X",
		(uchar*)"�Z���O�l�ܘZ������",
		(uchar*)"�E���Q�\�S�疜����"
};
#endif

WDS *set_num(UCODE ustr[], ULEN ulen, WDS *wdp, int type)
{
	uchar *s, *u;
	int i;
	int p, n, t, mod;

	if (type >= 2 && ulen > 13)		/* 13���ȏ� */
		return((WDS*)NULL);

	wdp->wdid = (unsigned short)type;
	wdp->rtime = 0;
	wdp->level = 0;					/* ���̂Ƃ���g���Ă��Ȃ� */
	u = s = wdp->kbuf;
	for (i = 0; i < ulen; i++) {
		n = ustr[i] - 0x30;			/* UCODE '0' */
		if (type < 2) {
			*s++ = num_chars[type][n*2];
			*s++ = num_chars[type][n*2 + 1];
		} else {
			p = ulen - i - 1;
			mod = p % 4;
			if (n == 0) 
				;
			else if (type == 2 && n == 1 && (mod == 1 || mod == 2))
				/* �S�\ */
				;
			else {
				t = (n >= 1 && n <= 3) ? type-1 : 1;
				*s++ = num_chars[t][n*2];
				*s++ = num_chars[t][n*2 + 1];
			}
			if (p == 0 || (mod == 0 && u == s) ||
					(mod != 0 && n == 0))
				continue;
						
			if (mod == 0)					/* ������ */
				n = 6 + p/4;
			else if (mod == 1)				/* �\�E */
				n = (type == 2) ? 4 : 0;
			else /* mod == 2 or 3 */		/* �S�� */
				n = 3+ mod;
			*s++ = num_chars[2][n*2];
			*s++ = num_chars[2][n*2 + 1];
			if (mod == 0)
				u = s;
		}
	}		
	if (s == wdp->kbuf)		/* �O�O...�O�O�O */
		return((WDS*)NULL);
	*s = '\0';
	return((WDS*)(s+1));
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_getallnum(ustr, ulen, wdsp, wnp)					*/
/*		�@�\�F�@�w�肵����̂��ׂĂ̐����ϊ���𓾂�					*/
/*		�����F�@UCODE	ustr[];			�w��ǂ݂̂��R�[�h��			*/
/*				ULEN	ulen;			���̒���						*/
/*				WDS		*wdsp;			��������i�[����o�b�t�@		*/
/*				int		*wnp;			�i�[�����ꐔ���i�[����A�h���X	*/
/*		�Ԓl�F�@DIC_HASMATCH	�w��̌ꂪ����							*/
/*				DIC_HASNONE		�w��̌�͂Ȃ�							*/
/*				DIC_BADARG		�����ɉ������ُ̈킪����				*/
/*				DIC_ISCLOSED	�������I�[�v������Ă��Ȃ�				*/
/*				DIC_FATAL		���������Ă���Ȃǂ̒v���I�ȃG���[	*/
/*		���ӁF�@wdsp->kbuf[] �̍Ō�ɂ� '\0' ���l�߂�					*/
/*----------------------------------------------------------------------*/
int dic_getallnum(UCODE ustr[], ULEN ulen, WDS *wdsp, int *wnp)
{
	WDS *w;
	int i;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* �������I�[�v������Ă��Ȃ� */

	*wnp = 0;
	if ((w=set_num(ustr, ulen, wdsp, suji_mode)) != (WDS*)NULL) {
		wdsp = w;
		(*wnp)++;
	}
	for (i=0; i<4; i++) {
		if (i == suji_mode)
			continue;
		if ((w=set_num(ustr, ulen, wdsp, i)) != (WDS*)NULL) {
			wdsp = w;
			(*wnp)++;
		}
	}
	return(DIC_HASMATCH);
}

/*----------------------------------------------------------------------*/
/*		�֐��F	dic_clearmem()											*/
/*		�@�\�F�@														*/
/*		�����F	�Ȃ�													*/
/*		���ӁF�@														*/
/*----------------------------------------------------------------------*/
int dic_clearmem(void)
{
	return(DIC_OK);
}

int ask_strncmp(const char *s1, const char *s2, int n)
{
	int d;

	while (n--) {
		d = (unsigned char)*s1 - (unsigned char)*s2;
		if (d) {
			return d;
		} else if (*s1 == 0) {
			return 0;
		}
		s1++;
		s2++;
	}
	return 0;
}
