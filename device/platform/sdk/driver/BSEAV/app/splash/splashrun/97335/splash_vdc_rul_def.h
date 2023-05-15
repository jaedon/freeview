/***************************************************************************
 File : splash_vdc_rul.h
 Date  : Apr 27 2009 18:24:58
 Description : RUL list for splash BVN configuration
 ***************************************************************************/

#include "splash_magnum.h"


const uint32_t       gSplashWidth   = 720;
const uint32_t       gSplashHeight  = 483;
const uint32_t       gSplashPhysAddr = 0x043e8680; 
const uint32_t       gSplashPitch   = 1440;
const BPXL_Format    gSplashFormat  = (BPXL_Format)0x00000018;
const BFMT_VideoFmt  gVideoFormat   = (BFMT_VideoFmt)24; /* BFMT_VideoFmt_e480p */
const BFMT_VideoFmt  gVideoFormat2   = (BFMT_VideoFmt)0; /* BFMT_VideoFmt_eNTSC */

uint32_t aListCount0[] = 
{
	0,
	393,
	396,
	508,
	557,
	606,
};

uint32_t aList0[] = 
{

	/* LIST: 0 (393 entries) */
	0x01000000,
	0x10184004,
	0x0000000f,
	0x01000000,
	0x10184004,
	0x00000000,
	0x01000000,
	0x10181ec0,
	0x00000210,
	0x06000003,
	0x10181ec4,
	0x00000004,
	0x00000001,
	0x00200000,
	0x00000000,
	0x06000007,
	0x10181e48,
	0x00003fda,
	0x001ffc00,
	0x12ec0000,
	0x00420000,
	0x00000d71,
	0xe61c0c74,
	0x0000f76f,
	0xf55f1332,
	0x06000002,
	0x10181e68,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x01000000,
	0x10181e84,
	0x00000021,
	0x06000004,
	0x10181e08,
	0x00870000,
	0x001fc05e,
	0x00000018,
	0x21f07c1f,
	0x0400003b,
	0x01000000,
	0x10181e2c,
	0x00010000,
	0x01000000,
	0x10181e30,
	0x00006030,
	0x01000000,
	0x10183008,
	0x00978231,
	0x01000000,
	0x10183014,
	0x0000003f,
	0x01000000,
	0x10183058,
	0x00004000,
	0x01000000,
	0x1018305c,
	0x00004000,
	0x01000000,
	0x10183060,
	0x00004000,
	0x01000000,
	0x10183054,
	0x00000234,
	0x01000000,
	0x1018306c,
	0x00004000,
	0x01000000,
	0x10183070,
	0x00004000,
	0x01000000,
	0x10183074,
	0x00004000,
	0x01000000,
	0x10183068,
	0x00000234,
	0x06000016,
	0x10181d08,
	0x00097000,
	0x0200047e,
	0x0f03c010,
	0x0008bc00,
	0x401a0800,
	0x64482e14,
	0x000a05f3,
	0x008604d0,
	0x008cc45f,
	0x9bc348bc,
	0x000a5af0,
	0x0004902a,
	0x005fe4d9,
	0x008b2018,
	0xa16f8745,
	0x0000a00f,
	0x0004902a,
	0x005fe4d9,
	0x008b2018,
	0xa16f8745,
	0x0000a00f,
	0x000000ff,
	0x00000c63,
	0x01000000,
	0x10181d10,
	0x0f03c010,
	0x0600000c,
	0x1018180c,
	0xfdfd1400,
	0x009efcfc,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000006,
	0x03805332,
	0x0120c148,
	0x110036e5,
	0x05899b8b,
	0x00cc000c,
	0x00000000,
	0x01000000,
	0x10181850,
	0x00f102d0,
	0x06000001,
	0x1018185c,
	0x00000803,
	0x0000090a,
	0x060000ff,
	0x10181880,
	0x0020400c,
	0x00a24000,
	0x00288059,
	0x0028001b,
	0x0024000d,
	0x0020703f,
	0x00205040,
	0x00204014,
	0x00214044,
	0x00284021,
	0x0028c00f,
	0x0028c1d2,
	0x00288075,
	0x002c800b,
	0x0028e040,
	0x0028c284,
	0x00801000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00460003,
	0x00ad8003,
	0x00462001,
	0x00442005,
	0x00441006,
	0x00442006,
	0x0063700b,
	0x0063c001,
	0x00214018,
	0x0041c1e2,
	0x0021c018,
	0x00414001,
	0x0082d001,
	0x0020200f,
	0x00242010,
	0x00442006,
	0x004c1006,
	0x004c2006,
	0x004c4001,
	0x0064100b,
	0x00646001,
	0x0049c1e0,
	0x0049c001,
	0x004bc001,
	0x00815000,
	0x00444002,
	0x00442005,
	0x004c1006,
	0x004c2006,
	0x0064100b,
	0x00646001,
	0x0049c1e0,
	0x0049c001,
	0x004bc001,
	0x00815000,
	0x0083a002,
	0x00454001,
	0x00554001,
	0x0041c001,
	0x0051c001,
	0x0083f002,
	0x00414001,
	0x00514001,
	0x0041c001,
	0x0051c001,
	0x00844001,
	0x004d4001,
	0x005d4001,
	0x0049c001,
	0x0059c001,
	0x00849001,
	0x00494001,
	0x00594001,
	0x0049c001,
	0x0059c001,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00200012,
	0x00a00018,
	0x0085c004,
	0x00400801,
	0x0020005a,
	0x00657008,
	0x00400801,
	0x00859008,
	0x003000db,
	0x0020103d,
	0x00208052,
	0x0030004c,
	0x00400801,
	0x0020005a,
	0x00660008,
	0x00400801,
	0x00862010,
	0x003000db,
	0x0020103d,
	0x00208052,
	0x0030004c,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00400806,
	0x00a00064,
	0x0067b00f,
	0x00400801,
	0x00c08020,
	0x00c08020,
	0x00c01040,
	0x00c01040,
	0x004008f7,
	0x00a00064,
	0x0067b00f,
	0x00400801,
	0x00c08020,
	0x00c08020,
	0x00c01040,
	0x00c01040,
	0x004008f0,
	0x00869000,
	0x00400801,
	0x00c08020,
	0x00d01040,
	0x00000000,
	0x00400017,
	0x0068c001,
	0x00400011,
	0x0068c001,
	0x0068b009,
	0x00400019,
	0x00400022,
	0x0068c001,
	0x00400011,
	0x0068c001,
	0x0068b009,
	0x0040000f,
	0x00400011,
	0x00a00080,
	0x00504004,
	0x00000000,
	0x00400001,
	0x0020006d,
	0x00c03080,
	0x0020300b,
	0x0020c02c,
	0x0020402a,
	0x00400001,
	0x0088f000,
	0x00000000,
	0x00000000,
	0x00200035,
	0x00202036,
	0x00200018,
	0x006a6002,
	0x00400003,
	0x004000f5,
	0x0040000a,
	0x006aa002,
	0x006a6003,
	0x00400003,
	0x004000f4,
	0x0040000a,
	0x006aa002,
	0x00400001,
	0x0020003f,
	0x00202036,
	0x00300018,
	0x00400001,
	0x0020007f,
	0x00202036,
	0x00300018,
	0x00000000,
	0x00200035,
	0x00202036,
	0x00200018,
	0x006bc002,
	0x00400003,
	0x004000f5,
	0x0040000b,
	0x006c0001,
	0x006bc003,
	0x00400003,
	0x004000f4,
	0x0040000b,
	0x006c0001,
	0x00400001,
	0x0020003f,
	0x00202036,
	0x00300018,
	0x00400001,
	0x0020007f,
	0x00202036,
	0x00300018,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x0040001d,
	0x006d5001,
	0x0040000f,
	0x006d5001,
	0x006d400b,
	0x0040001b,
	0x00400026,
	0x006d5001,
	0x0040000f,
	0x006d5001,
	0x006d400b,
	0x00400013,
	0x0040000f,
	0x00a00080,
	0x00504002,
	0x00400001,
	0x0020006d,
	0x00c03080,
	0x0020300b,
	0x0020c02c,
	0x0020402a,
	0x00400001,
	0x008d7000,
	0x00000000,
	0x00000000,
	0x00200035,
	0x00202036,
	0x00200018,
	0x006ee002,
	0x00400003,
	0x004000f5,
	0x00400009,
	0x006f2003,
	0x006ee003,
	0x00400003,
	0x004000f4,
	0x00400009,
	0x006f2003,
	0x00400001,
	0x0020003f,
	0x00202036,
	0x00300018,
	0x00400001,
	0x0020007f,
	0x00202036,
	0x00300018,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x008fd000,
	0x00083007,
	0x1cea2c9d,
	0x01000000,
	0x10102880,
	0x00000001,
	0x01000000,
	0x10181808,
	0x0201030b,
	0x01000000,
	0x10181828,
	0x03805332,

	/* LIST: 4 (3 entries) */
	0x01000000,
	0x101420c0,
	0xbaadf00d,

	/* LIST: 6 (112 entries) */
	0x01000000,
	0x10186004,
	0x00000001,
	0x01000000,
	0x10186010,
	0x00000000,
	0x01000000,
	0x10146400,
	0x00000200,
	0x01000000,
	0x10146400,
	0x00000000,
	0x01000000,
	0x101425d0,
	0x00000013,
	0x01000000,
	0x10142500,
	0x00000100,
	0x06000001,
	0x10142408,
	0x02d000f1,
	0x001cb876,
	0x01000000,
	0x10142410,
	0x0002ff34,
	0x01000000,
	0x10142414,
	0x000f0000,
	0x01000000,
	0x101424cc,
	0x00000000,
	0x01000000,
	0x10142434,
	0x00000000,
	0x06000003,
	0x101424a0,
	0x02d000f1,
	0x00000000,
	0x02d000f1,
	0x00000000,
	0x01000000,
	0x101424b0,
	0x00000009,
	0x01000000,
	0x1014145c,
	0x02d000f1,
	0x06000003,
	0x10141428,
	0x00000000,
	0x00000000,
	0x00000000,
	0x0000ff00,
	0x01000000,
	0x10141420,
	0x00000000,
	0x06000003,
	0x101414f0,
	0x00000000,
	0x10000000,
	0x00000000,
	0x08000000,
	0x01000000,
	0x10141418,
	0x00100000,
	0x01000000,
	0x1014141c,
	0x00000000,
	0x06000003,
	0x10141438,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10141440,
	0x043e8680,
	0x01000000,
	0x10141448,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x06000001,
	0x10141410,
	0x00000565,
	0x000b0500,
	0x0600000e,
	0x10141500,
	0x00000839,
	0x00001021,
	0x00000323,
	0x00000200,
	0x00000000,
	0x0000fb44,
	0x0000f6b0,
	0x00000e0c,
	0x00001000,
	0x00000000,
	0x00000e0c,
	0x0000f439,
	0x0000fdba,
	0x00001000,
	0x00000000,
	0x01000000,
	0x1014140c,
	0x00000010,
	0x01000000,
	0x10141404,
	0x00000001,
	0x01000000,
	0x10142404,
	0x00000001,

	/* LIST: 10 (49 entries) */
	0x01000000,
	0x10186010,
	0x00000000,
	0x06000001,
	0x10142408,
	0x02d000f1,
	0x001cb876,
	0x01000000,
	0x10142410,
	0x0002ff34,
	0x01000000,
	0x10142414,
	0x000f0000,
	0x01000000,
	0x101424cc,
	0x00000000,
	0x06000003,
	0x101424a0,
	0x02d000f1,
	0x00000000,
	0x02d000f1,
	0x00000000,
	0x01000000,
	0x101424b0,
	0x00000009,
	0x06000003,
	0x10141438,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10141440,
	0x043e8680,
	0x01000000,
	0x10141448,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x01000000,
	0x1014140c,
	0x00000010,
	0x01000000,
	0x10141404,
	0x00000001,
	0x01000000,
	0x10142404,
	0x00000001,

	/* LIST: 15 (49 entries) */
	0x01000000,
	0x10186010,
	0x00000000,
	0x06000001,
	0x10142408,
	0x02d000f1,
	0x001cb876,
	0x01000000,
	0x10142410,
	0x0002ff34,
	0x01000000,
	0x10142414,
	0x000f0000,
	0x01000000,
	0x101424cc,
	0x00000000,
	0x06000003,
	0x101424a0,
	0x02d000f1,
	0x00000000,
	0x02d000f1,
	0x00000000,
	0x01000000,
	0x101424b0,
	0x00000009,
	0x06000003,
	0x10141438,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10141440,
	0x043e8680,
	0x01000000,
	0x10141448,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x01000000,
	0x1014140c,
	0x00000010,
	0x01000000,
	0x10141404,
	0x00000001,
	0x01000000,
	0x10142404,
	0x00000001,
};

uint32_t aListCount1[] = 
{
	0,
	52,
	101,
	150,
};

uint32_t aList1[] = 
{

	/* LIST: 8 (52 entries) */
	0x01000000,
	0x10186010,
	0x00000000,
	0x06000001,
	0x10142408,
	0x02d000f1,
	0x001cb876,
	0x01000000,
	0x10142410,
	0x0002ff34,
	0x01000000,
	0x10142414,
	0x000f0000,
	0x01000000,
	0x101424cc,
	0x00000000,
	0x01000000,
	0x10142434,
	0x00000000,
	0x06000003,
	0x101424a0,
	0x02d000f1,
	0x00000000,
	0x02d000f1,
	0x00000000,
	0x01000000,
	0x101424b0,
	0x00000009,
	0x06000003,
	0x10141438,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10141440,
	0x043e8c20,
	0x01000000,
	0x10141448,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x01000000,
	0x1014140c,
	0x00000010,
	0x01000000,
	0x10141404,
	0x00000001,
	0x01000000,
	0x10142404,
	0x00000001,

	/* LIST: 12 (49 entries) */
	0x01000000,
	0x10186010,
	0x00000000,
	0x06000001,
	0x10142408,
	0x02d000f1,
	0x001cb876,
	0x01000000,
	0x10142410,
	0x0002ff34,
	0x01000000,
	0x10142414,
	0x000f0000,
	0x01000000,
	0x101424cc,
	0x00000000,
	0x06000003,
	0x101424a0,
	0x02d000f1,
	0x00000000,
	0x02d000f1,
	0x00000000,
	0x01000000,
	0x101424b0,
	0x00000009,
	0x06000003,
	0x10141438,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10141440,
	0x043e8c20,
	0x01000000,
	0x10141448,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x01000000,
	0x1014140c,
	0x00000010,
	0x01000000,
	0x10141404,
	0x00000001,
	0x01000000,
	0x10142404,
	0x00000001,

	/* LIST: 17 (49 entries) */
	0x01000000,
	0x10186010,
	0x00000000,
	0x06000001,
	0x10142408,
	0x02d000f1,
	0x001cb876,
	0x01000000,
	0x10142410,
	0x0002ff34,
	0x01000000,
	0x10142414,
	0x000f0000,
	0x01000000,
	0x101424cc,
	0x00000000,
	0x06000003,
	0x101424a0,
	0x02d000f1,
	0x00000000,
	0x02d000f1,
	0x00000000,
	0x01000000,
	0x101424b0,
	0x00000009,
	0x06000003,
	0x10141438,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10141440,
	0x043e8c20,
	0x01000000,
	0x10141448,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x01000000,
	0x1014140c,
	0x00000010,
	0x01000000,
	0x10141404,
	0x00000001,
	0x01000000,
	0x10142404,
	0x00000001,
};

uint32_t aListCount2[] = 
{
	0,
	422,
	425,
	567,
	625,
	1203,
	1206,
	1249,
	1304,
};

uint32_t aList2[] = 
{

	/* LIST: 1 (422 entries) */
	0x01000000,
	0x10184008,
	0x0000000f,
	0x01000000,
	0x10184008,
	0x00000000,
	0x01000000,
	0x10184008,
	0x00000030,
	0x01000000,
	0x10184008,
	0x00000000,
	0x01000000,
	0x101806c0,
	0x00000211,
	0x06000003,
	0x101806c4,
	0x000003e9,
	0x03e80000,
	0x00200831,
	0x00000000,
	0x06000007,
	0x10180808,
	0x00003fda,
	0x001ffc00,
	0x14050000,
	0xfd830000,
	0x00001392,
	0xec71fffe,
	0x00000003,
	0xec6d1393,
	0x01000000,
	0x10180844,
	0x00000021,
	0x06000004,
	0x10180608,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x01000000,
	0x1018062c,
	0x00010000,
	0x01000000,
	0x10180630,
	0x00006030,
	0x01000000,
	0x10183008,
	0x00978231,
	0x01000000,
	0x10183014,
	0x0000003f,
	0x01000000,
	0x10183058,
	0x00004000,
	0x01000000,
	0x1018305c,
	0x00004000,
	0x01000000,
	0x10183060,
	0x00004000,
	0x01000000,
	0x10183054,
	0x00000234,
	0x01000000,
	0x1018306c,
	0x00004000,
	0x01000000,
	0x10183070,
	0x00004000,
	0x01000000,
	0x10183074,
	0x00004000,
	0x01000000,
	0x10183068,
	0x00000234,
	0x01000000,
	0x1018070c,
	0x0200080e,
	0x06000016,
	0x10180508,
	0x000df14a,
	0x0200080e,
	0x0fb3ec10,
	0x0003ac00,
	0x15020000,
	0x15020000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x000000ff,
	0x00000c63,
	0x01000000,
	0x1018050c,
	0x0200080e,
	0x06000016,
	0x10180708,
	0x000df14a,
	0x0200080e,
	0x0fb3ec10,
	0x0003ac00,
	0x15020000,
	0x15020000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x000000ff,
	0x00000c63,
	0x0600000c,
	0x1018000c,
	0xfbfb1400,
	0x00fbfcfc,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000006,
	0x03805332,
	0x0120c048,
	0x110036e5,
	0x05899b8b,
	0x04cc000c,
	0x00000000,
	0x01000000,
	0x10180050,
	0x01e302d0,
	0x06000001,
	0x1018005c,
	0x00000803,
	0x00000800,
	0x060000ff,
	0x10180080,
	0x00a20000,
	0x0028c2a1,
	0x00288024,
	0x00a04000,
	0x00240010,
	0x0020703e,
	0x00204026,
	0x00800000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00a88000,
	0x004c2001,
	0x00242018,
	0x00442005,
	0x0024000f,
	0x00441006,
	0x0024100f,
	0x0044201e,
	0x0040c1e3,
	0x0022c018,
	0x00814000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00400801,
	0x0020003a,
	0x0020101e,
	0x00208028,
	0x00200012,
	0x0020101e,
	0x00208028,
	0x00200012,
	0x0020101e,
	0x00208028,
	0x00200012,
	0x0020101e,
	0x00208028,
	0x00200012,
	0x00828000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x0040000c,
	0x00644008,
	0x004001f8,
	0x00840000,
	0x0020900b,
	0x00509001,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x008fd000,
	0x00013105,
	0xbd6fe5b4,
	0x01000000,
	0x10102888,
	0x00000001,
	0x01000000,
	0x10180008,
	0x0201030b,
	0x01000000,
	0x10180028,
	0x03805332,

	/* LIST: 2 (3 entries) */
	0x01000000,
	0x101420c0,
	0xbaadf00d,

	/* LIST: 7 (142 entries) */
	0x01000000,
	0x10188004,
	0x00000001,
	0x01000000,
	0x10188010,
	0x00000000,
	0x01000000,
	0x10146400,
	0x00000100,
	0x01000000,
	0x10146400,
	0x00000000,
	0x01000000,
	0x101421d0,
	0x00000013,
	0x01000000,
	0x10142100,
	0x00000100,
	0x01000000,
	0x10144034,
	0x00000000,
	0x01000000,
	0x10144038,
	0x00000000,
	0x01000000,
	0x10144010,
	0x00000000,
	0x01000000,
	0x10144014,
	0x00000000,
	0x01000000,
	0x1014a404,
	0x00000000,
	0x01000000,
	0x1014a408,
	0x00000000,
	0x01000000,
	0x1014a40c,
	0x00080008,
	0x01000000,
	0x1014a410,
	0x00001111,
	0x01000000,
	0x1014a414,
	0x04040404,
	0x01000000,
	0x1014a418,
	0x04040404,
	0x06000001,
	0x10142008,
	0x02d001e3,
	0x001cb876,
	0x01000000,
	0x10142010,
	0x0002ff34,
	0x01000000,
	0x10142014,
	0x000f0000,
	0x01000000,
	0x101420cc,
	0x00000000,
	0x01000000,
	0x10142034,
	0x00000000,
	0x06000003,
	0x101420a0,
	0x02d001e3,
	0x00000000,
	0x02d001e3,
	0x00000000,
	0x01000000,
	0x101420b0,
	0x00000009,
	0x01000000,
	0x1014105c,
	0x02d001e3,
	0x06000003,
	0x10141028,
	0x00000000,
	0x00000000,
	0x00000000,
	0x0000ff00,
	0x01000000,
	0x10141020,
	0x00000000,
	0x06000003,
	0x101410f0,
	0x00000000,
	0x10000000,
	0x00000000,
	0x08000000,
	0x01000000,
	0x10141018,
	0x00100000,
	0x01000000,
	0x1014101c,
	0x00000000,
	0x06000003,
	0x10141038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x000005a0,
	0x01000000,
	0x10141040,
	0x043e8680,
	0x01000000,
	0x10141048,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x06000001,
	0x10141010,
	0x00000565,
	0x000b0500,
	0x0600000e,
	0x10141100,
	0x00000839,
	0x00001021,
	0x00000323,
	0x00000200,
	0x00000000,
	0x0000fb44,
	0x0000f6b0,
	0x00000e0c,
	0x00001000,
	0x00000000,
	0x00000e0c,
	0x0000f439,
	0x0000fdba,
	0x00001000,
	0x00000000,
	0x01000000,
	0x1014100c,
	0x00000010,
	0x01000000,
	0x10141004,
	0x00000001,
	0x01000000,
	0x10142004,
	0x00000001,

	/* LIST: 9 (58 entries) */
	0x01000000,
	0x10188010,
	0x00000000,
	0x01000000,
	0x10144080,
	0x00000001,
	0x01000000,
	0x10144084,
	0x00000001,
	0x01000000,
	0x10144018,
	0x00000001,
	0x01000000,
	0x1014401c,
	0x00000001,
	0x06000001,
	0x10142008,
	0x02d001e3,
	0x001cb876,
	0x01000000,
	0x10142010,
	0x0002ff34,
	0x01000000,
	0x10142014,
	0x000f0000,
	0x01000000,
	0x101420cc,
	0x00000000,
	0x01000000,
	0x10142034,
	0x00000000,
	0x06000003,
	0x101420a0,
	0x02d001e3,
	0x00000000,
	0x02d001e3,
	0x00000000,
	0x01000000,
	0x101420b0,
	0x00000009,
	0x01000000,
	0x10141040,
	0x043e8680,
	0x01000000,
	0x10141048,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x01000000,
	0x1014100c,
	0x00000010,
	0x01000000,
	0x10141004,
	0x00000001,
	0x01000000,
	0x10142004,
	0x00000001,

	/* LIST: 11 (578 entries) */
	0x01000000,
	0x10184008,
	0x0000000f,
	0x01000000,
	0x10184008,
	0x00000000,
	0x01000000,
	0x10184008,
	0x00000030,
	0x01000000,
	0x10184008,
	0x00000000,
	0x01000000,
	0x101806c0,
	0x00000211,
	0x06000003,
	0x101806c4,
	0x000003e9,
	0x03e80000,
	0x00200831,
	0x00000000,
	0x01000000,
	0x101a0300,
	0x00000241,
	0x01000000,
	0x101a0304,
	0x0000008f,
	0x01000000,
	0x101a0308,
	0x00230005,
	0x01000000,
	0x101a0314,
	0x0a028f5c,
	0x01000000,
	0x101a0318,
	0x02000000,
	0x01000000,
	0x101a02d0,
	0x03480412,
	0x06000007,
	0x10180808,
	0x00003fda,
	0x001ffc00,
	0x14050000,
	0xfd830000,
	0x00001392,
	0xec71fffe,
	0x00000003,
	0xec6d1393,
	0x01000000,
	0x10180844,
	0x00000021,
	0x06000004,
	0x10180608,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x01000000,
	0x1018062c,
	0x00010000,
	0x01000000,
	0x10180630,
	0x00006030,
	0x01000000,
	0x1018304c,
	0x00000001,
	0x01000000,
	0x10184038,
	0x00000000,
	0x01000000,
	0x10183008,
	0x00978231,
	0x01000000,
	0x10183014,
	0x0000003f,
	0x01000000,
	0x10183058,
	0x00004000,
	0x01000000,
	0x1018305c,
	0x00004000,
	0x01000000,
	0x10183060,
	0x00004000,
	0x01000000,
	0x10183054,
	0x00000234,
	0x01000000,
	0x1018306c,
	0x00004000,
	0x01000000,
	0x10183070,
	0x00004000,
	0x01000000,
	0x10183074,
	0x00004000,
	0x01000000,
	0x10183068,
	0x00000234,
	0x01000000,
	0x10184010,
	0x00000002,
	0x01000000,
	0x10184010,
	0x00000000,
	0x06000007,
	0x10181048,
	0x00003fda,
	0x001ffc00,
	0x20000000,
	0x00000000,
	0x00002000,
	0x00000000,
	0x00000000,
	0x00002000,
	0x06000002,
	0x10181068,
	0x80100401,
	0x00000610,
	0x000ef83d,
	0x01000000,
	0x10184010,
	0x00000005,
	0x01000000,
	0x10184010,
	0x00000000,
	0x01000000,
	0x10184038,
	0x00000000,
	0x01000000,
	0x10184044,
	0x00000003,
	0x0600003f,
	0x10182180,
	0x0065b005,
	0x0064f001,
	0x00653005,
	0x00657001,
	0x0065b01d,
	0x0065f001,
	0x006631df,
	0x00667001,
	0x0065b002,
	0x0064b001,
	0x00840000,
	0x002552d0,
	0x00254010,
	0x002c403e,
	0x003d4026,
	0x002552d0,
	0x00254010,
	0x0020403e,
	0x00314031,
	0x002152d0,
	0x00214010,
	0x0020403e,
	0x00314031,
	0x002152d0,
	0x00214010,
	0x0024403e,
	0x00354031,
	0x002552d0,
	0x00254010,
	0x0024403e,
	0x00354031,
	0x002512d0,
	0x00250010,
	0x0024003e,
	0x00352031,
	0x002732d0,
	0x00252010,
	0x0024203e,
	0x00352031,
	0x002732d0,
	0x00252010,
	0x0024003e,
	0x00350031,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00041609,
	0x9f83fb91,
	0x01000000,
	0x10181010,
	0x00000249,
	0x01000000,
	0x10181014,
	0x000001d9,
	0x01000000,
	0x10181018,
	0x00000003,
	0x01000000,
	0x1018101c,
	0x00000040,
	0x01000000,
	0x10181020,
	0x01e302d0,
	0x01000000,
	0x1018108c,
	0x00000000,
	0x01000000,
	0x1018102c,
	0x00000003,
	0x01000000,
	0x10181030,
	0x00000000,
	0x01000000,
	0x10181008,
	0x00000fc3,
	0x01000000,
	0x10181090,
	0x01008000,
	0x01000000,
	0x1018070c,
	0x0200080e,
	0x06000016,
	0x10180508,
	0x000df14a,
	0x0200080e,
	0x0fb3ec10,
	0x0003ac00,
	0x15020000,
	0x15020000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x000000ff,
	0x00000c63,
	0x01000000,
	0x1018050c,
	0x0200080e,
	0x06000016,
	0x10180708,
	0x000df14a,
	0x0200080e,
	0x0fb3ec10,
	0x0003ac00,
	0x15020000,
	0x15020000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00030000,
	0x000000ff,
	0x00000c63,
	0x0600000c,
	0x1018000c,
	0xfbfb1400,
	0x00fbfcfc,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000006,
	0x03805332,
	0x0120c048,
	0x110036e5,
	0x05899b8b,
	0x04cc000c,
	0x00000000,
	0x01000000,
	0x10180050,
	0x01e302d0,
	0x06000001,
	0x1018005c,
	0x00000803,
	0x00000800,
	0x060000ff,
	0x10180080,
	0x00a20000,
	0x0028c2a1,
	0x00288024,
	0x00a04000,
	0x00240010,
	0x0020703e,
	0x00204026,
	0x00800000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00a88000,
	0x004c2001,
	0x00242018,
	0x00442005,
	0x0024000f,
	0x00441006,
	0x0024100f,
	0x0044201e,
	0x0040c1e3,
	0x0022c018,
	0x00814000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00400801,
	0x0020003a,
	0x0020101e,
	0x00208028,
	0x00200012,
	0x0020101e,
	0x00208028,
	0x00200012,
	0x0020101e,
	0x00208028,
	0x00200012,
	0x0020101e,
	0x00208028,
	0x00200012,
	0x00828000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x0040000c,
	0x00644008,
	0x004001f8,
	0x00840000,
	0x0020900b,
	0x00509001,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x008fd000,
	0x00013105,
	0xbd6fe5b4,
	0x01000000,
	0x10182008,
	0x0000000b,
	0x01000000,
	0x10102888,
	0x00000001,
	0x01000000,
	0x10180008,
	0x0201030b,
	0x01000000,
	0x10180028,
	0x03805332,

	/* LIST: 13 (3 entries) */
	0x01000000,
	0x101420c0,
	0xbaadf00d,

	/* LIST: 16 (43 entries) */
	0x01000000,
	0x10188010,
	0x00000000,
	0x06000001,
	0x10142008,
	0x02d001e3,
	0x001cb876,
	0x01000000,
	0x10142010,
	0x0002ff34,
	0x01000000,
	0x10142014,
	0x000f0000,
	0x01000000,
	0x101420cc,
	0x00000000,
	0x06000003,
	0x101420a0,
	0x02d001e3,
	0x00000000,
	0x02d001e3,
	0x00000000,
	0x01000000,
	0x101420b0,
	0x00000009,
	0x01000000,
	0x10141040,
	0x043e8680,
	0x01000000,
	0x10141048,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x01000000,
	0x1014100c,
	0x00000010,
	0x01000000,
	0x10141004,
	0x00000001,
	0x01000000,
	0x10142004,
	0x00000001,

	/* LIST: 18 (55 entries) */
	0x01000000,
	0x10188010,
	0x00000000,
	0x01000000,
	0x10144080,
	0x00000001,
	0x01000000,
	0x10144084,
	0x00000001,
	0x01000000,
	0x10144018,
	0x00000001,
	0x01000000,
	0x1014401c,
	0x00000001,
	0x06000001,
	0x10142008,
	0x02d001e3,
	0x001cb876,
	0x01000000,
	0x10142010,
	0x0002ff34,
	0x01000000,
	0x10142014,
	0x000f0000,
	0x01000000,
	0x101420cc,
	0x00000000,
	0x06000003,
	0x101420a0,
	0x02d001e3,
	0x00000000,
	0x02d001e3,
	0x00000000,
	0x01000000,
	0x101420b0,
	0x00000009,
	0x01000000,
	0x10141040,
	0x043e8680,
	0x01000000,
	0x10141048,
	0x00000000,
	0x04000001,
	0x00000000,
	0x11001000,
	0x01000000,
	0x1014100c,
	0x00000010,
	0x01000000,
	0x10141004,
	0x00000001,
	0x01000000,
	0x10142004,
	0x00000001,
};

uint32_t aListCount3[] = 
{
	0,
	3,
};

uint32_t aList3[] = 
{

	/* LIST: 14 (3 entries) */
	0x01000000,
	0x101420c0,
	0xbaadf00d,
};


struct stTriggerMap
{
	int TriggerHwNum ;             /* The hardware position of the trigger */
	int SlotNum ;                  /* The RDMA slot correposponding to the trigger */
	int ListCountMaxIndex ;       /* The Max Index of the ListCount Array */
	uint32_t *aListCountArray ; /* The list count array pointer */
	uint32_t *aListArray ;      /* The list array pointer */
	} aTriggerMap[] = 
{
 { 8, 2, 5, aListCount0, aList0 },
 { 9, 3, 3, aListCount1, aList1 },
 { 10, 0, 8, aListCount2, aList2 },
 { 11, 1, 1, aListCount3, aList3 },
 { -1, -1, 0, NULL, NULL },
 { -1, -1, 0, NULL, NULL },
 { -1, -1, 0, NULL, NULL },
 { -1, -1, 0, NULL, NULL },

};

