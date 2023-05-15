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
	dic_temp.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#ifndef _ASK_DIC_TEMP_H_
#define _ASK_DIC_TEMP_H_

typedef struct node_entry {
		uchar	*ptr;			/* 레코드 에의 포인터 */
		struct node_entry *nxt; /* 다음에의 포인터 */
} NODE;


void	init_temp(void);
void	flush_temp(void);
void	init_node_tab(void);
void	init_hash_tab(void);
void	reset_temp(void);
int		hashfunc(UCODE	*yomi);
NODE	*alloc_node(void);
void	free_node(NODE	*p);
uchar	*alloc_record(int len);
NODE	**srch_node(UCODE	*yomi, ULEN ulen, NODE	**npp);
void	page_gc(void);
int		tmpoffset(uchar *p);
uchar	*load_temp(int tmp);
int		read_temp(void);
int		write_temp(void);
int		newwdlen(NEWWD	*newp);
int		updwdlen(NEWWD	*newp);
void	set_record(uchar *s, int len, NEWWD	*newp, int	rtime);
void	upd_record(uchar *s, int len, NEWWD	*newp, int rtime);
int		tmp_touroku(UCODE	*yomi, NEWWD	*newp, int rtime);
int		tmp_snssyn(UCODE	*yomi, ULEN	ulen, RECINFO *recp);
int		tmp_getsyn(UCODE	*yomi, ULEN	 ulen, RECINFO *recp);
void	shift_temp(int tmp);
void	update_nowtmp(void);
int		setkbuflen(uchar *a);
void	setkbuf(uchar *a, uchar *w);
int		getkbuflen(uchar *w, uchar *wend);
void	getkbuf(uchar *a, uchar *w, uchar *wend);
int		iskanji(uchar c);

#endif

