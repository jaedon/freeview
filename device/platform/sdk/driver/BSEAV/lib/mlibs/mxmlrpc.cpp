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
 * $brcm_Workfile: mxmlrpc.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/7/12 10:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mxmlrpc.cpp $
 * 
 * 2   9/7/12 10:06a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "mxmlrpc.h"
#include "mxmlparser.h"
#include "mhttpclient.h"

MXmlElement *MXmlRpc::newCall(const char *method) {
	MXmlElement *e = new MXmlElement(NULL, "methodCall");
	e->createChild("methodName")->createData(method);
	return e;
}

void MXmlRpc::addParam(MXmlElement *call, const char *paramvalue, const char *type) {
	// create the param value
	MXmlElement *param = NULL;
	if (type) {
		param = new MXmlElement(NULL, type);
		param->createChild(NULL)->setData(paramvalue);
	}
	else {
		param = new MXmlElement(NULL);
		param->setData(paramvalue);
	}
	// add it to the call
	addParam(call, param);
}

void MXmlRpc::addParam(MXmlElement *call, MXmlElement *param) {
	MXmlElement *params;
	if (call->totalChildren() == 1)
		params = call->createChild("params");
	else {
		call->firstChild();
		params = call->nextChild();
		// should be params
	}
	MXmlElement *value = params->createChild("param")->createChild("value");
	value->add(param);
}

MXmlElement *MXmlRpc::makeCall(const char *url, MXmlElement *call, bool printHttp)
{
	MHttpClient http;
	#define XMLRPC_BUFSIZE 1024*10
	char buf[XMLRPC_BUFSIZE];

	http.setPrintHttp(printHttp);

	int len = sprintf(buf, "<?xml version=\"1.0\"?>");
	len += call->print(&buf[len], XMLRPC_BUFSIZE-len);
	if (http.post(url))
		return NULL;
	http.setHeader("Content-type", "text/xml");
	http.setHeader("Content-length", len);
	http.write(buf, len);

	if (http.readResponseHeader())
		return NULL;
	len = http.read(buf, XMLRPC_BUFSIZE);
	if (len == -1 || http.responseCode() != 200)
		return NULL;
	MString data(buf, len);
	MXmlParser parser;
	return parser.parse(data);
}

MXmlElement *MXmlRpc::createArray() {
	MXmlElement *array = new MXmlElement(NULL, "array");
	array->createChild("data");
	return array;
}

void MXmlRpc::addArrayValue(MXmlElement *array, const char *value, const char *type) {
	array = array->firstChild();
	MXmlElement *data = array->createChild("value");
	if (type)
		data = data->createChild(type);
	data->createData(value);
}
