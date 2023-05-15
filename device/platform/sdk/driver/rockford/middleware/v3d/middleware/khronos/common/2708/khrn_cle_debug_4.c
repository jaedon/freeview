/*=============================================================================
Copyright (c) 2010 Broadcom Europe Limited.
All rights reserved.

Project  :  3D Tools
Module   :  Control list debugger

FILE DESCRIPTION
Dumps control lists to human readable form for debugging purposes.
=============================================================================*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "middleware/khronos/common/khrn_hw.h"
#include "middleware/khronos/common/2708/khrn_prod_4.h"

#ifdef PRINT_CLIST_DEBUG

#pragma pack(push,1)

/*Maximum number of nested branches in a control list */
#define SUBLIST_MAX_NESTING_DEPTH 2

typedef struct
{
   unsigned char code : 8;
   union {
      /*Used by various things, normally for an address */
      unsigned int word : 32;
      float fword;
      unsigned char byte;

      /* CONTROLPROC_STORE_FULL */
      /* CONTROLPROC_LOAD_FULL */
      struct {
         unsigned int nocolour : 1;
         unsigned int nodepthstencil : 1;
         unsigned int noclear : 1;
         unsigned int last : 1;
         unsigned int addr_shift4 : 28;
      } loadstore;

      /* CONTROLPROC_STORE_GENERAL */
      /* CONTROLPROC_LOAD_GENERAL */
      struct {
         unsigned short buffer        : 3;
         unsigned short xxx0          : 1;
         unsigned short format        : 2;
         unsigned short mode          : 2;
         unsigned short pixel_format  : 2;
         unsigned short xxx1          : 2;
         unsigned short dis_dbuf_swap : 1;
         unsigned short dis_col_clear : 1;
         unsigned short dis_z_clear   : 1;
         unsigned short dis_am_clear  : 1;
         unsigned int dis_col_dump    : 1;
         unsigned int dis_z_dump      : 1;
         unsigned int dis_am_dump     : 1;
         unsigned int eof             : 1;
         unsigned int addr_shift4     : 28;
      } loadstore_general;

      /*Used by: */
      /* CONTROLPROC_GLDRAWELEMENTS (this is the only one using max) */
      /* CONTROLPROC_GLDRAWARRAYS (addr is actually index of first vertex) */
      /* CONTROLPROC_VG_COORD_LIST (index_type must be zero) */
      /* CONTROLPROC_PRIMITIVE_LIST_FORMAT (length and addr unused) */
      struct {
         unsigned char prtype : 4;
         unsigned char index_type : 4;
         unsigned int length : 32;
         uintptr_t addr : 32;
         unsigned int max : 32;
      } primitive_list;

      /* CONTROLPROC_INLINE_VG_COORD_LIST */
      struct {
         unsigned char prtype : 4;
         unsigned char cont : 1;
      } inline_vg_coord_list;

      /* CONTROLPROC_CLIPPED_PRIM */
      struct {
         unsigned int clip0 : 1;
         unsigned int clip1 : 1;
         unsigned int clip2 : 1;
         unsigned int addr_shift3 : 29;
      } clipped_prim;

      /* CONTROLROC_GL_SHADER */
      struct {
         unsigned int num_attr : 3;
         unsigned int extended : 1;
         unsigned int addr_shift4 : 28;
      } gl_shader;

      /* CONTROLPROC_INLINE_VG_SHADER */
      struct {
         unsigned int sthrd : 1;
         unsigned int fcode_shift1 : 31;
         unsigned int funif : 32;
      } inline_vg_shader;

      /* CONTROLPROC_STATE_CFG */
      struct {
         unsigned int enfwd : 1;
         unsigned int enrev : 1;
         unsigned int cwise : 1;
         unsigned int endo  : 1;
         unsigned int aapl  : 1;
         unsigned int cvgreadrgb : 1;
         unsigned int rasosm : 2;
         unsigned int encvg : 1;
         unsigned int cvgmode : 2;
         unsigned int cvgread : 1;
         unsigned int zfunc : 3;
         unsigned int enzu  : 1;
         unsigned int enez  : 1;
         unsigned int enezu : 1;
      } cfg;

      /*CONTROLPROC_STATE_FLATSHADE */
      /* use .word */

      /*CONTROLPROC_STATE_POINT_SIZE */
      /* use .fword */

      /*CONTROLPROC_STATE_LINE_WIDTH */
      /* use .fword */

      /*CONTROLPROC_STATE_RHTX */
      short rhtx;

      /*CONTROLPROC_STATE_DEPTH_OFFSET */
      struct {
         unsigned int factor : 16;   /*actually float16 */
         unsigned int units : 16;    /*actually float16 */
      } depth_offset;

      /*CONTROLPROC_STATE_CLIP */
      struct {
         unsigned int x : 16;
         unsigned int y : 16;
         unsigned int w : 16;
         unsigned int h : 16;
      } clip;

      /*CONTROLPROC_STATE_VIEWPORT */
      struct {
         signed int x   : 16;
         signed int y   : 16;
      } viewport_offset;

      /*CONTROLPROC_STATE_CLIPZ */
      struct {
         float min;
         float max;
      } clipz;

      /*CONTROLPROC_STATE_CLIPPER_XY */
      struct {
         float hwidth;
         float hheight;
      } clipper_xy;

      /*CONTROLPROC_STATE_CLIPPER_Z */
      struct {
         float zscale;
         float zoffset;
      } clipper_z;

      /*CONTROLPROC_STATE_TILE_BINNING_MODE */
      struct {
         uintptr_t tamaddr               : 32;
         unsigned int tamsize            : 32;
         uintptr_t statearrayaddr        : 32;
         unsigned int width              :  8;
         unsigned int height             :  8;
         unsigned int msmode             :  1;
         unsigned int depth64            :  1;
         unsigned int statearrayautoinit :  1;
         unsigned int taibs              :  2;
         unsigned int tabs               :  2;
         unsigned int nonms_dbuf         :  1;
      } tilebincfg;

      /*CONTROLPROC_STATE_TILE_RENDERING_MODE */
      struct {
         uintptr_t memaddr               : 32;
         unsigned int width              : 16;
         unsigned int height             : 16;
         unsigned int msmode             :  1;
         unsigned int tilebufdepth       :  1;
         unsigned int framebufdepth      :  2;
         unsigned int decimationmode     :  2;
         unsigned int format             :  2;
         unsigned int enam               :  1;
         unsigned int cvgmode            :  1;
         unsigned int ezudir             :  1;
         unsigned int ezdis              :  1;
         unsigned int nonms_dbuf         :  1;
      } tilerencfg;

      /*CONTROLPROC_STATE_CLEARCOL */
      struct {
         unsigned long long col  : 64;
         unsigned int z          : 24;
         unsigned int am         : 8;
         unsigned int stencil    : 8;
      } clearcol;

      /*CONTROLPROC_STATE_TILE_COORDS */
      struct {
         unsigned int col : 8;
         unsigned int row : 8;
      } tilecoords;

   } ex;
} controlproc_instruction_t;

typedef struct
{
   unsigned int sthrd : 1;
   unsigned int incps : 1;
   unsigned int encl  : 1;
   unsigned int xxx0  : 13;

   unsigned int fnumu : 8;
   unsigned int fnumv : 8;
   unsigned int fcode;
   unsigned int funif;

   unsigned int vnumu : 16;
   unsigned int vattrsel : 8;
   unsigned int vattrsize : 8;
   unsigned int vcode;
   unsigned int vunif;

   unsigned int cnumu : 16;
   unsigned int cattrsel : 8;
   unsigned int cattrsize : 8;
   unsigned int ccode;
   unsigned int cunif;
   struct {
      unsigned int base;
      unsigned int sizem1 : 8;
      unsigned int stride : 8;
      unsigned int voffset : 8;
      unsigned int coffset : 8;
   } attr[8];
   unsigned int extended_stride[8];
} controlproc_gl_shader_t;

typedef struct
{
   unsigned int sthrd : 1;
   unsigned int incps : 1;
   unsigned int encl  : 1;
   unsigned int incch : 1;
   unsigned int xxx0  : 4;

   unsigned int vstride : 8;

   unsigned int fnumu : 8;
   unsigned int fnumv : 8;
   unsigned int fcode;
   unsigned int funif;

   unsigned int vbase;
} controlproc_nv_shader_t;

typedef struct {
   unsigned int sthrd : 1;
   unsigned int xxx0 : 7;
   unsigned int xxx1 : 8;
   unsigned int fnumu : 8;
   unsigned int xxx2 : 8;
   unsigned int fcode;
   unsigned int funif;
} controlproc_vg_shader_t;

#pragma pack(pop)

/* ----------------
 * Structs describing different encodings for compressed primitive lists
 */

#pragma pack(push,1)

#if (_MSC_VER == 1600)
#define STATIC_ASSERT(EXPR) static_assert((EXPR), "static_assert failed")
#else
#define STATIC_ASSERT(EXPR) extern int static_assert[(EXPR) ? 1 : -1]
#endif

typedef struct {
   unsigned char flag : 2;
   unsigned char diff : 6;
} vcm_prim_coding0;

STATIC_ASSERT(sizeof(vcm_prim_coding0) == 1);

typedef struct {
   unsigned char flag : 2;
   unsigned char dx_msbs : 6;
   unsigned char dx_lsb : 1;
   unsigned char dy : 7;
} vcm_prim_coding0_xy;

STATIC_ASSERT(sizeof(vcm_prim_coding0_xy) == 2);

typedef struct {
   unsigned short sig : 4;
   unsigned short diff0 : 4;
   unsigned short diff1 : 4;
   unsigned short diff2 : 4;
} vcm_prim_coding1;

STATIC_ASSERT(sizeof(vcm_prim_coding1) == 2);

typedef struct {
   unsigned short sig : 2;
   unsigned short diff0 : 14;
} vcm_prim_coding1_point;

STATIC_ASSERT(sizeof(vcm_prim_coding1_point) == 2);

/*
   msvc won't pack this into 3 bytes because it doesn't like bitfield members
   crossing underlying-type boundaries. at least, i think that's why it refuses
   to pack it into 3 bytes...
*/

typedef struct {
   unsigned int sig : 2;
   unsigned int flag : 2;
   unsigned int dx : 10;
   unsigned int dy : 10;
   unsigned int dummy : 8; /* ! */
} vcm_prim_coding1_xy;

STATIC_ASSERT(sizeof(vcm_prim_coding1_xy) == 4);

typedef struct {
   unsigned int sig : 4;
   unsigned int diff1 : 6;
   unsigned int diff2 : 6;
   unsigned int idx0 : 16;
} vcm_prim_coding2;

STATIC_ASSERT(sizeof(vcm_prim_coding2) == 4);

typedef struct {
   unsigned char sig : 2;
   unsigned char diff1 : 6;
   unsigned short idx0 : 16;
} vcm_prim_coding2_line;

STATIC_ASSERT(sizeof(vcm_prim_coding2_line) == 3);

typedef struct {
   unsigned int sig : 4;
   unsigned int dx1 : 7;
   unsigned int dy1 : 7;
   unsigned int dx2 : 7;
   unsigned int dy2 : 7;
   unsigned int x0 : 16;
   unsigned int y0 : 16;
} vcm_prim_coding2_xy;

STATIC_ASSERT(sizeof(vcm_prim_coding2_xy) == 8);

typedef struct {
   unsigned short sig : 2;
   unsigned short dx1_msbs : 6;
   unsigned short dx1_lsb : 1;
   unsigned short dy1 : 7;
   unsigned short x0 : 16;
   unsigned short y0 : 16;
} vcm_prim_coding2_line_xy;

STATIC_ASSERT(sizeof(vcm_prim_coding2_line_xy) == 6);

typedef struct {
   unsigned char sig : 8;
   unsigned short idx0 : 16;
   unsigned short idx1 : 16;
   unsigned short idx2 : 16;
} vcm_prim_coding3;

STATIC_ASSERT(sizeof(vcm_prim_coding3) == 7);

typedef struct {
   unsigned char sig : 8;
   unsigned short x0 : 16;
   unsigned short y0 : 16;
   unsigned short x1 : 16;
   unsigned short y1 : 16;
   unsigned short x2 : 16;
   unsigned short y2 : 16;
} vcm_prim_coding3_xy;

STATIC_ASSERT(sizeof(vcm_prim_coding3_xy) == 13);

typedef struct {
   unsigned char sig : 8;
   unsigned short amount : 16;
} vcm_prim_coding_branch;

STATIC_ASSERT(sizeof(vcm_prim_coding_branch) == 3);

typedef union {
   vcm_prim_coding0 c0;
   vcm_prim_coding0_xy c0_xy;
   vcm_prim_coding1 c1;
   vcm_prim_coding1_point c1_point;
   vcm_prim_coding1_xy c1_xy;
   vcm_prim_coding2 c2;
   vcm_prim_coding2_line c2_line;
   vcm_prim_coding2_xy c2_xy;
   vcm_prim_coding2_line_xy c2_line_xy;
   vcm_prim_coding3 c3;
   vcm_prim_coding3_xy c3_xy;
   vcm_prim_coding_branch branch;
} COMPRESSED_PRIM_T;

#undef STATIC_ASSERT

#pragma pack(pop)



static FILE *dbgFP = NULL;
static unsigned char *s_vertMem = NULL;

void control_list_debug_file(FILE *fp)
{
   if (fp != NULL)
      dbgFP = fp;
   else
      dbgFP = stdout;
}

static void debug_message_for_instr(controlproc_instruction_t const *instr)
{
#define KHRN_HW_INSTR_HALT                   0
#define KHRN_HW_INSTR_NOP                    1
#define KHRN_HW_INSTR_MARKER                 2
#define KHRN_HW_INSTR_RESET_MARKER_COUNT     3
#define KHRN_HW_INSTR_FLUSH                  4
#define KHRN_HW_INSTR_FLUSH_ALL_STATE        5
#define KHRN_HW_INSTR_START_TILE_BINNING     6
#define KHRN_HW_INSTR_INCR_SEMAPHORE         7
#define KHRN_HW_INSTR_WAIT_SEMAPHORE         8

   switch (instr->code)
   {
   case KHRN_HW_INSTR_HALT:
      fprintf(dbgFP, "HALT\n");
      break;
   case KHRN_HW_INSTR_NOP:
      fprintf(dbgFP, "NOP\n");
      break;
   case KHRN_HW_INSTR_MARKER:
      fprintf(dbgFP, "MARKER\n");
      break;
   case KHRN_HW_INSTR_RESET_MARKER_COUNT:
      fprintf(dbgFP, "RESET_MARKER_COUNT\n");
      break;
   case KHRN_HW_INSTR_FLUSH:
      fprintf(dbgFP, "FLUSH\n");
      break;
   case KHRN_HW_INSTR_FLUSH_ALL_STATE:
      fprintf(dbgFP, "FLUSH_ALL_STATE\n");
      break;
   case KHRN_HW_INSTR_START_TILE_BINNING:
      fprintf(dbgFP, "START_TILE_BINNING\n");
      break;
   case KHRN_HW_INSTR_INCR_SEMAPHORE:
      fprintf(dbgFP, "INCR_SEMAPHORE\n");
      break;
   case KHRN_HW_INSTR_WAIT_SEMAPHORE:
      fprintf(dbgFP, "WAIT_SEMAPHORE\n");
      break;
   case KHRN_HW_INSTR_STORE_SUBSAMPLE:
      fprintf(dbgFP, "STORE_SUBSAMPLE\n");
      break;
   case KHRN_HW_INSTR_STORE_SUBSAMPLE_EOF:
      fprintf(dbgFP, "STORE_SUBSAMPLE_EOF\n");
      break;
   case KHRN_HW_INSTR_BRANCH:
      fprintf(dbgFP, "BRANCH\n");
      break;
   case KHRN_HW_INSTR_BRANCH_SUB:
      fprintf(dbgFP, "BRANCH_SUB\n");
      break;
   case KHRN_HW_INSTR_RETURN:
      fprintf(dbgFP, "RETURN\n");
      break;
   case KHRN_HW_INSTR_REPEAT_START_MARKER:
      fprintf(dbgFP, "REPEAT_START_MARKER\n");
      break;
   case KHRN_HW_INSTR_REPEAT_FROM_START_MARKER:
      fprintf(dbgFP, "REPEAT_FROM_START_MARKER\n");
      break;
   case KHRN_HW_INSTR_STORE_FULL:
      fprintf(dbgFP, "STORE_FULL\n");
      break;
   case KHRN_HW_INSTR_LOAD_FULL:
      fprintf(dbgFP, "LOAD_FULL\n");
      break;
   case KHRN_HW_INSTR_STORE_GENERAL:
      fprintf(dbgFP, "STORE_GENERAL\n");
      break;
   case KHRN_HW_INSTR_LOAD_GENERAL:
      fprintf(dbgFP, "LOAD_GENERAL\n");
      break;
   case KHRN_HW_INSTR_GLDRAWELEMENTS:
      fprintf(dbgFP, "GLDRAWELEMENTS\n");
      break;
   case KHRN_HW_INSTR_GLDRAWARRAYS:
      fprintf(dbgFP, "GLDRAWARRAYS\n");
      break;
   case KHRN_HW_INSTR_COMPRESSED_LIST:
      fprintf(dbgFP, "COMPRESSED_LIST\n");
      break;
   case KHRN_HW_INSTR_CLIPPED_PRIM:
      fprintf(dbgFP, "CLIPPED_PRIM\n");
      break;
   case KHRN_HW_INSTR_PRIMITIVE_LIST_FORMAT:
      fprintf(dbgFP, "PRIMITIVE_LIST_FORMAT\n");
      break;
   case KHRN_HW_INSTR_VG_COORD_LIST:
      fprintf(dbgFP, "VG_COORD_LIST\n");
      break;
   case KHRN_HW_INSTR_GL_SHADER:
      fprintf(dbgFP, "GL_SHADER\n");
      break;
   case KHRN_HW_INSTR_NV_SHADER:
      fprintf(dbgFP, "NV_SHADER\n");
      break;
   case KHRN_HW_INSTR_STATE_CFG:
      fprintf(dbgFP, "STATE_CFG\n");
      break;
   case KHRN_HW_INSTR_STATE_FLATSHADE:
      fprintf(dbgFP, "STATE_FLATSHADE\n");
      break;
   case KHRN_HW_INSTR_STATE_POINT_SIZE:
      fprintf(dbgFP, "STATE_POINT_SIZE\n");
      break;
   case KHRN_HW_INSTR_STATE_LINE_WIDTH:
      fprintf(dbgFP, "STATE_LINE_WIDTH\n");
      break;
   case KHRN_HW_INSTR_STATE_CLIP:
      fprintf(dbgFP, "STATE_CLIP\n");
      break;
   case KHRN_HW_INSTR_STATE_CLIPZ:
      fprintf(dbgFP, "STATE_CLIPZ\n");
      break;
   case KHRN_HW_INSTR_STATE_TILE_BINNING_MODE:
      fprintf(dbgFP, "TILE_BINNING_MODE\n");
      break;
   case KHRN_HW_INSTR_STATE_TILE_RENDERING_MODE:
      fprintf(dbgFP, "TILE_RENDERING_MODE\n");
      break;
   case KHRN_HW_INSTR_STATE_CLEARCOL:
      fprintf(dbgFP, "STATE_CLEARCOL\n");
      break;
   case KHRN_HW_INSTR_STATE_TILE_COORDS:
      fprintf(dbgFP, "STATE_TILE_COORDS\n");
      break;
   case KHRN_HW_INSTR_VG_SHADER:
      fprintf(dbgFP, "VG_SHADER\n");
      break;
   case KHRN_HW_INSTR_INLINE_VG_SHADER:
      fprintf(dbgFP, "INLINE_VG_SHADER\n");
      break;
   case KHRN_HW_INSTR_STATE_RHTX:
      fprintf(dbgFP, "STATE_RHTX\n");
      break;
   case KHRN_HW_INSTR_STATE_DEPTH_OFFSET:
      fprintf(dbgFP, "STATE_DEPTH_OFFSET\n");
      break;
   case KHRN_HW_INSTR_STATE_VIEWPORT_OFFSET:
      fprintf(dbgFP, "STATE_VIEWPORT_OFFSET\n");
      break;
   case KHRN_HW_INSTR_STATE_CLIPPER_XY:
      fprintf(dbgFP, "STATE_CLIPPER_XY\n");
      break;
   case KHRN_HW_INSTR_STATE_CLIPPER_Z:
      fprintf(dbgFP, "STATE_CLIPPER_Z\n");
      break;   
   default:
      fprintf(dbgFP, "Unimplemented op-code: 0x%08x\n", instr->code);
      break;
   }
}

#define COMPRESSED_PRIM_TYPE_POINT 0
#define COMPRESSED_PRIM_TYPE_LINE 1
#define COMPRESSED_PRIM_TYPE_TRIANGLE 2
#define COMPRESSED_PRIM_TYPE_RHT 3

#define COMPRESSED_PRIM_DATA_16BIT 1
#define COMPRESSED_PRIM_DATA_24BIT 2
#define COMPRESSED_PRIM_DATA_XY 3

static int trans_rel_idx(int index, unsigned char size) {

   int ret = 0;
   /* Handle negative numbers */
   if (index & (1 << (size - 1)))
      ret = (index |= (-1 << (size - 1)));
   else ret = index;

   return ret;
}

static unsigned int trans_rel_xy(int x, int y, unsigned char size)
{
   return (trans_rel_idx(y, size) & 0xffff) << 16 | (trans_rel_idx(x, size) & 0xffff);
}

static unsigned int xy_add(unsigned int xy0, unsigned int xy1)
{
   return ((xy0 & 0xffff0000) + (xy1 & 0xffff0000)) | ((xy0 + xy1) & 0x0000ffff);
}

static void print12dot4andz(unsigned char *ptr)
{
/*
         unsigned short x, y;
         float          fx, fy, z;
      
         if (ptr != NULL)
         {
            x = (*ptr) & 0xFF;
            x |= ((*(ptr + 1)) & 0xFF) << 8;
            fx = (float)x / 15.0f;
      
            y = ((*(ptr + 2)) & 0xFF);
            y |= ((*(ptr + 3)) & 0xFF) << 8;
            fy = (float)y / 15.0f;
      
            z = *((float*)(ptr + 4));
      
            fprintf(dbgFP, "(%f,%f,%f)", fx, fy, z);
         }
         else
            fprintf(dbgFP, "(NULLP, NULLP, NULLP)");*/
      
}

uintptr_t handle_compressed_prim(unsigned int prim_type, unsigned int data_type, uintptr_t primPtr)
{
   char next_byte;
   uintptr_t ptr = primPtr;
   int terminate = 0;

   fprintf(dbgFP, " start of compressed primitive data at (0x%08x:dev)(0x%08x:virt)\n",
                     (uintptr_t)khrn_hw_addr((void*)primPtr), primPtr);

   while (!terminate)
   {
      next_byte = *(char*)ptr;
      
      fprintf(dbgFP, "0x%08x : %02x ", khrn_hw_addr((void*)ptr), (unsigned char)next_byte);

      if (next_byte == (char)0x82) 
      {
         /* branch */
/*
         wire [31:0] p1_relbr_addr = {p01_curr_addr[31:5] + {{11{p1_item[23]}}, p1_item[23:8]}, 5'h0};
*/
         unsigned short relBranch;
         uintptr_t add;
         uintptr_t oldPtr = ptr;
         relBranch = *((unsigned char*)(ptr+1));
         relBranch |= (*((unsigned char*)(ptr+2)) << 8);

         if ((relBranch >> 15) != 0)
            add = 0xFFFF0000 | relBranch;
         else
            add = relBranch;
         add = add << 5;

         ptr = ptr & (~31);

         ptr += (int)add;

         fprintf(dbgFP, "branch 0x%08x (%d bytes)", (short)relBranch, (int)add);
      } 
      else if(next_byte == (char)0x80) 
      {
         /* terminator */
         ptr += 1;
         terminate = 1;
      } 
      else if (prim_type == COMPRESSED_PRIM_TYPE_TRIANGLE && data_type == COMPRESSED_PRIM_DATA_16BIT) 
      {
         if (next_byte == (char)0x81) 
         {
            /* Coding 3 (triangles) */
            vcm_prim_coding3 prim;
            memcpy(&prim, (void*)ptr, 7);
            fprintf(dbgFP, "[%d,%d,%d]", prim.idx0, prim.idx1, prim.idx2);
            print12dot4andz(&s_vertMem[prim.idx0]);
            print12dot4andz(&s_vertMem[prim.idx1]);
            print12dot4andz(&s_vertMem[prim.idx2]);
            ptr += 7;
         }
         else if ((next_byte & (char)0xf) == 0xf) 
         {
            /* Coding 2 (triangles) */
            vcm_prim_coding2 prim;
            memcpy(&prim, (void*)ptr, 4);

            fprintf(dbgFP, "[%d,%d,%d]", 
               prim.idx0, 
               trans_rel_idx(prim.diff1, 6) + prim.idx0, 
               trans_rel_idx(prim.diff2, 6) + prim.idx0);

            ptr += 4;
         }
         else if ((next_byte & (char)0x3) == 0x3) 
         {
            /* Coding 1 (triangles) */
            vcm_prim_coding1 prim;
            memcpy(&prim, (void*)ptr, 2);

            fprintf(dbgFP, "[prev0 + %d, prev1 + %d, prev2 + %d]", 
               trans_rel_idx(prim.diff0, 4), 
               trans_rel_idx(prim.diff1, 4), 
               trans_rel_idx(prim.diff2, 4));

            ptr += 2;
         }
         else
         {
            /* Coding 0 (triangles) */
            vcm_prim_coding0 prim;
            memcpy(&prim, (void*)ptr, 1);

            assert(prim.flag <= 2);
            switch (prim.flag) {
            case 0:
               fprintf(dbgFP, "[prev2, prev1, prev2 + %d]", trans_rel_idx(prim.diff, 6));
               break;
            case 1:
               fprintf(dbgFP, "[prev0, prev2, prev2 + %d]", trans_rel_idx(prim.diff, 6));
               break;
            case 2:
               fprintf(dbgFP, "[prev1, prev0, prev2 + %d]", trans_rel_idx(prim.diff, 6));
               break;
            default:
               /* invalid flag */
               assert(0);
            }
            ptr += 1;
         }
      }
      else if ((prim_type == COMPRESSED_PRIM_TYPE_LINE || prim_type == COMPRESSED_PRIM_TYPE_RHT) && data_type == COMPRESSED_PRIM_DATA_16BIT) 
      {
         if (next_byte == (char)0x81) 
         {
            /* Coding 3 (Lines/RHTs) */
            ptr += 5;
         } 
         else if ((next_byte & (char)0x3) == 0x2) 
         {
            /* Coding 2 (Lines/RHTs) */
            ptr += 3;
         }
         else if ((next_byte & (char)0x3) == 0x3) 
         {
            /* Coding 1 (Lines/RHTs) */
            ptr += 2;

         }
         else
         {
            /* Coding 0 (Lines/RHTs) */
            ptr += 1;
         }
      }
      else if (prim_type == COMPRESSED_PRIM_TYPE_POINT && data_type == COMPRESSED_PRIM_DATA_16BIT) 
      {
         if (next_byte == (char)0x81) 
         {
            /* Coding 3 (Points) */
            ptr += 3;
         }
         else if ((next_byte & (char)0x3) == 0x3) 
         {
            ptr += 2;
         }
         else
         {
            /* Coding 0 (Points) */
            ptr += 1;
         }
      }
      else if (prim_type == COMPRESSED_PRIM_TYPE_TRIANGLE && data_type == COMPRESSED_PRIM_DATA_XY) 
      {
         if (next_byte == (char)0x81) 
         {
            /* Coding 3 (XY Triangles) */
            ptr += 13;
         }
         else if ((next_byte & (char)0xf) == 0xf) 
         {
            /* Coding 2 (XY Triangles) */
            ptr += 8;
         }
         else if ((next_byte & (char)0x3) == 0x3) 
         {
            /* Coding 1 (XY Triangles) */
            ptr += 3;
         }
         else
         {
            /* Coding 0 (XY Triangles) */
            ptr += 2;
         }
      }
      else if ((prim_type == COMPRESSED_PRIM_TYPE_LINE || prim_type == COMPRESSED_PRIM_TYPE_RHT) && data_type == COMPRESSED_PRIM_DATA_XY) 
      {
         if (next_byte == (char)0x81) {
            /* Coding 3 (XY RHTs) */
            ptr += 9;
         }
         else if ((next_byte & (char)0x3) == 0x2) 
         {
            /* Coding 2 (XY RHTs) */
            ptr += 6;
         }
         else if ((next_byte & (char)0x3) == 0x3) 
         {
            /* Coding 1 (XY RHTs) */
            ptr += 3;
         }
         else 
         {
            /* Coding 0 (XY RHTs) */
            ptr += 2;
         }
      }
      else
      {
         /* Unsupported mode */
         fprintf(dbgFP, "INVALID MODE IN COMPRESSED PRIM\n");
         fflush(dbgFP);
         assert(0);
      }

      fprintf(dbgFP, "\n");
   }

   fprintf(dbgFP, " compressed primitive data ends at (0x%08x:dev)(0x%08x:virt)\n",
                  (uintptr_t)khrn_hw_addr((void*)ptr), ptr);

   return ptr;
}

/* Rendering = 1 for render list, 0 for bin list */
void control_list_debug_dump(void *list_begin, void *list_end, unsigned int rendering)
{
   uintptr_t sublist_return_stack[SUBLIST_MAX_NESTING_DEPTH];
   int sublist = 0; /* current depth */

   uintptr_t list = (uintptr_t)list_begin;
   int halt = 0;

   unsigned int prim_type;
   unsigned int data_type;

   if (dbgFP == NULL)
      dbgFP = stdout;

   while (!halt && list != (uintptr_t)list_end)
   {
      controlproc_instruction_t const *instr = (controlproc_instruction_t const *)list;

      fprintf(dbgFP, "0x%08x : ", (uintptr_t)khrn_hw_addr((void*)list));
      debug_message_for_instr(instr);

      fflush(dbgFP);

      switch (instr->code)
      {
      case KHRN_HW_INSTR_HALT:
         halt = 1;
         list++;
         break;
      case KHRN_HW_INSTR_NOP:
         list++;
         break;
      case KHRN_HW_INSTR_MARKER:
         list++;
         break;
      case KHRN_HW_INSTR_RESET_MARKER_COUNT:
         list++;
         break;
      case KHRN_HW_INSTR_FLUSH:
         list++;
         break;
      case KHRN_HW_INSTR_FLUSH_ALL_STATE:
         list++;
         break;
      case KHRN_HW_INSTR_START_TILE_BINNING:
         list++;
         break;
      case KHRN_HW_INSTR_INCR_SEMAPHORE:
         list++;
         break;
      case KHRN_HW_INSTR_WAIT_SEMAPHORE:
         list++;
         break;

      case KHRN_HW_INSTR_BRANCH:
         /* Branch to the target address by setting the list position to it */
         list = (uintptr_t)khrn_hw_unaddr(instr->ex.word);
         fprintf(dbgFP, " branching to (0x%08x:dev)(0x%08x:virt)\n", instr->ex.word, list);
         break;

      case KHRN_HW_INSTR_BRANCH_SUB:
         /* Store the current list position before branching into a sublist */

         assert(sublist < SUBLIST_MAX_NESTING_DEPTH);
         sublist_return_stack[sublist++] = (list += 5);

         /* Branch to the target address by setting the list position to it */
         list = (uintptr_t)khrn_hw_unaddr(instr->ex.word);
         fprintf(dbgFP, " call subroutine at (0x%08x:dev)(0x%08x:virt)\n", instr->ex.word, list);

/*
list += 5;
fprintf(dbgFP, " IGNORING subroutine at (0x%08x:dev)(0x%08x:virt)\n", instr->ex.word, list);
*/

         break;

      case KHRN_HW_INSTR_RETURN:
         /* Restore the old list position from the return address stack */
         if(!sublist)
         {
            /* It's legal to have a return without a branch sub, and it means nop */
            list++;
         }
         else
         {
            assert(sublist <= SUBLIST_MAX_NESTING_DEPTH);
            list = sublist_return_stack[--sublist];
         }
         break;
         
      case KHRN_HW_INSTR_REPEAT_START_MARKER:
         list++;
         break;

      case KHRN_HW_INSTR_REPEAT_FROM_START_MARKER:
         list+=2;
         break;

      case KHRN_HW_INSTR_STORE_SUBSAMPLE:
      case KHRN_HW_INSTR_STORE_SUBSAMPLE_EOF:
         list++;
         break;

      case KHRN_HW_INSTR_STORE_FULL:
         list += 5;
         break;

      case KHRN_HW_INSTR_LOAD_FULL:
         list += 5;
         break;

      case KHRN_HW_INSTR_STORE_GENERAL:
         list += 7;
         break;

      case KHRN_HW_INSTR_LOAD_GENERAL:
         list += 7;
         break;

         /*Primitive lists */
      case KHRN_HW_INSTR_GLDRAWELEMENTS:
         /* counts as a sublist */
         assert(sublist < SUBLIST_MAX_NESTING_DEPTH);

         /* Record primitive type state change */
         assert(instr->ex.primitive_list.prtype <= 6);
         assert(instr->ex.primitive_list.index_type <= 2);
         assert(instr->ex.primitive_list.max <= 0xffff);

         fprintf(dbgFP, " %d indices at (0x%08x:dev)(0x%08x:virt)\n", instr->ex.primitive_list.length, 
               instr->ex.primitive_list.addr, (uintptr_t)khrn_hw_unaddr(instr->ex.primitive_list.addr));

         list += 14;
         break;

      case KHRN_HW_INSTR_GLDRAWARRAYS:

         /* Record primitive type state change */
         assert(instr->ex.primitive_list.prtype <= 6);

         fprintf(dbgFP, " %d verts at index %d\n", instr->ex.primitive_list.length, 
               instr->ex.primitive_list.addr);

         list += 10;
         break;

      case KHRN_HW_INSTR_COMPRESSED_LIST:
         list++;

         if (instr->ex.byte == 0x80) 
         {
            /* ignore completely empty compressed lists */
            fprintf(dbgFP, " list empty\n");
            list++;
         }
         else 
         {
            list = handle_compressed_prim(prim_type, data_type, list);
         }
         break;

      case KHRN_HW_INSTR_CLIPPED_PRIM:
         {
            list += 5;
            list = handle_compressed_prim(prim_type, data_type, list);
            break;
         }

      case KHRN_HW_INSTR_PRIMITIVE_LIST_FORMAT:
         assert(instr->ex.primitive_list.prtype <= 3);
         assert(instr->ex.primitive_list.index_type >= 1 && instr->ex.primitive_list.index_type <= 3);

         prim_type = instr->ex.primitive_list.prtype;
         data_type = instr->ex.primitive_list.index_type;

         list += 2;
         break;

      case KHRN_HW_INSTR_VG_COORD_LIST:
         /* counts as a sublist */
         assert(sublist < SUBLIST_MAX_NESTING_DEPTH);

         /* Record primitive type state change */
         assert(instr->ex.primitive_list.prtype == 1 || instr->ex.primitive_list.prtype >= 3 && instr->ex.primitive_list.prtype <= 6);
         assert(instr->ex.primitive_list.index_type == 0);

/*
         vcm_mode = VCM_MODE_COUNT;
         state->running_vcm = vcm_run_uncompressed(
            instr->ex.primitive_list.prtype,
            3,
            0,
            instr->ex.primitive_list.addr,
            instr->ex.primitive_list.length,
            0xffffffff); / * effectively disable max_index * /
*/
         list += 10;
         break;

         /*State data */
      case KHRN_HW_INSTR_GL_SHADER:
         {
            uintptr_t shaderaddr = instr->ex.gl_shader.addr_shift4 << 4;
            int num_attr = instr->ex.gl_shader.num_attr;
            char buffer[sizeof(controlproc_gl_shader_t)];
            int i;
            controlproc_gl_shader_t *shader;

            if ((num_attr == 0) || instr->ex.gl_shader.extended)
               num_attr = 8;

            khrn_memcpy(buffer, khrn_hw_unaddr(shaderaddr), sizeof(controlproc_gl_shader_t));
            shader = (controlproc_gl_shader_t*)buffer;

            fprintf(dbgFP, "  FS uniforms : %d\n", shader->fnumu);
            fprintf(dbgFP, "  FS varyings : %d\n", shader->fnumv);
            fprintf(dbgFP, "  FS code     : 0x%08x\n", shader->fcode);
            fprintf(dbgFP, "  FS uniforms : 0x%08x\n", shader->funif);

            fprintf(dbgFP, "  VAttr select : 0x%02x\n", shader->vattrsel);
            fprintf(dbgFP, "  VAttr size   : 0x%02x\n", shader->vattrsize);

            s_vertMem = 0;
            for (i = 0; i < 8; i++)
            {
               if (shader->vattrsel & (1 << i))
               {
                  if (s_vertMem == 0)
                     s_vertMem = (unsigned char*)khrn_hw_unaddr(shader->attr[0].base);

                  fprintf(dbgFP, "  Attr[%d]\n", i);
                  fprintf(dbgFP, "    Base    : 0x%08x\n", shader->attr[i].base);
                  fprintf(dbgFP, "    Size    : %d\n", shader->attr[i].sizem1 + 1);
                  fprintf(dbgFP, "    Stride  : %d\n", shader->attr[i].stride);
                  fprintf(dbgFP, "    voffset : %d\n", shader->attr[i].voffset);
                  fprintf(dbgFP, "    coffset : %d\n", shader->attr[i].coffset);
               }
            }

            list += 5;
            break;
         }
      case KHRN_HW_INSTR_NV_SHADER:
         {
            uintptr_t shaderaddr = instr->ex.word;
            assert(!(shaderaddr & 15));
/*
            controlproc_nv_shader_t shader;
            gr_memcpyb_from_vc(&shader, shaderaddr, sizeof(controlproc_nv_shader_t));
*/
            list += 5;
            break;
         }
      case KHRN_HW_INSTR_VG_SHADER:
         {
            uintptr_t shaderaddr = instr->ex.word;
            assert(!(shaderaddr & 15));
/*
            controlproc_vg_shader_t shader;
            gr_memcpyb_from_vc(&shader, shaderaddr, sizeof(controlproc_vg_shader_t));
*/
            list += 5;
            break;
         }
      case KHRN_HW_INSTR_INLINE_VG_SHADER:
         {
            list += 9;
            break;
         }
      case KHRN_HW_INSTR_STATE_CFG:
         list += 4;
         break;
      case KHRN_HW_INSTR_STATE_FLATSHADE:
         list += 5;
         break;
      case KHRN_HW_INSTR_STATE_POINT_SIZE:
         list += 5;
         break;
      case KHRN_HW_INSTR_STATE_LINE_WIDTH:
         list += 5;
         break;
      case KHRN_HW_INSTR_STATE_RHTX:
         list += 3;
         break;
      case KHRN_HW_INSTR_STATE_DEPTH_OFFSET:   /*needs 16->32bit float conversion */
         list += 5;
         break;
      case KHRN_HW_INSTR_STATE_CLIP:
         list += 9;
         break;
      case KHRN_HW_INSTR_STATE_VIEWPORT_OFFSET:
         list += 5;
         break;
      case KHRN_HW_INSTR_STATE_CLIPZ:
         list += 9;
         break;
      case KHRN_HW_INSTR_STATE_CLIPPER_XY:
         list += 9;
         break;
      case KHRN_HW_INSTR_STATE_CLIPPER_Z:
         list += 9;
         break;
      case KHRN_HW_INSTR_STATE_TILE_BINNING_MODE:
         fprintf(dbgFP, " num tiles X = %d, num tiles Y = %d\n", instr->ex.tilebincfg.width, instr->ex.tilebincfg.height);
         fprintf(dbgFP, " alloc_start = (0x%08x:dev)(0x%08x:virt)\n", instr->ex.tilebincfg.tamaddr, 
                                             (uintptr_t)khrn_hw_unaddr(instr->ex.tilebincfg.tamaddr));
         fprintf(dbgFP, " alloc_size = %d\n", instr->ex.tilebincfg.tamsize); 
         list += 16;
         break;
      case KHRN_HW_INSTR_STATE_TILE_RENDERING_MODE:
         fprintf(dbgFP, " addr = (0x%08x:dev)(0x%08x:virt)\n", instr->ex.tilerencfg.memaddr, 
                              (uintptr_t)khrn_hw_unaddr(instr->ex.tilerencfg.memaddr));
         fprintf(dbgFP, " width = %d\n", instr->ex.tilerencfg.width); 
         fprintf(dbgFP, " height = %d\n", instr->ex.tilerencfg.height); 
         list += 11;
         break;
      case KHRN_HW_INSTR_STATE_CLEARCOL:
/*
         miscReg.clearcol = instr->ex.clearcol.col;
         miscReg.cleardepth = instr->ex.clearcol.z;
         miscReg.clearstencil = instr->ex.clearcol.stencil;
*/
         list += 14;
         break;
      case KHRN_HW_INSTR_STATE_TILE_COORDS:
         fprintf(dbgFP, " coord = %d,%d\n", instr->ex.tilecoords.col, instr->ex.tilecoords.row); 
         list += 3;
         break;

      default:
         fprintf(dbgFP, "!!!! UNRECOGNISED INSTRUCTION 0x%04X (%d)\n", instr->code, instr->code);
         fflush(dbgFP);
         /*assert(0);*/
      }
   }

   if (halt)
      fprintf(dbgFP, "Halted\n");
   else
      fprintf(dbgFP, "Reached list_end\n");
}

#endif /* PRINT_CLIST_DEBUG */
