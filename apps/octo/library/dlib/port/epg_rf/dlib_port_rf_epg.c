
#include <hlib.h>
#include <silib.h>

#include "_dlib_port_rf_epg.h"
#include "_dlib_port_rf_epg_parser.h"

// for compile warning...
#define	DLIB_STATIC

#if defined(CONFIG_MW_EPG_DB_SAVE_FLASH) && !defined(CONFIG_MW_EPG_DB_SAVE_HDD)
#define DLIB_RFEPG_PF_DBFILE_FLASH	 CONFIG_PRODUCT_USERDATA_DIR "/rfepg/eDxEPG_TYPE_RF_PF.db"
#define DLIB_RFEPG_SC_DBFILE_FLASH	 CONFIG_PRODUCT_USERDATA_DIR "/rfepg/eDxEPG_TYPE_RF_SC.db"
#endif

typedef struct
{
	DxField_t	field;
	HBOOL		(* getData) (DxField_t *field, const HUINT8 *src, HUINT32 size);
} DxEPG_Field_t;

typedef struct
{
	HxLANG_Id_e		language;
	HUINT32			count;
	HUINT32			length;
	const HUINT8 *	desc[16];
} DxEventLongText_t;

#define	MACRO_RFEPG_STRLEN(s)				(sizeof(s)/sizeof(s[0])-1)

#if 0
static void	dlib_epg_dump_eit (const HUINT32 *src, HUINT32 size)
{
	static HINT32	count = 0;

	if (count++ < 100)
	{
		FILE 	*fp;
		HCHAR	buf[256];

		HxSTD_PrintToStrN(buf, 256, "/EIT/%03d_%u.eit", count, size);
		fp = fopen(buf, "wb");
		if (fp)
		{
			fwrite(src, 1, size, fp);
			fclose(fp);
		}
	}
}
#endif

static INLINE HBOOL dlib_epg_rf_CheckTag (HUINT8 tag, HINT32 n, ...)
{
	HINT32	t;
	va_list	ap;

	va_start(ap, n);
	while (n--)
	{
		t = va_arg(ap, HINT32);
		if (tag == (HUINT8)t)
		{
			va_end(ap);
			return TRUE;
		}
	}
	va_end(ap);
	return FALSE;
}
#define	DLIB_EPG_CHECK_TAG(tag,...)	dlib_epg_rf_CheckTag(tag, HxMACRO_VA_COUNT(__VA_ARGS__), __VA_ARGS__)

DLIB_STATIC HxLANG_Id_e dlib_epg_rf_GetSystemLanguage (const HUINT8 *src)
{
	HCHAR *delim;
	DxEPG_Header_t	header;

	memcpy(&header, src, sizeof(DxEPG_Header_t));

	delim = (HCHAR *)strchr(header.spec, ':');
	if (delim == NULL)
		return eLangID_NONE;
	*delim = '\0';
	return HLIB_LANG_639ToId(header.spec);
}

DLIB_STATIC DxDeliveryType_e dlib_epg_rf_GetDeliveryType (const HUINT8 *src)
{
	HCHAR *delim;
	DxEPG_Header_t	header;

	memcpy(&header, src, sizeof(DxEPG_Header_t));

	delim = (HCHAR *)strchr(header.spec, ':');
	if (delim)
	{
		delim = (HCHAR *)strchr(delim + 1, ':');
		if (delim)
		{
			delim++;
			if (HLIB_STD_StrCaseStr(delim, "cab"))
				return eDxDELIVERY_TYPE_CAB;
			if (HLIB_STD_StrCaseStr(delim, "ter"))
				return eDxDELIVERY_TYPE_TER;
		}
	}
	return eDxDELIVERY_TYPE_SAT;
}

DLIB_STATIC HINT32	dlib_epg_rf_GetOperatorIndex (const HUINT8 *src, ...)
{
	const HCHAR		*key;
	va_list			ap;
	HCHAR 			*delim, *op;
	DxEPG_Header_t	header;
	HINT32			i;

	memcpy(&header, src, sizeof(DxEPG_Header_t));

	op    = (HCHAR *)strchr(header.spec, ':');
	if (op == NULL)
		return -1;
	delim = (HCHAR *)strchr(++op, ':');
	if (delim == NULL)
		return -1;

	va_start(ap, src);
	for (i = 0 ; (key = va_arg(ap, const HCHAR *)) != NULL ; i++)
	{
		if (HLIB_STD_StrCaseStr(op, key))
		{
			va_end(ap);
			return i;
		}
	}
	va_end(ap);
	return -1;
}

static HINT32	dlib_epg_rf_GetDescs (const HUINT8 *src, HUINT8 tag, const HUINT8 *descs[], HINT32 n, HxLANG_Id_e language)
{
	const HUINT8	*section;
	const HUINT8	*event;
	const HUINT8	*desc;
	HINT32			count;
	HINT32			len;
	HCHAR			iso639[4];
	HxLANG_Id_e	descLang;

	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);
	count = 0;
	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2 && count < n
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] != tag)
			continue;

		if (desc[0] == eSIxDESCTAG_SHORT_EVENT)
			memcpy(iso639, desc + 2, 3);
		else if (desc[0] == eSIxDESCTAG_EXTENDED_EVENT)
			memcpy(iso639, desc + 3, 3);
		else continue;
		iso639[3] = '\0';

		descLang = HLIB_LANG_639ToId(iso639);
		if (language == eLangID_NONE)
			language = descLang;
		if (language == descLang)
			descs[count++] = desc;
	}
	return count;
}

DLIB_STATIC void		dlib_epg_rf_Dump (const HUINT8 *src)
{
	const HUINT8 *section;
	const HUINT8 *event;
	const HUINT8 *desc;
	HINT32		len;
	HCHAR		buf[256], *p;

	static FILE *s_fp;

	if (s_fp == NULL)
		s_fp = fopen("rf_dump.txt", "w");

	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);

	desc    = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event);
	len     = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event);
	for (p = buf ; len > 2 ; )
	{
		HxSTD_PrintToStrN(p, 10, "%02X(%3d) ", desc[0], desc[1]);
		p += 8;
		len  -= (HINT32)(desc[1] + 2);
		desc += (HINT32)(desc[1] + 2);
	}
	*p = '\0';

	{
		#if 0	// dump to file
		static FILE *s_fp;
		if (s_fp == NULL) s_fp = fopen("rf_dump.txt", "w");
		fprintf(s_fp,
		#endif
		HxLOG_Print(
			"dama_customized_event_information_section() {\n"
			"                     table_id: 0x%X\n"
			"               section_length: %d\n"
			"                   service_id: 0x%X\n"
			"                        ts_id: 0x%X\n"
			"                        on_id: 0x%X\n"
			"                      version: %d\n"
			"               section_number: %d\n"
			"          last_section_number: %d\n"
			"  segment_last_section_number: %d\n"
			"  for (i = 0 ; i < 1 ; i++) {\n"
			"                       event_id: 0x%X\n"
			"                     start_time: %s\n"
			"                       duration: %d(sec)\n"
			"                 running_status: %d\n"
			"                   free_CA_mode: %d\n"
			"        descriptors_loop_length: %d\n"
			"    for (i = 0 ; i < N ; i++) {\n"
			"      descriptor(%s)\n"
			"    }\n"
			"  }"
			"                    CRC(type): 0x%X(%d)\n"
			"}\n\n"
			, DLIB_EPG_RF_PARSER_SECTION_TABLE_ID(section)
			, DLIB_EPG_RF_PARSER_SECTION_LENGTH(section)
			, DLIB_EPG_RF_PARSER_SECTION_SERVICE_ID(section)
			, DLIB_EPG_RF_PARSER_SECTION_TS_ID(section)
			, DLIB_EPG_RF_PARSER_SECTION_ON_ID(section)
			, DLIB_EPG_RF_PARSER_SECTION_VERSION(section)
			, DLIB_EPG_RF_PARSER_SECTION_SECTION_NO(section)
			, DLIB_EPG_RF_PARSER_SECTION_LAST_SECTION_NO(section)
			, DLIB_EPG_RF_PARSER_SECTION_SEGMENT_LAST_SECTION_NO(section)
				, DLIB_EPG_RF_PARSER_EVENT_EVENT_ID(event)
				, HLIB_DATETIME_UnixTimeToString(DLIB_EPG_RF_PARSER_EVENT_StartTime(event), NULL, 0)
				, DLIB_EPG_RF_PARSER_EVENT_Duration(event)
				, DLIB_EPG_RF_PARSER_EVENT_RUNNING_STATUS(event)
				, DLIB_EPG_RF_PARSER_EVENT_FREE_CA_MODE(event)
				, DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event)
				, buf
			, DLIB_EPG_RF_PARSER_SECTION_CRC32(section)
			, DLIB_EPG_RF_PARSER_SECTION_CRC32(section)
		);
	}
}

static void dlib_epg_rf_DeleteEventString (DxEventString_t *string)
{
	if (string)
	{
		if (string->string)
			DLIB_Free(string->string);
		DLIB_Free(string);
	}
}


static void dlib_epg_rf_DeleteComponent (DxEventComponent_t *comp)
{
	if (comp)
	{
		if (comp->text)
			DLIB_Free(comp->text);
		DLIB_Free(comp);
	}
}

static void dlib_epg_rf_DeleteEventItem (DxEventItem_t *item)
{
	if (item)
	{
		if (item->description)
			DLIB_Free(item->description);
		if (item->item)
			DLIB_Free(item->item);
		DLIB_Free(item);
	}
}

static void	dlib_epg_rf_DeleteCRID (DxEventCRID_t *item)
{
	if (item)
	{
		if (item->crid)
			DLIB_Free(item->crid);
#if defined(CONFIG_OP_TDC)
		if (item->title)
			DLIB_Free(item->title);
#endif
		DLIB_Free(item);
	}
}

static void	dlib_epg_rf_DeleteLinkageInfo (DxEventLinkage_t *item)
{
	if (item)
	{
		// TODO:
		DLIB_Free(item);
	}
}

static DxEventString_t *dlib_epg_rf_CloneString (DxEventString_t *string)
{
	DxEventString_t	*clone;
	if (string == NULL)
		return NULL;

	clone = (DxEventString_t *)DLIB_MemDup(string, sizeof(DxEventString_t));
	if (clone)
	{
		if(string->string) clone->string = DLIB_StrDup(string->string);
	}
	return clone;
}

static DxEventItem_t *dlib_epg_rf_CloneEventItem (DxEventItem_t *item)
{
	DxEventItem_t	*clone;
	if (item == NULL)
		return NULL;

	clone = (DxEventItem_t *)DLIB_MemDup(item, sizeof(DxEventItem_t));
	if (clone)
	{
		clone->item = DLIB_StrDup(item->item);
		clone->description = DLIB_StrDup(item->description);
	}
	return clone;
}

static DxEventComponent_t *dlib_epg_rf_CloneComponent (DxEventComponent_t *item)
{
	DxEventComponent_t *clone;

	if (item == NULL)
		return NULL;

	clone = (DxEventComponent_t *)DLIB_MemDup(item, sizeof(DxEventComponent_t));
	if (clone)
	{
		clone->text = DLIB_StrDup(item->text);
	}
	return clone;
}

static DxEventCRID_t *dlib_epg_rf_CloneCRID (DxEventCRID_t *item)
{
	DxEventCRID_t *clone;

	if (item == NULL)
		return NULL;

	clone =  (DxEventCRID_t *)DLIB_MemDup(item, sizeof(DxEventCRID_t));
	if (clone)
	{
		clone->type = item->type;
		clone->crid = DLIB_StrDup(item->crid);
#if defined(CONFIG_OP_TDC)
		clone->title = DLIB_StrDup(item->title);
#endif
	}
	return clone;
}

static DxEventLinkage_t *dlib_epg_rf_CloneLinkageInfo(DxEventLinkage_t *item)
{
	DxEventLinkage_t *clone;

	if (item == NULL)
		return NULL;

	clone = (DxEventLinkage_t *)DLIB_MemDup(item, sizeof(DxEventLinkage_t));
	if (clone)
	{
		clone->privateData = NULL;
		HxLOG_Critical("%s:%d NOT IMPLEMENTED YET!!!\n", __FUNCTION__, __LINE__);
	}
	return clone;
}

static void *	dlib_epg_rf_CloneDescriptor (void *data)
{
	if (data)
	{
		HUINT8 *desc = (HUINT8 *)data;
		return HLIB_STD_MemDupEx(data, (HUINT32)(2 + desc[1]), DLIB_Malloc_CB);
	}
	return NULL;
}

static HxVector_t *	dlib_epg_rf_GetComponent (const HUINT8 *src, HUINT32 size)
{
	const HUINT8	*section;
	const HUINT8	*event;
	const HUINT8	*desc;
	HINT32			n;
	HINT32			len;
	SIxTextEncoding_e 	textenc;
	HCHAR			iso639[4];
	DxEventComponent_t	temp;
	DxEventComponent_t	*comp;
	HxVector_t		*list;

	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event	= DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);
	n = 0;
	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] == eSIxDESCTAG_COMPONENT)	// component descriptor
			n++;
	}
	if (n == 0)
		return NULL;

	list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteComponent, NULL);
	if (list == NULL)
		return NULL;

	switch (dlib_epg_rf_GetOperatorIndex(src, "freesat", "jcom","bbc", NULL))
	{
	case 0:/*freesat*/textenc = eSIxTEXT_ENC_300468_UK;  break;
	case 1:/*jcom*/   textenc = eSIxTEXT_ENC_JIS;       break;
	case 2:/*bbc*/   textenc = eSIxTEXT_ENC_300468_UK;  break;
	default: textenc = eSIxTEXT_ENC_300468; break;
	}

	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] != eSIxDESCTAG_COMPONENT)	// component descriptor
			continue;

		temp.content = desc[2] & 0x0F;
		temp.type    = desc[3];
		temp.tag     = desc[4];
		memcpy(iso639, desc + 5, 3); iso639[3] = '\0';
		temp.language = HLIB_LANG_639ToId(iso639);
		if (desc[1] > 6)
		{
			HCHAR	*tmpStr = SILIB_TEXT_MakeUtf8(textenc, iso639, desc + 8, desc[1] - 6);
			temp.text = DLIB_StrDup(tmpStr);
			SILIB_TEXT_Free(tmpStr);
		}
		else temp.text = NULL;

		comp = (DxEventComponent_t *)DLIB_MemDup(&temp, sizeof(DxEventComponent_t));
		if (comp)
			HLIB_VECTOR_Add(list, comp);
		else if (temp.text)
			DLIB_Free(temp.text);
	}

	if (HLIB_VECTOR_Length(list) == 0)
	{
		HLIB_VECTOR_Delete(list);
		return NULL;
	}
	return list;
}

static HxVector_t *	dlib_epg_rf_GetItemAndDescription (const HUINT8 *src, HUINT32 size)
{
	const HUINT8 	*desc;
	const HUINT8 	*extd[16];
	HINT32			i, n, len;
	HxLANG_Id_e	systemLang;
	SIxTextEncoding_e 	textenc;
	HCHAR			iso639[4];
	HCHAR			*text, *description;
	DxEventItem_t	*item;
	HxVector_t		*list;

	systemLang = dlib_epg_rf_GetSystemLanguage(src);
	switch (dlib_epg_rf_GetOperatorIndex(src, "freesat", "jcom","bbc", NULL))
	{
	case 0:/*freesat*/textenc = eSIxTEXT_ENC_300468_UK; break;
	case 1:/*jcom*/   textenc = eSIxTEXT_ENC_JIS;       break;
	case 2:/*bbc*/   textenc = eSIxTEXT_ENC_300468_UK;       break;
	default: textenc = eSIxTEXT_ENC_300468; break;
	}

	len = dlib_epg_rf_GetDescs(src, eSIxDESCTAG_EXTENDED_EVENT, extd, 16, systemLang);
	if (len == 0 && systemLang != eLangID_English)
		len = dlib_epg_rf_GetDescs(src, eSIxDESCTAG_EXTENDED_EVENT, extd, 16, eLangID_English);
	if (len == 0)
		len = dlib_epg_rf_GetDescs(src, eSIxDESCTAG_EXTENDED_EVENT, extd, 16, eLangID_NONE);

	if (len == 0)
		return NULL;

	// has description & item ??
	for (i = 0, n = 0 ; i < len ; i++)
		n += (HINT32)extd[i][6];
	if (n == 0)
		return NULL;

	memcpy(iso639, extd[0] + 3, 3); iso639[3] = '\0';

	list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteEventItem, NULL);
	if (list == NULL)
		return NULL;

	for (i = 0 ; i < len ; i++)
	{
		desc = extd[i];
		n    = (HINT32)desc[6];
		desc += 7;
		while (n > 0)
		{
			if (*desc > 0)
			{
				HCHAR	*tmpStr = SILIB_TEXT_MakeUtf8(textenc, iso639, desc + 1, (HUINT32)*desc);
				description = DLIB_StrDup(tmpStr);
				SILIB_TEXT_Free(tmpStr);
			}
			else description = NULL;
			n    -= (*desc + 1);
			desc += (*desc + 1);

			if (*desc > 0)
			{
				HCHAR	*tmpStr = SILIB_TEXT_MakeUtf8(textenc, iso639, desc + 1, (HUINT32)*desc);
				text = DLIB_StrDup(tmpStr);
				SILIB_TEXT_Free(tmpStr);
			}
			else text = NULL;
			n    -= (*desc + 1);
			desc += (*desc + 1);

			if (description || text)
			{
				item = (DxEventItem_t *)DLIB_Malloc(sizeof(DxEventItem_t));
				if (item)
				{
					item->description = description;
					item->item = text;
					HLIB_VECTOR_Add(list, item);
				}
				else
				{
					if (description)
						DLIB_Free(description);
					if (text)
						DLIB_Free(text);
				}
			}
		}
	}
	if (HLIB_VECTOR_Length(list) == 0)
	{
		HLIB_VECTOR_Delete(list);
		return NULL;
	}
	return list;
}




#define KEYWORD_CRID_PROGRAM 		"pCRID:"
#define KEYWORD_CRID_SERIES 		"sCRID:"
#define KEYWORD_CRID_RECOMMENDATION	"rCRID:"
#define KEYWORD_CRID_DLIMITER		","


static HCHAR * dlib_epg_rf_GetGroupCRIDs( HxVector_t * pCRIDList)
{
	HUINT32 		uCRIDLen,i;
	HCHAR			szKeyword[1024] = {0,};
	HCHAR			*pRet;
	DxEventCRID_t	*pCRID 			= NULL;

	if(pCRIDList == NULL)
		return NULL;

	uCRIDLen = HLIB_VECTOR_Length(pCRIDList);
	if(uCRIDLen <= 0)
		return NULL;

	for(i=0; i< uCRIDLen; i++)
	{
		pCRID = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(pCRIDList, i);

		if(i>0)
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_DLIMITER,MACRO_RFEPG_STRLEN(KEYWORD_CRID_DLIMITER));

		if(pCRID->type ==DxCRID_TYPE_SERIES)
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_SERIES,MACRO_RFEPG_STRLEN(KEYWORD_CRID_SERIES));
		else if(pCRID->type ==DxCRID_TYPE_PROG)
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_PROGRAM,MACRO_RFEPG_STRLEN(KEYWORD_CRID_PROGRAM));
		else
			continue;

		HxSTD_StrNCat(szKeyword ,pCRID->crid,HLIB_STD_StrNLen(pCRID->crid,DxCRID_LEN));
		HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_DLIMITER,MACRO_RFEPG_STRLEN(KEYWORD_CRID_DLIMITER));
	}

	HxLOG_Debug("[%s][%d] CRID Keyword: %s\n", __FUNCTION__,__LINE__,szKeyword);

	pRet = DLIB_StrDup(szKeyword);
	return pRet;
}

static DxEventCRID_t *dlib_epg_rf_MakeCRID(const HUINT8 *pCridPos, HUINT32 cridLength, HUINT8 cridType)
{
	DxEventCRID_t	*pstCrid = NULL;

	if(pCridPos == NULL)
	{
		HxLOG_Error("pCridPos is NULL..\n");
		return NULL;
	}

	if(cridLength > (DxRSV_CRID_DATA_LEN + DxRSV_CRID_IMI_LEN))
	{
		cridLength = (DxRSV_CRID_DATA_LEN + DxRSV_CRID_IMI_LEN);
	}

	pstCrid = (DxEventCRID_t *)DLIB_Calloc(sizeof(DxEventCRID_t));
	if (pstCrid)
	{
		pstCrid->type = cridType;
		pstCrid->crid = (HCHAR *)DLIB_Calloc(cridLength + 1);
		if(pstCrid->crid)
		{
			memcpy(pstCrid->crid, pCridPos, cridLength);
			pstCrid->crid[cridLength] = 0;
			HxLOG_Debug("CRID (%s): (%s) found!!\n", cridType==DxCRID_TYPE_PROG?"Program":(cridType==DxCRID_TYPE_SERIES?"Series":"Recommend"), pstCrid->crid);
		}
		else
		{
			HxLOG_Error("Fail to DLIB_Calloc()\n");
			DLIB_Free(pstCrid);
			pstCrid = NULL;
		}
	}
	else
	{
		HxLOG_Error("Fail to DLIB_Calloc()\n");
	}

	return pstCrid;

}

#if defined(CONFIG_OP_TDC)
#define DxRSV_TDC_CRID_DELIMITER		'/'
#define DxRSV_TDC_CRID_DELIMITER_LEN 	1
#define DxRSV_TDC_CRID_FLAG_LEN 		1
#define DxRSV_TDC_CRID_DATA_LEN 		8
#define DxRSV_TDC_CRID_TITLE_LEN 		54
#define DxRSV_TDC_CRID_MIN_LEN 			(DxRSV_TDC_CRID_DELIMITER_LEN + DxRSV_TDC_CRID_FLAG_LEN + DxRSV_TDC_CRID_DATA_LEN)
#define DxRSV_TDC_CRID_MAX_LEN 			(DxRSV_TDC_CRID_MIN_LEN + DxRSV_TDC_CRID_TITLE_LEN)

//</><1><8><0~54>	: max 64 bytes
//<'/'>
//<1 byte flag>
//<8 byte Series ID as HEX values presented in ASCII format>
//< 0 to 54 bytes Series Title in ASCII format>
static DxEventCRID_t *dlib_epg_rf_MakeTdcCRID(const HUINT8 *pCridPos, HUINT32 cridLength, HUINT8 cridType)
{
	DxEventCRID_t	*pstCrid = NULL;

	if(pCridPos == NULL)
	{
		HxLOG_Error("pCridPos is NULL..\n");
		return NULL;
	}

	if(cridType != DxCRID_TYPE_SERIES)
	{
		HxLOG_Print("Receive another CRID. Type( %s )\n", DxNAMEOF(cridType));
		return NULL;
	}

	//////////////////////////////
	// Only Series CRID below
	//////////////////////////////

	// <'/'><1 byte flag><8 byte Series ID>  <== minimum 10 bytes
	if(cridLength < DxRSV_TDC_CRID_MIN_LEN)
	{
		HxLOG_Debug("Too Shot CRID Length( %d ). TDC CRID minimum lengthis 10\n", cridLength);
		return NULL;
	}

	if(cridLength > DxRSV_TDC_CRID_MAX_LEN)
	{
		cridLength = DxRSV_TDC_CRID_MAX_LEN;
	}

	// 1byte : delimiter '/'
	if(pCridPos[0] != '/')
	{
		HxLOG_Error("Invalid CRID format. first byte( %c )\n", pCridPos[0]);
		return NULL;
	}

	pstCrid = (DxEventCRID_t *)DLIB_Calloc(sizeof(DxEventCRID_t));
	if(pstCrid == NULL)
	{
		HxLOG_Error("Fail to DLIB_Calloc()\n");
		return NULL;
	}
	pstCrid->type = cridType;

	// 1byte : search flag
	pstCrid->search_flag = pCridPos[1];

	// 8byte : seriesCRID
	// '/' + CRID + '\0' = 10 bytes
	// Base 구현 사항과 맞추기 위하여 반드시 첫번째 문자는 '/' 이 되도록 함.
	pstCrid->crid = (HCHAR *)DLIB_Calloc(DxRSV_TDC_CRID_DELIMITER_LEN+DxRSV_TDC_CRID_DATA_LEN+1);
	if(pstCrid->crid == NULL)
	{
		DLIB_Free(pstCrid);
		HxLOG_Error("Fail to DLIB_Calloc()\n");
		return NULL;
	}

	// make CRID : '/' + 8byte CRID + '\0'
	pstCrid->crid[0] = DxRSV_TDC_CRID_DELIMITER;
	memcpy(pstCrid->crid+DxRSV_TDC_CRID_DELIMITER_LEN, pCridPos+2, DxRSV_TDC_CRID_DATA_LEN);
	pstCrid->crid[DxRSV_TDC_CRID_DATA_LEN+1] = 0;

	// Length - 10byte : series title
	if(cridLength > DxRSV_TDC_CRID_MIN_LEN)
	{
		HCHAR *title = NULL;
		title = SILIB_TEXT_MakeUtf8(eSIxTEXT_ENC_DEFAULT, NULL, pCridPos+DxRSV_TDC_CRID_MIN_LEN, cridLength-DxRSV_TDC_CRID_MIN_LEN);
		if(title != NULL)
		{
			pstCrid->title = DLIB_StrDup(title);
			SILIB_TEXT_Free(title);
		}
	}
	HxLOG_Debug("CRID(%s), search_flag(%c), title(%s) found!!\n", pstCrid->crid, pstCrid->search_flag, pstCrid->title);

	return pstCrid;
}
#endif

static HxVector_t *	dlib_epg_rf_GetCRID (const HUINT8 *src, HUINT32 size)
{
	const HUINT8	*section;
	const HUINT8	*event;
	const HUINT8	*desc;
	HINT32			n;
	HINT32			len;
	HUINT32			descriptorLength;
	HUINT32			pos;
	HUINT8			cridType;
	HUINT32			cridLength;
	DxEventCRID_t	*crid;
	HxVector_t		*list;
#if defined (CONFIG_OP_UK_DTT)
	HBOOL			bFoundPrivate = FALSE;
#endif
	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event	= DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);
	n = 0;
	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] == eSIxDESCTAG_CONTENT_IDENTIFIER)	// content_identifier_descriptor
			n++;
	}
	if (n == 0)
		return NULL;

	list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteCRID, NULL);
	if (list == NULL)
		return NULL;

	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {

#if defined(CONFIG_OP_UK_DTT)
		if (desc[0] == eSIxDESCTAG_PRIVATE_DATA_SPECIFIER)
		{
			HUINT32	privateDataLength;
			HUINT32	ulPrivateData;

			privateDataLength = desc[1];
			if (privateDataLength != 4)
			{
				HxLOG_Debug("Unknown parsePrivateDataSpecDes Format!\n");
				continue;
			}
			ulPrivateData = HxMACRO_Get32Bit(desc+2);

			if (ulPrivateData == 0x0000233a) //PRIVATE_DATA_SPECIFIER_FOR_UK_DTT
			{
				bFoundPrivate = TRUE;
			}
			else
			{
				bFoundPrivate = FALSE;
			}
		}
		if (bFoundPrivate != TRUE)
		{
			continue;
		}
#endif

		if (desc[0] != eSIxDESCTAG_CONTENT_IDENTIFIER)	// content_identifier_descriptor
		{
			continue;
		}

		descriptorLength = desc[1];
		for (pos=0; pos<descriptorLength;pos += 1 + 1 +desc[pos+3]) // crid_type,location 1 byte + length 1 byte + crid length(pos+3) n byte
		{
			cridType = desc[pos+2] >> 2;
			switch (cridType)
			{
			case DxCRID_TYPE_PROG: //정상 녹화된 녹화물의 재 녹화 방지 위해 사용
			case DxCRID_TYPE_SERIES:
			case DxCRID_TYPE_RECOMMEND:
				// 호주향에서는 The parameter crid_location = '00'.
				if ((desc[pos+2] & 0x3) == 0)
				{
					cridLength = desc[pos+3];

					if (cridLength > 0)
					{
#if defined(CONFIG_OP_TDC)
						crid = dlib_epg_rf_MakeTdcCRID(desc+pos+4, cridLength, cridType);
#else // base code
						crid = dlib_epg_rf_MakeCRID(desc+pos+4, cridLength, cridType);
#endif

						if(crid)
						{
							HLIB_VECTOR_Add(list, crid);
						}
					}
				}
				break;
			}
		}
	}

	if (HLIB_VECTOR_Length(list) == 0)
	{
		HLIB_VECTOR_Delete(list);
		return NULL;
	}
	return list;
}

static HBOOL	dlib_epg_rf_GetName (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section;
	const HUINT8 *event;
	const HUINT8 *desc;
	const HUINT8 *descEng = NULL, *desc1st = NULL;
	HINT32	len;
	HCHAR	iso639[4];
	HxLANG_Id_e	systemLang;
	HxLANG_Id_e	descLang;
	SIxTextEncoding_e 	textenc;
	HCHAR *name;

	systemLang = dlib_epg_rf_GetSystemLanguage(src);
	switch (dlib_epg_rf_GetOperatorIndex(src, "freesat", "jcom","bbc", NULL))
	{
	case 0:/*freesat*/textenc = eSIxTEXT_ENC_300468_UK; break;
	case 1:/*jcom*/  textenc = eSIxTEXT_ENC_JIS;       break;
	case 2:/*bbc*/   textenc = eSIxTEXT_ENC_300468_UK; break;
	default: textenc = eSIxTEXT_ENC_300468; break;
	}
#if 0
{
	DxEPG_Header_t	header;

	memcpy(&header, src, sizeof(DxEPG_Header_t));
	if (header.type == eDxEPG_TYPE_PF)
		dlib_epg_dump_eit(src, size);
}
#endif
	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);

	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] == eSIxDESCTAG_SHORT_EVENT) // short_event_descriptor
		{
			if (desc1st == NULL)
				desc1st = desc;

			memcpy(iso639, desc + 2, 3); iso639[3] = '\0';
			descLang = HLIB_LANG_639ToId(iso639);
			if (descLang == eLangID_English)
				descEng = desc;
			if (descLang == systemLang)
				break;
		}
	}
	if (len <= 2)	// desc not founds
		desc = descEng ? descEng : desc1st;
	if (desc == NULL)
		return FALSE;

	memcpy(iso639, desc + 2, 3); iso639[3] = '\0';
	name = SILIB_TEXT_MakeUtf8(textenc, iso639, desc + 6, desc[5]);
	if (name == NULL)
		return FALSE;

	field->u.string = DLIB_StrDup(name);
	SILIB_TEXT_Free(name);
	return TRUE;
}

static HxVector_t * dlib_epg_rf_GetNameAll (const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section;
	const HUINT8 *event;
	const HUINT8 *desc;
	HINT32	len;
	SIxTextEncoding_e 	textenc;
	HxVector_t		*name = NULL;


	switch (dlib_epg_rf_GetOperatorIndex(src, "freesat", "jcom","bbc", NULL))
	{
		case 0:/*freesat*/textenc = eSIxTEXT_ENC_300468_UK; break;
		case 1:/*jcom*/  textenc = eSIxTEXT_ENC_JIS;       break;
		case 2:/*bbc*/   textenc = eSIxTEXT_ENC_300468_UK; break;
		default: textenc = eSIxTEXT_ENC_300468; break;
	}

	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);

	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] == eSIxDESCTAG_SHORT_EVENT) // short_event_descriptor
		{
			if (name == NULL)
			{
				name = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteEventString, NULL);
			}

			if (name != NULL)
			{
				HCHAR	iso639[4];
				HCHAR 	*text = NULL;
				DxEventString_t	* eventString = DLIB_Calloc(sizeof(DxEventString_t));

				memcpy(iso639, desc + 2, 3); iso639[3] = '\0';
				eventString->language = HLIB_LANG_639ToId(iso639);
				memcpy(iso639, desc + 2, 3); iso639[3] = '\0';

				text = SILIB_TEXT_MakeUtf8(textenc, iso639, desc + 6, desc[5]);
				if(text)
				{
					eventString->string = DLIB_StrDup(text);
					SILIB_TEXT_Free(text);
				}

				HLIB_VECTOR_Add(name, eventString);
			}
		}
	}

	return name;
}

static HxVector_t * dlib_epg_rf_GetText (const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section;
	const HUINT8 *event;
	const HUINT8 *desc;
	HINT32	len;
	HCHAR	iso639[4];
	SIxTextEncoding_e 	textenc;
	HxVector_t		*shortText = NULL;

	switch (dlib_epg_rf_GetOperatorIndex(src, "freesat", "jcom","bbc", NULL))
	{
		case 0:/*freesat*/textenc = eSIxTEXT_ENC_300468_UK; break;
		case 1:/*jcom*/   textenc = eSIxTEXT_ENC_JIS;       break;
		case 2:/*bbc*/   textenc = eSIxTEXT_ENC_300468_UK;       break;
		default: textenc = eSIxTEXT_ENC_300468; break;
	}

	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);
	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] == eSIxDESCTAG_SHORT_EVENT) // short_event_descriptor
		{
			if (shortText == NULL)
			{
				shortText = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteEventString, NULL);
			}

			if (shortText != NULL)
			{
				HCHAR *text = NULL;
				DxEventString_t	* eventShortString = DLIB_Calloc(sizeof(DxEventString_t));
				memcpy(iso639, desc + 2, 3); iso639[3] = '\0';

				text = SILIB_TEXT_MakeUtf8(textenc, iso639, desc + 6 + desc[5] + 1, desc[6 + desc[5]]);

				if(text)
				{
					eventShortString->language = HLIB_LANG_639ToId(iso639);
					eventShortString->string = DLIB_StrDup(text);
					SILIB_TEXT_Free(text);
				}

				HLIB_VECTOR_Add(shortText, eventShortString);
			}
		}
	}
	return shortText;
}

static HxVector_t * dlib_epg_rf_GetLongText (const HUINT8 *src, HUINT32 size)
{
	const HUINT8 		*desc;
	const HUINT8			*section;
	const HUINT8			*event;
	HINT32				i, len;
	SIxTextEncoding_e 	textenc;
	DxEventLongText_t	descAddr[16];
	HxVector_t			*longText = NULL;

	HxSTD_memset(descAddr, 0, sizeof(DxEventLongText_t)*16);

	switch (dlib_epg_rf_GetOperatorIndex(src, "freesat", "jcom","bbc", NULL))
	{
		case 0:textenc = eSIxTEXT_ENC_300468_UK; 	break;	/*freesat*/
		case 1:textenc = eSIxTEXT_ENC_JIS;       	break;	/*jcom*/
		case 2:textenc = eSIxTEXT_ENC_300468_UK;	break;	/*bbc*/
		default: textenc = eSIxTEXT_ENC_300468;	break;
	}

	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event	= DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);
	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] == eSIxDESCTAG_EXTENDED_EVENT)
		{
			HCHAR		iso639[4];
			HxLANG_Id_e descLang;

			memcpy(iso639, desc + 3, 3);
			iso639[3] = '\0';
			descLang = HLIB_LANG_639ToId(iso639);

			for(i=0 ; i< 16 ; i++)
			{
				HxLOG_Print("[%s:%d] descAddr[i].language:%d\n",__FUNCTION__,__LINE__,descAddr[i].language);
				if(descAddr[i].language == descLang	|| descAddr[i].language == eLangID_NONE)
				{
					descAddr[i].language = descLang;
					if(descAddr[i].count<16)
					{
						descAddr[i].desc[descAddr[i].count++] = desc;
						descAddr[i].length += desc[7 + desc[6]];
					}
					HxLOG_Print("[%s:%d] descAddr[i].count:%d\n",__FUNCTION__,__LINE__,descAddr[i].count);
					break;
				}
			}
		}
	}

	for(i = 0; i < 16 && descAddr[i].language != eLangID_NONE; i++)
	{
		HCHAR	buf[4096];
		HINT32	n, j;
		HCHAR	*text = NULL;

		HxLOG_Print("[%s:%d] i :%d , descAddr[i].count:%d, descAddr[i].length:%d  descAddr[i].language :%s\n",__FUNCTION__,__LINE__,i,descAddr[i].count,descAddr[i].length,HLIB_LANG_IdTo639(descAddr[i].language) );

		if(descAddr[i].count == 0 || descAddr[i].length == 0)
			continue;

		for (j = 0, n = 0 ; j < descAddr[i].count ; j++)
		{
			HxLOG_Print("[%s:%d] descAddr[i].desc[j]:%d  , *desc:%d\n",__FUNCTION__,__LINE__,descAddr[i].desc[j],*desc);
			desc  = descAddr[i].desc[j];
			desc += 7 + desc[6];
			if (*desc)
			{
				memcpy(buf + n, desc + 1, *desc);
			}
			n += *desc;
		}
		HxLOG_Print("buf (%d): %s, \n",n,buf);
		text = SILIB_TEXT_MakeUtf8(textenc, HLIB_LANG_IdTo639(descAddr[i].language), buf, n);
		if(text)
		{
			DxEventString_t	* eventLongString = DLIB_Calloc(sizeof(DxEventString_t));
			HxLOG_Print("[%s:%d] Long Text:%s\n",__FUNCTION__,__LINE__,text);

			if(eventLongString)
			{
				eventLongString->string = DLIB_StrDup(text);
				eventLongString->language = descAddr[i].language;

				if(longText == NULL)
				{
					longText = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteEventString, NULL);
				}				
				if(longText != NULL)
				{
					HLIB_VECTOR_Add(longText, eventLongString);
				}
				else
				{
					DLIB_Free(eventLongString);
				}
			}
			SILIB_TEXT_Free(text);
		}
		else
		{
			HxLOG_Print("[%s:%d] Not Found Long Text\n",__FUNCTION__,__LINE__);
		}
	}

	return longText;
}

static HBOOL	dlib_epg_rf_GetParentalRating (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 	*section;
	const HUINT8 	*event;
	const HUINT8 	*desc;
	HINT32			len, n, i;
	HCHAR			code[4];

	section = DLIB_EPG_RF_PARSER_SECTION(src);
	event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);
#ifdef	TEST_RATING
	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] != eSIxDESCTAG_PARENTAL_RATING)	// parental_rating_descriptor
			continue;

		n = (HINT32)(desc[1] / 4);
		if ((HINT32)desc[1] % 4)
			n--;
		code[3] = '\0';
		for (i = 0, desc += 2 ; i < n ; i++, desc += 4)
		{
			code[0] = desc[0];
			code[1] = desc[1];
			code[2] = desc[2];
			Hx_Print("%s("HxANSI_COLOR_YELLOW("%s:%d")")\n", __FUNCTION__, code, desc[3]);
		}
	}
#endif

	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		if (desc[0] != eSIxDESCTAG_PARENTAL_RATING)	// parental_rating_descriptor
			continue;
		n = (HINT32)(desc[1] / 4);
		if ((HINT32)desc[1] % 4)
			n--;
		code[3] = '\0';
		
		desc += 2; // tag, length 
			
		for(i = 0 ; i < n ; i++)
		{
			code[0] = desc[0];
			code[1] = desc[1];
			code[2] = desc[2];

			// TODO: check country => Content Descriptor 처럼 모든 Parental 정보를 Array로 관리하는 것이 좋을 것 같음. 추후에 수정이 필요함.

			//En300468 Spec  Table 68: Parental rating descriptor, rating 참조.
			//minimum age = rating + 3 years.
			field->u.integer = (HINT32)desc[3];

			// 현재는 하나만 parsing하기 때문에 필요없는 코드이나, Array로 관리할  때 필요한 내용임..
			desc += 4;
			return TRUE;
		}
	}
	(void)code;
	return FALSE;
}



static HBOOL	dlib_epg_rf_GetStartTime (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section = DLIB_EPG_RF_PARSER_SECTION(src);
	const HUINT8 *event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);

	field->u.integer = (HINT32)DLIB_EPG_RF_PARSER_EVENT_StartTime(event);

	(void)size;
	return TRUE;
}


static HBOOL	dlib_epg_rf_GetEndTime (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section = DLIB_EPG_RF_PARSER_SECTION(src);
	const HUINT8 *event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);

	field->u.integer = (HINT32)(DLIB_EPG_RF_PARSER_EVENT_StartTime(event) + DLIB_EPG_RF_PARSER_EVENT_Duration(event) - 1);

	(void)size;
	return TRUE;
}

static HBOOL	dlib_epg_rf_GetSvcUID (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	DxEPG_Header_t	header;

	memcpy(&header, src, sizeof(DxEPG_Header_t));

#ifdef CONFIG_FUNC_EPG_USE_SVCUID
	field->u.integer = (HINT32)((header.uid >> 32) & 0xFFFFFFFFULL);
#else
	field->u.integer = 0;
#endif

	(void)size;
	return TRUE;
}

/*
static HBOOL	dlib_epg_rf_GetTsID (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section = DLIB_EPG_RF_PARSER_SECTION(src);

	field->u.integer = (HINT32)DLIB_EPG_RF_PARSER_SECTION_TS_ID(section);

	(void)size;
	return TRUE;
}

static HBOOL	dlib_epg_rf_GetSID (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section = DLIB_EPG_RF_PARSER_SECTION(src);

	field->u.integer = (HINT32)DLIB_EPG_RF_PARSER_SECTION_SERVICE_ID(section);

	(void)size;
	return TRUE;
}


static HBOOL	dlib_epg_rf_GetOnID (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section = DLIB_EPG_RF_PARSER_SECTION(src);

	field->u.integer = (HINT32)DLIB_EPG_RF_PARSER_SECTION_ON_ID(section);

	(void)size;
	return TRUE;
}

static HBOOL	dlib_epg_rf_GetTriplet (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section = DLIB_EPG_RF_PARSER_SECTION(src);
	HCHAR	buf[32];

	HxSTD_PrintToStrN(buf, 32, "%x.%x.%x"
					, DLIB_EPG_RF_PARSER_SECTION_ON_ID(section)
					, DLIB_EPG_RF_PARSER_SECTION_TS_ID(section)
					, DLIB_EPG_RF_PARSER_SECTION_SERVICE_ID(section)
				);
	field->u.string = DLIB_StrDup(buf);
	if (field->u.string == NULL)
		return FALSE;

	(void)size;
	return TRUE;
}
*/
static HBOOL	dlib_epg_rf_GetTriplet (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section = DLIB_EPG_RF_PARSER_SECTION(src);
	HCHAR	buf[32];

	HxSTD_PrintToStrN(buf, 32, "%x.%x.%x"
					, DLIB_EPG_RF_PARSER_SECTION_ON_ID(section)
					, DLIB_EPG_RF_PARSER_SECTION_TS_ID(section)
					, DLIB_EPG_RF_PARSER_SECTION_SERVICE_ID(section)
				);
	field->u.string = DLIB_StrDup(buf);
	if (field->u.string == NULL)
		return FALSE;

	(void)size;
	return TRUE;
}

static HBOOL	dlib_epg_rf_GetEventID (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	const HUINT8 *section = DLIB_EPG_RF_PARSER_SECTION(src);
	const HUINT8 *event   = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);

	field->u.integer = (HINT32)DLIB_EPG_RF_PARSER_EVENT_EVENT_ID(event);

	(void)size;
	return TRUE;
}

static HBOOL	dlib_epg_rf_GetSeriesID (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	field->u.integer = 0;

	(void)size;
	return TRUE;
}

static HBOOL	dlib_epg_rf_GetPerson (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	field->u.string = NULL;

	(void)size;
	return TRUE;
}

#ifdef CONFIG_FUNC_RFEPG_USE_KEYWORDSEARCH
static HBOOL	dlib_epg_rf_GetKeyword (DxField_t *field, const HUINT8 *src, HUINT32 size)
{
	HCHAR * pKeyword = NULL;
	HxVector_t * pCRIDs = dlib_epg_rf_GetCRID(src,size);
	if(pCRIDs)
	{
		pKeyword = dlib_epg_rf_GetGroupCRIDs(pCRIDs);
		HLIB_VECTOR_Delete(pCRIDs);
	}

	if(pKeyword)
		field->u.string = pKeyword;
	else
		field->u.string = DLIB_StrDup("NULL");

	return TRUE;
}
#endif



static const DxEPG_Field_t	s_EPG_RF_FIELDS[] =
{
	  {{eDxTYPE_STRING , "name"      , TRUE , {0}}, dlib_epg_rf_GetName}
	, {{eDxTYPE_INTEGER, "start_time", FALSE, {0}}, dlib_epg_rf_GetStartTime}
	, {{eDxTYPE_INTEGER, "end_time"  , FALSE, {0}}, dlib_epg_rf_GetEndTime}
	, {{eDxTYPE_INTEGER, "svcuid"    , TRUE,  {0}}, dlib_epg_rf_GetSvcUID}
	, {{eDxTYPE_INTEGER, "eventid"   , TRUE,  {0}}, dlib_epg_rf_GetEventID}
	, {{eDxTYPE_STRING,  "tripleid"  , TRUE,  {0}}, dlib_epg_rf_GetTriplet}
	, {{eDxTYPE_INTEGER, "seriesid"  , TRUE , {0}}, dlib_epg_rf_GetSeriesID}
	, {{eDxTYPE_STRING,  "person"    , TRUE , {0}}, dlib_epg_rf_GetPerson}
#ifdef CONFIG_FUNC_RFEPG_USE_KEYWORDSEARCH
	, {{eDxTYPE_STRING,  "keyword"	 , TRUE , {0}}, dlib_epg_rf_GetKeyword}
#endif
	// ??
};


DxEvent_t *	dlib_epg_rf_NewEvent (HUID uid, const HUINT8 *src, HUINT32 size)
{
	DxEvent_t		*rf;
	const HUINT8	*section;
	const HUINT8	*eit;
	const HUINT8	*desc;
	const HUINT8	*p;
	HINT32			n, len;
	HINT32			content;
	DxField_t		field;
	HINT32			opindex;
	HxVector_t		*descriptor;
	DxEPG_Header_t	header;
#if defined(CONFIG_DEBUG)//DEBUG
	unsigned long CurTime;
	HxDATETIME_Time_t	ulCurTime,ulStartTime,ulDuration;
	HxDATETIME_Date_t	ulCurDate, ulStartDate;
#endif
	rf = (DxEvent_t *)DLIB_Calloc(sizeof(DxEvent_t));
	if (rf == NULL)
	{
		HxLOG_Error("%s() Out of memory!!\n", __FUNCTION__);
		return NULL;
	}

	memcpy(&header, src, sizeof(DxEPG_Header_t));
	HxLOG_Assert(header.uid == uid);

	section = DLIB_EPG_RF_PARSER_SECTION(src);
	eit     = DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section);

	rf->type = header.type;
	rf->refCount   = 1;
	rf->uid        = uid;
	rf->svcuid     = (HINT32)((uid >> 32) & 0xFFFFFFFFULL);
	rf->tsid       = DLIB_EPG_RF_PARSER_SECTION_TS_ID(section);
	rf->onid       = DLIB_EPG_RF_PARSER_SECTION_ON_ID(section);
	rf->svcid      = DLIB_EPG_RF_PARSER_SECTION_SERVICE_ID(section);
	rf->eventId    = DLIB_EPG_RF_PARSER_EVENT_EVENT_ID(eit);
	rf->startTime  = DLIB_EPG_RF_PARSER_EVENT_StartTime(eit);
	rf->duration   = DLIB_EPG_RF_PARSER_EVENT_Duration(eit);
	rf->runStatus  = DLIB_EPG_RF_PARSER_EVENT_RUNNING_STATUS(eit);
	rf->freeCaMode = DLIB_EPG_RF_PARSER_EVENT_FREE_CA_MODE(eit);
#if defined(CONFIG_DEBUG)//DEBUG
		HxLOG_Print("-----------------------------------------------------\n");
		HxLOG_Print("[dlib_epg_rf_NewEvent] serviceid : %d, runstatus:%d eventid :%d\n",rf->svcid,rf->runStatus,rf->eventId);
		CurTime = HLIB_STD_GetSystemTime();
		HLIB_DATETIME_ConvertUnixTimeToDate(CurTime,&ulCurDate);
		HLIB_DATETIME_ConvertUnixTimeToTime(CurTime,&ulCurTime);
		HxLOG_Print("[dlib_epg_rf_NewEvent] =====>ulCurDate[%02d:%02d] \n", ulCurDate.ucMonth,ulCurDate.ucDay);
		HxLOG_Print("[dlib_epg_rf_NewEvent] =====>ulCurTime[%02d:%02d] \n", ulCurTime.ucHour,ulCurTime.ucMinute);
		HLIB_DATETIME_ConvertUnixTimeToDate(CurTime,&ulStartDate);
		HLIB_DATETIME_ConvertUnixTimeToTime (rf->startTime, &ulStartTime);
		HxLOG_Print("[dlib_epg_rf_NewEvent] =====>ulStartDate[%02d:%02d] \n", ulStartDate.ucMonth,ulStartDate.ucDay);
		HxLOG_Print("[dlib_epg_rf_NewEvent] =====>startTime[%02d:%02d] \n",ulStartTime.ucHour,ulStartTime.ucMinute);
		HLIB_DATETIME_ConvertUnixTimeToTime (rf->duration, &ulDuration);
		HxLOG_Print("[dlib_epg_rf_NewEvent] =====>ulDuration[%02d:%02d] \n",ulDuration.ucHour,ulDuration.ucMinute);
		HxLOG_Print("-----------------------------------------------------\n");

#endif
	 rf->name = dlib_epg_rf_GetNameAll(src, size);
	 rf->text = dlib_epg_rf_GetText(src, size);
	rf->longText = dlib_epg_rf_GetLongText(src, size);
	if (dlib_epg_rf_GetParentalRating(&field, src, size))
		rf->parentalRating = field.u.integer;

	rf->item      = dlib_epg_rf_GetItemAndDescription(src, size);
	rf->component = dlib_epg_rf_GetComponent(src, size);

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	rf->crid = dlib_epg_rf_GetCRID(src, size);
#endif
#if defined(CONFIG_DEBUG)//DEBUG
	if(rf->name)
		HxLOG_Print("rf->name  : %s\n",DxEPG_EVENT_GET_STRING(rf->name));

		HxLOG_Print("rf->runStatus  : %d\n",rf->runStatus);
#endif

	opindex = dlib_epg_rf_GetOperatorIndex(src, "freesat", "jcom","bbc", NULL);
	content = 0;
	descriptor = NULL;
	for ( len  = DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(eit), desc = DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(eit)
		; len > 2
		; len -= (HINT32)(desc[1] + 2), desc += (HINT32)(desc[1] + 2)
	) {
		switch(desc[0])
		{
		case eSIxDESCTAG_CONTENT:	// content descriptor
			if(opindex != 2)// ukdtt(freeview) @see dlib_epg_rf_GetOperatorIndex
			{
				n = (HINT32)(desc[1] / 2);
				for (p = desc + 2 ; n-- && content < DxEVENT_CONTENT ; p += 2)
				{
					rf->content[content++] = (HUINT16)((p[0] << 8) | p[1]);
				}
			}
			break;
		default:
			break;
		}

		if (opindex == 0)	// freesat @see dlib_epg_rf_GetOperatorIndex
		{
			if (!DLIB_EPG_CHECK_TAG(desc[0], eSIxDESCTAG_PRIVATE_DATA_INDICATOR, eSIxDESCTAG_PRIVATE_DATA_SPECIFIER))
				continue;
		}
		else if (opindex == 1)	// jcom @see dlib_epg_rf_GetOperatorIndex
		{
			if (!DLIB_EPG_CHECK_TAG(desc[0], 0xC1, 0xC4, 0xC7, 0xD6, 0xD9, 0xDE, 0xCB, 0xFF))
				continue;
		}
		else if(opindex == 2)	// ukdtt(freeview) @see dlib_epg_rf_GetOperatorIndex
		{
// 사용하실 분만 열어서 사용하세요..열고 사용하면 세트 동작 늦어짐..
#if 0//defined(CONFIG_DEBUG)
			FILE *fp;
			fp = fopen("epgdata.txt", "wb+");
			DLIB_EPG_RF_Print(fp, rf);
			fclose(fp);
#endif
			if (!DLIB_EPG_CHECK_TAG(desc[0], eSIxDESCTAG_PRIVATE_DATA_INDICATOR, eSIxDESCTAG_PRIVATE_DATA_SPECIFIER,eSIxDESCTAG_DATA_BROADCAST,eSIxDESCTAG_LINKAGE,eSIxDESCTAG_UKDTT_GUIDANCE,eSIxDESCTAG_CONTENT))
				continue;
		}
		else continue;

		if (descriptor == NULL)
		{
			descriptor = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
			if (descriptor == NULL)
				continue;
		}
		HLIB_VECTOR_Add(descriptor, DLIB_MemDup(desc, (HUINT32)(desc[1] + 2)));
	}
	rf->descriptor = descriptor;


	return rf;
}

DxEvent_t *	dlib_epg_rf_CloneEvent (const DxEvent_t *event)
{
	DxEvent_t *clone;

	clone = (DxEvent_t *)DLIB_MemDup(event, sizeof(DxEvent_t));
	if (clone)
	{
		clone->refCount = 1;
		if (event->name)
			clone->name = HLIB_VECTOR_CloneEx((HxVector_t *)clone->name, (void *(*)(void *))dlib_epg_rf_CloneString);
		if (event->text)
			clone->text = HLIB_VECTOR_CloneEx((HxVector_t *)clone->text, (void *(*)(void *))dlib_epg_rf_CloneString);
		if (event->longText)
			clone->longText = HLIB_VECTOR_CloneEx((HxVector_t *)clone->longText, (void *(*)(void *))dlib_epg_rf_CloneString);
		if (event->item)
			clone->item =  HLIB_VECTOR_CloneEx((HxVector_t *)event->item, (void *(*)(void *))dlib_epg_rf_CloneEventItem);
		if (event->component)
			clone->component = HLIB_VECTOR_CloneEx((HxVector_t *)event->component, (void *(*)(void *))dlib_epg_rf_CloneComponent);
		if (event->descriptor)
			clone->descriptor = HLIB_VECTOR_CloneEx((HxVector_t *)event->descriptor, (void *(*)(void *))dlib_epg_rf_CloneDescriptor);
		if (event->linkage)
			clone->linkage = HLIB_VECTOR_CloneEx((HxVector_t *)event->linkage, (void *(*)(void *))dlib_epg_rf_CloneLinkageInfo);
		if (event->crid)
			clone->crid = HLIB_VECTOR_CloneEx((HxVector_t *)event->crid, (void *(*)(void *))dlib_epg_rf_CloneCRID);


	}
	return clone;
}

void		dlib_epg_rf_DeleteEvent (DxEvent_t *event)
{
	HxLOG_Assert(event && event->type & eDxEPG_TYPE_RF);
	if (event->name)
		HLIB_VECTOR_Delete(event->name);
	if (event->text)
		HLIB_VECTOR_Delete(event->text);
	if (event->longText)
		HLIB_VECTOR_Delete(event->longText);
	if (event->item)
		HLIB_VECTOR_Delete(event->item);
	if (event->component)
		HLIB_VECTOR_Delete(event->component);
	if (event->descriptor)
		HLIB_VECTOR_Delete(event->descriptor);
	if (event->linkage)
		HLIB_VECTOR_Delete(event->linkage);
	if (event->crid)
		HLIB_VECTOR_Delete(event->crid);



#ifdef	CONFIG_OP_FREESAT
		// TODO: delete extension

#endif
	DLIB_Free(event);
}

void			DLIB_EPG_RF_Init (void)
{
	static DxEnumStringTable_t s_DxEPG_Type_e =
	{
		(HCHAR*)DxNAMEOF(DxEPG_Type_e),
		{
			{"PF",    eDxEPG_TYPE_PF},
			{"SC",    eDxEPG_TYPE_SC},
			{"PF ex", eDxEPG_TYPE_PF_EX},
			{"SC ex", eDxEPG_TYPE_SC_EX},
			{"TVTV",  eDxEPG_TYPE_TVTV},
			{"IP",    eDxEPG_TYPE_IP},
			{NULL, 0}
		}
	};
	static HBOOL	s_initDone;

	if (!s_initDone)
	{
		s_initDone = TRUE;
		dlib_RegisterEnumTable(&s_DxEPG_Type_e);
	}
}

HUINT32			DLIB_EPG_PF_GetPushCycle (void)
{
	return 1000;
}

HUINT32			DLIB_EPG_SC_GetPushCycle (void)
{
	return 5000;
}

HUINT32			DLIB_EPG_PF_GetThreshold (void)
{
	//return 10;
	return 10000;
}

HUINT32			DLIB_EPG_SC_GetThreshold (void)
{
	//return 500;
	return 30000;
}

const HCHAR *	DLIB_EPG_PF_GetDBFullName (HINT32 dupIndex)
{
	const HCHAR		*path = NULL;

	if (dupIndex == 0)
	{
#if defined(CONFIG_MW_EPG_DB_SAVE_FLASH) && !defined(CONFIG_MW_EPG_DB_SAVE_HDD)
		path = DLIB_RFEPG_PF_DBFILE_FLASH;
#else
		path = ":memory:";
#endif
	}

	return path;
}

const HCHAR *	DLIB_EPG_SC_GetDBFullName (HINT32 dupIndex)
{
	const HCHAR		*path = NULL;

	if (dupIndex == 0)
	{
#if defined(CONFIG_MW_EPG_DB_SAVE_FLASH) && !defined(CONFIG_MW_EPG_DB_SAVE_HDD)
		path = DLIB_RFEPG_SC_DBFILE_FLASH;
#else
		path = ":memory:";
#endif
	}

	return path;
}

HINT32			DLIB_EPG_RF_GetFieldCount (void)
{
	return sizeof(s_EPG_RF_FIELDS) / sizeof(s_EPG_RF_FIELDS[0]);
}

DxField_t *		DLIB_EPG_RF_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_EPG_RF_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_EPG_RF_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_EPG_RF_FIELDS[index].getData)
	{
		if (!s_EPG_RF_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_EPG_RF_FIELDS[index].field.name);
			return NULL;
		}
	}
	// 메모리 함수는 HxSTD_MemXXX를 사용한다.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}


HERROR			DLIB_EPG_RF_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxEvent_t	*rf;
	if (srcSize == sizeof(DxEvent_t) && uid == DxEPG_CLONE_UID)
		rf = dlib_epg_rf_CloneEvent((const DxEvent_t *)src);
	else
		rf = dlib_epg_rf_NewEvent(uid, (const HUINT8 *) src, srcSize);
	if (rf == NULL)
		return ERR_FAIL;

	*type = rf;
	return ERR_OK;
}

void			DLIB_EPG_RF_FreeType (void *type)
{
	dlib_epg_rf_DeleteEvent((DxEvent_t *)type);
}

void			DLIB_EPG_RF_Print (FILE *fp, const void *t)
{
	DxEvent_t	*event;
	HINT32		i;


	event = (DxEvent_t *)t;
	if (event)
	{
		fprintf(fp,
			"\nDxEvent_t(0x%X) refCount:%d\n"
			"\ttype:%s\n"
			"\tsvcuid:%d,"
			"\teventId:0x%X\n"
			"\tstartTime:%s, duration:%usec\n"
			"\trunning status:0x%X, freeCAmode:%d\n"
			"\tname:\'%s\'\n"
			"\ttext:\'%s\'\n"
			"\tlongText:\'%s\'\n"
			"\tparantal rating:%d\n"
			, (int)event, event->refCount
			, DxEPG_NAMEOF(event->type)
			, event->svcuid
			, event->eventId
			, HLIB_DATETIME_UnixTimeToString((UNIXTIME)event->startTime, NULL, 0), event->duration
			, event->runStatus, event->freeCaMode
			, DxEPG_EVENT_GET_STRING(event->name)
			, DxEPG_EVENT_GET_STRING(event->text)
			, DxEPG_EVENT_GET_STRING(event->longText)
			, event->parentalRating
		);
		fprintf(fp, "\tcontent:");
		for (i = 0 ; i < DxEVENT_CONTENT ; i++)
			fprintf(fp, " %04X", event->content[i]);
		fprintf(fp, "\n");

		if (event->item)
		{
			DxEventItem_t *item;
			fprintf(fp, "\titem(0x%X):", (int)event->item);
			HxVECTOR_EACH(event->item, DxEventItem_t *, item, {
				fprintf(fp, "\t\t%s[%s]\n", item->description, item->item);
			});
			fprintf(fp, "\n");
		}

		if (event->linkage)
		{
			DxEventLinkage_t *item;
			fprintf(fp, "\tlinkage(0x%X):", (int)event->linkage);
			HxVECTOR_EACH(event->linkage, DxEventLinkage_t *, item, {
				fprintf(fp, "\t\t%d.%d.%d type:%d\n", item->onid, item->tsid, item->svcid, item->linkageType);
			});
			fprintf(fp, "\n");
		}

		if (event->component)
		{
			DxEventComponent_t *item;
			fprintf(fp, "\tcomponent(0x%X):", (int)event->component);
			HxVECTOR_EACH(event->component, DxEventComponent_t *, item, {
				fprintf(fp, "\t\tcontent:%d, type:%d, tag:%d, text:%s\n", item->content, item->type, item->tag, item->text);
			});
			fprintf(fp, "\n");
		}

		if (event->descriptor)
		{
			const HUINT8 *item;
			fprintf(fp, "\tdescriptor(0x%X):", (int)event->descriptor);
			HxVECTOR_EACH(event->descriptor, const HUINT8 *, item, {
				fprintf(fp, "\t\ttag:0x%X, length:%u\n", item[0], item[1]);
			});
			fprintf(fp, "\n");
		}



	}
}


static HBOOL dlib_epg_rf_EncodeEx (HINT32 jsonWriter, const DxEvent_t *event)
{
#if defined(CONFIG_OP_FREESAT)
	HxJSONWRITER_Object(jsonWriter, "freesat");
	{
		DxFreesatEvent_t *freesat = (DxFreesatEvent_t *)&(event->extension.fsateventinfo);

		HxJSONWRITER_Object(jsonWriter, "guidance");
		{
			HxJSONWRITER_Integer(jsonWriter, "type", freesat->fsatGuidance.guidancetype);
			if (freesat->fsatGuidance.guidancedata)
				HxJSONWRITER_String(jsonWriter, "data", freesat->fsatGuidance.guidancedata);
		}
		HxJSONWRITER_ObjectEnd(jsonWriter);

		if (freesat->fsatShortEventPromo.szShortEventPromo)
			HxJSONWRITER_String(jsonWriter, "short_event_promo", freesat->fsatShortEventPromo.szShortEventPromo);

		if (!HxVECTOR_IS_EMPTY(freesat->fsatMediaLocatorInfo))
		{
			DxEventMediaLocatorInfo_t *item;
			HxJSONWRITER_Array(jsonWriter, "media_locator");
			HxVECTOR_EACH(freesat->fsatMediaLocatorInfo, DxEventMediaLocatorInfo_t *, item, {
				HxJSONWRITER_ObjectBegin(jsonWriter);
					HxJSONWRITER_Integer(jsonWriter, "type", item->mediatype);
					HxJSONWRITER_Integer(jsonWriter, "usage", item->mediausage);
					HxJSONWRITER_Integer(jsonWriter, "prefix_location", item->media_prefix_location);
					HxJSONWRITER_Integer(jsonWriter, "prefix_index", item->media_prefix_index);
					if (item->URI_char)
						HxJSONWRITER_String(jsonWriter, "URI_char", item->URI_char);
				HxJSONWRITER_ObjectEnd(jsonWriter);
			});
			HxJSONWRITER_ArrayEnd(jsonWriter);
		}

		if (!HxVECTOR_IS_EMPTY(freesat->ext_descriptor))
		{
			HUINT8 *item;
			HxJSONWRITER_Array(jsonWriter, "descriptor");
			HxVECTOR_EACH(freesat->ext_descriptor, HUINT8 *, item, {
				HxJSONWRITER_BinaryValue(jsonWriter, item, 2 + item[1]);
			});
			HxJSONWRITER_ArrayEnd(jsonWriter);
		}
	}
	HxJSONWRITER_ObjectEnd(jsonWriter);
	return TRUE;
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMS1000T) || defined(CONFIG_PROD_HMSLITE) || defined (CONFIG_PROD_DIGIMAXT2)
	if (event->type == eDxEPG_TYPE_IP)
	{
		Ext_IpEpgInfo_t	*ext = (Ext_IpEpgInfo_t *)&(event->extension.ipEpg);
		HxJSONWRITER_Object(jsonWriter, "IP");
		{
			HxJSONWRITER_Integer(jsonWriter, "drm", ext->drm);
			HxJSONWRITER_Integer(jsonWriter, "series_id", ext->seriesId);
			HxJSONWRITER_Integer(jsonWriter, "episode", ext->episodeNum);
			HxJSONWRITER_Integer(jsonWriter, "total_episode", ext->totalNum);
			HxJSONWRITER_Integer(jsonWriter, "season", ext->season);
			HxJSONWRITER_Integer(jsonWriter, "series_type", ext->seriesType);
			if (ext->eventLink[0].ucType)
			{
				HINT32	i;
				HxJSONWRITER_Array(jsonWriter, "link");
				for (i = 0 ; i < EPG_HUMAX_LINK_MAX && ext->eventLink[i].ucType ; i++)
				{
					HxJSONWRITER_ObjectBegin(jsonWriter);
						HxJSONWRITER_Integer(jsonWriter, "type", ext->eventLink[i].ucType);
						HxJSONWRITER_Integer(jsonWriter, "programme_id", ext->eventLink[i].ulProgId);
					HxJSONWRITER_ObjectEnd(jsonWriter);
				}
				HxJSONWRITER_ArrayEnd(jsonWriter);
			}

			if (ext->actor)
				HxJSONWRITER_String(jsonWriter, "actor", ext->actor);
			if (ext->actress)
				HxJSONWRITER_String(jsonWriter, "actress", ext->actress);
			if (ext->director)
				HxJSONWRITER_String(jsonWriter, "director", ext->director);
			if (ext->person)
				HxJSONWRITER_String(jsonWriter, "person", ext->person);
			if (ext->thumbnailURL)
				HxJSONWRITER_String(jsonWriter, "thumbnailURL", ext->thumbnailURL);
		}
		HxJSONWRITER_ObjectEnd(jsonWriter);
	}
	return TRUE;
#else
	return FALSE;
#endif
}


static DxEvent_t *		dlib_epg_rf_DecodeEx (DxEvent_t *event, HxJSON_t json)
{
	HxJSON_t	ext;

#if defined(CONFIG_OP_FREESAT)
	ext = HLIB_JSON_Object_GetObject(json, "freesat");
	if (ext)
	{
		DxFreesatEvent_t *freesat = &(event->extension.fsateventinfo);
		HxJSON_t	obj, val;

		obj = HLIB_JSON_Object_GetObject(ext, "guidance");
		if (obj)
		{
			freesat->fsatGuidance.guidancetype = HLIB_JSON_Object_GetInteger(obj, "type");
			freesat->fsatGuidance.guidancedata = DLIB_StrDup(HLIB_JSON_Object_GetString(obj, "data"));
		}
		freesat->fsatShortEventPromo.szShortEventPromo = DLIB_StrDup(HLIB_JSON_Object_GetString(ext, "short_event_promo"));

		obj = HLIB_JSON_Object_GetArray(ext, "media_locator");
		if (obj && HLIB_JSON_GetSize(obj) > 0)
		{
			HxLOG_Warning("%s(%d) NOT IMPLEMENTED(media_locator)!!!\n", __FUNCTION__, __LINE__);
		}

		obj = HLIB_JSON_Object_GetArray(ext, "media_locator");
		if (obj && HLIB_JSON_GetSize(obj) > 0)
		{
			HxLOG_Warning("%s(%d) NOT IMPLEMENTED(descriptor)!!!\n", __FUNCTION__, __LINE__);

		}
	}
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE) || defined(CONFIG_PROD_HMS1000T) || defined (CONFIG_PROD_DIGIMAXT2)
	ext = HLIB_JSON_Object_GetObject(json, "IP");
	if (ext)
	{
		Ext_IpEpgInfo_t *e = &(event->extension.ipEpg);
		HxJSON_t	arr, val;

		e->drm        = HLIB_JSON_Object_GetInteger(ext, "drm");
		e->seriesId   = HLIB_JSON_Object_GetInteger(ext, "series_id");
		e->episodeNum = HLIB_JSON_Object_GetInteger(ext, "episode");
		e->totalNum   = HLIB_JSON_Object_GetInteger(ext, "total_episode");
		e->season     = HLIB_JSON_Object_GetInteger(ext, "season");
		e->seriesType = HLIB_JSON_Object_GetInteger(ext, "series_type");

		arr = HLIB_JSON_Object_GetArray(ext, "link");
		if (arr && HLIB_JSON_GetSize(arr) > 0)
		{
			HINT32 c = 0;
			HxJSON_ARRAY_EACH(arr, val, {
				e->eventLink[c].ucType   = HLIB_JSON_Object_GetInteger(val, "type");
				e->eventLink[c].ulProgId = HLIB_JSON_Object_GetInteger(val, "programme_id");
				if (++c == EPG_HUMAX_LINK_MAX)
					break;
			});
		}
		e->actor        = DLIB_StrDup(HLIB_JSON_Object_GetString(ext, "actor"));
		e->actress      = DLIB_StrDup(HLIB_JSON_Object_GetString(ext, "actress"));
		e->director     = DLIB_StrDup(HLIB_JSON_Object_GetString(ext, "director"));
		e->person     = DLIB_StrDup(HLIB_JSON_Object_GetString(ext, "person"));
		e->thumbnailURL = DLIB_StrDup(HLIB_JSON_Object_GetString(ext, "thumbnailURL"));
	}
#else
	(void)(ext);
#endif
	return event;
}

HCHAR *	DxEVENT_Encode (HINT32 jsonWriter, const DxEvent_t *event)
{
	HINT32	writer;
	HINT32	i, n;
	const HCHAR	*json;

	HxLOG_Assert(event);

	if (jsonWriter)
		writer = jsonWriter;
	else
		writer = HLIB_JSONWRITER_Open();

	HxJSONWRITER_ObjectBegin(writer);
	{
		HxJSONWRITER_Binary (writer, "uid", &event->uid, sizeof(HUID));
		HxJSONWRITER_String (writer, "type", dlib_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), event->type));
		HxJSONWRITER_Integer(writer, "svcuid", event->svcuid);
		HxJSONWRITER_Integer(writer, "tsid", event->tsid);
		HxJSONWRITER_Integer(writer, "onid", event->onid);
		HxJSONWRITER_Integer(writer, "svcid", event->svcid);
		HxJSONWRITER_Integer(writer, "event_id", event->eventId);
		HxJSONWRITER_Integer(writer, "start_time", event->startTime);
		HxJSONWRITER_Integer(writer, "duration", event->duration);
		HxJSONWRITER_Integer(writer, "running_status", event->runStatus);
		HxJSONWRITER_Boolean(writer, "free_CA_mode", event->freeCaMode);
#if 0 // 현재 미사용. 추후 사용시 coding
		if (event->name)
			HxJSONWRITER_String (writer, "name", event->name);
		if (event->text)
			HxJSONWRITER_String (writer, "text", event->text);
#endif
		HxJSONWRITER_Integer(writer, "parental_rating", event->parentalRating);
		if (event->content[0])
		{
			HxJSONWRITER_Array(writer, "content");
			for (i = 0 ; i < DxEVENT_CONTENT && event->content[i] ; i++)
			{
				HxJSONWRITER_IntegerValue(writer, event->content[i]);
			}
			HxJSONWRITER_ArrayEnd(writer);
		}
#if 0 // 현재 미사용. 추후 사용시 coding
		if (event->longText)
			HxJSONWRITER_String (writer, "long_text", event->longText);
#endif

		if (event->item && (n = HLIB_VECTOR_Length(event->item)) > 0)
		{
			DxEventItem_t *item;
			HxJSONWRITER_Array(writer, "item");
			for (i = 0 ; i < n ; i++)
			{
				item = (DxEventItem_t *)HLIB_VECTOR_ItemAt(event->item, i);
				HxJSONWRITER_ObjectBegin(writer);
				{
					if (item->description)
						HxJSONWRITER_String(writer, "description", item->description);
					if (item->item)
						HxJSONWRITER_String(writer, "item", item->item);
				}
				HxJSONWRITER_ObjectEnd(writer);
			}
			HxJSONWRITER_ArrayEnd(writer);
		}

		if (event->linkage && (n = HLIB_VECTOR_Length(event->linkage)) > 0)
		{
			DxEventLinkage_t *item;
			HxJSONWRITER_Array(writer, "linkage");
			for (i = 0 ; i < n ; i++)
			{
				item = (DxEventLinkage_t *)HLIB_VECTOR_ItemAt(event->linkage, i);
				HxJSONWRITER_ObjectBegin(writer);
				{
					HxJSONWRITER_Integer(writer, "onid", item->onid);
					HxJSONWRITER_Integer(writer, "tsid", item->tsid);
					HxJSONWRITER_Integer(writer, "svcid", item->svcid);
					HxJSONWRITER_Integer(writer, "type", item->linkageType);
					// TODO: privateData ?? linkage는 사용하지 않는 것을 원칙으로 했지만,.. 암튼 지우기 전이니
				}
				HxJSONWRITER_ObjectEnd(writer);
			}
			HxJSONWRITER_ArrayEnd(writer);
		}

		if (event->component && (n = HLIB_VECTOR_Length(event->component)) > 0)
		{
			DxEventComponent_t *item;
			HxJSONWRITER_Array(writer, "component");
			for (i = 0 ; i < n ; i++)
			{
				item = (DxEventComponent_t *)HLIB_VECTOR_ItemAt(event->component, i);
				HxJSONWRITER_ObjectBegin(writer);
				{
					HxJSONWRITER_Integer(writer, "content", item->content);
					HxJSONWRITER_Integer(writer, "type", item->type);
					HxJSONWRITER_Integer(writer, "tag", item->tag);
					HxJSONWRITER_String(writer, "language", HLIB_LANG_IdTo639(item->language));
					if (item->text)
						HxJSONWRITER_String(writer, "text", item->text);
				}
				HxJSONWRITER_ObjectEnd(writer);
			}
			HxJSONWRITER_ArrayEnd(writer);
		}

		if (event->crid && (n = HLIB_VECTOR_Length(event->crid)) > 0)
		{
			DxEventCRID_t *item;
			HxJSONWRITER_Array(writer, "crid");
			for (i = 0 ; i < n ; i++)
			{
				item = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(event->crid, i);
				if (item->crid == NULL)
					continue;
				HxJSONWRITER_ObjectBegin(writer);
				{
					HxJSONWRITER_Integer(writer, "type", item->type);
					HxJSONWRITER_String (writer, "crid", item->crid);
				}
				HxJSONWRITER_ObjectEnd(writer);
			}
			HxJSONWRITER_ArrayEnd(writer);
		}

		if (event->descriptor && (n = HLIB_VECTOR_Length(event->descriptor)) > 0)
		{
			HUINT8 *item;
			HxJSONWRITER_Array(writer, "descriptor");
			for (i = 0 ; i < n ; i++)
			{
				item = (HUINT8 *)HLIB_VECTOR_ItemAt(event->descriptor, i);
				HxJSONWRITER_BinaryValue(writer, item, 2 + item[1]);
			}
			HxJSONWRITER_ArrayEnd(writer);
		}



		HxJSONWRITER_Object(jsonWriter, "extension");
			(void) dlib_epg_rf_EncodeEx(writer, event);
		HxJSONWRITER_ObjectEnd(writer);
	}
	HxJSONWRITER_ObjectEnd(writer);

	json = HLIB_JSONWRITER_Error(writer);
	if (json)
	{
		HxLOG_Error("%s():%d error:%s\n", __FUNCTION__, __LINE__, json);
		if (!jsonWriter)
			HLIB_JSONWRITER_Close(writer);
		return NULL;
	}

	if (!jsonWriter)
	{
		HCHAR	*_json = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(writer));
		HLIB_JSONWRITER_Close(writer);
		return _json;
	}
	return NULL;
}


DxEvent_t *		DxEVENT_Decode (DxEvent_t *event, HxJSON_t json)
{
	HINT32       i;
	DxEvent_t   *e;
	HxVector_t	*list;
	HxJSON_t     arr, val;
	const HCHAR	*text;

	if (event == NULL)
		e = (DxEvent_t *)DLIB_Calloc(sizeof(DxEvent_t));
	else
		e = event;

	e->refCount  = 1;
	HLIB_JSON_Object_GetBinary(json, "uid", &e->uid, sizeof(HUID));
	text         = HLIB_JSON_Object_GetString(json, "type");
	if (text)
		e->type  = dlib_ConvertStr2Enum(DxNAMEOF(DxEPG_Type_e), text);
	e->svcuid    = HLIB_JSON_Object_GetInteger(json, "svcuid");
	e->tsid      = HLIB_JSON_Object_GetInteger(json, "tsid");
	e->onid 	 = HLIB_JSON_Object_GetInteger(json, "onid");
	e->svcid 	 = HLIB_JSON_Object_GetInteger(json, "tsvcid");
	e->eventId   = HLIB_JSON_Object_GetInteger(json, "event_id");
	e->startTime = (HUINT32)HLIB_JSON_Object_GetInteger(json, "start_time");
	e->duration  = (HUINT32)HLIB_JSON_Object_GetInteger(json, "duration");
	e->runStatus = (HUINT8 )HLIB_JSON_Object_GetInteger(json, "running_status");
	e->freeCaMode= HLIB_JSON_Object_GetBoolean(json, "free_CA_mode");
#if 0 // 현재 미사용. 추후 사용시 coding
	e->name      = DLIB_StrDup(HLIB_JSON_Object_GetString(json, "name"));
	e->text      = DLIB_StrDup(HLIB_JSON_Object_GetString(json, "text"));
#endif
	e->parentalRating = HLIB_JSON_Object_GetInteger(json, "parental_rating");

	arr = HLIB_JSON_Object_GetArray(json, "content");
	if (arr)
	{
		i = 0;
		HxJSON_ARRAY_EACH(arr, val, {
			e->content[i++] = (HUINT16)HxJSON_Integer_Get(val);
			if (i == DxEVENT_CONTENT) break;
		});
	}
#if 0 // 현재 미사용. 추후 사용시 coding
	e->longText  = DLIB_StrDup(HLIB_JSON_Object_GetString(json, "long_text"));
#endif
	arr = HLIB_JSON_Object_GetArray(json, "item");
	if (arr && HLIB_JSON_GetSize(arr) > 0)
	{
		DxEventItem_t item, *clone;

		list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteEventItem, NULL);
		if (list)
		{
			HxJSON_ARRAY_EACH(arr, val, {
				item.description = (HCHAR *)HLIB_JSON_Object_GetString(val, "description");
				item.item = (HCHAR *)HLIB_JSON_Object_GetString(val, "item");

				if (!item.description && !item.item)
					continue;
				clone = dlib_epg_rf_CloneEventItem(&item);
				if (!clone)
					continue;
				HLIB_VECTOR_Add(list, clone);
			});
		}
		HxVECTOR_DELETE_IF_EMPTY(list);
		e->item = list;
	}

	arr = HLIB_JSON_Object_GetArray(json, "linkage");
	if (arr && HLIB_JSON_GetSize(arr) > 0)
	{
		HxLOG_Warning("%s(%d) NOT IMPLEMENTED(linkage)!!!\n", __FUNCTION__, __LINE__);
	}

	arr = HLIB_JSON_Object_GetArray(json, "component");
	if (arr && HLIB_JSON_GetSize(arr) > 0)
	{
		DxEventComponent_t item, *clone;

		list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteComponent, NULL);
		if (list)
		{
			HxJSON_ARRAY_EACH(arr, val, {
				item.content  = HLIB_JSON_Object_GetInteger(val, "content");
				item.type     = HLIB_JSON_Object_GetInteger(val, "type");
				item.tag      = HLIB_JSON_Object_GetInteger(val, "tag");
				text = HLIB_JSON_Object_GetString(val, "language");
				if (text)
					item.language = HLIB_LANG_639ToId(text);
				item.text     = (HCHAR *)HLIB_JSON_Object_GetString(val, "text");

				clone = dlib_epg_rf_CloneComponent(&item);
				if (!clone)
					continue;
				HLIB_VECTOR_Add(list, clone);
			});
		}
		HxVECTOR_DELETE_IF_EMPTY(list);
		e->component = list;
	}

	arr = HLIB_JSON_Object_GetArray(json, "crid");
	if (arr && HLIB_JSON_GetSize(arr) > 0)
	{
		DxEventCRID_t item, *clone;

		list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))dlib_epg_rf_DeleteCRID, NULL);
		if (list)
		{
			HxJSON_ARRAY_EACH(arr, val, {
				item.type  = HLIB_JSON_Object_GetInteger(val, "type");
				item.crid     = (HCHAR *)HLIB_JSON_Object_GetString(val, "crid");

				clone = dlib_epg_rf_CloneCRID(&item);
				if (!clone)
					continue;
				HLIB_VECTOR_Add(list, clone);
			});
		}
		HxVECTOR_DELETE_IF_EMPTY(list);
		e->crid = list;
	}

	arr = HLIB_JSON_Object_GetArray(json, "descriptor");
	if (arr && HLIB_JSON_GetSize(arr) > 0)
	{
		HUINT8	*item;

		list = HLIB_VECTOR_NewEx(NULL, 0, NULL, (void *(*)(void *))DLIB_Free_CB, NULL);
		if (list)
		{
			HxJSON_ARRAY_EACH(arr, val, {
				text = HxJSON_String_Get(val);
				if (text == NULL)
					continue;

				item = (HUINT8 *)HLIB_MATH_DecodeBase64Easy(text, NULL);
				if (!item)
					continue;
				HLIB_VECTOR_Add(list, item);
			});
		}
		HxVECTOR_DELETE_IF_EMPTY(list);
		e->descriptor = list;
	}



	val = HLIB_JSON_Object_GetObject(json, "extension");
	if (val)
		(void) dlib_epg_rf_DecodeEx(e, val);
	return e;
}


DxEvent_t	*	DxEVENT_Clone (const DxEvent_t *event)
{
	if (event == NULL)
		return NULL;

	return DLIB_NewType(DxEPG_NAMEOF(event->type), DxEPG_CLONE_UID, event, sizeof(DxEvent_t));
}

void			DxEVENT_Delete (DxEvent_t *event)
{
	if (event == NULL)
		return;

	DLIB_FreeType(DxEPG_NAMEOF(event->type), event);
}

HxFreeFunc_t	DxEVENT_GetFreeFunc (const HCHAR *type)
{
	if (strcmp(type, HxNAMEOF(DxEventComponent_t)) == 0)
		return (HxFreeFunc_t)dlib_epg_rf_DeleteComponent;
	if (strcmp(type, HxNAMEOF(DxEventCRID_t)) == 0)
		return (HxFreeFunc_t)dlib_epg_rf_DeleteCRID;
	if (strcmp(type, HxNAMEOF(DxEventLinkage_t)) == 0)
		return (HxFreeFunc_t)dlib_epg_rf_DeleteLinkageInfo;
	if (strcmp(type, HxNAMEOF(DxEventItem_t)) == 0)
		return (HxFreeFunc_t)dlib_epg_rf_DeleteEventItem;
	return NULL;
}


