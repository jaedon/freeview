/******************************************************************************
* $Workfile: decompress.c	$
* $Modtime:$
* Author : 
* description :
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

#include "decompress.h"

/******************************************************************************
* definitions, typdef & const data
******************************************************************************/
#define N_BIT_BYTE 	8
#define SIZE_BitBuf 	(N_BIT_BYTE * sizeof(_bitBuf))
#define N_BIT_Dic		13
#define SIZE_Dic		(1U << N_BIT_Dic)
#define MAX_Match		256
#define THRESHOLD		3
#define NC				(0xff + MAX_Match+2-THRESHOLD)
#define N_BIT_C 		9
#define N_BIT_Code		16

#define NP				(N_BIT_Dic+1)
#define NT				(N_BIT_Code+3)
#define PBIT			4  // smallest integer such that (1U<<PBIT) > NP
#define TBIT			5  // smallest integer such that (1U<<TBIT) > NT
#if NT>NP
	#define NPT 		NT
#else
	#define NPT 		NP
#endif

/******************************************************************************
* Global variables and structures
******************************************************************************/

/******************************************************************************
* External variables and functions
******************************************************************************/

/******************************************************************************
* Static variables and structures
******************************************************************************/
static unsigned char	_cLen[NC], _ptLen[NPT], *_orgData, _dicBuf[SIZE_Dic];
static unsigned short	_left[2*NC-1], _right[2*NC-1], _blkSize, _cTbl[4096],
		_ptTbl[256], _bitBuf, _subBitBuf,
		_start[18], _count[17], _weight[17];
static short _bitCount;
static unsigned short _compSize;

/******************************************************************************
* Static function prototypes
******************************************************************************/

static void MakeTbl(unsigned short nChar, unsigned char *bitLen, unsigned short tblBits, unsigned short *tbl)
{
	unsigned short	*wp, *wpp, *wppp, i, k, avail, mask;
	unsigned short	len, ch, jutBits, nextCode;
	unsigned char	*bp;

	for (wp=&_count[1],i=1; i<=16; i++)
		*wp++ = 0;
	for (wp=_count, bp=bitLen, i=0; i<nChar; i++)
		wp[*bp++]++;

	for (wp=&_start[1],wpp=&_count[1],*wp=0,i=1,k=15; i<=16; i++,k--,wp++)
		wp[1] = *wp + (*wpp++ << k);

	jutBits = 16-tblBits;

	for (wp=&_start[1],wpp=&_weight[1],i=1,k=tblBits-1; i<=tblBits; i++,k--)
	{
		*wp >>= jutBits;
		wp++;
		*wpp++ = 1U << k;
	}

	for (k=16-i,wp=&_weight[i]; i<=16; i++,k--)
		*wp++ = 1U << k;

	i = _start[tblBits+1] >> jutBits;
	if (i != (unsigned short)(1U << 16))
	{
		k = 1U << tblBits;
		while (i!=k)
			tbl[i++] = 0;
	}

	avail = nChar;
	mask = 1U << (15-tblBits);
	wpp = _right;
	wppp = _left;
	for (ch=0,bp=bitLen; ch<nChar; ch++)
	{
		if ((len = *bp++) == 0)
			continue;

		k = _start[len];
		nextCode = k + _weight[len];

		if (len <= tblBits)
		{
			for (wp=tbl+k; k<nextCode; k++)
				*wp++ = ch;
		}
		else
		{
			wp = tbl + (k >> jutBits);
			i = len-tblBits;
			while (i != 0)
			{
				if (*wp == 0)
				{
					wpp[avail] = wppp[avail] = 0;
					*wp = avail++;
				}
				wp = (k&mask ? wpp : wppp) + *wp;
				k <<= 1;
				i--;
			}
			*wp = ch;
		}
		_start[len] = nextCode;
	}
} /* MakeTbl */


static void FillBuf(short n)	/* Shift _bitBuf n bits left, read n bits */
{
	_bitBuf <<= n;
	while (n > _bitCount)
	{
		n -= _bitCount;
		_bitBuf |= _subBitBuf << n;
		if (_compSize != 0)
		{
			_compSize--;
			_subBitBuf = *_orgData++;
		}
		else
			_subBitBuf = 0;
		_bitCount = N_BIT_BYTE;
	}

	_bitCount -= n;
	_bitBuf |= _subBitBuf >> _bitCount;
} /* FillBuf */


static unsigned short GetBits(unsigned short n)
{
	unsigned short	x;

	x = _bitBuf >> (SIZE_BitBuf-n);
	FillBuf(n);

	return x;
} /* GetBits */


static void ReadPtLen(short nn, short nBit, short iSpecial)
{
	short	i,c,n;
	unsigned short	mask, *wp;
	unsigned char	*bp;

	n = GetBits(nBit);
	if (n == 0)
	{
		c = GetBits(nBit);
		for (bp=_ptLen,i=0; i<nn; i++)
			*bp++ = 0;
		for (wp=_ptTbl,i=0; i<256; i++)
			*wp++ = c;
	}
	else
	{
		i = 0;
		bp = _ptLen;
		while (i < n)
		{
			c = _bitBuf >> (SIZE_BitBuf-3);
			if (c == 7)
			{
				mask = 1U << (SIZE_BitBuf-1-3);
				while (mask & _bitBuf)
				{
					mask >>= 1;
					c++;
				}
			}
			FillBuf((unsigned short)(c<7 ? 3 : c-3));
			bp[i++] = (unsigned char)c;
			if (i == iSpecial)
			{
				c = GetBits(2);
				while (--c >= 0)
					bp[i++] = 0;
			}
		}
		while (i<nn)
			bp[i++] = 0;
		MakeTbl(nn, bp, 8, _ptTbl);
	}
} /* ReadPtLen */


static void ReadCLen(void)
{
	short	i, c, n;
	unsigned short	mask, *wp, *wpp;
	unsigned char	*bp, *bpp;

	n = GetBits(N_BIT_C);
	bp = _cLen;
	if (n == 0)
	{
		c = GetBits(N_BIT_C);
		for (i=0; i<NC; i++)
			*bp++ = 0;
		for (wp=_cTbl,i=0; i<4096; i++)
			*wp++ = c;
	}
	else
	{
		i = 0;
		bpp = _ptLen;
		wp = _right;
		wpp = _left;
		while (i < n)
		{
			c = _ptTbl[_bitBuf>>(SIZE_BitBuf-8)];
			if (c >= NT)
			{
				mask = 1U << (SIZE_BitBuf-1-8);
				do
				{
					c = (_bitBuf & mask) ? wp[c] : wpp[c];
					mask >>= 1;
				} while (c >= NT);
			}
			FillBuf(bpp[c]);
			if (c <= 2)
			{
				if (c == 0) c = 1;
				else if (c == 1) c = GetBits(4) + 3;
				else		c = GetBits(N_BIT_C) + 20;

				while (--c >= 0)
					bp[i++]=0;
			}
			else
				bp[i++] = c-2;
		}
		while (i < NC)
			bp[i++] = 0;
		MakeTbl(NC, bp, 12, _cTbl);
	}
} /* ReadCLen */


unsigned short DecodeC(void)
{
	unsigned short	i, mask, *wp, *wpp;

	if (_blkSize == 0)
	{
		_blkSize = GetBits(16);
		ReadPtLen(NT, TBIT, 3);
		ReadCLen();
		ReadPtLen(NP, PBIT, -1);
	}
	_blkSize--;
	i = _cTbl[_bitBuf >> (SIZE_BitBuf-12)];
	if (i >= NC)
	{
		mask = 1U << (SIZE_BitBuf-1-12);
		wp = _right;
		wpp = _left;
		do
		{
			i = (_bitBuf & mask) ? wp[i] : wpp[i];
			mask >>= 1;
		} while (i >= NC);
	}
	FillBuf(_cLen[i]);

	return i;
} /* DecodeC */


static unsigned short DecodeP(void)
{
	unsigned short	i, mask, *wp, *wpp;

	i = _ptTbl[_bitBuf >> (SIZE_BitBuf-8)];
	if (i >= NP)
	{
		mask = 1U << (SIZE_BitBuf-1-8);
		wp = _right;
		wpp = _left;
		do
		{
			i = (_bitBuf & mask) ? wp[i] : wpp[i];
			mask >>= 1;
		} while (i >= NP);
	}
	FillBuf(_ptLen[i]);
	if (i != 0)
		i = (1U << (i-1)) + GetBits((unsigned short)(i-1));

	return i;
} /* DecodeP */


void Decompress(unsigned char *pucCompBuf, unsigned char *pucDecompBuf, unsigned short usCompSize, unsigned short usOrgSize)
{
	short	rem, n, i, r, c;	// rem: remaining unsigned chars to copy
	unsigned char	*dicBuf;

	_orgData  = pucCompBuf;
	_compSize = usCompSize;
	_bitBuf = _subBitBuf = 0;
	_bitCount = 0;
	FillBuf(SIZE_BitBuf);
	_blkSize = 0;
	rem = 0;
	i = 0;

	while (usOrgSize != 0)
	{
		dicBuf = _dicBuf;

		n = (short)((usOrgSize > SIZE_Dic) ? SIZE_Dic : usOrgSize);

		r = 0;
		while (--rem >= 0)
		{
			dicBuf[r] = dicBuf[i];
			i = (short)((i+1) & (SIZE_Dic-1));
			if (++r == n)
				goto NEXT;
		}

		while(1)
		{
			c = DecodeC();
			if (c <= 0xff)
			{
				dicBuf[r] = (unsigned char)c;
				if (++r == n)
					goto NEXT;
			}
			else
			{
				rem = c - (0xff+1-THRESHOLD);
				i = (short)((r-DecodeP()-1) & (SIZE_Dic-1));
				while (--rem >= 0)
				{
					dicBuf[r] = dicBuf[i];
					i = (short)((i+1) & (SIZE_Dic-1));
					if (++r == n)
						goto NEXT;
				}
			}
		}

		NEXT:

		for (r=0; r<n; r++)
			*pucDecompBuf++ = *dicBuf++;

		usOrgSize -= n;
	}
}

/*
* $Log:$
*/

/* end of file */

