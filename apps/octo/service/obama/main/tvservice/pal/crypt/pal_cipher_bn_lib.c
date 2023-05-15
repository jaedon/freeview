/***************************************************************
* $Workfile:   Pal_cipher_bn_lib.c  $
* $Modtime:
* Auther :
*
* Desc : Small BN library
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/
#include "octo_common.h"
#include "pal_cipher.h"
#include "pal_cipherLocal.h"

/***************************************************************
* local definition /////////////////////////////////////////////
***************************************************************/
#ifdef CONFIG_DEBUG
extern HUINT32 	s_ulPalCipher_Level;
#endif




/***************************************************************
* Pre-defined big numbers //////////////////////////////////////
***************************************************************/

PalCipher_BigNumber_e crypt_bn_zero	= {1, {0x00}};
PalCipher_BigNumber_e crypt_bn_one	= {1, {0x01}};
PalCipher_BigNumber_e crypt_bn_x10	= {1, {0x10}};
PalCipher_BigNumber_e crypt_bn_e		= {3, {0x01,0x00,0x01}};

/***************************************************************************/
#define ________Local_Functions________
/***************************************************************************/

#ifndef PAL_CIPHER_DEACTIVATE_NOT_USED_FUNCTIONS

/* compute big number (a x (2 ^ n)), n = 0 ~ 7 only !! */
static void local_cipher_bn_mul_2_n(PalCipher_BigNumber_e *a, int n, PalCipher_BigNumber_e *out)
{
	unsigned char *p, *q, mask, rshft;
	bnLenType i;

	if (pal_cipher_bn_cmp(a, &crypt_bn_zero) == 0) // 불필요한 연산을 막기 위해 (자리수 오류 보정 포함).
	{
		out->len = 1;
		out->num[0] = 0x00;
		return;
	}

	if (n == 0) // 불필요한 연산을 막기 위해 (자리수 오류 보정 포함).
	{
		out->len = a->len;
		HxSTD_memcpy(out->num, a->num, a->len);
		return;
	}

	p = a->num; q = out->num;

	mask = (0xff >> n) ^ 0xff;
	rshft = 8 - n;

	if (p[0] & mask)
	{
		out->len = a->len + 1;
		q[0] = p[0] >> rshft;
		for (i = 1; i < a->len; i++)
		{
			if (p[i] & mask)
				q[i] = (p[i-1] << n) | (p[i] >> rshft);
			else
				q[i] = p[i-1] << n;
		}
		q[i] = p[i-1] << n;
	}
	else
	{
		out->len = a->len;
		for (i = 0; i < a->len - 1; i++)
		{
			if (p[i+1] & mask)
				q[i] = (p[i] << n) | (p[i+1] >> rshft);
			else
				q[i] = p[i] << n;
		}
		q[i] = p[i] << n;
	}
}

/* compute big number (a >> n), n = 0 ~ 7 only !! */
static void local_cipher_bn_right_shift_n(PalCipher_BigNumber_e *a, int n, PalCipher_BigNumber_e *out)
{
	unsigned char *p, *q, mask, lshft;
	bnLenType i;

	if ((pal_cipher_bn_cmp(a, &crypt_bn_zero) == 0) || (pal_cipher_bn_cmp(a, &crypt_bn_one) == 0)) // 불필요한 연산을 막기 위해 (자리수 오류 보정 포함).
	{
		out->len = 1;
		out->num[0] = 0x00;
		return;
	}

	if (n == 0) // 불필요한 연산을 막기 위해 (자리수 오류 보정 포함).
	{
		out->len = a->len;
		HxSTD_memcpy(out->num, a->num, a->len);
		return;
	}

	p = a->num; q = out->num;

	mask = (0xff >> n) ^ 0xff;
	lshft = 8 - n;

	if (p[0] > n)
	{
		out->len = a->len;
		q[0] = p[0] >> n;
		for (i = 1; i < out->len; i++)
			q[i] = ((p[i-1] << lshft) & mask) | (p[i] >> n);
	}
	else
	{
		out->len = a->len - 1;
		for (i = 0; i < out->len; i++)
			q[i] = ((p[i] << lshft) & mask) | (p[i+1] >> n);
	}
}

/* compute big number (a ^ b) */
/* 주의 : b가 큰값을 가질 때 MAX_BN_LEN 값이 충분히 크지 않다면 이 함수는 buffer overflow 를 야기할 것이다.
          bn_mod(), bn_mod_exp()는 mod 의 성질을 이용해서 계산하기 때문에 실제로 b가 큰값을 가질 때 이 함수가 사용되는 일은 없다. */
static void local_cipher_bn_pow(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out)
{
	PalCipher_BigNumber_e cnt, tmp;

	if (pal_cipher_bn_cmp(b, &crypt_bn_zero) == 0) // any a ^ 0 = 1
	{
		out->len = 1;
		out->num[0] = 0x01;
		return;
	}

	if ((pal_cipher_bn_cmp(a, &crypt_bn_zero) == 0) || (pal_cipher_bn_cmp(a, &crypt_bn_one) == 0)) // 불필요한 연산을 막기 위해.
	{
		out->len = 1;
		out->num[0] = a->num[0];
		return;
	}

	if (pal_cipher_bn_cmp(b, &crypt_bn_one) == 0) // 불필요한 연산을 막기 위해.
	{
		out->len = a->len;
		HxSTD_memcpy(out->num, a->num, a->len);
		return;
	}

	out->len = a->len;
	HxSTD_memcpy(out->num, a->num, a->len);

	cnt.len = b->len;
	HxSTD_memcpy(cnt.num, b->num, b->len);

	while (1)
	{
		pal_cipher_bn_sub(&cnt, &crypt_bn_one, &cnt);
		if (pal_cipher_bn_cmp(&cnt, &crypt_bn_zero) == 0)
			break;
		pal_cipher_bn_mul(out, a, &tmp);
		out->len = tmp.len;
		HxSTD_memcpy(out->num, tmp.num, tmp.len);
	}
}

/* compute big number (0 - a) as two's complement representation */
static void local_cipher_bn_neg(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *out)
{
	bnLenType i;

	for (i = 0; i < a->len; i++)
	{
		out->num[i] = ~a->num[i];
	}

	pal_cipher_bn_add(out, &crypt_bn_one, out);
}

#endif // not used


#if (defined(INCLUDE_BN_TEST) && !defined(USE_MPI_LIB)) || !defined(PAL_CIPHER_USE_MPI_LIB)

/* compute big number (a ^ 2) */
static void local_cipher_bn_square(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *out)
{
	unsigned char *x = a->num;
	bnLenType t = a->len, tx2 = t<<1;
	signedBnLenType i, j, k;
	unsigned int mul;
	unsigned char w[MAX_BN_LEN];

	if (pal_cipher_bn_cmp(a, &crypt_bn_x10) < 0) // 불필요한 연산을 막기 위해 (자리수 오류 보정 포함).
	{
		out->len = 1;
		out->num[0] = x[0] * x[0];
		return;
	}

	if (tx2 > MAX_BN_LEN)
	{
		HxLOG_Error("[CICC]---> bn_square()---> Error : wrong length %d !!!\n", tx2);
		out->len = 1;
		out->num[0] = 0x00;
		return;
	}

	HxSTD_memset(w, 0x00, tx2);

	for (i = t - 1; i >= 0; i--)
	{
		mul = (unsigned int)(w[2*i+1] + x[i] * x[i]);
		w[2*i+1] = (unsigned char)(mul & 0xff);
		mul >>= 8;
		for (j = i - 1; j >= 0; j--)
		{
			mul = (unsigned int)(w[i+j+1] + 2 * x[j] * x[i] + mul);
			w[i+j+1] = (unsigned char)(mul & 0xff);
			mul >>= 8;
		}
		for (k = 0; k <= i && mul; k++)
		{
			mul += (unsigned int)w[i-k];
			w[i-k] = (unsigned char)(mul & 0xff);
			mul >>= 8;
		}
	}

	if (w[0] == 0x00)
	{
		out->len = tx2-1;
		HxSTD_memcpy(out->num, &w[1], out->len);
	}
	else
	{
		out->len = tx2;
		HxSTD_memcpy(out->num, w, out->len);
	}
}

#endif // #ifndef USE_MPI_LIB

/* compute big number (a x (256 ^ n)) */
static void local_cipher_bn_mul_256_n(PalCipher_BigNumber_e *a, bnLenType n, PalCipher_BigNumber_e *out)
{
	if (pal_cipher_bn_cmp(a, &crypt_bn_zero) == 0) // 불필요한 연산을 막기 위해 (자리수 오류 보정 포함).
	{
		out->len = 1;
		out->num[0] = 0x00;
		return;
	}

	if (n == 0) // 불필요한 연산을 막기 위해 (자리수 오류 보정 포함).
	{
		out->len = a->len;
		HxSTD_memcpy(out->num, a->num, a->len);
		return;
	}

	out->len = a->len + n;
	HxSTD_memcpy(out->num, a->num, a->len);
	HxSTD_memset(&out->num[a->len], 0x00, n);
}

/***************************************************************************/
#define _______Global_Functions________
/***************************************************************************/

/* compute big number (a mod b) */
void pal_cipher_bn_mod(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out)
{
//	#define CHECK_BN_MOD

	PalCipher_BigNumber_e mul1, mul2, factor;
	unsigned short m;
	int cmp;

#ifdef CHECK_BN_MOD
	int i = 0, j, k; // for debugging

	HxLOG_Print("\n\n === input : a (%d) ===", a->len);
	for (k = 0; k < a->len; k++)
	{
		if ((k & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02X", a->num[k]);
	}
	HxLOG_Print("\n\n === input : b (%d) ===", b->len);
	for (k = 0; k < b->len; k++)
	{
		if ((k & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02X", b->num[k]);
	}
	HxLOG_Print("\n");
#endif

	if (pal_cipher_bn_cmp(b, &crypt_bn_zero) == 0)
	{
		HxLOG_Error("[CICC]---> bn_mod()---> Error : divided by zero !!!\n");
		return;
	}

	out->len = a->len;
	HxSTD_memcpy(out->num, a->num, a->len);

	while (1) // i 루프
	{
#ifdef CHECK_BN_MOD
		HxLOG_Print("\n[i=%03d]", ++i); j = 0;
#endif

		cmp = pal_cipher_bn_cmp(out, b);
		if (cmp <= 0)
			goto EXIT;

		local_cipher_bn_mul_256_n(b, out->len - b->len, &mul1);
		cmp = pal_cipher_bn_cmp(&mul1, out);
		if (cmp == 0)
			goto EXIT;
		else if (cmp > 0)
			mul1.len -= 1;

		if (out->len > mul1.len)
		{
			m = (unsigned short)((out->num[0]*0x100+out->num[1]) / mul1.num[0]);
			if (m > 0xff)
			{
				factor.len = 2;
				factor.num[0] = m >> 8;
				factor.num[1] = m & 0xff;
			}
			else
			{
				factor.len = 1;
				factor.num[0] = m;
			}
		}
		else
		{
			factor.len = 1;
			factor.num[0] = out->num[0] / mul1.num[0];
		}

		pal_cipher_bn_mul(&mul1, &factor, &mul2);
		while (pal_cipher_bn_cmp(&mul2, out) > 0)
		{
#ifdef CHECK_BN_MOD
			j++;
#endif

			pal_cipher_bn_sub(&factor, &crypt_bn_one, &factor);
			pal_cipher_bn_mul(&mul1, &factor, &mul2);
		}

#ifdef CHECK_BN_MOD
		HxLOG_Print("[j=%d]", j);
		#if 0
		HxLOG_Print("\n --- out (%d) ---", out->len);
		for (k = 0; k < out->len; k++)
		{
			if ((k & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02X", out->num[k]);
		}
		HxLOG_Print("\n --- mul #1 (%d) ---", mul1.len);
		for (k = 0; k < mul1.len; k++)
		{
			if ((k & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02X", mul1.num[k]);
		}
		HxLOG_Print("\n --- factor (%d) ---", factor.len);
		for (k = 0; k < factor.len; k++)
		{
			if ((k & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02X", factor.num[k]);
		}
		HxLOG_Print("\n --- mul #2 (%d) ---", mul2.len);
		for (k = 0; k < mul2.len; k++)
		{
			if ((k & 15) == 0)
				HxLOG_Print("\n");
			HxLOG_Print(" %02X", mul2.num[k]);
		}
		#endif
#endif

		pal_cipher_bn_sub(out, &mul2, out);
	}

EXIT :

	if (cmp == 0)
	{
		out->len = 1;
		out->num[0] = 0x00;
	}

#ifdef CHECK_BN_MOD
	HxLOG_Print("\n\n === result (%d) ===", out->len);
	for (k = 0; k < out->len; k++)
	{
		if ((k & 15) == 0)
			HxLOG_Print("\n");
		HxLOG_Print(" %02X", out->num[k]);
	}
#endif
}

/* make big number a from byte array with len */
void pal_cipher_bn_init(PalCipher_BigNumber_e *a, unsigned char *c, bnLenType len)
{
	if (len < 1 || len > MAX_BN_LEN)
	{
		HxLOG_Error("[CICC]---> bn_init()---> Error : wrong length %d !!!\n", len);
		a->len = 1;
		a->num[0] = 0x00;
		return;
	}

	a->len = len;
	HxSTD_memcpy(a->num, c, len);
}

/* compare big number a and b : return 1 if a > b, 0 if a = b, -1 if a < b */
int pal_cipher_bn_cmp(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b)
{
	int cmp;

	if (a->len > b->len)
		return 1;

	if (b->len > a->len)
		return -1;

	cmp = HxSTD_memcmp(a->num, b->num, a->len);

	if (cmp > 0)
		return 1;

	if (cmp < 0)
		return -1;

	return 0;
}

/* compute big number (a + b) using pencil-and-paper method */
void pal_cipher_bn_add(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out)
{
	unsigned char *p, *q;
	bnLenType pLen, qLen, i;
	signedBnLenType ip, iq;
	unsigned short add;
	unsigned char tmp[MAX_BN_LEN];

	if (pal_cipher_bn_cmp(a, b) >= 0)
	{
		p = a->num; pLen = a->len;
		q = b->num; qLen = b->len;
	}
	else
	{
		p = b->num; pLen = b->len;
		q = a->num; qLen = a->len;
	}

	if (pLen+1 > MAX_BN_LEN)
	{
		HxLOG_Error("[CICC]---> bn_add()---> Error : wrong length %d !!!\n", pLen);
		out->len = 1;
		out->num[0] = 0x00;
		return;
	}

	HxSTD_memset(tmp, 0x00, pLen+1);

	i = 0;

	for (ip = pLen-1, iq = qLen-1; iq >= 0; ip--, iq--, i++)
	{
		add = (unsigned short)(p[ip] + q[iq] + tmp[i]);
		tmp[i] = (unsigned char)(add & 0xff);
		if (add > 0xff)
			tmp[i+1] = 0x01;
	}

	for (; ip >= 0; ip--, i++)
	{
		add = (unsigned short)(p[ip] + tmp[i]);
		tmp[i] = (unsigned char)(add & 0xff);
		if (add > 0xff)
			tmp[i+1] = 0x01;
	}

	out->len = (tmp[i] == 0x00) ? i : i+1;

	for (i = 0; i < out->len; i++)
		out->num[i] = tmp[out->len-1-i];
}

/* compute big number (a - b) using pencil-and-paper method : return 1 and compute b - a if a < b, return 0 and compute a - b if a >= b */
/* 주의 : 여기 구현된 PalCipher_BigNumber_e 라이브러리는 양수만을 취급한다. 따라서 b > a 인 경우에 대한 처리는 caller 쪽에서 핸들링해야 한다! */
int pal_cipher_bn_sub(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out)
{
	unsigned char *p, *q;
	bnLenType pLen, qLen, i;
	signedBnLenType ip, iq;
	unsigned short sub;
	unsigned char tmp[MAX_BN_LEN];
	int cmp;

	cmp = pal_cipher_bn_cmp(a, b);

	if (cmp == 0) // 불필요한 연산을 막기 위해.
	{
		out->len = 1;
		out->num[0] = 0x00;
		return 0;
	}
	else if (cmp > 0)
	{
		p = a->num; pLen = a->len;
		q = b->num; qLen = b->len;
	}
	else
	{
		p = b->num; pLen = b->len;
		q = a->num; qLen = a->len;
	}

	HxSTD_memset(tmp, 0x00, pLen);

	i = 0;

	for (ip = pLen-1, iq = qLen-1; iq >= 0; ip--, iq--, i++)
	{
		sub = q[iq] + tmp[i];
		if (p[ip] < sub)
		{
			tmp[i] = (unsigned char)((0x100 + p[ip] - sub) & 0xff);
			tmp[i+1] = 0x01;
		}
		else
		{
			tmp[i] = (unsigned char)((p[ip] - sub) & 0xff);
		}
	}

	for (; ip >= 0; ip--, i++)
	{
		if (p[ip] < tmp[i])
		{
			tmp[i] = (unsigned char)((0x100 + p[ip] - tmp[i]) & 0xff);
			tmp[i+1] = 0x01;
		}
		else
		{
			tmp[i] = (unsigned char)((p[ip] - tmp[i]) & 0xff);
		}
	}

	for (; i > 0; i--)
	{
		if (tmp[i-1] != 0x00)
			break;
	}
	out->len = (i == 0) ? 1 : i;

	for (i = 0; i < out->len; i++)
		out->num[i] = tmp[out->len-1-i];

	return (cmp < 0) ? 1 : 0;
}


/* compute big number (a x b) using pencil-and-paper method */
/* 입력값이 각각 P, Q 바이트일 때 O(PxQ)가 소요되는 일반적인(?) 방식. 나중에 Karatsuba multiplication 방식 같은 거로 개선해 보자.
   다만, b가 2의 배수이면 bn_mul_2_n(), bn_mul_256_n()을 이용하고 제곱의 경우엔 bn_square()를 이용하도록 한다.
   한편, a x b = ((a+b)^2 - (a-b)^2) / 4 이므로 퍼포먼스가 더 좋다면 bn_mul() 대신에 bn_square() 및 bn_right_shift_n()을 이용할 수도 있다. */
void pal_cipher_bn_mul(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *out)
{
	unsigned char *p, *q;
	bnLenType pLen, qLen, i, j, k;
	signedBnLenType ip, iq;
	unsigned short mul;
	unsigned char tmp[MAX_BN_LEN];

	p = a->num; pLen = a->len;
	q = b->num; qLen = b->len;

	if ((pal_cipher_bn_cmp(a, &crypt_bn_zero) == 0) || (pal_cipher_bn_cmp(b, &crypt_bn_zero) == 0)) // 불필요한 연산을 막기 위해 (자리수 오류 보정 포함).
	{
		out->len = 1;
		out->num[0] = 0x00;
		return;
	}

	if (pal_cipher_bn_cmp(a, &crypt_bn_one) == 0) // 불필요한 연산을 막기 위해.
	{
		out->len = qLen;
		HxSTD_memcpy(out->num, q, qLen);
		return;
	}

	if (pal_cipher_bn_cmp(b, &crypt_bn_one) == 0) // 불필요한 연산을 막기 위해.
	{
		out->len = pLen;
		HxSTD_memcpy(out->num, p, pLen);
		return;
	}

	if (pLen+qLen > MAX_BN_LEN)
	{
		HxLOG_Error("[CICC]---> bn_mul()---> Error : wrong length %d, %d !!!\n", pLen, qLen);
		out->len = 1;
		out->num[0] = 0x00;
		return;
	}

	HxSTD_memset(tmp, 0x00, pLen+qLen);

	i = 0;

	for (iq = qLen-1, j = 0; iq >= 0; iq--, j++)
	{
		for (ip = pLen-1, i = j; ip >= 0; ip--, i++)
		{
			mul = (unsigned short)(p[ip] * q[iq] + tmp[i]);
			tmp[i] = (unsigned char)(mul & 0xff);
			mul >>= 8;
			for (k = 1; mul; k++)
			{
				mul += (unsigned short)tmp[i+k];
				tmp[i+k] = (unsigned char)(mul & 0xff);
				mul >>= 8;
			}
		}
	}

	out->len = (tmp[i] == 0x00) ? i : i+1;

	for (i = 0; i < out->len; i++)
		out->num[i] = tmp[out->len-1-i];
}

#ifndef PAL_CIPHER_USE_MPI_LIB

/* compute big number (a ^ b mod n) using repeated square-and-multiply (right-to-left binary) method for modular exponentiation */
void pal_cipher_bn_mod_exp(PalCipher_BigNumber_e *a, PalCipher_BigNumber_e *b, PalCipher_BigNumber_e *n, PalCipher_BigNumber_e *out)
{
	PalCipher_BigNumber_e base, tmp;
	#if 0 // old
	PalCipher_BigNumber_e exponent;
	#else
	bnLenType totBits, modBits;
	#endif

	if (pal_cipher_bn_cmp(n, &crypt_bn_zero) == 0)
	{
		HxLOG_Error("[CICC]---> bn_mod_exp()---> Error : divided by zero !!!\n");
		return;
	}

	if (pal_cipher_bn_cmp(b, &crypt_bn_zero) == 0) // any a ^ 0 = 1
	{
		tmp.len = 1;
		tmp.num[0] = 0x01;
		pal_cipher_bn_mod(&tmp, n, out);
		return;
	}

	if ((pal_cipher_bn_cmp(a, &crypt_bn_zero) == 0) || (pal_cipher_bn_cmp(a, &crypt_bn_one) == 0)) // 불필요한 연산을 막기 위해.
	{
		pal_cipher_bn_mod(a, n, out);
		return;
	}

	if (pal_cipher_bn_cmp(b, &crypt_bn_one) == 0) // 불필요한 연산을 막기 위해.
	{
		pal_cipher_bn_mod(a, n, out);
		return;
	}

	out->len = 1;
	out->num[0] = 0x01;

	base.len = a->len;
	HxSTD_memcpy(base.num, a->num, a->len);

	#if 0 // old
	exponent.len = b->len;
	HxSTD_memcpy(exponent.num, b->num, b->len);
	while (pal_cipher_bn_cmp(&exponent, &crypt_bn_zero) > 0)
	{
		if (exponent.num[exponent.len-1] & 0x01)
		{
			pal_cipher_bn_mul(out, &base, &tmp);
			pal_cipher_bn_mod(&tmp, n, out);
		}
		bn_right_shift_n(&exponent, 1, &tmp);
		exponent.len = tmp.len;
		HxSTD_memcpy(exponent.num, tmp.num, tmp.len);
		pal_cipher_bn_square(&base, &tmp);
		pal_cipher_bn_mod(&tmp, n, &base);
		#ifdef PAL_CIPHER_GIVE_TASK_SLEEP_FOR_BIG_COMPUTATION
		VK_TASK_Sleep(10); // 상당 시간이 소요되므로 최소 이 정도 sleep 필요.
		#endif
	}
	#else //
	totBits = b->len * 8;
	for (modBits = 0; modBits < 8; modBits++)
	{
		if (b->num[0] & (0x80 >> modBits))
			break;
	}
	modBits = totBits - modBits;
	while (modBits)
	{
		totBits--;
		modBits--;
		if (b->num[totBits/8] & (0x80 >> (totBits&7)))
		{
			pal_cipher_bn_mul(out, &base, &tmp);
			pal_cipher_bn_mod(&tmp, n, out);
		}
		local_cipher_bn_square(&base, &tmp);
		pal_cipher_bn_mod(&tmp, n, &base);

		#ifdef PAL_CIPHER_GIVE_TASK_SLEEP_FOR_BIG_COMPUTATION
		VK_TASK_Sleep(10); // 상당 시간이 소요되므로 최소 이 정도 sleep 필요.
		#endif
	}
	#endif //
}

#endif // #ifndef USE_MPI_LIB

/***************************************************************
* for self-test ////////////////////////////////////////////////
***************************************************************/

#ifdef INCLUDE_BN_TEST
void pal_cipher_BN_Test(void)
{
	PalCipher_BigNumber_e A = {7, {0xff,0x77,0xee,0x22,0xdd,0x33,0xcc}};
	PalCipher_BigNumber_e B = {7, {0xff,0x76,0xff,0x32,0x10,0x01,0x02}};
	PalCipher_BigNumber_e C = {4, {0xff,0x11,0xee,0x22}};
	PalCipher_BigNumber_e D = {3, {0xfe,0x76,0x54}};
	PalCipher_BigNumber_e E = {3, {0x0e,0x76,0x54}};
	PalCipher_BigNumber_e F = {2, {0xab,0xcd}};
	PalCipher_BigNumber_e G = {1, {0x03}};
//	PalCipher_BigNumber_e H = {1, {0x02}};
//	PalCipher_BigNumber_e I = {1, {0x01}};
	PalCipher_BigNumber_e J = {1, {0x00}};
	PalCipher_BigNumber_e n = {128, {
		0xA5, 0x6E, 0x4A, 0x0E, 0x70, 0x10, 0x17, 0x58, 0x9A, 0x51, 0x87, 0xDC, 0x7E, 0xA8, 0x41, 0xD1,
		0x56, 0xF2, 0xEC, 0x0E, 0x36, 0xAD, 0x52, 0xA4, 0x4D, 0xFE, 0xB1, 0xE6, 0x1F, 0x7A, 0xD9, 0x91,
		0xD8, 0xC5, 0x10, 0x56, 0xFF, 0xED, 0xB1, 0x62, 0xB4, 0xC0, 0xF2, 0x83, 0xA1, 0x2A, 0x88, 0xA3,
		0x94, 0xDF, 0xF5, 0x26, 0xAB, 0x72, 0x91, 0xCB, 0xB3, 0x07, 0xCE, 0xAB, 0xFC, 0xE0, 0xB1, 0xDF,
		0xD5, 0xCD, 0x95, 0x08, 0x09, 0x6D, 0x5B, 0x2B, 0x8B, 0x6D, 0xF5, 0xD6, 0x71, 0xEF, 0x63, 0x77,
		0xC0, 0x92, 0x1C, 0xB2, 0x3C, 0x27, 0x0A, 0x70, 0xE2, 0x59, 0x8E, 0x6F, 0xF8, 0x9D, 0x19, 0xF1,
		0x05, 0xAC, 0xC2, 0xD3, 0xF0, 0xCB, 0x35, 0xF2, 0x92, 0x80, 0xE1, 0x38, 0x6B, 0x6F, 0x64, 0xC4,
		0xEF, 0x22, 0xE1, 0xE1, 0xF2, 0x0D, 0x0C, 0xE8, 0xCF, 0xFB, 0x22, 0x49, 0xBD, 0x9A, 0x21, 0x37}
	};
	/*
	PalCipher_BigNumber_e d = {128, {
		0x33, 0xA5, 0x04, 0x2A, 0x90, 0xB2, 0x7D, 0x4F, 0x54, 0x51, 0xCA, 0x9B, 0xBB, 0xD0, 0xB4, 0x47,
		0x71, 0xA1, 0x01, 0xAF, 0x88, 0x43, 0x40, 0xAE, 0xF9, 0x88, 0x5F, 0x2A, 0x4B, 0xBE, 0x92, 0xE8,
		0x94, 0xA7, 0x24, 0xAC, 0x3C, 0x56, 0x8C, 0x8F, 0x97, 0x85, 0x3A, 0xD0, 0x7C, 0x02, 0x66, 0xC8,
		0xC6, 0xA3, 0xCA, 0x09, 0x29, 0xF1, 0xE8, 0xF1, 0x12, 0x31, 0x88, 0x44, 0x29, 0xFC, 0x4D, 0x9A,
		0xE5, 0x5F, 0xEE, 0x89, 0x6A, 0x10, 0xCE, 0x70, 0x7C, 0x3E, 0xD7, 0xE7, 0x34, 0xE4, 0x47, 0x27,
		0xA3, 0x95, 0x74, 0x50, 0x1A, 0x53, 0x26, 0x83, 0x10, 0x9C, 0x2A, 0xBA, 0xCA, 0xBA, 0x28, 0x3C,
		0x31, 0xB4, 0xBD, 0x2F, 0x53, 0xC3, 0xEE, 0x37, 0xE3, 0x52, 0xCE, 0xE3, 0x4F, 0x9E, 0x50, 0x3B,
		0xD8, 0x0C, 0x06, 0x22, 0xAD, 0x79, 0xC6, 0xDC, 0xEE, 0x88, 0x35, 0x47, 0xC6, 0xA3, 0xB3, 0x25}
	};
	*/
	PalCipher_BigNumber_e p = {64, {
		0xE7, 0xE8, 0x94, 0x27, 0x20, 0xA8, 0x77, 0x51, 0x72, 0x73, 0xA3, 0x56, 0x05, 0x3E, 0xA2, 0xA1,
		0xBC, 0x0C, 0x94, 0xAA, 0x72, 0xD5, 0x5C, 0x6E, 0x86, 0x29, 0x6B, 0x2D, 0xFC, 0x96, 0x79, 0x48,
		0xC0, 0xA7, 0x2C, 0xBC, 0xCC, 0xA7, 0xEA, 0xCB, 0x35, 0x70, 0x6E, 0x09, 0xA1, 0xDF, 0x55, 0xA1,
		0x53, 0x5B, 0xD9, 0xB3, 0xCC, 0x34, 0x16, 0x0B, 0x3B, 0x6D, 0xCD, 0x3E, 0xDA, 0x8E, 0x64, 0x43}
	};
	PalCipher_BigNumber_e q = {64, {
		0xB6, 0x9D, 0xCA, 0x1C, 0xF7, 0xD4, 0xD7, 0xEC, 0x81, 0xE7, 0x5B, 0x90, 0xFC, 0xCA, 0x87, 0x4A,
		0xBC, 0xDE, 0x12, 0x3F, 0xD2, 0x70, 0x01, 0x80, 0xAA, 0x90, 0x47, 0x9B, 0x6E, 0x48, 0xDE, 0x8D,
		0x67, 0xED, 0x24, 0xF9, 0xF1, 0x9D, 0x85, 0xBA, 0x27, 0x58, 0x74, 0xF5, 0x42, 0xCD, 0x20, 0xDC,
		0x72, 0x3E, 0x69, 0x63, 0x36, 0x4A, 0x1F, 0x94, 0x25, 0x45, 0x2B, 0x26, 0x9A, 0x67, 0x99, 0xFD}
	};
	PalCipher_BigNumber_e dP = {64, {
		0x28, 0xFA, 0x13, 0x93, 0x86, 0x55, 0xBE, 0x1F, 0x8A, 0x15, 0x9C, 0xBA, 0xCA, 0x5A, 0x72, 0xEA,
		0x19, 0x0C, 0x30, 0x08, 0x9E, 0x19, 0xCD, 0x27, 0x4A, 0x55, 0x6F, 0x36, 0xC4, 0xF6, 0xE1, 0x9F,
		0x55, 0x4B, 0x34, 0xC0, 0x77, 0x79, 0x04, 0x27, 0xBB, 0xDD, 0x8D, 0xD3, 0xED, 0xE2, 0x44, 0x83,
		0x28, 0xF3, 0x85, 0xD8, 0x1B, 0x30, 0xE8, 0xE4, 0x3B, 0x2F, 0xFF, 0xA0, 0x27, 0x86, 0x19, 0x79}
	};
	PalCipher_BigNumber_e dQ = {64, {
		0x1A, 0x8B, 0x38, 0xF3, 0x98, 0xFA, 0x71, 0x20, 0x49, 0x89, 0x8D, 0x7F, 0xB7, 0x9E, 0xE0, 0xA7,
		0x76, 0x68, 0x79, 0x12, 0x99, 0xCD, 0xFA, 0x09, 0xEF, 0xC0, 0xE5, 0x07, 0xAC, 0xB2, 0x1E, 0xD7,
		0x43, 0x01, 0xEF, 0x5B, 0xFD, 0x48, 0xBE, 0x45, 0x5E, 0xAE, 0xB6, 0xE1, 0x67, 0x82, 0x55, 0x82,
		0x75, 0x80, 0xA8, 0xE4, 0xE8, 0xE1, 0x41, 0x51, 0xD1, 0x51, 0x0A, 0x82, 0xA3, 0xF2, 0xE7, 0x29}
	};
	PalCipher_BigNumber_e qInv = {64, {
		0x27, 0x15, 0x6A, 0xBA, 0x41, 0x26, 0xD2, 0x4A, 0x81, 0xF3, 0xA5, 0x28, 0xCB, 0xFB, 0x27, 0xF5,
		0x68, 0x86, 0xF8, 0x40, 0xA9, 0xF6, 0xE8, 0x6E, 0x17, 0xA4, 0x4B, 0x94, 0xFE, 0x93, 0x19, 0x58,
		0x4B, 0x8E, 0x22, 0xFD, 0xDE, 0x1E, 0x5A, 0x2E, 0x3B, 0xD8, 0xAA, 0x5B, 0xA8, 0xD8, 0x58, 0x41,
		0x94, 0xEB, 0x21, 0x90, 0xAC, 0xF8, 0x32, 0xB8, 0x47, 0xF1, 0x3A, 0x3D, 0x24, 0xA7, 0x9F, 0x4D}
	};
	PalCipher_BigNumber_e X, Y;
	bnLenType i;

	HxLOG_Print("\n BN_Test Start... ");

	pal_cipher_bn_add(&A, &B, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 8 : 01 FE EE ED 54 ED 34 CE

	pal_cipher_bn_add(&B, &A, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 8 : 01 FE EE ED 54 ED 34 CE

	pal_cipher_bn_add(&A, &J, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 7 : FF 77 EE 22 DD 33 CC

	pal_cipher_bn_sub(&A, &B, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 5 : EE F0 CD 32 CA

//	bn_sub(&B, &A, &X);
//	bn_neg(&X, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be ? : 11 0F 32 CD 36

	pal_cipher_bn_sub(&A, &J, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 7 : FF 77 EE 22 DD 33 CC

//	bn_sub(&J, &A, &X);
//	bn_neg(&X, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be ? : 00 88 11 DD 22 CC 34

	pal_cipher_bn_sub(&A, &A, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 00

	pal_cipher_bn_mul(&C, &D, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 7 : FD 89 B0 3B 61 CF 28

	pal_cipher_bn_mul(&C, &J, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 00

#ifndef USE_MPI_LIB

	local_cipher_bn_square(&C, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 8 : FE 24 B9 A9 3A 07 3C 84

	local_cipher_bn_square(&D, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 6 : FC EF 05 61 8B 90

	local_cipher_bn_square(&E, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 5 : D1 27 E1 8B 90

	local_cipher_bn_square(&F, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 4 : 73 4B 82 29

#endif

//	bn_mul_2_n(&C, 1, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 5 : 01 FE 23 DC 44

//	bn_mul_2_n(&E, 1, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 3 : 1C EC A8

//	bn_mul_2_n(&G, 1, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 06

//	bn_mul_2_n(&C, 4, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 5 : 0F F1 1E E2 20

//	bn_mul_2_n(&E, 4, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 3 : E7 65 40

//	bn_mul_2_n(&G, 4, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 30

	local_cipher_bn_mul_256_n(&C, 1, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 5 : FF 11 EE 22 00

//	bn_right_shift_n(&F, 1, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 2 : 55 E6

//	bn_right_shift_n(&F, 2, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 2 : 2A F3

//	bn_right_shift_n(&F, 3, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 2 : 15 79

//	bn_right_shift_n(&F, 4, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 2 : 0A BC

//	bn_pow(&F, &G, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 6 : 4D 5F C3 68 9D D5

//	bn_pow(&F, &H, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 4 : 73 4B 82 29

//	bn_pow(&F, &I, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 2 : AB CD

//	bn_pow(&F, &J, &X);
//	HxLOG_Print("\n %d :", X.len);
//	for (i = 0; i < X.len; i++)
//		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 01

	pal_cipher_bn_mod(&A, &B, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 5 : EE F0 CD 32 CA

	pal_cipher_bn_mod(&A, &C, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 4 : 34 54 49 2E

	pal_cipher_bn_mod(&A, &D, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 3 : 69 80 04

	pal_cipher_bn_mod(&A, &F, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 2 : 32 4E

	pal_cipher_bn_mod(&A, &G, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 00

	pal_cipher_bn_mod(&J, &A, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 00

	pal_cipher_bn_mod(&B, &A, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 7 : FF 76 FF 32 10 01 02

	pal_cipher_bn_mod_exp(&F, &G, &C, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 4 : FA 16 0B A7

	pal_cipher_bn_mod_exp(&F, &G, &D, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 3 : 7B 10 C9

	pal_cipher_bn_mod_exp(&F, &G, &E, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 3 : 08 E5 E5

	pal_cipher_bn_mod_exp(&F, &G, &F, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 00

	pal_cipher_bn_mod_exp(&F, &G, &G, &X);
	HxLOG_Print("\n %d :", X.len);
	for (i = 0; i < X.len; i++)
		HxLOG_Print(" %02X", X.num[i]); // should be 1 : 01

	pal_cipher_bn_mul(&p, &q, &X);
	if (pal_cipher_bn_cmp(&X, &n) == 0)
		HxLOG_Print("\n OK"); // should be
	else
		HxLOG_Print("\n NOT OK !!!");

	pal_cipher_bn_mul(&crypt_bn_e, &dP, &X);
	pal_cipher_bn_sub(&p, &crypt_bn_one, &Y);
	pal_cipher_bn_mod(&X, &Y, &X);
	if (pal_cipher_bn_cmp(&X, &crypt_bn_one) == 0)
		HxLOG_Print("\n OK"); // should be
	else
		HxLOG_Print("\n NOT OK !!!");

	pal_cipher_bn_mul(&crypt_bn_e, &dQ, &X);
	pal_cipher_bn_sub(&q, &crypt_bn_one, &Y);
	pal_cipher_bn_mod(&X, &Y, &X);
	if (pal_cipher_bn_cmp(&X, &crypt_bn_one) == 0)
		HxLOG_Print("\n OK"); // should be
	else
		HxLOG_Print("\n NOT OK !!!");

	pal_cipher_bn_mul(&q, &qInv, &X);
	pal_cipher_bn_mod(&X, &p, &X);
	if (pal_cipher_bn_cmp(&X, &crypt_bn_one) == 0)
		HxLOG_Print("\n OK"); // should be
	else
		HxLOG_Print("\n NOT OK !!!");

	HxLOG_Print("\n BN_Test End. \n");
}
#endif // #ifdef INCLUDE_BN_TEST

