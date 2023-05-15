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
	askdic.c (AVE-Front Version 2 ASK Dictionary ROM Interface)
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


#include <stdio.h>
#include <string.h>
#include "iaskdic.h"

#define zmemcpy		(memcpy)

/* ASK사전의 ROM데이터 */
static const unsigned char gROM_ASK[] = {
#ifdef ASK_DIC_SMALL
	#include "ask_s.h"
#endif
#ifdef ASK_DIC_NORMAL
	#include "ask_n.h"
#endif
};

typedef struct {
	long	length;
	char	*offset;
} FAskRom;

static FAskRom gAskRom;


/* ASK사전 ROM 제어 함수 */
int
FAskRom_Open(char *in_filename, int in_mode)
{
	gAskRom.length = sizeof(gROM_ASK);
	gAskRom.offset = (char*)gROM_ASK;

	/* 파일 핸들의 더미 */
	return 1;
}

int
FAskRom_Close(int in_stream)
{
	/* 항상 성공한다. */
	return 0;
}

int
FAskRom_Read(int in_stream, unsigned char *out_buf, int in_buflen)
{
	int s;

	s = gAskRom.length;
	if (s >= in_buflen)
		s = in_buflen;

	zmemcpy(out_buf, gAskRom.offset, s);

	gAskRom.length -= s;
	gAskRom.offset += s;

	return s;
}

int
FAskRom_Write(int in_stream, unsigned char *in_buf, int in_buflen)
{
	int len;

	len = gAskRom.length;
	if (len > in_buflen)
		len = in_buflen;
	zmemcpy(gAskRom.offset, in_buf, in_buflen);
	gAskRom.length -= len;
	gAskRom.offset += len;
	return len;
}

long
FAskRom_Seek(int in_stream, long in_posx, int in_mdx)
{
	long l;

	switch (in_mdx) {
		case SEEK_SET:
			if (in_posx < 0)
				return -1;
			gAskRom.length = sizeof(gROM_ASK) - in_posx;
			gAskRom.offset = (char*)gROM_ASK + in_posx;
			break;
		case SEEK_CUR:
			l = gAskRom.length - in_posx;
			if (l < 0 || l > sizeof(gROM_ASK))
				return -1;
			gAskRom.length = l;
			gAskRom.offset += in_posx;
			break;
		case SEEK_END:
			if (in_posx > 0)
				return -1;
			gAskRom.length = -in_posx;
			gAskRom.offset = ((char*)gROM_ASK + sizeof(gROM_ASK) - 1) + in_posx;
			break;
	}

	return (gAskRom.offset - (char*)gROM_ASK);
}
