#include <vkernel.h>
#include <hlib.h>

HUINT8*	getBuffer(HINT32 size)
{
	static HINT32	s_nSize = 0;
	static HUINT8	*s_pucBuffer = NULL;

	if (s_pucBuffer == NULL)
	{
		s_nSize = size;
		s_pucBuffer = HLIB_STD_MemAlloc(size);
	} else
	{
		if (s_nSize < size)
		{
			HLIB_STD_MemFree(s_pucBuffer);
			s_nSize = size;
			s_pucBuffer = HLIB_STD_MemAlloc(size);
		}
	}

	return s_pucBuffer;
}

void dump_binary (const HUINT8 *bin, HUINT32 size, HINT32 depth)
{
	HUINT8	line[16];
	HINT32	i, n;
	HINT32	len;
	HCHAR	tab[32];

	for (i = 0 ; i < 32 && i < depth ; i++)
		tab[i] = '\t';
	tab[i] = '\0';

	for (len = (HINT32)size ; len > 0 ;)
	{
		if (len > 16)
			n = 16;
		else
			n = len;

		memcpy(line, bin, n);
		len -= n;
		bin += n;

		printf("%s", tab);
		for (i = 0 ; i < 16 ; i++)
		{
			if (i < n)
				printf("%02X ", line[i]);
			else
				printf("   ");
		}
		printf("\t");
		for (i = 0 ; i < 16 ; i++)
		{
			if (i < n)
			{
				if (isprint(line[i]))
					printf("%c", line[i]);
				else
					printf(".");
			}
			else
				printf(" ");
		}
		printf("\n");
	}
}

void	extract(HCHAR *pszFileName, HUINT32 ulSystemTick)
{
	FILE	*file;
	HUINT32	ulLogSystemTick;
	HINT32	fnSize;
	HINT32	nReqId;
	HUINT8	*pucBuffer;

	HINT32			nSrzr;
	void			*pstObj;
	HxQueue_t		*pstJob;

	sync();

	file = fopen(pszFileName, "r");
	if (file == NULL)
	{
		printf("open file error [%s]\n", pszFileName);
		return;
	}

	pucBuffer = getBuffer(HxSTD_StrLen(RPC_LOG_TEXT) + 1);
	fread(pucBuffer, 1, HxSTD_StrLen(RPC_LOG_TEXT) + 1, file);

	nSrzr = HLIB_SERIALIZER_OpenEx(HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	pstObj = HLIB_SERIALIZER_OpenUnpackCache();
	pstJob = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)HLIB_STD_MemFree_CB);

	while (!feof(file))
	{
		HxObject_t	*pstObject;
		HxRPC_CMD_e	eCmd;
		HINT32		nProgId, i, nSize;
		HINT32		nBinary;
		HxObject_t	*astBinary[16];
		HxObject_t	*pstParam;
		HERROR			hReturnErr;

		nBinary = 0;
		pstParam = NULL;
		fread(&ulLogSystemTick, 1, 4, file);
		fread(&fnSize,          1, 4, file);
		fread(&nReqId,          1, 4, file);
		pucBuffer = getBuffer(fnSize);
		fread(pucBuffer,        1, fnSize, file);
		if (ulLogSystemTick > ulSystemTick)
		{
			printf("[%05d.%03d] ", ulLogSystemTick / 1000, ulLogSystemTick % 1000);
			pstObject = HLIB_SERIALIZER_UnpackWithCache(pstObj, pucBuffer, fnSize, TRUE);
			if (pstObject == NULL)
			{
				printf("unknown\n");
				continue;
			}
			eCmd = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
			nProgId = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
			hReturnErr = (HERROR)(HxOBJECT_ARR_VAL(pstObject, 2)->u.num); // for response.

			switch (eCmd)
			{
				case eHLIB_RPC_CMD_REQUEST:
					printf("[REQ] ");
					break;
				case eHLIB_RPC_CMD_CALLBACK:
					printf("[CBK] ");
					break;
				case eHLIB_RPC_CMD_RESPONSE:
					printf("[RET] ");
					break;
			}

			//printf("%08x - ", nReqId);
			printf("%s (", HxOBJECT_ARR_VAL(pstObject, 3)->u.str);
			if (HxOBJECT_ARR_LEN(pstObject) == 5)
			{

				pstParam = HxOBJECT_ARR_VAL(pstObject, 4);
				pstParam = HLIB_SERIALIZER_UnpackWithCache(pstObj, HxOBJECT_BIN_DATA(pstParam), HxOBJECT_BIN_SIZE(pstParam), FALSE);
				nSize = HxOBJECT_ARR_LEN(pstParam);
				for (i = 0; i < nSize; i++)
				{
					if (i)
						printf(", ");
					switch ((HCHAR)(HxOBJECT_ARR_VAL(pstParam, i)->type))
					{
						case HxOBJECT_TYPE_BIN:
							astBinary[nBinary] = HxOBJECT_ARR_VAL(pstParam, i);
							printf("binary%d", nBinary++);
							break;
						case HxOBJECT_TYPE_INT:
							printf("%d", HxOBJECT_INT(HxOBJECT_ARR_VAL(pstParam, i)));
							break;
						case HxOBJECT_TYPE_INT64:
							printf("%ld", HxOBJECT_INT64(HxOBJECT_ARR_VAL(pstParam, i)));
							break;
						case HxOBJECT_TYPE_STR:
							printf("\"%s\"", HxOBJECT_STR(HxOBJECT_ARR_VAL(pstParam, i)));
							break;
						case HxOBJECT_TYPE_ARR:
							printf("arr");
							break;
					}
				}
			}
			printf(")");
			printf("\n");
			for (i = 0; i < nBinary; i++)
			{
				printf("\tbinary %d\n", i);
				dump_binary(HxOBJECT_BIN_DATA(astBinary[i]), HxOBJECT_BIN_SIZE(astBinary[i]), 1);
			}
			if (pstParam)
				HLIB_SERIALIZER_FreeObjectWithCache(pstObj, pstParam);

			if (pstObject)
				HLIB_SERIALIZER_FreeObjectWithCache(pstObj, pstObject);
		}
	}

	fclose(file);
}

int	main(int argc, void *argv[])
{
	HINT32		n;
	HCHAR		*pszFileName;
	HUINT32		ulSystemDuration = 0;

	VK_Init();
	HLIB_DBG_Init();

	if (argc == 1)
		goto HELP;

	n = 1;
	if (HLIB_STD_StrCmpV(HxSTD_StrCmp, argv[n], "-h", "--help", NULL) >= 0)
	{
		goto HELP;
	}

	if (HLIB_STD_StrCmpV(HxSTD_StrCmp, argv[n], "-s", "--start", NULL) >= 0)
	{
		ulSystemDuration = HLIB_STD_StrToINT32(argv[++n]);
		n++;
	}

	if (n >= argc)
		goto HELP;

	pszFileName = argv[n];
	printf("extract rpclog (%s) from %d seconds\n", pszFileName, ulSystemDuration);

	extract(pszFileName, ulSystemDuration * 1000);

	return 0;

HELP:
	printf("RPC TOOL : extract rpc log to text file.\n");
	printf("         : -h    help\n");
	printf("         : -s startsec   extract from system sec\n");
	printf("      ex : rpctool sama.rpglog\n");
	printf("         : rpctool -d 1000 sama.rpclog\n");

	return 0;
}

