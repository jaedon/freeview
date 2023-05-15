/* $Header: $ */

#ifndef DLIB_FILEINFO_H__
#define DLIB_FILEINFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define	DLIB_FILEINFO_NAME_LEN			768
//#define	DLIB_FILEINFO_TYPE_LEN			32
#define	DLIB_FILEINFO_TYPE_MAX			24
#define	DLIB_FILEINFO_ONEDEPTH_LEN		128
#define	DLIB_FILEINFO_REPNAME_LEN		512

typedef enum
{
	eDxFILETYPE_UNKNOWN = 0,
	eDxFILETYPE_MUSIC,
	eDxFILETYPE_EXIF,
	eDxFILETYPE_PNG,
	eDxFILETYPE_RECORDINGS,
	eDxFILETYPE_AVSTREAM,
	eDxFILETYPE_DIRECTORY,
	eDxFILETYPE_END
} DxFileType_e;

typedef enum
{
	eDxISREG	=0,
	eDxISDIR,
	eDxISCHR,
	eDxISBLK,
	eDxISFIFO,
	eDxISLNK,
	eDxISSOCK,
	eDxUNKNOWN ,
}DxFileMode_e;


typedef	enum
{
	eDxINTERFACE_SATA =0,
	eDxINTERFACE_USB,
	eDxINTERFACE_SD,
	eDxINTERFACE_SAMBA,
	eDxINTERFACE_UNKNOWN
} DxStorageInterface_e;


typedef struct
{
	DxFileMode_e 	st_mode;
	HUINT64			st_size;  /* total size, in bytes */

	HUINT64			ullATime;
	HUINT64			ullMTime;
	HUINT64			ullCTime;
}DxFileState_t;


typedef	struct
{
	HCHAR	szRepFile[DLIB_FILEINFO_REPNAME_LEN]; /* not path, file name */
	HBOOL	bHasSubFile;
} DxDirInfo_t;

typedef	struct
{
	HUINT64					uid;
	DxStorageInterface_e	eInterface;
	DxFileState_t			st_stat;

	DxFileType_e			eDataType;
//	HCHAR					szExt[MAX_HXDIR_EXTNAME_LENGTH];			//	get extension from szFileName directly
	HCHAR					szOnedepth[DLIB_FILEINFO_ONEDEPTH_LEN];
	HCHAR					szFileName[DLIB_FILEINFO_NAME_LEN];
//	HCHAR					szDataType[DLIB_FILEINFO_TYPE_LEN];			//	move eDataType

	HINT32					size;
	HUINT8					data[];
} DxFileInfo_t;

extern DxFileState_t*	DILB_FILEINFO_Stat_StructConvert(const HCHAR* pszPath);
extern HERROR			DLIB_FILEINFO_ConvertStatToDxStat(const HxDIR_Stat_t *fstat, DxFileState_t *pstDxStat);

#ifdef __cplusplus
}
#endif

#endif // DLIB_FILEINFO_H__

