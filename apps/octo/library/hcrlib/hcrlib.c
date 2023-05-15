/* This module describes logging sys context for tracing crash  with based on specific arch context type referencing toolcahins. */
/* This contents got a validation only for " ABIO32".  then you shall have it may inaccuracy upon other mips types. merong~ */
/* Followings decribes the system users level context you may see it below chains.
typedef struct ucontext
{
	unsigned long int uc_flags;
	struct ucontext *uc_link;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	__sigset_t uc_sigmask;
} ucontext_t;  */

/// * Do avoid to have dependecy with the HLib *///
#include <sys/types.h> // Some cond. statements included features.h declare un-define __USE_GNU, then you shall check it in Make.
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/mount.h>
#include <dirent.h>

#include <errno.h>

#include "ucontext.h"

#include <fcntl.h>
#include <dlfcn.h>

#include <hlib.h>

/* Local switches. */
//#define DUMP_STACKS
//#define RUNTIME_TRACE //just for debug.
//#define CRASH_TEST //just for debug

/* Default : stdout, if it declared, use only logging file. */
/* --> default : file out, optionally uart out, it generates only a copied proc map file. */
//#define HCR_LOGGING_ON_TTY


#if defined(CONFIG_HUMAX_CRASHREPORT_TRANSFER_USB)
#define HCR_TRANSFER_USB
#endif
/* After logging on event, trasfer somewhere --> it removes implemented for these*/
//#define HCR_TRANSFER_HTTP
//#define HCR_TRANSFER_FTP


/* Declarations */
/* 	REV  0.1.0 : partial stack dump
	REV  0.2.0 : by cases dump a stack file
	REV  0.2.1 : add 'fdatasync' to protect og-file operation */
#define HCR_REV	"REV 0.2.1"
#define MAGIC_SIG1 (0x00D10000)
#define INVALID_REF_CNT (10001) /* if a logfile name got a num in invalid range, we can assume ther were cond. problem on file operation */
#define	BOUND_CNT 			(10) /* a modular number to remove cumulated log */
#define	HCR_FULLPATH_LENGTH	(64)

#define HCR_LOG_PATH CONFIG_PRODUCT_USERDATA_DIR"/hcr"
#define HCR_LOG_PREFIX "hcr_" //must be 4 char.

#define HCR_DATE __DATE__
#define HCR_TIME __TIME__

#define TEMP_LOG_FILE HCR_LOG_PATH"/temp.log."
#define TEMP_MAP_FILE HCR_LOG_PATH"/temp.map."
#define REF_CNT_FILE HCR_LOG_PATH"/ref.cnt"
#define VFF_FILE HCR_LOG_PATH"/vff.txt" //record logged ref_cnt

#define DEFAULT_STACK_DUMP_SIZE_UP (0x00040000) //256k
#define DEFAULT_STACK_DUMP_SIZE_DOWN (0x00040000) //256k

#define DEFAULT_STACK_TOP (0x7FFFFFFC) //nae mom.. kk
#define DEFAULT_STACK_BOT (0x10000000)
#define DEFAUT_SEG_START  (0x00400000) // i wanna believe it.

#define TCBUFSIZE (20) //trace operation buf.size
#define LOG_LINE_LEN (60)

/* rule#1 - try to keep a line inside 128 characters
 * rule#2 - try construct one line at a time and reduce inst. */
#define OUTBUF_MAX (160 ) /* it was adjusted due of dli/sli name.(128 -> 17 + 64 + 15+  48 + 15 = approx.160.)*/

#define abs(s) ((s) < 0 ? -(s) : (s))

static char s_outbuf[OUTBUF_MAX];
static const char HexTable[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};
static const char DecTable[10] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};


/* Do not use semaphores before trumbling, just use bss <-> data session case by case.
bss -> data,  2014-01-10 */
static	int s_fd_log = -1;
static 	int s_refcnt = 0;
static 	int s_virgin = 1;

/* Do not wrap belows into a structure type */
static pid_t	s_proc_pid;
static char		s_proc_pid_str[8] = {0,};
static int		s_proc_pid_strlen = 0;
static char		s_proc_name[32] = {0,};
//static char 	s_proc_path[32] ={ 0, };
static char 	s_proc_path[64] ={ 0, };	// prevent #182802

/* Use Data and BSS sections as possible than Stacks */
static char s_log_file_name[32] ={ 0, };
static char s_map_file_name[32] ={ 0, };
static char s_tmp_fullpath_log[HCR_FULLPATH_LENGTH] ={ 0, };
static char s_tmp_fullpath_map[HCR_FULLPATH_LENGTH] ={ 0, };
static char s_stack_fullpath[HCR_FULLPATH_LENGTH] ={ 0, };

static	unsigned int s_proc_stack_bot; /*  to .bss */
static	unsigned int s_proc_stack_top; /*  to .bss */

/* Use it to test hcr under release mode software */
static void hcr_f_print(const char *format,...);
static char *hcr_StrSigCode(int sig, int code);

/* user signal handler */
static void		(*s_pfnSignalUserHandler)(int sig, siginfo_t *sig_info, void *context);

#if defined(CONFIG_DEBUG)
#define DbgPrt printf
#define DbgTrc //printf("hcr:trc:[%d][%s]:%s, %d\n", s_proc_pid, s_proc_name, __FILE__, __LINE__ )
#define DbgTrcl //printf("hcr:l[%d][%s]:%s, %d\n", s_proc_pid, s_proc_name, __FILE__, __LINE__ )
#else
//#define HCR_F_PRINT
	#ifdef HCR_F_PRINT
	#define DbgPrt hcr_f_print
	#define DbgTrc hcr_f_print("hcr:f:[%d][%s]:%s, %d\n", s_proc_pid, s_proc_name, __FILE__, __LINE__ )
	#define DbgTrcl hcr_f_print("hcr:fl:[%d][%s]:%s, %d\n", s_proc_pid, s_proc_name, __FILE__, __LINE__ )
	#else //for testing it in release image.
	#define DbgPrt(...)
	#define DbgTrc
	#define DbgTrcl
	#endif
#endif

#ifdef HCR_F_PRINT
static void hcr_f_print(const char *format,...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
#endif




/* hcr_util_ul2hex -- converts unsigned long (32 bits) into 8 digit hex-decimal string
 *
 * Parameters:
 *  val [in] 32 bits unsigned long value to be converted into human readable
 *  hex [out] buffer to hold text converted format
 * similar to itoa ( hex)
 */
static void hcr_util_ul2hex(unsigned long val, char *hex)
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

/* similar to itoa (dec)
*/
static char * hcr_util_ul2dec(unsigned long num, char *str)
{
    int i=0;
    int radix = 10;
    int deg=1;
    int cnt = 0; // za rit su.

    while(1){
        if( (num/deg) > 0)
            cnt++;
        else
            break;
        deg *= radix;
    }
    deg /=radix;

    for(i=0; i<cnt; i++)    {
        *(str+i) = num/deg + '0';
        num -= ((num/deg) * deg);
        deg /=radix;
    }
    *(str+i) = '\0';

	return str;
}


static char *hcr_StrSigCode(int sig, int code)
{
	switch(sig)
	{
		case SIGILL:
			switch(code) {
				case ILL_ILLOPC:			return "ILL_ILLOPC\n";		//return "Illegal opcode\n";
				case ILL_ILLOPN:			return "ILL_ILLOPN\n";		//return "Illegal operand\n";
				case ILL_ILLADR:			return "ILL_ILLADR\n";		//return "Illegal addressing mode\n";
				case ILL_ILLTRP:			return "ILL_ILLTRP\n";		//return "Illegal trap\n";
				case ILL_PRVOPC:			return "ILL_PRVOPC\n";		//return "Privileged opcode\n";
				case ILL_PRVREG:			return "ILL_PRVREG\n";		//return "Privileged register\n";
				case ILL_COPROC:			return "ILL_COPROC\n";		//return "Coprocessor error\n";
				case ILL_BADSTK:			return "ILL_BADSTK\n";		//return "Internal stack error\n";
				default:					return "SIGILL:-\n";
			}

		case SIGFPE:
			switch(code) {
				case FPE_INTDIV:			return "FPE_INTDIV\n";		// return "Integer divide-by-zero\n";
				case FPE_INTOVF:			return "FPE_INTOVF\n";		// return "Integer overflow\n";
				case FPE_FLTDIV:			return "FPE_FLTDIV\n";		// return "Floating point divide-by-zero\n";
				case FPE_FLTOVF:			return "FPE_FLTOVF\n";		// return "Floating point overflow.\n";
				case FPE_FLTUND:			return "FPE_FLTUND\n";		// return "Floating point underflow\n";
				case FPE_FLTRES:			return "FPE_FLTRES\n";		// return "Floating point inexact result\n";
				case FPE_FLTINV:			return "FPE_FLTINV\n";		// return "Invalid floating point operation\n";
				case FPE_FLTSUB:			return "FPE_FLTSUB\n";		// return "Subscript out of range\n";
				default:					return "SIGFPE:-\n";

			}

		case SIGSEGV:
			switch(code) {
				case SEGV_MAPERR:		return "SEGV_MAPERR\n";			// return "Address not mapped\n";
				case SEGV_ACCERR:		return "SEGV_ACCERR\n";			// return "Invalid permissions\n";
				default:					return "SIGSEGV:-\n";
			}

		case SIGBUS:
			switch(code) {
				case BUS_ADRALN:			return "BUS_ADRALN\n";		// return "Invalid address alignment\n";
				case BUS_ADRERR:			return "BUS_ADRERR\n";		// return "Non-existent physical address\n";
				case BUS_OBJERR:			return "BUS_OBJERR\n";		// return "Object-specific hardware error\n";
				default:					return "SIGBUS:-\n";
			}

		case SIGHUP:
			return "SIGHUP\n";
        case SIGABRT:
            return "SIGABRT\n";
		default:
			break;
	}
	return "Unknown\n";
}

//for reducing .data it inserts  "\n-----LOG_LINE_LEN---\n"
static char * hcr_add_line(char * dest)
{
	char * p;
	p = dest;
	*p = '\n'; p++;
	memset(p, '-', LOG_LINE_LEN ); p+= LOG_LINE_LEN;
	*p = '\n';
	return dest + LOG_LINE_LEN + 2;
}

static int hcr_check_string(const char * str, int range)
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

static char * hcr_paste_limitedstring(char * dst, const char * str, int range)
{
	int len;

	len = hcr_check_string(str, range);
	if(0 > len){
		strcpy(dst, "unknown");
		len = 7;
	}
	else{//found it.
		strcpy( dst, str);
	}
	return (char *)(dst + len);
}

static int hcr_increae_refcnt(void)
{
	int fd = -1;

	fd = open(REF_CNT_FILE, O_WRONLY|O_CREAT, 0666); // how about using Non-block option?
	if(fd == -1)
	{
		DbgPrt("hcr:%s: Can't open fd(path:%s)\n", __FUNCTION__, REF_CNT_FILE);
		return -1;
	}
	s_refcnt++;
	if (s_refcnt > BOUND_CNT)
	{
		s_refcnt = 0;
	}
	write(fd, &s_refcnt, sizeof(s_refcnt) );
	close(fd);

	return 0;
}

static int hcr_loginfo_remove(void)
{
	char cmdname[HCR_FULLPATH_LENGTH+10];

	snprintf(cmdname, HCR_FULLPATH_LENGTH+10, "%s %s/%s%s.*", "rm -f", HCR_LOG_PATH, HCR_LOG_PREFIX, s_proc_name);
	system(cmdname);

	return 0;
}

static int hcr_loginfo_install(void)
{
	//Set hcr folder & Ref Counting file.
	int fd_cnt = -1;

	//step1. check access the cr folder.
	if ( 0 != access(HCR_LOG_PATH, F_OK) )
	{
		if ( 0 != mkdir( HCR_LOG_PATH, S_IRWXU | S_IRWXG | S_IRWXO) )
		{
			return -1;
		}
	}

	//step2. Erase expired  files. -> no no no.
#ifdef HCR_TRANSFER_USB
    system("cp -f "HCR_LOG_PATH"/*.log /media/drive1/");
    system("cp -f "HCR_LOG_PATH"/*.map /media/drive1/");
#ifdef DUMP_STACKS
    system("cp -f "HCR_LOG_PATH"/*.stack /media/drive1/");
#endif
#endif
	//    system("rm -f "HCR_LOG_PATH"/"HCR_LOG_PREFIX"*");

	//step3. create or read ref.cnt file then use it as ref count.
	fd_cnt = open(REF_CNT_FILE, O_RDONLY, 0666);
	if(fd_cnt == -1)
	{
	    fd_cnt = open(REF_CNT_FILE, (O_WRONLY |O_CREAT), 0666);
		if(fd_cnt == -1)
		{
			DbgTrcl;
			// TODO: make a sheild procedure for protecting hazard value in refcnt.
			s_refcnt = INVALID_REF_CNT;
			return -1;
	    	}
		s_refcnt =0;

		write(fd_cnt, &s_refcnt, sizeof(s_refcnt) );
		DbgPrt("hcr: initizled refcnt\n");
		close(fd_cnt);
		return 0;
	}

	if(read( fd_cnt, &s_refcnt, sizeof(s_refcnt) ) == -1)
		s_refcnt = 0;

	close(fd_cnt);

	DbgPrt("hcr: read done with s_refcnt = %d\n", s_refcnt);

	return 0;
}

/* int hcr_backtrace_sig_mips32(void **buffer, int size,struct my_ucontext *uc); */
int hcr_backtrace_sig_mips32(void **buffer, int size,struct ucontext *uc)
{
    unsigned long *addr, *ra, *sp, *pc;
    size_t ra_offset, stack_size;
    int depth;
    char *p;
	int	flag_dump = 0;

	DbgTrcl;
   	memset(s_outbuf, 0, sizeof(s_outbuf));
	p = s_outbuf;

	p = hcr_add_line(p);
	strcpy( p , "bt mips:\n"); p+= 9;
    write(s_fd_log, s_outbuf, p - s_outbuf);

    if(!size)
        return 0;
    if(!buffer || size < 0 || !uc)
        return -EINVAL;

    /* get current $pc,$ra and $sp */
	if ( uc->uc_mcontext.gregs[31] & 0x1 ) {
		/* pc is an odd address ! */
    	memset(s_outbuf, 0, sizeof(s_outbuf));
	    p = s_outbuf;
	    strcpy(p, " ra IS AN ODD ADDRESS - FIX TO: "); p += 32; /*  4 == strlen(" pc "); */
    	hcr_util_ul2hex(uc->uc_mcontext.gregs[31] & 0xFFFFFFFC , p); p += 8;
	    *p = '\n'; ++p;
	    write(s_fd_log, s_outbuf, p - s_outbuf);

		ra = (unsigned long*)(unsigned long) (uc->uc_mcontext.gregs[31] & 0xFFFFFFFC);
	}
	else
    	ra = (unsigned long*)(unsigned long)uc->uc_mcontext.gregs[31];

	DbgTrcl;

	if ( uc->uc_mcontext.pc & 0x1 ) {
		/* pc is an odd address ! */
    	memset(s_outbuf, 0, sizeof(s_outbuf));
	    p = s_outbuf;
	    strcpy(p, " pc IS AN ODD ADDRESS - FIX TO: "); p += 32; /*  4 == strlen(" pc "); */
    	hcr_util_ul2hex(uc->uc_mcontext.pc & 0xFFFFFFFC , p); p += 8;
	    *p = '\n'; ++p;
	    write(s_fd_log, s_outbuf, p - s_outbuf);

	    pc = (unsigned long*)(unsigned long)(uc->uc_mcontext.pc & 0xFFFFFFFC);
	}
	else
	    pc = (unsigned long*)(unsigned long)uc->uc_mcontext.pc;

	if ( uc->uc_mcontext.gregs[29] & 0x1 ) {
		/* sp is an odd address ! */
    	memset(s_outbuf, 0, sizeof(s_outbuf));
	    p = s_outbuf;
	    strcpy(p, " sp IS AN ODD ADDRESS - FIX TO: "); p += 32; /*  4 == strlen(" pc "); */
    	hcr_util_ul2hex(uc->uc_mcontext.gregs[29] & 0xFFFFFFFC , p); p += 8;
	    *p = '\n'; ++p;
	    write(s_fd_log, s_outbuf, p - s_outbuf);

	    sp = (unsigned long*)(unsigned long)(uc->uc_mcontext.gregs[29] & 0xFFFFFFFC);
	}
	else
	    sp = (unsigned long*)(unsigned long)uc->uc_mcontext.gregs[29];

	DbgTrcl;
    buffer[0] = pc;
    if(size == 1)
    {
    	memset(s_outbuf, 0, sizeof(s_outbuf));
	    p = s_outbuf;
	    strcpy(p, " size is 1, early return"); p += 24; /*  4 == strlen(" pc "); */
	    *p = '\n'; ++p;
	    write(s_fd_log, s_outbuf, p - s_outbuf);
        return 1;
    }
	DbgTrcl;

	if( ( (unsigned long) sp < s_proc_stack_bot) || ( (unsigned long) sp > s_proc_stack_top) )
	{
		DbgPrt("hcr:err:stack smashed: %p\n", sp );
    	memset(s_outbuf, 0, sizeof(s_outbuf) );
	    p = s_outbuf;
	    strcpy(p, "Out of stack:sp:"); p += 16;
		hcr_util_ul2hex( (unsigned long)sp, p); p+=8;

		//print out range.
		strcpy(p, "["); p++;
		hcr_util_ul2hex( s_proc_stack_bot, p); p+=8;
		strcpy(p, ":"); p++;
		hcr_util_ul2hex( s_proc_stack_top, p); p+=8;
		strcpy(p, "["); p++;

	    *p = '\n'; ++p;
	    write(s_fd_log, s_outbuf, p - s_outbuf);
	}

	if( (DEFAUT_SEG_START > (unsigned long) pc) || (DEFAUT_SEG_START > (unsigned long) ra) )
	{
		DbgPrt("hcr:warning:pc=%p,ra=%p,sp=%p\n", pc, ra, sp);
		if(DEFAUT_SEG_START >(unsigned long) pc){
	    	memset(s_outbuf, 0, sizeof(s_outbuf));
		    p = s_outbuf;
		    strcpy(p, "!too low pc:"); p += 12;
			hcr_util_ul2hex( (unsigned long)pc, p); p+=8;
		    *p = '\n'; ++p;
		    write(s_fd_log, s_outbuf, p - s_outbuf);
			stack_size = 0;
			flag_dump = 1; //set flag
			goto __dump_stacks; // to avoid hang-up calculation.
		}
	}

    /* scanning to find the size of current stack-frame */
//	DbgPrt("hcr:start pc = %p, val=%X\n", pc, *pc &0xffff0000  ); //sometimes it goes to proc-hangup due of accessing low pc.
	if(0 != (unsigned long )pc % sizeof(unsigned long *)  ){
		DbgPrt("hcr:abnormal pc-not aligned\n");
	}


	DbgTrcl;
    ra_offset = stack_size = 0;
    for(addr = pc; (!ra_offset || !stack_size) && addr ; --addr)
    {
		DbgTrcl;
//		DbgPrt("*addr & 0xffff0000 = %X\n", (unsigned int)(0xffff0000 & *addr ) );
        switch(*addr & 0xffff0000)
        {
            case 0x27bd0000:  /* 0x27bd 0000 == addiu sp,sp, 0  - so *addr & 0xffff gives you stack size */
                stack_size = abs((short)(*addr & 0xffff));
                break;
            case 0xafbf0000: /* 0xafbf 0000 == sw ra,0(sp)  - ie it is saving registers on local stack */
                ra_offset = (short)(*addr & 0xffff);
                break;
            case 0x3c1c0000:
                goto __out_of_loop;

            default:
                break;
        }
    }

	DbgTrcl;
__out_of_loop:
	/* Abnormal state RA Offset */
	DbgTrcl;
    if(ra_offset)
    {
        ra = *(unsigned long**)((unsigned long)sp + ra_offset); //adjust it
	    memset(s_outbuf, 0, sizeof(s_outbuf));
	    p = s_outbuf;
	    strcpy(p, "\tra_offset(ab)  = "); p += 18; /* 14 == strlen(" ra_offset  = "); */
	    hcr_util_ul2hex((unsigned long)ra_offset, p); p += 8;
	    *p = '\n'; ++p;
	    write(s_fd_log, s_outbuf, p - s_outbuf);
    }

	DbgTrcl;
    if(stack_size)
    {
        sp = (unsigned long*)((unsigned long)sp + stack_size); //adjust it
	    memset(s_outbuf, 0, sizeof(s_outbuf));
	    p = s_outbuf;
	    strcpy(p, "\tstack_size(ab) = "); p += 18; /* 14 == strlen(" stack_size = "); */
	    hcr_util_ul2hex((unsigned long)stack_size, p); p += 8;
	    *p = '\n'; ++p;
	    write(s_fd_log, s_outbuf, p - s_outbuf);
    }
	DbgTrcl;

// go write easily... for humaxians.. ah..zol rin da.
__dump_stacks:
#ifdef DUMP_STACKS
	 // to dump  whole of proc stack into file.
//	if(flag_dump) //always dump stack.
	{
		FILE* fst;
		unsigned long * vp = (unsigned long *)s_proc_stack_bot;
		fst = fopen(s_stack_fullpath, "w");
		if(fst)
		{
			while( (unsigned int)vp < s_proc_stack_top )
			{
			/*	if(i%8==0){	strcpy(p, "@"); p++; hcr_util_ul2hex((unsigned long)vp, p); p += 8;	strcpy(p, " : "); p+=3;		}
				hcr_util_ul2hex((unsigned long)*vp, p); p += 8;
				if(k%8==7){*p ='\n'; p++;	}
				else{*p =' '; p++;		}*/ // gui chan ah seo... change it into file api.
				fprintf(fst, "@%08X: %08X %08X %08X %08X\n", (unsigned int)vp, \
					(unsigned int)vp[0], 	(unsigned int)vp[1],	(unsigned int)vp[2],(unsigned int)vp[3] );
				vp+=4;
			}

			fclose(fst);
		}
		memset(s_outbuf, 0, sizeof(s_outbuf) ); p = s_outbuf;
		strcpy(p, "!find dumped stack file.\n"); p+=25;
		write(s_fd_log, s_outbuf, p - s_outbuf);
	}

#if 0	 //estimation method in specfic range. --> no use it in rev 0.2.0
{ // <- this brace has special meaning. do not erase it.
	int k, half_range;
	unsigned long * vp;

	half_range = (4000 /*default boundary*/ + stack_size) ;
	vp = sp - half_range; // set starter point in range.

    memset(s_outbuf, 0, sizeof(s_outbuf));	p = s_outbuf;
	p = hcr_add_line(p);
	strcpy(p, "dump stack:\n"); p+=12;
	write(s_fd_log, s_outbuf, p - s_outbuf);

	//calculate it with windowing method.
	for(k=0; vp < sp + half_range; k++  ){
	    memset(s_outbuf, 0, sizeof(s_outbuf));
		p = s_outbuf;

		//write offset.
		if(k%8==0){
			strcpy(p, "@"); p++;
			hcr_util_ul2hex((unsigned long)vp, p); p += 8;
			strcpy(p, " : "); p+=3;
		}

		//write values -64< >+ 64
		hcr_util_ul2hex((unsigned long)*vp, p); p += 8;

		if(k%8==7){
			*p ='\n'; p++;
		}else{
			*p =' '; p++;
		}

		write(s_fd_log, s_outbuf, p - s_outbuf);
		vp++; //		vp--;

	}
    memset(s_outbuf, 0, sizeof(s_outbuf));	p = s_outbuf;
	p = hcr_add_line(p);
	write(s_fd_log, s_outbuf, p - s_outbuf);
}
#endif
#endif
	DbgTrcl;

    memset(s_outbuf, 0, sizeof(s_outbuf));
    p = s_outbuf;
    strcpy(p, "\tra "); p += 4; /* 4 == strlen(" ra "); */
    hcr_util_ul2hex((unsigned long)ra, p); p += 8;
    strcpy(p, ",\tsp "); p += 5; /* 5 == strlen(", sp "); */
    hcr_util_ul2hex((unsigned long)sp, p); p += 8;
    *p = '\n'; ++p;
    write(s_fd_log, s_outbuf, p - s_outbuf);
    /* printf(hcr:"\t$0  : %08X %08X %08X %08X\n" */
    /*        "\t$4  : %08X %08X %08X %08X\n", r0, r1, r2, r3, r4, r5, r6, r7); */
    memset(s_outbuf, 0, sizeof(s_outbuf));
    p = s_outbuf;
    strcpy(p,   "\t$0       : "); p += 12;         /* 12 == strlen("\t$0  : ") */
    hcr_util_ul2hex(uc->uc_mcontext.gregs[0], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[1], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[2], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[3], p); p += 8;

    strcpy(p, "\n\t$4       : "); p += 13;         /* 13 == strlen("\n\t$4     : ") */
    hcr_util_ul2hex(uc->uc_mcontext.gregs[4], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[5], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[6], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[7], p); p += 8;    *p = '\n'; ++p;
    write(s_fd_log, s_outbuf, p - s_outbuf);

    /* printf(hcr:"\t$8  : %08X %08X %08X %08X\n"
     *             "\t$12 : %08X %08X %08X %08X\n", ...);
     *                  okay to skip memset(s_outbuf, 0, sizeof(s_outbuf)); */
    p = s_outbuf;
    strcpy(p,   "\t$8       : "); p += 12;         /* 12 == strlen("\t$8  : ") */
    hcr_util_ul2hex(uc->uc_mcontext.gregs[8], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[9], p); p += 8;    *p = ' '; ++p;

    hcr_util_ul2hex(uc->uc_mcontext.gregs[10], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[11], p); p += 8;
		;
    strcpy(p, "\n\t$12      : "); p += 13;         /* 13 == strlen("\n\t$12    : ") */
    hcr_util_ul2hex(uc->uc_mcontext.gregs[12], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[13], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[14], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[15], p); p += 8;    *p = '\n'; ++p;
    write(s_fd_log, s_outbuf, p - s_outbuf);

    /* printf(hcr:"\t$16 : %08X %08X %08X %08X\n"
              "\t$20 : %08X %08X %08X %08X\n", ...);
               okay to skip memset(s_outbuf, 0, sizeof(s_outbuf));  */
    p = s_outbuf;
    strcpy(p,   "\t$16      : "); p += 12;         /* 12 == strlen("\t$16   : ") */
    hcr_util_ul2hex(uc->uc_mcontext.gregs[16], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[17], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[18], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[19], p); p += 8;

    strcpy(p, "\n\t$20      : "); p += 13;         /* 13 == strlen("\n\t$20    : ") */
    hcr_util_ul2hex(uc->uc_mcontext.gregs[20], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[21], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[22], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[23], p); p += 8;    *p = '\n'; ++p;
    write(s_fd_log, s_outbuf, p - s_outbuf);
    /* printf(hcr:"\t$24 : %08X %08X %08X %08X\n"
              "\t$28 : %08X %08X %08X %08X\n", ...);
               okay to skip memset(s_outbuf, 0, sizeof(s_outbuf));  */
    p = s_outbuf;
    strcpy(p,   "\t$24      : "); p += 12;          /* 12 == strlen(  "\t$24       : ") */
    hcr_util_ul2hex(uc->uc_mcontext.gregs[24], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[25], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[26], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[27], p); p += 8;

    strcpy(p, "\n\t$28      : "); p += 13;          /* 13 == strlen("\n\t$28      : ") */
    hcr_util_ul2hex(uc->uc_mcontext.gregs[28], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[29], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[30], p); p += 8;    *p = ' '; ++p;
    hcr_util_ul2hex(uc->uc_mcontext.gregs[31], p); p += 8;    *p = '\n'; ++p;
    write(s_fd_log, s_outbuf, p - s_outbuf);

    /* printf(hcr:"\tStatus: %p\n"
              "\tCause : %p\n"
              "\tBadVaddr : %p\n",
            uc->uc_mcontext.sc_status, uc->uc_mcontext.sc_hi1,  uc->uc_mcontext.sc_lo1);
    */
    memset(s_outbuf, 0, sizeof(s_outbuf));
    p = s_outbuf;

#if 0 //org.
    strcpy(p,   "\tStatus   : "); p += 12;         /* 12 == strlen("\tStatus : ") */
    hcr_util_ul2hex(uc->uc_mcontext.sc_status, p); p += 8;
#else
	strcpy(p,   "\tStatus   : "); p += 12;         /* 12 == strlen("\tStatus : ") */
	hcr_util_ul2hex( uc->uc_mcontext.status, p); p += 8;		/* i don't believe and know it. don't go to struggle with it.*/
#endif
    strcpy(p, "\n\tCause    : "); p += 13;         /* 13 == strlen("\n\tCause  : ") */
    hcr_util_ul2hex(uc->uc_mcontext.hi1, p); p += 8;
    strcpy(p, "\n\tBadVaddr : "); p += 13;         /* 13 == strlen("\n\tBadVaddr : ") */
    hcr_util_ul2hex(uc->uc_mcontext.lo1, p); p += 8;
    *p = '\n'; ++p;

#if 0 //not supported. sipal.
	strcpy(p, "\n\tSS Size : "); p += 12;         /* 13 == strlen("\n\tBadVaddr : ") */
	hcr_util_ul2hex(uc->uc_stack.ss_size, p); p+= 8;
    *p = '\n'; ++p;
#endif

    write(s_fd_log, s_outbuf, p - s_outbuf);
	memset(s_outbuf, 0, sizeof(s_outbuf) );		p = s_outbuf;
	DbgTrcl;
    /* repeat backward scanning */
    for(depth = 1; depth < size && ra; ++depth)
    {
		//DbgPrt("hcr:depth = %d ra=%p addr=%p, line = %d\n", depth, ra, addr,  __LINE__ );
        buffer[depth] = ra;
        ra_offset = stack_size = 0;
#if 1
		DbgTrcl;
		if( 0 != (unsigned long)ra % 4  ) 	// iLLegal RA.
		{

			unsigned long * ra_truc;
			ra_truc = (unsigned long *)( ((unsigned long )ra / 4) * 4);
			DbgPrt("RA not address format then truncated, ra = %p, ra_truc =%p\n", ra,  ra_truc);
			memset(s_outbuf, 0, sizeof(s_outbuf) );		p = s_outbuf;
			strcpy(p, "ScaningRA: "); p+=11;
			hcr_util_ul2hex((unsigned long) ra, p); p+=8;
			strcpy(p, ", TruncatRA: "); p+=13;
			hcr_util_ul2hex((unsigned long) ra_truc, p); p+=8;
			strcpy(p, "\n"); p++;
		    write(s_fd_log, s_outbuf, p - s_outbuf);
			ra = ra_truc;
		}

		// TODO: Fix it.
		if( DEFAUT_SEG_START > (unsigned long) ra)
		{
			DbgPrt("Exception RA, %p\n", ra );
			memset(s_outbuf, 0, sizeof(s_outbuf) ); 	p = s_outbuf;
			strcpy(p, "\t*Exception:"); p+=12;
			hcr_util_ul2hex((unsigned long) ra, p); p+=8;
			p = hcr_add_line(p);
		    write(s_fd_log, s_outbuf, p - s_outbuf);
			return (depth | MAGIC_SIG1);
		}

#endif
        for(addr = ra; (!ra_offset) || (!stack_size) ; --addr)
        {
            switch(*addr & 0xffff0000)
            {
                case 0x27bd0000: /* 0x27bd 0000 == addiu sp,sp, 0  - so *addr & 0xffff gives you stack size */
    	            stack_size = abs((short)(*addr & 0xffff));
					DbgPrt("hcr:got stack size =0x%x\n", stack_size);
        	        break;
                case 0xafbf0000: /* 0xafbf 0000 == sw ra,0(sp)  - ie it is saving registers on local stack */
	                ra_offset = (short)(*addr & 0xffff);
					DbgPrt("hcr:got ra offset=0x%x\n", ra_offset);
	                break;
                case 0x3c1c0000:  /* in assembly = lui  gp,0x0 */
					DbgTrcl;
					/* Added because hcr_backtrace_sig_mips32 often returns and doesn't print rest of debug */
    				memset(s_outbuf, 0, sizeof(s_outbuf));
				    p = s_outbuf;
				    strcpy(p, "lui gp depth="); p += 13; /*  p=7 because spacedepth= */
				    hcr_util_ul2hex(depth+1, p); p += 8;
					p = hcr_add_line(p);
				    write(s_fd_log, s_outbuf, p - s_outbuf);
					DbgTrcl;
	                return depth+1;
                default:
//					DbgTrcl;
	                break;
            }
        }
		DbgTrcl;
        ra = *(unsigned long**)((unsigned long)sp + ra_offset);
        sp = (unsigned long*)((unsigned long)sp + stack_size);


    }
	/* Added because hcr_backtrace_sig_mips32 often returns and doesn't print rest of debug */
	DbgTrcl;

    strcpy(p, "\tDepth="); p += 7; /*  p=7 because spacedepth= */
    hcr_util_ul2hex(depth, p); p += 8;
	p = hcr_add_line(p);

    write(s_fd_log, s_outbuf, p - s_outbuf);
	DbgTrcl;
    return depth;
}

static HLIB_DBG_INFO_t *s_killer= NULL;

void hcr_get_dbg_timerlist(HLIB_DBG_INFO_t *pinfo, HBOOL bPass)
{
    memset(s_outbuf, 0, sizeof(s_outbuf));

    sprintf(s_outbuf,"\t[%s](%d-%d)[%s]%s\n",(bPass==TRUE)?"PASS":"WAIT",pinfo->tid,pinfo->timer,pinfo->caller,pinfo->message);

    if(s_killer && s_killer->timer == pinfo->timer)
         sprintf(s_outbuf,"\t>>[%s](%d-%d)[%s]%s\n",(bPass==TRUE)?"PASS":"WAIT",pinfo->tid,pinfo->timer,pinfo->caller,pinfo->message);

    write(s_fd_log, s_outbuf, strlen(s_outbuf));
}

static void hcr_print_timerinfo(void)
{

    char buffer[128] = {0,},*p;

    memset(s_outbuf, 0, sizeof(s_outbuf));
    p = s_outbuf;
    p = hcr_add_line(p);
    sprintf(buffer,"timer info\n");
    strcpy(p, buffer); p += strlen(buffer);
    write(s_fd_log, s_outbuf,strlen(s_outbuf));


    memset(s_outbuf, 0, sizeof(s_outbuf));
    sprintf(s_outbuf,"exceed time:\n");
    write(s_fd_log, s_outbuf, strlen(s_outbuf));

    s_killer = HLIB_DBG_GetExceedTimer();
    if(s_killer)
    {
        memset(s_outbuf, 0, sizeof(s_outbuf));
        sprintf(s_outbuf,"\t(%d-%d)[%s]%s\n",s_killer->tid,s_killer->timer,s_killer->caller,s_killer->message);
        write(s_fd_log, s_outbuf, strlen(s_outbuf));
    }

    memset(s_outbuf, 0, sizeof(s_outbuf));
    sprintf(s_outbuf,"list:\n");
    write(s_fd_log, s_outbuf, strlen(s_outbuf));
    HLIB_DBG_GetTimerList(hcr_get_dbg_timerlist);
}

static char ** hcr_backtrace_card_symbols ( void *const *array, int size)
{
    int cnt;
    size_t total = 0;
    char **result;
    /* Assume the worst for the width of an address.  */
    #define WORD_WIDTH 16

    if( size < 1 )
    {
        char *p;

        /* printf(hcr:"[%s] No stacked pointer(%d)\n", __FUNCTION__, size); */
        memset(s_outbuf, 0, sizeof(s_outbuf));
        p = s_outbuf;
        strcpy(p, "[hcr_backtrace_card_symbols] No stacked pointer\n");
        p += strlen(p);
        write(s_fd_log, s_outbuf, p - s_outbuf);
    }

    /* We can compute the text size needed for the symbols since we print
       them all as "[+0x<addr>]".  */
    total = size * (WORD_WIDTH + 6);

    /* Allocate memory for the result.  */
    result = (char **)malloc (size * sizeof (char *) + total);
    if (result != NULL)
    {
        char *last = (char *) (result + size);

        for (cnt = 0; cnt < size; ++cnt)
        {
            result[cnt] = last;
            /* last +=
               1 + sprintf (last, "[+0x%08x]", (unsigned int)array[cnt]);
            */
            strcpy(last, "[+0x"); last += 4; /* 4 == strlen("[+0x") */
            hcr_util_ul2hex((unsigned long)array[cnt], last); last += 8;
            *last = ']'; ++last;
            *last = '\0'; ++last;
        }
    }

    return result;
}



/* Comment for baton touch : Do NOT use non-thread safe function as like sprintf,
strlen within critical calculation. You MUST do best to go away from
using heavy inst. fn & non-reentrant fn. if may not, you could easily meet unexpected result.
You shall keep in mind that it's running upon small stack plus graceful bonus stack*/
static void hcr_siginfo_handler(int signal, siginfo_t *sig_info, void *context)
{
	void *buf[TCBUFSIZE];
	int d, i;
	char **syms;
	char SysCall[128]={0,};
	Dl_info info;
	char *p;
	struct ucontext *pUc = (struct ucontext*) context;

	char task_path[64]={0,};
	char task_name[32]={0,};
	int tid = syscall(4222);
	int fd;

	if (s_pfnSignalUserHandler)
		s_pfnSignalUserHandler(signal, sig_info, context);

//	char achHcrProcName[64]={0,};
//	char achTemp[16]={0.};

	/* print pid early because sometmes don't get to pid printing part */
//	Pid=getpid();

	if(	(signal != SIGHUP) && (signal != SIGILL)
		&& (signal != SIGFPE)	&& (signal != SIGSEGV)
		&& (signal != SIGBUS)  &&(signal != SIGABRT)  )
	{
		// TODO: Later marking this status & sig.
		DbgPrt("hcr:other signal entered: %s\n", hcr_StrSigCode(signal, 0) );
		exit(-1);
	}

	DbgTrcl;
	DbgPrt("hcr:signal entry: pid=%d, sig=%d\n", s_proc_pid, signal);


	/* Using it for hcr logging & proc dump prefiex */
	if(!s_virgin){
		DbgPrt("hcr: Drop!\n");
		/* Drop these signals after once done in one process. non-meaningful information. */
		exit(-1);
	}
	s_virgin = 0;

#ifdef HCR_LOGGING_ON_TTY
	s_fd_log = 1;
	//hide other devce.
#else
	if (((s_refcnt % BOUND_CNT) == 0) && (s_refcnt > 0))
	{
		hcr_loginfo_remove();
	}

    s_fd_log = open( s_tmp_fullpath_log, O_RDWR |O_CREAT | O_TRUNC, 0666);
    if(1 > s_fd_log)
    {
        DbgPrt("hcr:Can't open fd for logging(path:%s)\n", s_tmp_fullpath_log);
        exit(-1);
    }
#endif

	/*Pre.Get Tid */
	p = task_path;
	strcpy(p, s_proc_path ); p += hcr_check_string(task_path, sizeof(task_path) );
	strcpy(p, "/task/"); p += 6;
	hcr_util_ul2dec(tid, p);
	strcat(p, "/comm");

	fd = open(task_path, O_RDONLY);
	if (fd >= 0) // it has to include '0' value due of vir. file.
	{
		unsigned int ulSize = 0;

		if(sizeof(task_name) > 32)
		{
			ulSize = 32;
		}
		else
		{
			ulSize = sizeof(task_name);
		}

		//prevent #182801
		i = read(fd, task_name, ulSize -1 ) ; // do never check return value.
		close(fd);
		if(i > 0 )
			task_name[i-1] = 0;
		else
			strncpy(task_name, "unknown-r",sizeof(task_name));

		task_name[32-1] = 0;
	}
	else{
		strncpy(task_name, "unknown-o",sizeof(task_name));
	}
	DbgPrt("hcr: task_name: %s\n", task_name);

	/* Step 1. write a revision, the build date, PID and a indicator whether debug mode or not. */
	memset(s_outbuf, 0, sizeof(s_outbuf));
	strcpy(s_outbuf, HCR_REV"(" HCR_DATE "," HCR_TIME ")\nPID:\t");
	strcat(s_outbuf,	s_proc_pid_str );

	hcr_util_ul2dec(tid, SysCall); //multiple usage.
	strcat(s_outbuf, ",\tTID:\t");
	strcat(s_outbuf, SysCall);

#if defined(CONFIG_DEBUG)
	strcat(s_outbuf, "\tD");
#endif
	strcat(s_outbuf, "\n");
	write(s_fd_log, s_outbuf, strlen(s_outbuf) );

	/* Step 2. write the process name  */
	/*Read & Write the proc name --> Just write hereby. the reason is for reducting complexity in a sig handler. */
	memset(s_outbuf, 0, sizeof(s_outbuf));
	//len = sprintf(s_outbuf, "Process name: %s\n", s_proc_name);
	strcpy(s_outbuf, "Process name: ");
	strcat(s_outbuf, s_proc_name);

	strcat(s_outbuf, ",\tTask name: ");
	strcat(s_outbuf, task_name);
	strcat(s_outbuf, "\n");
	write(s_fd_log, s_outbuf, strlen(s_outbuf) );

	/* Step 3. describe a state from signal & si_code  */
	memset(s_outbuf, 0, sizeof(s_outbuf));
	strncpy(s_outbuf, "SIG: ", sizeof(s_outbuf));
//	strncat(s_outbuf, hcr_StrSigCode(signal, sig_info->si_code), sizeof(s_outbuf));
	strncat(s_outbuf, hcr_StrSigCode(signal, sig_info->si_code), sizeof(s_outbuf)-1);	// prevent #228461
	write(s_fd_log, s_outbuf, strlen(s_outbuf) );

	/* Step 4. write the issue date  */
	memset(s_outbuf, 0, sizeof(s_outbuf));
	p = s_outbuf;
	strcpy( p, "Issue date: "); p += 12;
	hcr_util_ul2hex((unsigned long)time(NULL), p ); p +=8;
	*p = '\n'; p++;
	write(s_fd_log, s_outbuf, p - s_outbuf );

	/* Step 5. make a summary from received signal info. not based ucontext. */
	memset(s_outbuf, 0, sizeof(s_outbuf));
	p = s_outbuf;
	p = hcr_add_line(p);
	strcpy(p, "sig info:\n"); p += 10;
	strcpy(p, "signo "); p += 6;
	hcr_util_ul2hex(sig_info->si_signo, p); p += 8; //  just curious?  hcr_util_ul2hex(signal, p); p += 8;

	strcpy(p, "\tsicode "); p += 8;
	hcr_util_ul2hex(sig_info->si_code, p); p +=8;

	strcpy(p, "\tsiaddr "); p += 8;
	hcr_util_ul2hex((unsigned long)sig_info->si_addr, p); p +=8;
	*p = '\n'; p++;
	write(s_fd_log, s_outbuf, p - s_outbuf);

	memset(s_outbuf, 0, sizeof(s_outbuf));
	p = s_outbuf;

	if (NULL == pUc)
	{
		/* printf("%s sig %d no ctxt\n", __FUNCTION__, signal); */
		strcpy(p, "no ctxt"); p += 7; /* 7 == strlen("no ctxt") */
		/* This should never happen, but if it does this signal handler will
		 * crash again after printing out "no ctxt". */
	}
	else
	{
		strcpy(p, "pc "); p += 3;
		hcr_util_ul2hex(pUc->uc_mcontext.pc, p); p += 8; /* pc */
		strcpy(p, "\tsp "); p += 4;
		hcr_util_ul2hex(pUc->uc_mcontext.gregs[29], p); p += 8; /* sp */
		strcpy(p, "\tfp "); p += 4;
		hcr_util_ul2hex(pUc->uc_mcontext.gregs[30], p); p += 8; /* fp */
		strcpy(p, "\tra "); p += 4;
		hcr_util_ul2hex(pUc->uc_mcontext.gregs[31], p); p += 8; /* ra */
	}

	*p = '\n'; ++p;
	write(s_fd_log, s_outbuf, p - s_outbuf);


    /* Check if either program counter or return address are ODD  */
    if ( (pUc != NULL) && (pUc->uc_mcontext.pc & 0x1 || pUc->uc_mcontext.gregs[31] & 0x1) ) {
        /* pc and ra should NEVER be ODD, dump stack contents so we can check */
        unsigned long *stackAddr;

        /* Put line feed space in between map info and stack info */
        memset(s_outbuf, 0, sizeof(s_outbuf));
        p = s_outbuf;
        *p = '\n'; ++p;
		strcpy(p, "odd process\n"); p +=12;
        write(s_fd_log, s_outbuf, p - s_outbuf);

        stackAddr = (unsigned long*)(unsigned long)pUc->uc_mcontext.gregs[29] - 32;
        for ( i=0 ; i < 256/16; i++ ) {
            memset(s_outbuf, 0, sizeof(s_outbuf));
            p = s_outbuf;
            strcpy(p, "\t"); p += 1; /* 7 == strlen("\t$0  : ") */
            hcr_util_ul2hex( (unsigned long)stackAddr, p); p += 8;
            strcpy(p, "  : "); p += 4; /* 7 == strlen("\t$0  : ") */

            hcr_util_ul2hex( *stackAddr, p); p += 8;
            *p = ' '; ++p;
            stackAddr++;
            hcr_util_ul2hex( *stackAddr, p); p += 8;
            *p = ' '; ++p;
            stackAddr++;
            hcr_util_ul2hex( *stackAddr, p); p += 8;
            *p = ' '; ++p;
            stackAddr++;
            hcr_util_ul2hex( *stackAddr, p); p += 8;
            stackAddr++;
            *p = '\n'; ++p;
            write(s_fd_log, s_outbuf, p - s_outbuf);
        }
        memset(s_outbuf, 0, sizeof(s_outbuf));
        p = s_outbuf;
        *p = '\n'; ++p;
        write(s_fd_log, s_outbuf, p - s_outbuf);
    }

    /* Copy the map file to nvram,  the best way, -.- */
    //sprintf(SysCall, "cp -f /proc/%d/maps %s", s_proc_pid, s_tmp_fullpath_map);
    memset(SysCall, 0, sizeof(SysCall) );
	p = SysCall;
    strcpy(p, "cp -f /proc/"); p += 12;
	strcpy(p, s_proc_pid_str); 	p += s_proc_pid_strlen; // don't use strlen hereby.
	strcpy(p, "/maps "); 		p += 6;
	strcpy(p, s_tmp_fullpath_map);
	system(SysCall);

	hcr_increae_refcnt();

#if 0
{
	/*The best surely mehtod to extract stack range with avoiding gui chanism*/
	FILE * pd; // TODO: change it raw I/O later.
	char out[0xFF] = {0,};

	DbgTrcl;
	p = SysCall;
	strcpy(p, "grep '\\[stack\\]' /proc/"); p += 23; //"grep '\\[stack\\]' /proc/"
	strcpy(p, s_proc_pid_str);  p += s_proc_pid_strlen;
	strcpy(p, "/maps ");
	DbgPrt("syscall : %s\n", SysCall);
	pd = popen(SysCall,  "r");
	if( NULL != pd)
	{
		fread(out, 17, 1, pd); // do not see ret due of mapped mem. //int r = fread(out, 17, 1, pd);
		pclose(pd); // it's not fclose.
		sscanf(out, "%08x-%08x", &s_proc_stack_bot, &s_proc_stack_top ); //do not use fscanf. it's not file operation. zol rin da..
		DbgPrt("hcr:proc.stack:%08x - %08x\n", s_proc_stack_bot, s_proc_stack_top);
	}
	else{
		DbgPrt("hcr:read stack bot-top:failed:set default\n");;
		//Set default boundary to process the smashed state. moe ri do a pa.
		s_proc_stack_bot = DEFAULT_STACK_BOT; //bot.. i usally choose TiMo.
		s_proc_stack_top = DEFAULT_STACK_TOP;
	}
}
#else
{
#if 1
	int rc;
	pthread_attr_t attr;
	size_t stack_size;
	void *stack_addr;
	unsigned int stackPointer;

	s_proc_stack_bot = DEFAULT_STACK_BOT; //bot.. i usally choose TiMo.
	s_proc_stack_top = DEFAULT_STACK_TOP;

	rc = pthread_getattr_np(pthread_self(), &attr);
	if (rc == 0)
	{
    		rc = pthread_attr_getstack(&attr,(void *)&stack_addr, (size_t *)&stack_size );


		if (rc == 0)
		{
#if 1
			if((pUc!= NULL)&&(stack_size > (DEFAULT_STACK_DUMP_SIZE_UP + DEFAULT_STACK_DUMP_SIZE_DOWN)))
			{
				stackPointer = (unsigned long)pUc->uc_mcontext.gregs[29];
				DbgPrt("hcr:stackPointer: 0x%08x \n", stackPointer);
				DbgPrt("hcr:proc.stack:%08x - %08x (orignal)\n", (unsigned int)stack_addr, (unsigned int)stack_addr + (unsigned int)stack_size);
				s_proc_stack_bot = ((stackPointer -DEFAULT_STACK_DUMP_SIZE_DOWN) > (unsigned int)stack_addr) ? (stackPointer -DEFAULT_STACK_DUMP_SIZE_DOWN) : (unsigned int)stack_addr;
				s_proc_stack_top = ((stackPointer + DEFAULT_STACK_DUMP_SIZE_UP) < (unsigned int)stack_addr+(unsigned int)stack_size) ? (stackPointer +DEFAULT_STACK_DUMP_SIZE_UP) : (unsigned int)stack_addr+(unsigned int)stack_size;
			}
			else
#endif
			{
				s_proc_stack_bot = (unsigned int)stack_addr; //bot.. i usally choose TiMo.
				s_proc_stack_top = (unsigned int)stack_addr + (unsigned int)stack_size;
			}
		}
	}

	rc = pthread_attr_destroy(&attr);

	DbgPrt("hcr:proc.stack:%08x - %08x\n", s_proc_stack_bot, s_proc_stack_top);


#else
	// uc_stack is always set to 0.... why?
	printf("shkotest sp: 0x%08x, size: 0x%08x \n", (unsigned long)pUc->uc_stack.ss_sp, (unsigned long)pUc->uc_stack.ss_size);
	s_proc_stack_bot = (unsigned long)pUc->uc_stack.ss_sp;
	s_proc_stack_top = (unsigned long)pUc->uc_stack.ss_sp + (unsigned long)pUc->uc_stack.ss_size;

#endif


}

#endif
    hcr_print_timerinfo();

	/* Step 6. start inspection registers from ucontex.  */
    d = hcr_backtrace_sig_mips32(buf, TCBUFSIZE, (struct ucontext*)context);

    if( d < 1 )
    {
        /* printf("[%s] Invalid depth(%d)\n", __FUNCTION__, d); */
		DbgTrcl;
        memset(s_outbuf, 0, sizeof(s_outbuf));
        p = s_outbuf;
        strcpy(p, "[sig_handler_with_arg] Invalid depth("); p += strlen(p);
        switch (d) {
        case 0:
            strcpy(p, "0)\n"); p += 3; /* 3 == strlen("0)\n") */
            break;
        case -EINVAL:
            strcpy(p, "EINVAL)\n"); p += 8; /* 8 == strlen("EINVAL)\n") */
            break;
        default:
            strcpy(p, "err)\n"); p += 5; /* 5 == strlen("err)\n") */
            break;
        }
        write(s_fd_log, s_outbuf, p - s_outbuf);

    }
	/* Step 7. if it have some possiblity of meaningful info, we go to find symbols with having calculated depth. */
    else
    {
		DbgTrcl;
		memset(s_outbuf, 0, sizeof(s_outbuf));

		if(d & MAGIC_SIG1){
			d &= 0xFF;
			strcpy(s_outbuf, ":You may assume stack corruption\n"); ; // 	        p += strlen(p);
			write(s_fd_log, s_outbuf, 33);
		}

		memset(s_outbuf, 0, sizeof(s_outbuf));
        syms = hcr_backtrace_card_symbols(buf, d);
        for(i = 0;i < d;i++)
        {
            memset(s_outbuf, 0, sizeof(s_outbuf));
            p = s_outbuf;
			*p = '@'; p++;

			DbgTrcl;
            if(!dladdr(buf[i], &info)){
				DbgTrcl;
                /* printf("%s -\n", syms[i]); */
				p = hcr_paste_limitedstring(p, syms[i], 16); //approx 14.
				strcpy(p, " -\n"); p+=3;
            }
            else{
				//Lib relocated addr.
				p = hcr_paste_limitedstring(p, syms[i], 16); //approx 14.
				strcpy(p, " - "); p+=3;
				//Lib name.
				p = hcr_paste_limitedstring(p, info.dli_fname, 64);
                *p = ':'; ++p;

				//Func offset from Lib.base
                strcpy(p, " [0x"); p += 4; /* 4 == strlen(" [0x") */
                hcr_util_ul2hex((char *)buf[i]-(char *)info.dli_fbase, p); p += 8;
                *p = ']'; p++;
                *p = '/'; p++;

				//Func name if matched offset.
				p = hcr_paste_limitedstring(p, info.dli_sname, 48);
				*p = ':'; ++p;

                strcpy(p, " [0x"); p += 4; /* 4 == strlen(" [0x") */
                hcr_util_ul2hex((char *)buf[i]-(char *)info.dli_saddr, p); p += 8;

                *p = ']'; p++;
                *p = '\n'; p++;
				DbgTrcl;
            }
            write(s_fd_log, s_outbuf, p - s_outbuf);
			DbgTrcl;
        }
		free(syms);
    }

	if(s_fd_log > 1){
		DbgTrcl;
		//add fdatasync
		fdatasync(s_fd_log);
		close(s_fd_log);
// move to stroing map file		hcr_increae_refcnt();

		// Step. 7.x touch vff file.
	    s_fd_log = open( VFF_FILE, O_RDWR |O_CREAT | O_APPEND, 0666);
		if (s_fd_log > 1)
		{
			memset(s_outbuf, 0, sizeof(s_outbuf) );
			p = s_outbuf;
			*p = '!';  p++;
			hcr_util_ul2hex(s_refcnt -1, p); p+=8; /* write a form '!00000001!00000002 ...!ref_cnt */
			write(s_fd_log, s_outbuf, p - s_outbuf);
			fdatasync(s_fd_log);
			close(s_fd_log);
		}
	}

	DbgTrcl;

	/* Step 8. Dumping out all values in stacks  */

	/* Step 9. Dumping threads' stack. -> no plan. */

	/* Step 10. Post procedure, in real we do not any control.  */
#if defined(HCR_TRANSFER_USB) //just for faster debugging.
	//hereby, check safety for storing files.
    system("cp -f "HCR_LOG_PATH"/*.log /media/drive1/");
    system("cp -f "HCR_LOG_PATH"/*.map /media/drive1/");
    system("cp -f "VFF_FILE "/media/drive1/");
#ifdef DUMP_STACKS
    system("cp -f "HCR_LOG_PATH"/*.stack /media/drive1/");
#endif

#elif defined(HCR_TRANSFER_HTTP)

#elif defined(HCR_TRANSFER_FTP)

#endif //elif defined(HCR_TRANSFER_FTP)

    exit(-1);
}

static int hcr_siginfo_install(void)
{
    struct sigaction sig_act;

	memset(&sig_act, 0, sizeof(struct sigaction) );
    sigfillset(&sig_act.sa_mask);// block all signal during processing sig info.    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_sigaction = hcr_siginfo_handler;

    sig_act.sa_flags = (SA_SIGINFO |SA_ONSTACK | SA_ONESHOT);

	sigaction(SIGHUP,&sig_act,NULL);
	sigaction(SIGILL,&sig_act,NULL);
	sigaction(SIGFPE,&sig_act,NULL);
	sigaction(SIGSEGV,&sig_act,NULL);
	sigaction(SIGBUS,&sig_act,NULL);
    sigaction(SIGABRT,&sig_act,NULL);
/* Discuss these later.
    sigaction(SIGINT,&sig_act,NULL);
    sigaction(SIGTERM,&sig_act,NULL);    */
    return 0;
}



int HCR_Init(void (*pfnSignalUserHandler)(int sig, siginfo_t *sig_info, void *context))
{
	int len = 0 ;
	int fd_proc;
	char proc_comm_path[64] = {0,};
	char *p;

	s_pfnSignalUserHandler = pfnSignalUserHandler;

	/* get process name from /proc */
	memset(s_proc_name, 0, sizeof(s_proc_name));
	memset(s_proc_pid_str, 0, sizeof(s_proc_pid_str));

	s_proc_pid = getpid();
	hcr_util_ul2dec(s_proc_pid, s_proc_pid_str );
	s_proc_pid_strlen = strlen(s_proc_pid_str);

	DbgPrt("hcr: init: enter\n");

	p =	proc_comm_path;
	strcpy(p, "/proc/"); p += 6;
	strcpy(p, s_proc_pid_str );
	strcpy(s_proc_path, proc_comm_path ); /*for other using.*/
	p += strlen(s_proc_pid_str);
	strcpy(p, "/comm");

	fd_proc = open(proc_comm_path, O_RDONLY);
	if (fd_proc >= 0)
	{
		len = read(fd_proc, (char*)s_proc_name, sizeof(s_proc_name)-1);
		close(fd_proc);
	}

	if (len <=0 )
	{
		strncpy(s_proc_name, "unknown", 7);
		s_proc_name[7] = 0;
		return -1;
	}

	/* when we read strings from comm file, last character value is '\n'. delete it */
	s_proc_name[len-1] = 0;

	if( hcr_loginfo_install() )
	{
		DbgPrt("hcr:err:%s,%d\n", __FUNCTION__, __LINE__ );
	}

	if( hcr_siginfo_install() )
	{
		DbgPrt("hcr:err:%s,%d\n", __FUNCTION__ , __LINE__ );
	}

	/*following 'snprintf' was allowed because it's not signal handler */
	snprintf(s_log_file_name, sizeof(s_log_file_name), "%s%s.%d.%d.log", HCR_LOG_PREFIX, s_proc_name, s_refcnt, s_proc_pid );
	snprintf(s_map_file_name, sizeof(s_map_file_name), "%s%s.%d.%d.map", HCR_LOG_PREFIX, s_proc_name, s_refcnt, s_proc_pid );

	snprintf(s_tmp_fullpath_log, sizeof(s_tmp_fullpath_log), "%s/%s", HCR_LOG_PATH, s_log_file_name);
	snprintf(s_tmp_fullpath_map, sizeof(s_tmp_fullpath_map), "%s/%s", HCR_LOG_PATH, s_map_file_name);

	snprintf(s_stack_fullpath, sizeof(s_stack_fullpath), \
		HCR_LOG_PATH"/%s%s.%d.%d.stack", HCR_LOG_PREFIX, s_proc_name, s_refcnt, s_proc_pid );

	DbgPrt("hcr: init: ok(%s)\n", s_tmp_fullpath_log);

	return 0;
}

#ifdef CRASH_TEST
void HCR_DBG_CRASH_SegV1(void)
{
	volatile int* a = (int*)(NULL);
	DbgPrt("hcr:homing:Lovely Kuma!\n");
	*a = 1;
}
void HCR_DBG_CRASH_SegV2(void)
{
    char *s = "hello world";
    *s = 'H';
}

//attack lower(next) or loaded inst.
void HCR_DBG_CRASH_StackOver1(void)
{
	int ulCount;
	int overSize = 16; // x 4 byte corrptted.
	int a[4];
	int *b;

	//for break. ssp guide.
	DbgPrt("hcr:crash:Stack lower corruption!(%d)\n", overSize * sizeof(int) );
	for(ulCount=0, b = a; ulCount < (sizeof(a ) /sizeof(int) ) + overSize; ulCount++, b-- )
	{
		*b = ulCount;
	}
}

//attack upper or expired inst stack.
void HCR_DBG_CRASH_StackOver2(void)
{
	int ulCount;
	int overSize = 16; // x 4 byte corrptted.
	int a[4];
	int *b;

	//for break. ssp guide.
	DbgPrt("hcr:crash:Stack upper overwrite!(%d)\n", overSize * sizeof(int) );
	for(ulCount=0, b = a; ulCount < (sizeof(a ) /sizeof(int) ) + overSize; ulCount++, b++ )
	{
		*b = ulCount;
	}
}

//slicely over.
void HCR_DBG_CRASH_StackOver3(void)
{
	char buf[1];

	DbgPrt("hcr:crash:Stack over! +4\n");
	sprintf(buf, "%s", "Kuma!");
}

void HCR_DBG_CRASH_StackNonSense(void)
{
	int i;
	char a = 0;
	char buf[0x100000 +1]; // the stack size of 0x100000 is default max stack size in nasm. then it's always going to make a hazard in any embedded dev.

//	DbgPrt("hcr:crash:Stack over! +4\n");
//	sprintf(buf, "%s", "Kuma!");

	for(i=0; i< sizeof(buf) ; i++)
	{
		buf[i] = a++;
		DbgPrt("hcr:crash:sbig:%c\n", buf[i]) ;
	}
}

void HCR_DBG_CRASH_HomingInfLoop(void)
{
	unsigned long *p;
	unsigned long *v;
	unsigned long *x;

	DbgPrt("hcr:homing:Inf. Op!\n");

	for(p = (unsigned long *)0xF ; p ; p--)
	{
		x =  p;
		for(v=x; v  ; --v)	{
			;
		}
	}
}
#endif

#ifdef RUNTIME_TRACING
//Trace 2
#define _GNU_SORUCE //must be.. is's equals to uppers cond.
#include <stdio.h>
#include <dlfcn.h>
void ** getEBP(int dummy)
{
	void **ebp = (void **) &dummy -2;
	return (ebp);
}

void print_walk_backtrace(void)
{
	int dummy;
	int frame=0;
	Dl_info dlip;
	void ** ebp = getEBP(dummy);
	void **ret =NULL;
	printf("Stack backtrace:\n");
	while(*ebp){
		ret = ebp +1;
		if(dladdr( *ret, &dlip) == 0)
			break;
		printf("Frame %d:[ebp=0x%08x][ret =0x%08x] %s\n", frame++,
			*ebp, *ret, dlip.dli_sname);
		ebp = (void***) (*ebp);
	}
}

#endif
