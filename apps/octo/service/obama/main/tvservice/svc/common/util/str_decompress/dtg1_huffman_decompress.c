/* $Header: $ */
/*****************************************************************************
* $Workfile: fsat_decompress.c  $
* Author : ultracat
*
* freesat compressed descriptor's uncompressing routines.
* this code from freesat joint venture.
* this code's original file's description is below.
---------------------------------------------------------------
 FreesatReferenceStringDecoder.cpp v1.0

 This file contains the implementation of the FreesatReferenceStringDecoder
 C++ class. For descriptions of the functions and their parameters see the
 associated header file, FreesatReferenceStringDecoder.h

 Copyright British Broadcasting Corporation
---------------------------------------------------------------
* Copyright (c) 2007 by Humax Co., Ltd.
* All right reserved
*****************************************************************************/


/***************************************************************************/
#define ____________Headers____________
/***************************************************************************/

/*--- os, tool-chain and chipset header files ---*/
#include <stdio.h>
#include <string.h>

/*--- humax header files ---*/
#include <htype.h>
#include <vkernel.h>
#include "dtg1_huffman_decode_tbl_1.h"
#include "dtg1_huffman_decode_tbl_2.h"

#define ESCAPE_CHARACTER 27



/***************************************************************************/
#define ________Debugging_Macro________
/***************************************************************************/

/***************************************************************************/
#define ____________Externs____________
/***************************************************************************/

/***************************************************************************/
#define ________Local_Variables________
/***************************************************************************/
static const unsigned char *arFSat_DecompLookUpTbl[2] = {DTG1_HUFFMAN_DecodeTbl_1, DTG1_HUFFMAN_DecodeTbl_2};


/***************************************************************************/
#define _______STATIC_FUNCTIONS________
/***************************************************************************/
/****************************************************************************
  Function    : INT_FSAT_GetBit
  Parameters  :
                str : source string.
				pos : bit index.
  Output      :
  Return      : TRUE or FALSE
  Description :
****************************************************************************/
static HBOOL INT_FSAT_GetBit(const char *str, unsigned int pos)
{
	// This code could be improved, but works
	// Get the byte containing the relevant bit
	unsigned int byte_pos = pos/8;
	char byte = str[byte_pos];

	// Work out the bit we want
	unsigned int bit_shift = 7 - (pos - (byte_pos*8));

	// Do an AND to return true if it is set, else false
	if  (byte & (1 << bit_shift))
		return TRUE;
	else
		return FALSE;
}

/****************************************************************************
  Function    : INT_FSAT_GetNext8Bits
  Parameters  :
                str : source string.
                pos : bit index.
  Output      :
  Return      : wanted index's 1 byte value.
  Description :
****************************************************************************/
static unsigned char INT_FSAT_GetNext8Bits(unsigned char *str,unsigned int pos)
{
	int i;
	// This code could be improved, but works

	// Use INT_FSAT_GetBit multiple times and build up the byte bit by bit
	unsigned char result = INT_FSAT_GetBit((const char *)str,pos);
	for (i=1; i<8; ++i)
	{
		result = result << 1;
		if (INT_FSAT_GetBit((const char *)str,pos+i))
			result+=1;
	}
	return result;
}

/***************************************************************************/
#define _______PUBLIC_FUNCTIONS________
/***************************************************************************/

/****************************************************************************
Function    : FSAT_DecompData
Parameters  :
            nCompressMethod : FreeSat's compressed method (0 or 1)
            szCompressedStr : FreeSat's original compressed string.
            szDestStrBuf : decompressed data's buffer pointer.
Output      :
Return      :	 TRUE or FALSE
Description :
****************************************************************************/
HBOOL DTG1_Huffman_DecompData(int nCompressMethod, unsigned char *szCompressedStr, unsigned long ulCompStrLen, void **szDestStrBuf,  HUINT32 *pulDecompStrLen)
{
	unsigned long encoded_str_len=0,next_bit_pos=0,last_bit_pos;
	unsigned char last_decoded_byte = 0;
	HBOOL last_char_was_compressed_escape = FALSE;
	HBOOL exit_loop = FALSE;
	unsigned char *szTmp = szCompressedStr;
	unsigned int	ulDecompPos=0;
	// if this function used in descriptor's stinrg, only,
	// 255 byte is enough for decoded buffer.
	// but, i using 1kbytes.
	char *szDecompressedString = (char *)OxSI_Malloc(1024);

	if ((NULL == szDecompressedString) || (NULL == pulDecompStrLen))
	{
		if (NULL != szDecompressedString)
			OxSI_Free(szDecompressedString);
		return FALSE;
	}

	*szDestStrBuf = NULL; *pulDecompStrLen = 0;

	memset((void *)szDecompressedString,0,1024);
	encoded_str_len = ulCompStrLen;
	if (encoded_str_len <= 0)
	{
		OxSI_Free(szDecompressedString);
		return FALSE;
	}

	last_bit_pos = encoded_str_len * 8 - 1;

	while (FALSE == exit_loop)
	{
		unsigned char current_decoded_byte = 0;

		if (last_char_was_compressed_escape || (last_decoded_byte > 127))
		{
			if (next_bit_pos+7 > last_bit_pos)
			{
				OxSI_Free(szDecompressedString);
				return FALSE;
			}
			current_decoded_byte = INT_FSAT_GetNext8Bits(szTmp,next_bit_pos);
			next_bit_pos += 8;
			last_char_was_compressed_escape = FALSE;
		}
		else
		{
			unsigned int tree_root_byte_offset =
				(arFSat_DecompLookUpTbl[nCompressMethod][2*last_decoded_byte] << 8) +
				arFSat_DecompLookUpTbl[nCompressMethod][2*last_decoded_byte+1];
			unsigned int current_table_word_offset = 0;
			HBOOL found = FALSE;

			while (FALSE == found)
			{
				unsigned char offset_value;
				unsigned int current_byte_offset =
					tree_root_byte_offset +
					(2*current_table_word_offset);
				if (next_bit_pos > last_bit_pos)
				{
					OxSI_Free(szDecompressedString);
					return FALSE;
				}
				if (INT_FSAT_GetBit((const char *)szTmp,next_bit_pos))
					current_byte_offset += 1;
				++next_bit_pos;
				offset_value = arFSat_DecompLookUpTbl[nCompressMethod][current_byte_offset];
				if (offset_value >127)
				{
					current_decoded_byte = offset_value & 0x7F;
					found = TRUE;
				}
				else
				{
					current_table_word_offset = offset_value;
				}
			}
			if (ESCAPE_CHARACTER == current_decoded_byte)
				last_char_was_compressed_escape = TRUE;
			else
				last_char_was_compressed_escape = FALSE;
		}
		if (0 == current_decoded_byte)
			exit_loop = TRUE;
		else if (FALSE == last_char_was_compressed_escape)
		{
			szDecompressedString[ulDecompPos] = current_decoded_byte;
			ulDecompPos++;
		}

		last_decoded_byte = current_decoded_byte;
	}

	*szDestStrBuf = szDecompressedString;
	*pulDecompStrLen = (ulDecompPos + 1);

	return TRUE;
}
/* end of file */
