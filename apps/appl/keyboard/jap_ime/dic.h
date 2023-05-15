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
	dic.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*------------------------------------------------------*/
/*		ASK3 ���� �Ŵ��� Ver. 2.00					*/
/*		��� ����									*/
/*		by Tomy Kamada, 1991-11-11				*/
/*------------------------------------------------------*/

/*		���� ������		*/
typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned long	ulong;

#include		"dictypes.h"

typedef union {
		SETU	setu;
		KATUYO	katuyo;
} SYNTAXINFO;			/* Ȱ��/���� ���� */

typedef struct {
		UCODE	*ustr;			/* �б��� u �ڵ� �� */
		ULEN	ulen;			/* �� ���� */
		LEVEL	level;			/* �ܾ� ���� */
		SYNTAX	syntax; 			/* ǰ�� ���� */
		SYNTAXINFO		syntaxinfo; 	/* Ȱ��/���� ���� */
		uchar	*kbuf;			/* '\0' �� ���Ŀ� �ٴ� �ڸ� ���� */
} NEWWD;				/* ��Ͽ��� �� */

typedef struct {
		SYNTAX	syntax; 		/* ǰ�� ���� */
		SYNTAXINFO		syntaxinfo; 	/* Ȱ�� / ���� ���� */
		LEVEL	level;			/* �ܾ� ���� */
		RTIME	rtime;			/* �� ǰ�����ִ� ���� �ð� */
} SYNREF;				/* ǰ�� ������ ���� �ð��� ����ü */

typedef short	SRLEN;	/* ǰ�� ������ ���� �ð��� ����ü �� */
#define SYNS	32		/* ���� �б�� ǰ���� ������ ���� */

typedef struct {
		ULEN	yomilen;			/* �� �ܾ��� �б� ����  */
		ENTID	entid;			/* �� ���ڵ��� �ĺ� �� */
		ULEN	used_ulen;		/* �� �ܾ� ���� �����ϴµ� ���� u �ڵ��� ���� */
		ULEN	page_ulen;		/* �� ���� �̻��� �ܾ�� 1������ �̳��� �ִ�.*/
		SRLEN	synreflen;		/* synref[] �� ��� �� */
		SYNREF	synref[SYNS];		/* �����ϴ� ǰ�� ������ �ִ� ���� �ð� */
} ENTRYS;					/* dic_snssyn() �� �����ϴ� ����ü */

typedef struct {
		WDID	wdid;			/* �ܾ� ID */
		RTIME	rtime;			/* ���� �ð� */
		LEVEL	level;			/* �ܾ� ���� */
		uchar	kbuf[1];		/* �ܾ��� �ڸ� */
} WDS;

/*		dic_???? �Լ��� ���� ��  		*/
#define DIC_OK			3
#define DIC_HASLONGER	2
#define DIC_HASMATCH	1
#define DIC_HASNONE 	0
#define DIC_BADARG		-1
#define DIC_FATAL		-2
#define DIC_ISCLOSED	-3
#define DIC_ISOPEN		-4
#define DIC_OVERFLOW	-5
#define DIC_READONLY	-6
#define DIC_NODRIVE 	-7
#define DIC_NOFILE		-8
