/*************************************************************
 * @file		hlib.h
 * @date		2011/01/17
 * @author	hwang ji yeon(jyhwang@humaxdigital.com)
 * @brief		HUMAX common library header.
 *
 * http://www.humaxdigital.com
 *************************************************************/
#ifndef _HLIB_H_
#define _HLIB_H_

//#define	OBAMA_IS_DFBMASTER

#ifdef __cplusplus
extern "C" {
#endif

/* types */
#include <_hlib_types.h>
#include <_hlib_ansicode.h>

/* include hlib files */
#include <_hlib_mem.h>
#include <_hlib_cmd.h>
#include <_hlib_language.h>
#include <_hlib_codepage.h>
#include <_hlib_cbuf.h>
#include <_hlib_file.h>
#include <_hlib_dir.h>
#include <_hlib_path.h>
#include <_hlib_log.h>
#include <_hlib_vector.h>
#include <_hlib_stack.h>
#include <_hlib_queue.h>
#include <_hlib_priorityqueue.h>
#include <_hlib_list.h>
#include <_hlib_tree.h>
#include <_hlib_hash.h>
#include <_hlib_math.h>
#include <_hlib_sem.h>
#include <_hlib_json.h>
#include <_hlib_jsonbin.h>
#include <_hlib_jsonwriter.h>
#include <_hlib_datetime.h>
#include <_hlib_stdlib.h>
#include <_hlib_rwstream.h>
#include <_hlib_country.h>
#include <_hlib_socket.h>
#include <_hlib_udb.h>
#include <_hlib_transport.h>
#include <_hlib_macro.h>
#include <_hlib_crypt.h>
#include <_hlib_encrypt.h>
#include <_hlib_aes.h>
#include <_hlib_xml.h>
#include <_hlib_zlib.h>
#include <_hlib_string.h>
#include <_hlib_msg.h>
#include <_hlib_versionfile.h>
#include <_hlib_png.h>
#include <_hlib_id3.h>
#include <_hlib_exif.h>
#include <_hlib_jpeg.h>
#include <_hlib_av.h>
#include <_hlib_dbg.h>
#include <_hlib_serializer.h>
#include <_hlib_rpc.h>
#include <_hlib_objdec.h>
#include <_hlib_storage.h>
#include <_hlib_ringbuf.h>
#include <_hlib_extra.h>
#include <_hlib_token.h>
#include <_hlib_convert.h>

#if	0		//	not used
#include <_hlib_mempool.h>
#include <_hlib_ringqueue.h>
#include <_hlib_smem.h>
#include <_hlib_collection.h>
#include <_hlib_shm.h>
#include <_hlib_sqlite.h>
#endif

#define	CONFIG_SILIB
#ifdef	CONFIG_SILIB
//#include "../silib/include/silib.h"
#endif

#if defined(WIN32)
#define		Hx_Print					DI_UART_Print
#else
#define		Hx_Print					printf
#endif
#define		Hx_GetCh					HLIB_STD_GetChar
#define		Hx_NULL_PRINT				while (0) ((int (*)(char *, ...)) 0)



/**
 * 64비트 타입에 대해 bit 연산을 하는 경우, 32비트 넘어가는 구간에 대해
 * 0ULL과 같이 타입 명시해주지 않으면 32비트 내에서만 빙글빙글 돌아요~
 */
typedef HINT64	HUID;
#define	HUID_FMT	"%lld"

HUID	HUID_String8(const HCHAR *str);
HCHAR *	HUID_ToString(HCHAR *str, HUID uid);


#define HANDLE_NULL		0xFFFFFFFF


#define	SIZE_1K		0x00000400
#define	SIZE_2K		0x00000800
#define	SIZE_4K		0x00001000
#define	SIZE_8K		0x00002000
#define	SIZE_16K 	0x00004000
#define	SIZE_32K 	0x00008000
#define	SIZE_64K	0x00010000
#define	SIZE_128K	0x00020000
#define	SIZE_256K	0x00040000
#define	SIZE_300K	0x0004B000
#define	SIZE_400K	0x00064000
#define	SIZE_512K	0x00080000

#define DBG_NONE		0x0000
#define	DBG_TRACE		0x0001
#define	DBG_PRINT		0x0002
#define	DBG_DATA		0x0004
#define DBG_FUNC		0x0008
#define	DBG_ASSERT		0x0010
#define	DBG_ALL			0xFFFF

#define	HxNAMEOF(type)	#type

#define NOT_USED_PARAM(x)				(void)(x)
#define ENUM_TO_STR(enumVal) 			case enumVal: return #enumVal;
#define	BOOL_TO_STR(b)					((b == TRUE) ? "TRUE" : "FALSE")


#ifdef __cplusplus
}
#endif

#endif /* _HLIB_H_ */

