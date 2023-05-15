/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: mhttpclient.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mhttpclient.h $
 * 
 * 2   9/7/12 10:06a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#ifndef MHTTPCLIENT_H
#define MHTTPCLIENT_H

#include "mhttpengine.h"

/**
* MHttpClient HOWTO
*
* GET
* 1. Call get(url)
* 2. Call getHeader() and read(buf,len) until read returns 0.
*
* POST
* 1. Call post(url)
* 2. Call setHeader() as many times as needed
* 3. Start calling write() after you have finished with the headers.
* 4. After you're done writing, call flush(), then readResponseHeader(),
*    then call responseCode() and getHeader()
* 5. Then you can call read() until it returns 0.
* 6. Call close()
*
* See webclient.cpp for a simple example.
*/
class MHttpClient : public MHttpEngine {
public:
	/**
	* If you aren't connected, get() and post() will connect for you
	* without Connection: Keep-Alive. If you've already called connect(),
	* then the Keep-Alive header is added.
	*/
	int connect(const char *host, int port);

	/**
	* get sends the request and reads the response headers.
	**/
	int get(const char *url);
	
	/**
	* post and put send the request only.
	**/
	int post(const char *url) {return sendRequest("POST", url);}
	int put(const char *url) {return sendRequest("PUT", url);}
	
	/**
	* send a request
	**/
	int sendRequest(const char *method, const char *url);

	int readResponseHeader();
	int responseCode() const {return _responseCode;}

protected:
	int _responseCode;
};

#endif //MHTTPCLIENT_H
