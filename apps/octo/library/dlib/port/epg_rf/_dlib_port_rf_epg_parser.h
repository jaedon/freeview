
#ifndef	DLIB_EPG_RF_PARSER_H__
#define DLIB_EPG_RF_PARSER_H__


#include <hlib.h>

#ifdef	__cplusplus
extern	"C" {
#endif

#define	DLIB_EPG_RF_PARSER_CAST(type,expr)				((type)(expr))
#define	DLIB_CAST(t,e)	DLIB_EPG_RF_PARSER_CAST(t,e)

#define	DLIB_EPG_RF_PARSER_SECTION(src)					((const HUINT8 *)src + sizeof(DxEPG_Header_t))
#define	DLIB_EPG_RF_PARSER_SECTION_TABLE_ID(section)	DLIB_CAST(HUINT8,  (section)[0])
#define	DLIB_EPG_RF_PARSER_SECTION_LENGTH(section)		DLIB_CAST(HINT32,  HxMACRO_Get12Bit(&((section)[1])))
#define	DLIB_EPG_RF_PARSER_SECTION_SERVICE_ID(section)	DLIB_CAST(HUINT16, HxMACRO_Get16Bit(&(section[3])))
#define	DLIB_EPG_RF_PARSER_SECTION_TS_ID(section)		DLIB_CAST(HUINT16, HxMACRO_Get16Bit(&(section[8])))
#define	DLIB_EPG_RF_PARSER_SECTION_ON_ID(section)		DLIB_CAST(HUINT16, HxMACRO_Get16Bit(&(section[10])))
#define	DLIB_EPG_RF_PARSER_SECTION_VERSION(section)		DLIB_CAST(HUINT8,  (section)[5])
#define	DLIB_EPG_RF_PARSER_SECTION_SECTION_NO(section)	DLIB_CAST(HUINT8,  (section)[6])
#define	DLIB_EPG_RF_PARSER_SECTION_LAST_SECTION_NO(section)			DLIB_CAST(HUINT8,  (section)[7])
#define	DLIB_EPG_RF_PARSER_SECTION_SEGMENT_LAST_SECTION_NO(section)	DLIB_CAST(HUINT8,  (section)[12])
#define	DLIB_EPG_RF_PARSER_SECTION_CRC32(section)		DLIB_CAST(HUINT32, HxMACRO_Get32Bit(&(section[DLIB_EPG_RF_PARSER_SECTION_LENGTH(section) - 1])))
#define	DLIB_EPG_RF_PARSER_SECTION_TOTAL_LENGTH(section)	(DLIB_EPG_RF_PARSER_SECTION_LENGTH(section) + 3)

//HINT32	DLIB_EPG_RF_PARSER_SECTION_EventLoopOffset(const HUINT8 *section);
#define	DLIB_EPG_RF_PARSER_SECTION_EVENTLOOP(section)	((const HUINT8 *)section + 14)

#define	DLIB_EPG_RF_PARSER_EVENT_EVENT_ID(event)		DLIB_CAST(HUINT16, HxMACRO_Get16Bit(&(event[0])))
#define	DLIB_EPG_RF_PARSER_EVENT_RUNNING_STATUS(event)	DLIB_CAST(HUINT8,  (event[10] & 0xE0) >> 5)
#define	DLIB_EPG_RF_PARSER_EVENT_FREE_CA_MODE(event)	((event[10] & 0x10) ? TRUE : FALSE)
#define	DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event)		DLIB_CAST(HINT32,  HxMACRO_Get12Bit(&(event[10])))
#define	DLIB_EPG_RF_PARSER_EVENT_DESCRIPTOR(event)		((const HUINT8 *)(event) + 12)
#define	DLIB_EPG_RF_PARSER_EVENT_TOTAL_LENGTH(event)	(DLIB_EPG_RF_PARSER_EVENT_LOOP_LENGTH(event) + 12)

HUINT32	DLIB_EPG_RF_PARSER_EVENT_StartTime (const HUINT8 *event);
HUINT32	DLIB_EPG_RF_PARSER_EVENT_Duration (const HUINT8 *event);

#undef	_DxCAST

#ifdef	__cplusplus
}
#endif

#endif /* DLIB_EPG_RF_PARSER_H__ */


