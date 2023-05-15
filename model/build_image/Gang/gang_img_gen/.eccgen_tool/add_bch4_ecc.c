#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> 


#define ERASE_UNUSED_UBI_BLOCK


#define BCH_LEVEL		4
#define SUB_PAGE_SIZE	512
#define OOB_SIZE		16
#define SUB_PAGE_OOB	(SUB_PAGE_SIZE+OOB_SIZE+OOB_SIZE)
#define NUM_SUB_PAGE	4

#define FIELD_ORDER		14	/* 65nm NAND controller 13, 40nm NAND congtroller 14 */
#define PAGE_SIZE		(SUB_PAGE_SIZE*NUM_SUB_PAGE)	/* 2KB */
#define BLOCK_SIZE		(128*1024)						/* 128KB */
#define RESERVED_SIZE	(2*1024*1024)					/* Reserve size for BBT */
#define NUM_PAGE		(BLOCK_SIZE/PAGE_SIZE)

/* Volume identifier header magic number (ASCII "UBI!") */
#define UBI_VID_HDR_MAGIC 		0x21494255
#define UBI_VID_HEADER_SIZE		64

#define MAX_FILE_LENGTH	256
#define MAX_FILE_NUM	64

#define TRUE			1
#define FALSE			0

#define SWAP32( a )  do{a=((a << 24) | ((a << 8) &  0xFF0000) | ((a >> 8) &  0xFF00) |(a >> 24));}while(0)

int  p_bch_level = BCH_LEVEL;				/*BCH_4*/
int  p_oob_size = OOB_SIZE;					/* OOB size 16*/
int  p_partial_page_size = SUB_PAGE_SIZE;	/*512 bytes partial page size*/
int  p_field_order = FIELD_ORDER;			/* Order of the finite field for BCH encoding */

int  gx13_4[13*4+1] =  {1,1,0,1,0,1,0,1,0,1,1,0,0,0,0,1,1,1,0,1,0,1,0,1,1,1,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,1,0,1,0,0,0,1,0,1};
int  gx14_4[14*4+1] =  {1,0,1,0,1,1,1,0,1,1,0,1,1,0,1,0,1,1,1,0,1,1,0,0,0,1,1,0,0,0,0,0,0,1,0,1,1,1,0,0,1,1,0,1,0,0,0,1,1,0,0,1,1,1,0,1,1};

int  num_file = 0; 	/* total number of file to make master image */
int  total_size;
int  total_page;
int  current_page;


typedef struct _file_info
{
	unsigned char FileName[MAX_FILE_LENGTH];
	unsigned int Offset;
	unsigned int fCleanup;
} FILE_INFO, *pFILE_INFO;

typedef struct _partition_info
{
	unsigned long StartBlock;
	unsigned long EndBlock;
	unsigned long UsedBlock;
} PARTITION_INFO, *pPARTITION_INFO;


/* UBI VID Header structure */
struct ubi_vid_hdr {
	unsigned long  magic;
	unsigned char  version;
	unsigned char  vol_type;
	unsigned char  copy_flag;
	unsigned char  compat;
	unsigned long  vol_id;
	unsigned long  lnum;
	unsigned char  padding1[4];
	unsigned long  data_size;
	unsigned long  used_ebs;
	unsigned long  data_pad;
	unsigned long  data_crc;
	unsigned char  padding2[4];
	long long  		sqnum;
	unsigned char   padding3[12];
	unsigned long  	hdr_crc;
};


FILE_INFO gFileInfo[MAX_FILE_NUM];
PARTITION_INFO gPartitionInfo[MAX_FILE_NUM];

static unsigned char gOnePageWihtOOB[ (SUB_PAGE_OOB*NUM_SUB_PAGE) ];	/* one page with OOB */
static unsigned char gOneBlockWihtOOB[ (SUB_PAGE_OOB*NUM_SUB_PAGE)*NUM_PAGE  ];	/* one block with OOB */

static const unsigned long crc32_table[256] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL
};

unsigned long mtd_crc32(unsigned long val, const void *ss, int len)
{
	const unsigned char *s = ss;


	while (--len >= 0)
		val = crc32_table[(val ^ *s++) & 0xff] ^ (val >> 8);
	return val;
}

static int check_ubi_vid_header(unsigned char *data)
{
	int i, valid_flag = FALSE;
	unsigned long buff[UBI_VID_HEADER_SIZE];
	struct ubi_vid_hdr *vid_hdr;


	memcpy((void *)&buff, (void *)data, UBI_VID_HEADER_SIZE);
	
	for (i=0; i<(UBI_VID_HEADER_SIZE/4); i++)
	{
	    SWAP32(buff[i]);
	}	
	vid_hdr = (struct ubi_vid_hdr *)buff;

	/* check magic number */
	if( vid_hdr->magic == UBI_VID_HDR_MAGIC )
	{
		unsigned long crc = -1;
		
		SWAP32(vid_hdr->hdr_crc);
		crc = mtd_crc32(crc, (unsigned char *)vid_hdr, UBI_VID_HEADER_SIZE-4);

		if( crc == vid_hdr->hdr_crc )
		{
			valid_flag =  TRUE;
		}
	}

	return valid_flag;
}

static int check_empty_block(unsigned long *data, unsigned long size)
{
	int i;
	unsigned long result_data = 0xFFFFFFFF;

	
	for(i=0; i<(size/4); i++)
	{
		result_data &= *data++;
	}

	if( result_data == 0xFFFFFFFF )
		return TRUE;
	else
		return FALSE;
}

static int check_empty_page(unsigned long *data)
{
	return check_empty_block(data, PAGE_SIZE);
}

static void nand_calculate_ecc_BRCM_bch(unsigned char *data,unsigned char *ecc_code,int m,int n,int k,int t,int u,int v)
{
	int  i;
	int  j;
	int  r;
	int  ff;
	int  msg[1 << FIELD_ORDER]; /* LSB first */
	int  cw[1 << FIELD_ORDER];  /* LSB first */
	int  *gx;       /* LSB first */

	/*------------------------------------------------------------------------------- */
	/* Construct msg in bit (LSB first) */
	/*------------------------------------------------------------------------------- */
	for (i=k-1;i>=0;i--)
		msg[i] = 0;

	r = 0;
	for (i=0;i<u;i++)
	{
		for (j=7;j>=0;j--)
		{
			msg[k-1-r] = (data[i]>>j)&0x01;
			r++;
		}
	}

	/*------------------------------------------------------------------------------- */
	/* Initialize generator polynomial */
	/*------------------------------------------------------------------------------- */    
#if (FIELD_ORDER == 13)
	gx = gx13_4;
#else
	gx = gx14_4;
#endif

	/*------------------------------------------------------------------------------- */
	/* Encoding */
	/*------------------------------------------------------------------------------- */
	for (i=0;i<n;i++)
	{
		if (i<n-k)
			cw[i] = 0;
		else
			cw[i] = msg[i-(n-k)];
	}

	for (i=k-1;i>=0;i--)
	{
		ff = cw[n-k-1]^msg[i];
		for (j=n-k-1;j>=1;j--)
			cw[j] = cw[j-1]^(ff&gx[j]);
		cw[0] = ff&gx[0];
	}

	for (i=0;i<v;i++)
	{
#if 0	
		ecc_code[i] = 128*cw[8*(v-i)-1] + 64*cw[8*(v-i)-2] + 32*cw[8*(v-i)-3] + 16*cw[8*(v-i)-4]+
			8*cw[8*(v-i)-5] +  4*cw[8*(v-i)-6] +  2*cw[8*(v-i)-7] +  1*cw[8*(v-i)-8];
#else
		ecc_code[i] = (cw[8*(v-i)-1]<<7) + (cw[8*(v-i)-2]<<6) + (cw[8*(v-i)-3]<<5) + (cw[8*(v-i)-4]<<4) +
			(cw[8*(v-i)-5]<<3) +  (cw[8*(v-i)-6]<<2) +  (cw[8*(v-i)-7]<<1) +  (cw[8*(v-i)-8]);
#endif
	}

}

int getECC(unsigned char *data, unsigned char *oob)
{
	int       s,n,k,p,t,u,v,i,m;   
	unsigned char   ecc_code[70];     
	unsigned long   *swap_data;
	 
	/* Finite field order */
	m = p_field_order;  
	/* Error correction capability (in bits). */
	t = p_bch_level;  
	/* Number of spare bytes per partial page. */
	s = p_oob_size;
	/* Codeword length (in bits) */
	n = (p_partial_page_size + s) * 8;
	/* Parity length (in bits). */
	p = m * t;
	/* Message length (in bits). */
	k = n - p;
	/* Number of data bytes = floor(k/8) */
	u = k / 8;
	/* Number of ecc_code bytes, v = ceil(p/8) = ceil(m*t/8) = p_partial_page_size+s-u */
	v = p_partial_page_size + s - u;
	
	swap_data = (unsigned long *)data;
	for (i=0; i<p_partial_page_size/4; i++)
	{
	    SWAP32(*swap_data);
		swap_data ++;
	}

	nand_calculate_ecc_BRCM_bch(data,ecc_code,m,n,k,t,u,v);	

	for(i=0; i<v; i++) {
	    oob[9+i] = ecc_code[i];
	}

	return 0;
}


int update_partition_info(int index)
{
	struct stat sb;
	

	/* Check file size */
	if( stat(gFileInfo[index].FileName, &sb) != 0) 
	{
		printf("Can get file info!\n");
		return -1;	
	}

	/* Get start block */
	gPartitionInfo[index].StartBlock = gFileInfo[index].Offset / NUM_PAGE;
	
	/* Get last block */
	if( index < (num_file-1) )
	{
		gPartitionInfo[index].EndBlock = (gFileInfo[index+1].Offset / NUM_PAGE) -1;
	}
	else
	{
		gPartitionInfo[index].EndBlock = ( (total_page - (RESERVED_SIZE/PAGE_SIZE)) / NUM_PAGE ) -1;
	}

	/* Get Used block */
	gPartitionInfo[index].UsedBlock = (sb.st_size+(BLOCK_SIZE-1)) / BLOCK_SIZE; /* align block size */
	
	printf(" Start Block %lu, End block %lu, Used Block %lu \n", 
		gPartitionInfo[index].StartBlock, gPartitionInfo[index].EndBlock, gPartitionInfo[index].UsedBlock);

	return 0;
}


int perfom_one_file(int index, FILE *output_file)
{
	FILE *pIn;
	struct stat sb;
	long size;
	int i, is_last=FALSE, result = 0;
	static unsigned char subpage_data[SUB_PAGE_OOB];
	static unsigned char oob_data[NUM_SUB_PAGE*2][OOB_SIZE];				/* oob data per one page */

	
	printf(" %dth <%s> offset page %u (cleanupMode=%s)\n", index, gFileInfo[index].FileName, gFileInfo[index].Offset, (gFileInfo[index].fCleanup==1?"ON":"OFF"));

	if( update_partition_info(index) != 0 )
	{
		printf("Can get partition info!\n");
		return -1;	
	}
	
	/* Open File */
	pIn = fopen(gFileInfo[index].FileName, "rb");
	if( pIn == 0 )
	{
		printf("Can not open file!\n");
		return -1;	
	}

	/* Check current page is overwite or not */
	if( current_page > gFileInfo[index].Offset )
	{
		printf("%dth (%s) file is overwite page\n", index, gFileInfo[index].FileName);
		result = -1;
		goto done;
	}
	
	/* Add dummy page */
	memset((void*)gOnePageWihtOOB, 0xff, sizeof(gOnePageWihtOOB));
	for(i=current_page; i<gFileInfo[index].Offset; i++)
	{
		fwrite(gOnePageWihtOOB, 1, sizeof(gOnePageWihtOOB), output_file);
	}

	/* update current page */
	current_page = gFileInfo[index].Offset;

	while(is_last == FALSE)
	{
		memset(gOnePageWihtOOB, 0xff, sizeof(gOnePageWihtOOB));
		memset((void*)oob_data, 0xff, sizeof(oob_data));

		if( (current_page%NUM_PAGE) == 0 )
		{
			memset(gOneBlockWihtOOB, 0xff, sizeof(gOneBlockWihtOOB));
		}

		/* read One Page */
		size = fread(gOnePageWihtOOB, 1, PAGE_SIZE, pIn);
		if( size != PAGE_SIZE)
		{
			/* EOF */
			if( size > 0 )
			{
				printf("%dth (%s) file is not aligned page(%ld/%u)\n", index, gFileInfo[index].FileName,
					size,PAGE_SIZE);
			}
			is_last = TRUE;
		}

		/* Check empty data */
		if( check_empty_page((unsigned long *)gOnePageWihtOOB) != TRUE )
		{
			for(i=0; i<NUM_SUB_PAGE; i++)
			{
				memcpy(subpage_data, &gOnePageWihtOOB[i*SUB_PAGE_SIZE], SUB_PAGE_SIZE);
				memset(&subpage_data[SUB_PAGE_SIZE], 0xff, OOB_SIZE);
				getECC(subpage_data, oob_data[i]);
				memcpy(&gOnePageWihtOOB[i*SUB_PAGE_SIZE], subpage_data, SUB_PAGE_SIZE);
			}
		}

		/* update OOB */
		memcpy((void*)&gOnePageWihtOOB[NUM_SUB_PAGE*SUB_PAGE_SIZE] , (void*)oob_data, sizeof(oob_data));

		/* copy Page with OOB to bloc */
		memcpy((void*)&gOneBlockWihtOOB[(current_page%NUM_PAGE)*sizeof(gOnePageWihtOOB)] , (void*)gOnePageWihtOOB, sizeof(gOnePageWihtOOB));

		if( ((current_page%NUM_PAGE) == (NUM_PAGE-1)) || (is_last==TRUE) )
		{
			int is_ubi_empty = FALSE;
			unsigned long data_offset = 2*(SUB_PAGE_OOB*NUM_SUB_PAGE); /* EC header page + VID header page */

#ifdef ERASE_UNUSED_UBI_BLOCK
			/* Check UBI VID header & empty block */
			if(gFileInfo[index].fCleanup == 1)
			{
				if( (check_ubi_vid_header(&gOneBlockWihtOOB[(SUB_PAGE_OOB*NUM_SUB_PAGE)])==TRUE) && 
					(check_empty_block((unsigned long *)&gOneBlockWihtOOB[data_offset], sizeof(gOneBlockWihtOOB)-data_offset)==TRUE) )
				{
					is_ubi_empty = TRUE;
					memset(gOneBlockWihtOOB, 0xff, sizeof(gOneBlockWihtOOB));
	 			}
			}
#endif
			printf(" %lu/%lu %s\n", (current_page/NUM_PAGE)-gPartitionInfo[index].StartBlock, gPartitionInfo[index].UsedBlock,
					(is_ubi_empty?"is UBI empty block":""));

			/* write one block or remained data */
			size = (is_last==FALSE) ? sizeof(gOneBlockWihtOOB) : ((current_page%NUM_PAGE)+1)*sizeof(gOnePageWihtOOB);
			fwrite(gOneBlockWihtOOB, 1, size, output_file);
		}	

		current_page ++;
	}

done:
	printf("\n perform end (%s) \n\n\n", gFileInfo[index].FileName);
	
	fclose(pIn);

	return result;
}


int parse_config(char *config_file)
{
	FILE	*config_p;
	char 	cmd_string[MAX_FILE_LENGTH+16], tmp_string[MAX_FILE_LENGTH];
	
	/* Open File */
	config_p = fopen(config_file, "rt");
	if( config_p == 0 )
	{
		printf("Can not open file!\n");
		return -1;	
	}

	num_file = 0;
	while(1)
	{
		if( fgets(cmd_string, sizeof(cmd_string), config_p) == NULL )
			break;

		/* Check comment line */
		if(cmd_string[0] == '#')
			continue;

		/* get filename & offset */
		sscanf(cmd_string, "%s %s %x %x", gFileInfo[num_file].FileName, tmp_string, &gFileInfo[num_file].Offset, &gFileInfo[num_file].fCleanup);

		/* convert to page number */
		gFileInfo[num_file].Offset /= PAGE_SIZE;
		
		num_file ++;
	}

	fclose(config_p);
	
	return 0;
}


int main(int argc, char **argv)
{
	int i;
	FILE *p_output, *p_partition;

	
	/* Clear File Info & Partition Info */
	memset((void *)gFileInfo, 0x00, sizeof(gFileInfo));
	memset((void *)gPartitionInfo, 0x00, sizeof(gPartitionInfo));

	if(argc != 5)
	{
		printf("usage : %s [config file] [nand size(MB)] [master file] [partition file]\n", argv[0]);
		return -1;	
	}

	/* Get Filename & offset */
	if( parse_config( argv[1] ) != 0 )
	{
		printf("can't parse config file!!\n");
		return -1;	
	}

	/* Get NAND Size */
	sscanf( argv[2], "%d", &total_size);
	total_size = total_size * (1024*1024);
	total_page = total_size / PAGE_SIZE;

	/* create output file */
	p_output = fopen(argv[3], "wb");
	if( p_output == 0 )
	{
		printf("Can create file!\n");
		return -1;	
	}

	/* create partition table file */
	p_partition = fopen(argv[4], "wt");
	if( p_partition == 0 )
	{
		printf("Can create file!\n");
		return -1;	
	}
	
	printf("NAND Size %dMB (%d pages), merge %d files \n", total_size/(1024*1024), total_page, num_file);

	current_page = 0;
	
	for(i=0; i<num_file; i++)
	{
		if( perfom_one_file(i, p_output) != 0)
		{
			printf("can't make master file!\n");
			goto done;
		}
	}

	/* Add dummy page */
	memset((void*)gOnePageWihtOOB, 0xff, sizeof(gOnePageWihtOOB));
	for(i=current_page; i<total_page; i++)
	{
		fwrite(gOnePageWihtOOB, 1, sizeof(gOnePageWihtOOB), p_output);
	}

	/* write partition table */
	for(i=0; i<num_file; i++)
	{
		fprintf(p_partition, "%lu;%lu;%lu\n", 
			gPartitionInfo[i].StartBlock, gPartitionInfo[i].EndBlock, gPartitionInfo[i].UsedBlock);
	}

done:

	fclose(p_output);
	fclose(p_partition);

	return 0;
}

