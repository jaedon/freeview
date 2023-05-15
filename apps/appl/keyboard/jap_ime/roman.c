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
	roman.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include	"askdef.h"
#include	"askkey.h"
#include	"rometab.h"
#include	"mean.h"
#include	"imode.h"
#include	"dic.h"
#include	"pword.h"
#include	"hwork.h"
#include	"henkan.h"
#include	"roman.h"
#include	"autab.h"

static int	bytesin_kana_buf(JSYMBOL *sk, JSYMBOL *ek);


#define SMALL_BY_SHIFT	1				/* shift+'A' などで小さい字に変換 */
#define TSU_BY_SHIFT_Z	1				/* shift+'Z' で小さい「っ」に変換 */

int		roman_japan = 1;				/* 句点変換する／しない */
BIT16	ic_flag = IC_ZENKAKU;			/* 入力モード */

#define INPC_BUF_SIZE	100
#define KANA_BUF_SIZE	36
int		kana_buf_size = KANA_BUF_SIZE;

BIT16	inpc_buf[INPC_BUF_SIZE];		/* 入力キャラクタ列用			*/
BIT16	*pinpc_buf, *qinpc_buf, *cinpc_buf, *einpc_buf;
										/*		   〃		 ポインタ	*/

JSYMBOL kana_buf[KANA_BUF_SIZE+3];		/* ローマ字変換後の文字列用		*/
JSYMBOL *pkana_buf, *ekana_buf;			/*		   〃		 ポインタ	*/
int		kana_len;

u_char	kana_ustr[KANA_BUF_SIZE+3];		/* ローマ字 ucode 文字列		*/
u_char	*p_ustr, *e_ustr;

/*
**
**		inpc_buf[] =
**				+---+---+---+---+---+---+---+---+---+---+---+---+---
**				| k | a | n | j | h | e | n | s | h | u | u |	|	
**				+---+---+---+---+---+---+---+---+---+---+---+---+---
**							 ↑		 ↑		 ↑				 ↑
**							 ｜		 ｜		 pinpc_buf		 einpc_buf
**							 ｜		 cinpc_buf（＝カーソル位置）
**							 qinpc_buf
**
**		kana_buf[] =
**				+---+---+---+---+---+---+---+---+---+---+---+---+---
**				|か2|ん1|し3|ゅ0|う1|	|	|	|	|	|	|	|	
**				+---+---+---+---+---+---+---+---+---+---+---+---+---
**						 ↑			 ↑
**						 pkana_buf	 ekana_buf
**
**		画面出力 =
**				+---+---+---+---+---+---+---+---+---+---+---+---+---
**				|か |ん |ｊ |ｈ |ｅ<|ｎ |し |ゅ |う |	|	|	|	
**				+---+---+---+---+---+---+---+---+---+---+---+---+---
**
*/
#if 0 /* Changed By J.Katoh.1998/01/12 */
/*
AIUEOaiueo						0001 - 0101		母音
ﾞ								0110			濁点
ﾟ								0111			半濁点
一般のカタカナ・記号			1000			そのまま
,-./Xx[]						1001			１文字で決まるが要変換
ｳｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄ				1010			濁音可
ﾊﾋﾌﾍﾎ							1011			半濁音可
Nn								1100			Ｎ
BCDFGHJKPQSTZbcdfghjkpqstz		1101			子音
LMRVWlmrvw						1110			その他のアルファベット
Yy								1111			Ｎにつくその他のアルファベット
*/
#else
/*
AIUEOaiueo						0001 - 0101		母音
ﾞ								0110			濁点
ﾟ								0111			半濁点
一般のカタカナ・記号			1000			そのまま
,-./[]							1001			１文字で決まるが要変換
ｳｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄ				1010			濁音可
ﾊﾋﾌﾍﾎ							1011			半濁音可
Nn								1100			Ｎ
BCDFGHJKLMPQRSTVWXZ				1101			子音
bcdfghjklmpqrstvwxz
Yy								1111			Ｎにつくその他のアルファベット
*/
#endif

#define RM_NONE			0
#define RM_ENU			1
#define RM_DAKU			2
#define RM_HANDAKU		3

int to_roman(BIT16 *pi, int l, BIT16 *mbuf, int *mlen)
{
	extern u_char rmtype[];
	register BIT16 *p;
	u_char rmtyp, key;
	u_short rmsts;
	u_char *rda;
	int i;

	key = (u_char)(*pi & 0xff);
	rmtyp = (u_char)(rmtype[key] & 0x0f);
	*mlen = 1;
	if (rmtyp <= 9) {		/* この１キーで決定する */
		if (rmtyp <= 5) {
			/* ローマ字変換 */
			rda = NULLprmtab->rmch;
			i = 1;
			p = pi;
			goto romantrans;
		}
		if (rmtyp <= 8 || !roman_japan) {
			/* 全角変換 */
			*mbuf = to_zenkaku(*pi);
		} else /* if (rmtype == 9) */ {
			/* 句点変換 */
			*mbuf = to_zenkaku_spec(*pi);
		}
		return(1);
	}
	rmsts = RM_NONE;
	if (rmtyp <= 12) {		/* この１キーで仮決定 */
		if (rmtyp == 12) {
			/* Ｎ */
			*mbuf = ext_jis(0x73,*pi);
			rmsts = RM_ENU;
		} else {
			/* かなキー */
			*mbuf = to_zenkaku(*pi);
			if (rmtyp == 10)
				rmsts = RM_DAKU;
			else /* if (rmtyp == 11) */
				rmsts = RM_HANDAKU;
		}
	}
	if (l == 1) {
		if (rmsts != RM_NONE)
			return(-1);
		return(0);
	}

	p = pi + 1;
	key = (u_char)(*p & 0xff);
	if ((*pi & 0xff) == key) {		/* ２キー連続 */
		if (rmtyp == 12) {				/* ん */
			/* ＮＮ */
			return(2);
#if 0 /* Changed By J.Katoh.1999/03/09(Yやyも仲間に入れてあげる) */
		} else if (rmtyp == 13) {		/* っ */
#else
		} else if (rmtyp >= 13) {		/* っ */
#endif
			/* 子音子音 */
			*mbuf = ext_jis(0x43,*p);
			return(1);
		}
	}
	if (rmsts == RM_ENU && key == '\'')
		return(2);

	rmtyp = (u_char)(rmtype[key] & 0x0f);
	if (rmtyp == 7 && rmsts == RM_HANDAKU) {
		/* 半濁点（゜）を付ける */
		(*mbuf) += 2;
		return(2);
	}
	if (rmtyp == 6 && rmsts >= RM_DAKU) {
		/* 濁点（゛）を付ける */
		(*mbuf)++;
		if ((*pi & 0xff) == 0xb3)		/* ｳ */
			*mbuf = ext_jis(0x74,*p);
		return(2);
	}
	if (rmsts >= RM_DAKU || (rmtyp > 5 && rmtyp <= 14 && rmsts == RM_ENU))
		/* 仮決定で決定 */
		return(1);

	i = 2;
	for ( ;; ) {
		if (rmtyp <= 5) {		/* 前の文字と併せて変換 */
			if ((rda = getrda2(pi,p + 1)) == NULL) {
				if ((rda = getrda1(pi,p)) == NULL)
					return(-2);
				romantrans:
				rda += (rmtyp - 1) * 2;
			}
			*mbuf++ = ext_jis(*rda++,*p);
			if (*rda) {
				*mbuf = ext_jis(*rda,*p);
				*mlen = 2;
			}
			return(i);
		}
		if (i++ >= l)
			break;
		p++;
		key = (u_char)(*p & 0xff);
		rmtyp = (u_char)(rmtype[key] & 0x0f);
		if ((rmtyp >= 6 && rmtyp <= 0x0c) || i > 4)
			return(-2);		/* 変換エラー */
	}
	return(0);
}

u_char ToUpper(u_char c)
{
	if (c >= 'a' && c <= 'z')
		return((u_char)(c - 'a' + 'A'));
	return(c);
}


u_char *getrda2(BIT16 *p, BIT16 *q)
{
	register u_char *s;
	register BIT16 *r;
	int i, plen, l;

	plen = q - p;
	for (s = rmspec; *s; ) {
		l = *s++;
		if (plen == l) {
			r = p;
			for (i = plen; i > 0; i--, s++) {
				if ( *s != ToUpper((u_char)(*r++ & 0xff)) )
					break;
			}
			if (i == 0)
				return(s);
			s += i;
		} else {
			s += l;
		}
		while (*s++ != '\0')
			;
	}
	return(NULL);
}


u_char *getrda1(BIT16 *p, BIT16 *q)
{
	int plen = q - p;
	int ch1, ch2;
	ROMAN5 *rp, *rq;

	if (plen == 0)
		return(NULLprmtab->rmch);
	if (plen == 1 || plen == 2) {
		ch1 = *p & 0xff;
		if (ch1 >= 'a' && ch1 <= 'z')
			ch1 -= 'a';
		else if (ch1 >= 'A' && ch1 <= 'Z')
			ch1 -= 'A';
		else
			return(NULL);
		rp = prmtab[ch1];
		if (plen == 1)
			return(rp->rmch);
		do {
			ch1++;
		} while ((rq = prmtab[ch1]) == NULL);
		ch2 = ToUpper((u_char)(*(p + 1) & 0xff));
		for (rp++; rp < rq; rp++) {
			if (rp->exch == ch2)
				return(rp->rmch);
		}
	}
	return(NULL);
}

int add_kana_buf(register u_char *str)
{
#define MAX_BYTES		79

	register JSYMBOL *pk;
	register u_char *pu;
	int k_len;
	int b_len;
	BIT16 c = 0;

	pk = pkana_buf;
	pu = p_ustr;
	k_len = kana_len;
	b_len = bytesin_kana_buf(kana_buf,pkana_buf);
	for ( ; *str; str++) {
		if (c) {				/* シフトＪＩＳ、２バイト目 */
			c = (BIT16)((c << 8) | *str);
			if ((c = ask_sjis2jis(c)) != 0) {
				if (k_len >= KANA_BUF_SIZE
				|| b_len >= MAX_BYTES - 1) {
					return(-1);
				}
				pk->symbol = c;
				pk->klen = 2;
				pk++;
				*pu++ = to_ucode(c);
				k_len++;
				b_len += 2;
			}
			c = 0;
			continue;
		}
		if (is_kanji(*str)) {	/* シフトＪＩＳ、１バイト目 */
			c = *str;
			continue;
		}
		if ((*str >= 0x20 && *str < 0x7f)
		|| (*str >= 0xa0 && *str < 0xe0)) {
			if (k_len >= KANA_BUF_SIZE
			|| b_len >= MAX_BYTES) {
				return(-1);
			}
			pk->symbol = *str;
			pk->klen = 1;
			pk++;
			*pu++ = 0;
			k_len++;
			b_len++;
			c = 0;
		}
	}
	ekana_buf = pkana_buf = pk;
	e_ustr = p_ustr = pu;
	kana_len = k_len;
	return(kana_len);
}

static int bytesin_kana_buf(JSYMBOL *sk, JSYMBOL *ek)
{
	register JSYMBOL *pk;
	int ret = 0;

	for (pk = sk; pk < ek; pk++) {
		if (pk->symbol & 0xff00) {
			ret += 2;
		} else {
			ret++;
		}
	}
	return(ret);
}

int del_kana_buf(int n)
{
	register JSYMBOL *js;
	int kl;

	if (n <= 0)
		return(kana_len);
	for (js = pkana_buf - 1, kl = 1; js >= kana_buf; js--, kl++) {
		if ((n -= js->klen) <= 0)
			break;
	}
	if (n != 0)
		return(-1);
	kana_len -= kl;
	pkana_buf -= kl;
	ekana_buf -= kl;
	p_ustr -= kl;
	e_ustr -= kl;
	return(kana_len);
}


int count_byte_kana_buf(int pos, int len)
{
	register JSYMBOL *js;
	int kl;

	kl = 0;
	for (js = &kana_buf[pos]; len > 0; js++, len--)
		kl += js->klen;
	return(kl);
}

/*
**		ローマ字表示用ルーチン
*/

MEAN	meanbuf[(KANA_BUF_SIZE + 2) * 4];
u_char	outbuf[(KANA_BUF_SIZE + 2) * 4];
/*
**		アンドゥ（入力文字列）
*/

BIT16	ud_inpc_buf[INPC_BUF_SIZE];		/* 入力キャラクタ列				*/
int		ud_inpc_len;
JSYMBOL ud_kana_buf[KANA_BUF_SIZE+3];	/* ローマ字変換後の文字列		*/
u_char	ud_kana_ustr[KANA_BUF_SIZE+3];	/* ローマ字 ucode 文字列		*/
int		ud_kana_len;

extern int undo_prepared;
