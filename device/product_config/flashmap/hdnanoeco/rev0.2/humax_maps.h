#ifndef __HUMAX_MAPS_H__
#define __HUMAX_MAPS_H__

/* only change the default size value */
#define DEFAULT_SIZE_MB 		4

#define HUMAX_CFE_OFFSET		0x00000000
#define HUMAX_RESERVE_OFFSET	0x00200000

#define HUMAX_TOTAL_SIZE		0x00400000
#define HUMAX_CFE_SIZE			0x00200000
#define HUMAX_RESERVE_SIZE		0x00200000

static struct mtd_partition fixed_nor_partition_map[] = 
{
	{ name: "cfe",			offset: (HUMAX_CFE_OFFSET), 				size: (HUMAX_CFE_SIZE) },
	{ name: "reserve0",		offset: (HUMAX_RESERVE_OFFSET), 			size: (HUMAX_RESERVE_SIZE) },
	{ name: "all",			offset: (HUMAX_CFE_OFFSET), 				size: (HUMAX_TOTAL_SIZE) }
};

#endif