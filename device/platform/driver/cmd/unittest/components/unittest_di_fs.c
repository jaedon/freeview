/**
 * unittest_di_fs.c
*/

/**
 * @author		Kevin Woo
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "unittest.h"

#include "di_err.h"
#include "di_uart.h"
#include "di_fs.h"
#include "vkernel.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_FS

#define FS_DEV eDI_FS_DEV_sda
#define FS_PART 0
#define FS_TYPE DI_FS_FAT32_FILESYSTEM

#define DST_MOUNT_DIR "mnt/user"

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

static UT_RESULT ut_ReadDevLabel(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;
	char buf[256];

	result = DI_FS_GetVolumeName(FS_DEV, FS_PART, buf, sizeof(buf) - 1);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Print("devIdx(%d) partIdx(%d) label(%s)\n", FS_DEV, FS_PART, buf);

	ret = UT_OK;

done:
	return ret; 	
}

static UT_RESULT ut_MountDevPartition(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;
	DI_FileSystemType_t eFsType;

	DI_FS_GetFsType(FS_DEV, FS_PART, &eFsType);
	result = DI_FS_MountPartition(FS_DEV,FS_PART,DST_MOUNT_DIR,eFsType);
	DONE_ERR(result != DI_ERR_OK);


	ret = UT_OK;

done:
	return ret; 	
}

static UT_RESULT ut_UnMountDevPartition(void)
{
	DI_ERR_CODE 	result;
	int 			ret = UT_FAIL;
	DI_FileSystemType_t eFsType;

	DI_FS_GetFsType(FS_DEV, FS_PART, &eFsType);
	result = DI_FS_UnmountPartition(FS_DEV,FS_PART,eFsType);

	DONE_ERR(result != DI_ERR_OK);

	ret = UT_OK;

done:
	return ret; 	
}

static UT_MENU_ITEM_T	menu_fs[] = 
{
	MENU_START,
	MENU_FUNC(ut_MountDevPartition, 5),	
	MENU_FUNC(ut_ReadDevLabel, 5), 
	MENU_FUNC(ut_UnMountDevPartition, 5),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_fs[] = 
{	
	MENU_SUB(menu_fs),
};

void UT_FS_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_fs)/sizeof(menu_main_fs[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_fs[i]);
	}
	array_size = sizeof(menu_fs)/sizeof(menu_fs[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_fs[i]);
	}
}



