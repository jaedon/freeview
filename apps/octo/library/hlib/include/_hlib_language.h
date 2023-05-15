/*************************************************************
 * @file		hxlanguage.h
 * @date		2011/07/04
 * @author	Jack Lee (jhlee4@humaxdigital.com)
 * @brief		HUMAX common library header, language
 * http:	//www.humaxdigital.com
 *************************************************************/

#define	____HxLANGUAGE_README___________________

/******************************************************************************
 [Example Code]

int main (int argc, char *argv[])
{


	return 0;
}


 [Expected Result]

 ************************************************************************************/

#ifndef __HxLANGUAGE_H__
#define __HxLANGUAGE_H__

	//#include <htype.h>
#include "htype.h"

typedef enum
{
	  eLangID_NONE		= 0x00000000

	, eLangID_Albanian	= 0x00626C61	// "alb" Albanian
	, eLangID_Arabic	= 0x00617261	//"ara" Arabic
	, eLangID_Australian	= 0x00737561	//"aus" Australian
	, eLangID_Basque	= 0x00716162	//"baq" Basque
	, eLangID_Basaa		= 0x00736162	//"bas" Basaa
	, eLangID_Bulgarian	= 0x006C7562	//"bul" Bulgarian
	, eLangID_Catalan	= 0x00746163	//"cat" Catalan
	, eLangID_Czech		= 0x00657A63	//"cze" Czech
	, eLangID_Danish	= 0x006E6164	//"dan" Danish
	, eLangID_Dutch		= 0x00747564	//"dut" Dutch
	, eLangID_English	= 0x00676E65	//"eng" English
	, eLangID_Estonian	= 0x00747365	//"est" Estonian
	, eLangID_Faroese	= 0x006F6166	//"fao" Faroese
	, eLangID_Finnish	= 0x006E6966	//"fin" Finnish
	, eLangID_French	= 0x00657266	//"fre" French
	, eLangID_German	= 0x00726567	//"ger" German
	, eLangID_Gaelic	= 0x00616C67	//"gla" Scottish Gaelic
	, eLangID_Irish		= 0x00656C67	//"gle" Irish
	, eLangID_Galician	= 0x00676C67	//"glg" Galician
	, eLangID_Greek		= 0x00657267	//"gre" Greek
	, eLangID_Hebrew	= 0x00626568	//"heb" Hebrew
	, eLangID_Hindi		= 0x006E6968	//"hin" Hindi
	, eLangID_Hungarian	= 0x006E7568	//"hun" Hungarian
	, eLangID_Icelandic	= 0x00656369	//"ice" Icelandic
	, eLangID_Italian	= 0x00617469	//"ita" Italian
	, eLangID_Japanese	= 0x006E706A	//"jpn" Japanese
	, eLangID_Korean	= 0x00726F6B	//"kor" Korean
	, eLangID_Latvian	= 0x0076616C	//"lav" Latvian
	, eLangID_Lithuanian	= 0x0074696C	//"lit" Lithuanian
	, eLangID_Letzeburgesh	= 0x007A746C	//"ltz" Letzeburgesh
	, eLangID_Narration	= 0x0072616E	//"nar" Audio Description
	, eLangID_Norwegian	= 0x00726F6E	//"nor" Norwegian
	, eLangID_Persian	= 0x00726570	//"per" Persian
	, eLangID_Polish	= 0x006C6F70	//"pol" Polish
	, eLangID_Portuguese	= 0x00726F70	//"por" Portuguese
	, eLangID_OriginalAudio	= 0x00616171	//"qaa" Original Language
	, eLangID_Romanian	= 0x006D7572	//"rum" Romanian
	, eLangID_Russian	= 0x00737572	//"rus" Russian
	, eLangID_Croatian	= 0x00726373	//"scr" Croatian
	, eLangID_Slovak	= 0x006F6C73	//"slo" Slovak
	, eLangID_Slovenian	= 0x00766C73	//"slv" Slovenian
	, eLangID_Spanish	= 0x00617073	//"spa" Spanish
	, eLangID_Serbian	= 0x00707273	//"srp" Serbian
	, eLangID_Swedish	= 0x00657773	//"swe" Swedish
	, eLangID_Thai		= 0x00616874	//"tha" Thai
	, eLangID_Turkish	= 0x00727574	//"tur" Turkish
	, eLangID_Ukrainian	= 0x00726B75	//"ukr" Ukrainian
	, eLangID_Undefined	= 0x00646E75	//"und" Undefine
	, eLangID_Vietnamese	= 0x00656976	//"vie" Vietnamese
	, eLangID_Welsh		= 0x006C6577	//"wel" Welsh
	, eLangID_Miscellaneous	= 0x0073696D	//"mis" Miscellaneous languages
	, eLangID_Multiple	= 0x006C756D	//"mul" Multiple languages
	, eLangID_MAX		= 0x00FFFFFF
} HxLANG_Id_e;

HxLANG_Id_e	HLIB_LANG_639ToId (const HCHAR *code639);
const HCHAR *	HLIB_LANG_IdTo639 (HxLANG_Id_e id);
const HCHAR *	HLIB_LANG_IdToFullName (HxLANG_Id_e id);
const HCHAR *	HLIB_LANG_639ToFullName (const HCHAR *code639);
HxLANG_Id_e 	HLIB_LANG_FullNameToId (const HCHAR *fullname);
const HCHAR *	HLIB_LANG_FullNameTo639 (const HCHAR *fullname);

#endif /* __HxLANGUAGE_H__ */
