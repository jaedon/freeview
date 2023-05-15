/******************************************************************************
 *    (c)2011 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: whrlpool.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/17/12 11:13p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/applications/nocs3/api/sec/cryptolib/openssl/include/openssl/whrlpool.h $
 * 
 * Hydra_Software_Devel/1   7/17/12 11:13p yili
 * SW7346-555:NOCS3.0 Integration
 * 
 * 1   8/16/11 4:17p cdisc
 * SW7425-1114: adding Common DRM for 7425 into AppLibs
 * 
 * 1   4/29/11 3:00p ajutras
 * SWDTV-6858: Move openssl library to dtv_refsw and build it for 35233
 * 
 *****************************************************************************/
#ifndef HEADER_WHRLPOOL_H
#define HEADER_WHRLPOOL_H

#include <openssl/e_os2.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WHIRLPOOL_DIGEST_LENGTH	(512/8)
#define WHIRLPOOL_BBLOCK	512
#define WHIRLPOOL_COUNTER	(256/8)

typedef struct	{
	union	{
		unsigned char	c[WHIRLPOOL_DIGEST_LENGTH];
		/* double q is here to ensure 64-bit alignment */
		double		q[WHIRLPOOL_DIGEST_LENGTH/sizeof(double)];
		}	H;
	unsigned char	data[WHIRLPOOL_BBLOCK/8];
	unsigned int	bitoff;
	size_t		bitlen[WHIRLPOOL_COUNTER/sizeof(size_t)];
	} WHIRLPOOL_CTX;

#ifndef OPENSSL_NO_WHIRLPOOL
int WHIRLPOOL_Init	(WHIRLPOOL_CTX *c);
int WHIRLPOOL_Update	(WHIRLPOOL_CTX *c,const void *inp,size_t bytes);
void WHIRLPOOL_BitUpdate(WHIRLPOOL_CTX *c,const void *inp,size_t bits);
int WHIRLPOOL_Final	(unsigned char *md,WHIRLPOOL_CTX *c);
unsigned char *WHIRLPOOL(const void *inp,size_t bytes,unsigned char *md);
#endif

#ifdef __cplusplus
}
#endif

#endif
