/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: b_namevalue_pairs.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/19/07 2:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/b_namevalue_pairs.c $
 * 
 * 1   6/19/07 2:59p erickson
 * PR32352: make reusable name/value pair mappings for Settop API enums
 * 
 ********************************************************/
#include "b_namevalue_pairs.h"
#include <string.h>

int b_get_name(const b_namevalue_pair *pairs, unsigned value, const char **name)
{
	int i;
	for (i=0; pairs[i].name; i++) {
		if (pairs[i].value == value) {
			*name = pairs[i].name;
			return 0;
		}
	}
	return -1;
}

int b_get_value_i(const b_namevalue_pair *pairs, const char *name, unsigned *value)
{
	int i;
	for (i=0; pairs[i].name; i++) {
		if (!strcasecmp(pairs[i].name, name)) {
			*value = pairs[i].value;
			return 0;
		}
	}
	return -1;
}

int b_get_value_n_i(const b_namevalue_pair *pairs, const char *name, unsigned n, unsigned *value)
{
	int i;
	for (i=0; pairs[i].name; i++) {
		if (!strncasecmp(pairs[i].name, name, n)) {
			*value = pairs[i].value;
			return 0;
		}
	}
	return -1;
}

int b_get_value(const b_namevalue_pair *pairs, const char *name, unsigned *value)
{
	int i;
	for (i=0; pairs[i].name; i++) {
		if (!strcmp(pairs[i].name, name)) {
			*value = pairs[i].value;
			return 0;
		}
	}
	return -1;
}

int b_get_value_n(const b_namevalue_pair *pairs, const char *name, unsigned n, unsigned *value)
{
	int i;
	for (i=0; pairs[i].name; i++) {
		if (!strncmp(pairs[i].name, name, n)) {
			*value = pairs[i].value;
			return 0;
		}
	}
	return -1;
}

