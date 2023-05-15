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
	henkan.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
**		변환 루틴의 모드 
*/
#define KYOUSEI_HENKAN	0
#define FIXED_HENKAN	1
#define FIXEDRAW_HENKAN 2
		/*** 여기 윗쪽은 강제 변환 (입력 끝 까지 문절을 결정) **/
#define FREE_HENKAN		3

/*
**		가입력의 표시 모드
*/
#define KD_SAKIYOMI		0
#define KD_CHIKUJI		1
extern int		kd_mode;

/*
**		HCHAR 해방의 레벨
*/
#define ALL_FREE		0
#define HALF_FREE		1

/*
**		가입력 로마자 격납용
*/
typedef struct {
	BIT16	symbol;
	int		klen;
} JSYMBOL;

/*
**		문자종 변경의 종류
*/
#define CT_HIRA			1
#define CT_KATA			2
#define CT_HANKAKU		3
#define CT_ZENKAKU		4
#define CT_RAW			5
#define CT_RAWHAN		6

/*
**		문자종 변경을 어느 모드로 부터 읽을까.
*/
#define FROM_EDITING		0
#define FROM_SELECT		1
#define FROM_CHLEN		2

void	init_hchar(void);
void	clear_hchar(register HCHAR	*hp);
void	free_hchar(int spos, int epos, int level);
void	free_hchar_one(register HCHAR *hp, int level);
void	henkan(int spos, int epos, int mode, int fstlen);
int		concat_bslen(int pos, int epos);
int		muhenkan(int spos, int epos);
void	fl_check(int spos, int epos);
int		ch_check(int spos, int epos);
CHMEM	*make_chmem(int pos, ENTRYS	*pent);
void	hchar_addchmem(int pos, CHMEM	*chmem);
void	add_dummy_chmem(int pos, int ylen, SYNTAX syn);
int		bs_check(int spos, int epos);
BSMEM	*make_bsmem(int spos, int epos, CHMEM	*chmem);
void	hchar_addbsmem(int pos, BSMEM *bsmem);
void	unify_bsmem(int pos, int len);
int		bunsetu_len(int pos);
void	save_fst_bslen(int pos);
PWORD	*srch_pword(u_char *ustr, int ulen, int *pused);
PWMEM	*pword_list(int spos, int epos, u_char sn, u_char kt);
int		not_bhead(u_char uc);
int		is_kuten(u_char uc);
