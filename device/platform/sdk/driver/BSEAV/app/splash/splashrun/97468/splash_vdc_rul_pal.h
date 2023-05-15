/***************************************************************************
 File : splash_vdc_rul.h
 Date  : Apr 30 2010 16:13:05
 Description : RUL list for splash BVN configuration
 ***************************************************************************/

#include "splash_magnum.h"


const uint32_t       gSplashWidth   = 720;
const uint32_t       gSplashHeight  = 576;
const uint32_t       gSplashPhysAddr = 0x043e8680; 
const uint32_t       gSplashPitch   = 1440;
const BPXL_Format    gSplashFormat  = (BPXL_Format)0x06e40565; /* BPXL_eR5_G6_B5 */
const BFMT_VideoFmt  gVideoFormat   = (BFMT_VideoFmt)7; /* BFMT_VideoFmt_ePAL_G */

uint32_t aListCount0[] = 
{
	0,
	564,
	567,
	735,
	1531,
	1534,
	1598,
	1645,
	1692,
};

uint32_t aList0[] = 
{

	/* LIST: 0 (564 entries) */
	0x01000000,
	0x1048441c,
	0x00000000,
	0x01000000,
	0x10484420,
	0x00000001,
	0x01000000,
	0x10484428,
	0x00000000,
	0x01000000,
	0x10484430,
	0x00000004,
	0x01000000,
	0x10484418,
	0x00000002,
	0x01000000,
	0x10484424,
	0x00000002,
	0x01000000,
	0x1048442c,
	0x00000002,
	0x01000000,
	0x10484440,
	0x00000001,
	0x01000000,
	0x10484440,
	0x00000000,
	0x01000000,
	0x10484448,
	0x00000001,
	0x01000000,
	0x10484448,
	0x00000000,
	0x01000000,
	0x10484454,
	0x00000001,
	0x01000000,
	0x10484454,
	0x00000000,
	0x01000000,
	0x1048445c,
	0x00000001,
	0x01000000,
	0x1048445c,
	0x00000000,
	0x01000000,
	0x10484430,
	0x00000000,
	0x01000000,
	0x10484418,
	0x00000000,
	0x01000000,
	0x10484424,
	0x00000000,
	0x01000000,
	0x1048442c,
	0x00000001,
	0x0600000c,
	0x1048100c,
	0xfdfd1400,
	0x00fdfdfd,
	0x00000000,
	0x00000000,
	0x00000000,
	0xffff0000,
	0x00000006,
	0x03805332,
	0x0120c148,
	0x110036e5,
	0x05899b8b,
	0x00cc000c,
	0x00000000,
	0x01000000,
	0x10481050,
	0x012002d0,
	0x06000001,
	0x1048105c,
	0x00001803,
	0x0000193c,
	0x060000ff,
	0x10481400,
	0x0020400c,
	0x00a24000,
	0x00288058,
	0x00240028,
	0x00207040,
	0x00205040,
	0x00204018,
	0x0021403e,
	0x00204039,
	0x0028400d,
	0x0028c00f,
	0x0028c1b5,
	0x00288075,
	0x002c800b,
	0x0028e040,
	0x0028c28a,
	0x00801000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00462003,
	0x00ad8000,
	0x00461001,
	0x00441004,
	0x00442005,
	0x00444002,
	0x00454022,
	0x00254018,
	0x0041c23c,
	0x0040c002,
	0x0020c018,
	0x004c2005,
	0x004c1005,
	0x004c2005,
	0x004c4001,
	0x004d4022,
	0x002d4018,
	0x0049c23c,
	0x0048c002,
	0x00404001,
	0x00204018,
	0x00442004,
	0x00462001,
	0x00a98000,
	0x00461001,
	0x00441004,
	0x00442005,
	0x00454024,
	0x00254018,
	0x0041c23c,
	0x0041c002,
	0x0021c018,
	0x004c2005,
	0x004c1005,
	0x004c2005,
	0x004c4003,
	0x004d4020,
	0x002d4018,
	0x0049c23e,
	0x00404001,
	0x00204018,
	0x00442004,
	0x00462001,
	0x00815000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00200012,
	0x00a00006,
	0x0085f001,
	0x00400001,
	0x00200101,
	0x0065a008,
	0x00000000,
	0x00000000,
	0x0020000b,
	0x00400801,
	0x0085c002,
	0x0030007a,
	0x00200019,
	0x0020102d,
	0x00308034,
	0x00400001,
	0x002000c5,
	0x00666008,
	0x00000000,
	0x00000000,
	0x0020000b,
	0x00400801,
	0x00868004,
	0x0030009a,
	0x00200035,
	0x00201031,
	0x00308034,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00400806,
	0x00a00019,
	0x0068000f,
	0x00400801,
	0x00c08008,
	0x00c08008,
	0x00c01010,
	0x00c01010,
	0x00400928,
	0x00a00019,
	0x0068000f,
	0x00400801,
	0x00c08008,
	0x00c08008,
	0x00c01010,
	0x00c01010,
	0x00400923,
	0x0086e000,
	0x00400801,
	0x00c08008,
	0x00d01010,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00400003,
	0x00400126,
	0x0040000b,
	0x006a0002,
	0x0069c002,
	0x00400003,
	0x00400127,
	0x0040000b,
	0x006a0002,
	0x0069c002,
	0x00400003,
	0x00400126,
	0x0040000b,
	0x006a0002,
	0x0069c002,
	0x00400003,
	0x00400127,
	0x0040000b,
	0x006a0002,
	0x0069c002,
	0x00887000,
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
	0x00400003,
	0x00400126,
	0x00400009,
	0x006c3004,
	0x006bf002,
	0x00400003,
	0x00400127,
	0x00400009,
	0x006c3004,
	0x006bf002,
	0x00400003,
	0x00400126,
	0x00400009,
	0x006c3004,
	0x006bf002,
	0x00400003,
	0x00400127,
	0x00400009,
	0x006c3004,
	0x006bf002,
	0x008aa000,
	0x00400001,
	0x0020003f,
	0x00202036,
	0x00300018,
	0x00400001,
	0x0020007f,
	0x00202036,
	0x00300018,
	0x00000000,
	0x00400003,
	0x00400126,
	0x0040000a,
	0x006e1003,
	0x006dd002,
	0x00400003,
	0x00400127,
	0x0040000a,
	0x006e1003,
	0x006dd002,
	0x00400003,
	0x00400126,
	0x0040000a,
	0x006e1003,
	0x006dd002,
	0x00400003,
	0x00400127,
	0x0040000a,
	0x006e1003,
	0x006dd002,
	0x008c8000,
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
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
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
	0x00051409,
	0xce3120ae,
	0x06000003,
	0x10483604,
	0x00000004,
	0x00000001,
	0x00200000,
	0x00000000,
	0x01000000,
	0x10480018,
	0x00000000,
	0x01000000,
	0x1048441c,
	0x00000002,
	0x01000000,
	0x10484420,
	0x00000002,
	0x01000000,
	0x10484428,
	0x00000002,
	0x01000000,
	0x1048444c,
	0x00000001,
	0x01000000,
	0x1048444c,
	0x00000000,
	0x01000000,
	0x10484450,
	0x00000001,
	0x01000000,
	0x10484450,
	0x00000000,
	0x01000000,
	0x10484458,
	0x00000001,
	0x01000000,
	0x10484458,
	0x00000000,
	0x01000000,
	0x1048441c,
	0x00000000,
	0x01000000,
	0x10484420,
	0x00000001,
	0x01000000,
	0x10484428,
	0x00000000,
	0x01000000,
	0x10480034,
	0x00000234,
	0x01000000,
	0x10480004,
	0x00001001,
	0x01000000,
	0x10480038,
	0x00004000,
	0x01000000,
	0x10480008,
	0x00001007,
	0x01000000,
	0x1048003c,
	0x00002000,
	0x01000000,
	0x1048000c,
	0x00001007,
	0x01000000,
	0x10480040,
	0x00002000,
	0x01000000,
	0x10480010,
	0x00001007,
	0x01000000,
	0x10480044,
	0x00002000,
	0x06000003,
	0x10483604,
	0x00000004,
	0x00000001,
	0x00200000,
	0x00000000,
	0x01000000,
	0x104834a4,
	0x00000021,
	0x06000007,
	0x10483508,
	0x00003fda,
	0x01fff000,
	0x14050000,
	0xfd7f0000,
	0x00001188,
	0xee780000,
	0x00000000,
	0xe74618b8,
	0x06000002,
	0x10483528,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x06000025,
	0x10483008,
	0x00097000,
	0x0401153e,
	0x0fb3ec10,
	0x00089000,
	0x582a1205,
	0x44331005,
	0x00000082,
	0x00000057,
	0x00000829,
	0x00000457,
	0x00000ccd,
	0x0000403a,
	0x00009f6b,
	0x0004acdd,
	0x0009b94c,
	0x00a5f50a,
	0x00000085,
	0x000000f0,
	0x00000409,
	0x0000051d,
	0x00008656,
	0x00002940,
	0x0004d8b3,
	0x00013b0a,
	0x000adea5,
	0x000f0ff0,
	0x00000085,
	0x000000f0,
	0x00000409,
	0x0000051d,
	0x00008656,
	0x00002940,
	0x0004d8b3,
	0x00013b0a,
	0x000adea5,
	0x000f0ff0,
	0x000000ff,
	0x00000403,
	0x01000000,
	0x10483010,
	0x0fb3ec10,
	0x01000000,
	0x1048311c,
	0x01c54000,
	0x01000000,
	0x10483484,
	0x00000021,
	0x06000003,
	0x1048340c,
	0x001e9000,
	0x00000018,
	0x2a098acb,
	0x0400003b,
	0x01000000,
	0x1048342c,
	0x00014000,
	0x06000007,
	0x10483588,
	0x00003fda,
	0x01fff000,
	0x14050000,
	0xfd7f0000,
	0x00001188,
	0xee780000,
	0x00000000,
	0xe74618b8,
	0x06000002,
	0x104835a8,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x06000025,
	0x10483208,
	0x00097000,
	0x0401153e,
	0x0fb3ec10,
	0x00089000,
	0x582a1205,
	0x44331005,
	0x00000082,
	0x00000057,
	0x00000829,
	0x00000457,
	0x00000ccd,
	0x0000403a,
	0x00009f6b,
	0x0004acdd,
	0x0009b94c,
	0x00a5f50a,
	0x00000085,
	0x000000f0,
	0x00000409,
	0x0000051d,
	0x00008656,
	0x00002940,
	0x0004d8b3,
	0x00013b0a,
	0x000adea5,
	0x000f0ff0,
	0x00000085,
	0x000000f0,
	0x00000409,
	0x0000051d,
	0x00008656,
	0x00002940,
	0x0004d8b3,
	0x00013b0a,
	0x000adea5,
	0x000f0ff0,
	0x000000ff,
	0x00000403,
	0x01000000,
	0x10483210,
	0x0fb3ec10,
	0x01000000,
	0x1048331c,
	0x01c54000,
	0x01000000,
	0x10402888,
	0x00000001,
	0x01000000,
	0x10481008,
	0x06017f0b,
	0x01000000,
	0x10483600,
	0x00000840,
	0x01000000,
	0x10484430,
	0x00000000,

	/* LIST: 1 (3 entries) */
	0x01000000,
	0x10442050,
	0xbaadf00d,

	/* LIST: 3 (168 entries) */
	0x00000000,
	0x01000000,
	0x10446400,
	0x00000100,
	0x01000000,
	0x10446400,
	0x00000000,
	0x01000000,
	0x10442034,
	0x00000013,
	0x01000000,
	0x10442030,
	0x00000100,
	0x01000000,
	0x10447004,
	0x0000000c,
	0x01000000,
	0x10444034,
	0x00000000,
	0x01000000,
	0x10444038,
	0x00000000,
	0x01000000,
	0x10444010,
	0x00000000,
	0x01000000,
	0x10444014,
	0x00000000,
	0x01000000,
	0x1044a404,
	0x00000000,
	0x01000000,
	0x1044a408,
	0x00000000,
	0x01000000,
	0x1044a40c,
	0x00080008,
	0x01000000,
	0x1044a410,
	0x00001111,
	0x01000000,
	0x1044a414,
	0x04040404,
	0x01000000,
	0x1044a418,
	0x04040404,
	0x06000001,
	0x10442008,
	0x02d00120,
	0x001cb876,
	0x01000000,
	0x10442010,
	0x0002ff34,
	0x01000000,
	0x10442014,
	0x000f0000,
	0x01000000,
	0x10442044,
	0x00000000,
	0x01000000,
	0x10442088,
	0x00000000,
	0x06000003,
	0x104423e0,
	0x02d00120,
	0x00000000,
	0x02d00120,
	0x00000000,
	0x01000000,
	0x1044201c,
	0x0000000c,
	0x01000000,
	0x10442020,
	0x00000000,
	0x01000000,
	0x10442024,
	0x00000000,
	0x01000000,
	0x10442028,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x01000000,
	0x1044105c,
	0x02d00120,
	0x06000003,
	0x10441028,
	0x00000000,
	0x00000000,
	0x00000000,
	0x0000ff00,
	0x01000000,
	0x10441020,
	0x00000000,
	0x06000003,
	0x104410f0,
	0x00000000,
	0x10000000,
	0x00000000,
	0x08000000,
	0x06000001,
	0x10441210,
	0x10000000,
	0x08000000,
	0x01000000,
	0x10441018,
	0x00100000,
	0x01000000,
	0x1044121c,
	0x00000007,
	0x01000000,
	0x1044120c,
	0x00200000,
	0x01000000,
	0x10441204,
	0x00000120,
	0x01000000,
	0x10441218,
	0x00000001,
	0x01000000,
	0x10441200,
	0x00000031,
	0x01000000,
	0x1044101c,
	0x00000000,
	0x06000003,
	0x10441038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10441208,
	0x00000000,
	0x01000000,
	0x10441040,
	0x043e8680,
	0x06000001,
	0x10441010,
	0x00000565,
	0x000b0500,
	0x0600000e,
	0x10441100,
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
	0x1044100c,
	0x00000010,
	0x01000000,
	0x10441004,
	0x00000001,
	0x01000000,
	0x10442004,
	0x00000001,

	/* LIST: 5 (796 entries) */
	0x01000000,
	0x10484430,
	0x00000004,
	0x01000000,
	0x10484418,
	0x00000002,
	0x01000000,
	0x10484424,
	0x00000002,
	0x01000000,
	0x1048442c,
	0x00000002,
	0x01000000,
	0x10484440,
	0x00000001,
	0x01000000,
	0x10484440,
	0x00000000,
	0x01000000,
	0x10484448,
	0x00000001,
	0x01000000,
	0x10484448,
	0x00000000,
	0x01000000,
	0x10484454,
	0x00000001,
	0x01000000,
	0x10484454,
	0x00000000,
	0x01000000,
	0x1048445c,
	0x00000001,
	0x01000000,
	0x1048445c,
	0x00000000,
	0x01000000,
	0x10484430,
	0x00000000,
	0x01000000,
	0x10484418,
	0x00000000,
	0x01000000,
	0x10484424,
	0x00000000,
	0x01000000,
	0x1048442c,
	0x00000001,
	0x0600000c,
	0x1048100c,
	0xfdfd1400,
	0x00fdfdfd,
	0x00000000,
	0x00000000,
	0x00000000,
	0xffff0000,
	0x00000006,
	0x03805332,
	0x0120c148,
	0x110036e5,
	0x05899b8b,
	0x00cc000c,
	0x00000000,
	0x01000000,
	0x10481050,
	0x012002d0,
	0x06000001,
	0x1048105c,
	0x00001803,
	0x0000193c,
	0x060000ff,
	0x10481400,
	0x0020400c,
	0x00a24000,
	0x00288058,
	0x00240028,
	0x00207040,
	0x00205040,
	0x00204018,
	0x0021403e,
	0x00204039,
	0x0028400d,
	0x0028c00f,
	0x0028c1b5,
	0x00288075,
	0x002c800b,
	0x0028e040,
	0x0028c28a,
	0x00801000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00462003,
	0x00ad8000,
	0x00461001,
	0x00441004,
	0x00442005,
	0x00444002,
	0x00454022,
	0x00254018,
	0x0041c23c,
	0x0040c002,
	0x0020c018,
	0x004c2005,
	0x004c1005,
	0x004c2005,
	0x004c4001,
	0x004d4022,
	0x002d4018,
	0x0049c23c,
	0x0048c002,
	0x00404001,
	0x00204018,
	0x00442004,
	0x00462001,
	0x00a98000,
	0x00461001,
	0x00441004,
	0x00442005,
	0x00454024,
	0x00254018,
	0x0041c23c,
	0x0041c002,
	0x0021c018,
	0x004c2005,
	0x004c1005,
	0x004c2005,
	0x004c4003,
	0x004d4020,
	0x002d4018,
	0x0049c23e,
	0x00404001,
	0x00204018,
	0x00442004,
	0x00462001,
	0x00815000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00200012,
	0x00a00006,
	0x0085f001,
	0x00400001,
	0x00200101,
	0x0065a008,
	0x00000000,
	0x00000000,
	0x0020000b,
	0x00400801,
	0x0085c002,
	0x0030007a,
	0x00200019,
	0x0020102d,
	0x00308034,
	0x00400001,
	0x002000c5,
	0x00666008,
	0x00000000,
	0x00000000,
	0x0020000b,
	0x00400801,
	0x00868004,
	0x0030009a,
	0x00200035,
	0x00201031,
	0x00308034,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00400806,
	0x00a00019,
	0x0068000f,
	0x00400801,
	0x00c08008,
	0x00c08008,
	0x00c01010,
	0x00c01010,
	0x00400928,
	0x00a00019,
	0x0068000f,
	0x00400801,
	0x00c08008,
	0x00c08008,
	0x00c01010,
	0x00c01010,
	0x00400923,
	0x0086e000,
	0x00400801,
	0x00c08008,
	0x00d01010,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00400003,
	0x00400126,
	0x0040000b,
	0x006a0002,
	0x0069c002,
	0x00400003,
	0x00400127,
	0x0040000b,
	0x006a0002,
	0x0069c002,
	0x00400003,
	0x00400126,
	0x0040000b,
	0x006a0002,
	0x0069c002,
	0x00400003,
	0x00400127,
	0x0040000b,
	0x006a0002,
	0x0069c002,
	0x00887000,
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
	0x00400003,
	0x00400126,
	0x00400009,
	0x006c3004,
	0x006bf002,
	0x00400003,
	0x00400127,
	0x00400009,
	0x006c3004,
	0x006bf002,
	0x00400003,
	0x00400126,
	0x00400009,
	0x006c3004,
	0x006bf002,
	0x00400003,
	0x00400127,
	0x00400009,
	0x006c3004,
	0x006bf002,
	0x008aa000,
	0x00400001,
	0x0020003f,
	0x00202036,
	0x00300018,
	0x00400001,
	0x0020007f,
	0x00202036,
	0x00300018,
	0x00000000,
	0x00400003,
	0x00400126,
	0x0040000a,
	0x006e1003,
	0x006dd002,
	0x00400003,
	0x00400127,
	0x0040000a,
	0x006e1003,
	0x006dd002,
	0x00400003,
	0x00400126,
	0x0040000a,
	0x006e1003,
	0x006dd002,
	0x00400003,
	0x00400127,
	0x0040000a,
	0x006e1003,
	0x006dd002,
	0x008c8000,
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
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
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
	0x00051409,
	0xce3120ae,
	0x06000003,
	0x10483604,
	0x00000004,
	0x00000001,
	0x00200000,
	0x00000000,
	0x01000000,
	0x10480018,
	0x00000000,
	0x01000000,
	0x1048441c,
	0x00000002,
	0x01000000,
	0x10484420,
	0x00000002,
	0x01000000,
	0x10484428,
	0x00000002,
	0x01000000,
	0x1048444c,
	0x00000001,
	0x01000000,
	0x1048444c,
	0x00000000,
	0x01000000,
	0x10484450,
	0x00000001,
	0x01000000,
	0x10484450,
	0x00000000,
	0x01000000,
	0x10484458,
	0x00000001,
	0x01000000,
	0x10484458,
	0x00000000,
	0x01000000,
	0x1048441c,
	0x00000000,
	0x01000000,
	0x10484420,
	0x00000001,
	0x01000000,
	0x10484428,
	0x00000000,
	0x01000000,
	0x10484438,
	0x00000004,
	0x01000000,
	0x10484478,
	0x00000001,
	0x01000000,
	0x1048447c,
	0x00000001,
	0x01000000,
	0x10484480,
	0x00000001,
	0x01000000,
	0x10484484,
	0x00000001,
	0x01000000,
	0x10484478,
	0x00000000,
	0x01000000,
	0x1048447c,
	0x00000000,
	0x01000000,
	0x10484480,
	0x00000000,
	0x01000000,
	0x10484484,
	0x00000000,
	0x01000000,
	0x10484438,
	0x00000000,
	0x01000000,
	0x104a0b00,
	0x00000900,
	0x01000000,
	0x104a0b04,
	0x00000001,
	0x01000000,
	0x104a0b08,
	0x00000003,
	0x01000000,
	0x104a0b14,
	0x0c800000,
	0x01000000,
	0x104a0b18,
	0x02002710,
	0x01000000,
	0x104a0ad4,
	0x02580512,
	0x04000000,
	0xf80001ff,
	0x04000001,
	0x051cfe00,
	0x03000002,
	0x104a0ac8,
	0x0b002002,
	0x0d002042,
	0x02002000,
	0x104a0ac8,
	0x04000000,
	0xffffcff8,
	0x04000001,
	0x00002004,
	0x03000002,
	0x104a0acc,
	0x0b002002,
	0x0d002042,
	0x02002000,
	0x104a0acc,
	0x0600003f,
	0x10483980,
	0x00204026,
	0x0028407e,
	0x00668001,
	0x00666002,
	0x00662013,
	0x0066b120,
	0x00662002,
	0x0065a001,
	0x00658002,
	0x0065e001,
	0x0064f013,
	0x0066f120,
	0x0064f001,
	0x00653001,
	0x00842000,
	0x0024c07e,
	0x0025c08a,
	0x0025d5a0,
	0x0035c00d,
	0x0024c07e,
	0x0025c08a,
	0x0025d5a0,
	0x0025c018,
	0x00384068,
	0x0020c07e,
	0x0031c637,
	0x0024c07e,
	0x0025c2e2,
	0x0021c348,
	0x0031c00d,
	0x0020c07e,
	0x0021c2e2,
	0x0025c348,
	0x0035c00d,
	0x0024407e,
	0x0025408a,
	0x002555a0,
	0x0035400d,
	0x0020407e,
	0x00314637,
	0x0021408a,
	0x002155a0,
	0x0031400d,
	0x0024207e,
	0x0025208a,
	0x002735a0,
	0x0035200d,
	0x0024a07e,
	0x0025a08a,
	0x0027b5a0,
	0x0035a00d,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00060106,
	0xe34dc96b,
	0x01000000,
	0x10483808,
	0x0000000b,
	0x01000000,
	0x10484010,
	0x00000249,
	0x01000000,
	0x10484014,
	0x000001d9,
	0x01000000,
	0x10484018,
	0x00000003,
	0x01000000,
	0x1048401c,
	0x00000040,
	0x01000000,
	0x10484020,
	0x012002d0,
	0x01000000,
	0x1048402c,
	0x00000003,
	0x01000000,
	0x10484030,
	0x0000013c,
	0x01000000,
	0x1048430c,
	0x00000001,
	0x01000000,
	0x10484310,
	0x0076b81c,
	0x06000007,
	0x10484208,
	0x00003fda,
	0x01fff000,
	0x20000000,
	0x00000000,
	0x00002000,
	0x00000000,
	0x00000000,
	0x00002000,
	0x06000002,
	0x10484228,
	0x80100401,
	0x00000610,
	0x000ef83d,
	0x01000000,
	0x10480020,
	0x00000000,
	0x01000000,
	0x10480034,
	0x00000234,
	0x01000000,
	0x10480004,
	0x00001001,
	0x01000000,
	0x10480038,
	0x00004000,
	0x01000000,
	0x10480008,
	0x00001007,
	0x01000000,
	0x1048003c,
	0x00002000,
	0x01000000,
	0x1048000c,
	0x00001007,
	0x01000000,
	0x10480040,
	0x00002000,
	0x01000000,
	0x10480010,
	0x00001007,
	0x01000000,
	0x10480044,
	0x00002000,
	0x06000003,
	0x10483604,
	0x00000004,
	0x00000001,
	0x00200000,
	0x00000000,
	0x01000000,
	0x104834a4,
	0x00000021,
	0x06000007,
	0x10483508,
	0x00003fda,
	0x01fff000,
	0x14050000,
	0xfd7f0000,
	0x00001188,
	0xee780000,
	0x00000000,
	0xe74618b8,
	0x06000002,
	0x10483528,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x06000025,
	0x10483008,
	0x00097000,
	0x0401153e,
	0x0fb3ec10,
	0x00089000,
	0x582a1205,
	0x44331005,
	0x00000082,
	0x00000057,
	0x00000829,
	0x00000457,
	0x00000ccd,
	0x0000403a,
	0x00009f6b,
	0x0004acdd,
	0x0009b94c,
	0x00a5f50a,
	0x00000085,
	0x000000f0,
	0x00000409,
	0x0000051d,
	0x00008656,
	0x00002940,
	0x0004d8b3,
	0x00013b0a,
	0x000adea5,
	0x000f0ff0,
	0x00000085,
	0x000000f0,
	0x00000409,
	0x0000051d,
	0x00008656,
	0x00002940,
	0x0004d8b3,
	0x00013b0a,
	0x000adea5,
	0x000f0ff0,
	0x000000ff,
	0x00000403,
	0x01000000,
	0x10483010,
	0x0fb3ec10,
	0x01000000,
	0x1048311c,
	0x01c54000,
	0x01000000,
	0x10483484,
	0x00000021,
	0x06000003,
	0x1048340c,
	0x001e9000,
	0x00000018,
	0x2a098acb,
	0x0400003b,
	0x01000000,
	0x1048342c,
	0x00014000,
	0x06000007,
	0x10483588,
	0x00003fda,
	0x01fff000,
	0x14050000,
	0xfd7f0000,
	0x00001188,
	0xee780000,
	0x00000000,
	0xe74618b8,
	0x06000002,
	0x104835a8,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x06000025,
	0x10483208,
	0x00097000,
	0x0401153e,
	0x0fb3ec10,
	0x00089000,
	0x582a1205,
	0x44331005,
	0x00000082,
	0x00000057,
	0x00000829,
	0x00000457,
	0x00000ccd,
	0x0000403a,
	0x00009f6b,
	0x0004acdd,
	0x0009b94c,
	0x00a5f50a,
	0x00000085,
	0x000000f0,
	0x00000409,
	0x0000051d,
	0x00008656,
	0x00002940,
	0x0004d8b3,
	0x00013b0a,
	0x000adea5,
	0x000f0ff0,
	0x00000085,
	0x000000f0,
	0x00000409,
	0x0000051d,
	0x00008656,
	0x00002940,
	0x0004d8b3,
	0x00013b0a,
	0x000adea5,
	0x000f0ff0,
	0x000000ff,
	0x00000403,
	0x01000000,
	0x10483210,
	0x0fb3ec10,
	0x01000000,
	0x1048331c,
	0x01c54000,
	0x01000000,
	0x10402888,
	0x00000001,
	0x01000000,
	0x10484008,
	0x00000fc3,
	0x01000000,
	0x10481008,
	0x06017f0b,
	0x01000000,
	0x10483600,
	0x00000840,
	0x01000000,
	0x104a0b00,
	0x00000900,
	0x01000000,
	0x104a0b04,
	0x00000001,
	0x01000000,
	0x104a0b08,
	0x00000003,
	0x01000000,
	0x104a0b14,
	0x0c800000,
	0x01000000,
	0x104a0b18,
	0x02002710,
	0x01000000,
	0x104a0ad4,
	0x02580512,
	0x04000000,
	0xf80001ff,
	0x04000001,
	0x051cfe00,
	0x03000002,
	0x104a0ac8,
	0x0b002002,
	0x0d002042,
	0x02002000,
	0x104a0ac8,
	0x04000000,
	0xffffcff8,
	0x04000001,
	0x00002004,
	0x03000002,
	0x104a0acc,
	0x0b002002,
	0x0d002042,
	0x02002000,
	0x104a0acc,
	0x06000007,
	0x10484208,
	0x00003fda,
	0x01fff000,
	0x20000000,
	0x00000000,
	0x00002000,
	0x00000000,
	0x00000000,
	0x00002000,
	0x06000002,
	0x10484228,
	0x80100401,
	0x00000610,
	0x000ef83d,
	0x01000000,
	0x10484430,
	0x00000000,
	0x01000000,
	0x10484438,
	0x00000000,

	/* LIST: 6 (3 entries) */
	0x01000000,
	0x10442050,
	0xbaadf00d,

	/* LIST: 7 (64 entries) */
	0x00000000,
	0x01000000,
	0x10447004,
	0x0000000c,
	0x06000001,
	0x10442008,
	0x02d00120,
	0x001cb876,
	0x01000000,
	0x10442010,
	0x0002ff34,
	0x01000000,
	0x10442014,
	0x000f0000,
	0x01000000,
	0x10442044,
	0x00000000,
	0x06000003,
	0x104423e0,
	0x02d00120,
	0x00000000,
	0x02d00120,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x06000003,
	0x10441038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10441208,
	0x00000000,
	0x01000000,
	0x10441040,
	0x043e8680,
	0x0600000e,
	0x10441100,
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
	0x1044100c,
	0x00000010,
	0x01000000,
	0x10441004,
	0x00000001,
	0x01000000,
	0x10442004,
	0x00000001,

	/* LIST: 9 (47 entries) */
	0x00000000,
	0x01000000,
	0x10447004,
	0x0000000c,
	0x06000001,
	0x10442008,
	0x02d00120,
	0x001cb876,
	0x01000000,
	0x10442010,
	0x0002ff34,
	0x01000000,
	0x10442014,
	0x000f0000,
	0x01000000,
	0x10442044,
	0x00000000,
	0x06000003,
	0x104423e0,
	0x02d00120,
	0x00000000,
	0x02d00120,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x06000003,
	0x10441038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10441208,
	0x00000000,
	0x01000000,
	0x10441040,
	0x043e8680,
	0x01000000,
	0x1044100c,
	0x00000010,
	0x01000000,
	0x10441004,
	0x00000001,
	0x01000000,
	0x10442004,
	0x00000001,

	/* LIST: 11 (47 entries) */
	0x00000000,
	0x01000000,
	0x10447004,
	0x0000000c,
	0x06000001,
	0x10442008,
	0x02d00120,
	0x001cb876,
	0x01000000,
	0x10442010,
	0x0002ff34,
	0x01000000,
	0x10442014,
	0x000f0000,
	0x01000000,
	0x10442044,
	0x00000000,
	0x06000003,
	0x104423e0,
	0x02d00120,
	0x00000000,
	0x02d00120,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x06000003,
	0x10441038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10441208,
	0x00000000,
	0x01000000,
	0x10441040,
	0x043e8680,
	0x01000000,
	0x1044100c,
	0x00000010,
	0x01000000,
	0x10441004,
	0x00000001,
	0x01000000,
	0x10442004,
	0x00000001,
};

uint32_t aListCount1[] = 
{
	0,
	123,
	199,
	258,
	317,
};

uint32_t aList1[] = 
{

	/* LIST: 4 (123 entries) */
	0x00000000,
	0x01000000,
	0x10447004,
	0x0000000c,
	0x01000000,
	0x10444080,
	0x00000001,
	0x01000000,
	0x10444084,
	0x00000001,
	0x01000000,
	0x10444018,
	0x00000001,
	0x01000000,
	0x1044401c,
	0x00000001,
	0x06000001,
	0x10442008,
	0x02d00120,
	0x001cb876,
	0x01000000,
	0x10442010,
	0x0002ff34,
	0x01000000,
	0x10442014,
	0x000f0000,
	0x01000000,
	0x10442044,
	0x00000000,
	0x01000000,
	0x10442088,
	0x00000000,
	0x06000003,
	0x104423e0,
	0x02d00120,
	0x00000000,
	0x02d00120,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x01000000,
	0x1044105c,
	0x02d00120,
	0x06000003,
	0x10441028,
	0x00000000,
	0x00000000,
	0x00000000,
	0x0000ff00,
	0x01000000,
	0x10441020,
	0x00000000,
	0x06000003,
	0x104410f0,
	0x00000000,
	0x10000000,
	0x00000000,
	0x08000000,
	0x06000001,
	0x10441210,
	0x10000000,
	0x08000000,
	0x01000000,
	0x10441018,
	0x00100000,
	0x01000000,
	0x1044121c,
	0x00000007,
	0x01000000,
	0x1044120c,
	0x00200000,
	0x01000000,
	0x10441204,
	0x00000120,
	0x01000000,
	0x10441218,
	0x00000001,
	0x01000000,
	0x10441200,
	0x00000031,
	0x06000003,
	0x10441038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10441208,
	0x00100000,
	0x01000000,
	0x10441040,
	0x043e8c20,
	0x06000001,
	0x10441010,
	0x00000565,
	0x000b0500,
	0x0600000e,
	0x10441100,
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
	0x1044100c,
	0x00000010,
	0x01000000,
	0x10441004,
	0x00000001,
	0x01000000,
	0x10442004,
	0x00000001,

	/* LIST: 8 (76 entries) */
	0x00000000,
	0x01000000,
	0x10447004,
	0x0000000c,
	0x01000000,
	0x10444080,
	0x00000001,
	0x01000000,
	0x10444084,
	0x00000001,
	0x01000000,
	0x10444018,
	0x00000001,
	0x01000000,
	0x1044401c,
	0x00000001,
	0x06000001,
	0x10442008,
	0x02d00120,
	0x001cb876,
	0x01000000,
	0x10442010,
	0x0002ff34,
	0x01000000,
	0x10442014,
	0x000f0000,
	0x01000000,
	0x10442044,
	0x00000000,
	0x06000003,
	0x104423e0,
	0x02d00120,
	0x00000000,
	0x02d00120,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x06000003,
	0x10441038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10441208,
	0x00100000,
	0x01000000,
	0x10441040,
	0x043e8c20,
	0x0600000e,
	0x10441100,
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
	0x1044100c,
	0x00000010,
	0x01000000,
	0x10441004,
	0x00000001,
	0x01000000,
	0x10442004,
	0x00000001,

	/* LIST: 10 (59 entries) */
	0x00000000,
	0x01000000,
	0x10447004,
	0x0000000c,
	0x01000000,
	0x10444080,
	0x00000001,
	0x01000000,
	0x10444084,
	0x00000001,
	0x01000000,
	0x10444018,
	0x00000001,
	0x01000000,
	0x1044401c,
	0x00000001,
	0x06000001,
	0x10442008,
	0x02d00120,
	0x001cb876,
	0x01000000,
	0x10442010,
	0x0002ff34,
	0x01000000,
	0x10442014,
	0x000f0000,
	0x01000000,
	0x10442044,
	0x00000000,
	0x06000003,
	0x104423e0,
	0x02d00120,
	0x00000000,
	0x02d00120,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x06000003,
	0x10441038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10441208,
	0x00100000,
	0x01000000,
	0x10441040,
	0x043e8c20,
	0x01000000,
	0x1044100c,
	0x00000010,
	0x01000000,
	0x10441004,
	0x00000001,
	0x01000000,
	0x10442004,
	0x00000001,

	/* LIST: 12 (59 entries) */
	0x00000000,
	0x01000000,
	0x10447004,
	0x0000000c,
	0x01000000,
	0x10444080,
	0x00000001,
	0x01000000,
	0x10444084,
	0x00000001,
	0x01000000,
	0x10444018,
	0x00000001,
	0x01000000,
	0x1044401c,
	0x00000001,
	0x06000001,
	0x10442008,
	0x02d00120,
	0x001cb876,
	0x01000000,
	0x10442010,
	0x0002ff34,
	0x01000000,
	0x10442014,
	0x000f0000,
	0x01000000,
	0x10442044,
	0x00000000,
	0x06000003,
	0x104423e0,
	0x02d00120,
	0x00000000,
	0x02d00120,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x06000003,
	0x10441038,
	0x00000000,
	0x000002d0,
	0x00000000,
	0x00000b40,
	0x01000000,
	0x10441208,
	0x00100000,
	0x01000000,
	0x10441040,
	0x043e8c20,
	0x01000000,
	0x1044100c,
	0x00000010,
	0x01000000,
	0x10441004,
	0x00000001,
	0x01000000,
	0x10442004,
	0x00000001,
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
 { 8, 0, 8, aListCount0, aList0 },
 { 9, 1, 4, aListCount1, aList1 },
 { -1, -1, 0, NULL, NULL },
 { -1, -1, 0, NULL, NULL },
 { -1, -1, 0, NULL, NULL },
 { -1, -1, 0, NULL, NULL },
 { -1, -1, 0, NULL, NULL },
 { -1, -1, 0, NULL, NULL },

};

