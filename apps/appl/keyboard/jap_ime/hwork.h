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
	hwork.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include		"dictypes.h"

typedef unsigned short	PRIOR;

typedef struct _chmem {
		short	yomilen;				/* 2 */
		SYNTAX	ch_syn;					/* 1 */
		KATUYO	ch_ktu;					/* 1 */
		ENTID	entid;					/* 4 */
		RTIME	maxprior;				/* 2 */
		struct _chmem	*ch_next;		/* 2 */
} CHMEM;

typedef struct _bsmem {
		short	bs_len;					/* 2 */
		SYNTAX	bs_syn;					/* 1 */
		KATUYO	bs_ktu;					/* 1 */
		CHMEM	*wd_mem;				/* 2 */
		PRIOR	itsprior;				/* 2 */
		PRIOR	ctdprior;				/* 2 */
		struct _bsmem	*bs_next;		/* 2 */
} BSMEM;

typedef struct _pwmem {
		short	pw_len;					/* 2 */
		u_char	fst_sn;					/* 1 */
		u_char	fst_kt;					/* 1 */
		u_char	lst_sn;					/* 1 */
		u_char	lst_kt;					/* 1 */
		struct _pwmem	*fnext;			/* 2 */
} PWMEM;

#define KHLEN			4
typedef struct _khmem {
		u_char	khtype;					/* 1 */
		u_char	khbuf[KHLEN + 1];		/* 5 */
		u_char	chlen;					/* 1 */
		LEVEL	level;					/* 1 */
		CHMEM	*wd_mem;				/* 2 */
		WDID	wdid;					/* 2 */
		RTIME	kh_rtime;				/* 2 */
		struct _khmem	*kh_next;		/* 2 */
} KHMEM;
/*
**		Definitions for 'khtype'
**
**		2亮枰� 이하의 KHMEM 은,
**				struct {
**						u_char	khtype;
**						u_char	khbuf[KHLENX];
**						struct _khmem	*kh_next;
**				};
**		으로써 사용된다. 
**
**		'khtype' 의 예 가능성
**				0					// 1개의 khbuf 에서 만족함
**				0x01 - 0x02			// 2개의 khbuf 를 사용 
**				0x01 - 0x03 - .... - 0x03 - 0x02 // 2개 이상의 khbuf 를 사용
**				0x80					// 후보는, 입력 그대로. 
*/
#define KH_NORMAL		0
#define KH_CONTINUE		0x01
#define KH_SECOND		0x02
#define KH_ROMAN		0x80
#define KHLENX			(short)(KHLEN + sizeof(u_char) + sizeof(LEVEL) + sizeof(CHMEM *) + sizeof(WDID) + sizeof(RTIME))

typedef struct _klmem {
		BSMEM	*bnst;					/* 대응하는 문절 */
		PRIOR	maxpri;					/* 최대의 우선도 */
		KHMEM	*khlist;				/* 후보의 리스트 */
		struct _klmem	*kl_next;		/* 다음 문절 예 */
} KLMEM;

typedef union _univmem {
		CHMEM	_c;
		BSMEM	_b;
		PWMEM	_p;
		KHMEM	_k;
		KLMEM	_kl;
		union _univmem	*uv_next;
} UNIVMEM;

typedef struct {
		PWORD	*f_list;
		CHMEM	*ch_mem;
		BSMEM	*bs_mem;
		KHMEM	*kh_mem;
		KLMEM	*kl_mem;
		char	merged;				/* 무자문절의 머지가 끝났다. */
		char	bnstlen;				/* 현재의 문절 길이 */
		char	fst_bslen;				/* 최초의 문절 길이 */
		char	w_usedlen;				/* 1 */
		char	f_usedlen;
		char	page_ulen;
} HCHAR;

#define HCHAR_SIZE		80
extern HCHAR	hchar[];

