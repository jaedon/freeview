
#include <rxlib.h>
	//#include <htype.h>
#include "rxtype.h"

typedef enum
{
	  eRxLangID_NONE		= 0x00000000

	, eRxLangID_Albanian	= 0x00626C61	// "alb" Albanian
	, eRxLangID_Arabic	= 0x00617261	//"ara" Arabic
	, eRxLangID_Australian	= 0x00737561	//"aus" Australian
	, eRxLangID_Basque	= 0x00716162	//"baq" Basque
	, eRxLangID_Basaa		= 0x00736162	//"bas" Basaa
	, eRxLangID_Bulgarian	= 0x006C7562	//"bul" Bulgarian
	, eRxLangID_Catalan	= 0x00746163	//"cat" Catalan
	, eRxLangID_Czech		= 0x00657A63	//"cze" Czech
	, eRxLangID_Danish	= 0x006E6164	//"dan" Danish
	, eRxLangID_Dutch		= 0x00747564	//"dut" Dutch
	, eRxLangID_English	= 0x00676E65	//"eng" English
	, eRxLangID_Estonian	= 0x00747365	//"est" Estonian
	, eRxLangID_Faroese	= 0x006F6166	//"fao" Faroese
	, eRxLangID_Finnish	= 0x006E6966	//"fin" Finnish
	, eRxLangID_French	= 0x00657266	//"fre" French
	, eRxLangID_German	= 0x00726567	//"ger" German
	, eRxLangID_Gaelic	= 0x00616C67	//"gla" Scottish Gaelic
	, eRxLangID_Irish		= 0x00656C67	//"gle" Irish
	, eRxLangID_Galician	= 0x00676C67	//"glg" Galician
	, eRxLangID_Greek		= 0x00657267	//"gre" Greek
	, eRxLangID_Hebrew	= 0x00626568	//"heb" Hebrew
	, eRxLangID_Hindi		= 0x006E6968	//"hin" Hindi
	, eRxLangID_Hungarian	= 0x006E7568	//"hun" Hungarian
	, eRxLangID_Icelandic	= 0x00656369	//"ice" Icelandic
	, eRxLangID_Italian	= 0x00617469	//"ita" Italian
	, eRxLangID_Japanese	= 0x006E706A	//"jpn" Japanese
	, eRxLangID_Korean	= 0x00726F6B	//"kor" Korean
	, eRxLangID_Latvian	= 0x0076616C	//"lav" Latvian
	, eRxLangID_Lithuanian	= 0x0074696C	//"lit" Lithuanian
	, eRxLangID_Narration	= 0x0072616E	//"nar" Audio Description
	, eRxLangID_Norwegian	= 0x00726F6E	//"nor" Norwegian
	, eRxLangID_Persian	= 0x00726570	//"per" Persian
	, eRxLangID_Polish	= 0x006C6F70	//"pol" Polish
	, eRxLangID_Portuguese	= 0x00726F70	//"por" Portuguese
	, eRxLangID_OriginalAudio	= 0x00616171	//"qaa" Original Language
	, eRxLangID_Romanian	= 0x006D7572	//"rum" Romanian
	, eRxLangID_Russian	= 0x00737572	//"rus" Russian
	, eRxLangID_Croatian	= 0x00726373	//"scr" Croatian
	, eRxLangID_Slovak	= 0x006F6C73	//"slo" Slovak
	, eRxLangID_Slovenian	= 0x00766C73	//"slv" Slovenian
	, eRxLangID_Spanish	= 0x00617073	//"spa" Spanish
	, eRxLangID_Serbian	= 0x00707273	//"srp" Serbian
	, eRxLangID_Swedish	= 0x00657773	//"swe" Swedish
	, eRxLangID_Thai		= 0x00616874	//"tha" Thai
	, eRxLangID_Turkish	= 0x00727574	//"tur" Turkish
	, eRxLangID_Ukrainian	= 0x00726B75	//"ukr" Ukrainian
	, eRxLangID_Undefined	= 0x00646E75	//"und" Undefine
	, eRxLangID_Vietnamese	= 0x00656976	//"vie" Vietnamese
	, eRxLangID_Welsh		= 0x006C6577	//"wel" Welsh
	, eRxLangID_Miscellaneous	= 0x0073696D	//"mis" Miscellaneous languages
	, eRxLangID_Multiple	= 0x006C756D	//"mul" Multiple languages
	, eRxLangID_MAX		= 0x00FFFFFF
} RxLANG_Id_e;

RxLANG_Id_e	RxLANG_639ToId (const HCHAR *code639);
const HCHAR *	RxLANG_IdTo639 (RxLANG_Id_e id);
const HCHAR *	RxLANG_IdToFullName (RxLANG_Id_e id);
const HCHAR *	RxLANG_639ToFullName (const HCHAR *code639);
RxLANG_Id_e 	RxLANG_FullNameToId (const HCHAR *fullname);
const HCHAR *	RxLANG_FullNameTo639 (const HCHAR *fullname);


STATIC const HCHAR *	s_639map[] =
{
	 "alb" //Albanian,	eLangID_Albanian
	,"ara" //Arabic,	eLangID_Arabic
	,"aus" //Australian,	eLangID_Australian
	,"baq" //Basque,	eLangID_Basque
	,"bas" //Basaa,	eLangID_Basaa
	,"bul" //Bulgarian,	eLangID_Bulgarian
	,"cat" //Catalan,	eLangID_Catalan
	,"cze" //Czech,	eLangID_Czech
	,"dan" //Danish,	eLangID_Danish
	,"dut" //Dutch,	eLangID_Dutch
	,"eng" //English,	eLangID_English
	,"est" //Estonian,	eLangID_Estonian
	,"fao" //Faroese,	eLangID_Faroese
	,"fin" //Finnish,	eLangID_Finnish
	,"fre" //French,	eLangID_French
	,"ger" //German,	eLangID_German
	,"gla" //Scottish Gaelic,	eLangID_Gaelic
	,"gle" //Irish,	eLangID_Irish
	,"glg" //Galician,	eLangID_Galician
	,"gre" //Greek,	eLangID_Greek
	,"heb" //Hebrew,	eLangID_Hebrew
	,"hin" //Hindi,	eLangID_Hindi
	,"hun" //Hungarian,	eLangID_Hungarian
	,"ice" //Icelandic,	eLangID_Icelandic
	,"ita" //Italian,	eLangID_Italian
	,"jpn" //Japanese,	eLangID_Japanese
	,"kor" //Korean,	eLangID_Korean
	,"lav" //Latvian,	eLangID_Latvian
	,"lit" //Lithuanian,	eLangID_Lithuanian
	,"mis" //Miscellaneous languages,	eLangID_Miscellaneous
	,"mul" //Multiple languages,	eLangID_Multiple
	,"nar" //Audio Description,	eLangID_Narration
	,"nor" //Norwegian,	eLangID_Norwegian
	,"per" //Persian,	eLangID_Persian
	,"pol" //Polish,	eLangID_Polish
	,"por" //Portuguese,	eLangID_Portuguese
	,"qaa" //Original Language,	eLangID_OriginalAudio
	,"rum" //Romanian,	eLangID_Romanian
	,"rus" //Russian,	eLangID_Russian
	,"scr" //Croatian,	eLangID_Croatian
	,"slo" //Slovak,	eLangID_Slovak
	,"slv" //Slovenian,	eLangID_Slovenian
	,"spa" //Spanish,	eLangID_Spanish
	,"srp" //Serbian,	eLangID_Serbian
	,"swe" //Swedish,	eLangID_Swedish
	,"tha" //Thai,	eLangID_Thai
	,"tur" //Turkish,	eLangID_Turkish
	,"ukr" //Ukrainian,	eLangID_Ukrainian
	,"und" //Undefine,	eLangID_Undefined
	,"vie" //Vietnamese,	eLangID_Vietnamese
	,"wel" //Welsh,	eLangID_Welsh
};

STATIC const HCHAR *	s_additional639Map[][2] =
{
	 {"ces", "cze"} //Czech, eLangID_Czech
	,{"cym", "wel"} //Welsh, eLangID_Welsh
	,{"deu", "ger"} //German, eLangID_German
	,{"ell", "gre"} //Greek, eLangID_Greek
	,{"esl", "spa"} //Spanish, eLangID_Spanish
	,{"esp", "spa"} //Spanish, eLangID_Spanish
	,{"fas", "per"} //Persian, eLangID_Persian
	,{"fra", "fre"} //French, eLangID_French
	,{"gai", "gle"} //Irish, eLangID_Irish
	,{"hrv", "scr"} //Croatian, eLangID_Croatian
	,{"iri", "gle"} //Irish, eLangID_Irish
	,{"isl", "ice"} //Icelandic, eLangID_Icelandic
	,{"nla", "dut"} //Dutch, eLangID_Dutch
	,{"nld", "dut"} //Dutch, eLangID_Dutch
	,{"ron", "rum"} //Romanian, eLangID_Romanian
	,{"scc", "srp"} //Serbian, eLangID_Serbian
	,{"sga", "gle"} //Irish, eLangID_Irish
	,{"slk", "slo"} //Slovak, eLangID_Slovak
	,{"sqi", "alb"} //Albanian, eLangID_Albanian
	,{"sve", "swe"} //Swedish, eLangID_Swedish
};

typedef struct _LanguageFullName
{
	HCHAR 			*lang;
	RxLANG_Id_e		id;
} RxLANG_FullName_t;

STATIC const RxLANG_FullName_t s_langFullName[] =
{
	{"Albanian",	eRxLangID_Albanian},
	{"Arabic",	eRxLangID_Arabic},
	{"Australian",	eRxLangID_Australian},
	{"Basque",	eRxLangID_Basque},
	{"Basaa",	eRxLangID_Basaa},
	{"Bulgarian",	eRxLangID_Bulgarian},
	{"Catalan",	eRxLangID_Catalan},
	{"Czech",	eRxLangID_Czech},
	{"Danish",	eRxLangID_Danish},
	{"Dutch",	eRxLangID_Dutch},
	{"English",	eRxLangID_English},
	{"Estonian",	eRxLangID_Estonian},
	{"Faroese",	eRxLangID_Faroese},
	{"Finnish",	eRxLangID_Finnish},
	{"French",	eRxLangID_French},
	{"German",	eRxLangID_German},
	{"Scottish Gaelic",	eRxLangID_Gaelic},
	{"Irish",	eRxLangID_Irish},
	{"Galician",	eRxLangID_Galician},
	{"Greek",	eRxLangID_Greek},
	{"Hebrew",	eRxLangID_Hebrew},
	{"Hindi",	eRxLangID_Hindi},
	{"Hungarian",	eRxLangID_Hungarian},
	{"Icelandic",	eRxLangID_Icelandic},
	{"Italian",	eRxLangID_Italian},
	{"Japanese",	eRxLangID_Japanese},
	{"Korean",	eRxLangID_Korean},
	{"Latvian",	eRxLangID_Latvian},
	{"Lithuanian",	eRxLangID_Lithuanian},
	{"Audio Description",	eRxLangID_Narration},
	{"Norwegian",	eRxLangID_Norwegian},
	{"Persian",	eRxLangID_Persian},
	{"Polish",	eRxLangID_Polish},
	{"Portuguese",	eRxLangID_Portuguese},
	{"Original Language",	eRxLangID_OriginalAudio},
	{"Romanian",	eRxLangID_Romanian},
	{"Russian",	eRxLangID_Russian},
	{"Croatian",	eRxLangID_Croatian},
	{"Slovak",	eRxLangID_Slovak},
	{"Slovenian",	eRxLangID_Slovenian},
	{"Spanish",	eRxLangID_Spanish},
	{"Serbian",	eRxLangID_Serbian},
	{"Swedish",	eRxLangID_Swedish},
	{"Thai",	eRxLangID_Thai},
	{"Turkish",	eRxLangID_Turkish},
	{"Ukrainian",	eRxLangID_Ukrainian},
	{"Undefine",	eRxLangID_Undefined},
	{"Vietnamese",	eRxLangID_Vietnamese},
	{"Welsh",	eRxLangID_Welsh},
	{"Miscellaneous languages",	eRxLangID_Miscellaneous},
	{"Multiple languages",	eRxLangID_Multiple},
};

STATIC HINT32		_rxlib_lang_StrCaseCmp (const HCHAR *s1, const HCHAR *s2)
{
	HINT32	ch1;
	HINT32	ch2;

	do
	{
		ch1	= toupper(*s1++);
		ch2	= toupper(*s2++);
	} while (ch1 == ch2 && ch1 != 0);

	return ch1 - ch2;
}


STATIC int	rxlib_lang_CompareMap (const void *arg1, const void *arg2)
{
	const HCHAR *lhs = *(const HCHAR **)arg1;
	const HCHAR *rhs = *(const HCHAR **)arg2;

	return _rxlib_lang_StrCaseCmp(lhs, rhs);
}

STATIC const HCHAR *	rxlib_lang_FindCode (const HCHAR *code639)
{
	const HCHAR **ret;

	ret = (const HCHAR **)bsearch(&code639,
							s_639map,
							sizeof(s_639map) / sizeof(s_639map[0]),
							sizeof(HCHAR *),
							rxlib_lang_CompareMap
						);
	if (ret)
	{
		return *ret;
	}

	ret = (const HCHAR **)bsearch(&code639,
							s_additional639Map,
							sizeof(s_additional639Map) / sizeof(s_additional639Map[0]),
							sizeof(HCHAR *) * 2,
							rxlib_lang_CompareMap
						);
	if (ret)
	{
		return *(ret + 1);
	}
	return NULL;
}

STATIC HUINT32	rxlib_lang_ToId (const HCHAR *code)
{
	return (HUINT32)(((code[0] << 0) | (code[1] << 8) | (code[2] << 16)) & 0x00ffffff);
}

STATIC const HCHAR *	rxlib_lang_ToCode (HCHAR *buf, HUINT32 id)
{
	HCHAR *code = buf;

	*code++ = (HCHAR)((id >>  0) & 0xFF);
	*code++ = (HCHAR)((id >>  8) & 0xFF);
	*code++ = (HCHAR)((id >> 16) & 0xFF);
	*code++ = '\0';

	return buf;
}


RxLANG_Id_e		RxLANG_639ToId (const HCHAR *code639)
{
	const HCHAR *code;

	RxLOG_Assert(code639);

	code = rxlib_lang_FindCode(code639);
	if (code == NULL)
	{
		RxLOG_Warning("Cannot find 639 code:%s\n", code639);
		return 0;
	}
	return (RxLANG_Id_e)rxlib_lang_ToId(code);
}

const HCHAR *	RxLANG_IdTo639 (RxLANG_Id_e id)
{
	HCHAR	code[4];
	const HCHAR *_639;


	(void)rxlib_lang_ToCode(code, (HUINT32)id);

	_639 = rxlib_lang_FindCode(code);
	if (_639 == NULL)
	{
		RxLOG_Warning("Cannot find 639 code:%s(0x%X:%d)\n", code, id, id);
		return 0;
	}
	return _639;
}

const HCHAR *	RxLANG_639ToFullName (const HCHAR *code639)
{
	return RxLANG_IdToFullName(RxLANG_639ToId(code639));
}

const HCHAR *	RxLANG_IdToFullName (RxLANG_Id_e id)
{
	HINT32	max = sizeof(s_langFullName) / sizeof(s_langFullName[0]);
	HINT32	i = 0;

	for(i=0; i < max; i++)
	{
		if(id == s_langFullName[i].id)
		{
			return s_langFullName[i].lang;
		}
	}

	return NULL;
}


RxLANG_Id_e RxLANG_FullNameToId (const HCHAR *fullname)
{
	HINT32	max = sizeof(s_langFullName) / sizeof(s_langFullName[0]);
	HINT32	i = 0;

	for(i=0; i < max; i++)
	{
		if(_rxlib_lang_StrCaseCmp(fullname, s_langFullName[i].lang) == 0)
		{
			return s_langFullName[i].id;
		}
	}

	return eRxLangID_NONE;
}

const HCHAR *	RxLANG_FullNameTo639 (const HCHAR *fullname)
{
	return RxLANG_IdTo639(RxLANG_FullNameToId(fullname));
}


HINT32	RxLANG_MakeId (const HCHAR *code639)
{
	return rxlib_lang_ToId(code639);
}


