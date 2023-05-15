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
 * $brcm_Workfile: mxpath.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/7/12 10:06a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mxpath.cpp $
 * 
 * 3   9/7/12 10:06a erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include "mxpath.h"
#include "mstring.h"
#include <string.h>

MXmlElement *MXPath::find(MXmlElement *element, const char *query) {
	if (*query == '[') {
		// formats supports:
		// [attr=value]
		// [#]
		// [last()]
		query++;
		char *end = strchr(query, ']');
		if (!end) return NULL;
		char *value = strchr(query, '=');
		if (value) {
			bool attr = *query == '@';
			if (attr)
				query++;
			MString cmp(query, value-query);
			value++;
			MString val(value, end-value);
			// find child with data==value or @attr=value
			for (MXmlElement *child = element->firstChild(); child; child = element->nextChild()) {
				if (attr && child->attrValue(cmp) == val
					||
					!attr && child->tag() == cmp)
				{
					if (attr || child->childData() == val) {
						return child;
					}
				}
			}
		}
		else {
			MXmlElement *e;
			MString s(query, end-query);
			//printf("found [%s]\n", (const char *)s);
			if (s == "last()")
				e = element->lastChild();
			else
				e = element->childAt(s.toInt()-1);
			return e;
		}
		return NULL;
	}

	char *delim = strpbrk(query, "/[");
	
	// nextSearch is the next tag to look for
	MString nextSearch(query, delim?delim-query:-1);
	// determine if that tag has a namespace
	bool searchForNamespace = strchr(nextSearch, ':');

	//printf("nextSearch=%s\n", (const char *)nextSearch);
	if (nextSearch == "..") {
		element = element->parent();
		if (delim) {
			if (*delim == '/')
				delim++;
			return find(element, delim);
		}
		else
			return element;
	}
	else {
		for (MXmlElement *c = element->firstChild(); c; c = element->nextChild()) {
			const char *tagmatch = NULL;
			// if our search patten had a namespace, then the whole thing has to match
			if (searchForNamespace)
				tagmatch == c->tag();
			else {
				// if our search pattern didn't have a namespace, then we
				// skip any namespace in the element
				tagmatch = strchr(c->tag(), ':');
				if (tagmatch)
					tagmatch++;
				else
					tagmatch = c->tag();
			}

			if (nextSearch == tagmatch) {
				//printf("%s == %s\n", (const char *)c->tag(), (const char *)nextSearch);
				if (delim) {
					if (*delim == '/')
						delim++;
					c = find(c, delim);
				}
				if (c)
					return c;
			}
		}
		return NULL;
	}
}

const MString &MXPath::getData(MXmlElement *element, const char *path) {
	static MString stub;
	element = find(element, path);
	return element?element->childData():stub;
}
