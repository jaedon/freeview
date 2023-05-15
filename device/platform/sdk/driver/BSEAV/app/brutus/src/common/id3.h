/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: id3.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 8:03p $
 *
 * Module Description: ID3 v1.0 and v1.1 reader
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/id3.h $
 * 
 * 1   2/7/05 8:03p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   5/12/03 4:42p erickson
 * cheops devel
 * 
 * Irvine_BSEAVSW_Devel/2   5/2/03 4:00p erickson
 * initial cheops work
 * 
 * Irvine_BSEAVSW_Devel/1   4/30/03 5:02p erickson
 * new code common to narmer and cheops
 *
 ****************************************************************************/
#ifndef ID3_READER_H
#define ID3_READER_H

typedef struct {
	char title[31];
	char artist[31];
	char album[31];
	int year;
	char comment[31];
	int albumtrack; /* v1.1 only */
	int genre;
} id3_info;

#ifdef __cplusplus
extern "C" {
#endif

/**
* Read id3 v1.0 and v1.1 information from an open file.
* Returns 0 on success.
**/
int id3_read(int fd, id3_info *info);

/**
* Write id3 v1.1 information to an open file.
* If the tag is not present, it will append to the file.
* Returns 0 on success.
**/
int id3_write(int fd, id3_info *info);

/**
* Converts genre integer to genre string.
* Returns NULL if the genre isn't defined.
**/
const char *id3_genreStr(int genre);

#ifdef __cplusplus
}
#endif

#endif /* ID3_READER_H */
