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


HAND	dic_fd = CLOSE; /* 辞書用のファイルポインタ */
int		gaku_mode = 0;	/* 学習モード */
int		suji_mode = 0;	/* 数字変換モード */
int		dic_rw = RDWR;	/* 辞書書き込みモード */

/*----------------------------------------------------------------------*/
/*		関数：	dic_open(dicname)										*/
/*		機能：	指定辞書をオープンする									*/
/*		引数：	char	*dicname;		辞書名							*/
/*		返値：	DIC_OK			正常オープンした						*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISOPEN		辞書がすでにオープンされている			*/
/*				DIC_FATAL		致命的な事態が発生						*/
/*				DIC_NODRIVE		ドライブの準備ができていない			*/
/*				DIC_NOFILE		辞書ファイルがない						*/
/*				DIC_READONLY	辞書が書き込み禁止						*/		
/*		解説：	ファイルハンドルは静的に保存する						*/
/*		注意：	辞書名が指定されていない時は DIC_BADARG を返す			*/
/*				辞書は rw でオープンする								*/
/*----------------------------------------------------------------------*/
int dic_open(char *dicname)
{
	int ret;

	if (dic_fd != CLOSE)
		return(DIC_ISOPEN); /* すでに辞書がオープンされている */

	if (*dicname == '\0')
		return(DIC_BADARG); /* 辞書名が指定されていない */

	/* 辞書ファイルをオープンする */
	set_dicname((uchar *)dicname);
	if ((ret=open_dic()) != OK) {
		/* 辞書ファイルオープンの失敗 */
		/* -1 -> DIC_NODRIVE */
		/* -2 -> DIC_NOFILE	 */
		return((ret == -1) ? DIC_NODRIVE : DIC_NOFILE);
	}

	/* ヘッダを読み込む */
	if (read_head() == ERR) {
		close_dic();
		return(DIC_FATAL);
	}

	/* インデックスを読み込む */
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
/*		関数：	dic_close()												*/
/*		機能：　オープン中の辞書をクローズする							*/
/*		引数：　なし													*/
/*		返値：	DIC_OK			正常クローズした						*/
/*				DIC_ISCLOSED	辞書がすでにクローズされている			*/
/*				DIC_FATAL		致命的な事態が発生						*/
/*----------------------------------------------------------------------*/
int dic_close(void)
{
	if (dic_fd == CLOSE) {
#if 0
		printf("すでに辞書がクローズされている \n");
#endif
		return(DIC_ISCLOSED); /* すでに辞書がクローズされている */
	}
		
	flush_head();
	flush_temp();
	flush_pages();

	if (close_dic() == ERR) {
#if 0
		printf("辞書をクローズできなかった\n");
#endif
		return(DIC_FATAL);
	}

#if 0
		printf("正常にクローズできた\n");
#endif
	/* 正常にクローズできた */
	return(DIC_OK);
}

/*----------------------------------------------------------------------*/
/*		関数：　dic_snssyn(ustr, ulen, buf)								*/
/*		機能：	指定読みに最長マッチする語群を得る						*/
/*		引数：　UCODE	ustr[];			指定読みのｕコード列			*/
/*				ULEN	ulen;			その長さ						*/
/*				ENTRYS	*buf;			見つかった語群情報を返すバッファ*/
/*		返値：　DIC_HASLONGER	与えられた読みを prefix とする語がある	*/
/*				DIC_HASMATCH	指定読みに最長マッチする語群がある		*/
/*				DIC_HASNONE		指定読みにマッチする語は一つもない		*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*		注意：	ustr[ulen] == 0 とは限らない							*/
/*----------------------------------------------------------------------*/
int dic_snssyn(UCODE ustr[], ULEN ulen, ENTRYS *buf)
{
	UCODE yomi[ULENMAX];
	RECINFO rec, rec2;
	int ret, ret2;
	int n, maxtime;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

	strncpy((char *)yomi, (char *)ustr, ulen);		/* 入力文字列をコピー */
	yomi[ulen] = '\0';
	if ((ret=main_snssyn(yomi, ulen, &rec)) == DIC_HASLONGER)
		return(DIC_HASLONGER);

	strncpy((char *)yomi, (char *)ustr, ulen);		/* 入力文字列をコピー */
	yomi[ulen] = '\0';
	if ((ret2=tmp_snssyn(yomi, ulen, &rec2)) == DIC_HASLONGER)
		return(DIC_HASLONGER);

	if (ret == DIC_HASNONE && ret2 == DIC_HASNONE)
		return(DIC_HASNONE);

	buf->page_ulen = 1;		/* 今は使っていない */
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
		return(DIC_HASNONE);	/* ENTIDテーブルがいっぱい */

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
	if (page_fix(page, yomi) == NO) /* ページ確定しているか？ */
		return(DIC_HASLONGER);

	usedlen = 0;	/* その単語を決定するのに使った読みの長さ */
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
				match_rec = p;	/* マッチしたレコード */
			} else if (cmp > 0) {
				if (strncmp((char *)UCODE_STR(p), (char *)yomi, ulen) == 0 && firstpage)
					return(DIC_HASLONGER);
				else
					break;
			}
		}
		if (yomilen > 0)		/* マッチした */
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

/* SYNREF 構造体にセット	*/
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
		synp->level = 0;		/* 今のところ使っていない */

		/* 同じ品詞＆活用のときは重複しないようにする */
		if (exist_synref(syns, synp) == NO) {
			synp++;
			if (++count >= SYNS)
				break;
		}
		p = next_wd(p, pend);
	}
	return(count);	/* SYNREF の総数を返す */
}						

int exist_synref(register SYNREF *s, register SYNREF *p)
{
	for ( ; s < p; s++) {
		if (s->syntax == p->syntax &&
			s->syntaxinfo.setu == p->syntaxinfo.setu) {
			/* RTIME を大きい方にする */
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
/*		関数：　dic_getsyn(ustr, ulen, buf)								*/
/*		機能：	指定読みに完全マッチする語群を得る						*/
/*		引数：　UCODE	ustr[];			指定読みのｕコード列			*/
/*				ULEN	ulen;			その長さ						*/
/*				ENTRYS	*buf;			見つかった語群情報を返すバッファ*/
/*		返値：　DIC_HASMATCH	指定読みに最長マッチする語群がある		*/
/*				DIC_HASNONE		指定読みにマッチする語は一つもない		*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*		注意：	ustr[ulen] == 0 とは限らない							*/
/*				buf に値が返される場合は、buf->yomilen == ulen である	*/
/*----------------------------------------------------------------------*/
int dic_getsyn(UCODE ustr[], ULEN ulen, ENTRYS *buf)
{
	UCODE yomi[ULENMAX];
	RECINFO rec, rec2;
	int ret, ret2;
	int maxtime;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

	strncpy((char *)yomi, (char *)ustr, ulen);		/* 入力文字列をコピー */
	yomi[ulen] = '\0';

	ret = main_getsyn(yomi, ulen, &rec);
	ret2 = tmp_getsyn(yomi, ulen, &rec2);
	if (ret == DIC_HASNONE && ret2 == DIC_HASNONE)
		return(DIC_HASNONE);
	buf->yomilen = ulen;
	buf->used_ulen = ulen;
	buf->page_ulen = 1;		/* 今は使っていない */
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
		return(DIC_HASNONE);	/* ENTIDテーブルがいっぱい */
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
				break;	/* 完全マッチ */
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
/*		関数：	dic_get1wd(entid, syn, ktu, wdsp)						*/
/*		機能：　指定語群から指定品詞の最大優先度の一語を得る			*/
/*		引数：　ENTID	entid;			レコードＩＤ					*/
/*				SYNTAX	syn;			指定品詞						*/
/*				KATUYO	ktu;			指定活用						*/
/*				WDS		*wdsp;			得た語を格納するバッファ		*/
/*		返値：　DIC_HASMATCH	指定の語がある							*/
/*				DIC_HASNONE		指定の語はない							*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*		注意：　wdsp->kbuf[] の最後には '\0' を詰める					*/
/*----------------------------------------------------------------------*/

int dic_get1wd(ENTID	entid, SYNTAX	syn, KATUYO	ktu, WDS	*wdsp)
{
	int page, offset, tmp;
	int i, maxtime;
	uchar *p;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

	if ((page=get_entid_tab(entid, &offset, &tmp, &i)) == ERR)
		return(DIC_HASNONE);	/* 通常起きないはず */

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
		return(DIC_HASNONE);	/* 該当する品詞の単語がない */

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
		/* 品詞＆活用が一致しているか？ */
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
	wp->level = 0;			/* 今のところ使っていない */

	wdp += 2;				/* SYN GAKU (KATU) */
	if (IS_SYNTAXINFO(*wdp))
		wdp++;
	getkbuf(wp->kbuf, wdp, end);
}

/*----------------------------------------------------------------------*/
/*		関数：	dic_getallwd(entid, syn, ktu, wdsp, wnp)				*/
/*		機能：　指定語群から指定品詞の全語をソートして得る				*/
/*		引数：　ENTID	entid;			レコードＩＤ					*/
/*				SYNTAX	syn;			指定品詞						*/
/*				KATUYO	ktu;			指定活用						*/
/*				WDS		*wdsp;			得た語を格納するバッファ		*/
/*				int		*wnp;			格納した語数を格納するアドレス	*/
/*		返値：　DIC_HASMATCH	指定の語がある							*/
/*				DIC_HASNONE		指定の語はない							*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*		解説：　RTIME でソートし優先度の高い語から順に詰める			*/
/*				語と語の間は '\0' で区切られる							*/
/*		注意：	dic_get1wd() で得られる最大優先度の語も含めて返す		*/
/*				wdsp->kbuf[] の最後には '\0' を詰める					*/
/*				バッファには十分な領域が取られているとする				*/
/*----------------------------------------------------------------------*/
int dic_getallwd(ENTID entid, SYNTAX syn, KATUYO ktu, WDS *wdsp, int *wnp)
{
	int page, offset, tmp;
	int cnt, total;
	uchar *p;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

	if ((page=get_entid_tab(entid, &offset, &tmp, &cnt)) == ERR)
		return(DIC_HASNONE);	/* 通常起きないはず */

	cnt = 0;		/* WDS[] の個数 */
	total = 0;		/* WDS[] の総バイト数 */
	if (offset != NONE) {
		p = load_page(page) + offset;
		getallwd(p, syn, ktu, &cnt, &total, wdsp, MAIN);
	}
	if (tmp != NONE) {
		p = load_temp(tmp);
		getallwd(p, syn, ktu, &cnt, &total, wdsp, TEMP);
	}
		
	if ((*wnp = cnt) == 0)
		return(DIC_HASNONE);	/* 該当する品詞の単語がない */

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

		/* 品詞＆活用が一致しているか？ */
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

/*		WDS[] に rtime で位置を調べて挿入するための空きを作る	*/
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

	if (wp < wend) {		/* シフトする必要あり */
		s = ((uchar*)wend) - 1;
		while (s >= (uchar*)wp) {
			*(s + size) = *s;
			s--;
		}
	}
	return(wp);
}

/*----------------------------------------------------------------------*/
/*		関数：	dic_learn(entid, wdid, entids, n)						*/
/*		機能：	指定語を学習する										*/
/*		引数：	ENTID	entid;			レコードＩＤ					*/
/*				WDID	wdid;			単語ＩＤ						*/
/*				ENTID	entids[];		関連するレコードＩＤ			*/
/*				int		n;				entids[] の個数					*/
/*		返値：	DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_HASNONE		学習しようとした単語がなかった			*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*				DIC_OK			正常終了								*/
/*		注意：	レコード内で最大の RTIME に設定する						*/
/*				一時単語の学習では、その単語が GC ためにすでにないかも	*/
/*				しれない												*/
/*----------------------------------------------------------------------*/
int dic_learn(ENTID	entid, WDID	wdid, ENTID entids[], int n)
{
	int page, offset, tmp;
	int maxtime;
	uchar *p, *top_rec;

	if (gaku_mode == OFF)
		return(DIC_OK);

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

	/* 数字変換単語 */
	if ((entid == (unsigned long)(-1)) && (wdid <= 3)) {
		suji_mode = wdid;
		clear_allrtime(entids, n);
		update_entid_rtime(entids, n, 0);
		return(DIC_OK);
	}

	if ((page=get_entid_tab(entid, &offset, &tmp, &maxtime)) == ERR)
		return(DIC_HASNONE);	/* 通常起きないはず */

	if (wdid & TEMP) {			/* 一時単語 */
		if (tmp == NONE)
			return(DIC_HASNONE);
		top_rec = load_temp(tmp);
	} else {					/* メイン辞書 */
		if (offset == NONE)
			return(DIC_HASNONE);
		top_rec = load_page(page) + offset;
	}
	maxtime = get_maxtime(entids, n);
	p = top_rec + (wdid & ~TEMP) + 1;		/* 指定された WDID + 1 */
	if (*p && *p == maxtime) {				/* すでに最大 RTIME になっている */
		if (wdid & TEMP) {					/* 一時単語をレコードシフトする */
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
	if (wdid & TEMP) {			/* 一時単語をレコードシフトする */
		shift_temp(tmp);
		update_nowtmp();
	} else {
		update_nowpage();
	}
	update_entid_rtime(entids, n, maxtime);
	return(DIC_OK);
}

/*		指定された ENTID のすべてのレコードのすべての単語の		*/
/*		RTIME をクリヤーする									*/
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

/*		指定されたレコードのすべての単語の最大 RTIME を取得する */
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
/*		関数：	dic_touroku(newwdp)										*/
/*		機能：	指定語を単語登録する									*/
/*		引数：	NEWWD	*newwdp;		登録する語についての情報		*/
/*		返値：	DIC_OK			正常終了								*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*				DIC_OVERFLOW	ページ領域を越えてしまった				*/
/*				DIC_READONLY	書き込み禁止							*/
/*		注意：　登録する語と同じ読みで ENTID, WDID を取得してい場合		*/
/*				それらは無効になる										*/
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
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */
	if (dic_rw == READ)
		return(DIC_READONLY);	/* 辞書が書き込み禁止 */

	strncpy((char *)yomi, (char *)newwdp->ustr, newwdp->ulen);
	yomi[newwdp->ulen] = '\0';
	page = srch_page(yomi);
	top = load_page(page);
	end = end_page(top);
	for (p=top; p < end; p += RECORD_LEN(p)) {
		rulen = UCODE_LEN(p);
		if ((cmp=strncmp((char *)UCODE_STR(p), (char *)yomi, rulen)) == 0) {
			if (rulen == newwdp->ulen) {	/* 完全マッチ */
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
	init_entid_tab();		/* ENTIDテーブルの初期化 */
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
/*		関数：	dic_delete(newwdp)										*/
/*		機能：	指定語を単語削除する									*/
/*		引数：	NEWWD	*newwdp;		削除する語についての情報		*/
/*		返値：	DIC_OK			正常終了								*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*				DIC_HASNONE		削除すべき単語がなかった				*/
/*				DIC_READONLY	書き込み禁止							*/
/*		注意：　削除する語と同じ読みで ENTID, WDID を取得してい場合		*/
/*				それらは無効になる										*/
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
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */
	if (dic_rw == READ)
		return(DIC_READONLY);	/* 辞書が書き込み禁止 */

	strncpy((char *)yomi, (char *)newwdp->ustr, newwdp->ulen);

	yomi[newwdp->ulen] = '\0';
	page = srch_page(yomi);
	p = load_page(page);
	end = end_page(p);
	for ( ; p < end; p += RECORD_LEN(p)) {
		rulen = UCODE_LEN(p);
		if ((cmp=strncmp((char *)UCODE_STR(p), (char *)yomi, rulen)) == 0) {
			if (rulen == newwdp->ulen) {	/* 完全マッチ */
				none = 0;
				break;
			}
		} else if (cmp > 0)
			break;
	}
	if (none)
		return(DIC_HASNONE);
	if ((len=delwd(p, newwdp)) == 0)		/* 削除した長さ */
		return(DIC_HASNONE);
	if (len == -1)	/* レコードごと削除 */
		shiftpage(p + RECORD_LEN(p), end+2, -RECORD_LEN(p));
	else
		shiftpage(p + RECORD_LEN(p) + len, end+2, -len);
	init_entid_tab();		/* ENTIDテーブルの初期化 */
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
		len = p - k;	/* 変換先文字列の長さ */
		if (*wd == newp->syntax &&
					len == klen && strncmp((char *)k, (char *)dumywd, len) == 0)
			break;
	}
	if ((len = p - wd) > 0) { /* 削除する単語の長さ */
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
/*		関数：	dic_tmptouroku(newwdp)									*/
/*		機能：	指定語を一時単語登録する								*/
/*		引数：	NEWWD	*newwdp;		登録する語についての情報		*/
/*		返値：	DIC_OK			正常終了								*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*				DIC_OVERFLOW	ページ領域を越えてしまった				*/
/*		注意：　登録する語と同じ読みの一時単語がすでにある場合は		*/
/*				削除される												*/
/*----------------------------------------------------------------------*/
int dic_tmptouroku(NEWWD *newwdp)
{
	UCODE yomi[ULENMAX];

	if (gaku_mode == OFF)
		return(DIC_OK);

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

	strncpy((char *)yomi, (char *)newwdp->ustr, newwdp->ulen);
	if (newwdp->syntax == 40 || newwdp->syntax == 41)
		newwdp->syntax = 39;	/* 慣用句 */
	yomi[newwdp->ulen] = '\0';
	tmp_touroku(yomi, newwdp, 1);
	return(DIC_OK);
}

/*----------------------------------------------------------------------*/
/*		関数：	dic_newlearn(newwdp, entids, n)							*/
/*		機能：	指定語を一時単語学習する								*/
/*		引数：	NEWWD	*newwdp;		登録する語についての情報		*/
/*				ENTID	entids[];		関連するレコードＩＤ			*/
/*				int		n;				entids[] の個数					*/
/*		返値：	DIC_OK			正常終了								*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*		注意：　登録する語と同じ読みの一時単語がすでにある場合は		*/
/*				削除される												*/
/*				関連するレコード内で最大の RTIME に設定する				*/
/*----------------------------------------------------------------------*/
int dic_newlearn(NEWWD *newwdp, ENTID entids[], int n)
{
	UCODE yomi[ULENMAX];
	int maxtime;

	if (gaku_mode == OFF)
		return(DIC_OK);

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

	strncpy((char *)yomi, (char *)newwdp->ustr, newwdp->ulen);

	yomi[newwdp->ulen] = '\0';
	if (isnum(yomi))
		newwdp->syntax = 31;	/* 数字・数表現 */
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
/*		関数：	dic_freeentid(entid)									*/
/*		機能：	レコードＩＤの解放										*/
/*		引数：	ENTID	entid;			レコードＩＤ					*/
/*		返値：	DIC_OK			正常終了								*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*----------------------------------------------------------------------*/
int dic_freeentid(ENTID	entid)
{
	free_entid_tab(entid);
	return(DIC_OK);
}

/*----------------------------------------------------------------------*/
/*		関数：	dic_setgaku(mode)										*/
/*		機能：	学習モードの設定										*/
/*		引数：	int		mode;			学習モード（ON/OFF）			*/
/*		返値：	DIC_OK			正常終了								*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*		注意：　mode = 0/1 でディスク学習なし／あり						*/
/*				初期状態はディスク学習なし								*/
/*----------------------------------------------------------------------*/
int dic_setgaku(int mode)
{
	/* 辞書がオープンされていなくても設定できる */
	if (mode == ON || mode == OFF) {
		gaku_mode = mode;
		return(DIC_OK);
	} else
		return(DIC_BADARG);
}

/*----------------------------------------------------------------------*/
/*		関数：	dic_getgaku()											*/
/*		機能：	学習モードの取得										*/
/*		引数：	なし													*/
/*		返値：	学習モード												*/
/*		注意：　mode = 0/1 でディスク学習なし／あり						*/
/*----------------------------------------------------------------------*/
int dic_getgaku(void)
{
	return(gaku_mode);
}

/*----------------------------------------------------------------------*/
/*		関数：	dic_get1num(ustr, ulen, wdsp)							*/
/*		機能：　指定した語の数字変換語を得る							*/
/*		引数：　UCODE	ustr[];			指定読みのｕコード列			*/
/*				ULEN	ulen;			その長さ						*/
/*				WDS		*wdsp;			得た語を格納するバッファ		*/
/*		返値：　DIC_HASMATCH	指定の語がある							*/
/*				DIC_HASNONE		指定の語はない							*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*		注意：　wdsp->kbuf[] の最後には '\0' を詰める					*/
/*----------------------------------------------------------------------*/
int dic_get1num(UCODE ustr[], ULEN ulen, WDS *wdsp)
{
	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

	if (set_num(ustr, ulen, wdsp, suji_mode) == (WDS*)NULL) {
		set_num(ustr, ulen, wdsp, 0);
	}
	return(DIC_HASMATCH);
}

#ifdef ASK_SJIS
static uchar num_chars3[] = {
/* 拾壱弐参十百千万億兆 */
	0x8F, 0x45, 0x88, 0xEB, 0x93, 0xF3, 0x8E, 0x51, 0x8F, 0x5C, 0x95, 0x53, 0x90, 0xE7, 0x96, 0x9C,
	0x89, 0xAD, 0x92, 0x9B,
};

/*
　コメント情報にある通り、機種に依存するため、
　warningは強制的に削除している。
*/
static	uchar	*num_chars[] = {
		(uchar*)"０１２３４５６７８９",		/* Hitachiのbrain-damagedなコンパイラがエラーにするので... */
		(uchar*)"〇一二三四五六七八九",		/* 1999/3/10 N. Kimura */
		/* (uchar*) */ num_chars3,
};

#else
static	uchar	*num_chars[] = {
		(uchar*)"０１２３４５６７８９",
		(uchar*)"〇一二三四五六七八九",
		(uchar*)"拾壱弐参十百千万億兆"
};
#endif

WDS *set_num(UCODE ustr[], ULEN ulen, WDS *wdp, int type)
{
	uchar *s, *u;
	int i;
	int p, n, t, mod;

	if (type >= 2 && ulen > 13)		/* 13桁以上 */
		return((WDS*)NULL);

	wdp->wdid = (unsigned short)type;
	wdp->rtime = 0;
	wdp->level = 0;					/* 今のところ使っていない */
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
				/* 百十 */
				;
			else {
				t = (n >= 1 && n <= 3) ? type-1 : 1;
				*s++ = num_chars[t][n*2];
				*s++ = num_chars[t][n*2 + 1];
			}
			if (p == 0 || (mod == 0 && u == s) ||
					(mod != 0 && n == 0))
				continue;
						
			if (mod == 0)					/* 万億兆 */
				n = 6 + p/4;
			else if (mod == 1)				/* 十拾 */
				n = (type == 2) ? 4 : 0;
			else /* mod == 2 or 3 */		/* 百千 */
				n = 3+ mod;
			*s++ = num_chars[2][n*2];
			*s++ = num_chars[2][n*2 + 1];
			if (mod == 0)
				u = s;
		}
	}		
	if (s == wdp->kbuf)		/* ００...０００ */
		return((WDS*)NULL);
	*s = '\0';
	return((WDS*)(s+1));
}

/*----------------------------------------------------------------------*/
/*		関数：	dic_getallnum(ustr, ulen, wdsp, wnp)					*/
/*		機能：　指定した語のすべての数字変換語を得る					*/
/*		引数：　UCODE	ustr[];			指定読みのｕコード列			*/
/*				ULEN	ulen;			その長さ						*/
/*				WDS		*wdsp;			得た語を格納するバッファ		*/
/*				int		*wnp;			格納した語数を格納するアドレス	*/
/*		返値：　DIC_HASMATCH	指定の語がある							*/
/*				DIC_HASNONE		指定の語はない							*/
/*				DIC_BADARG		引数に何等かの異常がある				*/
/*				DIC_ISCLOSED	辞書がオープンされていない				*/
/*				DIC_FATAL		辞書が壊れているなどの致命的なエラー	*/
/*		注意：　wdsp->kbuf[] の最後には '\0' を詰める					*/
/*----------------------------------------------------------------------*/
int dic_getallnum(UCODE ustr[], ULEN ulen, WDS *wdsp, int *wnp)
{
	WDS *w;
	int i;

	if (dic_fd == CLOSE)
		return(DIC_ISCLOSED);	/* 辞書がオープンされていない */

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
/*		関数：	dic_clearmem()											*/
/*		機能：　														*/
/*		引数：	なし													*/
/*		注意：　														*/
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
