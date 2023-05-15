/***************************************************************************
 *     Copyright (c) 2003-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: b_namevalue_pairs.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/19/07 3:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/b_namevalue_pairs.h $
 * 
 * 2   6/19/07 3:20p erickson
 * PR32352: add __cplusplus
 * 
 * 1   6/19/07 2:59p erickson
 * PR32352: make reusable name/value pair mappings for Settop API enums
 * 
 ********************************************************/
#ifndef B_NAMEVALUE_PAIRS_H__
#define B_NAMEVALUE_PAIRS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* A simple name/value pair lookup interface */

/* Name/value pair. Pair arrays should be terminated with name == NULL. */
typedef struct b_namevalue_pair {
	const char *name;
	unsigned value;
} b_namevalue_pair;

/**
Summary:
Look up a name with a value

Description:
Returns 0 if found, -1 if not.
**/
int b_get_name(const b_namevalue_pair *pairs, unsigned value, const char **name);

/**
Summary:
Look up a value with a case insensitive name

Description:
Returns 0 if found, -1 if not.
**/
int b_get_value_i(const b_namevalue_pair *pairs, const char *name, unsigned *value);

/**
Summary:
Look up a value with a case sensitive name

Description:
Returns 0 if found, -1 if not.
**/
int b_get_value(const b_namevalue_pair *pairs, const char *name, unsigned *value);

/**
Summary:
Look up a value with a case insensitive name and a limited number of characters

Description:
Returns 0 if found, -1 if not.
**/
int b_get_value_n_i(const b_namevalue_pair *pairs, const char *name, unsigned n, unsigned *value);

/**
Summary:
Look up a value with a case sensitive name and a limited number of characters

Description:
Returns 0 if found, -1 if not.
**/
int b_get_value_n(const b_namevalue_pair *pairs, const char *name, unsigned n, unsigned *value);

#ifdef __cplusplus
}
#endif

#endif
