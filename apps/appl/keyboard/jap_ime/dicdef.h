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
	dicdef.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*		���� ����			*/
#ifndef NULL
#define NULL	((void*)0)
#endif

#define YES		1
#define NO		0

#define OK		0
#define ERR		-1

#define ON		1
#define OFF		0

/*		���� ���� �ڵ� 	*/
typedef int		HAND;			/* ���� �ڵ� */
#define 	CLOSE	-1				/* Ŭ���� �Ǿ� �ִ�. */
extern	HAND	dic_fd;			/* ������ ���� ������  */
extern	int		dic_rw;			/* READ or RDWR */

/*		�б�(u�ڵ�)�� ����	*/
#define ULENMAX			80

/*		���ڵ� ���� ��ũ��		*/
#define RECORD_LEN(p)	((ushort)(*p + (*(p+1))*256))
#define UCODE_LEN(p)	(*(p+2))
#define UCODE_STR(p)	(p+3)
#define WD_STR(p)		(p + 3 + *(p+2))
#define IS_SYNTAX(c)			(c <= 56)
#define IS_SYNTAXINFO(c)		(c <= 11)

/*		WDS �� ������		*/
#define SIZEWDS			(short)(sizeof(WDID)+sizeof(RTIME)+sizeof(LEVEL))

/*		�ִ� RTIME				*/
#define RTIMEMAX		255

/*		�ܾ� ID 					*/
#define MAIN	0x0000
#define TEMP	0x8000

/*		�н� ��� 				*/
extern	int		gaku_mode;

/*		���� ��ȯ ���				*/
extern	int		suji_mode;

/*		Ŀ��Ʈ �ӽ� �ܾ� ������ 	*/
extern	int		temp_page;

/*		���ڵ� �˻� ����ü			*/
#define NONE	-1
typedef struct recinfo_entry {
/*		int		page;			* ���ڵ� ������  */
/*		int		offset;			* ���ڵ��� ������ (���� �� NONE) */
		short		page;			/* ���ڵ� ������  */
		short		offset;			/* ���ڵ��� ������ (���� �� NONE) */
		ULEN	yomilen;			/* ENTRYS �� ���� �ǹ� */
		ULEN	used_ulen;		/* ENTRYS �� ���� �ǹ� */
		ULEN	page_ulen;		/* ENTRYS �� ���� �ǹ� */
		uchar	*ptr;			/* ���ڵ� ���� ������ */
} RECINFO;

/*		������ ���� 				*/
#define PAGEMAX			1024
#define HEADERSIZE		1024
#define INDEXLEN		4
#define INDEXSIZE		INDEXLEN*PAGEMAX
#define TEMPPAGES		8
#define TEMPSIZE		TEMPPAGES*PAGEMAX
#define INDEXOFFSET		HEADERSIZE
#define TEMPOFFSET		INDEXOFFSET+INDEXSIZE
#define PAGEOFFSET		TEMPOFFSET+TEMPSIZE

/*		�Ͻ� �ܾ� ������		*/
/*		PPPP OOOO OOOO OOOO		*/
#define TEMP_PAGE(x)	((short)(x >> 12))
#define TEMP_OFFSET(x)	((short)(x & 0x0FFF))

/*		���� �׼��� 			*/
#ifdef	X68K
#define READ			0x0000
#define WRITE			0x0001
#define RDWR			0x0002
#else	/* PC98 */
#define READ			0x8000
#define WRITE			0x8001
#define RDWR			0x8002
#endif
