#ifndef	_MWC_CODEPAGE_H_
#define	_MWC_CODEPAGE_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <stb_params.h>
#include <isosvc.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Function Definition  ********************/
/*******************************************************************/
CPContext_t 	*MWC_UTIL_CodePage_GetContext(eHxCP_CodePage_e eTextEncoding);
HERROR 			MWC_UTIL_CodePage_Register(eHxCP_CodePage_e eTextEncoding,
								MwcCountDrawableCharNum 	cbCountDrawableCharNum,
					  			MwcGetDrawableChar			cbGetDrawableChar,
					  			MwcConvertCodepageToUCS2	cbConvertCodepageToUCS2,
							  	MwcConvertUCS2ToCodepage	cbConvertUCS2ToCodepage, /* optional */
							  	MwcGetEmphasizedString		cbGetEmphasizedString,	 /* optional */
							  	void					*pTable);
HERROR 			MWC_UTIL_CodePage_Init(void);
HERROR			MWC_UTIL_CodePage_GetEncodingWithLenByLocaleCode(const CH_1BYTE *pcStr, eHxCP_CodePage_e *peEncoding, HINT32 *pulEncodingLen, eHxCP_CodePage_e eDefaultEncoding);



HERROR MWC_UTIL_CodePage_RegisterASCII(void);
HERROR MWC_UTIL_CodePage_Register8859(void);
HERROR MWC_UTIL_CodePage_RegisterUFT8(void);
HERROR MWC_UTIL_CodePage_RegisterUCS2(void);

HERROR MWC_UTIL_CodePage_Register6937With300468(void);
HERROR MWC_UTIL_CodePage_GetCharSizeByEncoding(eHxCP_CodePage_e eDefaultEncoding, HINT32 *pnCharLength);



#if defined(CONFIG_MWC_CODEPAGE_JAPAN)
HERROR MWC_UTIL_CodePage_RegisterJP(void);
#endif

#if defined(CONFIG_MWC_CODEPAGE_WINCP)
HERROR MWC_UTIL_CodePage_RegisterWINCP(void);
#endif

#endif /* _MWC_CODEPAGE_H_ */

