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
	memory.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#ifndef _ASK_MEMORY_H_
#define _ASK_MEMORY_H_

void	init_univmem(void);

UNIVMEM *alloc_mem(void);
void	free_mem(UNIVMEM *mem);
void	free_pwmemlist(register PWMEM *p);
void	free_chmemlist(register CHMEM *p);
void	free_bsmemlist(register BSMEM *p);
void	free_khmemlist(register KHMEM *p);
void	free_klmemlist(register KLMEM *p);

CHMEM *alloc_chmem(void);
BSMEM *alloc_bsmem(void);
PWMEM *alloc_pwmem(void);
KHMEM *alloc_khmem(void);
KLMEM *alloc_klmem(void);
#define free_chmem(cm)	free_mem((UNIVMEM *)(cm))
#define free_bsmem(bm)	free_mem((UNIVMEM *)(bm))
#define free_pwmem(pm)	free_mem((UNIVMEM *)(pm))
#define free_khmem(km)	free_mem((UNIVMEM *)(km))
#define free_klmem(kl)	free_mem((UNIVMEM *)(kl))

#endif
