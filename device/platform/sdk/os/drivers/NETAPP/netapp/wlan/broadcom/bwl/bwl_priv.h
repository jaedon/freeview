
extern bool bwl_g_swap;
/* IOCTL swapping mode for Big Endian host with Little Endian dongle.  Default to off */
#ifdef IL_BIGENDIAN
#define htod32(i) (bwl_g_swap?bcmswap32(i):i)
#define htod16(i) (bwl_g_swap?bcmswap16(i):i)
#define dtoh32(i) (bwl_g_swap?bcmswap32(i):i)
#define dtoh16(i) (bwl_g_swap?bcmswap16(i):i)
#define htodchanspec(i) htod16(i)
#define dtohchanspec(i) dtoh16(i)
#define htodenum(i) ((sizeof(i) == 4) ? htod32(i) : ((sizeof(i) == 2) ? htod16(i) : i))
#define dtohenum(i) ((sizeof(i) == 4) ? dtoh32(i) : ((sizeof(i) == 2) ? htod16(i) : i))
#else /* IL_BIGENDIAN */
#define htod32(i) i
#define htod16(i) i
#define dtoh32(i) i
#define dtoh16(i) i
#define htodchanspec(i) i
#define dtohchanspec(i) i
#define htodenum(i) i
#define dtohenum(i) i
#endif /* IL_BIGENDIAN */
