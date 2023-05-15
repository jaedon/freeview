#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <dlfcn.h>

#include <htype.h>
#include <vkernel.h>
#include <dlfcn.h>
#include "_memory_debug_int.h"

/*



Âü°í site.
http://yosefk.com/blog/getting-the-call-stack-without-a-frame-pointer.html








*/

#define	_____memdbg_type______________________________________________________________

#define	_____memdbg_private______________________________________________________________
static int memdbg_mips_getPreviousSpRa (void** prev_sp, void** prev_ra, void* sp, void* ra)
{
	unsigned* wra = (unsigned*)ra;
	int spofft;

	/* scan towards the beginning of the function -
	 addui sp,sp,spofft should be the first command */
	while((*wra >> 16) != 0x27bd)
	{
		/* test for "scanned too much" elided */
		wra--;
	}

	spofft = ((int)*wra << 16) >> 16; /* sign-extend */
	*prev_sp = (char*)sp - spofft;
	/* now scan forward for sw r31,raofft(sp) */
	while(wra < (unsigned*)ra)
	{
		if((*wra >> 16) == 0xafbf)
		{
			int raofft = ((int)*wra << 16) >> 16; /* sign */

			*prev_ra = *(void**)((char*)sp + raofft);
			return 1;
		}

		wra++;
	}

	return 0; /* failed to find where ra is saved */
}



static const char HexTable[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

static void memdbg_mips_util_unsignedlong_to_hex(unsigned long val, char *hex)
{
    if (NULL == hex) return;

    hex[7] = HexTable[val&0xF];    val >>= 4;
    hex[6] = HexTable[val&0xF];    val >>= 4;
    hex[5] = HexTable[val&0xF];    val >>= 4;
    hex[4] = HexTable[val&0xF];    val >>= 4;
    hex[3] = HexTable[val&0xF];    val >>= 4;
    hex[2] = HexTable[val&0xF];    val >>= 4;
    hex[1] = HexTable[val&0xF];    val >>= 4;
    hex[0] = HexTable[val&0xF];
}

static int memdbg_mips_util_check_string(const char * str, int range)
{
	int i;
	if(NULL == str)
		return -1;

	for(i = 0 ; i< range; i++){
		if( '\0' == str[i]  )
			return i;
	}
	return -1;
}

static char * memdbg_mips_util_paste_limitedstring(char * dst, const char * str, int range)
{
	int len;

	len = memdbg_mips_util_check_string(str, range);
	if(0 > len){
		strcpy(dst, "unknown");
		len = 7;
	}
	else{//found it.
		strcpy( dst, str);
	}
	return (char *)(dst + len);
}


#define	_____memdbg_members______________________________________________________________
int memdbg_mips_getCallStack (void **retaddrs, int max_size)
{
	void		*sp, *ra;
	/* adjust sp by the offset by which this function
     	has just decremented it */
	int			*funcbase = (int*)(int)&memdbg_mips_getCallStack;
	/* funcbase points to an addiu sp,sp,spofft command */
	int			spofft = (*(funcbase + 3) << 16) >> 16; /* 16 LSBs */
	int			i = 0;

	  /* read sp and ra registers */
	__asm__("move %0, $29"
	:"=r"(sp)
	:
	);

	__asm__("move %0, $31"
	:"=r"(ra)
	:
	);

	sp = (char*)sp-spofft;

	do {
	    if (i < max_size)
		{
			retaddrs[i++] = ra;
		}
	} while (memdbg_mips_getPreviousSpRa(&sp,&ra,sp,ra));

	return i; /* stack size */
}



#define WORD_WIDTH (160) /* it was adjusted due of dli/sli name.(128 -> 17 + 64 + 15+  48 + 15 = approx.160.)*/
static char s_outbuf[WORD_WIDTH];

/*
Dl_info,



char  dli_fname [PATH_MAX]
  Filename of defining object.


 void *  dli_fbase
  Load address of that object.


 const char *  dli_sname
  Name of nearest lower symbol.


 void *  dli_saddr
  Exact value of nearest symbol.

*/
int memdbg_mips_backtrace_symbols ( void *const *array, int size, MemPrintCb_t pfPrintCb)
{
	int cnt = 0;
	Dl_info info;
	char *p;

	if( size < 1 )
	{
		memset(s_outbuf, 0, sizeof(s_outbuf));
		p = s_outbuf;
		strcpy(p, "Nothing\n");
		pfPrintCb("- Frame (%02d) : ptr(0x%08x) with %s\n", cnt, array[cnt], s_outbuf);
	}
	else
	{
	        for (cnt = 0; cnt < size; ++cnt)
		{
			memset(s_outbuf, 0, sizeof(s_outbuf));
			p = s_outbuf;


			if(!dladdr(array[cnt], &info))
			{
				/* printf("%s -\n", syms[i]); */
				p = memdbg_mips_util_paste_limitedstring(p, array[cnt], 16); //approx 14.
				strcpy(p, " -"); p+=2;
			}
			else
			{
				//Lib relocated addr.
				p = memdbg_mips_util_paste_limitedstring(p, array[cnt], 16); //approx 14.
				strcpy(p, " - ");
				p+=3;
				//Lib name.
				p = memdbg_mips_util_paste_limitedstring(p, info.dli_fname, 64);
				*p = ':';
				++p;

				//Func offset from Lib.base
				strcpy(p, " [0x");
				p += 4; /* 4 == strlen(" [0x") */
				memdbg_mips_util_unsignedlong_to_hex((char *)array[cnt]-(char *)info.dli_fbase, p); p += 8;
				*p = ']';
				p++;
				*p = '/';
				p++;

				//Func name if matched offset.
				p = memdbg_mips_util_paste_limitedstring(p, info.dli_sname, 48);
				*p = ':';
				++p;

				strcpy(p, " [0x");
				p += 4; /* 4 == strlen(" [0x") */
				memdbg_mips_util_unsignedlong_to_hex((char *)array[cnt]-(char *)info.dli_saddr, p);
				p += 8;

				*p = ']';
				p++;

			}

			pfPrintCb("- Frame (%02d) : ptr(0x%08x) with %s \n", cnt, array[cnt], s_outbuf);
		}
	}

	return 0;
}




