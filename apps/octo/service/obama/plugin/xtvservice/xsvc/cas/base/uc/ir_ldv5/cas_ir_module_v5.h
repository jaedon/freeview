#ifndef	__CAS_IR_V5_MODULE_H__
#define	__CAS_IR_V5_MODULE_H__


#include "flash_map.h"
/*
   1) 전체 시스템의 모듈 개수
   2) 각 모듈의 로더에서 바라보는 실제 Address 가 표기되어야 함.
      - reserved 된 mod 의 경우 필요 없다. due of using nvram.
      - App code 영역에 대한 Address 만 존재하면 된다.
*/


#define NVRAM_CAS_IRV5_MPL_FILENAME_PFX		"ir_mod_md_"
#define NVRAM_CAS_IRV5_MIH_FILENAME_PFX		"ir_mod_mih_"
//#define NVRAM_CAS_IRV5_MOD_MASTER_FILE_NAME			"id_mod_master"
#define NVRAM_CAS_IRV5_PS_MODFIED_FLAG_FILE_NAME "ir_ps_modified"
#define NVRAM_CAS_IRV5_LOADER_PT_BACKUP_FILE_NAME NVRAM_CAS_IRV5_MPL_FILENAME_PFX"3_backup"


#define HX_MOD_MASTER_MOD_CNT_OFFSET	(0)
#define HX_MOD_MASTER_MOD_CNT_LEN		(2)
#define HX_MOD_MASTER_MODID_ARRAY_OFFSET		(HX_MOD_MASTER_MOD_CNT_OFFSET + HX_MOD_MASTER_MOD_CNT_LEN )
#define HX_MOD_MASTER_MODID_ARRAY_LEN	(HX_MOD_ID_CNT_MAX * sizeof(unsigned short) )

#define HX_MOD_ID_CNT_MAX	(2)
#define HX_MOD_ID_20_KERNEL (0x0020)
#define HX_MOD_ID_21_ROOTFS (0x0021)

#define HX_MOD_ID_START HX_MOD_ID_20_KERNEL
#define HX_MOD_ID_LAST HX_MOD_ID_21_ROOTFS
#define HX_MOD_TOTAL_COUNT ((HX_MOD_ID_LAST - HX_MOD_ID_START) +1)

#define HX_MOD_ID_20_KERNEL_OFFSET		N_UBI_KERNEL_OFFSET
#define HX_MOD_ID_20_KERNEL_SIZE_MAX	N_UBI_KERNEL_SIZE
#define HX_MOD_ID_21_ROOTFS_OFFSET		N_UBI_ROOT_FS_OFFSET
#define HX_MOD_ID_21_ROOTFS_SIZE_MAX	N_UBI_ROOT_FS_SIZE

typedef struct _hx_ir_v5_mod_master
{
	unsigned short	total_module_count;
	unsigned short	array_module_ids[HX_MOD_ID_CNT_MAX];

}hx_ir_v5_mod_master;


#endif


