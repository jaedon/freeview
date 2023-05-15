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
// #include <mpatrol.h>


#if defined(CONFIG_ENHANCED_SECURITY)
#define	mkdir	PAL_SYS_MkDir
#endif

FILE *cache_fd = NULL;
/* TODO This should be stored in obj_carousel structure  */

dsmcc_writefile_callback		 g_libdsmcc_writefile_callback = NULL;
dsmcc_writeevt_callback			 g_libdsmcc_writeevt_callback = NULL;
dsmcc_writedir_callback			 g_libdsmcc_writedir_callback = NULL;


void dsmcc_cache_init(struct cache *filecache, const char *channel_name, FILE *debug_fd)
{

	/* TODO - load cache from disk into obj_carousel */

	filecache->gateway = NULL;
	filecache->dir_cache = NULL;

	filecache->file_cache = NULL;
	filecache->data_cache = NULL;

	filecache->event_cache = NULL;
	filecache->event_data_cache = NULL;

	/* Write contents under channel name */
	if(channel_name)
	{
		filecache->name = (char*)malloc(strlen(channel_name)+1);
		strcpy(filecache->name, channel_name);
	}
	else
	{
		filecache->name = (char*)malloc(1);
		filecache->name = '\0';
	}

	mkdir("/tmp/cache", 0755);	/* By popular demand... */

	filecache->num_files	= 0;
	filecache->total_files	= 0;

	filecache->num_dirs		= 0;
	filecache->total_dirs	= 0;

	filecache->num_events	= 0;
	filecache->total_events	= 0;

	filecache->debug_fd = debug_fd;
}

void
dsmcc_cache_free(struct cache *filecache) {
	struct cache_file *f, *fn;
	struct cache_dir *d, *dn;

	/* Free unconnected files */
	f = filecache->file_cache;
	while(f!=NULL) {
	  fn = f->next;
	  if(f->key_len>0) { free(f->key); }
	  if(f->filename!=NULL) { free(f->filename); }
	  if(f->data!=NULL) { free(f->data); }
	  if(f->p_key_len>0) { free(f->p_key); }
	  free(f);
	  f = fn;
	}
	/* Free cached data */
	f = filecache->data_cache;
	while(f!=NULL) {
	  fn = f->next;
	  if(f->key_len>0) { free(f->key); }
	  if(f->data!=NULL) { free(f->data); }
	  free(f);
	  f = fn;
	}
	/* Free unconnected dirs */
	d = filecache->dir_cache;
	while(d!=NULL) {
	  dn = d->next;
	  if(d->name!=NULL) { free(d->name); }
	  if(d->dirpath!=NULL) { free(d->dirpath); }
	  if(d->key_len>0) { free(d->key); }
	  if(d->p_key_len>0) { free(d->p_key); }
	  f = d->files;
	  while(f!=NULL) {
	    fn = f->next;
	    if(f->key_len>0) { free(f->key); }
	    if(f->filename!=NULL) { free(f->filename); }
	    if(f->data!=NULL) { free(f->data); }
	    if(f->p_key_len>0) { free(f->p_key); }
	    free(f);
	    f = fn;
	  }
	  free(d);
	  d = dn;
	}

	/* Free cache - TODO improve this */

	if(filecache->gateway != NULL)
		dsmcc_cache_free_dir(filecache->gateway);

	filecache->file_cache = filecache->data_cache = NULL;
	filecache->gateway = filecache->dir_cache = NULL;

	if(filecache->name)
		free(filecache->name);
}

void
dsmcc_cache_free_dir(struct cache_dir *d) {
	struct cache_dir *dn, *dnn;
	struct cache_file *f, *fn;

	if(d->sub!=NULL) {
		dn = d->sub;
		while(dn!=NULL) {
		  dnn = dn->next;
		  dsmcc_cache_free_dir(dn);
		  dn = dnn;
		}
	}

	if(d->name!=NULL) { free(d->name); }
	if(d->dirpath!=NULL) { free(d->dirpath); }
	if(d->key_len>0) { free(d->key); }
	if(d->p_key_len>0) { free(d->p_key); }
	f = d->files;
	while(f!=NULL) {
	  fn = f->next;
	  if(f->key_len>0) { free(f->key); }
	  if(f->filename!=NULL) { free(f->filename); }
	  if(f->data!=NULL) { free(f->data); }
	  if(f->p_key_len>0) { free(f->p_key); }
	  free(f);
	  f = fn;
	}
	free(d);
}

unsigned int
dsmcc_cache_key_cmp(char *str1, char *str2, unsigned int len1, unsigned int len2) {
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

struct cache_dir *dsmcc_cache_scan_dir (struct cache_dir *dir, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key)
{
	struct cache_dir *founddir, *subdir;

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

struct cache_dir *dsmcc_cache_dir_find (struct cache *filecache, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key)
{
	struct cache_dir *dir, *fdir, *nextDir;
	struct cache_file *file, *nf;

	/* Scan through known dirs and return details if known else NULL */
	if(module_id == 0 && key_len == 0)
	{
		/* Return gateway object. Create if not already */
		if(filecache->gateway == NULL)
		{
			filecache->gateway = (struct cache_dir *)malloc(sizeof(struct cache_dir));
			memset (filecache->gateway, 0, sizeof(struct cache_dir));

			filecache->gateway->name = (char *)malloc(2);
			filecache->gateway->carousel_id = car_id;
			filecache->gateway->module_id = filecache->gateway->key_len =
			filecache->gateway->p_key_len = 0;

			/*TODO argg! a hack to fix a bug caused by a hack.Need better linking */
			strcpy(filecache->gateway->name, "/");
			filecache->gateway->dirpath = (char *)malloc(2);
			strcpy(filecache->gateway->dirpath, "/");
			filecache->gateway->sub = filecache->gateway->parent =
			filecache->gateway->prev = filecache->gateway->next = NULL;
			filecache->gateway->files = NULL;

			/* Attach any subdirs or files that arrived prev. */
			for(file=filecache->file_cache; file!=NULL; file=nf)
			{
				nf=file->next;
				if ((file->carousel_id == filecache->gateway->carousel_id) &&
					(file->p_module_id == filecache->gateway->module_id) &&
 					dsmcc_cache_key_cmp (file->p_key, filecache->gateway->key, file->p_key_len, filecache->gateway->key_len))
				{
					dsmcc_cache_attach_file (filecache,filecache->gateway, file);
				}
			}

			fdir = filecache->dir_cache;
			while (fdir != NULL)
			{
				nextDir = fdir->next;
				dsmcc_cache_attach_dir (filecache, filecache->gateway, fdir);
				fdir = nextDir;
			}

			dsmcc_cache_write_dir (filecache, filecache->gateway);	/* Write files to filesystem */
		}

		return filecache->gateway;
	}

	/* Find dir magic */
	dir = dsmcc_cache_scan_dir(filecache->gateway, car_id, module_id, key_len, key);
	if (dir == NULL)
	{	/* Try looking in unlinked dirs list */
		for (fdir=filecache->dir_cache; (dir==NULL) && (fdir!=NULL); fdir = fdir->next)
		{
			dir = dsmcc_cache_scan_dir(fdir, car_id, module_id, key_len, key);
		}
	}

	/* TODO - Directory not known yet, cache it ? */

	return dir;
}

void dsmcc_cache_attach_file(struct cache *filecache, struct cache_dir *root, struct cache_file *file)
{
	struct cache_file *cfile;

	/* Search for any files that arrived previously in unknown files list*/
	if(root->files == NULL)
	{
		if(file->prev!=NULL)
		{
			file->prev->next = file->next;
		}
		else
		{
			filecache->file_cache=file->next;
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
			filecache->file_cache=file->next;
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

void dsmcc_cache_attach_dir(struct cache *filecache, struct cache_dir *root, struct cache_dir *dir)
{
	struct cache_dir *last;

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
			filecache->dir_cache = dir->next;
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

struct cache_file *dsmcc_cache_scan_file(struct cache_dir *dir, unsigned long car_id, unsigned int mod_id, unsigned int key_len, char *key)
{
	struct cache_file *file;
	struct cache_dir *subdir;

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

struct cache_file *dsmcc_cache_file_find(struct cache *filecache, unsigned long car_id, unsigned short module_id, unsigned int key_len, char *key)
{
	struct cache_dir  *dir;
	struct cache_file *file;

	 /* Try looking in parent-less list */

	for(file = filecache->file_cache; file != NULL; file = file->next)
	{
		if((file->carousel_id == car_id) &&
			(file->module_id == module_id) &&
			dsmcc_cache_key_cmp(file->key, key, file->key_len, key_len))
		{
			return file;
		}
	}

	/* Try looking in parent-less dir list */
	for (dir = filecache->dir_cache; dir != NULL; dir = dir->next)
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
	file = dsmcc_cache_scan_file(filecache->gateway, car_id, module_id, key_len, key);

	return file;
}

void dsmcc_cache_dir_info(struct cache *filecache, unsigned short module_id, unsigned int objkey_len, char *objkey, struct biop_binding *bind, unsigned int comp_idx)
{
	unsigned char	 bWriteFile;
	struct cache_dir *dir, *last, *subdir, *nextDir;
	struct cache_file *file, *nf;

	dir = dsmcc_cache_dir_find(filecache,
			   bind->ior.body.full.obj_loc.carousel_id,
			   bind->ior.body.full.obj_loc.module_id,
			   bind->ior.body.full.obj_loc.objkey_len,
			   bind->ior.body.full.obj_loc.objkey);

	if(dir != NULL) { return; }	/* Already got (check version TODO) */

	dir = (struct cache_dir *)malloc(sizeof(struct cache_dir));
	memset (dir, 0, sizeof(struct cache_dir));

	dir->name = (char *)malloc(bind->name.comps[comp_idx].id_len);
	memcpy(dir->name, bind->name.comps[comp_idx].id, bind->name.comps[comp_idx].id_len);

	dir->dirpath = NULL;
	dir->next = dir->prev = dir->sub = NULL;
	dir->files = NULL;
	dir->carousel_id = bind->ior.body.full.obj_loc.carousel_id;
	dir->module_id = bind->ior.body.full.obj_loc.module_id;
	dir->key_len = bind->ior.body.full.obj_loc.objkey_len;
	dir->key = (char *)malloc(dir->key_len);
	memcpy(dir->key, bind->ior.body.full.obj_loc.objkey, dir->key_len);

//	dir->p_carousel_id = carousel_id; Must be the same ?

	dir->p_module_id = module_id;
	dir->p_key_len = objkey_len;
	dir->p_key = (char *)malloc(dir->p_key_len);
	memcpy(dir->p_key, objkey, objkey_len);

	dir->parent = dsmcc_cache_dir_find (filecache, dir->carousel_id, module_id, objkey_len, objkey);
	if(dir->parent == NULL)
	{
		if(filecache->dir_cache == NULL)
		{
			filecache->dir_cache = dir;
		}
		else
		{
			/* Directory not yet known. Add this to dirs list */
			for (last=filecache->dir_cache;last->next!=NULL;last=last->next){;}
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
	file = filecache->file_cache;
	while (file != NULL)
	{
		nf = file->next;

		if ((file->carousel_id == dir->carousel_id) &&
			(file->p_module_id == dir->module_id) &&
			dsmcc_cache_key_cmp(file->p_key, dir->key, file->p_key_len, dir->key_len))
		{
			dsmcc_cache_attach_file (filecache, dir, file);
		}

		file = nf;
	}

	/* Attach any subdirs that arrived beforehand */
	subdir = subdir = filecache->dir_cache;
	while (subdir != NULL)
	{
		nextDir = subdir->next;
		dsmcc_cache_attach_dir (filecache, dir, subdir);
		subdir = nextDir;
	}

	if ((dir->parent!=NULL) && (dir->parent->dirpath!=NULL))
	{
		dsmcc_cache_write_dir (filecache, dir);	/* Write dir/files to filesystem */
	}

	filecache->num_dirs++; filecache->total_dirs++;
}

void dsmcc_cache_write_dir(struct cache *filecache, struct cache_dir *dir)
{
	struct cache_dir *subdir;
	struct cache_file *file;
	char dirbuf[256];

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
		g_libdsmcc_writedir_callback (dir->name, dir->dirpath, filecache->name, filecache->pid);
	}
	else
	{
		sprintf(dirbuf, "%s/%s/%s", "/tmp/cache/", filecache->name, dir->dirpath);
		if(filecache->debug_fd != NULL) {
			fprintf(filecache->debug_fd,"[libcache] Writing directory %s to filesystem\n", dir->dirpath);
		}

		mkdir(dirbuf, 0755);
	}

#if 0
#if 1 /* by Dylan = set URL prefix */
	if (strlen(dir->name) == 1 && strncmp(dir->name, "/", 1) == 0)
	{
		sprintf(dirbuf,"/tmp/cache/%u", filecache->idx);
		mkdir(dirbuf, 0755);
		memset(dirbuf, 0x0, 256);
		sprintf(dirbuf,"/tmp/cache/%u/%u", filecache->idx, filecache->pid);
		mkdir(dirbuf, 0755);
		memset(dirbuf, 0x0, 256);
	}
	else
	{
		sprintf(dirbuf, "%s/%u/%u/%s/%s", "/tmp/cache", filecache->idx, filecache->pid, filecache->name, dir->dirpath);
		if(filecache->debug_fd != NULL) {
			fprintf(filecache->debug_fd,"[libcache] Writing directory %s to filesystem\n", dir->dirpath);
  		}
		mkdir(dirbuf, 0755);
	}

#else
	sprintf(dirbuf, "%s/%s/%s", "/tmp/cache/", filecache->name, dir->dirpath);
	if(filecache->debug_fd != NULL) {
		fprintf(filecache->debug_fd,"[libcache] Writing directory %s to filesystem\n", dir->dirpath);
  	}
	mkdir(dirbuf, 0755);
#endif
#endif

	/* Write out files that had arrived before directory */
	for (file=dir->files; file!=NULL; file=file->next)
	{
		if(file->data != NULL)
		{
			dsmcc_cache_write_file(filecache, file);
		}
	}

	/* Recurse thorugh child directories */

	for(subdir=dir->sub;subdir!=NULL;subdir=subdir->next) {
		dsmcc_cache_write_dir(filecache, subdir);
	}

}

void dsmcc_cache_file (struct cache *filecache, struct biop_message *bm, struct cache_module_data *cachep)
{
	struct cache_dir	*dir;
	struct cache_file	*last;
	struct cache_file	*file;

	file = dsmcc_cache_file_find (filecache, cachep->carousel_id, cachep->module_id, bm->hdr.objkey_len, bm->hdr.objkey);
	if(file == NULL)
	{
		file = (struct cache_file *)malloc(sizeof(struct cache_file));
		memset (file, 0, sizeof(struct cache_file));

		file->eFileType = DSMCC_OBJTYPE_FILE;

		file->data_len = bm->body.file.content_len;
		file->data = (char*)malloc(file->data_len);
		memcpy (file->data, cachep->data+cachep->curp, file->data_len);
		file->carousel_id = cachep->carousel_id;
		file->module_id = cachep->module_id;
		file->key_len = bm->hdr.objkey_len;
		file->key = (char*)malloc(file->key_len);
		memcpy (file->key, bm->hdr.objkey, file->key_len);
		file->next = file->prev = NULL;
		// Add to unknown data cache

		if(filecache->data_cache == NULL)
		{
			filecache->data_cache = file;
		}
		else
		{
			for (last=filecache->data_cache;last->next!=NULL;last=last->next){;}

			last->next = file;
			file->prev = last;
		}
	}

	if (file->parent == NULL && (file->p_key != NULL))
	{
		dir = dsmcc_cache_dir_find (filecache, file->p_carousel_id, file->p_module_id, file->p_key_len, file->p_key);
		if (dir != NULL)
		{
			dsmcc_cache_attach_file (filecache, dir, file);
		}
	}

	// Print Debug
#if 0
{
	int nCount;

	if (file->carousel_id == 11 && file->module_id == 5 && file->key_len == 2 && file->key[0] == 0x08 && file->key[1] ==0x07)
	{
		printf ("\n\n ===== File Found in dsmcc_cache_file ===== \n");
		printf (" CarID:%d, ModID:%d\n Key ", file->carousel_id, file->module_id);
		for (nCount = 0; nCount < file->key_len; nCount++)
		{
			printf ("[%02x]", file->key[nCount]);
		}
		printf ("\n Parental CarID:%d, Parental ModID:%d\n Parental Key ", file->p_carousel_id, file->p_module_id);
		for (nCount = 0; nCount < file->p_key_len; nCount++)
		{
			printf ("[%02x]", file->p_key[nCount]);
		}
		printf ("\n Parent Dir (0x%08x), Data (0x%08x)\n", file->parent, file->data);
		printf (" File Cache : (0x%08x)\n", filecache);
		printf (" ===== File End ===== \n\n\n");
	}
}
#endif

	if(file->data == NULL)
	{
		file->data_len = bm->body.file.content_len;
		file->data = (char *)malloc(file->data_len);
		memcpy(file->data,cachep->data+cachep->curp,
						file->data_len);
		/* TODO this should be a config option */
		dsmcc_cache_write_file(filecache, file);
	}


}

void dsmcc_cache_make_file_from_event (struct biop_message *bm, struct cache_module_data *cachep, struct cache_file *file)
{
	unsigned int	 nSvcCntxCnt;
	unsigned int	 nObjInfoLen, nContentLen;
	unsigned char	*pucObjInfo, *pucContent;

	nObjInfoLen	= bm->hdr.objinfo_len;
	pucObjInfo	= bm->hdr.objinfo;

	nContentLen = 0;
	pucContent = cachep->data + cachep->curp;

	/* skip service contect count */
	nSvcCntxCnt = (int)(pucContent[nContentLen]);
	nContentLen += (nSvcCntxCnt + 1);

	/* skip msgbody len */
	nContentLen += 4;

	/* skip msgbody */
	nContentLen += bm->body.eve.msgbody_len;

	file->data_len = (nObjInfoLen + nContentLen + 2);
	file->data = (char *)malloc (file->data_len);

	file->data[0] = (char)((nObjInfoLen >> 8) & 0xff);
	file->data[1] = (char)(nObjInfoLen & 0xff);

	if (nObjInfoLen > 0)
	{
		memcpy (file->data + 2, pucObjInfo, nObjInfoLen);
	}

	memcpy (file->data + 2 + nObjInfoLen, pucContent, nContentLen);
}

void dsmcc_cache_make_event_from_raw (unsigned char *szFileName, unsigned char *szFilePath, unsigned char *pucData, int nDataLen, char *szChanName, int nPid)
{
	int 				  nCount, nEvtNameCnt, nEvtIdCnt, nTotalNum;
	int					  nObjInfoLen;
	int					  nSvcContextCnt, nContextDataLen;
	int					  nTapsCnt;
	int					 *pnEvtId;
	unsigned char		  ucDescLen, ucEvtNameLen;
	unsigned char		 *szDesc;
	unsigned char		**pszEvtName;
	unsigned char		 *pucBuf, *pucMsgBody;
	unsigned char		 *pucObjInfoByte;
	libdsmcc_event_t	  stDsmccEvt;

	pucBuf = (unsigned char *)pucData;

	nObjInfoLen =  (((int)pucBuf[0]) << 8) | (int)pucBuf[1];
	pucBuf += 2;
	pucMsgBody = pucBuf + nObjInfoLen;

	// 1. Description
	ucDescLen = *pucBuf;
	pucBuf++;
	nObjInfoLen--;

	szDesc = malloc (ucDescLen + 1);
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
	nEvtNameCnt = (((int)pucBuf[0]) << 8) | (int)pucBuf[1];
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
		nContextDataLen = (((int)pucBuf[0]) << 8) | (int)pucBuf[1];
		pucBuf ++;

		// context_data_byte : skip
		pucBuf += nContextDataLen;
	}

	// messageBody_length : skip
	pucBuf += 4;

	// taps_count
	nTapsCnt = pucBuf[0];
	pucBuf ++;

	// stream body : Skip
	pucBuf += (nTapsCnt * 7);

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
			pnEvtId[nCount] = ((int)pucBuf[0] << 8) | ((int)pucBuf[1]);
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
			stDsmccEvt.nDescriptionLen	= (int)ucDescLen;
			stDsmccEvt.szDescription	= (unsigned char *)szDesc;
			stDsmccEvt.szEventName		= (unsigned char *)pszEvtName[nCount];
			stDsmccEvt.nObjInfoByteLen	= (int)nObjInfoLen;
			stDsmccEvt.pucObjInfoByte	= (unsigned char *)pucObjInfoByte;
			stDsmccEvt.nEvtId			= (int)pnEvtId[nCount];

			g_libdsmcc_writeevt_callback (szFileName, szFilePath, &stDsmccEvt, szChanName, nPid);
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


void dsmcc_cache_event (struct cache *filecache, struct biop_message *bm, struct cache_module_data *cachep)
{
	unsigned char		 ucEvtCnt;
	unsigned long		 ulTapSize, ulEvtIdSize;
	struct cache_dir	*dir;
	struct cache_file	*last;
	struct cache_file	*file;

	file = dsmcc_cache_file_find (filecache, cachep->carousel_id, cachep->module_id, bm->hdr.objkey_len, bm->hdr.objkey);
	if(file == NULL)
	{
		file = (struct cache_file *)malloc(sizeof(struct cache_file));
		if (file == NULL)
		{
			return;
		}

		memset (file, 0, sizeof(struct cache_file));
		file->eFileType = DSMCC_OBJTYPE_EVENT;

		dsmcc_cache_make_file_from_event (bm, cachep, file);

		file->carousel_id = cachep->carousel_id;
		file->module_id = cachep->module_id;
		file->key_len = bm->hdr.objkey_len;
		file->key = (char*)malloc(file->key_len);
		memcpy (file->key, bm->hdr.objkey, file->key_len);
		file->next = file->prev = NULL;
		// Add to unknown data cache

		if(filecache->data_cache == NULL)
		{
			filecache->data_cache = file;
		}
		else
		{
			for (last=filecache->data_cache;last->next!=NULL;last=last->next){;}

			last->next = file;
			file->prev = last;
		}
	}

	if (file->eFileType != DSMCC_OBJTYPE_EVENT)
	{
		return;
	}

	if (file->parent == NULL && (file->p_key != NULL))
	{
		dir = dsmcc_cache_dir_find (filecache, file->p_carousel_id, file->p_module_id, file->p_key_len, file->p_key);
		if (dir != NULL)
		{
			dsmcc_cache_attach_file (filecache, dir, file);
		}
	}

	if(file->data == NULL)
	{
		dsmcc_cache_make_file_from_event (bm, cachep, file);

		/* TODO this should be a config option */
		dsmcc_cache_write_file(filecache, file);
	}
}


void dsmcc_cache_write_file(struct cache *filecache, struct cache_file *file)
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
				dsmcc_cache_make_event_from_raw (file->filename, file->parent->dirpath, file->data, file->data_len, filecache->name, (int)filecache->pid);
			}

			break;

		case DSMCC_OBJTYPE_FILE:
		default:
			if (g_libdsmcc_writefile_callback != NULL)
			{
				g_libdsmcc_writefile_callback (file->filename, file->parent->dirpath, filecache->name, (int)filecache->pid, (int)file->data_len, (unsigned char *)file->data);
			}
			else
			{
				sprintf(buf,"/tmp/cache/%s/%s/%s", filecache->name, file->parent->dirpath, file->filename);

				data_fd = fopen(buf, "wb");
				fwrite(file->data, 1, file->data_len, data_fd);
				fclose(data_fd);
			}

			break;
		}

		/* Free data as no longer needed */
		if (file->data != NULL)
		{
			free(file->data);
			file->data = NULL;
		}

		file->data_len = 0;

		filecache->num_files--;
	}
	else
	{
		// printf ("- File Write Faild : [%s]\n", file->filename);
	}
}

void
dsmcc_cache_unknown_dir_info(struct cache *filecache, struct cache_dir *newdir) {
	struct cache_dir *last;

	if(filecache->dir_cache == NULL) {
		filecache->dir_cache = newdir;
		newdir->next = newdir->prev = NULL;
	} else {
		for(last=filecache->dir_cache;last->next!=NULL;last=last->next) { ; }
		last->next = newdir;
		newdir->prev = last;
		newdir->next = NULL;
	}
}

void dsmcc_cache_unknown_file_info(struct cache *filecache, struct cache_file *newfile)
{
	struct cache_file *last;

/* TODO Check if already unknown file (i.e. data arrived twice before
 * dir/srg or missed dir/srg message, if so skip.
 */

	if(filecache->file_cache == NULL)
	{
		filecache->file_cache = newfile;
		filecache->file_cache->next = filecache->file_cache->prev = NULL;
	}
	else
	{
		for(last=filecache->file_cache;last->next!=NULL; last = last->next) { ; }
		last->next = newfile;
		newfile->prev = last;
		newfile->next = NULL;
	}
}

struct cache_file *dsmcc_cache_file_find_data(struct cache *filecache, unsigned long car_id, unsigned short mod_id, unsigned int key_len, char *key)
{
	struct cache_file *last;

	for(last=filecache->data_cache; last!=NULL; last = last->next)
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
				filecache->data_cache = last->next;
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

void dsmcc_cache_file_info(struct cache *filecache, unsigned short mod_id, unsigned int key_len, char *key, struct biop_binding *bind, unsigned int comp_idx)
{
	struct cache_file *newfile, *last;
	struct cache_dir *dir;

	// Check we do not already have file (or file info) cached
	if(dsmcc_cache_file_find(filecache,
			bind->ior.body.full.obj_loc.carousel_id,
			bind->ior.body.full.obj_loc.module_id,
			bind->ior.body.full.obj_loc.objkey_len,
			bind->ior.body.full.obj_loc.objkey) != NULL) {
		return;
	}

	// See if the data had already arrived for the file

	newfile = dsmcc_cache_file_find_data(filecache,
				bind->ior.body.full.obj_loc.carousel_id,
				bind->ior.body.full.obj_loc.module_id,
				bind->ior.body.full.obj_loc.objkey_len,
				bind->ior.body.full.obj_loc.objkey);

	if(newfile == NULL)
	{
		// Create the file from scratch
		newfile = (struct cache_file*)malloc(sizeof(struct cache_file));
		memset (newfile, 0, sizeof(struct cache_file));

		newfile->eFileType = DSMCC_OBJTYPE_FILE;

		newfile->carousel_id = bind->ior.body.full.obj_loc.carousel_id;
		newfile->module_id = bind->ior.body.full.obj_loc.module_id;
		newfile->key_len = bind->ior.body.full.obj_loc.objkey_len;
		newfile->key= (char *)malloc(newfile->key_len);
		memcpy(newfile->key, bind->ior.body.full.obj_loc.objkey,
							newfile->key_len);
		newfile->data = NULL;
	}
	else
	{
	}

	newfile->filename = (char*)malloc(bind->name.comps[comp_idx].id_len);
	memcpy (newfile->filename, bind->name.comps[comp_idx].id, bind->name.comps[comp_idx].id_len);
	newfile->next = NULL;

	dir = dsmcc_cache_dir_find(filecache, newfile->carousel_id, mod_id, key_len, key);

	filecache->num_files++;
	filecache->total_files++;

	if(dir == NULL)
	{
		/* Parent directory not yet known */
		newfile->p_module_id = mod_id;
		newfile->p_key_len = key_len;
		newfile->p_key = (char *)malloc(newfile->p_key_len);
		memcpy(newfile->p_key, key, key_len);
		newfile->parent = NULL;
		dsmcc_cache_unknown_file_info(filecache, newfile);
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
			dsmcc_cache_write_file(filecache, newfile);
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
			printf (" File Cache : (0x%08x)\n", filecache);

			srchFile = dsmcc_cache_file_find (filecache, newfile->carousel_id, newfile->module_id, newfile->key_len, newfile->key);
			printf (" File found from Cache : (0x%08x) : (0x%08x)\n", srchFile, newfile);


			printf (" ===== File End ===== \n\n\n");
		}
	}
#endif
}

void dsmcc_cache_event_info(struct cache *filecache, unsigned short mod_id, unsigned int key_len, char *key, struct biop_binding *bind, unsigned int comp_idx)
{
	struct cache_file *newfile, *last;
	struct cache_dir *dir;

	// Check we do not already have file (or file info) cached
	if(dsmcc_cache_file_find(filecache,
			bind->ior.body.full.obj_loc.carousel_id,
			bind->ior.body.full.obj_loc.module_id,
			bind->ior.body.full.obj_loc.objkey_len,
			bind->ior.body.full.obj_loc.objkey) != NULL) {
		return;
	}

	// See if the data had already arrived for the file

	newfile = dsmcc_cache_file_find_data(filecache,
				bind->ior.body.full.obj_loc.carousel_id,
				bind->ior.body.full.obj_loc.module_id,
				bind->ior.body.full.obj_loc.objkey_len,
				bind->ior.body.full.obj_loc.objkey);

	if(newfile == NULL)
	{
		// Create the file from scratch
		newfile = (struct cache_file*)malloc(sizeof(struct cache_file));
		memset (newfile, 0, sizeof(struct cache_file));

		newfile->eFileType = DSMCC_OBJTYPE_EVENT;

		newfile->carousel_id = bind->ior.body.full.obj_loc.carousel_id;
		newfile->module_id = bind->ior.body.full.obj_loc.module_id;
		newfile->key_len = bind->ior.body.full.obj_loc.objkey_len;
		newfile->key= (char *)malloc(newfile->key_len);
		memcpy(newfile->key, bind->ior.body.full.obj_loc.objkey,
							newfile->key_len);
		newfile->data = NULL;
	}
	else
	{
	}

	newfile->filename = (char*)malloc(bind->name.comps[comp_idx].id_len);
	memcpy (newfile->filename, bind->name.comps[comp_idx].id, bind->name.comps[comp_idx].id_len);
	newfile->next = NULL;

	dir = dsmcc_cache_dir_find(filecache, newfile->carousel_id, mod_id, key_len, key);

	filecache->num_files++;
	filecache->total_files++;

	if(dir == NULL)
	{
		/* Parent directory not yet known */
		newfile->p_module_id = mod_id;
		newfile->p_key_len = key_len;
		newfile->p_key = (char *)malloc(newfile->p_key_len);
		memcpy(newfile->p_key, key, key_len);
		newfile->parent = NULL;
		dsmcc_cache_unknown_file_info(filecache, newfile);
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
			dsmcc_cache_write_file(filecache, newfile);
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
			printf (" File Cache : (0x%08x)\n", filecache);

			srchFile = dsmcc_cache_file_find (filecache, newfile->carousel_id, newfile->module_id, newfile->key_len, newfile->key);
			printf (" File found from Cache : (0x%08x) : (0x%08x)\n", srchFile, newfile);


			printf (" ===== File End ===== \n\n\n");
		}
	}
#endif
}

