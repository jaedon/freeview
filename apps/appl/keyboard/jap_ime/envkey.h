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
**		Å° ¾î»çÀÎ È¯°æ ÆÄÀÏ (Çì´õ) 
*/

#if 0
#ifndef X68K

/****************************/
/***  ‚o‚b‚X‚W—pƒL[’è‹`  ***/
/****************************/

#define EKBEGIN 		0		/* Š¿š“ü—ÍŠJn 		*/
#define EKEND			1		/* Š¿š“ü—ÍI—¹ 		*/
#define EK1XFER 		2		/* •ÏŠ· 				*/
#define EK2XFER 		3		/* •ÏŠ· 				*/
#define EKENTER 		4		/* ‘SŠm’è				*/
#define EKDEL			5		/* ƒJ[ƒ\ƒ‹ˆÊ’uíœ 	*/
#define EKRIGHT 		6		/* ƒJ[ƒ\ƒ‹‰E			*/
#define EKLEFT			7		/* ƒJ[ƒ\ƒ‹¶			*/
#define EKHOME			8		/* ƒJ[ƒ\ƒ‹Å¶ 		*/
#define EKCLR			9		/* ‰¼“ü—ÍƒNƒŠƒA 		*/
#define EKHIRAKATA		10		/* ‚Ğ‚çƒJƒ^Ø‘Ö 		*/
#define EKZENHAN		11		/* ‘SŠp”¼ŠpØ‘Ö 		*/
#define EKEISUU 		12		/* ‰p”ƒ[ƒ}šØ‘Ö 	*/
#define EKZH			13		/* ‘SŠp”¼ŠpØ‘Ö 		*/
#define EKHIRA			14		/* ‚Ğ‚ç‚ª‚È•ÏŠ· 		*/
#define EKKATA			15		/* ƒJƒ^ƒJƒi•ÏŠ· 		*/
#define EKHAN			16		/* ”¼Šp•ÏŠ· 			*/
#define EKRAW			17		/* “ü—ÍƒL[•ÏŠ· 		*/
#define EK1NXK			18		/* ŸŒó•â				*/
#define EK2NXK			19		/* ŸŒó•â				*/
#define EK3NXK			20		/* ŸŒó•â				*/
#define EK1BKK			21		/* ‘OŒó•â				*/
#define EK2BKK			22		/* ‘OŒó•â				*/
#define EK3BKK			23		/* ‘OŒó•â				*/
#define EKNXB			24		/* ŸŒó•âŒQ 			*/
#define EKBKB			25		/* ‘OŒó•âŒQ 			*/
#define EKSH			26		/* •¶ß’·’Z‚­			*/
#define EKLN			27		/* •¶ß’·’·‚­			*/
#define EKNBUN			28		/* Ÿ•¶ß‚Ö 			*/
#define EKBBUN			29		/* ‘O•¶ß‚Ö 			*/
#define EK1ONERET		30		/* æ“ª•¶ßŠm’è 		*/
#define EK2ONERET		31		/* æ“ª•¶ßŠm’è 		*/
#define EKCODEIN		32		/* ƒR[ƒh“ü—Í			*/
#define EKCODE			33		/* ‹æ“_Ø‘Ö 			*/
#define EKLEARN 		34		/* ŠwKON/OFF			*/
#define EKDIC			35		/* «‘–¼•ÏX			*/
#define EKTYPE			36		/* æ“Ç^’€Ÿ			*/
#define EKECHO			37		/* ƒGƒR[ƒ‚[ƒh 		*/
#define EKUNDO			38		/* ƒAƒ“ƒhƒD 			*/
#define EKONLINE		39		/* ƒIƒ“ƒ‰ƒCƒ““o˜^		*/
#define EKCANCEL		40		/* ƒLƒƒƒ“ƒZƒ‹			*/
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
/***  ‚w‚U‚W‚O‚O‚O—pƒL[’è‹`  ***/
/********************************/

#define EKBEGIN 		0		/* Š¿š“ü—ÍŠJn 		*/
#define EKEND			1		/* Š¿š“ü—ÍI—¹ 		*/
#define EK1XFER 		2		/* •ÏŠ· 				*/
#define EK2XFER 		3		/* •ÏŠ· 				*/
#define EKENTER 		4		/* ‘SŠm’è				*/
#define EKTYPE			5		/* æ“Ç^’€Ÿ			*/
#define EKDEL			6		/* ƒJ[ƒ\ƒ‹ˆÊ’uíœ 	*/
#define EKRIGHT 		7		/* ƒJ[ƒ\ƒ‹‰E			*/
#define EKLEFT			8		/* ƒJ[ƒ\ƒ‹¶			*/
#define EKHOME			9		/* ƒJ[ƒ\ƒ‹Å¶ 		*/
#define EKCLR			10		/* ‰¼“ü—ÍƒNƒŠƒA 		*/
#define EKHIRA			11		/* ‚Ğ‚ç‚ª‚È•ÏŠ· 		*/
#define EKKATA			12		/* ƒJƒ^ƒJƒi•ÏŠ· 		*/
#define EKHAN			13		/* ”¼Šp•ÏŠ· 			*/
#define EKRAW			14		/* “ü—ÍƒL[•ÏŠ· 		*/
#define EK1ONERET		15		/* æ“ª•¶ßŠm’è 		*/
#define EKCODE			16		/* ‹æ“_Ø‘Ö 			*/
#define EKLEARN 		17		/* ŠwKON/OFF			*/
#define EKDIC			18		/* «‘–¼•ÏX			*/
#define EK1NXK			19		/* ŸŒó•â				*/
#define EK2NXK			20		/* ŸŒó•â				*/
#define EK3NXK			21		/* ŸŒó•â				*/
#define EK1BKK			22		/* ‘OŒó•â				*/
#define EK2BKK			23		/* ‘OŒó•â				*/
#define EK3BKK			24		/* ‘OŒó•â				*/
#define EKNXB			25		/* ŸŒó•âŒQ 			*/
#define EKBKB			26		/* ‘OŒó•âŒQ 			*/
#define EKSH			27		/* •¶ß’·’Z‚­			*/
#define EKLN			28		/* •¶ß’·’·‚­			*/
#define EKNBUN			29		/* Ÿ•¶ß‚Ö 			*/
#define EKBBUN			30		/* ‘O•¶ß‚Ö 			*/
#define EKECHO			31		/* ƒGƒR[ƒ‚[ƒh 		*/
#define EKCANCEL		32		/* ƒLƒƒƒ“ƒZƒ‹			*/
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

/* Ä’è‹`•s‰Â */
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
