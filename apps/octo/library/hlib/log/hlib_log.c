/*
 * hxlog.c
 *
 *  Created on: 2011. 1. 17.
 */


#include <vkernel.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#include <hlib.h>

#define	OS_malloc	HLIB_MEM_Malloc
#define	OS_free		HLIB_MEM_Free
#define	OS_memset	memset
#define	OS_memcpy	memcpy

#define	OS_snprintf		snprintf
#define	OS_vsnprintf	vsnprintf
#define OS_GetPID		HLIB_STD_GetPID
#define OS_GetTID		HLIB_STD_GetTID
#define	OS_printf		Hx_Print


#define	HxLOG_DUMP_NUMBER_IN_LINE		16
#define	HxLOG_DUMP_NUMBER_IN_PAGE		0x200


#define	HxLOG_NAME_LEN			128
#define	HxLOG_DOMAIN_MAX		64
#define	HxLOG_BUF_LEN			4096
#define	HxLOG_PROCESS_MAX		24
#define	HxLOG_HIGHLIGHT_MAX		5
#define	HxLOG_HIGHLIGHT_NAME	32
#define	HxLOG_TOKEN_LEN			32

typedef struct
{
	HxLOG_Level_e	level;
	HCHAR			name[HxLOG_NAME_LEN];
	HCHAR			secondName[HxLOG_NAME_LEN];
} HxLOG_Dom_t;

typedef struct
{
	HxLOG_Level_e	glevel;
	HxLOG_Dom_t		domain[HxLOG_DOMAIN_MAX];
	HUINT32			dcount;

	HxLOG_Dom_t		process[HxLOG_PROCESS_MAX];
	HUINT32			pcount;

	HxLOG_Dom_t		processAndDomain[HxLOG_PROCESS_MAX];
	HUINT32			pdcount;

	HCHAR			highlight[HxLOG_HIGHLIGHT_MAX][HxLOG_HIGHLIGHT_NAME];
	HUINT32			hcount;

	HBOOL			trace;
	HBOOL			tick;
	HBOOL			bNoColorfulPrint;
} HxLOG_Config_t;

typedef struct
{
	HxLOG_Config_t		config;
	HxLOG_Config_t		temp;
	HUINT32			semaphore;
	HCHAR			buf[HxLOG_BUF_LEN];
	HCHAR			*token[HxLOG_TOKEN_LEN];

	HCHAR			*file;
	HUINT32			cycle;
	HUINT32			timestamp;

	time_t			ctime;
} HxLOG_Logger_t;

STATIC
HxLOG_Logger_t *	hlib_log_Get (const HCHAR *file)
{
	STATIC HxLOG_Logger_t *s_logger;

	if (s_logger == NULL)
	{
		s_logger = (HxLOG_Logger_t *)OS_malloc(sizeof(HxLOG_Logger_t));
		if (s_logger == NULL)
			return NULL;

		memset(s_logger, 0, sizeof(HxLOG_Logger_t));

		if (HxSEMT_Create(&s_logger->semaphore, "HxLOG", HxSEMT_FIFO) != 0)
		{
			OS_free(s_logger);
			s_logger = NULL;
			return NULL;
		}

		s_logger->config.glevel = HxLOG_FATAL | HxLOG_WARNING;
		s_logger->config.trace = TRUE;
		s_logger->config.tick = FALSE;
		if (file)
			s_logger->file  = HLIB_MEM_StrDup(file);
		else
		{
			s_logger->file	= HLIB_MEM_StrDup("log.config");
		}
#if defined(CONFIG_PROD_OS_EMULATOR)
		s_logger->cycle = 0x01000000; // 0x01000000sec
#else
		s_logger->cycle = 10000; // 10sec
#endif
	}
	HxSEMT_Get(s_logger->semaphore);
	return s_logger;
}

STATIC
void		hlib_log_Release (HxLOG_Logger_t *logger)
{
	HxSEMT_Release(logger->semaphore);
}

STATIC
HxLOG_Level_e	hlib_log_StringToLevel (const HCHAR *str)
{
	HxLOG_Level_e	level;

	if (str == NULL)
		return HxLOG_NONE;

	if (HLIB_STD_StrCaseCmp(str, "ALL") == 0)
		return HxLOG_ALL;

	for (level = HxLOG_NONE ; *str != '\0' ; str++)
	{
		switch (toupper(*str))
		{
		case 'P': level |= HxLOG_PRINT;    break;
		case 'M': level |= HxLOG_MESSAGE;  break;
		case 'I': level |= HxLOG_INFO;     break;
		case 'D': level |= HxLOG_DEBUG;    break;
		case 'T': level |= HxLOG_TRACE;    break;
		case 'W': level |= HxLOG_WARNING;  break;
		case 'E': level |= HxLOG_ERROR;    break;
		case 'C': level |= HxLOG_CRITICAL; break;
		case 'A': level |= HxLOG_ASSERT;   break;
		default:
			break;
		}
	}
	return level;
}


STATIC
HCHAR * 	hlib_log_LevelToString (HxLOG_Level_e lv, HCHAR *str)
{
	HCHAR *p = str;

	if (lv & HxLOG_PRINT)    *p++ = 'P';
	if (lv & HxLOG_MESSAGE)  *p++ = 'M';
	if (lv & HxLOG_INFO)     *p++ = 'I';
	if (lv & HxLOG_DEBUG)    *p++ = 'D';
	if (lv & HxLOG_TRACE)    *p++ = 'T';
	if (lv & HxLOG_WARNING)  *p++ = 'W';
	if (lv & HxLOG_ERROR)    *p++ = 'E';
	if (lv & HxLOG_CRITICAL) *p++ = 'C';
	if (lv & HxLOG_ASSERT)   *p++ = 'A';
	*p++ = '\0';

	return str;
}

STATIC
FILE *	hlib_log_OpenConfigFile (HxLOG_Logger_t *logger, const HCHAR *file)
{
#if defined(WIN32)
	//TODO
#else
	struct stat	fstat;

	if (lstat(file, &fstat) != 0)
		return NULL;

	if (logger->ctime == fstat.st_ctime)
		return NULL;
	logger->ctime = fstat.st_ctime;
#endif
	return fopen(file, "r");
}

STATIC
void		hlib_log_PrintConfig (const HxLOG_Config_t *conf)
{
	HCHAR	temp[32];
	HUINT32	i;
	const HxLOG_Dom_t 	*dom;

	OS_printf(
		HxANSI_COLOR_GREEN("******************* LOG CONFIG(%s) *******************")"\n"
		"\t   GLOBAL:%20s: %s\n"
		"\t    TRACE:%20s: %s\n"
		"\t    TICK:%20s: %s\n"
		, HLIB_STD_GetCurProcessName()
		, " ", hlib_log_LevelToString(conf->glevel, temp)
		, " ", conf->trace ? "ON" : "OFF"
		, " ", conf->tick ? "ON" : "OFF"
	);
	for (i = 0 ; i < conf->dcount ; i++)
	{
		dom = conf->domain + i;
		OS_printf("\t   DOMAIN:%20s: %s\n", dom->name, hlib_log_LevelToString(dom->level, temp));
	}
	for (i = 0 ; i < conf->pcount ; i++)
	{
		dom = conf->process + i;
		OS_printf("\t  PROCESS:%20s: %s\n", dom->name, hlib_log_LevelToString(dom->level, temp));
	}
	for (i = 0 ; i < conf->pdcount ; i++)
	{
		dom = conf->processAndDomain + i;
		OS_printf("\t  PROCESS AND DOMAIN:%10s:%10s: %s\n", dom->name, dom->secondName,hlib_log_LevelToString(dom->level, temp));
	}
	for (i = 0 ; i < conf->hcount ; i++)
	{
		OS_printf("\tHIGHLIGHT:%20s\n", conf->highlight[i]);
	}
}

STATIC
HxLOG_Config_t *	hlib_log_LoadConfig (HxLOG_Logger_t *logger, HxLOG_Config_t *config, const HCHAR *file)
{
	HxLOG_Dom_t 		*item;
	HCHAR			*buf;
	HCHAR			*key;
	HCHAR			*save;
	FILE			*fp;
	HINT32			i, n;

	buf = logger->buf;

#ifdef	CONFIG_TARGET_ROOTFS_SQUASHFS
	snprintf(buf, HxLOG_BUF_LEN, CONFIG_PRODUCT_DATA_DIR "/%s", file);
	fp = hlib_log_OpenConfigFile(logger, buf);
#else
	fp = hlib_log_OpenConfigFile(logger, file);
#endif
	if (!fp)
	{
		return NULL;
	}

	memset(config, 0, sizeof(HxLOG_Config_t));

	while (fgets(buf, HxLOG_BUF_LEN, fp))
	{
		memset(logger->token, 0, sizeof(HCHAR *) * HxLOG_TOKEN_LEN);

		key = HxSTD_StrTok(buf, " \n\t:=,", &save);
		for (n = 0 ; key && n < HxLOG_TOKEN_LEN ; n++)
		{
			while (*key != '\0' && !isgraph(*key))	//trim non ascii or control characters
				key++;
			logger->token[n] = key;
			key = HxSTD_StrTok(NULL, " \n\t:=,", &save);
		}
		if (n == 0)
			continue;

		if (HLIB_STD_StrStartWith(logger->token[0], "#"))
		{
			continue;	// skip comment
		}

		if (HLIB_STD_StrCmpV(HLIB_STD_StrCaseCmp, logger->token[0], "@GLOBAL", "GLOBAL", NULL) >= 0)
		{
			config->glevel = hlib_log_StringToLevel(logger->token[1]);
		}
		else if (HLIB_STD_StrCaseCmp(logger->token[0], "@HIGHLIGHT") == 0)
		{
			for (i = 1 ; i < n && config->hcount < HxLOG_HIGHLIGHT_MAX ; i++)
			{
				HLIB_STD_StrNCpySafe(config->highlight[config->hcount++], logger->token[i], HxLOG_HIGHLIGHT_NAME);
			}
		}
		else if (HLIB_STD_StrCaseCmp(logger->token[0], "@NOCOLOR") == 0)
		{
			config->bNoColorfulPrint = TRUE;
		}
		else if (HLIB_STD_StrCaseCmp(logger->token[0], "@TRACE") == 0)
		{
			if (HLIB_STD_StrCmpV(HLIB_STD_StrCaseCmp, logger->token[1], "TRUE", "YES", "ON", "Y", NULL) >= 0)
			{
				config->trace = TRUE;
			}
		}
		else if (HLIB_STD_StrCaseCmp(logger->token[0], "@TICK") == 0)
		{
			if (HLIB_STD_StrCmpV(HLIB_STD_StrCaseCmp, logger->token[1], "TRUE", "YES", "ON", "Y", NULL) >= 0)
			{
				config->tick = TRUE;
			}
		}
		else if (HLIB_STD_StrStartWith(logger->token[0], "@") && config->pcount < HxLOG_PROCESS_MAX)
		{
			item = config->process + config->pcount++;
			HLIB_STD_StrCaseCpy(item->name, logger->token[0] + 1, TRUE);
			item->level = hlib_log_StringToLevel(logger->token[1]);
		}
		else if (HLIB_STD_StrStartWith(logger->token[0], "$") && config->pdcount < HxLOG_PROCESS_MAX)
		{
			item = config->processAndDomain + config->pdcount++;
			HLIB_STD_StrCaseCpy(item->name, logger->token[0] + 1, TRUE);	// process
			HLIB_STD_StrCaseCpy(item->secondName, logger->token[1], TRUE);	// domain
			item->level = hlib_log_StringToLevel(logger->token[2]);
		}
		else if (config->dcount < HxLOG_DOMAIN_MAX)
		{
			item = config->domain + config->dcount++;
			HLIB_STD_StrCaseCpy(item->name, logger->token[0], TRUE);
			item->level = hlib_log_StringToLevel(logger->token[1]);
		}
	}
	fclose(fp);

	return config;
}

STATIC
void	hlib_log_UpdateConfig (HxLOG_Logger_t *logger)
{
	HUINT32 tick;

	tick = HLIB_STD_GetSystemTick();
	if (tick - logger->timestamp >= logger->cycle)
	{
		HxLOG_Config_t *newConf;

		logger->timestamp = tick;

		newConf = hlib_log_LoadConfig(logger, &logger->temp, logger->file);
		if (newConf && memcmp(&logger->config, newConf, sizeof(HxLOG_Config_t)) != 0)
		{
			memcpy(&logger->config, newConf, sizeof(HxLOG_Config_t));
			hlib_log_PrintConfig(newConf);
		}
	}
}

STATIC
const HBOOL	hlib_log_IsPrintable (HxLOG_Logger_t *logger, const HCHAR *path, HxLOG_Level_e lv, HCHAR *domain, HUINT32 n)
{
	HUINT32		i;
	HxLOG_Dom_t	*dom;
	HxLOG_Level_e dlv = HxLOG_NONE;
	HxLOG_Level_e plv = HxLOG_NONE;

	hlib_log_UpdateConfig(logger);
	domain[0] = '\0';

	for (i = 0 ; i < logger->config.dcount ; i++)
	{
		dom = logger->config.domain + i;
		if (HLIB_STD_StrCaseStr(path, dom->name))
		{
			if (!(dom->level & lv))
				return FALSE;
			dlv = dom->level;
			HLIB_STD_StrNCpySafe(domain, dom->name, n);
			break;
		}
	}

	for (i = 0 ; i < logger->config.pcount ; i++)
	{
		dom = logger->config.process + i;
		if (HLIB_STD_StrCaseCmp(HLIB_STD_GetCurProcessName(), dom->name) == 0)
		{
			if (!(dom->level & lv))
				return FALSE;
			plv = dom->level;
			break;
		}
	}

	for (i = 0 ; i < logger->config.pdcount ; i++)
	{
		dom = logger->config.processAndDomain + i;
		if (HLIB_STD_StrCaseCmp(HLIB_STD_GetCurProcessName(), dom->name) == 0
			&& HLIB_STD_StrCaseStr(path, dom->secondName))
		{
			if (!(dom->level & lv))
				return FALSE;
			plv = dom->level;
			break;
		}
	}

	if (dlv & lv || plv & lv || logger->config.glevel & lv)
	{
		return TRUE;
	}
	return FALSE;
}

STATIC
void	hlib_log_Print (HxLOG_Logger_t *logger, const HCHAR *dmn, HxLOG_Level_e lv, const HCHAR *fmt, va_list ap, HBOOL highlight, const HUINT32 ulTick, const HCHAR *function, HUINT32 line)
{
#define	HxLOG_LOCAL_BUF_LEN	512
	HCHAR		lvstr[32];
	HCHAR		pname[12];
	HCHAR		buf[HxLOG_LOCAL_BUF_LEN];
	HINT32		written;
	HCHAR		*log, *garbage = NULL;
	HINT32		pid;

	written = OS_vsnprintf(buf, HxLOG_LOCAL_BUF_LEN, fmt, ap);
	if (written < 0 || HxLOG_LOCAL_BUF_LEN <= written)
	{
		garbage = log = (HCHAR *)HLIB_MEM_Malloc(HxLOG_BUF_LEN);
		if (log == NULL)
		{
			hlib_log_Release(logger);
			return;
		}
		OS_vsnprintf(log, HxLOG_BUF_LEN, fmt, ap);
	} else  log = buf;

	HLIB_STD_StrNCpySafe(pname, HLIB_STD_GetCurProcessName(), 12);
	pid   = OS_GetPID();

	if (highlight)
	{
		if (lv == HxLOG_PRINT)
		{
			if(logger->config.bNoColorfulPrint == TRUE)
			{
				OS_printf(HxANSI_CODE("%s", HxANSI_CODE_UNDERLINED, HxANSI_COLOR_FG_YELLOW), log);
			}
			else
			{
				OS_printf("%s",log);
			}
		}
		else
		{
			if (function)
			{
				if(0 == ulTick)
				{
					if(logger->config.bNoColorfulPrint == FALSE)
					{
		
						OS_printf(HxANSI_CODE("%11s(%4d);%s;<%s:%d>;", HxANSI_CODE_UNDERLINED, HxANSI_COLOR_FG_YELLOW)" %s"
							, pname, pid, hlib_log_LevelToString(lv, lvstr), function, line, log
						);
					}
					else
					{
						OS_printf("%11s(%4d);%s;<%s:%d>; %s", pname, pid, hlib_log_LevelToString(lv, lvstr), function, line, log);
					}
				}
				else
				{
					if(logger->config.bNoColorfulPrint == FALSE)
					{
						OS_printf(HxANSI_CODE("%11s(%4d);%s;[%d] <%s:%d>;", HxANSI_CODE_UNDERLINED, HxANSI_COLOR_FG_YELLOW)" %s"
							, pname, pid, hlib_log_LevelToString(lv, lvstr), ulTick, function, line, log
						);
					}
					else
					{
						OS_printf("%11s(%4d);%s;[%d] <%s:%d>; %s", pname, pid, hlib_log_LevelToString(lv, lvstr), ulTick, function, line, log);
					}
				}
			}
			else
			{
				if(logger->config.bNoColorfulPrint == FALSE)
				{
					OS_printf(HxANSI_CODE("%11s(%4d);%s;", HxANSI_CODE_UNDERLINED, HxANSI_COLOR_FG_YELLOW)" %s"
						, pname, pid, hlib_log_LevelToString(lv, lvstr), log
					);
				}
				else
				{
					OS_printf("%11s(%4d);%s; %s", pname, pid, hlib_log_LevelToString(lv, lvstr), log);
				}
			}
		}
		if (garbage)
			HLIB_MEM_Free(garbage);
		return;
	}

	switch (lv)
	{
	case HxLOG_PRINT:
		if (function)
		{
			if(0 == ulTick)
				OS_printf("<%s:%d>; %s", function, line, log);
			else
				OS_printf("[%d] <%s:%d>; %s", ulTick, function, line, log);
		}
		else
			OS_printf("%s", log);
		break;

	case HxLOG_WARNING:
		if (function)
		{
			if(0 == ulTick)
			{
				if(logger->config.bNoColorfulPrint == FALSE)
				{
					OS_printf(HxANSI_COLOR_CYAN("%11s(%4d);%s;<%s:%d>;")" %s", pname, OS_GetTID(), "W", function, line, log);
				}
				else
				{
					OS_printf("%11s(%4d);%s;<%s:%d>; %s", pname, OS_GetTID(), "W", function, line, log);
				}
			}
			else
			{
				if(logger->config.bNoColorfulPrint == FALSE)
				{
					OS_printf(HxANSI_COLOR_CYAN("%11s(%4d);%s;[%d] <%s:%d>;")" %s", pname, OS_GetTID(), "W", ulTick, function, line, log);
				}
				else
				{
					OS_printf("%11s(%4d);%s;[%d] <%s:%d>; %s", pname, OS_GetTID(), "W", ulTick, function, line, log);
				}
			}
		}
		else
		{
			if(logger->config.bNoColorfulPrint == FALSE)
			{
				OS_printf(HxANSI_COLOR_CYAN("%11s(%4d);%s;")" %s", pname, OS_GetTID(), "W", log);
			}
			else
			{
				OS_printf("%11s(%4d);%s; %s", pname, OS_GetTID(), "W", log);
			}
		}
		break;

	case HxLOG_ERROR:
	case HxLOG_CRITICAL:
	case HxLOG_ASSERT:
		if (function)
		{
			if(0 == ulTick)
			{
				if(logger->config.bNoColorfulPrint == FALSE)
				{
					OS_printf(HxANSI_COLOR_RED("%11s(%4d);%s;<%s:%d>;")" %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), function, line, log);
				}
				else
				{
					OS_printf("%11s(%4d);%s;<%s:%d>; %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), function, line, log);
				}
			}
			else
			{
				if(logger->config.bNoColorfulPrint == FALSE)
				{
					OS_printf(HxANSI_COLOR_RED("%11s(%4d);%s;[%d] <%s:%d>;")" %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), ulTick, function, line, log);
				}
				else
				{
					OS_printf("%11s(%4d);%s;[%d] <%s:%d>; %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), ulTick, function, line, log);
				}
			}
		}
		else
		{
			if(logger->config.bNoColorfulPrint == FALSE)
			{
				OS_printf(HxANSI_COLOR_RED("%11s(%4d);%s;")" %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), log);
			}
			else
			{
				OS_printf("%11s(%4d);%s; %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), log);
			}
		}
		break;

	default:
		if (function)
		{
			if(0 == ulTick)
				OS_printf("%11s(%4d);%s;<%s:%d>; %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), function, line, log);
			else
				OS_printf("%11s(%4d);%s;[%d] <%s:%d>; %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), ulTick, function, line, log);
		}
		else
			OS_printf("%11s(%4d);%s; %s", pname, OS_GetTID(), hlib_log_LevelToString(lv, lvstr), log);
		break;
	}
	if (garbage)
		HLIB_MEM_Free(garbage);

	hlib_log_Release(logger);
}

void HLIB_LOG_Init (HBOOL bTemp)
{
	HxLOG_Logger_t *pstLogger;

	(void)bTemp;
	pstLogger = hlib_log_Get(NULL);
	if (pstLogger)
	{
		hlib_log_Release(pstLogger);
	}
}


void HLIB_LOG_Debug(const HCHAR *file, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv, const HCHAR *format, ...)
{
	static FILE	*__file = NULL;
	const HCHAR *p;
	HCHAR	*q, szBuf[1024];
	va_list ap;

	if (__file == NULL)
	{
		static	HCHAR	filename[64];

		snprintf(filename, 64, "%s.log", HLIB_STD_GetCurProcessName());
		__file = fopen(filename, "wt");
	}

	p = format;
	q = szBuf;
	while (*p)
	{
		if ((*p == '%') && (*(p+1) == 's'))
		{
			*q++ = *p++;
			*q++ = 'p';	*p++;
			*q++ = '~';
		} else
			*q++ = *p++;
	}
	*q = '\0';

	va_start(ap, format);
	vfprintf(__file, szBuf, ap);
	fflush(__file);
	va_end(ap);

}

void HLIB_LOG (const HCHAR *file, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv, const HCHAR *format, ...)
{
	va_list	ap;

	va_start(ap, format);
	HLIB_LOG_v(file, function, line, lv, format, ap);
	va_end(ap);
}


void	HLIB_LOG_v (const HCHAR *path, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv, const HCHAR *fmt, va_list ap)
{
	HxLOG_Logger_t	*logger;
	HCHAR		domain[64];
	HBOOL		printable;
	HBOOL		highlight = FALSE;
	HUINT32		i;
	HUINT32		ulTick;


	logger = hlib_log_Get(NULL);
	if (logger == NULL)
	{
		return;
	}

	printable = hlib_log_IsPrintable(logger, path, lv, domain, 64);
	if (!printable)
	{
		hlib_log_Release(logger);
		return;
	}

	for (i = 0 ; i < logger->config.hcount ; i++)
	{
		if (HLIB_STD_StrCaseStr(fmt, logger->config.highlight[i])
			|| HLIB_STD_StrCaseStr(function, logger->config.highlight[i]))
		{
			highlight = TRUE;
			break;
		}
	}

	if (!logger->config.trace)
	{
		function = NULL;
	}

	if (!logger->config.tick)
	{
		ulTick = 0;
	}
	else
	{
		ulTick = HLIB_STD_GetSystemTick();
	}


	hlib_log_Print(logger, domain, lv, fmt, ap, highlight, ulTick, function, line);
	hlib_log_Release(logger);

	return;
}

void HLIB_LOG_DumpEx (HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail, FILE *file)
{
	HUINT32		unChar, unCount, unNumOfDump;
	HUINT32		ullAddr = 0;
	HUINT8		aucChar[HxLOG_DUMP_NUMBER_IN_LINE + 1];
	int			i;
	HxLOG_Logger_t	*pLog;

	pLog = hlib_log_Get(NULL);
	if (pLog)
	{
		VK_memset (aucChar, 0, HxLOG_DUMP_NUMBER_IN_LINE + 1);

		i = 0;
		unNumOfDump = 0;

		ullAddr += unBaseAddr;

		while (unNumOfDump < unSize)
		{
			unChar = 0x000000ff & (*pucBuffer++);

			if ((unChar >= 0x20) && (unChar <= 0x7e))
				aucChar[i++] = (HUINT8)unChar;
			else
				aucChar[i++] = '.';

			if ((ullAddr % HxLOG_DUMP_NUMBER_IN_PAGE) == 0)
			{
				if (bFlagDetail)
					fprintf (file, "\n-----------------------------------------------\n");
				else
				{
					fprintf (file, "\n\n\n"
								"\t\t\t00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"
								"\t\t\t-----------------------------------------------\n"
								"\t\t\t |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |"
							);
				}
			}

			if ((ullAddr % HxLOG_DUMP_NUMBER_IN_LINE) == 0)
			{
				if (bFlagDetail)
					fprintf (file, "\n");
				else
					fprintf (file, "\n\t%8x : \t", ullAddr);

			}

			fprintf (file, "%02x ", unChar);

			if ((ullAddr % HxLOG_DUMP_NUMBER_IN_LINE) == (HxLOG_DUMP_NUMBER_IN_LINE - 1))
			{
				fprintf (file, "\t%s", aucChar);
				i = 0;
			}

			ullAddr++;
			unNumOfDump++;
		}

		unCount = ullAddr % HxLOG_DUMP_NUMBER_IN_LINE;

		aucChar[i] = 0;
		if (unCount != 0)
		{
			for (i = unCount; i < HxLOG_DUMP_NUMBER_IN_LINE; ++i)
				fprintf (file, "   ");
		}

		fprintf (file, "\t%s\n", aucChar);

		fflush(file);
		hlib_log_Release(pLog);
	}
}

void HLIB_LOG_Dump (HUINT8 *pucBuffer, HUINT32 unSize, HUINT32 unBaseAddr, HUINT8 bFlagDetail)
{
	HUINT32		unChar, unCount, unNumOfDump;
	HUINT32		ullAddr = 0;
	HUINT8		aucChar[HxLOG_DUMP_NUMBER_IN_LINE + 1];
	int			i;
	HxLOG_Logger_t	*pLog;

	pLog = hlib_log_Get(NULL);
	if (pLog)
	{
		VK_memset (aucChar, 0, HxLOG_DUMP_NUMBER_IN_LINE + 1);

		i = 0;
		unNumOfDump = 0;

		ullAddr += unBaseAddr;

		while (unNumOfDump < unSize)
		{
			unChar = 0x000000ff & (*pucBuffer++);

			if ((unChar >= 0x20) && (unChar <= 0x7e))
				aucChar[i++] = (HUINT8)unChar;
			else
				aucChar[i++] = '.';

			if ((ullAddr % HxLOG_DUMP_NUMBER_IN_PAGE) == 0)
			{
				if (bFlagDetail)
					OS_printf ("\n-----------------------------------------------\n");
				else
				{
					OS_printf(	"\n\n\n"
								"\t\t\t00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n"
								"\t\t\t-----------------------------------------------\n"
								"\t\t\t |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |"
							);
				}
			}

			if ((ullAddr % HxLOG_DUMP_NUMBER_IN_LINE) == 0)
			{
				if (bFlagDetail)
					OS_printf ("\n");
				else
					OS_printf ("\n\t%8x : \t", ullAddr);

			}

			OS_printf ("%02x ", unChar);

			if ((ullAddr % HxLOG_DUMP_NUMBER_IN_LINE) == (HxLOG_DUMP_NUMBER_IN_LINE - 1))
			{
				OS_printf ("\t%s", aucChar);
				i = 0;
			}

			ullAddr++;
			unNumOfDump++;
		}

		unCount = ullAddr % HxLOG_DUMP_NUMBER_IN_LINE;

		aucChar[i] = 0;
		if (unCount != 0)
		{
			for (i = unCount; i < HxLOG_DUMP_NUMBER_IN_LINE; ++i)
				OS_printf ("   ");
		}

		OS_printf ("\t%s\n", aucChar);
		hlib_log_Release(pLog);
	}
}

// TODO: delete
void	HLIB_LOG_SetLevel	(const HCHAR *domain, HxLOG_Level_e lv)
{
	(void)domain;
	(void)lv;
}

HxLOG_Level_e	HLIB_LOG_GetLevel	(const HCHAR *domain)
{
	(void)domain;
	return HxLOG_NONE;
}

void	HLIB_LOG_SetHandler (const HCHAR *domain, HxLOG_Cb_t handler, void *userdata)
{
	(void)domain;
	(void)handler;
	(void)userdata;
}

void	HLIB_LOG_SetGlobalLevel (HxLOG_Level_e lv)
{
	(void)lv;
}


void HLIB_LOG_PrintDomain(void)
{
	HUINT32		i = 0;
	HCHAR 		str[16] = {0, };
	HxLOG_Logger_t	*logger = NULL;

	logger = hlib_log_Get(NULL);
	if (logger)
	{
		OS_printf("=============================================================\r\n");
		OS_printf("   HLIB_LOG info\n");
		OS_printf("-------------------------------------------------------------\r\n");
		hlib_log_LevelToString(logger->config.glevel, str);
		OS_printf("+ global: \r\n");
		OS_printf("\t - %9s\r\n", str);
		OS_printf("+ domain(%d): \r\n", logger->config.dcount);
		for (i = 0 ; i < logger->config.dcount ; i++)
		{
			hlib_log_LevelToString(logger->config.domain[i].level, str);
			OS_printf("\t - %3d. %9s %s\r\n", i, str, logger->config.domain[i].name);
		}
		OS_printf("=============================================================\r\n");

		hlib_log_Release(logger);
	}
}


#if HxLOG_FILE_WITHOUT_PATH == 1 && defined(__FILE__)
const char *	HLIB_LOG_RemovePath(const char *file)
{
	char *ch;

	if (file == NULL)
		return file;

	ch = strrchr(file, (int)'/');
	if (ch == NULL)
	{
#if defined(WIN32)
		ch = strrchr(file, (int)'\\');
		if (ch == NULL)
#endif
		return file;
	}

	return ++ch;
}
#endif

void	HLIB_LOG_ErinPrint(const HCHAR *file, const HCHAR *function, HUINT32 line, HxLOG_Level_e lv,HINT32 err_code, const HCHAR *format, ...)
{
	va_list	ap;

	(void)err_code;

	va_start(ap, format);
	HLIB_LOG_v(file, function, line, lv, format, ap);
	va_end(ap);
}


