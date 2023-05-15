
#include <hlib.h>

#include "_hlib_language.h"



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
	,"ltz" //Luxembourgish,	eLangID_Luxembourgish
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
	HxLANG_Id_e		id;
} HxLANG_FullName_t;

STATIC const HxLANG_FullName_t s_langFullName[] =
{
	{"Albanian",	eLangID_Albanian},
	{"Arabic",	eLangID_Arabic},
	{"Australian",	eLangID_Australian},
	{"Basque",	eLangID_Basque},
	{"Basaa",	eLangID_Basaa},
	{"Bulgarian",	eLangID_Bulgarian},
	{"Catalan",	eLangID_Catalan},
	{"Czech",	eLangID_Czech},
	{"Danish",	eLangID_Danish},
	{"Dutch",	eLangID_Dutch},
	{"English",	eLangID_English},
	{"Estonian",	eLangID_Estonian},
	{"Faroese",	eLangID_Faroese},
	{"Finnish",	eLangID_Finnish},
	{"French",	eLangID_French},
	{"German",	eLangID_German},
	{"Scottish Gaelic",	eLangID_Gaelic},
	{"Irish",	eLangID_Irish},
	{"Galician",	eLangID_Galician},
	{"Greek",	eLangID_Greek},
	{"Hebrew",	eLangID_Hebrew},
	{"Hindi",	eLangID_Hindi},
	{"Hungarian",	eLangID_Hungarian},
	{"Icelandic",	eLangID_Icelandic},
	{"Italian",	eLangID_Italian},
	{"Japanese",	eLangID_Japanese},
	{"Korean",	eLangID_Korean},
	{"Latvian",	eLangID_Latvian},
	{"Lithuanian",	eLangID_Lithuanian},
	{"Letzeburgesh",	eLangID_Letzeburgesh},
	{"Audio Description",	eLangID_Narration},
	{"Norwegian",	eLangID_Norwegian},
	{"Persian",	eLangID_Persian},
	{"Polish",	eLangID_Polish},
	{"Portuguese",	eLangID_Portuguese},
	{"Original Language",	eLangID_OriginalAudio},
	{"Romanian",	eLangID_Romanian},
	{"Russian",	eLangID_Russian},
	{"Croatian",	eLangID_Croatian},
	{"Slovak",	eLangID_Slovak},
	{"Slovenian",	eLangID_Slovenian},
	{"Spanish",	eLangID_Spanish},
	{"Serbian",	eLangID_Serbian},
	{"Swedish",	eLangID_Swedish},
	{"Thai",	eLangID_Thai},
	{"Turkish",	eLangID_Turkish},
	{"Ukrainian",	eLangID_Ukrainian},
	{"Undefine",	eLangID_Undefined},
	{"Vietnamese",	eLangID_Vietnamese},
	{"Welsh",	eLangID_Welsh},
	{"Miscellaneous languages",	eLangID_Miscellaneous},
	{"Multiple languages",	eLangID_Multiple},
};

STATIC int	hlib_lang_CompareMap (const void *arg1, const void *arg2)
{
	const HCHAR *lhs = *(const HCHAR **)arg1;
	const HCHAR *rhs = *(const HCHAR **)arg2;

	return HLIB_STD_StrCaseCmp(lhs, rhs);
}

STATIC const HCHAR *	hlib_lang_FindCode (const HCHAR *code639)
{
	const HCHAR **ret;

	ret = (const HCHAR **)bsearch(&code639,
							s_639map,
							sizeof(s_639map) / sizeof(s_639map[0]),
							sizeof(HCHAR *),
							hlib_lang_CompareMap
						);
	if (ret)
	{
		return *ret;
	}

	ret = (const HCHAR **)bsearch(&code639,
							s_additional639Map,
							sizeof(s_additional639Map) / sizeof(s_additional639Map[0]),
							sizeof(HCHAR *) * 2,
							hlib_lang_CompareMap
						);
	if (ret)
	{
		return *(ret + 1);
	}
	return NULL;
}

STATIC HUINT32	hlib_lang_ToId (const HCHAR *code)
{
	return (HUINT32)(((code[0] << 0) | (code[1] << 8) | (code[2] << 16)) & 0x00ffffff);
}

STATIC const HCHAR *	hlib_lang_ToCode (HCHAR *buf, HUINT32 id)
{
	HCHAR *code = buf;

	*code++ = (HCHAR)((id >>  0) & 0xFF);
	*code++ = (HCHAR)((id >>  8) & 0xFF);
	*code++ = (HCHAR)((id >> 16) & 0xFF);
	*code++ = '\0';

	return buf;
}


HxLANG_Id_e		HLIB_LANG_639ToId (const HCHAR *code639)
{
	const HCHAR *code;

	HxLOG_Trace();
	HxLOG_Assert(code639);

	code = hlib_lang_FindCode(code639);
	if (code == NULL)
	{
		HxLOG_Message("Cannot find 639 code:%s\n", code639);
		return eLangID_NONE;
	}
	return (HxLANG_Id_e)hlib_lang_ToId(code);
}

const HCHAR *	HLIB_LANG_IdTo639 (HxLANG_Id_e id)
{
	HCHAR	code[4];
	const HCHAR *_639;

	HxLOG_Trace();

	(void)hlib_lang_ToCode(code, (HUINT32)id);

	_639 = hlib_lang_FindCode(code);
	if (_639 == NULL)
	{
		HxLOG_Message("Cannot find 639 code:%s(0x%X:%d)\n", code, id, id);
		return 0;
	}
	return _639;
}

const HCHAR *	HLIB_LANG_639ToFullName (const HCHAR *code639)
{
	return HLIB_LANG_IdToFullName(HLIB_LANG_639ToId(code639));
}

const HCHAR *	HLIB_LANG_IdToFullName (HxLANG_Id_e id)
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

HxLANG_Id_e HLIB_LANG_FullNameToId (const HCHAR *fullname)
{
	HINT32	max = sizeof(s_langFullName) / sizeof(s_langFullName[0]);
	HINT32	i = 0;

	for(i=0; i < max; i++)
	{
		if(fullname[0] == s_langFullName[i].lang[0]
			&& fullname[1] == s_langFullName[i].lang[1])
		{
			if(HLIB_STD_StrCaseCmp(fullname, s_langFullName[i].lang))
			{
				return s_langFullName[i].id;
			}
		}
	}

	return eLangID_NONE;
}

const HCHAR *	HLIB_LANG_FullNameTo639 (const HCHAR *fullname)
{
	return HLIB_LANG_IdTo639(HLIB_LANG_FullNameToId(fullname));
}


HINT32	HLIB_LANG_MakeId (const HCHAR *code639)
{
	return hlib_lang_ToId(code639);
}

