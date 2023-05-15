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
	dic_main.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#ifndef _ASK_DIC_MAIN_H_
#define _ASK_DIC_MAIN_H_

int		dic_open(char *dicname);
int		dic_close(void);
int		dic_snssyn(UCODE ustr[], ULEN ulen, ENTRYS *buf);
int		main_snssyn(UCODE *yomi, ULEN ulen, RECINFO *recp);
int		set_synref(register uchar *p, SYNREF *synp, int count, int *gakup);
int		exist_synref(register SYNREF *s, register SYNREF *p);
uchar	*next_wd(register uchar	*p, register uchar	*pend);
int		dic_getsyn(UCODE ustr[], ULEN ulen, ENTRYS	*buf);
int		main_getsyn(UCODE	*yomi, ULEN ulen, RECINFO *recp);
int		dic_get1wd(ENTID	entid, SYNTAX	syn, KATUYO	ktu, WDS	*wdsp);
void	get1wd(uchar	*top_rec, int syn, int ktu, int *ptime, WDS *wdsp, int mode);
void	set_wds(WDS		*wp, uchar	*wdp, uchar	*end, int wdid);
int		dic_getallwd(ENTID	entid, SYNTAX	syn, KATUYO	ktu, WDS	*wdsp, int	*wnp);
void	getallwd(uchar	*top_rec, int syn, int ktu, int *pcnt, int *ptotal, WDS *wdsp, int mode);
WDS		*ins_wds(WDS	*wp, int rtime, int size, int total);
int		dic_learn(ENTID	entid, WDID	wdid, ENTID entids[], int n);
void	clear_rtime(uchar *p);
void	clear_allrtime(ENTID entids[], int n);
int		max_rtime(uchar *p);
int		dic_touroku(NEWWD	*newwdp);
uchar	*end_page(uchar *p);
void shiftpage(uchar *s, uchar *t, int len);	/* len > 0 forward */
int		dic_delete(NEWWD	*newwdp);
int		delwd(uchar *p, NEWWD	*newp);
int		dic_tmptouroku(NEWWD	*newwdp);
int		dic_newlearn(NEWWD	*newwdp, ENTID	entids[], int n);
int		isnum(UCODE	*s);
int		dic_freeentid(ENTID	entid);
int		dic_setgaku(int mode);
int		dic_getgaku(void);
int		dic_get1num(UCODE	ustr[], ULEN ulen, WDS	*wdsp);
WDS		*set_num(UCODE ustr[], ULEN ulen, WDS *wdp, int type);
int		dic_getallnum(UCODE ustr[], ULEN ulen, WDS *wdsp, int *wnp);
int		dic_clearmem(void);

#endif
