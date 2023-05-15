/*
 * utils.h
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>
#include <stdarg.h>

#if !defined(WIN32) // TODO : ���� ���ŵ� �����...
#include <pthread.h> // for pthread_t, pthread_mutex_t, pthread_cond_t, pthread_mutex_init, pthread_mutex_lock, pthread_mutex_unlock, pthread_mutex_destroy, pthread_cond_init, pthread_cond_signal, pthread_cond_destroy
//#include "./libavformat/avformat.h"
//#include "./libswscale/swscale.h"
//#include <alsa/asoundlib.h>
#endif

#include <vkernel.h>
#include "adaptation.h"

#ifndef MIN
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)	((a) > (b) ? (a) : (b))
#endif

#define PIX_FMT_RGBA32	PIX_FMT_RGB32

//enum PixelFormat find_av_pix_fmt(int, unsigned long, unsigned long, unsigned long);

unsigned int char2hex(unsigned char);

int next_utf8(unsigned char *, int, int *);

/*--- for memory debugging ---*/

#define DEBUG_MALLOC
//#define DEBUG_MALLOC_LIST

#ifdef DEBUG_MALLOC
void safe_print_malloc_count(void);
#endif

#ifdef DEBUG_MALLOC_LIST
void safe_print_malloc_list(void);
void *safe_malloc_(size_t nbytes, char *caller, int line);
void *safe_mallocz_(size_t nbytes, char *caller, int line);
void *safe_realloc_(void *oldbuf, size_t nbytes, char *caller, int line);
void *safe_fast_realloc_(void *oldbuf, size_t *oldsize, size_t nbytes, char *caller, int line);
char *safe_strdup_(const char *, char *caller, int line);
#define safe_malloc(nbytes) safe_malloc_(nbytes, (char*)__FUNCTION__, __LINE__)
#define safe_mallocz(nbytes) safe_mallocz_(nbytes, (char*)__FUNCTION__, __LINE__)
#define safe_realloc(oldbuf,nbytes) safe_realloc_(oldbuf, nbytes, (char*)__FUNCTION__, __LINE__)
#define safe_fast_realloc(oldbuf,oldsize,nbytes) safe_fast_realloc_(oldbuf, oldsize, nbytes, (char*)__FUNCTION__, __LINE__)
#define safe_strdup(src) safe_strdup_(src, (char*)__FUNCTION__, __LINE__)
#else
void *safe_malloc(size_t);
void *safe_mallocz(size_t);
void *safe_realloc(void *, size_t);
void *safe_fast_realloc(void *, size_t *, size_t);
char *safe_strdup(const char *);
#endif

void safe_free(void *);

/*--- for error debugging ---*/

void error_at(char *fn, int line, char *message, ...);
#define error(message, ...) error_at((char*)__FUNCTION__, __LINE__, message, ## __VA_ARGS__)
void fatal_at(char *fn, int line, char *message, ...);
#define fatal(message, ...) fatal_at((char*)__FUNCTION__, __LINE__, message, ## __VA_ARGS__)

/*--- for engine debugging ---*/

#define DEBUG_ENGINE

#define ENG_VERBOSE	1	// �뵵 : ������ �ֿ� �÷ο츦 ������� (���� ���� 0, 1 �̰� ����Ʈ 1�� ����)
#define DER_VERBOSE	0	// �뵵 : DER ������ �Ľ��� ������� (���� ���� 0, 1 �̰� ����Ʈ 0���� ����)
#define CLS_VERBOSE	0	// �뵵 : �� Ŭ������ Preparation / Activation / Deactivation / Destruction �÷ο츦 ������� (���� ���� 0, 1 �̰� ����Ʈ 0���� ����)
#define EVT_VERBOSE	1	// �뵵 : �̺�Ʈ ó���� �ֿ� �÷ο츦 ������� (���� ���� 0, 1, 2 �̰� ����Ʈ 1�� ����)

#define MHEG_IB_DBG_MASK_MAIN			0x0001		// �뵵 : ������ �ֿ� �÷ο츦 �������
#define MHEG_IB_DBG_MASK_FILE			0x0002		// �뵵 : ���� ó���� ���� �÷ο츦 ������� (�ֿ� �÷ο�� MHEG_IB_DBG_MASK_MAIN �� �����)
#define MHEG_IB_DBG_MASK_CLS				0x0004		// �뵵 : �� Ŭ������ �Լ� �÷ο� �� ó�� �����͸� �������
#define MHEG_IB_DBG_MASK_EVT				0x0008		// �뵵 : �̺�Ʈ ó���� ���� �÷ο츦 ������� (�ֿ� �÷ο�� MHEG_IB_DBG_MASK_MAIN �� �����)

#define MHEG_IB_DBG_MASK_OSD				0x0010		// �뵵 : OSD ó���� �ֿ� �÷ο츦 �������
#define MHEG_IB_DBG_MASK_AVSTR			0x0020		// �뵵 : AV ��Ʈ�� ��Ʈ���� �ֿ� �÷ο츦 �������

#define MHEG_IB_DBG_MASK_AMMI			0x0100		// �뵵 : CI Application MMI ó���� �ֿ� �÷ο츦 �������
#define MHEG_IB_DBG_MASK_DSM				0x0200		// �뵵 : DSM ó���� �ֿ� �÷ο츦 �������

#define MHEG_IB_DBG_MASK_TODO			0x1000		// �뵵 : �̱��� �Լ�/��ƾ�� ǥ����
#define MHEG_IB_DBG_MASK_TODO_PORTING	0x2000		// �뵵 : �����ؾ� �� �Լ�/��ƾ�� ǥ����
#define MHEG_IB_DBG_MASK_MISC			0x8000		// �뵵 : ��Ÿ

#define MHEG_IB_DBG_MASK_ALL				0xffff

extern int g_MhegIbDbgFlag;

#ifdef CONFIG_DEBUG

void verbose_(int flag, char *message, ...);

#define verbose(message, ...)					verbose_(MHEG_IB_DBG_MASK_MAIN, message, ## __VA_ARGS__)	// �뵵 : ������ �ֿ� �÷ο츦 �������
#define verbose_file(message, ...)			verbose_(MHEG_IB_DBG_MASK_FILE, message, ## __VA_ARGS__)	// �뵵 : ���� ó���� ���� �÷ο츦 ������� (�ֿ� �÷ο�� MHEG_IB_DBG_MASK_MAIN �� �����)
#define verbose_class(message, ...)			verbose_(MHEG_IB_DBG_MASK_CLS, message, ## __VA_ARGS__)	// �뵵 : �� Ŭ������ �Լ� �÷ο� �� ó�� �����͸� �������
#define verbose_event(message, ...)			verbose_(MHEG_IB_DBG_MASK_EVT, message, ## __VA_ARGS__)	// �뵵 : �̺�Ʈ ó���� ���� �÷ο츦 ������� (�ֿ� �÷ο�� MHEG_IB_DBG_MASK_MAIN �� �����)
#define verbose_osd(message, ...)			verbose_(MHEG_IB_DBG_MASK_OSD, message, ## __VA_ARGS__)	// �뵵 : OSD ó���� �ֿ� �÷ο츦 �������
#define verbose_av_stream(message, ...)		verbose_(MHEG_IB_DBG_MASK_AVSTR, message, ## __VA_ARGS__)	// �뵵 : AV ��Ʈ�� ��Ʈ���� �ֿ� �÷ο츦 �������
#define verbose_ammi(message, ...)			verbose_(MHEG_IB_DBG_MASK_AMMI, message, ## __VA_ARGS__)	// �뵵 : CI Application MMI ó���� �ֿ� �÷ο츦 �������
#define verbose_dsm(message, ...)			verbose_(MHEG_IB_DBG_MASK_DSM, message, ## __VA_ARGS__)	// �뵵 : DSM ó���� �ֿ� �÷ο츦 �������
#define verbose_todo(message, ...)			verbose_(MHEG_IB_DBG_MASK_TODO, message, ## __VA_ARGS__)	// �뵵 : �̱��� �Լ�/��ƾ�� ǥ����
#define verbose_todo_porting(message, ...)	verbose_(MHEG_IB_DBG_MASK_TODO_PORTING, message, ## __VA_ARGS__)	// �뵵 : �����ؾ� �� �Լ�/��ƾ�� ǥ����
#define verbose_misc(message, ...)			verbose_(MHEG_IB_DBG_MASK_MISC, message, ## __VA_ARGS__)	// �뵵 : ��Ÿ

#else

#define verbose(message, ...)					((void)0)
#define verbose_file(message, ...)			((void)0)
#define verbose_class(message, ...)			((void)0)
#define verbose_event(message, ...)			((void)0)
#define verbose_osd(message, ...)			((void)0)
#define verbose_av_stream(message, ...)		((void)0)
#define verbose_ammi(message, ...)			((void)0)
#define verbose_dsm(message, ...)			((void)0)
#define verbose_todo(message, ...)			((void)0)
#define verbose_todo_porting(message, ...)	((void)0)
#define verbose_misc(message, ...)			((void)0)

#endif

#endif	/* __UTILS_H__ */

