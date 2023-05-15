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
	autab.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include	"askdef.h"
#include	"askkey.h"
#include	"autab.h"
#include	"mean.h"

/*
**
**		ASCII code ∏¶ JIS code ∑Œ ∫Ø»Ø«œ±‚ ¿ß«— µ•¿Ã≈Õ 
**
*/

BIT16	asc2jis[] = {
/* 0 */ 0x2223, 0x2223, 0x2223, 0x2223,			/* °·°·°·°· */
		0x2223, 0x2223, 0x2223, 0x2223,			/* °·°·°·°· */
		0x2223, 0x2223, 0x2223, 0x2223,			/* °·°·°·°· */
		0x2223, 0x2223, 0x2223, 0x2223,			/* °·°·°·°· */
/* 1 */ 0x2223, 0x2223, 0x2223, 0x2223,			/* °·°·°·°· */
		0x2223, 0x2223, 0x2223, 0x2223,			/* °·°·°·°· */
		0x2223, 0x2223, 0x2223, 0x2223,			/* °·°·°·°· */
		0x222A, 0x222B, 0x222C, 0x222D,			/* °Ê°Á°Ë°È */
/* 2 */ 0x2121, 0x212A, 0x2149, 0x2174,			/* °°£°"££ */
		0x2170, 0x2173, 0x2175, 0x2147,			/* £§£•£¶' */
		0x214A, 0x214B, 0x2176, 0x215C,			/*  £®£©£™£´ */
		0x2124, 0x215D, 0x2125, 0x213F,			/*  £¨£≠£Æ£Ø */
/* 3 */ 0x2330, 0x2331, 0x2332, 0x2333,			/* £∞£±£≤£≥  */
		0x2334, 0x2335, 0x2336, 0x2337,			/* £¥£µ£∂£∑  */
		0x2338, 0x2339, 0x2127, 0x2128,			/* £∏£π£∫£ª */
		0x2163, 0x2161, 0x2164, 0x2129,			/* £º£Ω£æ£ø */
/* 4 */ 0x2177, 0x2341, 0x2342, 0x2343,			/* £¿£¡£¬£√ */
		0x2344, 0x2345, 0x2346, 0x2347,			/* £ƒ£≈£∆£« */
		0x2348, 0x2349, 0x234A, 0x234B,			/* £»£…£ £À */
		0x234C, 0x234D, 0x234E, 0x234F,			/* £Ã£Õ£Œ£œ */
/* 5 */ 0x2350, 0x2351, 0x2352, 0x2353,			/* £–£—£“£” */
		0x2354, 0x2355, 0x2356, 0x2357,			/* £‘£’£÷£◊ */
		0x2358, 0x2359, 0x235A, 0x214E,			/* £ÿ£Ÿ£⁄£€  */
		0x216F, 0x214F, 0x2130, 0x2132,			/* °Õ£›£ﬁ£ﬂ  */
/* 6 */ 0x2146, 0x2361, 0x2362, 0x2363,			/* ÅeÇÅÇÇÇÉ */
		0x2364, 0x2365, 0x2366, 0x2367,			/* ÇÑÇÖÇÜÇá */
		0x2368, 0x2369, 0x236A, 0x236B,			/* ÇàÇâÇäÇã */
		0x236C, 0x236D, 0x236E, 0x236F,			/* ÇåÇçÇéÇè */
/* 7 */ 0x2370, 0x2371, 0x2372, 0x2373,			/* ÇêÇëÇíÇì */
		0x2374, 0x2375, 0x2376, 0x2377,			/* ÇîÇïÇñÇó */
		0x2378, 0x2379, 0x237A, 0x2150,			/* ÇòÇôÇöÅo */
		0x2143, 0x2151, 0x2141, 0x2223,			/* ÅbÅpÅ`Å° */
/* 8 */ 0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
/* 9 */ 0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
/* A */ 0x2121, 0x2123, 0x2156, 0x2157,			/* Å@ÅBÅuÅv */
		0x2122, 0x2126, 0x2572, 0x2521,			/* ÅAÅEÉíÉ@ */
		0x2523, 0x2525, 0x2527, 0x2529,			/* ÉBÉDÉFÉH */
		0x2563, 0x2565, 0x2567, 0x2543,			/* ÉÉÉÖÉáÉb */
/* B */ 0x213C, 0x2522, 0x2524, 0x2526,			/* Å[ÉAÉCÉE */
		0x2528, 0x252A, 0x252B, 0x252D,			/* ÉGÉIÉJÉL */
		0x252F, 0x2531, 0x2533, 0x2535,			/* ÉNÉPÉRÉT */
		0x2537, 0x2539, 0x253B, 0x253D,			/* ÉVÉXÉZÉ\ */
/* C */ 0x253F, 0x2541, 0x2544, 0x2546,			/* É^É`ÉcÉe */
		0x2548, 0x254A, 0x254B, 0x254C,			/* ÉgÉiÉjÉk */
		0x254D, 0x254E, 0x254F, 0x2552,			/* ÉlÉmÉnÉq */
		0x2555, 0x2558, 0x255B, 0x255E,			/* ÉtÉwÉzÉ} */
/* D */ 0x255F, 0x2560, 0x2561, 0x2562,			/* É~ÉÄÉÅÉÇ */
		0x2564, 0x2566, 0x2568, 0x2569,			/* ÉÑÉÜÉàÉâ */
		0x256A, 0x256B, 0x256C, 0x256D,			/* ÉäÉãÉåÉç */
		0x256F, 0x2573, 0x212B, 0x212C,			/* ÉèÉìÅJÅK */
/* E */ 0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x217C, 0x217B, 0x2223, 0x2223,			/* ÅúÅõÅ°Å° */
/* F */ 0x2223, 0x315F, 0x472F, 0x376E,			/* Å°â~îNåé */
		0x467C, 0x3B7E, 0x4A2C, 0x4943,			/* ì˙éûï™ïb */
		0x2223, 0x2223, 0x2223, 0x2223,			/* Å°Å°Å°Å° */
		0x2223, 0x2223, 0x2223, 0x2223			/* Å°Å°Å°Å° */
};

BIT16 to_zenkaku(BIT16 key)
{
	extern BIT16 asc2jis[];
	BIT16 ret;

	ret = asc2jis[key & 0xff];
	if ((ret & 0xff00) == 0x2500 && !(key & IC_KATAKANA))
		return((BIT16)((ret & 0x00ff) | 0x2400));

	return(ret);
}


BIT16 to_zenkaku_spec(BIT16 key)
{
	static u_char spec_key[] = ",-./Xx[]";
	static BIT16 spec_tran[] = {
		0x2122, /* ƒﬁ∏∂ */		0x213c, /* ¿Â¿Ω±‚»£ */
		0x2123, /* ∏∂ƒß«• */		0x2126, /* ¡ﬂ∞£¡° */
		0x2573, /* ¿¿ */		0x2573, /* ¿¿ */
		0x2156, /* ≤©ºËø≠∏≤  */		0x2157	/* ≤©ºË¥›»˚ */
	};
	register u_char *p = spec_key;
	register BIT16	*t = spec_tran;

	while (*p) {
		if (*p == (u_char)key) {
			if ((*t & 0xff00) == 0x2500 && !(key & IC_KATAKANA))
				return((BIT16)((*t & 0x00ff) | 0x2400));
			return(*t);
		}
		p++;
		t++;
	}
	return(0);
}

BIT16 ext_jis(u_char code, BIT16 ic)
{
	return((BIT16)(code | ((ic & IC_KATAKANA) + 0x2400)));
}

BIT16	utobyte[] = {				// π›∞¢ ƒ´≈∏ƒ´≥™ 
/* 9 */ 0x00a1, 0x00a2, 0x00a3, 0x00a4,			/* ? ? ? ? */
		0x00a5, 0x00b0, 0x00de, 0x00df,			/* ? ? ? ? */
/* A */ 0x0020, 0x00a7, 0x20b1, 0x00a8,			/*	 ß ± ® */
		0x20b2, 0x00a9, 0x20b3, 0x00aa,			/* ≤ © ≥ ™ */
		0x20b4, 0x00ab, 0x20b5, 0x00b6,			/* ¥ ´ µ ∂ */
		0x80b6, 0x00b7, 0x80b7, 0x00b8,			/* ∂ﬁ∑ ∑ﬁ∏ */
/* B */ 0x80b8, 0x00b9, 0x80b9, 0x00ba,			/* ∏ﬁπ πﬁ∫ */
		0x80ba, 0x00bb, 0x80bb, 0x00bc,			/* ∫ﬁª ªﬁº */
		0x80bc, 0x00bd, 0x80bd, 0x00be,			/* ºﬁΩ Ωﬁæ */
		0x80be, 0x00bf, 0x80bf, 0x00c0,			/* æﬁø øﬁ¿ */
/* C */ 0x80c0, 0x00c1, 0x80c1, 0x00af,			/* ¿ﬁ¡ ¡ﬁØ */
		0x20c2, 0x80c2, 0x00c3, 0x80c3,			/* ¬ ¬ﬁ√ √ﬁ*/
		0x00c4, 0x80c4, 0x00c5, 0x00c6,			/* ƒ ƒﬁ≈ ∆ */
		0x00c7, 0x00c8, 0x00c9, 0x00ca,			/* « » …   */
/* D */ 0x80ca, 0x40ca, 0x00cb, 0x80cb,			/*  ﬁ ﬂÀ Àﬁ*/
		0x40cb, 0x00cc, 0x80cc, 0x40cc,			/* ÀﬂÃ ÃﬁÃﬂ*/
		0x00cd, 0x80cd, 0x40cd, 0x00ce,			/* Õ ÕﬁÕﬂŒ */
		0x80ce, 0x40ce, 0x00cf, 0x00d0,			/* ŒﬁŒﬂœ – */
/* E */ 0x00d1, 0x00d2, 0x00d3, 0x00ac,			/* — “ ” ¨ */
		0x20d4, 0x00ad, 0x20d5, 0x00ae,			/* ‘ ≠ ’ Æ */
		0x20d6, 0x00d7, 0x00d8, 0x00d9,			/* ÷ ◊ ÿ Ÿ */
		0x00da, 0x00db, 0x00dc, 0x20dc,			/* ⁄ € ‹ ‹ */
/* F */ 0x00b2, 0x00b4, 0x00a6, 0x00dd,			/* ≤ ¥ ¶ › */
		0x80b3, 0x00b6, 0x00b9					/* ≥ﬁ∂ π   */
};

#define CAN_DAKU		0x8000
#define CAN_HANDAKU		0x4000
#define CAN_SMALL		0x2000

int to_hankaku(u_char *buf, BIT16 code)
{
	u_char uc;

	switch (code & 0xff00) {
	case 0x2300:
	case 0x0000:
		*buf = (u_char)(code & 0x00ff);
		return(1);
	case 0x2500:
		code &= 0x24FF;
	case 0x2400:
	case 0x2100:
		uc = to_ucode(code);
		if (uc < 0x98 || uc >= 0xF7) {
			*buf = uc;
			return(1);
		} else {
			code = utobyte[uc - 0x98];
			*buf++ = (u_char)code;
			if (code & CAN_DAKU) {
				*buf = (u_char)0xde; /* 0xde = 'ﬁ' */
				return(2);
			} else if (code & CAN_HANDAKU) {
				*buf = (u_char)0xdf; /* 0xdf = 'ﬂ' */
				return(2);
			} else
				return(1);
		}
	}
	*buf = ' ';
	return(1);
}

extern u_char rmtype[];

int can_daku(BIT16 code)
{
	u_char htmp[2];

	if (is_shift(code))
		code = ask_sjis2jis(code);
	if (to_hankaku(htmp, code) != 1)
		return(0);

	return((rmtype[htmp[0]] & 0x0f) == 0x0a);
}


int can_handaku(BIT16 code)
{
	u_char htmp[2];

	if (is_shift(code))
		code = ask_sjis2jis(code);

	if (to_hankaku(htmp, code) != 1)
		return(0);

	return((rmtype[htmp[0]] & 0x0f) == 0x0b);
}

int can_small(u_char code)
{
	if (code <= 0x20 || code >= 0x77)
		return(0);
	return(utobyte[code - 0x18] & CAN_SMALL);
}

/*
**
**		JIS code ∏¶ ucode ∑Œ ∫Ø»Ø«œ±‚ ¿ß«— µ•¿Ã≈Õ
**
*/
struct byteucode {
	u_short jis;
	u_char	ucode;
}btoudata[] ={
	{0x2121, 0x20},			/* Å@ */
	{0x2122, 0x9B},			/* ÅA */
	{0x2123, 0x98},			/* ÅB */
	{0x2124, 0x2C},			/* ÅC */
	{0x2125, 0x2E},			/* ÅD */
	{0x2126, 0x9C},			/* ÅE */
	{0x2127, 0x3A},			/* ÅF */
	{0x2128, 0x3B},			/* ÅG */
	{0x2129, 0x3F},			/* ÅH */
	{0x212A, 0x21},			/* ÅI */
	{0x212B, 0x9E},			/* ÅJ */
	{0x212C, 0x9F},			/* ÅK */
	{0x2130, 0x5E},			/* ÅO */
	{0x2132, 0x5F},			/* ÅQ */
	{0x213C, 0x9D},			/* Å[ */
	{0x213F, 0x2F},			/* Å^ */
	{0x2141, 0x7E},			/* Å` */
	{0x2143, 0x7C},			/* Åb */
	{0x2146, 0x60},			/* Åe */
	{0x2147, 0x27},			/* Åf */
	{0x2149, 0x22},			/* Åh */
	{0x214A, 0x28},			/* Åi */
	{0x214B, 0x29},			/* Åj */
	{0x214E, 0x5B},			/* Åm */
	{0x214F, 0x5D},			/* Ån */
	{0x2150, 0x7B},			/* Åo */
	{0x2151, 0x7D},			/* Åp */
	{0x2156, 0x99},			/* Åu */
	{0x2157, 0x9A},			/* Åv */
	{0x215C, 0x2B},			/* Å{ */
	{0x215D, 0x2D},			/* Å| */
	{0x2161, 0x3D},			/* ÅÅ */
	{0x2163, 0x3C},			/* ÅÉ */
	{0x2164, 0x3E},			/* ÅÑ */
	{0x216F, 0x5C},			/* Åè */
	{0x2170, 0x24},			/* Åê */
	{0x2173, 0x25},			/* Åì */
	{0x2174, 0x23},			/* Åî */
	{0x2175, 0x26},			/* Åï */
	{0x2176, 0x2A},			/* Åñ */
	{0x2177, 0x40},			/* Åó */
	{0x222A, 0x1C},			/* Å® */
	{0x222B, 0x1D},			/* Å© */
	{0x222C, 0x1E},			/* Å™ */
	{0x222D, 0x1F}			/* Å´ */
};

#define Endof(array)	&(array)[sizeof(array)/sizeof(array[0])]


u_char srch_ucode(BIT16 code)
{
	register struct byteucode *p;

	for (p = btoudata; p < Endof(btoudata); p++) {
		if (p->jis == code)
			return(p->ucode);
		else if (p->jis > code)
			break;
	}
	return(0);
}
