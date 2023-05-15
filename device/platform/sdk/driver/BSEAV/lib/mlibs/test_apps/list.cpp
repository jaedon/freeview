/***************************************************************************
 *     Copyright (c) 2004, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: list.cpp $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 11:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/test_apps/list.cpp $
 * 
 * 1   2/7/05 11:22p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/1   2/26/04 10:59a erickson
 * PR9900: added more unit test
 *
 ****************************************************************************/
#include "mvoidlist.h"
#include <stdio.h>
#include <assert.h>

int main()
{
	MVoidList list;
	int i;

#define TOTAL 10

	printf("test add\n");
	for (i=0;i<TOTAL;i++)
		list.add((void*)i);
	assert(list.total() == TOTAL);

	printf("test internal iteration (fwd)\n");
	for (i=0,list.first();list.current();i++,list.next())
		assert((int)list.current() == i);

	printf("test external iteration (rev)\n");
	MVoidListItr itr(&list);
	for (i=TOTAL-1,itr.last();itr.current();i--,itr.prev())
		assert((int)itr.current() == i);

	printf("test removing first\n");
	list.first();
	list.remove();
	assert((int)list.at(0) == 1);
	assert(list.total() == TOTAL - 1);

	printf("test removing last\n");
	list.last();
	list.remove();
	assert(list.total() == TOTAL - 2);
	assert((int)list.at(list.total()-1) == TOTAL - 2);

	printf("test clear\n");
	list.clear();
	assert(list.total() == 0);
	assert(list.first() == NULL);
	assert(list.last() == NULL);

	return 0;
}
