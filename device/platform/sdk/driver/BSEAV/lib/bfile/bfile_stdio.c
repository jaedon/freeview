/***************************************************************************
 *     Copyright (c) 2006-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bfile_stdio.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 9/12/08 6:56p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bfile/bfile_stdio.c $
 * 
 * 6   9/12/08 6:56p vsilyaev
 * PR 31887: Log reads and seeks that are used by bmedia framework
 * 
 * 5   9/11/08 10:35a vsilyaev
 * PR 45201: PR 10479_DVD 10627_DVD: Replaced all fseek and ftell with
 * fseeko/ftello
 * 
 * 4   8/6/08 7:19p vsilyaev
 * PR 45201: Use 64 bit ready functions to support DIVX HD (where files
 * could be larger than 4GB)
 * 
 * 3   3/5/08 10:44a erickson
 * PR40307: b_stdio_seek must return current position
 * 
 * 2   8/31/06 9:46a vsilyaev
 * PR 23826: Fixed warnings
 * 
 * 1   4/7/06 12:31p vsilyaev
 * PR 20680: Added bfile library
 * 
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bfile_stdio.h"
#include "bkni.h"

BDBG_MODULE(bfile_stdio);

BDBG_OBJECT_ID(bfile_io_read_stdio);
struct bfile_io_read_stdio {
	struct bfile_io_read ops; /* shall be the first member */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
	VFIO_Handle_t tHandle;
#else
	FILE *fin;
#endif
	BDBG_OBJECT(bfile_io_read_stdio)
};


static ssize_t 
b_stdio_read(bfile_io_read_t fd, void *buf, size_t length)
{
	struct bfile_io_read_stdio *f=(void *)fd;
	BDBG_OBJECT_ASSERT(f, bfile_io_read_stdio);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO) 
{
	ssize_t	readNum=0;
	int		rc;

	BDBG_MSG(("[++b_stdio_read] length=%d\n", length));
	
	rc = VFIO_Read (f->tHandle, buf, length, 1, &readNum);
	if (rc)
	{
		/* error */
		BDBG_ERR(("[--b_stdio_read] Error -  VFIO_Read()=%d\n", rc));
		return 0;
	}
	else
	{
		BDBG_MSG(("[--b_stdio_read]\n"));
		return readNum;
	}
}
#else
    BDBG_MSG(("b_stdio_read: %#lx %lu:%u", (unsigned long)fd, (unsigned long)ftello(f->fin), (unsigned)length));
	return fread(buf, 1, length, f->fin);
#endif
}

static off_t 
b_stdio_seek(bfile_io_read_t fd, off_t offset, int whence)
{
	struct bfile_io_read_stdio *f=(void *)fd;
	int rc;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
	off_t curOffset=0;
#endif
	BDBG_OBJECT_ASSERT(f, bfile_io_read_stdio);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
	BDBG_MSG(("[++b_stdio_seek] offset=%d, whence=%d\n", offset, whence));

	rc = VFIO_Seek (f->tHandle, offset, whence);

	BDBG_MSG(("[--b_stdio_seek] rc=%d\n", rc));
	if (rc!=0) { return -1;}
	rc = VFIO_Tell (f->tHandle, &curOffset);
	if (rc!=0) { return -1;}
	return curOffset;
#else
    BDBG_MSG(("b_stdio_seek: %#lx %lu:%d", (unsigned long)fd, (unsigned long)offset, whence));
	rc = fseeko(f->fin, offset, whence);
	if (rc!=0) { return -1;}
	return ftello(f->fin);
#endif
}

static int 
b_stdio_bounds(bfile_io_read_t fd, off_t *first, off_t *last)
{
	struct bfile_io_read_stdio *f=(void *)fd;
	off_t cur;
	int rc;
	BDBG_OBJECT_ASSERT(f, bfile_io_read_stdio);

    *first = *last = 0;

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
	BDBG_MSG(("[++b_stdio_bounds]\n"));

	/* TODO :: comment by junsy
         * ?Ì ÇÔ¼ö?Ç ±â´É?Ì ¹«¾ù?ÎÁö ÆÄ¾ÇÇØ¼­ ´ë??ÇØ¾ß ÇÒ µí...
	 * file size¸¦ ±¸ÇÏ·Á´Â °Í?Ì¶ó¸é p2p engine¿¡¼­´Â ¾î¶»°Ô Ã³¸®ÇØ¾ß ÇÒÁö È®?Î ÇÊ¿ä.
	 */
	rc = VFIO_Tell(f->tHandle, &cur);
	if (rc<0)
	{
		BDBG_ERR(("[b_stdio_bounds] Error -  VFIO_Tell()=%d - pos1\n", rc));
		return -1;
	}

	rc = VFIO_Seek (f->tHandle, 0, SEEK_END);
	if (rc<0)
	{
		BDBG_ERR(("[b_stdio_bounds] Error -  VFIO_Seek()=%d - pos1\n", rc));
		return rc;
	}

	*first = 0;
	rc = VFIO_Tell(f->tHandle, last);
	if (rc<0)
	{
		BDBG_ERR(("[b_stdio_bounds] Error -  VFIO_Tell()=%d - pos2\n", rc));
		return -1;
	}

	rc = VFIO_Seek (f->tHandle, cur, SEEK_SET);
	if (rc<0)
	{
		BDBG_ERR(("[b_stdio_bounds] Error -  VFIO_Seek()=%d - pos2\n", rc));
		return rc;
	}

	BDBG_MSG(("[--b_stdio_bounds] *first=%d, *last=%d\n", *first, *last));
	return 0;
#else
	cur = ftello(f->fin);
	if (cur<0) { return -1; }

	rc = fseeko(f->fin, 0, SEEK_END);
	if (rc<0) { return rc; }

	*first = 0;
	*last = ftello(f->fin);
	if (*last<0) { return -1; }
	rc = fseeko(f->fin, cur, SEEK_SET);
	if (rc<0) {return rc;}
	return 0;
#endif
}

static const struct bfile_io_read b_stdio_read_ops = {
	b_stdio_read,
	b_stdio_seek,
	b_stdio_bounds,
	BIO_DEFAULT_PRIORITY
};

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
bfile_io_read_t 
bfile_stdio_read_attach(VFIO_Handle_t tHandle)
{
	struct bfile_io_read_stdio *f;
	f=BKNI_Malloc(sizeof(*f));
	if (!f) { return NULL; }
	BDBG_OBJECT_INIT(f, bfile_io_read_stdio);
	f->ops = b_stdio_read_ops;
	f->tHandle = tHandle;
	return &f->ops;
}
#else
bfile_io_read_t 
bfile_stdio_read_attach(FILE *fin)
{
	struct bfile_io_read_stdio *f;
	f=BKNI_Malloc(sizeof(*f));
	if (!f) { return NULL; }
	BDBG_OBJECT_INIT(f, bfile_io_read_stdio);
	f->ops = b_stdio_read_ops;
	f->fin = fin;
	return &f->ops;
}
#endif

void 
bfile_stdio_read_detach(bfile_io_read_t file)
{
	struct bfile_io_read_stdio *f=(void *)file;
	BDBG_OBJECT_ASSERT(f, bfile_io_read_stdio);

	BDBG_OBJECT_DESTROY(f, bfile_io_read_stdio);
	BKNI_Free(f);

	return;
}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_VFIO)
/* TODO :: comment by junsy
 *  omit below functions.
 */
#else

BDBG_OBJECT_ID(bfile_io_write_stdio);
struct bfile_io_write_stdio {
	struct bfile_io_write ops;
	FILE *fout;
	BDBG_OBJECT(bfile_io_write_stdio)
};

static ssize_t 
b_stdio_write(bfile_io_write_t fd, const void *buf, size_t length)
{
	struct bfile_io_write_stdio *f=(void *)fd;
	BDBG_OBJECT_ASSERT(f, bfile_io_write_stdio );

	return fwrite(buf, 1, length, f->fout);
}

static off_t 
b_stdio_trim(bfile_io_write_t fd, off_t trim_pos)
{
	BSTD_UNUSED(fd);
	BSTD_UNUSED(trim_pos);
	BDBG_WRN(("trim is not supported"));
	return 0;
}

static const struct bfile_io_write b_stdio_write_ops = {
	b_stdio_write,
	b_stdio_trim,
	BIO_DEFAULT_PRIORITY
};


bfile_io_write_t 
bfile_stdio_write_attach(FILE *fout)
{
	struct bfile_io_write_stdio *f;
	f=BKNI_Malloc(sizeof(*f));
	if (!f) { return NULL; }
	BDBG_OBJECT_INIT(f, bfile_io_write_stdio );
	f->ops = b_stdio_write_ops;
	f->fout = fout;
	return &f->ops;
}

void 
bfile_stdio_write_detach(bfile_io_read_t fd)
{
	struct bfile_io_write_stdio *f=(void *)fd;
	BDBG_OBJECT_ASSERT(f, bfile_io_write_stdio );

	BDBG_OBJECT_DESTROY(f, bfile_io_write_stdio);
	BKNI_Free(f);

	return;
}

#endif

