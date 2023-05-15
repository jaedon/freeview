/***************************************************************
**
** Broadcom Corp. Confidential
** Copyright 1998-2002 Broadcom Corp. All Rights Reserved.
**
** THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED 
** SOFTWARE LICENSE AGREEMENT BETWEEN THE USER AND BROADCOM. 
** YOU HAVE NO RIGHT TO USE OR EXPLOIT THIS MATERIAL EXCEPT 
** SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
**
** File:		venom2_fb.c
** Description: API to access venom2 frame buffer
**
** Created: 12/18/2001 Vladimir N. Silyaev
**
**
****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "venom2_fb.h"

#define MPEG_MB_HEIGHT  16
#define MPEG_MB_WIDTH  16

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

struct venom2_fb
{
    unsigned fb_width;
    unsigned fb_height;
    void    *base_addr;
    enum v2fbColorFormat format;
    unsigned fb_mwidth, fb_mheight;
    unsigned fb_chroma_offset;
};

typedef struct  {
    void *Y;
    void *Cb;
    void *Cr;
} frame_buffer_YUV;

typedef struct  {
    uint8 R,G,B;
} frame_buffer_RGB;


struct venom2_fb_zone {
    venom2_fb fb;
    unsigned width;
    unsigned height;
    enum v2fbColorFormat format;

    frame_buffer_YUV *yuv422;
    frame_buffer_YUV *yuv444;
    frame_buffer_RGB *rgb;
    unsigned mbwidth;
    unsigned mbheight;

    unsigned cur_line;
};

struct frame_buffer_conv  {
    frame_buffer_YUV *prev, *next;  /* Pointers to the prev and next macro line */
};

int
Venom2FBCreate(venom2_fb *pfb, unsigned long frameb_addr)
{
    struct venom2_fb *fb;

    *pfb = NULL;

    fb = malloc(sizeof(*fb));
    if (!fb) {
        return -1;
    }
    fb->fb_width = 1920;
    fb->fb_height = 1088;

    fb->format = v2fb422;

    *pfb = fb;

    fb->fb_mwidth = (fb->fb_width+MPEG_MB_WIDTH-1)/MPEG_MB_WIDTH;
    fb->fb_mheight = (fb->fb_height+MPEG_MB_HEIGHT-1)/MPEG_MB_HEIGHT;

    fb->base_addr =  (void *)frameb_addr;

    fb->fb_chroma_offset = ((fb->fb_height + MPEG_MB_HEIGHT-1)/MPEG_MB_HEIGHT)*MPEG_MB_HEIGHT * (fb->fb_mwidth * MPEG_MB_WIDTH);

    return 0;
}

int
Venom2FBDestroy(venom2_fb fb)
{
    free(fb);
    return 0;
}

uint32
v2FB_Read(venom2_fb fb, unsigned offset)
{
    uint32 tmp;

    tmp = ((uint32 *)fb->base_addr)[offset];
    tmp = (tmp<<24) | ((tmp<<8) & 0xFF0000) | ((tmp>>8)  & 0xFF00) | (tmp>>24);
    return tmp;
}

/* Read macroblock line Y with width from the frame buffer */

int
Venom2FBReadLine(venom2_fb fb, unsigned y, unsigned mbwidth, const frame_buffer_YUV *line)
{
    uint32 tmp;
    unsigned fy,fx;
    unsigned offset_fb, offset;


    offset_fb = (y*fb->fb_mwidth*MPEG_MB_HEIGHT*MPEG_MB_WIDTH)/sizeof(uint32);
    offset = 0;
    for (fx=0; fx<mbwidth; fx++) {
        offset = 4*fx;
        for (fy=0; fy<MPEG_MB_HEIGHT; fy++) {
            tmp = v2FB_Read(fb, offset_fb);
            offset_fb++;
            ((uint32 *)line->Y)[offset] = tmp;
            tmp = v2FB_Read(fb, offset_fb);
            offset_fb++;
            ((uint32 *)line->Y)[offset+1] = tmp;

            tmp = v2FB_Read(fb, offset_fb);
            offset_fb++;
            ((uint32 *)line->Y)[offset+2] = tmp;
            tmp = v2FB_Read(fb, offset_fb);
            offset_fb++;
            ((uint32 *)line->Y)[offset+3] = tmp;

            offset += mbwidth*MPEG_MB_WIDTH/sizeof(uint32);
        }
        offset +=2;
    }

    offset_fb = (fb->fb_chroma_offset/sizeof(uint32)) + (y*fb->fb_mwidth*MPEG_MB_HEIGHT*MPEG_MB_WIDTH)/sizeof(uint32);
    offset = 0;

    for (fx=0; fx<mbwidth; fx++) {
        offset = fx*2;
        for (fy=0; fy<MPEG_MB_HEIGHT; fy++) {
            tmp = v2FB_Read(fb, offset_fb);
            offset_fb++;
            ((uint32 *)line->Cb)[offset] = tmp;
            tmp = v2FB_Read(fb, offset_fb);
            offset_fb++;
            ((uint32 *)line->Cr)[offset] = tmp;

            tmp = v2FB_Read(fb, offset_fb);
            offset_fb++;
            ((uint32 *)line->Cb)[offset+1] = tmp;
            tmp = v2FB_Read(fb, offset_fb);
            offset_fb++;
            ((uint32 *)line->Cr)[offset+1] = tmp;

            offset += mbwidth*MPEG_MB_WIDTH/(sizeof(uint32)*2);
        }
    }

    return 0;
}

int
v2fb_alloc_YUV_line(frame_buffer_YUV **pline, enum v2fbColorFormat format, unsigned mbwidth)
{
    frame_buffer_YUV *line;
    unsigned size;

    *pline = NULL;

    line = malloc(sizeof(*line));
    if (!line) {
         goto err_line;
         return -1;
    }
    size = mbwidth*MPEG_MB_WIDTH*MPEG_MB_HEIGHT;
    line->Y = malloc(size);
    if (!line->Y) {
        goto err_Y;
    }
    switch(format) {
    case v2fb420:
        size /= 2*2; /* Chroma size reduced by 2 both in vertical and horizontal direction */
        break;
    case v2fb422:
        size /= 2; /* Chroma size reduced by 2 in vertical direction */
        break;
    case v2fb444: /* Do nothing */
        break;
    }
    line->Cb = malloc(size);
    if (!line->Cb) {
        goto err_Cb;
    }

    line->Cr = malloc(size);
    if (!line->Cr) {
        goto err_Cr;
    }

    *pline = line;
    return 0;

err_Cr:
    free(line->Cb);
err_Cb:
    free(line->Y);
err_Y:
    free(line);
err_line:
    return -1;
}

int
v2fb_free_YUV_line(frame_buffer_YUV *line)
{
   free(line->Y);
   free(line->Cb);
   free(line->Cr);
   free(line);

   return 0;
}

uint8 v2fb_Clip(int sample)
{
    if (sample<0) {
        return 0;
    } else if (sample>255) {
        return 255;
    } else {
        return (uint8) sample;
    }
}

//const int filter_422_444={21, -52, 159, 159, -52, 21};

/* Convert macro line from 4:2:2 YCrCb to 4:4:4 YCrCb */
int
v2fb_ConvertChromaLine422_444(const void *Csrc, void *Cdest,unsigned  mbwidth)
{
    unsigned y, x;
        unsigned i2, im2, im1, ip1, ip2, ip3, w;
    unsigned offset;

    w = mbwidth*MPEG_MB_WIDTH/2;
    for (x=0; x<w; x++) {
        i2 = x<<1;
        im2 = (x<2) ? 0 : x-2;
        im1 = (x<1) ? 0 : x-1;
        ip1 = (x<w-1) ? x+1 : w-1;
        ip2 = (x<w-2) ? x+2 : w-1;
        ip3 = (x<w-3) ? x+3 : w-1;


        offset = 0;
        for (y=0;y<MPEG_MB_HEIGHT;y++) {


            /* FIR filter coefficients (*256): 21 0 -52 0 159 256 159 0 -52 0 21 */
            /* even samples (0 0 256 0 0) */
            ((uint8 *)Cdest)[i2+offset*2] = ((uint8 *)Csrc)[x];

            /* odd samples (21 -52 159 159 -52 21) */
            ((uint8 *)Cdest)[i2+offset*2 +1 ] = v2fb_Clip((int)(21*(((uint8 *)Csrc)[im2+offset]+((uint8 *)Csrc)[ip3+offset])
                        -52*(((uint8 *)Csrc)[im1+offset]+((uint8 *)Csrc)[ip2+offset])
                       +159*(((uint8 *)Csrc)[x+offset]+((uint8 *)Csrc)[ip1+offset])+128)>>8);

            offset += w;
        }
    }

    return 0;
}

int
v2fb_alloc_RGB_line(frame_buffer_RGB **pline, unsigned  mbwidth)
{
 frame_buffer_RGB *line;

 *pline = NULL;
 line = malloc(sizeof(*line) * mbwidth*MPEG_MB_WIDTH*MPEG_MB_HEIGHT);
 if (!line) {
     return -1;
 }

 *pline = line;
 return 0;
}

int
v2fb_free_RGB_line(frame_buffer_RGB *line)
{
  free(line);
  return 0;
}

const int  Table_YUV2RGB[] = {104597, 132201, 25675, 53279};

int
v2fb_Conv_444YUV_RGB(frame_buffer_YUV *src, frame_buffer_RGB *dst, unsigned  mbwidth)
{
  unsigned i, j;
  int y, u, v;
  unsigned offset;


  for (i=0; i<MPEG_MB_HEIGHT; i++) {
    offset = i*mbwidth*MPEG_MB_WIDTH;
    for (j=0; j<mbwidth*MPEG_MB_WIDTH; j++)
    {
      u = ((uint8 *)src->Cb)[offset] - 128;
      v = ((uint8 *)src->Cr)[offset] - 128;
      y = 76309 * (((uint8 *)src->Y)[offset] - 16); /* (255/219)*65536 */
      dst[offset].R = v2fb_Clip((y + Table_YUV2RGB[0]*v + 32768)>>16);
      dst[offset].G = v2fb_Clip((y - Table_YUV2RGB[2]*u - Table_YUV2RGB[3]*v + 32768)>>16);
      dst[offset].B = v2fb_Clip((y + Table_YUV2RGB[1]*u + 32786)>>16);
      offset ++;
    }
  }
  return 0;
}



#if 0
const static uint8 tga24[14] = {0,0,2,0,0,0,0, 0,0,0,0,0,24,32};

int
putbyte(FILE *fout, uint8 byte)
{
    int rc;

    rc = fwrite(&byte, 1, 1, fout);
    return 0;
}

int
putword(FILE *fout, uint16 word)
{
    putbyte(fout, (uint8) (word&0xFF));
    putbyte(fout, (uint8) ((word>>8)&0xFF));
    return 0;
}

int
Venom2FBWrite2TGA(venom2_fb fb, FILE *fout, unsigned width, unsigned height)
{
    frame_buffer_YUV *in, in44;
    frame_buffer_RGB *dst;
    unsigned y,x;
    unsigned i;

    width /= MPEG_MB_WIDTH;
    height /= MPEG_MB_HEIGHT;

    v2fb_alloc_YUV_line(&in, fb->format, width);
    v2fb_Alloc_RGB_Line(&dst, width);

    in44.Y=in->Y;
    in44.Cr = malloc(fb->fb_mwidth*MPEG_MB_WIDTH*MPEG_MB_HEIGHT);
    in44.Cb = malloc(fb->fb_mwidth*MPEG_MB_WIDTH*MPEG_MB_HEIGHT);

    for (i=0; i<12; i++) {
            putbyte(fout, tga24[i]);
    }

        putword(fout, (uint16)(width*MPEG_MB_WIDTH)); putword(fout, (uint16)(height*MPEG_MB_HEIGHT));
        putbyte(fout, tga24[12]); putbyte(fout, tga24[13]);

    for(y=0;y<height;y++) {
        Venom2FBReadLine(fb, y, width, in);
        v2fb_ConvertChromaLine422_444(in->Cb, in44.Cb, width);
        v2fb_ConvertChromaLine422_444(in->Cr, in44.Cr, width);
        v2fb_Conv_444YUV_RGB(&in44, dst, width);
        for (i=0;i<MPEG_MB_HEIGHT;i++) {
            for (x=0;x<width*MPEG_MB_WIDTH;x++) {
                  putbyte(fout, dst[i*width*MPEG_MB_WIDTH+x].B);
                  putbyte(fout, dst[i*width*MPEG_MB_WIDTH+x].G);
                  putbyte(fout, dst[i*width*MPEG_MB_WIDTH+x].R);
            }
        }
    }


    return 0;
}

int
main(void)
{
    venom2_fb fb;
    FILE *fin;
    FILE *fout;
    int rc;

    Venom2FBCreate(&fb);

    fin = fopen("frame.dmp", "rb");
    if (!fin) {
        fprintf(stderr, "Can't open input file\n");
        return 1;
    }

    rc = fread(fb->base_addr, 1, fb->fb_width*fb->fb_height*2, fin);

    fclose(fin);

    fout = fopen("pic.tga", "wb");

    Venom2FBWrite2TGA(fb, fout, 1360, 1008);

    fclose(fout);

    return 0;
};
#endif

int
Venom2FBZoneCreate(venom2_fb fb, venom2_fb_zone *pzone, unsigned width, unsigned height, enum v2fbColorFormat targetCf)
{
    venom2_fb_zone zone;
    int rc;

    *pzone = NULL;

    zone  = malloc(sizeof(*zone));

    if (!zone) {
        return -1;
    }
    zone->format = targetCf;
    zone->width = width;
    zone->height = height;
    zone->fb = fb;


    zone->mbwidth = (zone->width + MPEG_MB_WIDTH - 1)/MPEG_MB_WIDTH;
    zone->mbheight = (zone->height + MPEG_MB_HEIGHT -1)/MPEG_MB_HEIGHT;

    zone->rgb = NULL;
    zone->yuv422 = NULL;
    zone->yuv444 = NULL;

    switch(zone->format) {
    case v2fb420:
    default:
        return -1;
    case v2fbRGB:
        rc = v2fb_alloc_RGB_line(&zone->rgb, zone->mbwidth);
        if (rc<0) {
            return -1;
        }
        /* continue */
    case v2fb444:
    case v2fb422:
        rc = v2fb_alloc_YUV_line(&zone->yuv422, v2fb422, width);
        if (rc<0) {
            return -1;
        }
        if (zone->format==v2fb422) {
            break;
        }
        zone->yuv444 = malloc(sizeof(*zone->yuv444));
        if (!zone->yuv444) {
            return -1;
        }
        zone->yuv444->Y=zone->yuv422->Y;
        zone->yuv444->Cr = malloc(zone->mbwidth*MPEG_MB_WIDTH*MPEG_MB_HEIGHT);
        zone->yuv444->Cb = malloc(zone->mbwidth*MPEG_MB_WIDTH*MPEG_MB_HEIGHT);
        break;
    }

    *pzone = zone;
    return 0;
}

int
Venom2FBZoneDestroy(venom2_fb_zone zone)
{

    if (zone->rgb) {
        v2fb_free_RGB_line(zone->rgb);
    }
    if (zone->yuv422) {
        v2fb_free_YUV_line(zone->yuv422);
    }
    if (zone->yuv444) {
        free(zone->yuv444->Cr);
        free(zone->yuv444->Cb);
        free(zone->yuv444);
    }
    return 0;
}

int
Venom2FBZoneCapture(venom2_fb_zone zone, unsigned mbline)
{
    if (mbline>=zone->mbheight) {
        return -1; /* EOF */
    }
    zone->cur_line = 0;

    Venom2FBReadLine(zone->fb, mbline, zone->mbwidth, zone->yuv422);
    v2fb_ConvertChromaLine422_444(zone->yuv422->Cb, zone->yuv444->Cb, zone->mbwidth);
    v2fb_ConvertChromaLine422_444(zone->yuv422->Cr, zone->yuv444->Cr, zone->mbwidth);
    v2fb_Conv_444YUV_RGB(zone->yuv444, zone->rgb, zone->mbwidth);

    return MPEG_MB_HEIGHT;
}

int
Venom2FBZoneTransfer(venom2_fb_zone zone, unsigned char *mbline)
{
    unsigned x;
    unsigned line_offset;

    if (zone->cur_line >= MPEG_MB_HEIGHT) {
        return 1;  /* EOF */
    }
    line_offset = zone->cur_line*zone->mbwidth*MPEG_MB_WIDTH;
    for (x=0;x<zone->mbwidth*MPEG_MB_WIDTH;x++) {
        mbline[0] = zone->rgb[line_offset+x].R;
        mbline[1] = zone->rgb[line_offset+x].G;
        mbline[2] = zone->rgb[line_offset+x].B;

        mbline+=3;
    }

    zone->cur_line++;

    return 0;
}

