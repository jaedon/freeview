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
 * $brcm_Workfile: opensslv.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/17/12 11:26p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/applications/nocs3/api/sec/cryptolib/openssl/include/openssl/opensslv.h $
 * 
 * Hydra_Software_Devel/1   7/17/12 11:26p yili
 * SW7346-555:NOCS3.0 Integration
 * 
 * 1   8/16/11 4:16p cdisc
 * SW7425-1114: adding Common DRM for 7425 into AppLibs
 * 
 * 1   4/29/11 3:00p ajutras
 * SWDTV-6858: Move openssl library to dtv_refsw and build it for 35233
 * 
 *****************************************************************************/
#ifndef HEADER_OPENSSLV_H
#define HEADER_OPENSSLV_H

/* Numeric release version identifier:
 * MNNFFPPS: major minor fix patch status
 * The status nibble has one of the values 0 for development, 1 to e for betas
 * 1 to 14, and f for release.  The patch level is exactly that.
 * For example:
 * 0.9.3-dev	  0x00903000
 * 0.9.3-beta1	  0x00903001
 * 0.9.3-beta2-dev 0x00903002
 * 0.9.3-beta2    0x00903002 (same as ...beta2-dev)
 * 0.9.3	  0x0090300f
 * 0.9.3a	  0x0090301f
 * 0.9.4 	  0x0090400f
 * 1.2.3z	  0x102031af
 *
 * For continuity reasons (because 0.9.5 is already out, and is coded
 * 0x00905100), between 0.9.5 and 0.9.6 the coding of the patch level
 * part is slightly different, by setting the highest bit.  This means
 * that 0.9.5a looks like this: 0x0090581f.  At 0.9.6, we can start
 * with 0x0090600S...
 *
 * (Prior to 0.9.3-dev a different scheme was used: 0.9.2b is 0x0922.)
 * (Prior to 0.9.5a beta1, a different scheme was used: MMNNFFRBB for
 *  major minor fix final patch/beta)
 */
#define OPENSSL_VERSION_NUMBER	0x1000004fL
#ifdef OPENSSL_FIPS
#define OPENSSL_VERSION_TEXT	"OpenSSL 1.0.0d-fips 8 Feb 2011"
#else
#define OPENSSL_VERSION_TEXT	"OpenSSL 1.0.0d 8 Feb 2011"
#endif
#define OPENSSL_VERSION_PTEXT	" part of " OPENSSL_VERSION_TEXT


/* The macros below are to be used for shared library (.so, .dll, ...)
 * versioning.  That kind of versioning works a bit differently between
 * operating systems.  The most usual scheme is to set a major and a minor
 * number, and have the runtime loader check that the major number is equal
 * to what it was at application link time, while the minor number has to
 * be greater or equal to what it was at application link time.  With this
 * scheme, the version number is usually part of the file name, like this:
 *
 *	libcrypto.so.0.9
 *
 * Some unixen also make a softlink with the major verson number only:
 *
 *	libcrypto.so.0
 *
 * On Tru64 and IRIX 6.x it works a little bit differently.  There, the
 * shared library version is stored in the file, and is actually a series
 * of versions, separated by colons.  The rightmost version present in the
 * library when linking an application is stored in the application to be
 * matched at run time.  When the application is run, a check is done to
 * see if the library version stored in the application matches any of the
 * versions in the version string of the library itself.
 * This version string can be constructed in any way, depending on what
 * kind of matching is desired.  However, to implement the same scheme as
 * the one used in the other unixen, all compatible versions, from lowest
 * to highest, should be part of the string.  Consecutive builds would
 * give the following versions strings:
 *
 *	3.0
 *	3.0:3.1
 *	3.0:3.1:3.2
 *	4.0
 *	4.0:4.1
 *
 * Notice how version 4 is completely incompatible with version, and
 * therefore give the breach you can see.
 *
 * There may be other schemes as well that I haven't yet discovered.
 *
 * So, here's the way it works here: first of all, the library version
 * number doesn't need at all to match the overall OpenSSL version.
 * However, it's nice and more understandable if it actually does.
 * The current library version is stored in the macro SHLIB_VERSION_NUMBER,
 * which is just a piece of text in the format "M.m.e" (Major, minor, edit).
 * For the sake of Tru64, IRIX, and any other OS that behaves in similar ways,
 * we need to keep a history of version numbers, which is done in the
 * macro SHLIB_VERSION_HISTORY.  The numbers are separated by colons and
 * should only keep the versions that are binary compatible with the current.
 */
#define SHLIB_VERSION_HISTORY ""
#define SHLIB_VERSION_NUMBER "1.0.0"


#endif /* HEADER_OPENSSLV_H */
