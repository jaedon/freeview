/*
 * (c) 2011-2012 Humax Co., Ltd. 
 * This program is produced by Humax Co., Ltd. ("Humax") and 
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee, 
 * non-assignable, non-transferable and non-exclusive license to use this Software. 
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software 
 * you agree to the responsibility to take all reasonable efforts to protect the any information 
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and 
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, 
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE. 
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies 
 * of the Software including all documentation. This License will terminate immediately without notice from Humax 
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies 
 * of the Software and all documentation.  

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice. 
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court, 
 * in the Republic of Korea.
*/
/*
	envkey.h
	Copyright(c) 1996-2005 ACCESS CO., LTD.
	All rights are reserved by ACCESS CO., LTD., whether the whole or
	part of the source code including any modifications.
*/


/*
**		Ű ����� ȯ�� ���� (���) 
*/

#if 0
#ifndef X68K

/****************************/
/***  �o�b�X�W�p�L�[��`  ***/
/****************************/

#define EKBEGIN 		0		/* �������͊J�n 		*/
#define EKEND			1		/* �������͏I�� 		*/
#define EK1XFER 		2		/* �ϊ� 				*/
#define EK2XFER 		3		/* �ϊ� 				*/
#define EKENTER 		4		/* �S�m��				*/
#define EKDEL			5		/* �J�[�\���ʒu�폜 	*/
#define EKRIGHT 		6		/* �J�[�\���E			*/
#define EKLEFT			7		/* �J�[�\����			*/
#define EKHOME			8		/* �J�[�\���ō� 		*/
#define EKCLR			9		/* �����̓N���A 		*/
#define EKHIRAKATA		10		/* �Ђ�J�^�ؑ� 		*/
#define EKZENHAN		11		/* �S�p���p�ؑ� 		*/
#define EKEISUU 		12		/* �p�����[�}���ؑ� 	*/
#define EKZH			13		/* �S�p���p�ؑ� 		*/
#define EKHIRA			14		/* �Ђ炪�ȕϊ� 		*/
#define EKKATA			15		/* �J�^�J�i�ϊ� 		*/
#define EKHAN			16		/* ���p�ϊ� 			*/
#define EKRAW			17		/* ���̓L�[�ϊ� 		*/
#define EK1NXK			18		/* �����				*/
#define EK2NXK			19		/* �����				*/
#define EK3NXK			20		/* �����				*/
#define EK1BKK			21		/* �O���				*/
#define EK2BKK			22		/* �O���				*/
#define EK3BKK			23		/* �O���				*/
#define EKNXB			24		/* �����Q 			*/
#define EKBKB			25		/* �O���Q 			*/
#define EKSH			26		/* ���ߒ��Z��			*/
#define EKLN			27		/* ���ߒ�����			*/
#define EKNBUN			28		/* �����߂� 			*/
#define EKBBUN			29		/* �O���߂� 			*/
#define EK1ONERET		30		/* �擪���ߊm�� 		*/
#define EK2ONERET		31		/* �擪���ߊm�� 		*/
#define EKCODEIN		32		/* �R�[�h����			*/
#define EKCODE			33		/* ��_�ؑ� 			*/
#define EKLEARN 		34		/* �w�KON/OFF			*/
#define EKDIC			35		/* �������ύX			*/
#define EKTYPE			36		/* ��ǁ^����			*/
#define EKECHO			37		/* �G�R�[���[�h 		*/
#define EKUNDO			38		/* �A���h�D 			*/
#define EKONLINE		39		/* �I�����C���o�^		*/
#define EKCANCEL		40		/* �L�����Z��			*/
#define EKSIZEOF		41

#define ekbegin 		ektable[EKBEGIN]
#define ekend			ektable[EKEND]
#define ek1xfer 		ektable[EK1XFER]
#define ek2xfer 		ektable[EK2XFER]
#define ekenter 		ektable[EKENTER]
#define ekdel			ektable[EKDEL]
#define ekright 		ektable[EKRIGHT]
#define ekleft			ektable[EKLEFT]
#define ekhome			ektable[EKHOME]
#define ekclr			ektable[EKCLR]
#define ekhirakata		ektable[EKHIRAKATA]
#define ekzenhan		ektable[EKZENHAN]
#define ekeisuu 		ektable[EKEISUU]
#define ekzh			ektable[EKZH]
#define ekhira			ektable[EKHIRA]
#define ekkata			ektable[EKKATA]
#define ekhan			ektable[EKHAN]
#define ekraw			ektable[EKRAW]
#define ek1nxk			ektable[EK1NXK]
#define ek2nxk			ektable[EK2NXK]
#define ek3nxk			ektable[EK3NXK]
#define ek1bkk			ektable[EK1BKK]
#define ek2bkk			ektable[EK2BKK]
#define ek3bkk			ektable[EK3BKK]
#define eknxb			ektable[EKNXB]
#define ekbkb			ektable[EKBKB]
#define eksh			ektable[EKSH]
#define ekln			ektable[EKLN]
#define eknbun			ektable[EKNBUN]
#define ekbbun			ektable[EKBBUN]
#define ek1oneret		ektable[EK1ONERET]
#define ek2oneret		ektable[EK2ONERET]
#define ekcodein		ektable[EKCODEIN]
#define ekcode			ektable[EKCODE]
#define eklearn 		ektable[EKLEARN]
#define ekdic			ektable[EKDIC]
#define ektype			ektable[EKTYPE]
#define ekecho			ektable[EKECHO]
#define ekundo			ektable[EKUNDO]
#define ekonline		ektable[EKONLINE]
#define ekcancel		ektable[EKCANCEL]

extern BIT16	ektable[];

#else

/********************************/
/***  �w�U�W�O�O�O�p�L�[��`  ***/
/********************************/

#define EKBEGIN 		0		/* �������͊J�n 		*/
#define EKEND			1		/* �������͏I�� 		*/
#define EK1XFER 		2		/* �ϊ� 				*/
#define EK2XFER 		3		/* �ϊ� 				*/
#define EKENTER 		4		/* �S�m��				*/
#define EKTYPE			5		/* ��ǁ^����			*/
#define EKDEL			6		/* �J�[�\���ʒu�폜 	*/
#define EKRIGHT 		7		/* �J�[�\���E			*/
#define EKLEFT			8		/* �J�[�\����			*/
#define EKHOME			9		/* �J�[�\���ō� 		*/
#define EKCLR			10		/* �����̓N���A 		*/
#define EKHIRA			11		/* �Ђ炪�ȕϊ� 		*/
#define EKKATA			12		/* �J�^�J�i�ϊ� 		*/
#define EKHAN			13		/* ���p�ϊ� 			*/
#define EKRAW			14		/* ���̓L�[�ϊ� 		*/
#define EK1ONERET		15		/* �擪���ߊm�� 		*/
#define EKCODE			16		/* ��_�ؑ� 			*/
#define EKLEARN 		17		/* �w�KON/OFF			*/
#define EKDIC			18		/* �������ύX			*/
#define EK1NXK			19		/* �����				*/
#define EK2NXK			20		/* �����				*/
#define EK3NXK			21		/* �����				*/
#define EK1BKK			22		/* �O���				*/
#define EK2BKK			23		/* �O���				*/
#define EK3BKK			24		/* �O���				*/
#define EKNXB			25		/* �����Q 			*/
#define EKBKB			26		/* �O���Q 			*/
#define EKSH			27		/* ���ߒ��Z��			*/
#define EKLN			28		/* ���ߒ�����			*/
#define EKNBUN			29		/* �����߂� 			*/
#define EKBBUN			30		/* �O���߂� 			*/
#define EKECHO			31		/* �G�R�[���[�h 		*/
#define EKCANCEL		32		/* �L�����Z��			*/
#define EKSIZEOF		33

#define ekbegin 		ektable[EKBEGIN]
#define ekend			ektable[EKEND]
#define ek1xfer 		ektable[EK1XFER]
#define ek2xfer 		ektable[EK2XFER]
#define ekenter 		ektable[EKENTER]
#define ektype			ektable[EKTYPE]
#define ekdel			ektable[EKDEL]
#define ekright 		ektable[EKRIGHT]
#define ekleft			ektable[EKLEFT]
#define ekhome			ektable[EKHOME]
#define ekclr			ektable[EKCLR]
#define ekhira			ektable[EKHIRA]
#define ekkata			ektable[EKKATA]
#define ekhan			ektable[EKHAN]
#define ekraw			ektable[EKRAW]
#define ek1oneret		ektable[EK1ONERET]
#define ekcode			ektable[EKCODE]
#define eklearn 		ektable[EKLEARN]
#define ekdic			ektable[EKDIC]
#define ek1nxk			ektable[EK1NXK]
#define ek2nxk			ektable[EK2NXK]
#define ek3nxk			ektable[EK3NXK]
#define ek1bkk			ektable[EK1BKK]
#define ek2bkk			ektable[EK2BKK]
#define ek3bkk			ektable[EK3BKK]
#define eknxb			ektable[EKNXB]
#define ekbkb			ektable[EKBKB]
#define eksh			ektable[EKSH]
#define ekln			ektable[EKLN]
#define eknbun			ektable[EKNBUN]
#define ekbbun			ektable[EKBBUN]
#define ekecho			ektable[EKECHO]
#define ekcancel		ektable[EKCANCEL]

extern BIT16	ektable[];

/* �Ē�`�s�� */
#define ekhiragana		(NOT_ASCII | HIRAGANA_IN)
#define ekkatagana		(NOT_ASCII | HIRAGANA_OUT)
#define ekzenkaku		(NOT_ASCII | ZENKAKU_IN)
#define ekhankaku		(NOT_ASCII | ZENKAKU_OUT)
#define ekromaji		(NOT_ASCII | ROMAJI_IN)
#define ekeisuu 		(NOT_ASCII | ROMAJI_OUT)
#define ekcode_in		(NOT_ASCII | CODE_IN)
#define ekcode_out		(NOT_ASCII | CODE_OUT)
#define ekundo			(NOT_ASCII | UNDO_KEY)
#define ekonline		(NOT_ASCII | TOUROKU_KEY)
#define ekcodein		(NOT_ASCII | KIGOU_KEY)

#endif
#endif
