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
	memory.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include	"askdef.h"
#include	"pword.h"
#include	"hwork.h"
#include	"memory.h"
#include	"dic.h"
#include	"dictypes.h"
#include	"dicdef.h"
#include	"dic_main.h"

#if 0
#define HCHAR_SIZE		80
HCHAR	hchar[HCHAR_SIZE];
#endif

#if 0 /* Changed By J.Katoh.1999/05/06 */
#define MEM_SIZE		1000
#else
#define MEM_SIZE		3000
	/* 	"카" 35문자를 변환 -> 이전 후보했을 때, 약 1100개 사용하므로 
		걁MEM_SIZE = 2000일 때 sizeof(mem) = 40000bytes)		*/
#endif
UNIVMEM mem[MEM_SIZE];
UNIVMEM *free_univ;

#define DEBUG			1
#ifdef	DEBUG
int		mem_count;
#endif

void init_univmem(void)
{
	static int first_init = 1;
	register UNIVMEM *p;

	if (!first_init && mem_count == MEM_SIZE)
		return;
	free_univ = mem;
	for (p = mem; p < &mem[MEM_SIZE - 1]; p++) {
		p->uv_next = p + 1;
	}
	p->uv_next = NULL;

#ifdef	DEBUG
	mem_count = MEM_SIZE;
#endif
	first_init = 0;
}

UNIVMEM *alloc_mem(void)
{
	UNIVMEM *ret;

	if ((ret = free_univ) == NULL) {
		return(NULL);
	}
	free_univ = free_univ->uv_next;

#ifdef	DEBUG
	mem_count--;
#endif
	return(ret);
}


void free_mem(UNIVMEM *mem)
{
	if (mem == NULL)
		return;
	mem->uv_next = free_univ;
	free_univ = mem;

#ifdef	DEBUG
	mem_count++;
#endif
}

CHMEM *alloc_chmem(void)
{
	UNIVMEM *m = alloc_mem();
	return (m != NULL) ? &m->_c : NULL;
}

BSMEM *alloc_bsmem(void)
{
	UNIVMEM *m = alloc_mem();
	return (m != NULL) ? &m->_b : NULL;
}

PWMEM *alloc_pwmem(void)
{
	UNIVMEM *m = alloc_mem();
	return (m != NULL) ? &m->_p : NULL;
}

KHMEM *alloc_khmem(void)
{
	UNIVMEM *m = alloc_mem();
	return (m != NULL) ? &m->_k : NULL;
}

KLMEM *alloc_klmem(void)
{
	UNIVMEM *m = alloc_mem();
	return (m != NULL) ? &m->_kl : NULL;
}


void free_pwmemlist(register PWMEM *p)
{
	PWMEM	*pnext;

	while (p) {
		pnext = p->fnext;
		free_pwmem(p);
		p = pnext;
	}
}

void free_chmemlist(register CHMEM *p)
{
	CHMEM *pnext;
	ENTID dup = (u_long)-1;

	while (p) {
		pnext = p->ch_next;
		if (dup == -1 || p->entid != dup) {
			if (p->entid != -1)
				dic_freeentid(p->entid);
		}
		dup = p->entid;
		free_chmem(p);
		p = pnext;
	}
}


void free_bsmemlist(register BSMEM *p)
{
	BSMEM *pnext;

	while (p) {
		pnext = p->bs_next;
		free_bsmem(p);
		p = pnext;
	}
}

void free_khmemlist(register KHMEM *p)
{
	KHMEM *pnext;

	while (p) {
		pnext = p->kh_next;
		free_khmem(p);
		p = pnext;
	}
}

void free_klmemlist(register KLMEM *p)
{
	KLMEM *pnext;

	while (p) {
		pnext = p->kl_next;
		free_klmem(p);
		p = pnext;
	}
}
