/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mw_ttxerrchk.h
	@brief	  mw_ttxerrchk.h : TTX Error Protection

	Description: File�� ���� ������ �����ϴ�.		\n
	Module: Layer�� ��� ���� �����ϴ�.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MW_TTXERRCHK_H__
#define	__MW_TTXERRCHK_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "htype.h"


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

enum
{
	eTTX_ERR_PARITY		= 0xFF,
	eTTX_ERR_HAMMING84	= 0xFF
};




/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/


//1 TELETEXT Error Protection APIs

/**
 * Ȧ�� �и�Ƽ �˻�
 * [D1|D2|D3|D4|D5|D6|D7| P]
 * => [ 0|D7]D6|D5|D4|D3|D2|D1]
 *
 * @param	pucDest	�˻�� ��� �迭 (bit order = big endian)
 * @param	pucSrc	Ȧ�� �и�Ƽ�� ���ڵ��� ����Ʈ �迭(bit order = little endian)
 * @param	ulLen	�˻��ϰ��� �ϴ� �迭�� ����
 * @return	���� ���� or -1(���� ����!!)
 */
HINT32	TTX_ERRCHK_OddParity	(HUINT8 *pucDest, const HUINT8 *pucSrc, HUINT32 ulLen);


/**
 * �ع� 8/4 �ڵ�
 * [P1|D1|P2|D2|P3|D3|P4|D4]
 * => [ 0| 0| 0| 0|D4|D3|D2|D1]
 *
 * @param	pucDest	���ڵ� �� ��� �� �迭(bit order = big endian)
 * @param	pucSrc	�ع� 8/4 ���ڵ� �� ����Ʈ �迭(bit order = little endian)
 * @param	ulLen	�˻��ϰ��� �ϴ� �迭�� ����
 * @return	���� ���� or -1(���� ����!!)
 */
HINT32	TTX_ERRCHK_Hamming84	(HUINT8 *pucDest, const HUINT8 *pucSrc, HUINT32 ulLen);


/**
 * �ع� 24/18 �ڵ�
 * [P1|P2|D1|P3|D2|D3|D4|P4] [D5|D6|D7|D8|D9|D10|D11|P5] [D12|D13|D14|D15|D16|D17|D18|P6]
 * => [D4|D3|D2|D1] [D11|D10|D9|D8|D7|D6|D5] [D18|D17|D16|D15|D14|D13|D12]
 *
 * @param	punDest	���ڵ� �� ��� �� (bit order = big endian)
 * @param	pucSrc	�ع� 24/18�� ���ڵ��� ���ӵ� �� ���� ����Ʈ(bit order = little endian)
 * @param	ulLen	�˻��ϰ��� �ϴ� �迭�� ����
 * @return	���� ���� or -1(���� ����!!)
 */
HINT32	TTX_ERRCHK_Hamming2418 (HUINT8 *pucDest, const HUINT8 *pucSrc, HUINT32 ulLen);


/**
 * Change bit order (big to little, little to big)
 *
 * @param	pucDest	bit order = big endian
 * @param	pucSrc	bit order = little endian
 * @param	ulLen	�˻��ϰ��� �ϴ� �迭�� ����
 * @return	���� ���� or -1(���� ����!!)
 */
HINT32	TTX_ERRCHK_ReverseBit	(HUINT8 *pucDest, const HUINT8 *pucSrc, HUINT32 ulLen);


#endif /* !__MW_TTXERRCHK_H__ */

