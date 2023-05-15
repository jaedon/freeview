#include "bsdiff.h"

#include <bzlib.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>

#if 0
# define USE_BZ2
#endif

static int64_t offtin(uint8_t *buf)
{
    int64_t y=buf[7]&0x7F;
    y=y*256;y+=buf[6];
    y=y*256;y+=buf[5];
    y=y*256;y+=buf[4];
    y=y*256;y+=buf[3];
    y=y*256;y+=buf[2];
    y=y*256;y+=buf[1];
    y=y*256;y+=buf[0];
    if(buf[7]&0x80) y=-y;
    return y;
}

static void offtout(int64_t x,uint8_t *buf)
{
    int64_t y = x;
    buf[0]=y%256;y-=buf[0];
    y=y/256;buf[1]=y%256;y-=buf[1];
    y=y/256;buf[2]=y%256;y-=buf[2];
    y=y/256;buf[3]=y%256;y-=buf[3];
    y=y/256;buf[4]=y%256;y-=buf[4];
    y=y/256;buf[5]=y%256;y-=buf[5];
    y=y/256;buf[6]=y%256;y-=buf[6];
    y=y/256;buf[7]=y%256;
    if(x<0) buf[7]|=0x80;
}

static int bz2_read(const struct bspatch_stream* stream, void* buffer, int length)
{
    int n;
    int bz2err;
    BZFILE* bz2;

    bz2 = (BZFILE*)stream->opaque;
    n = BZ2_bzRead(&bz2err, bz2, buffer, length);
    if (n != length)
        return -1;

    return 0;
}

int patch(const char *file1, const char *file2, const char *output)
{
    FILE * f;
    int fd, i;
    int bz2err;
    uint8_t header[24];
    uint8_t *old, *new;
    int64_t oldsize, newsize;
    BZFILE* bz2;
    struct bspatch_stream stream;

    printf("PATCH: %s->%s [%s]\n", file1, file2, output);

    /* Open patch file */
    if ((f = fopen(file2, "r")) == NULL)
        err(1, "fopen(%s)", file2);

    /* Read header */
    if (fread(header, 1, 24, f) != 24) {
        if (feof(f))
            errx(1, "Corrupt patch\n");
        err(1, "fread(%s)", file2);
    }

    /* Check for appropriate magic */
    if (memcmp(header, "ENDSLEY/BSDIFF43", 16) != 0)
        errx(1, "Corrupt patch\n");

    /* Read lengths from header */
    newsize=offtin(header+16);
    if(newsize<0)
        errx(1,"Corrupt patch\n");

    /* Close patch file and re-open it via libbzip2 at the right places */
    if(((fd=open(file1,O_RDONLY,0))<0) ||
       ((oldsize=lseek(fd,0,SEEK_END))==-1) ||
       ((old=malloc(oldsize+1))==NULL) ||
       (lseek(fd,0,SEEK_SET)!=0) ||
       (read(fd,old,oldsize)!=oldsize) ||
       (close(fd)==-1)) err(1,"%s",file1);
    if((new=malloc(newsize+1))==NULL) err(1,NULL);

    if (NULL == (bz2 = BZ2_bzReadOpen(&bz2err, f, 0, 0, NULL, 0)))
        errx(1, "BZ2_bzReadOpen, bz2err=%d", bz2err);

    stream.read = bz2_read;
    stream.opaque = bz2;
    if (bspatch(old, oldsize, new, newsize, &stream))
        errx(1, "bspatch");

    /* Clean up the bzip2 reads */
    BZ2_bzReadClose(&bz2err, bz2);
    fclose(f);

    /* Write the new file */
    if(((fd=open(output,O_CREAT|O_TRUNC|O_WRONLY,0666))<0) ||
       (write(fd,new,newsize)!=newsize) || (close(fd)==-1))
        err(1,"%s",output);

    free(new);
    free(old);

    return 0;
}

static int bz2_write(struct bsdiff_stream* stream, const void* buffer, int size)
{
    int bz2err;
    BZFILE* bz2;

    bz2 = (BZFILE*)stream->opaque;
    BZ2_bzWrite(&bz2err, bz2, (void*)buffer, size);
    if (bz2err != BZ_STREAM_END && bz2err != BZ_OK)
        return -1;

    return 0;
}

int diff(const char *file1, const char *file2, const char *output)
{
    int fd, i;
    int bz2err;
    uint8_t *old,*new;
    int64_t oldsize,newsize;
    uint8_t buf[8];
    FILE * pf;
    struct bsdiff_stream stream;
    BZFILE* bz2;

    printf("DIFF: %s->%s [%s]\n", file1, file2, output);

    memset(&bz2, 0, sizeof(bz2));
    stream.malloc = malloc;
    stream.free = free;
    stream.write = bz2_write;

    /* Allocate oldsize+1 bytes instead of oldsize bytes to ensure
       that we never try to malloc(0) and get a NULL pointer */
    if(((fd=open(file1,O_RDONLY,0))<0) ||
       ((oldsize=lseek(fd,0,SEEK_END))==-1) ||
       ((old=malloc(oldsize+1))==NULL) ||
       (lseek(fd,0,SEEK_SET)!=0) ||
       (read(fd,old,oldsize)!=oldsize) ||
       (close(fd)==-1)) err(1,"%s",file1);


    /* Allocate newsize+1 bytes instead of newsize bytes to ensure
       that we never try to malloc(0) and get a NULL pointer */
    if(((fd=open(file2,O_RDONLY,0))<0) ||
       ((newsize=lseek(fd,0,SEEK_END))==-1) ||
       ((new=malloc(newsize+1))==NULL) ||
       (lseek(fd,0,SEEK_SET)!=0) ||
       (read(fd,new,newsize)!=newsize) ||
       (close(fd)==-1)) err(1,"%s",file2);

    /* Create the patch file */
    if ((pf = fopen(output, "w")) == NULL)
        err(1, "%s", output);

    /* Write header (signature+newsize)*/
    offtout(newsize, buf);
    if (fwrite("ENDSLEY/BSDIFF43", 16, 1, pf) != 1 ||
        fwrite(buf, sizeof(buf), 1, pf) != 1)
        err(1, "Failed to write header");


    if (NULL == (bz2 = BZ2_bzWriteOpen(&bz2err, pf, 9, 0, 0)))
        errx(1, "BZ2_bzWriteOpen, bz2err=%d", bz2err);

    stream.opaque = bz2;
    if (bsdiff(old, oldsize, new, newsize, &stream))
        err(1, "bsdiff");

    BZ2_bzWriteClose(&bz2err, bz2, 0, NULL, NULL);
    if (bz2err != BZ_OK)
        err(1, "BZ2_bzWriteClose, bz2err=%d", bz2err);

    if (fclose(pf))
        err(1, "fclose");

    /* Free the memory we used */
    free(old);
    free(new);

    return 0;
}

int main(int argc,char *argv[])
{
    int i;
    int doPatch = !strcmp(argv[0], "bspatch");
    const char *file1=0, *file2=0, *output=0;

    for(i = 1; i < argc; ++i) {
        if(argv[i][0] == '-') {
            if(!strcmp(argv[i], "-p"))
                doPatch = 1;
            else if(!strcmp(argv[i], "-d"))
                doPatch = 0;
            else
                printf("Unprocessed: %s\n", argv[i]);
        } else if(!file1) {
            file1 = argv[i];
        } else if(!file2) {
            file2 = argv[i];
        } else if(!output) {
            output = argv[i];
        }
    }
    if(!file1 || !file2 || !output)
        errx(1,"usage: %s file1 file2 output\n", argv[0]);
    if(doPatch)
        return patch(file1, file2, output);
    return diff(file1, file2, output);
}
