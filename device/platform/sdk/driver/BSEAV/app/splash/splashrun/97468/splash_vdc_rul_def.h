/***************************************************************************
 File : splash_vdc_rul.h
 Date  : May 17 2010 17:45:38
 Description : RUL list for splash BVN configuration
 ***************************************************************************/

#include "splash_magnum.h"


const uint32_t       gSplashWidth   = 720;
const uint32_t       gSplashHeight  = 482;
const uint32_t       gSplashPhysAddr = 0x043e8680; 
const uint32_t       gSplashPitch   = 1440;
const BPXL_Format    gSplashFormat  = (BPXL_Format)0x06e40565; /* BPXL_eR5_G6_B5 */
const BFMT_VideoFmt  gVideoFormat   = (BFMT_VideoFmt)0; /* BFMT_VideoFmt_eNTSC */

uint32_t aListCount0[] = 
{
	0,
	561,
	564,
	732,
	1528,
	1531,
	1595,
	1642,
	1689,
};

uint32_t aList0[] = 
{

	/* LIST: 0 (561 entries) */
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
	0x00000000,
	0x00000006,
	0x03805332,
	0x0120c148,
	0x110036e5,
	0x05899b8b,
	0x00cc000c,
	0x00000000,
	0x01000000,
	0x10481050,
	0x00f102d0,
	0x06000001,
	0x1048105c,
	0x00001803,
	0x0000190a,
	0x060000ff,
	0x10481400,
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
	0x10402888,
	0x00000001,
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
	0x12ec0000,
	0x00420000,
	0x00000d71,
	0xe61c0c74,
	0x0000f76f,
	0xf55f1332,
	0x06000002,
	0x10483528,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x06000025,
	0x10483008,
	0x00097000,
	0x0400147e,
	0x0f03c010,
	0x0008bc00,
	0x401a0800,
	0x64482e14,
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
	0x00000c63,
	0x01000000,
	0x10483010,
	0x0f03c010,
	0x01000000,
	0x1048311c,
	0x00870000,
	0x01000000,
	0x10483484,
	0x00000021,
	0x06000003,
	0x1048340c,
	0x001fc05e,
	0x00000018,
	0x21f07c1f,
	0x0400003b,
	0x01000000,
	0x1048342c,
	0x00010000,
	0x06000007,
	0x10483588,
	0x00003fda,
	0x01fff000,
	0x12ec0000,
	0x00420000,
	0x00000d71,
	0xe61c0c74,
	0x0000f76f,
	0xf55f1332,
	0x06000002,
	0x104835a8,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x06000025,
	0x10483208,
	0x00097000,
	0x0400147e,
	0x0f03c010,
	0x0008bc00,
	0x401a0800,
	0x64482e14,
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
	0x00000c63,
	0x01000000,
	0x10483210,
	0x0f03c010,
	0x01000000,
	0x1048331c,
	0x00870000,
	0x01000000,
	0x10481008,
	0x02017f0b,
	0x01000000,
	0x10483600,
	0x00000840,

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
	0x02d000f1,
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
	0x02d000f1,
	0x00000000,
	0x02d000f1,
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
	0x02d000f1,
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
	0x000000f1,
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
	0x00000000,
	0x00000006,
	0x03805332,
	0x0120c148,
	0x110036e5,
	0x05899b8b,
	0x00cc000c,
	0x00000000,
	0x01000000,
	0x10481050,
	0x00f102d0,
	0x06000001,
	0x1048105c,
	0x00001803,
	0x0000190a,
	0x060000ff,
	0x10481400,
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
	0x00000000,
	0x0064f001,
	0x00653002,
	0x0066b003,
	0x0066300f,
	0x0066d0f0,
	0x00667002,
	0x00663002,
	0x0065d001,
	0x0065b002,
	0x00660001,
	0x0065300f,
	0x006710f0,
	0x00657001,
	0x00841000,
	0x002cc07c,
	0x0025c072,
	0x0025d5a0,
	0x0035c01b,
	0x0024c07c,
	0x0025c072,
	0x0025d5a0,
	0x0035c01b,
	0x0024807c,
	0x00258072,
	0x002595a0,
	0x00358010,
	0x0020c07c,
	0x0031c62d,
	0x0024c07c,
	0x0025c2de,
	0x0031c34f,
	0x0020c07c,
	0x0021c2de,
	0x0035c34f,
	0x0024407c,
	0x00254072,
	0x002555a0,
	0x0035401b,
	0x0024007c,
	0x00250072,
	0x002515a0,
	0x0035001b,
	0x0020407c,
	0x0031462d,
	0x0024207c,
	0x00252072,
	0x002735a0,
	0x0035201b,
	0x0024a07c,
	0x0025a072,
	0x0027b5a0,
	0x0035a01b,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00032205,
	0xff0c57d3,
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
	0x00f102d0,
	0x01000000,
	0x1048402c,
	0x00000003,
	0x01000000,
	0x10484030,
	0x0000010a,
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
	0x10402888,
	0x00000001,
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
	0x12ec0000,
	0x00420000,
	0x00000d71,
	0xe61c0c74,
	0x0000f76f,
	0xf55f1332,
	0x06000002,
	0x10483528,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x06000025,
	0x10483008,
	0x00097000,
	0x0400147e,
	0x0f03c010,
	0x0008bc00,
	0x401a0800,
	0x64482e14,
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
	0x00000c63,
	0x01000000,
	0x10483010,
	0x0f03c010,
	0x01000000,
	0x1048311c,
	0x00870000,
	0x01000000,
	0x10483484,
	0x00000021,
	0x06000003,
	0x1048340c,
	0x001fc05e,
	0x00000018,
	0x21f07c1f,
	0x0400003b,
	0x01000000,
	0x1048342c,
	0x00010000,
	0x06000007,
	0x10483588,
	0x00003fda,
	0x01fff000,
	0x12ec0000,
	0x00420000,
	0x00000d71,
	0xe61c0c74,
	0x0000f76f,
	0xf55f1332,
	0x06000002,
	0x104835a8,
	0x80100401,
	0x00000535,
	0x000ffc01,
	0x06000025,
	0x10483208,
	0x00097000,
	0x0400147e,
	0x0f03c010,
	0x0008bc00,
	0x401a0800,
	0x64482e14,
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
	0x00000c63,
	0x01000000,
	0x10483210,
	0x0f03c010,
	0x01000000,
	0x1048331c,
	0x00870000,
	0x01000000,
	0x10484008,
	0x00000fc3,
	0x01000000,
	0x10481008,
	0x02017f0b,
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
	0x02d000f1,
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
	0x02d000f1,
	0x00000000,
	0x02d000f1,
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
	0x02d000f1,
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
	0x02d000f1,
	0x00000000,
	0x02d000f1,
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
	0x02d000f1,
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
	0x02d000f1,
	0x00000000,
	0x02d000f1,
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
	0x02d000f1,
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
	0x02d000f1,
	0x00000000,
	0x02d000f1,
	0x00000000,
	0x01000000,
	0x104423f0,
	0x00000009,
	0x01000000,
	0x1044105c,
	0x02d000f1,
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
	0x000000f1,
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
	0x02d000f1,
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
	0x02d000f1,
	0x00000000,
	0x02d000f1,
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
	0x02d000f1,
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
	0x02d000f1,
	0x00000000,
	0x02d000f1,
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
	0x02d000f1,
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
	0x02d000f1,
	0x00000000,
	0x02d000f1,
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

