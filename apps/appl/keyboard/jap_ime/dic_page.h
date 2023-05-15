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
	dic_page.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#ifndef _ASK_DIC_PAGE_H_
#define _ASK_DIC_PAGE_H_

typedef struct page_entry {
		int		pageno;					/* 페이지 번호 */
		int		updated;					/* 갱신되었는지.. */
		struct 	page_entry		*nxt;	/* 페이지 Queue */
		uchar	pagebuf[PAGEMAX];		/* 페이지의 내용 */
} PAGE;


int d_open(char *mydicname, int dic_rw);
int d_read(int fn,void *buf,int size);
int d_write(int fn,void *buf,int size);
int	d_close(int fn);
long d_seek(int fn,long posx,int mdx);

void	set_dicname(uchar *name);

HAND	open_dic(void);

int		close_dic(void);
int		seek_dic(long offset);
void	init_page(void);

int		read_head(void);
int		flush_head(void);
int		read_index(void);
int		chk_entry2(UCODE *ucode, ULEN ulen);
void	set_entry2(UCODE *ucode, ULEN ulen);
int		srch_page(UCODE *ucode);
int		page_fix(int page, UCODE *ucode);
int		calc_pulen(int page, UCODE *ucode);
int		prefix(uchar *a, uchar *b, int len);
void	init_page_tab(void);
int		write_page(PAGE *pt);
int		read_page(PAGE *pt);
uchar	*load_page(int page);
void	update_nowpage(void);
void	flush_pages(void);
void	init_entid_tab(void);
int		set_entid_tab(int page, int offset, int tmp, int maxtime);
int		get_entid_tab(ENTID entid, int *poff, int *ptmp, int *ptime);
int		get_maxtime(ENTID	entids[], int n);
int		update_entid_rtime(ENTID	entids[], int n, int maxtime);
int		free_entid_tab(ENTID entid);
void	clear_entid_tmpall(int tmppage);
void	clear_entid_tmp(int tmp);

#endif

