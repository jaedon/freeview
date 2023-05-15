/**************************************************************
 *	@file		dama_debug.c
 *	Humax Data Manager for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			HumaxDataManager for OCTO 2.0 Project
 **************************************************************/
#ifdef	CONFIG_DEBUG
 static DAMA_DB_t	 *cmdDB = NULL;

 static HINT32		 dama_cmd_SQL(void *szArgList)
 {
	 return DAMA_DB_Command(cmdDB, "SQL", (HCHAR*)szArgList);
 }

 static HINT32		 dama_cmd_SQLITE(void *szArgList)
 {
	 return DAMA_DB_Command(cmdDB, "SQLITE", (HCHAR*)szArgList);
 }

 static HINT32		 dama_cmd_Backup(void *szArgList)
 {
	 HBOOL ret;

	 HxLOG_Print("Backup Start...\n");
	 ret = DAMA_DB_Dump(cmdDB, (const HCHAR *)szArgList);
	 HxLOG_Print("Backup Done. (result: %s)\n", ret ? "success" : "failed");

	 return 0;
 }

 static HINT32		 dama_cmd_Monitor(void *szArgList)
 {
	 return DAMA_DB_Command(cmdDB, "MON", szArgList);
 }

 static HINT32		 dama_cmd_Schema(void *szArgList)
 {
	 return DAMA_DB_Command(cmdDB, "SCHEMA", szArgList);
 }

 static HINT32		 dama_cmd_printdbmem(void *szArgList)
 {
	 return DAMA_DB_Command(cmdDB, "MEM", szArgList);
 }

 static void dama_cmdtool_init(DAMA_DB_t *db)
 {
#define	cmdHandle		"sqlcmd"

	 cmdDB = db;
	 if (cmdDB == NULL)
	 {
		 HxLOG_Critical("Error : cmdDB is not VALID!!!\n");
		 return;
	 }

	 HLIB_CMD_RegisterWord(cmdHandle,
		 dama_cmd_SQL,
		 "sql",
		 "sql sql_statement",
		 "sql sql_statement"
		 );
	 HLIB_CMD_RegisterWord(cmdHandle,
		 dama_cmd_SQLITE,
		 "sqlite",
		 "sqlite sql_statement",
		 "sqlite sql_statement"
		 );
	 HLIB_CMD_RegisterWord(cmdHandle,
		 dama_cmd_Backup,
		 "backup",
		 "backup db",
		 "backup db"
		 );
	 HLIB_CMD_RegisterWord(cmdHandle,
		 dama_cmd_Monitor,
		 "monitor",
		 "monitor db table",
		 "monitor db table"
		 );
	 HLIB_CMD_RegisterWord(cmdHandle,
		 dama_cmd_Schema,
		 "schema",
		 "schema db table",
		 "schema db table"
		 );
	 HLIB_CMD_RegisterWord("mem",
		 dama_cmd_printdbmem,
		 "printdbmem",
		 "printdbmem",
		 "printdbmem"
		 );
 }
#endif

#if defined(DAMA_LOGGING)
 static void dama_debug_dump_binary (FILE *fp, const HUINT8 *bin, HUINT32 size, const HCHAR *prefix)
 {
	 HUINT8  line[16];
	 HINT32  i, n;
	 HINT32  len;

	 if (fp == NULL)
		 fp = stdout;
	 if (prefix == NULL)
		 prefix = "";

	 for (len = (HINT32)size ; len > 0 ;)
	 {
		 if (len > 16)
			 n = 16;
		 else
			 n = len;

		 memcpy(line, bin, n);
		 len -= n;
		 bin += n;

		 fprintf(fp, "%s", prefix);
		 for (i = 0 ; i < 16 ; i++)
		 {
			 if (i < n)
				 fprintf(fp, "%02X ", line[i]);
			 else
				 fprintf(fp, "	 ");
		 }
		 fprintf(fp, "\t");
		 for (i = 0 ; i < 16 ; i++)
		 {
			 if (i < n)
			 {
				 if (isprint(line[i]))
					 fprintf(fp, "%c", line[i]);
				 else
					 fprintf(fp, ".");
			 }
			 else
				 fprintf(fp, " ");
		 }
		 fprintf(fp, "\n");
	 }
 }


 int dama_debug_extract_log (int argc, char *argv[])
 {
	 FILE		 *fp;
	 FILE		 *out;
	 DAMA_Log_t  log;
	 HUINT8 	 *buf;
	 HUINT8 	 *data;
	 HUINT32	 size;
	 HINT32 	 ret;
	 HINT32 	 sd;
	 HINT32 	 remain;
	 DAMA_Header_t	 header;
	 void		 *item;

#define ADVANCE(offset)	\
		 data	+= offset;\
		 remain -= offset;\
		 if (remain <= 0)\
			 break;\

	 out  = stdout;
	 sd   = HLIB_RWSTREAM_Open(NULL, 0);
	 size = 1024 * 1024; // 1M
	 buf  = (HUINT8 *)HLIB_STD_MemAlloc(size);

	 while (argc--)
	 {
		 fprintf(out, "extract %s...\n", *argv);

		 fp = fopen(*argv++, "rb");
		 if (fp == NULL)
		 {
			 fprintf(out, "failed to open file!!\n");
			 continue;
		 }

		 while (fread(&log, sizeof(DAMA_Log_t), 1, fp) == 1)
		 {
			 if (log.size > size)
			 {
				 HLIB_STD_MemFree(buf);
				 size = log.size * 2;
				 buf  = (HUINT8 *)HLIB_STD_MemAlloc(size);
			 }

			 if (fread(buf, 1, log.size, fp) != log.size)
			 {
				 break;
			 }

			 memcpy(&header, buf, sizeof(DAMA_Header_t));
			 data = buf + sizeof(DAMA_Header_t);

			 fprintf(out, "[%s] %s (pname:%s, id:0x%08X, name:\"%s\", uid:%lld, size:%u) | "
				 , HLIB_DATETIME_UnixTimeToString(log.time, NULL, 0)
				 , (log.cmd[0] == '>' ? ">>>" : "<<<")
				 , HLIB_STD_GetProcessName((pid_t)header.requestId >> 16)
				 , header.requestId
				 , header.name
				 , header.uid
				 , header.size
			 );
			 switch (header.cmd)
			 {
			 case eDAMA_CMD_SET:
				 if (HxSTD_StrEmpty(header.name))
					 fprintf(out, "RESET ALL\n");
				 else if (header.size == 0)
					 fprintf(out, "RESET\n");
				 else
				 {
					 fprintf(out, "SET\n");
					 item = DLIB_NewType(header.name, header.uid, data, header.size);
					 ret  = -1;
					 if (item)
					 {
						 ret = DLIB_Print(header.name, out, item);
						 DLIB_FreeType(header.name, item);
					 }
					 if (ret < 0)
						 dama_debug_dump_binary(out, data, header.size, "\t");
				 }
				 break;

			 case eDAMA_CMD_REQUEST:
				 fprintf(out, "REQUEST\n");
				 if (header.size > 0)
				 {
					 data[header.size] = '\0';
					 fprintf(out, "\tsql:\"%s\"\n", data);
				 }
				 break;

			 case eDAMA_CMD_RESPONSE:
				 fprintf(out, "RESPONSE\n");
				 if (header.size > 0)
				 {
					 remain = (HINT32)header.size;

					 fprintf(out, "\tresponse: [\n");
					 for (;;)
					 {
						 HUID	 uid;
						 HUINT32 itemsize;
						 HUINT32 namelen;

						 memcpy(&uid, data, sizeof(HUID));
						 ADVANCE(sizeof(HUID));
						 memcpy(&itemsize, data, sizeof(HUINT32));
						 ADVANCE(sizeof(HUINT32));
						 memcpy(&namelen, data, sizeof(HUINT32));

						 fprintf(out, "\t\t%s(%lld) size:%u\n", data + sizeof(HUINT32), uid, itemsize - namelen - sizeof(HUINT32));
						 item = DLIB_NewType(data + sizeof(HUINT32), uid, data + sizeof(HUINT32) + namelen, itemsize - namelen - sizeof(HUINT32));
						 ret  = -1;
						 if (item)
						 {
							 ret = DLIB_Print(data + sizeof(HUINT32), out, item);
							 DLIB_FreeType(data + sizeof(HUINT32), item);
						 }
						 if (ret < 0)
							 dama_debug_dump_binary(out, data + sizeof(HUINT32) + namelen, itemsize - namelen - sizeof(HUINT32), "\t\t\t");
						 ADVANCE(itemsize);
					 }
					 fprintf(out, "\t]\n");
				 }
				 break;

			 case eDAMA_CMD_ADD_NOTIFIER:
				 if (header.uid == DAMA_TABLE_NOTIFIER)
					 fprintf(out, "ADD TABLE NOTIFIER\n");
				 else
					 fprintf(out, "ADD NOTIFIER\n");
				 break;

			 case eDAMA_CMD_REMOVE_NOTIFIER:
				 fprintf(out, "REMOVE NOTIFIER\n");
				 break;

			 case eDAMA_CMD_NOTIFICATION:
				 fprintf(out, "NOTIFICATION\n");
				 if (header.size > 0)
				 {
					 HUID	 uid;
					 HUINT32 c = header.size / sizeof(HUID);
					 HLIB_RWSTREAM_Rewind(sd);
					 while (c--)
					 {
						 memcpy(&uid, data, sizeof(HUID));
						 data += sizeof(HUID);
						 HLIB_RWSTREAM_Print(sd, "%lld ", uid);
					 }
					 fprintf(out, "\t%s\n", (char *)HLIB_RWSTREAM_GetBuf(sd));
				 }
				 break;

			 case eDAMA_CMD_SYNC:
				 if (HxSTD_StrEmpty(header.name))
					 fprintf(out, "SYNC ALL\n");
				 else
					 fprintf(out, "SYNC\n");
				 break;

			 case eDAMA_CMD_RESTORE:
				 if (HxSTD_StrEmpty(header.name))
					 fprintf(out, "RESTORE ALL\n");
				 else
					 fprintf(out, "RESTORE\n");
				 break;

			 case eDAMA_CMD_BATCH:
				 fprintf(out, "BATCH\n");
				 if (header.size > 0)
				 {
					 remain = (HINT32)header.size;
					 for (; remain > 0 ;)
					 {
						 memcpy(&header, data, sizeof(DAMA_Header_t));
						 ADVANCE(sizeof(DAMA_Header_t));
						 fprintf(out, "\tpname:%s, id:0x%08X, name:\"%s\", uid:%lld, size:%u | %s(%d)\n"
							 , HLIB_STD_GetProcessName((pid_t)header.requestId >> 16)
							 , header.requestId
							 , header.name
							 , header.uid
							 , header.size
							 , header.cmd == eDAMA_CMD_SET ? "SET" : ""
							 , header.cmd
						 );
						 if (header.size > 0)
						 {
							 item = DLIB_NewType(header.name, header.uid, data, header.size);
							 ret  = -1;
							 if (item)
							 {
								 ret = DLIB_Print(header.name, out, item);
								 DLIB_FreeType(header.name, item);
							 }
							 if (ret < 0)
								 dama_debug_dump_binary(out, data, header.size, "\t\t");
							 ADVANCE(header.size);
						 }
					 }
				 }
				 break;

			 case eDAMA_CMD_FACTORY_RESET:
				 fprintf(out, "FACTORY RESET\n");
				 break;

			 case eDAMA_CMD_SERVER_SYNC:
				 fprintf(out, "Server Sync\n");
				 break;

			 case eDAMA_CMD_EXIT:
				 fprintf(out, "EXIT\n");
				 break;

			 case eDAMA_CMD_MONITOR:
				 fprintf(out, "MONITOR\n");
				 if (header.size > 0)
				 {
					 DAMA_Status_e	 status;
					 memcpy(&status, data, sizeof(DAMA_Status_e));
					 switch (status)
					 {
					 case eDAMA_LIMIT:
					 {
						 HUINT32 count;
						 memcpy(&count, data + sizeof(DAMA_Status_e), sizeof(HUINT32));
						 fprintf(out, "\tLIMIT: count(%u)\n", count);
						 break;
					 }
					 case eDAMA_BUSY:
					 case eDAMA_READY:
					 default:
						 fprintf(out, "\tstatus(%d)\n", status);
						 break;
					 }
				 }
				 break;

			 default:
				 break;
			 }
		 }
		 fclose(fp);
	 }
	 HLIB_RWSTREAM_Close(sd);
	 HLIB_STD_MemFree(buf);
	 return 0;
 }
#endif


