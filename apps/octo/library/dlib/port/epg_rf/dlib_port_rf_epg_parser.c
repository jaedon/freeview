
#include <hlib.h>

#include "_dlib_port_rf_epg_parser.h"


#define	RETURN_IF_VOID(expr)	do{if(expr)return;}while(0)
#define	RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)
#define	GOTO_IF(expr, pos)		do{if(expr)goto pos;}while(0)

#define	D_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return;}}while(0)
#define	D_RETURN_IF(expr, err)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);return (err);}}while(0)
#define	D_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Debug("%s failed!!\n", #expr);goto pos;}}while(0)

#define	W_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return;}}while(0)
#define	W_RETURN_IF(expr, err)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);return (err);}}while(0)
#define	W_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Warning("%s failed!!\n", #expr);goto pos;}}while(0)

#define	E_RETURN_IF_VOID(expr)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return;}}while(0)
#define	E_RETURN_IF(expr, err)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)
#define	E_GOTO_IF(expr, pos)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);goto pos;}}while(0)


HINT32	DLIB_EPG_RF_PARSER_SECTION_EventLoopOffset(const HUINT8 *section)
{
	HINT32			sectionLen;
	HINT32			loopLen;
	HINT32			length;
	HINT32			n;
	const HUINT8	*loop;
	static const HINT32	s_offset[] = {13, 14};

	sectionLen = DLIB_EPG_RF_PARSER_SECTION_LENGTH(section);
	W_RETURN_IF(sectionLen < 0, 0);

	n = sizeof(s_offset) / sizeof(s_offset[0]);
	while (n--)
	{
		loop   = section + s_offset[n];
		length = sectionLen - 4 - (s_offset[n] - 3);
		while (length > 0 && length < 4096)
		{
			if (length < 12)
				break;

			loopLen = (HINT32)HxMACRO_Get12Bit(&(loop[10]));
			if (loopLen > length)
				break;

			loop   += (12 + loopLen);
			length -= (12 + loopLen);
		}
		RETURN_IF(length == 0, s_offset[n]);
	}
	HxLOG_Warning("event_loop syntax error\n");
	return 0;
}


static const HUINT8 s_invalidData[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

HUINT32	DLIB_EPG_RF_PARSER_EVENT_StartTime (const HUINT8 *data)
{
	HERROR	err;
	HUINT32	time;

	data += 2;

	W_RETURN_IF( memcmp(data, s_invalidData, 5) == 0, 0);

	err = HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime ((HUINT8 *)data, &time);
	W_GOTO_IF( err != ERR_OK , FAILED);

	return time;
FAILED:
	HxLOG_Debug("start_time: %02X %02X %02X %02X %02X\n", (int)data[0], (int)data[1], (int)data[2], (int)data[3], (int)data[4]);
	return 0;
}


HUINT32	DLIB_EPG_RF_PARSER_EVENT_Duration (const HUINT8 *data)
{
	HERROR	err;
	HUINT32 time;

	data += 7;

	W_RETURN_IF( memcmp(data, s_invalidData, 3) == 0, 0);

	err = HLIB_DATETIME_ConvertBcdTimeToUnixTime((HUINT8 *)data, &time);
	W_GOTO_IF( err != ERR_OK , FAILED);

	return time;
FAILED:
	HxLOG_Debug("duration: %02X %02X %02X\n", (int)data[0], (int)data[1], (int)data[2]);
	return 0;
}



