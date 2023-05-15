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
	kouho.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#ifndef _ASK_KOUHO_H_
#define _ASK_KOUHO_H_

void	first_kouho(int pos, int len);
void	init_kouho(int kh_pos, int mode);
void	all_kouho(void);
KHMEM	*get_kouholist(BSMEM	*bsm);
void	free_kouholists(register KLMEM	*kls);
KHMEM	*null_kouho(int ylen);
KHMEM	*create_kouho(WDS *wdsp, CHMEM	*chm, int ylen, KHMEM **pkhs, KHMEM	**pkhe);
int		kstrncpy(register u_char *d, register u_char *s, int n);
KHMEM	*raw_kouho(int spos, int ylen, int type);
void	khmem_raw(int type);
void	kh_mergesort(int pos, KLMEM	*klist);
KHMEM	*kh_skip(register KHMEM	*kh);
KHMEM	*kh_followed(register KHMEM	*kh);
KHMEM	*kh_endof(register KHMEM	*kh);
int		kh_count(KHMEM	*kh);
int		kh_length(register KHMEM	*kh);
KHMEM	*take_kouho(KHMEM *kp, int n);
void	disp_kouho(void);
int		kouho_makedisp(int pos, int len, KHMEM *kouho, MEAN	*mbuf);
int		jiritu_makedisp(KHMEM *kouho, MEAN *mbuf);
int		inc_gun(KHMEM	*kh);
int		next_gun(int sw, int wrap);
int		back_gun(int sw, int wrap);
int		next_kouho(int sw, int wrap);
int		back_kouho(int sw, int wrap);

#endif

