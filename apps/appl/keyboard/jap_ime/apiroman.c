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
	apiroman.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
 *
 * ask\roman ÀÌÇÏÀÇ °¡ ÀÔ·Â Ã³¸®ÀÇ ¼Ò½º·Î ºÎÅÍ 
 * API¸¦ ½ÇÇöÇÏ±â À§ÇÑ ÇÊ¿äÇÑ °ÍÀ» ¹ßÃéÇÑ ÆÄÀÏ.
 *
 */

#include	"askdef.h"
#include	"askkey.h"
#include	"mean.h"
#include	"imode.h"
#include	"pword.h"
#include	"hwork.h"
#include	"dic.h"
#include	"henkan.h"
#include	"aci.h"
#include	"../apiask.h"
#include	"apiroman.h"
#include	"select.h"
#include	"autab.h"
#include	"dicdef.h"
#include	"dic_main.h"
#include	"memory.h"

extern int kana_len;
extern BIT16 inpc_buf[];
extern BIT16 *pinpc_buf, *qinpc_buf, *cinpc_buf, *einpc_buf;
extern JSYMBOL kana_buf[];
extern JSYMBOL *pkana_buf, *ekana_buf;
extern int	kana_len;
extern u_char	kana_ustr[];
extern u_char	*p_ustr, *e_ustr;

/*
 * »çÀüÀÇ ¸íÄª
 * API·Î ¼³Á¤ / ÃëµæÇÒ ¶§¿¡ »ç¿ëÇÏ´Â Á¤Àû º¯
 */
u_char dic_name[128] = "ask3.dic";

/*
 * ÀÔ·Â ¸ðµå
 * µî·ÏÇÒ ¶§¿¡ ÆíÁý ¸ðµå·Î µÇ¾î ÀÖÁö ¾ÊÀº°¡? ÀÇ Ã¼Å©¿¡ »ç¿ëµÉ »ÓÀÌ´Ù. 
 */
int im_state = IM_NOKANJI;

/*
 * ´Ü¾îÀÇ ÀÐ±â ¹öÆÛ
 * ´Ü¾î µî·Ï½ÃÀÇ ÀÛ¾÷ ¿µ¿ª
 */
u_char yomi_buf[80];

/*
 * ÈÄº¸ À©µµ¿ìÀÇ ±æÀÌ ÀÎ°¡ ? 
 */
int kwin_length(void)
{
	return 72;
}

int nwin_length(void)
{
	return 72;
}

int undo_prepared = 0;

/* ROMANÁßÀÇ ÇÔ¼ö (½º°ÔÄ«¿¡ª¹ª²ôðª¨) */
void init_roman(void)
{
	pinpc_buf = qinpc_buf = cinpc_buf = einpc_buf = inpc_buf;
	pkana_buf = ekana_buf = kana_buf;
	p_ustr = e_ustr = kana_ustr;
	kana_len = 0;
}

/* ROMANÁßÀÇ ÇÔ¼ö.  */
void init_edit0(void)
{
	init_roman();
	free_hchar(0,HCHAR_SIZE-1,ALL_FREE);
	clear_prevwd();
}

/* °¡ÀÔ·ÂÀÇ ¹®ÀÚ Á¾·ù º¯È¯ */
void trans_roman(u_char *buf, int spos, int len, int type)
{
	register JSYMBOL *js;
	JSYMBOL *je;
	BIT16 cd, precd;

	if (type >= CT_RAW) {
		int	i, ls, le;
		register BIT16 *p;

		js = &kana_buf[0];
		ls = 0;
		for (i = 0; i < spos; i++, js++)
			ls += js->klen;
		le = ls;
		for (i = 0; i < len; i++, js++)
			le += js->klen;
		for (p = &inpc_buf[ls]; ls < le; ls++) {
			if (type == CT_RAW) {
				cd = (BIT16)(*p++ | IC_KATAKANA);
				cd = ask_jis2sjis(to_zenkaku(cd));
				/* Ç×»ó Àü°¢À¸·Î ÇÏ¸é µÇ³ª ? */
				*buf++ = (u_char)(cd >> 8);
				*buf++ = (u_char)cd;
			} else if (type == CT_RAWHAN) {
				*buf++ = (u_char)*p++;
			}
		}
		*buf = '\0';
		return;
	}
	precd = 0;
	js = &kana_buf[spos];
	je = js + len;
	for ( ; js < je; js++) {
		cd = js->symbol;
		if (type == CT_HIRA) {
			if ((cd & 0xff00) == 0x0000) {
				if (cd == 0xde && can_daku(precd)) { /* 0xde = 'Þ' */
					buf -= 2;
					if (precd == 0x2426)	/* ¿ì */
						cd = 0x2474;		/* ¿ì-Å¹À½ */
					else
						cd = (BIT16)(precd + 1);
				} else if (cd == 0xdf && can_handaku(precd)) { /* 0xdf = '¹ÝÅ¹À½±âÈ£' */
					buf -= 2;
					cd = (BIT16)(precd + 2);
				} else {
					cd = to_zenkaku(cd);
				}
			}
			if ((cd & 0xff00) == 0x2500)
				cd &= 0x24FF;
		/** ¿ì-Å¹À½ À» ¾î¶»°Ô ÇÒ±î ?**/
		} else if (type == CT_KATA) {
			if ((cd & 0xff00) == 0x0000) {
				if (cd == 0xde && can_daku(precd)) { /* 0xde = 'Þ' */
					buf -= 2;
					if (precd == 0x2526)	/* ¿ì */
						cd = 0x2574;		/* ¿ì-Å¹À½ */
					else
						cd = (BIT16)(precd + 1);
				} else if (cd == 0xdf && can_handaku(precd)) { /* 0xdf = '¹ÝÅ¹À½±âÈ£' */
					buf -= 2;
					cd = (BIT16)(precd + 2);
				} else {
					cd = to_zenkaku((BIT16)(cd | IC_KATAKANA));
				}
			}
			if ((cd & 0xff00) == 0x2400)
				cd |= 0x2500;
		} else if (type == CT_HANKAKU) {
			buf += to_hankaku(buf,cd);
			continue;
		} else if (type == CT_ZENKAKU) {
			if ((cd & 0xff00) == 0x0000)
				cd = to_zenkaku((BIT16)(cd | IC_KATAKANA));
		}
		precd = cd;
		if (cd & 0xff00) {
			cd = ask_jis2sjis(cd);
			*buf++ = (u_char)(cd >> 8);
		}
		*buf++ = (u_char)cd;
	}
	*buf = '\0';
}

/* È¯°æ Å×ÀÌºí (»ç¿ëµÇÁö ¾ÊÀ½) */
BIT16 ektable[1];
int	undo_cur_pos = 0;
int undo_im_state = 0;
int undo_length = 0;

int	save_im_state = 0;

u_char	tango_buf[80];

void into_editing(int sw)
{
	extern int	learn_on;
	extern int	kana_buf_size;

	learn_on = dic_getgaku();
	init_univmem();
	init_hchar();
	init_edit0();
	if (sw == ON) {
		kana_buf_size = (nwin_length() - 1) / 2;
		im_state = IM_EDIT0;
	}
}


int	roman_makedisp(int start, int len, MEAN	*mbuf, int cursor)
{
	register JSYMBOL *js;
	register MEAN *mp;
	JSYMBOL	*je;
	BIT16 *p;
	BIT16 jis;
	int	cur, l;

	cur = 0;
	mp = mbuf;
	for (js = &kana_buf[start], je = &kana_buf[start+len]; js < je; js++) {
		if (js == pkana_buf) {
			for (p = qinpc_buf; p < pinpc_buf; p++) {
				if (*p & IC_ZENKAKU) {
					jis = to_zenkaku(*p);
				} else {
					jis = (BIT16)(*p & 0xff);
				}
				if (cursor && p == cinpc_buf) {
					l = setmean(mp,jis,CURSOR);
					cursor = 0;
				} else {
					l = setmean(mp,jis,UNCERTAIN);
				}
				mp += l;
			}
			cur = 1;
		}
		if (js >= ekana_buf)
			break;
		if (cursor && cur) {
			l = setmean(mp,js->symbol,CURSOR);
			cursor = 0;
		} else {
			l = setmean(mp,js->symbol,UNCERTAIN);
		}
		mp += l;
		cur = 0;
	}
	if (cursor) {
		l = setmean(mp,' ',CURSOR);
		mp += l;
	}
	return(mp - mbuf);
}
