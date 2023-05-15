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
	mean.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
**		MEAN 코드에 관한 함수. 
*/
#include	"askdef.h"
#include	"mean.h"
#include	"autab.h"


int is_jis(BIT16 code)
{
	register u_char high, low;

	high = (u_char)(code >> 8);
	low = (u_char)(code & 255);
	return((high >= 0x21 && high <= 0x7e) && (low >= 0x21 && low <= 0x7e));
}


int is_kanji(u_char c)
{
	if (c < 0x81 || c > 0xfc || (c >= 0xa0 && c < 0xe0))
		return(0);

	return(1);
}


int is_shift(BIT16 code)
{
	register u_char high, low;

	high = (u_char)(code >> 8);
	low = (u_char)(code & 255);
	if (!is_kanji(high))
		return(0);
	if (low < 0x40 || low > 0xfc || low == 0x7f)
		return(0);
	return(1);
}


int setmean(MEAN *mbuf, BIT16 code, u_short kind)
{
	register MEAN kindtype;

	kindtype = (MEAN)ToKind(kind);			/* real kind */
	if (is_shift(code)) {					/* SHIFT JIS CODE */
		if (shiftlen(code) == 1)
			kindtype |= ToType(SJIS1HALF);
		else
			kindtype |= ToType(SJIS1FULL);
	} else if (is_jis(code)) {				/* JIS 2BYTE CODE */
		kindtype |= ToType(JIS1FULL);
		if (code == 0x2474)					/* 부->부(카타카나)*/
			code = 0x2574;
#if 1 /* ★[QA:01-056-B225] ASK에 의한 문자변환이 이상동작한다. */
		if (code == 0x2475)					/* 카 (작은) */
			code = 0x2575;
		if (code == 0x2476)					/* 케 (작은) */
			code = 0x2576;
#endif /* ★[QA:01-056-B225] ASK에 의한 문자변환이 이상동작한다. */
	} else {								/* ASCII CODE */
		*mbuf = (MEAN)(kindtype | ToType(ASCII_CODE) | ToCode(code));
		return(1);
	}
	*mbuf++ = (MEAN)(kindtype | (code >> 8));
	*mbuf = (MEAN)(To2type(kindtype) | (code & 0xff));
	return(2);
}

void change_kind(register MEAN *mbuf, int len, u_short kind)
{
	register MEAN kindtype;

	kindtype = (MEAN)ToKind(kind);		/* real kind */
	while (len--) {
		*mbuf = (MEAN)((*mbuf & 0x0fff) | kindtype);
		mbuf++;
	}
}

int shiftlen(BIT16 code)
{
	switch (code & 0xff00) {
	case 0x8000:
	case 0x8500:
		return(1);
	case 0x8600:
		if ((code & 0x00ff) < 0x9e)
		return(1);
	default:
		return(2);
	}
}


int sstrtom(MEAN *mbuf, register u_char *str, u_short kind)
{
	register MEAN *mp = mbuf;
	int l;
	BIT16 code;

	while (*str) {
		if (*str < 0x80 || (*str >= 0xa0 && *str < 0xe0)) {
			/* is ASCII */
			l = setmean(mp,(BIT16)*str++,kind);
			mp += l;
		} else {
			code = (BIT16)((*str << 8) | *(str + 1));
			l = setmean(mp,code,kind);
			mp += l;
			str += 2;
		}
	}
	return(mp - mbuf);
}

/*
**		CODE ( JIS and SHIFT JIS ) to U_CODE
*/
u_char to_ucode(BIT16 code)
{
	if (code > 0x20 && code < 0x7f)
		return(0);

	switch (code & 0xff00) {
	case 0x2300:						/* 알파벳 / 숫자 */
		return((u_char)(code & 0x7f));
	case 0x2400:						/* 히라가나 */
		return((u_char)((code & 0x7f) | 0x80));
	case 0x2500:						/* 카타카나 */
		return(0);
	}
	return(srch_ucode(code));
}

/*
**		입력 키의 카타카나 체크 
*/
int is_kata(BIT16 cd, int chou_on)
{
	if (chou_on && cd == 0x213c)
		return(1);		/* - 장음표기 */
	if ((cd & 0xff00) == 0x2500)
		return(1);
	return(0);
}

int is_jisknj(BIT16 cd)
{
	if (cd >= 0x3020)
		return(1);
	return(0);
}

int is_jiskig(BIT16 cd)
{
	/* u-code = 0 일 때만 호출될 것을 전제로 함. */
	if (cd >= 0x2120 && cd < 0x3020) {
		if (is_kata(cd,0))
			return(0);
		return(1);
	}
	return(0);
}

/*
**		convert between MEAN code and SHIFT JIS code
*/
int meantosjis(MEAN *sour, char *dest, int len)
{
	BIT16 type, cd;
	register int i;
	register char *p;

	for (i = 0, p = dest; i < len; ) {
		if ((type = (BIT16)out_mtype(*sour)) == ASCII_CODE) {
			*p++ = (char)*sour++;
			i++;
		} else if (type & 1) {
			cd = (BIT16)(*sour++ << 8);
			i++;
			if (out_mtype(*sour) != type + 1)
				continue;
			cd |= *sour++ & 0x00ff;
			i++;
			if (is_jis(cd))
				cd = ask_jis2sjis(cd);
			*p++ = (char)(cd >> 8);
			*p++ = (char)cd;
		} else
			continue;
	}
	*p = '\0';
	return(p - dest);
}

/*
**		convert SHIFT-JIS-code and JIS-code
*/
BIT16 ask_sjis2jis(BIT16 c)
{
	register u_char high, low;

	high = (u_char)(c >> 8);
	low = (u_char)(c & 0x00ff);
	if (low < 0x40 || low > 0xfc || low == 0x7f)
		return(0);
	if (high >= 0xe0)
		high -= 0x40;
	high = (u_char)(((high - 0x81) << 1) + 0x21);
	if (low >= 0x9f) {
		high++;
		low -= 0x7e;
	} else if (low >= 0x80) {
		low -= 0x20;
	} else {
		low -= 0x1f;
	}
	return((BIT16)((high << 8) | low));
}

BIT16 ask_jis2sjis(BIT16 jis)
{
	register u_char high, low;

	high = (u_char)(jis >> 8);
	low = (u_char)(jis & 255);

	if (jis < 0x2121 || jis > 0x7E7E)
		return(0);
	if (low < 0x21 || low >= 0x7f)
		return(0);

	low += 0x1F;
	if (high & 1)
		high >>= 1;
	else {
		high >>= 1;
		high--;
		low += 0x5E;
	}
	if (low >= 0x7F)
		low++;
	if (high < 0x2F)
		high += 0x71;
	else
		high += 0xB1;
	return(((BIT16)((high << 8)|(low & 0xff))));
}
