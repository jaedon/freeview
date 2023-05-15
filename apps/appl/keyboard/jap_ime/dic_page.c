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
	dic_page.c
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/



#include	<string.h>		/* Nag */

#include	"askdef.h"
#include	"dic.h"
#include	"dicdef.h"
#include	"iaskdic.h"
#include	"dic_page.h"


/*------------------------------*/
/*		�����\��				*/
/*------------------------------*/
uchar	header[HEADERSIZE];				/* �����w�b�_ */
uchar	*entry2code = header+78;		/* �Q�����G���g�� */
int		entry2upd = 0;					/* �Q�����G���g���X�V��� */
int		mainupper;						/* ���C�������̑��y�[�W�� */
int		mainlower;						/* ���C�������̍ŏ��̃y�[�W(=0) */
uchar	mainindex[PAGEMAX][INDEXLEN];	/* ���C�������C���f�b�N�X */

#define PAGES			10

#define EMPTY			-1

PAGE	page_tab[PAGES];		/* �y�[�W�e�[�u�� */
PAGE	*page_top;				/* �J�����g�y�[�W  */

/*------------------------------*/
/*		ENTID�e�[�u���\��		*/
/*------------------------------*/
#define ENTIDMAX		128
struct entid_entry {
		uchar	cnt;			/* ENTID �̃��t�@�����X�J�E���g */
		uchar	maxtime;		/* ���R�[�h���̍ő� RTIME */
		ushort	page;			/* �y�[�W�ԍ� */
#if 0 /* Changed By Ogawa. For henkan bug. 1997/10/21 */
		ushort	offset;			/* ���R�[�h�̃y�[�W���I�t�Z�b�g */
		ushort	tmp;			/* �ꎞ�w�K */
#else
		short	offset;			/* ���R�[�h�̃y�[�W���I�t�Z�b�g */
		short	tmp;			/* �ꎞ�w�K */
#endif
};
struct	entid_entry entid_tab[ENTIDMAX];
static	char	mydicname[65];


int d_open(char *mydicname, int dic_rw)
{
#ifdef ASK_ROMDIC
	return FAskRom_Open(mydicname, dic_rw);
#else
	return _open(mydicname, dic_rw);
#endif
}

int d_read(int fn,void *buf,int size)
{
#ifdef ASK_ROMDIC
	return FAskRom_Read(fn, buf, size);
#else
	return _read(fn, buf, size);
#endif
}

int d_write(int fn,void *buf,int size)
{
#ifdef ASK_ROMDIC
	return FAskRom_Write(fn, buf, size);
#else
	return _write(fn,buf,size);
#endif
}

int	d_close(int fn)
{
#ifdef ASK_ROMDIC
	return FAskRom_Close(fn);
#else
	return _close(fn);
#endif
}

long d_seek(int fn,long posx,int mdx)
{
#ifdef ASK_ROMDIC
	long	curx;

	curx = FAskRom_Seek(fn, posx, mdx);
	return(curx);
#else
	long	curx;

	curx = _lseek(fn,posx,mdx);
	return(curx);
#endif
}

/*--------------------------------------*/
/*		�����̃I�[�v���E�N���[�Y		*/
/*--------------------------------------*/
void set_dicname(uchar *name)
{
	strncpy((char *)mydicname, (char *)name, 64);
	mydicname[64]='\0';
}

HAND open_dic(void)
{
	int hand;
	if ((hand=d_open(mydicname, dic_rw)) == -1) {
		dic_fd = CLOSE;
		return(ERR);
	}
	dic_fd = hand;
	return(OK);
}


int close_dic(void)
{
	if (d_close(dic_fd) == -1) {
		dic_fd = CLOSE;
		return(ERR);
	}
	dic_fd = CLOSE;
	return(OK);
}


int seek_dic(long offset)
{
	if (d_seek(dic_fd, offset, 0) == -1L) {
		if (open_dic() != OK)
			return(ERR);
		if (d_seek(dic_fd, offset, 0) == -1L)
			return(ERR);
	}		
	return(OK);
}

/*--------------------------------------*/
/*		�y�[�W�֌W������				*/
/*--------------------------------------*/
void init_page(void)
{
	init_page_tab();
	init_entid_tab();
}

/*--------------------------------------*/
/*		�w�b�_��ǂݍ���				*/
/*--------------------------------------*/
char title[] = "ASK Copyright (C) ACCESS CO.,LTD.";
static int old_temp, /*old_gaku,*/ old_suji;

int read_head(void)
{
	uchar *s;

	if (seek_dic(0L) == ERR)
		return(ERR);

	if (d_read(dic_fd, header, HEADERSIZE) != HEADERSIZE)
		return(ERR);

	s = header;
	if (strncmp((char *)s, (char *)title, strlen(title)) != 0)
		return(ERR);

	s += 46;
	mainlower = 0;
	mainupper = *s + (*(s+1))*256;
	s += 2;
	old_temp = temp_page = *s + (*(s+1))*256;
	s += 2;
	s += 2;
	old_suji = suji_mode = *s + (*(s+1))*256;
	entry2upd = 0;

	return(OK);
}

/*--------------------------------------*/
/*		�w�b�_����������				*/
/*--------------------------------------*/
int flush_head(void)
{
	uchar *s = header+48;

	if (temp_page == old_temp
			&& suji_mode == old_suji
			&& entry2upd == 0)
		return(OK);

	*s++ = (unsigned char)(temp_page % 256);
	*s++ = (unsigned char)(temp_page / 256);
	*s++ = (unsigned char)(gaku_mode % 256);
	*s++ = (unsigned char)(gaku_mode / 256);
	*s++ = (unsigned char)(suji_mode % 256);
	*s++ = (unsigned char)(suji_mode / 256);

	if (seek_dic(0L) == ERR)
		return(ERR);

	if (d_write(dic_fd, header, HEADERSIZE) != HEADERSIZE)
		return(ERR);

	return(OK);
}

/*--------------------------------------*/
/*		�C���f�b�N�X��ǂݍ���			*/
/*--------------------------------------*/
int read_index(void)
{
	if (seek_dic((long)INDEXOFFSET) == ERR)
		return(ERR);

	if (d_read(dic_fd, (char*)mainindex, INDEXSIZE) != INDEXSIZE)
		return(ERR);

	return(OK);
}

/*--------------------------------------*/
/*		�Q�����G���g���̃`�F�b�N		*/
/*--------------------------------------*/
int chk_entry2(UCODE *ucode, ULEN ulen)
{
	int code1, code2, offset;

	if (*ucode < 0xa1)
		return(0);		/* �e�[�u���O */
	code1 = *ucode++ - 0xa1;
	if (ulen == 1)
		code2 = 0x00;
	else if (*ucode < 0xa1)
		return(0);		/* �e�[�u���O */
	else
		code2 = *ucode - 0xa0;
	offset = code1 * 11 + (code2 >> 3);
	if (entry2code[offset] & (1 << (code2 & 0x07)))
		return(0);		/* ���̒P��͎����ɑ��݂��� */
	else
		return(1);		/* ���̒P��͎����ɑ��݂��Ȃ� */
}


void set_entry2(UCODE *ucode, ULEN ulen)
{
	int code1, code2, offset;

	if (*ucode < 0xa1)
		return;			/* �e�[�u���O */
	code1 = *ucode++ - 0xa1;
	if (ulen == 1)
		code2 = 0x00;
	else if (*ucode < 0xa1)
		return;			/* �e�[�u���O */
	else
		code2 = *ucode - 0xa0;
	offset = code1 * 11 + (code2 >> 3);
	entry2code[offset] |= (1 << (code2 & 0x07));
	entry2upd = 1;
}

/*--------------------------------------*/
/*		�ǂ݂���y�[�W����������		*/
/*--------------------------------------*/
int srch_page(UCODE *ucode)
{
	int mid, up, low;
	int dif;

	up = mainupper;
	low = mainlower;
	while (low+1 < up) {
		mid = (up + low) / 2;
		if ((dif=strncmp((char *)ucode, (char *)mainindex[mid], INDEXLEN)) == 0)
			return(mid);
		else if (dif > 0)
			low = mid;
		else /* dif < 0 */
			up = mid;
	}
	if (strncmp((char *)ucode, (char *)mainindex[low], INDEXLEN) < 0)
		return(low - 1);
	return(low);
}

/*--------------------------------------*/
/*		�y�[�W�m�肵�Ă��邩�ǂ����H	*/
/*--------------------------------------*/
int page_fix(int page, UCODE *ucode)
{
	int plen;
		
	plen = prefix(mainindex[page+1], ucode, INDEXLEN);
	if (ucode[plen] == '\0')
		return(NO);
	return(YES);
}

/*--------------------------------------*/
/*		�y�[�W���ɂ����͒����H�ȏ�	*/
/*--------------------------------------*/
int calc_pulen(int page, UCODE *ucode)
{
	int plen;

	if ((plen=prefix(mainindex[page], ucode, INDEXLEN)) >= INDEXLEN)
		return(plen);

	if (ucode[plen] == '\0' )
		return(plen);

	return(plen + 1);
}

/*------------------------------------------------------*/
/*		������ a �� b �̉��o�C�g�܂ł�����������Ԃ��B	*/
/*		�������A���� len �܂ł�����r���Ȃ��B			*/
/*------------------------------------------------------*/
int prefix(uchar *a, uchar *b, int len)
{
	int i;

	for(i=0; i<len; i++, a++, b++)
		if (*a != *b || *a == '\0')
			break;
	return(i);
}

/*--------------------------------------*/
/*		�y�[�W�e�[�u���̏�����			*/
/*--------------------------------------*/
void init_page_tab(void)
{
	register PAGE *pt;

	page_top = page_tab;
	for (pt=page_tab; pt < &page_tab[PAGES-1]; pt++) {
		pt->pageno = EMPTY;
		pt->updated = NO;
		pt->nxt = pt+1;
	}
	pt->pageno = EMPTY;
	pt->updated = NO;
	pt->nxt = NULL;
}

/*--------------------------------------*/
/*		�y�[�W�̂�ǂݏ���				*/
/*--------------------------------------*/
int write_page(PAGE *pt)
{
	long offset;

	offset = PAGEOFFSET + (long)pt->pageno * (long)PAGEMAX;
	if (seek_dic(offset) == ERR)
		return(ERR);
	if (d_write(dic_fd, pt->pagebuf, PAGEMAX) != PAGEMAX)
		return(ERR);
	return(OK);
}

int read_page(PAGE *pt)
{
	long offset;

	offset = PAGEOFFSET + (long)pt->pageno * (long)PAGEMAX;
	if (seek_dic(offset) == ERR)
		return(ERR);
	d_read(dic_fd, pt->pagebuf, PAGEMAX);
	return(OK);
}

uchar *load_page(int page)
{
	register PAGE *pt, *prev;

	prev = NULL;
	pt = page_top;
	for ( ;; ) {
		if (pt->pageno == page) {
			/* ���łɃy�[�W�e�[�u���ɂ����� */
			/* �擪�Ɏ����Ă���				*/
			if (prev != NULL) {
				prev->nxt = pt->nxt;
				pt->nxt = page_top;
				page_top = pt;
			}
			return(pt->pagebuf);
		}
		else if (pt->nxt == NULL)		/* prev -> �Ō�� PAGE */
			break;
		prev = pt;
		pt = pt->nxt;
	}

	if (prev != NULL)
	{
		prev->nxt = NULL;
	}
	pt->nxt = page_top;
	page_top = pt;

	if (pt->updated == YES) {
		write_page(pt);
	}
	pt->pageno = page;
	pt->updated = NO;
	read_page(pt);
	return(pt->pagebuf);
}

/*--------------------------------------*/
/*		�J�����g�y�[�W���X�V�A����		*/
/*--------------------------------------*/
void update_nowpage(void)
{
	page_top->updated = YES;
}

/*--------------------------------------*/
/*		�X�V�y�[�W����������			*/
/*--------------------------------------*/
void flush_pages(void)
{
	register PAGE	*pt;

	for (pt = page_top; pt != NULL; pt = pt->nxt)
		if (pt->updated == YES) 
			write_page(pt);
}

/*--------------------------------------*/
/*		ENTID�e�[�u���̏�����			*/
/*--------------------------------------*/
void init_entid_tab(void)
{
	register int i;

	for (i=0; i < ENTIDMAX; i++)
		entid_tab[i].cnt = 0;
}

/*--------------------------------------*/
/*		ENTID�e�[�u���ւ̓o�^			*/
/*--------------------------------------*/
int set_entid_tab(int page, int offset, int tmp, int maxtime)
{
	register int i;
	register struct entid_entry *pe;
	int free_i;

	pe = entid_tab;
	free_i = -1;
	for (i=0; i < ENTIDMAX; i++, pe++)
		if (pe->cnt == 0) {
			if (free_i == -1)
				free_i = i;
		} else if (pe->page == page &&
					pe->offset == offset && pe->tmp == tmp) {
			pe->cnt++;
			return(i);
		}
	if (free_i == -1)
		return(ERR);
	pe = &entid_tab[free_i];
	pe->cnt = 1;
	pe->page = (unsigned short)page;
#if 0 /* Changed By Ogawa. For henkan bug. 1997/10/21 */
	pe->offset = (unsigned short)offset;
	pe->tmp = (unsigned short)tmp;
#else
	pe->offset = (short)offset;
	pe->tmp = (short)tmp;
#endif
	pe->maxtime = (unsigned char)maxtime;

	return(free_i);
}

/*--------------------------------------*/
/*		ENTID�e�[�u�����Q��				*/
/*		�y�[�W�ԍ���Ԃ�				*/
/*--------------------------------------*/
int get_entid_tab(ENTID entid, int *poff, int *ptmp, int *ptime)
{
	register struct entid_entry *pe;

	if (entid < 0 || entid >= ENTIDMAX)
		return(ERR);
	pe = &entid_tab[(int)entid];
	if (pe->cnt == 0)
		return(ERR);
#if 0 /* Changed By Ogawa. For henkan bug. 1997/10/21 */
	*poff = pe->offset;
	*ptmp = pe->tmp;
#else
	*poff = (int)pe->offset;
	*ptmp = (int)pe->tmp;
#endif
	*ptime = pe->maxtime;
	return(pe->page);
}

/*--------------------------------------*/
/*		ENTID�e�[�u���� maxtime ���擾	*/
/*--------------------------------------*/
int get_maxtime(ENTID	entids[], int n)
{
	register int i, id;
	int maxtime = 0;

	for (i=0; i<n; i++) {
		id = entids[i];
		if (id < 0 || id >= ENTIDMAX)
			continue;
		if (entid_tab[id].cnt > 0) {
			if (maxtime < entid_tab[id].maxtime)
				maxtime = entid_tab[id].maxtime;
		}
	}
	return(maxtime);
}

/*--------------------------------------*/
/*		ENTID�e�[�u���� maxtime ���X�V	*/
/*--------------------------------------*/
int update_entid_rtime(ENTID entids[], int n, int maxtime)
{
	register int i, id;

	for (i=0; i<n; i++) {
		id = entids[i];
		if (id < 0 || id >= ENTIDMAX)
			continue;
		if (entid_tab[id].cnt > 0)
			entid_tab[id].maxtime = (unsigned char)maxtime;
	}
	return(OK);
}

/*--------------------------------------*/
/*		ENTID�e�[�u���̉��				*/
/*--------------------------------------*/
int free_entid_tab(ENTID entid)
{
	register struct entid_entry *pe;

	if (entid < 0 || entid >= ENTIDMAX)
		return(ERR);
	pe = &entid_tab[(int)entid];
	if (pe->cnt == 0)
		return(ERR);
	pe->cnt--;
	return(OK);
}

/*--------------------------------------*/
/*		ENTID�e�[�u���� tmp ���X�V		*/
/*--------------------------------------*/
void clear_entid_tmpall(int tmppage)
{
	/* �ꎞ�P��y�[�W tmppage ���N���A */
	/* ���Ɏ擾����Ă��� ENTID, WDID �Ŋw�K���Ă�
	   ���̎��_�ł͂������݂��Ȃ����Ƃ�����B */

	register int i;

	for (i=0; i < ENTIDMAX; i++) {
		if (TEMP_PAGE(entid_tab[i].tmp) == tmppage)
#if 0 /* Changed By Ogawa. For henkan bug. 1997/10/21 */
			entid_tab[i].tmp = (unsigned short)NONE;
#else
			entid_tab[i].tmp = NONE;
#endif
	}
}

void clear_entid_tmp(int tmp)
{
	/* �ꎞ�P�ꃌ�R�[�h tmp ���N���A */
	/* ���Ɏ擾����Ă��� ENTID, WDID �Ŋw�K���Ă�
	   ���̎��_�ł͂������݂��Ȃ����Ƃ�����B */

	register int i;

	for (i=0; i < ENTIDMAX; i++) {
		if (entid_tab[i].tmp == tmp)
#if 0 /* Changed By Ogawa. For henkan bug. 1997/10/21 */
			entid_tab[i].tmp = (unsigned short)NONE;
#else
			entid_tab[i].tmp = NONE;
#endif
	}
}
