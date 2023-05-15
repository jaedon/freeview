/* This File is a temporary file for developing irdeto loader version 5
 * Then, It would be only in develompment period, it shall be deleted on expired time. jichoi.
*/
#ifndef ___IRV5_DEV_H___
#define ___IRV5_DEV_H___


#define ir_getbits32(q) (HUINT32)(((*(HUINT8 *)(q)) << 24) | (*((HUINT8 *)(q)+1) << 16) | (*((HUINT8 *)(q)+2) << 8) | (*((HUINT8 *)(q)+3)) )
#define ir_getbits16(q) (HUINT16)(((*(HUINT8 *)(q)) << 8 ) | (*((HUINT8 *)(q)+1)) )

#define IRV5_LOG_PRINT
#if defined(CONFIG_DEBUG) && defined(IRV5_LOG_PRINT)
#define not_yet 	printf("\033[""33""m""[%s:%d] not implemtented yet""\033[0m""\n", __FUNCTION__, __LINE__ )
#define dev_going 	printf("\033[""32""m""[%s:%d] Developing. ""\033[0m""\n", __FUNCTION__, __LINE__)
#define jichoi(...)		printf("\033[1;33;40m""jichoi:[%s:%d], ",__FUNCTION__,__LINE__);printf(__VA_ARGS__);printf("\033[00m")
#define dbgtrc			printf("lc:trc:%s:%d\n",__FUNCTION__, __LINE__)
#define dbgdump			cas_ir_dbg_api_dump

#define print_ir_log(...) 		printf("\033[1;36;40m""ir:dbg:[%s:%d],",__FUNCTION__,__LINE__);printf(__VA_ARGS__);printf("\033[00m")
#define print_ir_warn(...)		printf("\033[1;31;40m""ir:warn:[%s:%d],",__FUNCTION__,__LINE__);printf(__VA_ARGS__);printf("\033[00m")
#define print_ir_err(...)		printf("\033[1;31;40m""ir:err:[%s:%d],",__FUNCTION__,__LINE__);printf(__VA_ARGS__);printf("\033[00m")

#define dbgprt0(fmt) 						print_ir_log(fmt)
#define dbgprt1(fmt, arg1) 					print_ir_log(fmt, arg1)
#define dbgprt2(fmt, arg1, arg2) 				print_ir_log(fmt, arg1, arg2)
#define dbgprt3(fmt, arg1, arg2, arg3) 		print_ir_log(fmt, arg1, arg2, arg3)
#define dbgprt4(fmt, arg1, arg2, arg3, arg4)	print_ir_log(fmt, arg1, arg2, arg3, arg4)

#define dbgwarn0(fmt) 						print_ir_warn(fmt)
#define dbgwarn1(fmt, arg1) 					print_ir_warn(fmt, arg1)
#define dbgwarn2(fmt, arg1, arg2) 			print_ir_warn(fmt, arg1, arg2)
#define dbgwarn3(fmt, arg1, arg2, arg3) 		print_ir_warn(fmt, arg1, arg2, arg3)
#define dbgwarn4(fmt, arg1, arg2, arg3, arg4)	print_ir_warn(fmt, arg1, arg2, arg3, arg4)

#define dbgerr0(fmt) 						print_ir_err(fmt)
#define dbgerr1(fmt, arg1) 					print_ir_err(fmt, arg1)
#define dbgerr2(fmt, arg1, arg2) 				print_ir_err(fmt, arg1, arg2)
#define dbgerr3(fmt, arg1, arg2, arg3) 		print_ir_err(fmt, arg1, arg2, arg3)
#define dbgerr4(fmt, arg1, arg2, arg3, arg4)	print_ir_err(fmt, arg1, arg2, arg3, arg4)

#define dbgassert(c)		\
	if(c==0) \
	do{ \
		dbgwarn0("Homing-Assertion(SegV1-Kuma) was occured\n"); \
		{volatile int* a = (int*)(0); *a = 1; }\
	}while(1)
#else // CONFIG_DEBUG }}

#define not_yet
#define dev_going
#define jichoi(...)
#define dbgtrc
#define dbgdump(...)

#define print_ir_log(...)
#define print_ir_warn(...)
#define print_ir_err(...)

#define dbgprt0(...)
#define dbgprt1(...)
#define dbgprt2(...)
#define dbgprt3(...)
#define dbgprt4(...)

#define dbgwarn0(...)
#define dbgwarn1(...)
#define dbgwarn2(...)
#define dbgwarn3(...)
#define dbgwarn4(...)

#define dbgerr0(...)
#define dbgerr1(...)
#define dbgerr2(...)
#define dbgerr3(...)
#define dbgerr4(...)

#define dbgassert(c)	(void)(c)

#endif

#define USE_ONLY_LOADER
#undef USE_ONLY_LOADER

//declare specific operation.
//#define V5_DEV_BBCB_FORCED_WRITE
//#define V5_DEV_ERASE_DRV_UK // when "CONFIG_DEBUG was declared and the CD5 file name has "MAN" , erase V5_DEV_ERASE_DRV_UK
//#define V5_DEV_LOADER_PT_NON_CRYPT // make non-crypt partion.
#define V5_DEV_CREATE_LOADERPT //When detect invalid state on LoaderPT , Create LOADER PT
//#define V5_DEV_PERMIT_SAME_VERSION_MODULE
//#define V5_USE_USB_DNBUFFER_WITH_PALHDFBUFFER
//#define V5_DEV_ESTIMATE_COMPLEXITY
#define V5_DEV_USE_FP_ONEKEY


 typedef enum _lc_download_type
{

    /** (0x00) No download required. */
    LCH_NONE_DOWNLOAD           = 0x00,

    /** (0x01) Serial download (e.g. UART, USB Serial, Flash Disk) */
    LCH_SERIAL_DOWNLOAD         = 0x01,

    /** (0x02) USB download */
    LCH_USB_DOWNLOAD            = 0x02,

    /** (0x03) OTA download */
    LCH_OTA_DOWNLOAD            = 0x03

} lc_download_type;

#endif

