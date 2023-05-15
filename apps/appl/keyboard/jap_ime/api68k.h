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
	api68k.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#ifndef _ASK_API68K_H_
#define _ASK_API68K_H_

typedef int		APIARG;
typedef int		APIRET;
typedef u_char	*APIPTR;
typedef short	*APISHORTPTR;

APIRET	api_funcent(APIARG	*argv);
int		apis_gaku(APIARG	av[]);
int		apig_gaku(void);
int		api_Zstrconv(APIARG	av[]);
int		api_Hstrconv(APIARG	av[]);
int		api_Rstrconv(APIARG	av[]);
int		api_Kstrconv(APIARG	av[]);
int		api_henkan(APIARG	av[]);
int		api_movekh(APIARG	av[]);
int		api_moveblk(APIARG	av[]);
int		api_allfix(APIARG	av[]);
int		api_select(APIARG	av[]);
APIRET	api_dicopen(APIARG *argv);	//int		api_dicopen(void);
APIRET	api_dicclose(APIARG *argv);	//	int		api_dicclose(void);
int		api_touroku(APIARG	av[]);
int		syn_2to3(int syn2);
int		apis_dicname(APIARG	av[]);
int		api_khlong(APIARG	av[]);
int		api_khshort(APIARG	av[]);
int		api_backbunsetu(APIARG	av[]);
int		api_nextbunsetu(APIARG	av[]);
int		api_khhenkan(APIARG	av[]);
int		apig_dicname(APIARG	av[]);
int		api_clearmem(void);
int		api_askver(void);
int		api_dicver(void);
int		api_sstrtom(APIARG	av[]);
int		api_chgkind(APIARG	av[]);
APIRET	api_none(APIARG *argv);	//int		api_none(void);

#endif
