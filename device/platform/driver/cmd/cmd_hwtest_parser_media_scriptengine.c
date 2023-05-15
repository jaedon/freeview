#if defined(CONFIG_VK_STDLIB)
#else
#include<stdio.h>
#include<string.h>
#include<malloc.h>
#endif

#include "vkernel.h"

#include"cmd_hwtest_parser_media_scriptengine.h"
/* rev 0.2 : #loop, #loop-end 추가 */

/* 순서 변경 금지 */
/* CMD_MEDIA_COM_e 과 순서가 같아야함 */
const CMD_MEDIA_TAG_TABLE_t g_TagTable[CMD_MEDIA_COM_MAX-1] =
{
	{CMD_MEDIA_TAG_TEST,1},
	{CMD_MEDIA_TAG_TESTEND,1},
	{CMD_MEDIA_TAG_ASSET,1},
	{CMD_MEDIA_TAG_ASSETEND,1},
	{CMD_MEDIA_TAG_COMMAND,1},
	{CMD_MEDIA_TAG_COMMANDEND,1},
	{CMD_MEDIA_TAG_CREATE,1},
	{CMD_MEDIA_TAG_STARTBUF,1},
	{CMD_MEDIA_TAG_START,1},
	{CMD_MEDIA_TAG_STOP,1},
	{CMD_MEDIA_TAG_STOPBUF,1},
	{CMD_MEDIA_TAG_DESTROY,1},
	{CMD_MEDIA_TAG_SEEK,1},
	{CMD_MEDIA_TAG_TRICK,1},
	{CMD_MEDIA_TAG_DELAY,1},
	{CMD_MEDIA_TAG_LOOP,1},
	{CMD_MEDIA_TAG_LOOPEND,1},
	{CMD_MEDIA_TAG_WAIT,1},
	{CMD_MEDIA_TAG_LOG,1},
	{CMD_MEDIA_TAG_SETAUDIO,1},
};

static unsigned long long _atohui64(char *nptr)
{
	char *s = (char *)nptr;
	unsigned long long acc = 0;
	while(isspace((int)*s))
		s++;
	while (VK_isdigit((int)*s))
	{
		acc = 10 * acc + ((int)*s - '0');
		s++;
	}
	return acc;
}
static CMD_MEDIA_ERR_e P_MEDIA_Script_GetTag(const char * buffer, char * found)
{
	char *p = NULL;
	int i=0,result=0;
	p = (char*)buffer;
	if ( p )
	{
		for(i =0; i< (int)VK_strlen(p); i++ )
		{
			result = VK_strncmp(p+/*skip+*/i, ":" ,1);
			if ( result == 0 )
			{
				break;
			}
			result = VK_strncmp(p+/*skip+*/i, "\n", 1);
			if ( result == 0 )
			{
				break;
			}
			result = VK_strncmp(p+/*skip+*/i, "\0", 1);
			if ( result == 0 )
			{
				break;
			}
		}
		VK_strncpy(found, p, i);
		return CMD_MEDIA_ERR_OK;
	}
	else
	{
		return CMD_MEDIA_ERR_FAIL;
	}
}
static int P_MEDIA_Script_GetValue(const char * buffer, char * niddle, char * found)
{
	char *p = NULL;
	int i=0,skip=0,result=0;
	short bOpen = 1;
	p = (char*)VK_strstr((const char*)buffer,(const char*)niddle);
	if ( p )
	{
		for(i =0; i< (int)VK_strlen(p); i++ )
		{
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "\"" ,1);
			if ( result == 0 )
			{
				skip++;
				i++;
				for(; i<(int)VK_strlen(p); i++ )
				{
					result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "\"" ,1);
					if ( result == 0 )
					{
						break;
					}
				}
				if ( result == 0 )
				{
					break;
				}
				else
					continue;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "=" ,1);
			if ( result == 0 )
			{
				if(bOpen == 1)
				{
					skip++;
					bOpen = 0;
				}
				continue;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), ":" ,1);
			if ( result == 0 )
			{
				if(bOpen == 1)
				{
					skip++;
					bOpen = 0;
				}
				continue;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "," ,1);
			if ( result == 0 )
			{
				break;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "\n", 1);
			if ( result == 0 )
			{
				break;
			}
			result = VK_strncmp(p+/*skip+*/i+VK_strlen(niddle), "\0", 1);
			if ( result == 0 )
			{
				break;
			}
			if ( p+/*skip+*/i+VK_strlen(niddle) == NULL )
			{
				return 1;
			}
		}
		VK_strncpy(found, p+skip+VK_strlen(niddle), i-skip);
		return 0;
	}
	else
	{
		return 1;
	}
}

static int P_MEDIA_Script_ReadLine(char** pLine, char* pBuf, unsigned int unSize)
{
	char *p = pBuf;
	unsigned int unLineLen = 0;
	char *pEnd = NULL;
	if(p == NULL)
	{
		*pLine = NULL;
		return 0;
	}
	pEnd = p + unSize;
	while(p < pEnd)
	{
		if( *p == '\n' || *p == '\r')
		{
			if(unLineLen == 0)
			{
				*pLine = NULL;
				return 1;
			}
			unLineLen++;
			break;
		}
		else
		{
			p++;
			unLineLen++;
		}
	}
	if(unLineLen == 0)
	{
		p++;
		*pLine = NULL;
		return 0;
	}
	*pLine = VK_malloc(unLineLen+1);
	VK_memset(*pLine,0x00,unLineLen+1);
	VK_memcpy(*pLine, pBuf, p - pBuf);
	return unLineLen;
}

static CMD_MEDIA_COM_e P_MEDIA_Script_IdentifyTag(char* pLine)
{
	int i = 0;
	CMD_MEDIA_COM_e eTag = CMD_MEDIA_COM_MAX;
	for(i = 0 ; i < CMD_MEDIA_COM_MAX-1 ; i++)
	{
		if(VK_strlen(g_TagTable[i].cTagName) == VK_strlen(pLine))
		{
			if(!VK_strncmp(pLine,g_TagTable[i].cTagName,VK_strlen(g_TagTable[i].cTagName)))
			{
				eTag = i;
				break;
			}
		}
	}
	return eTag;
}

static CMD_MEDIA_ScriptParser_t g_script;

CMD_MEDIA_ERR_e CMD_MEDIA_Script_Init(char *pBuf,unsigned int unSize)
{
	if(g_script.bInit == 1)
	{
		return CMD_MEDIA_ERR_FAIL;
	}
	g_script.bInit = 1;
	g_script.pBuf = pBuf;
	g_script.p = pBuf;
	g_script.unBufSize = unSize;
	g_script.unLeftSize = unSize;
	g_script.bAssetOpened = 0;
	g_script.bCmdOpened = 0;
	g_script.bTestOpened = 0;
	return CMD_MEDIA_ERR_OK;
}

CMD_MEDIA_ERR_e CMD_MEDIA_Script_Uninit()
{
	if(g_script.bInit == 0)
	{
		return CMD_MEDIA_ERR_FAIL;
	}
	g_script.bInit = 0;
	g_script.pBuf = NULL;
	g_script.p = NULL;
	g_script.unBufSize = 0;
	g_script.unLeftSize = 0;
	g_script.bAssetOpened = 0;
	g_script.bCmdOpened = 0;
	g_script.bTestOpened = 0;
	g_script.pLoop = NULL;
	g_script.bLoopOpened = 0;
	g_script.unLoopCount = 0;
	g_script.unLoopStartOffset = 0;
	return CMD_MEDIA_ERR_OK;
}
CMD_MEDIA_ERR_e CMD_MEDIA_Script_GetLine(CMD_MEDIA_LINE_t *pData)
{
	char value[512] = {0};
	unsigned int unLineSize = 0;
	CMD_MEDIA_ERR_e nRet = CMD_MEDIA_ERR_OK;
	CMD_MEDIA_COM_e eTag = CMD_MEDIA_COM_MAX;
	char *pLine = NULL;
	if( g_script.bInit == 0)
	{
		return -1;
	}
retry_parse:
	if( g_script.unLeftSize > 0 && ((unLineSize = P_MEDIA_Script_ReadLine(&pLine, g_script.p, g_script.unLeftSize)) > 0) )
	{
		eTag = CMD_MEDIA_COM_MAX;
		g_script.p += unLineSize;
		g_script.unLeftSize -= unLineSize;
		if(pLine == NULL)
		{
			if( g_script.unLeftSize == 0 )
			{
				nRet = CMD_MEDIA_ERR_EOF;
			}
			else
			{
				goto retry_parse;
			}
		}
		else
		{
			VK_memset(value,0x00,512);
			if(pLine[0] == '/' && pLine[1] == '/')
			{
				if(pLine != NULL)
				{
					VK_free(pLine);
				}
				pLine = NULL;
				goto retry_parse;
			}
			if(P_MEDIA_Script_GetTag(pLine,value)==CMD_MEDIA_ERR_FAIL)
			{
				if(pLine != NULL)
				{
					VK_free(pLine);
				}
				pLine = NULL;
				return CMD_MEDIA_ERR_SYNTAX;
			}
			eTag = P_MEDIA_Script_IdentifyTag(value);
			pData->eCmd = eTag;
			/* Parsing */
			switch(eTag)
			{
				case CMD_MEDIA_COM_TEST:
					if(g_script.bTestOpened == 0)
					{
						g_script.bTestOpened = 1;
					}
					else
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					break;
				case CMD_MEDIA_COM_TESTEND:
					if(g_script.bTestOpened == 1)
					{
						g_script.bTestOpened = 0;
					}
					else
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					break;
				case CMD_MEDIA_COM_ASSET:
					if(g_script.bTestOpened == 1 && g_script.bAssetOpened == 0)
					{
						g_script.bAssetOpened = 1;
					}
					else
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					break;
				case CMD_MEDIA_COM_ASSETEND:
					if(g_script.bTestOpened == 1 && g_script.bAssetOpened == 1)
					{
						g_script.bAssetOpened = 0;
					}
					else
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					break;
				case CMD_MEDIA_COM_COMMAND:
					if(g_script.bTestOpened == 1 && g_script.bCmdOpened == 0)
					{
						g_script.bCmdOpened = 1;
					}
					else
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					break;
				case CMD_MEDIA_COM_COMMANDEND:
					if(g_script.bTestOpened == 1 && g_script.bCmdOpened == 1)
					{
						g_script.bCmdOpened = 0;
					}
					else
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					break;
				case CMD_MEDIA_COM_CREATE:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_NUM,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->param2 = (unsigned long long)_atohui64(value);
						}
					}
					break;
				case CMD_MEDIA_COM_STARTBUF:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ID,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->id = (unsigned int)VK_atoi(value);
						}
					}
					break;
				case CMD_MEDIA_COM_START:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ID,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->id = (unsigned int)VK_atoi(value);
						}
					}
					break;
				case CMD_MEDIA_COM_STOP:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ID,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->id = (unsigned int)VK_atoi(value);
						}
					}
					break;
				case CMD_MEDIA_COM_STOPBUF:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ID,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->id = (unsigned int)VK_atoi(value);
						}
					}
					break;
				case CMD_MEDIA_COM_DESTROY:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					break;
				case CMD_MEDIA_COM_SETAUDIO:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ID,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->id = (unsigned int)VK_atoi(value);
						}
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_NUM,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->param1 = (unsigned int)VK_atoi(value);
						}
					}
					break;
				case CMD_MEDIA_COM_SEEK:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ID,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->id = (unsigned int)VK_atoi(value);
						}
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ORIGIN,(char*)value) != 0)
						{
							pData->param3 = CMD_MEDIA_SEEKDIR_START;
						}
						else
						{
							if(!VK_strncmp(value,CMD_MEDIA_VALUE_START,VK_strlen(CMD_MEDIA_VALUE_START)))
							{
								pData->param3 = CMD_MEDIA_SEEKDIR_START ;
							}
							else if(!VK_strncmp(value,CMD_MEDIA_VALUE_FWD,VK_strlen(CMD_MEDIA_VALUE_FWD)))
							{
								pData->param3 = CMD_MEDIA_SEEKDIR_CURFW;
							}
							else if(!VK_strncmp(value,CMD_MEDIA_VALUE_BWD,VK_strlen(CMD_MEDIA_VALUE_BWD)))
							{
								pData->param3 = CMD_MEDIA_SEEKDIR_CURBW ;
							}
							else if(!VK_strncmp(value,CMD_MEDIA_VALUE_END,VK_strlen(CMD_MEDIA_VALUE_END)))
							{
								pData->param3 = CMD_MEDIA_SEEKDIR_END ;
							}
							else if(!VK_strncmp(value,CMD_MEDIA_VALUE_RAND,VK_strlen(CMD_MEDIA_VALUE_RAND)))
							{
								pData->param3 = CMD_MEDIA_SEEKDIR_RAND ;
							}
							else
							{
								pData->param3 = CMD_MEDIA_SEEKDIR_START;
							}
						}
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_TIME,(char*)value) != 0)
						{
							if(pData->param3 != CMD_MEDIA_SEEKDIR_RAND)
							{
								nRet = CMD_MEDIA_ERR_SYNTAX;
							}
						}
						else
						{
							pData->param2 = (unsigned long long)_atohui64(value);
						}
					}
					break;
				case CMD_MEDIA_COM_TRICK:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ID,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->id = (unsigned int)VK_atoi(value);
						}
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_SPEED,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->param1 = VK_atoi(value);
						}
					}
					break;
				case CMD_MEDIA_COM_DELAY:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_TIME,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->param2 = (unsigned long long)_atohui64(value);
						}
					}
					break;
				case CMD_MEDIA_COM_LOOP:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 || g_script.bLoopOpened == 1)
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_TIME,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							g_script.pLoop = g_script.p;
							g_script.unLoopCount = VK_atoi(value);
							g_script.bLoopOpened = 1;
							g_script.unLoopStartOffset = g_script.unLeftSize;
							if(pLine != NULL)
							{
								VK_free(pLine);
							}
							pLine = NULL;
							goto retry_parse;
						}
					}
					break;
				case CMD_MEDIA_COM_LOOPEND:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 || g_script.bLoopOpened != 1)
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(g_script.unLoopCount <= 1)
						{
							g_script.pLoop = NULL;
							g_script.bLoopOpened = 0;
							g_script.unLoopCount = 0;
							g_script.unLoopStartOffset = 0;
						}
						else
						{
							g_script.unLoopCount--;
							g_script.p = g_script.pLoop;
							g_script.unLeftSize = g_script.unLoopStartOffset;
						}
						if(pLine != NULL)
						{
							VK_free(pLine);
						}
						pLine = NULL;
						goto retry_parse;
					}
					break;
				case CMD_MEDIA_COM_WAIT:
					if(g_script.bTestOpened != 1 || g_script.bCmdOpened != 1 || g_script.bAssetOpened == 1 )
					{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_ATTR_ID,(char*)value) != 0)
						{
							nRet = CMD_MEDIA_ERR_SYNTAX;
						}
						else
						{
							pData->id = (unsigned int)VK_atoi(value);
						}
					}
					break;
				case CMD_MEDIA_COM_LOG:
					if(P_MEDIA_Script_GetValue((const char*)pLine,CMD_MEDIA_TAG_LOG,(char*)value) != 0)
        	               		{
						nRet = CMD_MEDIA_ERR_SYNTAX;
					}
					else
					{
						pData->param4 = strdup(value);
					}
					break;
				case CMD_MEDIA_COM_MAX:
					if(g_script.bAssetOpened == 1)
					{
						pData->eCmd = CMD_MEDIA_COM_URL;
						pData->param4 = strdup(pLine);
					}
					else
					{
						if(pLine != NULL)
						{
							VK_free(pLine);
						}
						pLine = NULL;
						goto retry_parse;
					}
					break;
				default :
					if(pLine != NULL)
					{
						VK_free(pLine);
					}
					pLine = NULL;
					goto retry_parse;
			}
		}
	}
	else
	{
		nRet = CMD_MEDIA_ERR_EOF;
	}
	if(pLine != NULL)
	{
		VK_free(pLine);
	}
	pLine = NULL;
	return nRet;
}

