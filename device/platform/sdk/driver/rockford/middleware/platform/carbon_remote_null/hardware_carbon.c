
#include "default_carbon.h"

#include "v3d_regs.h"
#include "bstd.h" /* For BSTD_ENDIAN_BIG define */

#include <malloc.h>
#include <stdio.h>
#include <string.h>

void * carbon_v3d_open(void* irq_vector, void* readmem, void* writemem);
void carbon_v3d_close(void * inst);
void carbon_v3d_slavewrite(void * inst, uint32_t addr, uint32_t data);
uint32_t carbon_v3d_slaveread(void * inst, uint32_t addr);
uint32_t carbon_v3d_reset(void * inst);
uint32_t carbon_v3d_flushcache(void * inst);

typedef struct
{
   uint32_t                commandMap[BEGL_HW_NUM_COMMANDS];
   void * cm;        /* carbon model */
} CARPL_Hardware;

/*****************************************************************************
 * H/W driver interface
 *****************************************************************************/


#ifdef DEBUG_CARBON

static volatile int read_dbg = 0;
static volatile int write_dbg = 0;

void carbon_readmem(void * addr, unsigned int size, void * mem)
{
   if (read_dbg) printf("(R %p) ", addr);

   for (int i = 0; i < (int)size; i++)
   {
      unsigned char d = ((unsigned char *)addr)[i];
      if (read_dbg) printf("0x%2.2x ", d);
      ((unsigned char *)mem)[i] = d;
   }
   if (read_dbg) putchar('\n');
}

void carbon_writemem(void * addr, unsigned int size, unsigned int enables, void * mem)
{
   if (write_dbg) printf("(W %p) ", addr);

   for (int i = 0; i < (int)size; i++)
   {
      if (enables & 0x1)
      {
         unsigned char d = ((unsigned char *)mem)[i];
         if (write_dbg) printf("0x%2.2x ", d);
         ((unsigned char *)addr)[i] = d;
      }
      else
         if (write_dbg) printf("0x?? ");
      enables >>= 1;
   }
   if (write_dbg) putchar('\n');
}
#endif



/* Acquire 3D hardware*/
static bool DriverAcquire(void *context, const BEGL_HWAcquireSettings *settings)
{
   CARPL_Hardware                   *data = (CARPL_Hardware*)context;

   /* need to defer to here the carbon creation as we need the irq vector */
   if (data->cm == NULL)
   {
      unsigned int ident0, ident1, ident2;
      char bcg_version[3];
      bool defaultBigEndian = false;
      bool endianSwappable = false;
      bool wantBigEndian = false;

#if BSTD_CPU_ENDIAN == BSTD_ENDIAN_BIG
      wantBigEndian = true;
#endif

#ifdef DEBUG_CARBON
      /* hook out the read/write transactions */
      data->cm = carbon_v3d_open(settings->ISR, carbon_readmem, carbon_writemem);
#else
      data->cm = carbon_v3d_open(settings->ISR, NULL, NULL);
#endif

      /* dump some debug */
      ident0 = carbon_v3d_slaveread(data->cm, IDENT0);
      ident1 = carbon_v3d_slaveread(data->cm, IDENT1);
      ident2 = carbon_v3d_slaveread(data->cm, IDENT2);

      bcg_version[0] = ((ident0 >> 24) == 0x2) ? 'A' : 'B';
      bcg_version[1] = (ident1 & 0xF) + '0';
      bcg_version[2] = '\0';

      printf("\nSimCarbon - %s Software Simulator\n", bcg_version);
      printf("Technology Version 0x%2.2x\n", ident0 >> 24);
      printf("Revision 0x%2.2x\n", ident1 & 0xF);
      if (((ident0 >> 24) == 0x2) && ((ident1 & 0xF) == 0))
      {
         printf("Number of slices 0x%2.2x\n", (ident1 >> 4) & 0xF);
         printf("QPUs per slice 0x%2.2x\n", (ident1 >> 8) & 0xF);
         printf("TMUs per slice 0x%2.2x\n", (ident1 >> 12) & 0xF);
      }
      else
      {
         printf("Number of slices 0x%2.2x\n", (ident1 >> 4) & 0xF);
         printf("QPUs per slice 0x%2.2x\n", (ident1 >> 8) & 0xF);
         printf("TMUs per slice 0x%2.2x\n", (ident1 >> 12) & 0xF);
         printf("Num semaphores 0x%2.2x\n", (ident1 >> 16) & 0xFF);
         printf("HDR available 0x%2.2x\n", (ident1 >> 24) & 0xF);
         printf("V3D_VPM_MEMSIZE 0x%2.2x\n", (ident1 >> 28) & 0xF);
         printf("V3D_VRI_MEMSIZE 0x%2.2x\n", ident2 & 0xF);
         printf("V3D_TLB_L2SZ 0x%2.2x\n", (ident2 >> 4) & 0xF);
         printf("V3D_TLB_DBLBUF 0x%2.2x\n", (ident2 >> 8) & 0xF);

         defaultBigEndian = ((ident2 >> 20) & 0x1) != 0;
         endianSwappable = ((ident2 >> 21) & 0x1) != 0;

         printf("Default endian %s\n", defaultBigEndian ? "BIG" : "LITTLE");
         printf("Endian swappable %d\n", endianSwappable);
      }

      /* Setup the endian handling (when it's available) */
      if (wantBigEndian)
      {
         if (!defaultBigEndian && !endianSwappable)
         {
            printf(("THIS V3D VARIANT DOES NOT SUPPORT BIG-ENDIAN"));
         }
#ifdef V3D_HAS_ENDIAN_CONTROL
         else if (endianSwappable)
         {
            printf("Requesting BIG-ENDIAN V3D\n");
            carbon_v3d_slavewrite(data->cm, ENDSWP, 1);
         }
#endif
      }
      else
      {
         if (defaultBigEndian && !endianSwappable)
         {
            printf(("THIS V3D VARIANT DOES NOT SUPPORT LITTLE-ENDIAN"));
         }
#ifdef V3D_HAS_ENDIAN_CONTROL
         else if (endianSwappable)
         {
            printf("Requesting LITTLE-ENDIAN V3D\n");
            carbon_v3d_slavewrite(data->cm, ENDSWP, 0);
         }
#endif
      }
   }

   return true;
}

/* Release 3D hardware*/
static bool DriverRelease(void *context, const BEGL_HWReleaseSettings *settings)
{
   return true;
}

/* Issue command to the 3D hardware*/
static void DriverSendCommand(void *context, BEGL_HWCommand command, uint32_t sendData)
{
   CARPL_Hardware *data = (CARPL_Hardware*)context;

   if (data != NULL && data->cm != NULL && command > BEGL_HW_NO_COMMAND && command < BEGL_HW_NUM_COMMANDS)
   {
      if (data->commandMap[command] < 0xFFFF0000)
         carbon_v3d_slavewrite(data->cm, data->commandMap[command], sendData);
      else
      {
         if (data->commandMap[command] == 0xFFFF0000)
         {
            if (sendData == 0x1)
            {
               /* clear caches */
               carbon_v3d_flushcache(data->cm);
            }
         }
         else
         {
            printf("DriverSendCommand - ignoring command %d\n", command);
         }
      }
   }
}

/* Read status from 3D hardware*/
static uint32_t DriverReadStatus(void *context, BEGL_HWCommand status)
{
   CARPL_Hardware *data = (CARPL_Hardware*)context;

   if (data != NULL && data->cm != NULL && status > BEGL_HW_NO_COMMAND && status < BEGL_HW_NUM_COMMANDS)
   {
      if (data->commandMap[status] < 0xFFFF0000)
         return carbon_v3d_slaveread(data->cm, data->commandMap[status]);
      else
      {
         printf("DriverReadStatus - ignoring status read %d\n", status);
         return 0;
      }
   }
   else
      return 0;
}

BEGL_HWInterface *WINPL_CreateHWInterface(void)
{
   BEGL_HWInterface *hw = (BEGL_HWInterface*)malloc(sizeof(BEGL_HWInterface));

   if (hw != NULL)
   {
      CARPL_Hardware *data = (CARPL_Hardware*)malloc(sizeof(CARPL_Hardware));

      if (data != NULL)
      {
         memset(hw, 0, sizeof(BEGL_HWInterface));
         memset(data, 0, sizeof(CARPL_Hardware));
         hw->context = (void *)data;
         hw->Acquire = DriverAcquire;
         hw->Release = DriverRelease;
         hw->SendCommand = DriverSendCommand;
         hw->ReadStatus = DriverReadStatus;

         data->cm = NULL;

         /* Setup the command mappings */
         data->commandMap[BEGL_HW_NO_COMMAND]     = 0;
         data->commandMap[BEGL_HW_L2CACTL]        = L2CACTL;
         data->commandMap[BEGL_HW_SLCACTL]        = SLCACTL;
         data->commandMap[BEGL_HW_INTCTL]         = INTCTL;
         data->commandMap[BEGL_HW_INTENA]         = INTENA;
         data->commandMap[BEGL_HW_INTDIS]         = INTDIS;
         data->commandMap[BEGL_HW_CT0CS]          = CT0CS;
         data->commandMap[BEGL_HW_CT1CS]          = CT1CS;
         data->commandMap[BEGL_HW_CT0EA]          = CT0EA;
         data->commandMap[BEGL_HW_CT1EA]          = CT1EA;
         data->commandMap[BEGL_HW_CT0CA]          = CT0CA;
         data->commandMap[BEGL_HW_CT1CA]          = CT1CA;
         data->commandMap[BEGL_HW_PCS]            = PCS;
         data->commandMap[BEGL_HW_BFC]            = BFC;
         data->commandMap[BEGL_HW_RFC]            = RFC;
         data->commandMap[BEGL_HW_BPOA]           = BPOA;
         data->commandMap[BEGL_HW_BPOS]           = BPOS;
         data->commandMap[BEGL_HW_CACHE_CTRL]     = 0xFFFF0000;
         data->commandMap[BEGL_HW_PCTRC]          = PCTRC;
         data->commandMap[BEGL_HW_PCTRE]          = PCTRE;
         data->commandMap[BEGL_HW_PCTR0]          = PCTR0;
         data->commandMap[BEGL_HW_PCTRS0]         = PCTRS0;
         data->commandMap[BEGL_HW_PCTR1]          = PCTR1;
         data->commandMap[BEGL_HW_PCTRS1]         = PCTRS1;
         data->commandMap[BEGL_HW_PCTR2]          = PCTR2;
         data->commandMap[BEGL_HW_PCTRS2]         = PCTRS2;
         data->commandMap[BEGL_HW_PCTR3]          = PCTR3;
         data->commandMap[BEGL_HW_PCTRS3]         = PCTRS3;
         data->commandMap[BEGL_HW_PCTR4]          = PCTR4;
         data->commandMap[BEGL_HW_PCTRS4]         = PCTRS4;
         data->commandMap[BEGL_HW_PCTR5]          = PCTR5;
         data->commandMap[BEGL_HW_PCTRS5]         = PCTRS5;
         data->commandMap[BEGL_HW_PCTR6]          = PCTR6;
         data->commandMap[BEGL_HW_PCTRS6]         = PCTRS6;
         data->commandMap[BEGL_HW_PCTR7]          = PCTR7;
         data->commandMap[BEGL_HW_PCTRS7]         = PCTRS7;
         data->commandMap[BEGL_HW_PCTR8]          = PCTR8;
         data->commandMap[BEGL_HW_PCTRS8]         = PCTRS8;
         data->commandMap[BEGL_HW_PCTR9]          = PCTR9;
         data->commandMap[BEGL_HW_PCTRS9]         = PCTRS9;
         data->commandMap[BEGL_HW_PCTR10]         = PCTR10;
         data->commandMap[BEGL_HW_PCTRS10]        = PCTRS10;
         data->commandMap[BEGL_HW_PCTR11]         = PCTR11;
         data->commandMap[BEGL_HW_PCTRS11]        = PCTRS11;
         data->commandMap[BEGL_HW_PCTR12]         = PCTR12;
         data->commandMap[BEGL_HW_PCTRS12]        = PCTRS12;
         data->commandMap[BEGL_HW_PCTR13]         = PCTR13;
         data->commandMap[BEGL_HW_PCTRS13]        = PCTRS13;
         data->commandMap[BEGL_HW_PCTR14]         = PCTR14;
         data->commandMap[BEGL_HW_PCTRS14]        = PCTRS14;
         data->commandMap[BEGL_HW_PCTR15]         = PCTR15;
         data->commandMap[BEGL_HW_PCTRS15]        = PCTRS15;
         data->commandMap[BEGL_HW_GCA_PM_SEL]     = 0xFFFF0004;
         data->commandMap[BEGL_HW_GCA_BW_CNT]     = 0xFFFF0008;
         data->commandMap[BEGL_HW_GCA_BW_MEM_CNT] = 0xFFFF000C;
         data->commandMap[BEGL_HW_IDENT0]         = IDENT0;
         data->commandMap[BEGL_HW_IDENT1]         = IDENT1;
         data->commandMap[BEGL_HW_IDENT2]         = IDENT2;
      }
      else
      {
         free(hw);
         hw = NULL;
      }
   }

   return hw;
}

void WINPL_DestroyHWInterface(BEGL_HWInterface *hw)
{
   if (hw)
   {
      CARPL_Hardware *data = (CARPL_Hardware *)hw->context;
      if (data)
      {
         carbon_v3d_close(data->cm);
         free(data);
      }
      memset(hw, 0, sizeof(BEGL_HWInterface));
      free(hw);
   }
}
