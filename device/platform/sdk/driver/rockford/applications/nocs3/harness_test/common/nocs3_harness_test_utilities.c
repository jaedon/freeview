#include <bstd.h>
#include <string.h>
#include <stdio.h>
#include "nocs3_harness_test_utilities.h"

unsigned int nocs3_harness_test_utility_ascii_2int (const char * pIn)
{
	unsigned int rv = 0;
	while (*pIn)
	{
		unsigned int v = *pIn - '0';
		if (v > 9)
		v -= 7;
		v &= 15; /* in case it's lower case */
		rv = rv*16 + v;
		pIn++;
	}
	return rv;
}


unsigned int  nocs3_harness_test_utility_ascii2array (const char * pIn, 
											 unsigned char * pOut, 
											 unsigned int size)
{
	unsigned int string_length;
	unsigned int leftover;
	unsigned int processed_length;
	unsigned int i;
	unsigned int sizeOut=0;

	if ( (pIn==NULL) || (pOut==NULL) || (size==0) )
		return 0;

	string_length = strlen(pIn);

	if ((2*size) > string_length ) /* (2*size) since each array element holes two ASCII char */
	{
		processed_length = string_length;
		leftover = size - (string_length>>1);
	}
	else
	{
		/* We only take as many as the input array size */
		processed_length = 2*size;
		leftover = 0;
	}

	for (i=0;i<processed_length;i+=2 )
	{
		/* Each byte is made up of two ASCII chars */
		unsigned int rv, v;

		/* 1st ASCII char */
			
		rv = *pIn++ - '0';
		if (rv > 9)
		rv -= 7;
		rv &= 15; /* in case it's lower case */

		/* 2nd ASCII char */
		v = *pIn++ - '0';
		if (v > 9)
		v -= 7;
		v &= 15; /* in case it's lower case */
		
		/* Store the byte */
		* pOut++ = rv*16 + v;
		sizeOut++;

	}

	/* Half byte */
	if ( i>processed_length )
	{
		unsigned int rv = *pIn++ - '0';
		if (rv > 9)
		rv -= 7;
		rv &= 15; /* in case it's lower case */

		* pOut++ = rv*16;
		sizeOut++;
	}

	/* Leftover are zero filled */
	for ( i=0; i<leftover; i++ )
	{
		* pOut++ = 0;
	}

	return sizeOut;
}

void nocs3_harness_test_utility_dump_array (unsigned char * pIn, unsigned int size)
{
	unsigned int i;

	for (i=0; i<size; i++ )
	{
		if (i%16==0 )
			printf("\n");

		printf("%02x ", pIn[i]);
	}
	
	printf("\n");
}

void nocs3_harness_test_utility_increase_16bytes_counter(unsigned char * iv, unsigned int amount)
{
	unsigned int counterLow_high,counterLow_low,counterHigh_high,counterHigh_low;

	if ( (iv==NULL)||(amount==0))
		return;

	counterLow_high = (((unsigned int) iv[8])<<24) |
					(((unsigned int) iv[9])<<16) |
					(((unsigned int) iv[10])<<8) |
					(((unsigned int) iv[11]));
	counterLow_low = (((unsigned int) iv[12])<<24) |
					(((unsigned int) iv[13])<<16) |
					(((unsigned int) iv[14])<<8) |
					((unsigned int) iv[15]) ;
	counterHigh_high = (((unsigned int) iv[0])<<24) |
					(((unsigned int) iv[1])<<16) |
					(((unsigned int) iv[2])<<8) |
					(((unsigned int) iv[3])) ;
	counterHigh_low = (((unsigned int) iv[4])<<24) |
					(((unsigned int) iv[5])<<16) |
					(((unsigned int) iv[6])<<8) |
					((unsigned int) iv[7]) ;

	/* Update counter */
	if ( (0xFFFFFFFF - counterLow_low) < amount )
	{
		/* counterLow_low overflow */
		counterLow_high ++;
		if ( counterLow_high == 0 )
		{
			/* counterLow_high overflow */
			counterHigh_low ++;
			if ( counterHigh_low == 0 )
			{
				/* counterHigh_low overflow */
				counterHigh_high++;
			}
		}
	}				
				
	counterLow_low += amount;

	iv[0] = (unsigned char)(counterHigh_high>>24);
	iv[1] = (unsigned char)(counterHigh_high>>16);
	iv[2] = (unsigned char)(counterHigh_high>>8);
	iv[3] = (unsigned char)(counterHigh_high);
	iv[4] = (unsigned char)(counterHigh_low>>24);
	iv[5] = (unsigned char)(counterHigh_low>>16);
	iv[6] = (unsigned char)(counterHigh_low>>8);
	iv[7] = (unsigned char)(counterHigh_low);
	iv[8] = (unsigned char)(counterLow_high>>24);
	iv[9] = (unsigned char)(counterLow_high>>16);
	iv[10] = (unsigned char)(counterLow_high>>8);
	iv[11] = (unsigned char)(counterLow_high);
	iv[12] = (unsigned char)(counterLow_low>>24);
	iv[13] = (unsigned char)(counterLow_low>>16);
	iv[14] = (unsigned char)(counterLow_low>>8);
	iv[15] = (unsigned char)(counterLow_low);

}

