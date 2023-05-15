#include <stdio.h>
#include <string.h>

#define	MAX_SIZE	(8*1024)

#define	SYNC_BYTE		0x47

unsigned char ts_data[188];
unsigned long	section_cur_index = 0;
long	section_remain = 0;
long	section_length = 0;

FILE	*psi_fp;

void send_section(unsigned char *section, unsigned long size)
{
	int i;

	//printf("send SECTION. size %d\n", size);

	for(i=0; i<size; i++)
	{
		fputc(section[i], psi_fp);
	}

}

int	extract_psi(unsigned char *ts_pointer, int extract_pid)
{
	int				size, ts_index;
	unsigned short	pid;
	char	first_section_pk;

	/* First byte must be 0x47 */
	if( ts_pointer[0] != SYNC_BYTE )
	{
		printf("ERROR : First byte is 0x%02X. \n", ts_pointer[0]);
		return -1;
	}

	/* compare PID */
	pid = (ts_pointer[1]&0x1F)<<8 | ts_pointer[2];
	if( pid != extract_pid )
	{
	//	printf("WARRING : not match PID (0x%02X-0x%02X)\n", pid, extract_pid);
		return 0;
	}

	/* don't care adaptation field */
	/* check first byte or not */
	if( ts_pointer[1] & 0x40 )
	{
		ts_index = 5;
		size = 188 - ts_index;
		section_cur_index = 0;
		section_length = (((ts_pointer[6]&0x0F)<<8) | ts_pointer[7]) + 3;
		section_remain = section_length;
		//printf("<extract_psi> size %d\n", section_length);
	}
	else
	{
		ts_index = 4;
		size = 188 - ts_index;
	}

	size = (section_remain>size) ? size : section_remain;
	send_section(&ts_pointer[ts_index], size);
		
	section_cur_index += size;
	section_remain -= size;

	return 0;
}


int main(int argc, char *argv[])
{
	FILE			*inp;
	int			size, error = 0;
	int			byte = 0;
	int			pid;

	if(argc != 4)
	{
		printf("usage : TS2PSI [pid] [input] [output]\n");
		return -1;
	}

	sscanf(argv[1], "%d", &pid);
	
	inp = fopen(argv[2], "rb");
	psi_fp = fopen(argv[3], "wb");

	if( (inp==0) || (psi_fp==0) )
	{
		printf("Can not open or create file!\n");
		return -1;
	}

	printf("======== Information =============\n");
	printf(" PID      : %d \n", pid);
	printf(" TS File  : %s\n", argv[2]);
	printf(" SEC File : %s\n", argv[3]);
	printf("==================================\n");
	
	// get PES data
	size = 0;
	while(byte != EOF)
	{
		byte = fgetc(inp);
		ts_data[size++] =  (unsigned char)byte&0xFF;

		if( size==188)
		{
			size = 0;
			error = extract_psi(ts_data, pid);
			if( error < 0 )
				break;
		}
	}

	if( error < 0)
	{
		printf("Input TS is not correct!\n");
	}

	printf(" Conversion complete!!\n");

	fclose(inp);
	fclose(psi_fp);

	return 0;
}
