#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "libdsmcc.h"
#include "dsmcc-cache.h"
#include "dsmcc-biop.h"
#include "dsmcc-receiver.h"
#include "dsmcc-descriptor.h"

FILE *cache_fd = NULL;
/* TODO This should be stored in obj_carousel structure  */

dsmcc_writefile_callback		 	g_libdsmcc_writefile_callback = NULL;
dsmcc_writeevt_callback			 	g_libdsmcc_writeevt_callback = NULL;
dsmcc_writedir_callback			 	g_libdsmcc_writedir_callback = NULL;
dsmcc_writedescriptorevt_callback	g_libdsmcc_writedesriptorevent_callback = NULL;

#define _____STATIC_FUNCTION_PROTOTYPE_____
static void	dsmcc_cache_free_dir(Cache_Dir_t	*d) ;
static unsigned int dsmcc_cache_key_cmp(char *str1, char *str2, unsigned int len1, unsigned int len2) ;
static Cache_File_t *dsmcc_cache_scan_file(Cache_Dir_t *dir, unsigned long car_id, unsigned int mod_id, unsigned int key_len, char *key);
static Cache_Dir_t *dsmcc_cache_scan_dir (Cache_Dir_t *dir, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key);
static Cache_Dir_t *dsmcc_cache_dir_find (Cache_All_t *pstCacheAll, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key);
static Cache_File_t *dsmcc_cache_file_find(Cache_All_t *pstCacheAll, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key);
static Cache_File_t *dsmcc_cache_file_find_data(Cache_All_t  *pstCacheAll, unsigned long car_id, unsigned short mod_id, unsigned int key_len, char *key);
static void dsmcc_cache_attach_file(Cache_All_t *pstCacheAll, Cache_Dir_t *root, Cache_File_t *file);
static void dsmcc_cache_attach_dir(Cache_All_t *pstCacheAll, Cache_Dir_t *root, Cache_Dir_t *dir);
static void dsmcc_cache_write_dir(Cache_All_t *pstCacheAll, Cache_Dir_t *dir);
static void dsmcc_cache_write_file(Cache_All_t *pstCacheAll, Cache_File_t *file);
static void dsmcc_cache_make_file_from_event (BIOP_Message_t*pstBiopMsg, CacheModuleData_t *pstCacheModuleData, Cache_File_t  *pstCacheFile);
static void dsmcc_cache_make_event_from_raw (unsigned char *szFileName, unsigned char *szFilePath, unsigned char *pucData, int nDataLen, char *szChanName, int nCarouselId, int nPid, int nModuleId);
static void dsmcc_cache_unknown_file_info(Cache_All_t *pstCacheAll, Cache_File_t *newfile);
#define ______STATIC_FUNCTION______

static void	dsmcc_cache_free_dir(Cache_Dir_t	*d)
{
	Cache_Dir_t *dn, *dnn;
	Cache_File_t *f, *fn;

	if(d->sub!=NULL)
	{
		dn = d->sub;
		while(dn!=NULL)
		{
			dnn = dn->next;
			dsmcc_cache_free_dir(dn);
			dn = dnn;
		}
	}

	if(d->name!=NULL) { free(d->name); }
	if(d->dirpath!=NULL) { free(d->dirpath); }
	if(d->key!=NULL) { free(d->key); d->key=NULL;}
	if(d->p_key!=NULL) { free(d->p_key); d->p_key=NULL;}
	f = d->files;

	while(f!=NULL)
	{
		fn = f->next;
		if(f->key!=NULL) { free(f->key); f->key=NULL;}
		if(f->filename!=NULL) { free(f->filename); }
		if(f->data!=NULL) { free(f->data); }
		if(f->p_key!=NULL) { free(f->p_key); f->p_key=NULL;}
		free(f);
		f = fn;
	}
	free(d);
}

static unsigned int dsmcc_cache_key_cmp(char *str1, char *str2, unsigned int len1, unsigned int len2)
{
        unsigned int i;

        /* Key Len must be equal */

        if(len1 != len2) { return 0; }

        for(i = 0; i < len1; i++) {
                if(str1[i] != str2[i]) {
                        return 0;
                }
        }

        return 1;
}

static Cache_Dir_t *dsmcc_cache_scan_dir (Cache_Dir_t *dir, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key)
{
	Cache_Dir_t *founddir, *subdir;

	if(dir == NULL) { return NULL; }

	if ((dir->carousel_id == car_id) &&
		(dir->module_id == module_id) &&
		dsmcc_cache_key_cmp(dir->key, key, dir->key_len, key_len))
	{
		return dir;
	}

	/* Search sub dirs */
	for(subdir = dir->sub; subdir != NULL; subdir=subdir->next)
	{
		founddir = dsmcc_cache_scan_dir(subdir, car_id, module_id, key_len,key);
		if(founddir != NULL)
		{
			return founddir;
		}
	}

	return NULL;
}

static Cache_Dir_t *dsmcc_cache_dir_find (Cache_All_t *pstCacheAll, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key)
{
	Cache_Dir_t *dir, *fdir, *nextDir;
	Cache_File_t *file, *nf;

	/* Scan through known dirs and return details if known else NULL */
	if(module_id == 0 && key_len == 0)
	{
		/* Return gateway object. Create if not already */
		if(pstCacheAll->gateway == NULL)
		{
			pstCacheAll->gateway = (Cache_Dir_t *)malloc(sizeof(Cache_Dir_t));
			if(pstCacheAll->gateway == NULL)
			{
				return NULL;
			}
			memset (pstCacheAll->gateway, 0, sizeof(Cache_Dir_t));

			pstCacheAll->gateway->name = (char *)malloc(2);
			pstCacheAll->gateway->carousel_id = car_id;
			pstCacheAll->gateway->module_id = pstCacheAll->gateway->key_len =
			pstCacheAll->gateway->p_key_len = 0;

			/*TODO argg! a hack to fix a bug caused by a hack.Need better linking */
			strcpy(pstCacheAll->gateway->name, "/");
			pstCacheAll->gateway->dirpath = (char *)malloc(2);
			if(pstCacheAll->gateway->dirpath == NULL)
			{
				if(pstCacheAll->gateway != NULL)
					free(pstCacheAll->gateway);

				return NULL;
			}
			strcpy(pstCacheAll->gateway->dirpath, "/");
			pstCacheAll->gateway->sub = pstCacheAll->gateway->parent =
			pstCacheAll->gateway->prev = pstCacheAll->gateway->next = NULL;
			pstCacheAll->gateway->files = NULL;

			/* Attach any subdirs or files that arrived prev. */
			for(file=pstCacheAll->file_cache; file!=NULL; file=nf)
			{
				nf=file->next;
				if ((file->carousel_id == pstCacheAll->gateway->carousel_id) &&
					(file->p_module_id == pstCacheAll->gateway->module_id) &&
 					dsmcc_cache_key_cmp (file->p_key, pstCacheAll->gateway->key, file->p_key_len, pstCacheAll->gateway->key_len))
				{
					dsmcc_cache_attach_file (pstCacheAll,pstCacheAll->gateway, file);
				}
			}

			fdir = pstCacheAll->dir_cache;
			while (fdir != NULL)
			{
				nextDir = fdir->next;
				dsmcc_cache_attach_dir (pstCacheAll, pstCacheAll->gateway, fdir);
				fdir = nextDir;
			}

			dsmcc_cache_write_dir (pstCacheAll, pstCacheAll->gateway);	/* Write files to filesystem */
		}

		return pstCacheAll->gateway;
	}

	/* Find dir magic */
	dir = dsmcc_cache_scan_dir(pstCacheAll->gateway, car_id, module_id, key_len, key);
	if (dir == NULL)
	{	/* Try looking in unlinked dirs list */
		for (fdir=pstCacheAll->dir_cache; (dir==NULL) && (fdir!=NULL); fdir = fdir->next)
		{
			dir = dsmcc_cache_scan_dir(fdir, car_id, module_id, key_len, key);
		}
	}

	/* TODO - Directory not known yet, cache it ? */

	return dir;
}

static Cache_File_t *dsmcc_cache_file_find(Cache_All_t *pstCacheAll, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key)
{
	Cache_Dir_t *dir;
	Cache_File_t *file;

	 /* Try looking in parent-less list */

	for(file = pstCacheAll->file_cache; file != NULL; file = file->next)
	{
		if((file->carousel_id == car_id) &&
			(file->module_id == module_id) &&
			dsmcc_cache_key_cmp(file->key, key, file->key_len, key_len))
		{
			return file;
		}
	}

	/* Try looking in parent-less dir list */
	for (dir = pstCacheAll->dir_cache; dir != NULL; dir = dir->next)
	{
		for (file = dir->files; file != NULL; file = file->next)
		{
			if((file->carousel_id == car_id) &&
				(file->module_id == module_id) &&
				dsmcc_cache_key_cmp(file->key, key, file->key_len, key_len))
			{
				return file;
			}
		}
	}

	/* Scan through known files and return details if known else NULL */
	file = dsmcc_cache_scan_file(pstCacheAll->gateway, car_id, module_id, key_len, key);

	return file;
}

Cache_File_t *dsmcc_cache_file_find_data(Cache_All_t  *pstCacheAll, unsigned long car_id, unsigned short mod_id, unsigned int key_len, char *key)
{
	Cache_File_t *last;

	for(last=pstCacheAll->data_cache; last!=NULL; last = last->next)
	{
		if ((last->carousel_id==car_id) &&
			(last->module_id==mod_id) &&
			 dsmcc_cache_key_cmp(key, last->key, key_len, last->key_len))
		{
			if(last->prev != NULL)
			{
				last->prev->next = last->next;
			}
			else
			{
				pstCacheAll->data_cache = last->next;
			}

			if(last->next != NULL)
			{
				last->next->prev = last->prev;
			}

			break;
		}
	}

	return last;
}

static void dsmcc_cache_attach_file(Cache_All_t *pstCacheAll, Cache_Dir_t *root, Cache_File_t *file)
{
	Cache_File_t *cfile;

	/* Search for any files that arrived previously in unknown files list*/
	if(root->files == NULL)
	{
		if(file->prev!=NULL)
		{
			file->prev->next = file->next;
		}
		else
		{
			pstCacheAll->file_cache=file->next;
		}

		if(file->next!=NULL)
		{
			file->next->prev = file->prev;
		}

		root->files = file;
		root->files->next = root->files->prev = NULL;
		file->parent = root;
	}
	else
	{
		if(file->prev!=NULL)
		{
			file->prev->next = file->next;
		}
		else
		{
			pstCacheAll->file_cache=file->next;
		}

		if(file->next!=NULL)
		{
			file->next->prev = file->prev;
		}

		for (cfile=root->files;cfile->next!=NULL;cfile=cfile->next){;}
		cfile->next = file;
		file->prev = cfile;
		file->next = NULL;	/* TODO uurrgh */
		file->parent = root;
	}
}

static void dsmcc_cache_attach_dir(Cache_All_t *pstCacheAll, Cache_Dir_t *root, Cache_Dir_t *dir)
{
	Cache_Dir_t*last;

	if ((dir->carousel_id == root->carousel_id) &&
		(dir->p_module_id == root->module_id) &&
		dsmcc_cache_key_cmp(dir->p_key,root->key,dir->p_key_len,root->key_len))
	{
		// Remove dir from the directory cache : It is to be attached.
		if(dir->prev != NULL)
		{
			dir->prev->next = dir->next;
		}
		else
		{
			pstCacheAll->dir_cache = dir->next;
		}

		if(dir->next!=NULL)
		{
			dir->next->prev = dir->prev;
		}

		// Attach dir to the sub of the root
		if(root->sub == NULL)
		{
			root->sub = dir;
			root->sub->next = root->sub->prev = NULL;
		}
		else
		{
			for (last=root->sub; last->next!=NULL; last=last->next) { ; }
			last->next = dir;
			dir->prev = last;
			dir->next = NULL;
		}

		dir->parent = root;
	}
	else
	{
	}
}

static Cache_File_t *dsmcc_cache_scan_file(Cache_Dir_t *dir, unsigned long car_id, unsigned int mod_id, unsigned int key_len, char *key)
{
	Cache_File_t *file;
	Cache_Dir_t *subdir;

	if(dir == NULL) { return NULL; }

	/* Search files in this dir */
	for(file = dir->files; file != NULL; file = file->next)
	{
		if ((file->carousel_id == car_id) &&
			(file->module_id == mod_id) &&
			dsmcc_cache_key_cmp(file->key, key, file->key_len, key_len))
		{
			return file;
		}
	}

	/* Search sub dirs */
	for(subdir = dir->sub; subdir != NULL; subdir=subdir->next)
	{
		file = dsmcc_cache_scan_file(subdir, car_id, mod_id, key_len, key);
		if(file != NULL)
		{
			return file;
		}
	}

	return NULL;
}

static void dsmcc_cache_write_dir(Cache_All_t *pstCacheAll, Cache_Dir_t *dir)
{
	Cache_Dir_t *subdir;
	Cache_File_t *file;
	char dirbuf[256];
	unsigned int nRet = 0;

	if(dir->dirpath == NULL)
	{
		if (dir->parent != NULL && dir->parent->dirpath != NULL)
		{
			dir->dirpath= (char*) malloc(strlen(dir->parent->dirpath)+strlen(dir->name)+2);
			strcpy(dir->dirpath, dir->parent->dirpath);
		}
		else
		{
			dir->dirpath= (char*) malloc(strlen(dir->name)+2);
			dir->dirpath[0] = '\0';
		}

		strcat(dir->dirpath, "/");
		strcat(dir->dirpath, dir->name);
	}

	if (g_libdsmcc_writedir_callback != NULL)
	{
		g_libdsmcc_writedir_callback (dir->name, dir->dirpath, pstCacheAll->name, (int)dir->carousel_id, (int)pstCacheAll->usPid, (int)dir->module_id);
	}
	else
	{
		sprintf(dirbuf, "%s/%s/%s", "/tmp/cache/", pstCacheAll->name, dir->dirpath);

#if 0//defined(CONFIG_ENHANCED_SECURITY)
		SECAPI_Mkdir(dirbuf, 0755);
#else
		nRet = mkdir(dirbuf, 0755);
		if(nRet < 0)
		{
			return;
		}
#endif
	}

	/* Write out files that had arrived before directory */
	for (file=dir->files; file!=NULL; file=file->next)
	{
		if(file->data != NULL)
		{
			dsmcc_cache_write_file(pstCacheAll, file);
		}
	}

	/* Recurse thorugh child directories */

	for(subdir=dir->sub;subdir!=NULL;subdir=subdir->next)
	{
		dsmcc_cache_write_dir(pstCacheAll, subdir);
	}
}


static void dsmcc_cache_make_file_from_event (BIOP_Message_t*pstBiopMsg, CacheModuleData_t *pstCacheModuleData, Cache_File_t  *pstCacheFile)
{
	unsigned int	 nSvcCntxCnt;
	unsigned int	 nObjInfoLen, nContentLen;
	unsigned char	*pucObjInfo, *pucContent;

	nObjInfoLen	= pstBiopMsg->stBiopMsgHeader.objinfo_len;
	pucObjInfo	= pstBiopMsg->stBiopMsgHeader.objinfo;

	nContentLen = 0;
	pucContent = pstCacheModuleData->pucModuleData + pstCacheModuleData->ulReadPosition;

	/* skip service contect count */
	nSvcCntxCnt = (int)(pucContent[nContentLen]);
	nContentLen += (nSvcCntxCnt + 1);

	/* skip msgbody len */
	nContentLen += 4;

	/* skip msgbody */
	nContentLen += pstBiopMsg->body.stBiopBodyStreamEvent.msgbody_len;

	pstCacheFile->data_len = (nObjInfoLen + nContentLen + 2);
	pstCacheFile->data = (char *)malloc (pstCacheFile->data_len);
	if(pstCacheFile->data == NULL)
	{
		return ;
	}

	pstCacheFile->data[0] = (char)((nObjInfoLen >> 8) & 0xff);
	pstCacheFile->data[1] = (char)(nObjInfoLen & 0xff);

	if (nObjInfoLen > 0)
	{
		memcpy (pstCacheFile->data + 2, pucObjInfo, nObjInfoLen);
	}

	memcpy (pstCacheFile->data + 2 + nObjInfoLen, pucContent, nContentLen);
}

static void dsmcc_cache_make_event_from_raw (unsigned char *szFileName, unsigned char *szFilePath, unsigned char *pucData, int nDataLen, char *szChanName, int nCarouselId, int nPid, int nModuleId)
{
	int 				  nCount, nTagIdx, nEvtNameCnt, nEvtIdCnt, nTotalNum;
	int					  nObjInfoLen;
	int					  nSvcContextCnt, nContextDataLen;
	int					  nUse, nTapsCnt, nTagsNum;
	int					 *pnTagArray = NULL;
	int					 *pnEvtId;
	unsigned char		  ucDescLen, ucEvtNameLen;
	unsigned char		 *szDesc;
	unsigned char		**pszEvtName;
	unsigned char		 *pucBuf, *pucMsgBody;
	unsigned char		 *pucObjInfoByte;
	libdsmcc_event_t	  stDsmccEvt;

	pucBuf = (unsigned char *)pucData;

	nObjInfoLen =  (int)((pucBuf[0] << 8) | pucBuf[1] );
	pucBuf += 2;
	pucMsgBody = pucBuf + nObjInfoLen;

	// 1. Description
	ucDescLen = *pucBuf;
	pucBuf++;
	nObjInfoLen--;

	szDesc = malloc (ucDescLen + 1);

	if(szDesc == NULL)
		return;

	if (ucDescLen > 0)
	{
		memcpy (szDesc, pucBuf, ucDescLen);
	}
	szDesc[ucDescLen] = '\0';

	pucBuf += ucDescLen;
	nObjInfoLen -= ucDescLen;

	// 2. Duration (4+4)/audio(1)/vidio(1)/data(1) : skip 11 bytes
	pucBuf += 11;
	nObjInfoLen -= 11;

	// 3. Event Lists
	// eventNames_Count
	nEvtNameCnt = (int)((pucBuf[0] << 8) | pucBuf[1] );
	pucBuf += 2;
	nObjInfoLen -= 2;

	if (nEvtNameCnt <= 0)
	{
		nEvtNameCnt = 1;
		pszEvtName = (unsigned char **)malloc (sizeof(unsigned char *));
		pszEvtName[0] = (unsigned char *)malloc (1);
		pszEvtName[0][0] = '\0';
	}
	else
	{
		pszEvtName = (unsigned char **)malloc (sizeof(unsigned char *) * nEvtNameCnt);

		for (nCount = 0; nCount < nEvtNameCnt; nCount++)
		{
			//eventName_length
			ucEvtNameLen = pucBuf[0];
			pucBuf ++;
			nObjInfoLen --;

			// eventName_data_byte
			pszEvtName[nCount] = malloc (ucEvtNameLen + 1);
			memcpy (pszEvtName[nCount], pucBuf, ucEvtNameLen);
			pszEvtName[nCount][ucEvtNameLen] = '\0';
			pucBuf += ucEvtNameLen;
			nObjInfoLen -= ucEvtNameLen;
		}
	}

	// Object Info
	if (nObjInfoLen > 0)
	{
		pucObjInfoByte = malloc (nObjInfoLen);
		memcpy (pucObjInfoByte, pucBuf, nObjInfoLen);
	}
	else
	{
		pucObjInfoByte = NULL;
	}

	pucBuf = pucMsgBody;

	// serviceContextList_count : skip
	nSvcContextCnt = (int)pucBuf[0];
	pucBuf ++;

	// contexts : skip
	for (nCount = 0; nCount < nSvcContextCnt; nCount++)
	{
		// context ID : skip
		pucBuf += 4;

		// context_data_length : skip
		nContextDataLen = (int)((pucBuf[0] << 8) | pucBuf[1] );
		pucBuf ++;

		// context_data_byte : skip
		pucBuf += nContextDataLen;
	}

	// messageBody_length : skip
	pucBuf += 4;

	// taps_count
	nTapsCnt = pucBuf[0];
	pucBuf ++;
	nTagsNum = 0;

	if (nTapsCnt > 0)
	{
		pnTagArray = malloc (sizeof(int) * nTapsCnt);
		for (nCount = 0; nCount < nTapsCnt; nCount++)
		{
			// ID : 2byte (skip)
			pucBuf += 2;

			// Use : 2byte
			nUse = (int)((pucBuf[0] << 8) | pucBuf[1] );
			pucBuf += 2;

			if (nUse == 0x000C || nUse == 0x000D)
			{
				// association_tag : 2byte
				pnTagArray[nTagsNum] = (int)((pucBuf[0] << 8) | pucBuf[1] );
				pucBuf += 2;

				// selector_length : 1byte (skip)
				pucBuf ++;

				nTagsNum++;
			}
			else
			{
				pucBuf += 3;
			}
		}

		if (nTagsNum == 0)
		{
			free (pnTagArray);
			pnTagArray = NULL;
		}
	}

	if (nTagsNum <= 0)
	{
		nTagsNum = 1;
		pnTagArray = malloc (sizeof(int) * nTagsNum);
		pnTagArray[0] = 0x00;
	}

	// eventIds_count
	nEvtIdCnt = (int)pucBuf[0];
	pucBuf ++;

	if (nEvtIdCnt <= 0)
	{
		nEvtIdCnt = 1;
		pnEvtId = (int *)malloc (sizeof(int));
		pnEvtId[0] = (int)0;
	}
	else
	{
		pnEvtId = (int *)malloc (sizeof(int) * nEvtIdCnt);

		for (nCount = 0; nCount < nEvtIdCnt; nCount++)
		{
			//eventName_length
			pnEvtId[nCount] =  (int)((pucBuf[0] << 8) | pucBuf[1] );
			pucBuf += 2;
		}
	}


	// Execute the Event Call-back
	//
	nTotalNum = (nEvtNameCnt > nEvtIdCnt) ? nEvtIdCnt : nEvtNameCnt;

#if 1
	if (g_libdsmcc_writeevt_callback != NULL)
	{
		for (nCount = 0; nCount < nTotalNum; nCount ++)
		{
			for (nTagIdx = 0; nTagIdx < nTagsNum; nTagIdx++)
			{
				stDsmccEvt.nDescriptionLen	= (int)ucDescLen;
				stDsmccEvt.szDescription	= (unsigned char *)szDesc;
				stDsmccEvt.szEventName		= (unsigned char *)pszEvtName[nCount];
				stDsmccEvt.nObjInfoByteLen	= (int)nObjInfoLen;
				stDsmccEvt.pucObjInfoByte	= (unsigned char *)pucObjInfoByte;
				stDsmccEvt.nEvtId			= (int)pnEvtId[nCount];
				stDsmccEvt.nTag				= (int)pnTagArray[nTagIdx];

				g_libdsmcc_writeevt_callback (szFileName, szFilePath, &stDsmccEvt, szChanName, nCarouselId, nPid, nModuleId);
			}

		}
	}
#else
	{
		int nSubCount;
		printf ("\n\n\n\n\n\n\n\n\n\n");

		for (nCount = 0; nCount < nTotalNum; nCount++)
		{

			printf ("==== Event (%d) ====\n", nCount);
			printf ("  Description [%s] \n", (szDesc != NULL) ? szDesc : "");
			printf ("  Event Name  [%s] \n", pszEvtName[nCount]);
			printf ("  ObjInfo : \n");
			for (nSubCount = 0; nSubCount < nObjInfoLen; nSubCount++)
			{
				printf ("[%02X]", pucObjInfoByte[nSubCount]);
				if ((nSubCount % 16) == 15)
				{
					printf ("\n");
				}
			}
			printf ("  Event Id  [0x%04X] \n\n\n", pnEvtId[nCount]);

		}

		printf ("\n\n\n\n\n\n\n\n\n\n");
	}
#endif

	if (szDesc != NULL)
	{
		free (szDesc);
	}

	if (pnEvtId != NULL)
	{
		free (pnEvtId);
	}

	if (pnTagArray != NULL)
	{
		free (pnTagArray);
	}

	if (pszEvtName != NULL)
	{
		for (nCount = 0; nCount < nEvtNameCnt; nCount++)
		{
			if (pszEvtName[nCount] != NULL)
			{
				free (pszEvtName[nCount]);
			}
		}

		free (pszEvtName);
	}

	if (pucObjInfoByte != NULL)
	{
		free (pucObjInfoByte);
	}

}

static void dsmcc_cache_write_file(Cache_All_t *pstCacheAll, Cache_File_t *file)
{
	FILE *data_fd;
	char buf[128];

	/* TODO create directory structure rather than one big mess! */
	if (file == NULL)
	{
		return;
	}

	if((file->parent!=NULL) && (file->parent->dirpath != NULL))
	{
		switch (file->eFileType)
		{
		case DSMCC_OBJTYPE_EVENT:
			if (g_libdsmcc_writeevt_callback != NULL)
			{
				dsmcc_cache_make_event_from_raw (file->filename, file->parent->dirpath, file->data, file->data_len, pstCacheAll->name, (int)file->carousel_id, (int)pstCacheAll->usPid, (int)file->module_id);
			}

			break;

		case DSMCC_OBJTYPE_FILE:
		default:
			if (g_libdsmcc_writefile_callback != NULL)
			{
				g_libdsmcc_writefile_callback (file->filename, file->parent->dirpath, pstCacheAll->name, (int)file->carousel_id, (int)pstCacheAll->usPid, (int)file->module_id, (int)file->data_len, (unsigned char *)file->data);
			}
			else
			{
				sprintf(buf,"/tmp/cache/%s/%s/%s", pstCacheAll->name, file->parent->dirpath, file->filename);

				data_fd = fopen(buf, "wb");
				fwrite(file->data, 1, file->data_len, data_fd);
				fclose(data_fd);
			}

			break;
		}

		pstCacheAll->num_files--;
	}
	else
	{
		// printf ("- File Write Faild : [%s]\n", file->filename);
	}
}

static void dsmcc_cache_unknown_file_info(Cache_All_t *pstCacheAll, Cache_File_t *newfile)
{
	Cache_File_t *last;/* TODO Check if already unknown file (i.e. data arrived twice before * dir/srg or missed dir/srg message, if so skip. */

	if(pstCacheAll->file_cache == NULL)
	{
		pstCacheAll->file_cache = newfile;
		pstCacheAll->file_cache->next = pstCacheAll->file_cache->prev = NULL;
	}
	else
	{
		for(last=pstCacheAll->file_cache;last->next!=NULL; last = last->next) { ; }
		last->next = newfile;
		newfile->prev = last;
		newfile->next = NULL;
	}
}

#define ______GOBAL_FUNCTION_____

void dsmcc_cache_init(Cache_All_t *pstCacheAll, const char *channel_name)
{
	unsigned int nRet = 0;
	/* TODO - load cache from disk into obj_carousel */

	pstCacheAll->gateway = NULL;
	pstCacheAll->dir_cache = NULL;

	pstCacheAll->file_cache = NULL;
	pstCacheAll->data_cache = NULL;

	pstCacheAll->event_cache = NULL;
	pstCacheAll->event_data_cache = NULL;

	/* Write contents under channel name */
	if(channel_name)
	{
		pstCacheAll->name = (char*)malloc(strlen(channel_name)+1);
		strcpy(pstCacheAll->name, channel_name);
	}
	else
	{
		pstCacheAll->name = (char*)malloc(1);
		pstCacheAll->name = '\0';
	}

#if 0//defined(CONFIG_ENHANCED_SECURITY)
	SECAPI_Mkdir("/tmp/cache", 0755);	/* By popular demand... */
#else
	nRet =mkdir("/tmp/cache", 0755);	/* By popular demand... */
	if(nRet < 0)
	{
		return;
	}
#endif

	pstCacheAll->num_files	= 0;
	pstCacheAll->total_files	= 0;

	pstCacheAll->num_dirs		= 0;
	pstCacheAll->total_dirs	= 0;

	pstCacheAll->num_events	= 0;
	pstCacheAll->total_events	= 0;
}

void dsmcc_cache_free(Cache_All_t *pstCacheAll)
{
	Cache_File_t 	*f, *fn;
	Cache_Dir_t *d, *dn;

	if(pstCacheAll == NULL)
	{
		return;
	}

	/* Free unconnected files */
	f = pstCacheAll->file_cache;
	while(f!=NULL)
	{
		fn = f->next;
		if(f->key!=NULL) { free(f->key); f->key=NULL; }
		if(f->filename!=NULL) { free(f->filename); }
		if(f->data!=NULL) { free(f->data); }
		if(f->p_key!=NULL) { free(f->p_key); f->p_key=NULL; }
		free(f);
		f = fn;
	}
	/* Free cached data */
	f = pstCacheAll->data_cache;
	while(f!=NULL) {
		fn = f->next;
		if(f->key != NULL) { free(f->key); f->key = NULL;}
		if(f->data!=NULL) { free(f->data); }
		free(f);
		f = fn;
	}
	/* Free unconnected dirs */
	d = pstCacheAll->dir_cache;
	while(d!=NULL)
	{
		dn = d->next;
		if(d->name!=NULL) { free(d->name); }
		if(d->dirpath!=NULL) { free(d->dirpath); }
		if(d->key!=NULL) { free(d->key); d->key=NULL;}
		if(d->p_key!=NULL) { free(d->p_key); d->p_key=NULL;}
		f = d->files;

		while(f!=NULL)
		{
			fn = f->next;
			if(f->key!=NULL) { free(f->key); f->key=NULL;}
			if(f->filename!=NULL) { free(f->filename); }
			if(f->data!=NULL) { free(f->data); }
			if(f->p_key!=NULL) { free(f->p_key); f->p_key=NULL;}
			free(f);
			f = fn;
		}
		free(d);
		d = dn;
	}

	/* Free cache - TODO improve this */

	if(pstCacheAll->gateway != NULL)
		dsmcc_cache_free_dir(pstCacheAll->gateway);

	pstCacheAll->file_cache = pstCacheAll->data_cache = NULL;
	pstCacheAll->gateway = pstCacheAll->dir_cache = NULL;

	if(pstCacheAll->name)
		free(pstCacheAll->name);

	if(pstCacheAll != NULL)
	{
		free(pstCacheAll);
		pstCacheAll = NULL;
	}
}

void dsmcc_cache_file (Cache_All_t *pstCacheAll, BIOP_Message_t *pstBiopMsg, CacheModuleData_t *pstCacheModuleData)
{
	Cache_Dir_t	*dir;
	Cache_File_t	*last, *file;

	file = dsmcc_cache_file_find (pstCacheAll, pstCacheModuleData->carousel_id, pstCacheModuleData->module_id, pstBiopMsg->stBiopMsgHeader.objkey_len, pstBiopMsg->stBiopMsgHeader.objkey);
	if(file == NULL)
	{
		file = (Cache_File_t *)malloc(sizeof(Cache_File_t));
		if(file == NULL)
		{
			return;
		}
		memset (file, 0, sizeof(Cache_File_t));

		file->eFileType = DSMCC_OBJTYPE_FILE;

		file->data_len = pstBiopMsg->body.stBiopBodyFile.content_len;
		if (file->data_len > 0)
		{
			file->data = (char*)malloc(file->data_len);
		}
		else
		{
			file->data = (char*)malloc(4);
		}

		if(file->data == NULL)
		{
			if(file != NULL)
				free(file);

			return;
		}

		memcpy (file->data, pstCacheModuleData->pucModuleData+pstCacheModuleData->ulReadPosition, file->data_len);
		file->carousel_id = pstCacheModuleData->carousel_id;
		file->module_id = pstCacheModuleData->module_id;
		file->key_len = pstBiopMsg->stBiopMsgHeader.objkey_len;
		file->key = (char*)malloc(file->key_len);
		memcpy (file->key, pstBiopMsg->stBiopMsgHeader.objkey, file->key_len);
		file->next = file->prev = NULL;
		// Add to unknown data cache

		if(pstCacheAll->data_cache == NULL)
		{
			pstCacheAll->data_cache = file;
		}
		else
		{
			for (last=pstCacheAll->data_cache;last->next!=NULL;last=last->next){;}

			last->next = file;
			file->prev = last;
		}
	}

	if (file->parent == NULL && (file->p_key != NULL))
	{
		dir = dsmcc_cache_dir_find (pstCacheAll, file->p_carousel_id, file->p_module_id, file->p_key_len, file->p_key);
		if (dir != NULL)
		{
			dsmcc_cache_attach_file (pstCacheAll, dir, file);
		}
	}

	if(file->data == NULL)
	{
		file->data_len = pstBiopMsg->body.stBiopBodyFile.content_len;
		file->data = (char *)malloc(file->data_len);
		memcpy(file->data,pstCacheModuleData->pucModuleData+pstCacheModuleData->ulReadPosition, file->data_len);
		/* TODO this should be a config option */
		dsmcc_cache_write_file(pstCacheAll, file);
	}
}

void dsmcc_cache_file_info(Cache_All_t *pstCacheAll, unsigned short mod_id, unsigned int key_len, char *key, BIOP_Binding_t *pstBiopBinding, unsigned int comp_idx)
{
	Cache_File_t *newfile, *last;
	Cache_File_t *oldfile;
	Cache_Dir_t *dir;

	// Check we do not already have file (or file info) cached
	oldfile = dsmcc_cache_file_find(pstCacheAll,
			pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id,
			pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.module_id,
			pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len,
			pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey);
	if (NULL != oldfile)
	{
		char		*newfile_name = pstBiopBinding->stBiopName.comps[comp_idx].id;

		dir = dsmcc_cache_dir_find(pstCacheAll, pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id, mod_id, key_len, key);
		if ((NULL != dir) && (NULL != dir->dirpath))
		{
			if (g_libdsmcc_writefile_callback != NULL)
			{
				g_libdsmcc_writefile_callback(newfile_name, dir->dirpath, pstCacheAll->name, (int)oldfile->carousel_id, (int)pstCacheAll->usPid, (int)oldfile->module_id, (int)oldfile->data_len, (unsigned char *)oldfile->data);
			}
		}

		return;
	}

	// See if the data had already arrived for the file

	newfile = dsmcc_cache_file_find_data(pstCacheAll,
				pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id,
				pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.module_id,
				pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len,
				pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey);

	if(newfile == NULL)
	{
		// Create the file from scratch
		newfile = (Cache_File_t*)malloc(sizeof(Cache_File_t));
		if(newfile == NULL)
		{
			return ;
		}
		memset (newfile, 0, sizeof(Cache_File_t));

		newfile->eFileType = DSMCC_OBJTYPE_FILE;

		newfile->carousel_id = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id;
		newfile->module_id = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.module_id;
		newfile->key_len = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len;
		newfile->key= (char *)malloc(newfile->key_len);
		memcpy(newfile->key, pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey, newfile->key_len);
		newfile->data = NULL;
	}
	else
	{
	}

	newfile->filename = (char*)malloc(pstBiopBinding->stBiopName.comps[comp_idx].id_len);
	if(newfile->filename == NULL)
	{
		if(newfile != NULL)
			free(newfile);

		return;
	}

	memcpy (newfile->filename, pstBiopBinding->stBiopName.comps[comp_idx].id, pstBiopBinding->stBiopName.comps[comp_idx].id_len);
	newfile->next = NULL;

	dir = dsmcc_cache_dir_find(pstCacheAll, newfile->carousel_id, mod_id, key_len, key);

	pstCacheAll->num_files++;
	pstCacheAll->total_files++;

	if(dir == NULL)
	{
		/* Parent directory not yet known */
		newfile->p_module_id = mod_id;
		newfile->p_key_len = key_len;
		newfile->p_key = (char *)malloc(newfile->p_key_len);
		if(newfile->p_key == NULL)
		{
			if(newfile->filename!= NULL)
				free(newfile->filename);

			if(newfile != NULL)
				free(newfile);

			return;
		}

		memcpy(newfile->p_key, key, key_len);
		newfile->parent = NULL;
		dsmcc_cache_unknown_file_info(pstCacheAll, newfile);
	}
	else
	{
		/* TODO Check if already stored under directory (new version?)
		*      Checking version info for a file is difficult,
		*      the data should not be passed to us by dsmcc layer
		*      unless the version has changed. Need to remove old
		*      and store new.
		*/
		/* If not append to list */

		newfile->p_key_len = dir->key_len;
		newfile->p_key = malloc(dir->key_len);
		memcpy(newfile->p_key, dir->key, dir->key_len);
		if(newfile->p_key == NULL)
		{
			if(newfile->filename!= NULL)
				free(newfile->filename);

			if(newfile != NULL)
				free(newfile);

			return;
		}

		newfile->parent = dir;
		if(dir->files == NULL)
		{
			dir->files = newfile;
			newfile->prev = NULL;
		}
		else
		{
			for(last=dir->files;last->next!=NULL;last=last->next){;}
			last->next = newfile;
			newfile->prev = last;
		}

		if(newfile->data != NULL)
		{
			dsmcc_cache_write_file(pstCacheAll, newfile);
		}
	}

// Print Debug
#if 0
	{
		int nCount;
		struct cache_file *srchFile;

		if (strncmp (newfile->filename, "ardtext_1001.xml", 12) == 0 ||
			strncmp (newfile->filename, "ardtext_1021.xml", 12) == 0)
		{
			printf ("\n\n ===== File Found in dsmcc_cache_file_info ===== \n");
			printf (" File Name [%s]\n CarID:%d, ModID:%d\n Key ", newfile->filename, newfile->carousel_id, newfile->module_id);
			for (nCount = 0; nCount < newfile->key_len; nCount++)
			{
				printf ("[%02x]", newfile->key[nCount]);
			}
			printf ("\n Parental CarID:%d, Parental ModID:%d\n Parental Key ", newfile->p_carousel_id, newfile->p_module_id);
			for (nCount = 0; nCount < newfile->p_key_len; nCount++)
			{
				printf ("[%02x]", newfile->p_key[nCount]);
			}
			printf ("\n Parent Dir (0x%08x), Data (0x%08x)\n", newfile->parent, newfile->data);
			printf (" File Cache : (0x%08x)\n", pstCacheAll);

			srchFile = dsmcc_cache_file_find (pstCacheAll, newfile->carousel_id, newfile->module_id, newfile->key_len, newfile->key);
			printf (" File found from Cache : (0x%08x) : (0x%08x)\n", srchFile, newfile);


			printf (" ===== File End ===== \n\n\n");
		}
	}
#endif
}

void dsmcc_cache_dir_info(Cache_All_t *pstCacheAll, unsigned short module_id, unsigned int objkey_len, char *objkey, BIOP_Binding_t *pstBiopBinding, unsigned int comp_idx)
{
	//unsigned char	 bWriteFile;
	Cache_Dir_t *dir, *last, *subdir, *nextDir;
	Cache_File_t *file, *nf;

	dir = dsmcc_cache_dir_find(pstCacheAll,
			   pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id,
			   pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.module_id,
			   pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len,
			   pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey);

	if(dir != NULL) { return; }	/* Already got (check version TODO) */

	dir = (Cache_Dir_t *)malloc(sizeof(Cache_Dir_t));
	if(dir == NULL)
	{
		return;
	}
	memset (dir, 0, sizeof(Cache_Dir_t));

	dir->name = (char *)malloc(pstBiopBinding->stBiopName.comps[comp_idx].id_len);
	if(dir->name == NULL)
	{
		if(dir != NULL)
			free(dir);

		return;
	}
	memcpy(dir->name, pstBiopBinding->stBiopName.comps[comp_idx].id, pstBiopBinding->stBiopName.comps[comp_idx].id_len);

	dir->dirpath = NULL;
	dir->next = dir->prev = dir->sub = NULL;
	dir->files = NULL;
	dir->carousel_id = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id;
	dir->module_id = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.module_id;
	dir->key_len = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len;
	dir->key = (char *)malloc(dir->key_len);
	if(dir->key == NULL)
	{
		if(dir->name != NULL)
			free(dir->name);

		if(dir != NULL)
			free(dir);

			return;
	}

	memcpy(dir->key, pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey, dir->key_len);

	dir->p_module_id = module_id;
	dir->p_key_len = objkey_len;
	dir->p_key = (char *)malloc(dir->p_key_len);
	if(dir->p_key == NULL)
	{
		if(dir->key != NULL)
			free(dir->key);

		if(dir->name != NULL)
			free(dir->name);

		if(dir != NULL)
			free(dir);

		return;
	}

	memcpy(dir->p_key, objkey, objkey_len);

	dir->parent = dsmcc_cache_dir_find (pstCacheAll, dir->carousel_id, module_id, objkey_len, objkey);
	if(dir->parent == NULL)
	{
		if(pstCacheAll->dir_cache == NULL)
		{
			pstCacheAll->dir_cache = dir;
		}
		else
		{
			/* Directory not yet known. Add this to dirs list */
			for (last=pstCacheAll->dir_cache;last->next!=NULL;last=last->next){;}
			last->next = dir;
			dir->prev = last;
		}
	}
	else
	{
		/* Create under parent directory */
		if(dir->parent->sub == NULL)
		{
			dir->parent->sub = dir;
		}
		else
		{
			for (last=dir->parent->sub; last->next!=NULL; last=last->next) { ; }
			last->next = dir;
			dir->prev = last;
		}
	}

	/* Attach any files that arrived previously */
	file = pstCacheAll->file_cache;
	while (file != NULL)
	{
		nf = file->next;

		if ((file->carousel_id == dir->carousel_id) &&
			(file->p_module_id == dir->module_id) &&
			dsmcc_cache_key_cmp(file->p_key, dir->key, file->p_key_len, dir->key_len))
		{
			dsmcc_cache_attach_file (pstCacheAll, dir, file);
		}

		file = nf;
	}

	/* Attach any subdirs that arrived beforehand */
	subdir = pstCacheAll->dir_cache;
	while (subdir != NULL)
	{
		nextDir = subdir->next;
		dsmcc_cache_attach_dir (pstCacheAll, dir, subdir);
		subdir = nextDir;
	}

	if ((dir->parent!=NULL) && (dir->parent->dirpath!=NULL))
	{
		dsmcc_cache_write_dir (pstCacheAll, dir);	/* Write dir/files to filesystem */
	}

	pstCacheAll->num_dirs++; pstCacheAll->total_dirs++;
}

void dsmcc_cache_event (Cache_All_t *pstCacheAll, BIOP_Message_t  *pstBiopMsg, CacheModuleData_t *pstCacheModuleData)
{
	Cache_Dir_t	*dir;
	Cache_File_t	*last;
	Cache_File_t	*file;

	BIOP_Message_Header_t  *pstBiopMsgHeader = &(pstBiopMsg->stBiopMsgHeader);

	file = dsmcc_cache_file_find (pstCacheAll, pstCacheModuleData->carousel_id, pstCacheModuleData->module_id, pstBiopMsg->stBiopMsgHeader.objkey_len, pstBiopMsg->stBiopMsgHeader.objkey);
	if(file == NULL)
	{
		file = (Cache_File_t *)malloc(sizeof(Cache_File_t));
		if (file == NULL)
		{
			return;
		}

		memset (file, 0, sizeof(Cache_File_t));
		file->eFileType = DSMCC_OBJTYPE_EVENT;

		dsmcc_cache_make_file_from_event (pstBiopMsg, pstCacheModuleData, file);

		file->carousel_id = pstCacheModuleData->carousel_id;
		file->module_id = pstCacheModuleData->module_id;
		file->key_len = pstBiopMsgHeader->objkey_len;
		file->key = (char*)malloc(file->key_len);
		if(file->key == NULL)
		{
			if(file != NULL)
				free(file);

			return;
		}

		memcpy (file->key, pstBiopMsgHeader->objkey, file->key_len);
		file->next = file->prev = NULL;
		// Add to unknown data cache

		if(pstCacheAll->data_cache == NULL)
		{
			pstCacheAll->data_cache = file;
		}
		else
		{
			for (last=pstCacheAll->data_cache;last->next!=NULL;last=last->next){;}

			last->next = file;
			file->prev = last;
		}
	}

	if (file->eFileType != DSMCC_OBJTYPE_EVENT)
	{
		if (file != NULL)
			free(file);

		return;
	}

	if (file->parent == NULL && (file->p_key != NULL))
	{
		dir = dsmcc_cache_dir_find (pstCacheAll, file->p_carousel_id, file->p_module_id, file->p_key_len, file->p_key);
		if (dir != NULL)
		{
			dsmcc_cache_attach_file (pstCacheAll, dir, file);
		}
	}

	if(file->data == NULL)
	{
		dsmcc_cache_make_file_from_event (pstBiopMsg, pstCacheModuleData, file);

		/* TODO this should be a config option */
		dsmcc_cache_write_file(pstCacheAll, file);
	}
}

void dsmcc_cache_event_info(Cache_All_t *pstCacheAll, unsigned short mod_id, unsigned int key_len, char *key, BIOP_Binding_t *pstBiopBinding, unsigned int comp_idx)
{
	Cache_File_t *newfile, *last;
	Cache_Dir_t *dir;

	// Check we do not already have file (or file info) cached
	if(dsmcc_cache_file_find(pstCacheAll,
			pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id,
			pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.module_id,
			pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len,
			pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey) != NULL) {
		return;
	}

	// See if the data had already arrived for the file

	newfile = dsmcc_cache_file_find_data(pstCacheAll,
				pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id,
				pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.module_id,
				pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len,
				pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey);

	if(newfile == NULL)
	{
		// Create the file from scratch
		newfile = (Cache_File_t*)malloc(sizeof(Cache_File_t));
		if(newfile== NULL)
		{
			return;
		}

		memset (newfile, 0, sizeof(Cache_File_t));

		newfile->eFileType = DSMCC_OBJTYPE_EVENT;

		newfile->carousel_id = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.carousel_id;
		newfile->module_id = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.module_id;
		newfile->key_len = pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey_len;
		newfile->key= (char *)malloc(newfile->key_len);
		memcpy(newfile->key, pstBiopBinding->stBiopIor.profile.stBiopProfileBody.stBiopObjLocation.objkey, newfile->key_len);
		newfile->data = NULL;
	}
	else
	{
	}

	newfile->filename = (char*)malloc(pstBiopBinding->stBiopName.comps[comp_idx].id_len);
	if(newfile->filename== NULL)
	{
		if(newfile != NULL)
			free(newfile);

		return;
	}


	memcpy (newfile->filename, pstBiopBinding->stBiopName.comps[comp_idx].id, pstBiopBinding->stBiopName.comps[comp_idx].id_len);
	newfile->next = NULL;

	dir = dsmcc_cache_dir_find(pstCacheAll, newfile->carousel_id, mod_id, key_len, key);

	pstCacheAll->num_files++;
	pstCacheAll->total_files++;

	if(dir == NULL)
	{
		/* Parent directory not yet known */
		newfile->p_module_id = mod_id;
		newfile->p_key_len = key_len;
		newfile->p_key = (char *)malloc(newfile->p_key_len);
		if(newfile->p_key == NULL)
		{
			if(newfile->filename!= NULL)
				free(newfile->filename);

			if(newfile != NULL)
				free(newfile);

			return;
		}

		memcpy(newfile->p_key, key, key_len);
		newfile->parent = NULL;
		dsmcc_cache_unknown_file_info(pstCacheAll, newfile);
	}
	else
	{
		/* TODO Check if already stored under directory (new version?)
		*      Checking version info for a file is difficult,
		*      the data should not be passed to us by dsmcc layer
		*      unless the version has changed. Need to remove old
		*      and store new.
		*/
		/* If not append to list */

		newfile->p_key_len = dir->key_len;
		newfile->p_key = malloc(dir->key_len);
		if(newfile->p_key == NULL)
		{
			if(newfile->filename!= NULL)
				free(newfile->filename);

			if(newfile != NULL)
				free(newfile);

			return;
		}

		memcpy(newfile->p_key, dir->key, dir->key_len);
		newfile->parent = dir;
		if(dir->files == NULL)
		{
			dir->files = newfile;
			newfile->prev = NULL;
		}
		else
		{
			for(last=dir->files;last->next!=NULL;last=last->next){;}
			last->next = newfile;
			newfile->prev = last;
		}

		if(newfile->data != NULL)
		{
			dsmcc_cache_write_file(pstCacheAll, newfile);
		}
	}

// Print Debug
#if 0
	{
		int nCount;
		struct cache_file *srchFile;

		if (strncmp (newfile->filename, "ardtext_1001.xml", 12) == 0 ||
			strncmp (newfile->filename, "ardtext_1021.xml", 12) == 0)
		{
			printf ("\n\n ===== File Found in dsmcc_cache_file_info ===== \n");
			printf (" File Name [%s]\n CarID:%d, ModID:%d\n Key ", newfile->filename, newfile->carousel_id, newfile->module_id);
			for (nCount = 0; nCount < newfile->key_len; nCount++)
			{
				printf ("[%02x]", newfile->key[nCount]);
			}
			printf ("\n Parental CarID:%d, Parental ModID:%d\n Parental Key ", newfile->p_carousel_id, newfile->p_module_id);
			for (nCount = 0; nCount < newfile->p_key_len; nCount++)
			{
				printf ("[%02x]", newfile->p_key[nCount]);
			}
			printf ("\n Parent Dir (0x%08x), Data (0x%08x)\n", newfile->parent, newfile->data);
			printf (" File Cache : (0x%08x)\n", pstCacheAll);

			srchFile = dsmcc_cache_file_find (pstCacheAll, newfile->carousel_id, newfile->module_id, newfile->key_len, newfile->key);
			printf (" File found from Cache : (0x%08x) : (0x%08x)\n", srchFile, newfile);


			printf (" ===== File End ===== \n\n\n");
		}
	}
#endif
}

