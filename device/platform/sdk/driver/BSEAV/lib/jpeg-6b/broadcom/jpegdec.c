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
** File:		jpegdec.c
** Description: General helper for IJG libjpeg
**
** Created: 12/18/2001 Vladimir N. Silyaev
**
**
****************************************************************/

#define JPEG_INTERNALS
#include "../jinclude.h"
#include "../jpeglib.h"

#undef INLINE
#include "jpg2mpgi.h"
#include "jpg2mpg.h"
#include "venom2_fb.h"


struct bcm_jpeg_decode {
    jpg2mpg_t *jpg2mpg;
    unsigned res_x, res_y;
    venom2_fb v2fb;
    venom2_fb_zone v2zone;
    unsigned mbline, line_left;
    unsigned long frame_addr;
};

static void convert_dht(jpg2mpg_t *mp, j_decompress_ptr cinfo);
static void convert_dqt(jpg2mpg_t *mp, j_decompress_ptr cinfo);
static void convert_sof(jpg2mpg_t *mp, j_decompress_ptr cinfo);
static int inherit_mpegcntx(j_decompress_ptr cinfo, struct bcm_jpeg_decode *jpgdec);
static int brcmFBTransferDone(struct bcm_jpeg_decode *jpgdec);


FILE *fout;
/* Write data from the bit buffer to to media (file) */
static int
file_write(void *cntx, const void *buf, unsigned nbytes)
{

    fwrite(buf, 1, nbytes, fout); /* pass control directly to the stream routine */

    return 0;
}

static int
read_from_jpeglib(j_decompress_ptr cinfo, void *buf, unsigned nbytes)
{
    unsigned ncopy;
    int nread=0;

    while(nbytes>0) {
      if (cinfo->src->bytes_in_buffer) {
        ncopy = nbytes>cinfo->src->bytes_in_buffer ? cinfo->src->bytes_in_buffer : nbytes;
        memcpy((unsigned char *)buf+nread, cinfo->src->next_input_byte, ncopy);
        nread += ncopy;
        cinfo->src->next_input_byte += ncopy;
        cinfo->src->bytes_in_buffer -= ncopy;
        nbytes -= ncopy;
      } else {
          cinfo->src->fill_input_buffer(cinfo);
          if (cinfo->src->bytes_in_buffer==2 && cinfo->src->next_input_byte[0] == (JOCTET) 0xFF &&  cinfo->src->next_input_byte[1] == (JOCTET) JPEG_EOI) {
              break; /* EOF reached */
          }
      }
    }
    return nread;
}
/*
 * read data from the media to bit buffer, it should always read exactly nbytes
 * also this function perform filtration (when it needed) of the JPEG escapes, conversion FF 00 -> FF
 */
static int
file_read(void *cntx, void *buf, unsigned nbytes, bitfifo_byte_filter_t *filter)
{
    j_decompress_ptr cinfo = cntx;
    jpg2mpg_t *mp = cinfo->inputctl->bcm_decode->jpg2mpg;
    int rc;
    unsigned dst;
    unsigned junk;


    rc = read_from_jpeglib(cinfo, buf, nbytes);
    if (rc<0) {
        junk = nbytes;
    } else  {
        junk = nbytes - rc;
    }
    if (junk) {
        memset((char *)buf+nbytes-junk, 0, junk); /* Fill data with filler */
        mp->data_in.added += junk;
        mp->data_in.endOfFile = 1;
    };

    if (filter) { /* filter FF 00 to FF */
        for(dst = 0;;) {
            dst += filter(mp, ((unsigned char *)buf)+dst, nbytes - dst);
            if (dst == nbytes || mp->data_in.endOfFile) {
                break;
            }
            /* read data untill all buffer will be filled out */
            rc = read_from_jpeglib(cinfo, (unsigned char*)buf+dst, nbytes-dst);
            if (rc<0) {
                junk = nbytes - dst;
            } else  {
                junk = nbytes - dst - rc;
            }
            if (junk) {
                memset((char *)buf+dst-junk, 0, junk); /* Fill data with filler */
                mp->data_in.added += junk;
                mp->data_in.endOfFile = 1;
                mp->data_in.prev = 0; /* reset FF trigger */
                break;
            }

        }
    }

    return rc;
}


int PlayBackMPEGFile(const char *fileName, unsigned long *frame_addr);

int
brcmJPEGStartDecode(j_decompress_ptr cinfo)
{
    return 0;
}

int
brcmJPEGDecodeSetCntx(j_decompress_ptr cinfo)
{
    int rc;
    struct bcm_jpeg_decode *jpgdec;
    jpg2mpg_t *j_core;

    jpgdec = malloc(sizeof(*jpgdec));
    j_core = malloc(sizeof(*j_core));

    jpgdec->jpg2mpg = j_core;

    jpgdec->v2fb = NULL;
    jpgdec->v2zone = NULL;

    fout = fopen("test.es2", "wb");

    /* initialize converter structure */
    memset(j_core, 0, sizeof(*j_core));

    cinfo->inputctl->bcm_decode = jpgdec;


    bitfifo_out_init(&j_core->pb, file_write, cinfo);

    bitfifo_in_init(&j_core->gb, file_read, NULL /* no filter */, cinfo);


    convert_sof(j_core, cinfo);
    /* inherit Huffman table */
    convert_dht(j_core, cinfo);
    /* inherit Quant table */
    convert_dqt(j_core, cinfo);


    bitfifo_in_reserve(&j_core->gb, 16384);

    putpicturehdr(j_core);

    jpg_vld(j_core);

    free_huff(j_core);
    putseqend(j_core);
    bitfifo_out_flush(&j_core->pb);

    fclose(fout);

    PlayBackMPEGFile("test.es2", &jpgdec->frame_addr);

    inherit_mpegcntx(cinfo, jpgdec);

    free(jpgdec->jpg2mpg);
    jpgdec->jpg2mpg = NULL;

    return 0;
}

JDIMENSION
brcmReadScanLines (j_decompress_ptr cinfo, JSAMPARRAY scanlines, JDIMENSION max_lines)
{
  JDIMENSION  i;
  struct bcm_jpeg_decode *jpgdec = cinfo->inputctl->bcm_decode;

  for(i=0;i<max_lines;i++) {
    if (jpgdec->line_left==0) {
      jpgdec->line_left = Venom2FBZoneCapture(jpgdec->v2zone, jpgdec->mbline);
      jpgdec->mbline++;
      fprintf(stderr, "m %d\n", jpgdec->mbline);
    }

    Venom2FBZoneTransfer(jpgdec->v2zone, scanlines[i]);
    jpgdec->line_left--;

    cinfo->output_scanline++;
    if (cinfo->output_scanline == cinfo->image_height) {
        cinfo->global_state = DSTATE_STOPPING;
	brcmFBTransferDone(jpgdec);
    }
  }
  return i;
}
	
int
brcmFBTransferDone(struct bcm_jpeg_decode *jpgdec)
{
   return 0;
}

int
inherit_mpegcntx(j_decompress_ptr cinfo, struct bcm_jpeg_decode *jpgdec)
{
  jpgdec->res_x = jpgdec->jpg2mpg->horizontal_size;
  jpgdec->res_y = jpgdec->jpg2mpg->vertical_size;
  Venom2FBCreate(&jpgdec->v2fb, jpgdec->frame_addr);
  Venom2FBZoneCreate(jpgdec->v2fb, &jpgdec->v2zone, jpgdec->res_x, jpgdec->res_y, v2fbRGB);
  jpgdec->mbline = 0;
  jpgdec->line_left = 0;

  return 0;
}

void
convert_sof(jpg2mpg_t *mp, j_decompress_ptr cinfo)
{

    int i, c;

    mp->jpeg_height = cinfo->image_height;
    mp->jpeg_width = cinfo->image_width;
    mp->components = cinfo->num_components;

    mp->Hmax = mp->Vmax = 0;
    for (i=0; i<mp->components; i++) {
        c = cinfo->comp_info[i].component_index;
        mp->H[c-1] = cinfo->comp_info[i].h_samp_factor;
        mp->V[c-1] = cinfo->comp_info[i].v_samp_factor;
        mp->Tq[c-1] = cinfo->comp_info[i].quant_tbl_no;

        if (mp->Hmax < mp->H[c-1]) mp->Hmax = mp->H[c-1];
        if (mp->Vmax < mp->V[c-1]) mp->Vmax = mp->V[c-1];
    }
    set_hdr_para(mp);
}

void
convert_dht(jpg2mpg_t *mp, j_decompress_ptr cinfo)
{
    int i,k;
    JHUFF_TBL *htblptr;


    for (k = 0;k < 4;k++)
    {
        switch (k)
        {
        default:
        case 0:
            htblptr = cinfo->dc_huff_tbl_ptrs[0];
            break;
        case 1:
            htblptr = cinfo->ac_huff_tbl_ptrs[0];
            break;
        case 2:
            htblptr = cinfo->dc_huff_tbl_ptrs[1];
            break;
        case 3:
            htblptr = cinfo->ac_huff_tbl_ptrs[1];
            break;
        }

        (mp->gh.lastk)[k] = 0;
        for (i = 0;i < 16;i++)
        {
                (mp->gh.Lhuff)[k][i] = htblptr->bits[i+1];
                (mp->gh.lastk)[k] += (mp->gh.Lhuff)[k][i];
        }

        for (i = 0;i < (mp->gh.lastk)[k];i++)
        {
                (mp->gh.Huffval)[k][i] = htblptr->huffval[i];
        }

        gen_huff_size_table(mp,k);
        gen_huff_code_table(mp,k);
    }

}

void
convert_dqt(jpg2mpg_t *mp, j_decompress_ptr cinfo)
{
        unsigned int i,j;

        for (j = 0;j < 2;j++)
        {
                for (i = 0;i < 64;i++) {
                        (mp->intra_q)[j][i] = (uint8) cinfo->quant_tbl_ptrs[j]->quantval[jpeg_natural_order[i]]; /* Convert to zig-zag order */
                }

        }
}
