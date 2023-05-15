/***************************************************************************
 File : splash_vdc_reg.h
 Date  : Oct 25 2012 12:21:26
 Description : Register list for splash BVN configuration
 ***************************************************************************/

#include "splash_magnum.h"

uint32_t aulReg[] = 
{

	0x00404318,   0x0005a300, /* SUN_TOP_CTRL_SW_RESET */

	0x0040431c,   0x0005a300, /* SUN_TOP_CTRL_SW_INIT_0_CLEAR */

	0x0042004c,   0x00000012, /* BCHP_CLKGEN_... */
	0x00420050,   0x0000000e,
	0x00420170,   0x0000001f,
	0x00420170,   0x0000001f,
	0x00420170,   0x0000001f,
	0x00420170,   0x0000001f,
	0x0042017c,   0x00000002,
	0x0042017c,   0x00000001,
	0x0042017c,   0x00000000,
	0x00420160,   0x00000000,
	0x0042015c,   0x00000000,
	0x00420138,   0x00000096,
	0x0042035c,   0x0000007f,
	0x00420054,   0x0000000c,
	0x004202a8,   0x00000007,
	0x00420140,   0x000001f4,
	0x00420014,   0x00000000,
	0x00420010,   0x00000000,
	0x00420038,   0x00000000,
	0x00420034,   0x00000000,
	0x004202b4,   0x00000002,
	0x004202b4,   0x00000001,
	0x004202b4,   0x00000000,
	0x004201fc,   0x0000000f,
	0x00420198,   0x00000007,
	0x00420200,   0x00000001,
	0x004201fc,   0x0000000f,

	0x006a0a80,   0x00000030, /* HDMI TX Control */

	0x00420198,   0x00000007, /* BCHP_CLKGEN_... */
	0x004201a0,   0x00000002,
	0x004201a0,   0x00000001,
	0x004201a0,   0x00000000,
	0x00420358,   0x00000000,
	0x0042036c,   0x00000002,
	0x0042036c,   0x00000001,
	0x0042036c,   0x00000000,
	0x004201e4,   0x00000007,
	0x004201e4,   0x00000007,
	0x004201e0,   0x00000000,
	0x004201f0,   0x00000002,
	0x004201f0,   0x00000001,
	0x004201f0,   0x00000000,
	0x004202a0,   0x00000001,
	0x004202a4,   0x00000002,
	0x0042020c,   0x00000002,
	0x0042020c,   0x00000000,
	0x00420238,   0x0000000f,
	0x00420238,   0x0000000f,
	0x00420234,   0x00000000,
	0x00420244,   0x00000002,
	0x00420244,   0x00000001,
	0x00420244,   0x00000000,
	0x00420314,   0x00000002,
	0x004200a8,   0x00000000,
	0x004200a4,   0x00000000,
	0x00420080,   0x00000050,
	0x00420314,   0x00000000,
	0x00420084,   0x00000050,
	0x004202b8,   0x00000001,
	0x004202c4,   0x00000002,
	0x004202c4,   0x00000001,
	0x004202c4,   0x00000000,
	0x004202bc,   0x00000001,

	0x00060080,   0x00000000, /* ?? */
	0x00c0030c,   0x80000000,

	0x00420258,   0x00000007, /* BCHP_CLKGEN_... */

	0x00626010,   0x00000001, /* ?? */
	0x00626010,   0x00000002,
	0x00626010,   0x00000004,
	0x00626010,   0x00000008,
	0x00626010,   0x00000010,
	0x00626010,   0x00000020,
	0x00626010,   0x00000040,
	0x00626010,   0x00000080,
	0x00626010,   0x00000100,
	0x00626010,   0x00000200,
	0x00626010,   0x00000400,
	0x00626010,   0x00000800,
	0x00626010,   0x00001000,
	0x00626010,   0x00002000,
	0x00626010,   0x00004000,
	0x00626010,   0x00008000,
	0x00626010,   0x00010000,
	0x00626010,   0x00020000,
	0x00626010,   0x00040000,
	0x00626010,   0x00080000,
	0x00626010,   0x00100000,
	0x00626010,   0x00200000,
	0x00626010,   0x00400000,
	0x00626010,   0x00800000,
	0x00626010,   0x01000000,
	0x00626010,   0x02000000,
	0x00626010,   0x04000000,
	0x00626010,   0x08000000,
	0x00626010,   0x10000000,
	0x00626010,   0x20000000,
	0x00626010,   0x40000000,
	0x00626010,   0x80000000,
	0x00424010,   0x00000001,
	0x00424010,   0x00000002,
	0x00424010,   0x00000004,
	0x00424010,   0x00000008,
	0x00424010,   0x00000010,
	0x00424010,   0x00000020,
	0x00424010,   0x00000040,
	0x00424010,   0x00000080,
	0x00424010,   0x00000100,
	0x00424010,   0x00000200,
	0x00424010,   0x00000400,
	0x00424010,   0x00000800,
	0x00424010,   0x00001000,
	0x00424010,   0x00002000,
	0x00424010,   0x00004000,
	0x00424010,   0x00008000,
	0x00424010,   0x00010000,
	0x00424010,   0x00020000,
	0x00424010,   0x00040000,
	0x00424010,   0x00080000,
	0x00424010,   0x00100000,
	0x00424010,   0x00200000,
	0x00424010,   0x00400000,
	0x00424010,   0x00800000,
	0x00424010,   0x01000000,
	0x00424010,   0x02000000,
	0x00424010,   0x04000000,
	0x00424010,   0x08000000,
	0x00424010,   0x10000000,
	0x00424010,   0x20000000,
	0x00424010,   0x40000000,
	0x00424010,   0x80000000,
	0x00451010,   0x00000001,
	0x00451010,   0x00000002,
	0x00451010,   0x00000004,
	0x00451010,   0x00000008,
	0x00451010,   0x00000010,
	0x00451010,   0x00000020,
	0x00451010,   0x00000040,
	0x00451010,   0x00000080,
	0x00451010,   0x00000100,
	0x00451010,   0x00000200,
	0x00451010,   0x00000400,
	0x00451010,   0x00000800,
	0x00451010,   0x00001000,
	0x00451010,   0x00002000,
	0x00451010,   0x00004000,
	0x00451010,   0x00008000,
	0x00451010,   0x00010000,
	0x00451010,   0x00020000,
	0x00451010,   0x00040000,
	0x00451010,   0x00080000,
	0x00451010,   0x00100000,
	0x00451010,   0x00200000,
	0x00451010,   0x00400000,
	0x00451010,   0x00800000,
	0x00451010,   0x01000000,
	0x00451010,   0x02000000,
	0x00451010,   0x04000000,
	0x00451010,   0x08000000,
	0x00451010,   0x10000000,
	0x00451010,   0x20000000,
	0x00451010,   0x40000000,
	0x00451010,   0x80000000,
	0x003b3010,   0x00000001,
	0x003b3010,   0x00000002,
	0x003b3010,   0x00000004,
	0x003b3010,   0x00000008,
	0x003b3010,   0x00000010,
	0x003b3010,   0x00000020,
	0x003b3010,   0x00000040,
	0x003b3010,   0x00000080,
	0x003b3010,   0x00000100,
	0x003b3010,   0x00000200,
	0x003b3010,   0x00000400,
	0x003b3010,   0x00000800,
	0x003b3010,   0x00001000,
	0x003b3010,   0x00002000,
	0x003b3010,   0x00004000,
	0x003b3010,   0x00008000,
	0x003b3010,   0x00010000,
	0x003b3010,   0x00020000,
	0x003b3010,   0x00040000,
	0x003b3010,   0x00080000,
	0x003b3010,   0x00100000,
	0x003b3010,   0x00200000,
	0x003b3010,   0x00400000,
	0x003b3010,   0x00800000,
	0x003b3010,   0x01000000,
	0x003b3010,   0x02000000,
	0x003b3010,   0x04000000,
	0x003b3010,   0x08000000,
	0x003b3010,   0x10000000,
	0x003b3010,   0x20000000,
	0x003b3010,   0x40000000,
	0x003b3010,   0x80000000,
	0x00822410,   0x00000001,
	0x00822410,   0x00000002,
	0x00822410,   0x00000004,
	0x00822410,   0x00000008,
	0x00822410,   0x00000010,
	0x00822410,   0x00000020,
	0x00822410,   0x00000040,
	0x00822410,   0x00000080,
	0x00822410,   0x00000100,
	0x00822410,   0x00000200,
	0x00822410,   0x00000400,
	0x00822410,   0x00000800,
	0x00822410,   0x00001000,
	0x00822410,   0x00002000,
	0x00822410,   0x00004000,
	0x00822410,   0x00008000,
	0x00822410,   0x00010000,
	0x00822410,   0x00020000,
	0x00822410,   0x00040000,
	0x00822410,   0x00080000,
	0x00822410,   0x00100000,
	0x00822410,   0x00200000,
	0x00822410,   0x00400000,
	0x00822410,   0x00800000,
	0x00822410,   0x01000000,
	0x00822410,   0x02000000,
	0x00822410,   0x04000000,
	0x00822410,   0x08000000,
	0x00822410,   0x10000000,
	0x00822410,   0x20000000,
	0x00822410,   0x40000000,
	0x00822410,   0x80000000,
	0x00403410,   0x00000001,
	0x00403410,   0x00000002,
	0x00403410,   0x00000004,
	0x00403410,   0x00000008,
	0x00403410,   0x00000010,
	0x00403410,   0x00000020,
	0x00403410,   0x00000040,
	0x00403410,   0x00000080,
	0x00403410,   0x00000100,
	0x00403410,   0x00000200,
	0x00403410,   0x00000400,
	0x00403410,   0x00000800,
	0x00403410,   0x00001000,
	0x00403410,   0x00002000,
	0x00403410,   0x00004000,
	0x00403410,   0x00008000,
	0x00403410,   0x00010000,
	0x00403410,   0x00020000,
	0x00403410,   0x00040000,
	0x00403410,   0x00080000,
	0x00403410,   0x00100000,
	0x00403410,   0x00200000,
	0x00403410,   0x00400000,
	0x00403410,   0x00800000,
	0x00403410,   0x01000000,
	0x00403410,   0x02000000,
	0x00403410,   0x04000000,
	0x00403410,   0x08000000,
	0x00403410,   0x10000000,
	0x00403410,   0x20000000,
	0x00403410,   0x40000000,
	0x00403410,   0x80000000,

	0x00403010,   0x00000001, /* SUN_L2_... */
	0x00403010,   0x00000002,
	0x00403010,   0x00000004,
	0x00403010,   0x00000008,
	0x00403010,   0x00000010,
	0x00403010,   0x00000020,
	0x00403010,   0x00000040,
	0x00403010,   0x00000080,
	0x00403010,   0x00000100,
	0x00403010,   0x00000200,
	0x00403010,   0x00000400,
	0x00403010,   0x00000800,
	0x00403010,   0x00001000,
	0x00403010,   0x00002000,
	0x00403010,   0x00004000,
	0x00403010,   0x00008000,
	0x00403010,   0x00010000,
	0x00403010,   0x00020000,
	0x00403010,   0x00040000,
	0x00403010,   0x00080000,
	0x00403010,   0x00100000,
	0x00403010,   0x00200000,
	0x00403010,   0x00400000,
	0x00403010,   0x00800000,
	0x00403010,   0x01000000,
	0x00403010,   0x02000000,
	0x00403010,   0x04000000,
	0x00403010,   0x08000000,
	0x00403010,   0x10000000,
	0x00403010,   0x20000000,
	0x00403010,   0x40000000,
	0x00403010,   0x80000000,

	0x00408410,   0x00000001, /* ?? */
	0x00408410,   0x00000002,
	0x00408410,   0x00000004,
	0x00408410,   0x00000008,
	0x00408410,   0x00000010,
	0x00408410,   0x00000020,
	0x00408410,   0x00000040,
	0x00408410,   0x00000080,
	0x00408410,   0x00000100,
	0x00408410,   0x00000200,
	0x00408410,   0x00000400,
	0x00408410,   0x00000800,
	0x00408410,   0x00001000,
	0x00408410,   0x00002000,
	0x00408410,   0x00004000,
	0x00408410,   0x00008000,
	0x00408410,   0x00010000,
	0x00408410,   0x00020000,
	0x00408410,   0x00040000,
	0x00408410,   0x00080000,
	0x00408410,   0x00100000,
	0x00408410,   0x00200000,
	0x00408410,   0x00400000,
	0x00408410,   0x00800000,
	0x00408410,   0x01000000,
	0x00408410,   0x02000000,
	0x00408410,   0x04000000,
	0x00408410,   0x08000000,
	0x00408410,   0x10000000,
	0x00408410,   0x20000000,
	0x00408410,   0x40000000,
	0x00408410,   0x80000000,
	0x00408e50,   0x00000001,
	0x00408e50,   0x00000002,
	0x00408e50,   0x00000004,
	0x00408e50,   0x00000008,
	0x00408e50,   0x00000010,
	0x00408e50,   0x00000020,
	0x00408e50,   0x00000040,
	0x00408e50,   0x00000080,
	0x00408e50,   0x00000100,
	0x00408e50,   0x00000200,
	0x00408e50,   0x00000400,
	0x00408e50,   0x00000800,
	0x00408e50,   0x00001000,
	0x00408e50,   0x00002000,
	0x00408e50,   0x00004000,
	0x00408e50,   0x00008000,
	0x00408e50,   0x00010000,
	0x00408e50,   0x00020000,
	0x00408e50,   0x00040000,
	0x00408e50,   0x00080000,
	0x00408e50,   0x00100000,
	0x00408e50,   0x00200000,
	0x00408e50,   0x00400000,
	0x00408e50,   0x00800000,
	0x00408e50,   0x01000000,
	0x00408e50,   0x02000000,
	0x00408e50,   0x04000000,
	0x00408e50,   0x08000000,
	0x00408e50,   0x10000000,
	0x00408e50,   0x20000000,
	0x00408e50,   0x40000000,
	0x00408e50,   0x80000000,
	0x00408b80,   0x00000063,
	0x00408b80,   0x00000062,
	0x00408b80,   0x00000060,
	0x00408b80,   0x00000060,
	0x00408b80,   0x00000060,
	0x00408b80,   0x00000060,
	0x00408b80,   0x00000040,
	0x00408b80,   0x00000000,
	0x00408b80,   0x00000000,
	0x00408b80,   0x00000000,
	0x00408b80,   0x00000000,
	0x00408b80,   0x00000000,
	0x00080010,   0x00000001,
	0x00080010,   0x00000002,
	0x00080010,   0x00000004,
	0x00080010,   0x00000008,
	0x00080010,   0x00000010,
	0x00080010,   0x00000020,
	0x00080010,   0x00000040,
	0x00080010,   0x00000080,
	0x00080010,   0x00000100,
	0x00080010,   0x00000200,
	0x00080010,   0x00000400,
	0x00080010,   0x00000800,
	0x00080010,   0x00001000,
	0x00080010,   0x00002000,
	0x00080010,   0x00004000,
	0x00080010,   0x00008000,
	0x00080010,   0x00010000,
	0x00080010,   0x00020000,
	0x00080010,   0x00040000,
	0x00080010,   0x00080000,
	0x00080010,   0x00100000,
	0x00080010,   0x00200000,
	0x00080010,   0x00400000,
	0x00080010,   0x00800000,
	0x00080010,   0x01000000,
	0x00080010,   0x02000000,
	0x00080010,   0x04000000,
	0x00080010,   0x08000000,
	0x00080010,   0x10000000,
	0x00080010,   0x20000000,
	0x00080010,   0x40000000,
	0x00080010,   0x80000000,
	0x00c22024,   0x00000001,
	0x00c22024,   0x00000002,
	0x00c22024,   0x00000004,
	0x00c22024,   0x00000008,
	0x00c22024,   0x00000010,
	0x00c22024,   0x00000020,
	0x00c22024,   0x00000040,
	0x00c22024,   0x00000080,
	0x00c22024,   0x00000100,
	0x00c22024,   0x00000200,
	0x00c22024,   0x00000400,
	0x00c22024,   0x00000800,
	0x00c22024,   0x00001000,
	0x00c22024,   0x00002000,
	0x00c22024,   0x00004000,
	0x00c22024,   0x00008000,
	0x00c22024,   0x00010000,
	0x00c22024,   0x00020000,
	0x00c22024,   0x00040000,
	0x00c22024,   0x00080000,
	0x00c22024,   0x00100000,
	0x00c22024,   0x00200000,
	0x00c22024,   0x00400000,
	0x00c22024,   0x00800000,
	0x00c22024,   0x01000000,
	0x00c22024,   0x02000000,
	0x00c22024,   0x04000000,
	0x00c22024,   0x08000000,
	0x00c22024,   0x10000000,
	0x00c22024,   0x20000000,
	0x00c22024,   0x40000000,
	0x00c22024,   0x80000000,
	0x00406d50,   0x00000001,
	0x00406d50,   0x00000002,
	0x00406d50,   0x00000004,
	0x00406d50,   0x00000008,
	0x00406d50,   0x00000010,
	0x00406d50,   0x00000020,
	0x00406d50,   0x00000040,
	0x00406d50,   0x00000080,
	0x00406d50,   0x00000100,
	0x00406d50,   0x00000200,
	0x00406d50,   0x00000400,
	0x00406d50,   0x00000800,
	0x00406d50,   0x00001000,
	0x00406d50,   0x00002000,
	0x00406d50,   0x00004000,
	0x00406d50,   0x00008000,
	0x00406d50,   0x00010000,
	0x00406d50,   0x00020000,
	0x00406d50,   0x00040000,
	0x00406d50,   0x00080000,
	0x00406d50,   0x00100000,
	0x00406d50,   0x00200000,
	0x00406d50,   0x00400000,
	0x00406d50,   0x00800000,
	0x00406d50,   0x01000000,
	0x00406d50,   0x02000000,
	0x00406d50,   0x04000000,
	0x00406d50,   0x08000000,
	0x00406d50,   0x10000000,
	0x00406d50,   0x20000000,
	0x00406d50,   0x40000000,
	0x00406d50,   0x80000000,
	0x003b3008,   0x00000001,
	0x003b3008,   0x00000001,
	0x003b3008,   0x00000001,
	0x003b3014,   0x00000001,
	0x003b3008,   0x00000002,
	0x003b3008,   0x00000002,
	0x003b3008,   0x00000002,
	0x003b3014,   0x00000002,
	0x003b3008,   0x00000004,
	0x003b3008,   0x00000004,
	0x003b3008,   0x00000004,
	0x003b3014,   0x00000004,
	0x003b0290,   0x00000001,
	0x003b0290,   0x00000000,
	0x003b3008,   0x00000008,
	0x003b3008,   0x00000008,
	0x003b3008,   0x00000008,
	0x003b3014,   0x00000008,

	0x0040000c,   0x00a4cb80, /* SUN_GISB_ARB_... */

	0x00403008,   0x00000001, /* SUN_L2_... */

	0x004000c8,   0x00000001, /* SUN_GISB_ARB_... */

	0x00403014,   0x00000001, /* SUN_L2_... */

	0x00404118,   0x11111131, /* ?? */
	0x0040411c,   0x11111111,
	0x00404120,   0x31111111,
	0x00404124,   0x11112223,
	0x00404128,   0x01111111,
	0x0040412c,   0x33332210,
	0x00404130,   0x01111111,
	0x0040413c,   0x11112222,
	0x00408500,   0x30111000,
	0x00408504,   0x55333633,
	0x00408508,   0x15555554,
	0x0040850c,   0x00000111,
	0x0040439c,   0x00000230,
	0x00404380,   0x00000010,
	0x00422800,   0x00001000,

	0x00420160,   0x00000002, /* BCHP_CLKGEN_... */

	0x00422804,   0x00000020, /* ?? */
	0x00422808,   0x001d0003,
	0x0042280c,   0x0007d634,
	0x00422810,   0x00000000,
	0x00422818,   0x02002710,
	0x00422814,   0x10000000,
	0x00422800,   0x000008a0,

	0x00420160,   0x00000000, /* BCHP_CLKGEN_... */

	0x00406228,   0x000000d1, /* ?? */
	0x00406250,   0x00000040,
	0x00406228,   0x000000d1,

	0x00604000,   0x01000000, /* BVN Front SW Init */
	0x00604000,   0x00000000,

	0x00400008,   0x00000000, /* SUN_GISB_ARB_... */

	0x00604000,   0xfeffffff, /* BVN Front SW Init */
	0x00604000,   0x00000000,

	0x00604018,   0x00000000, /* ?? */

	0x00622800,   0xffffffff, /* BVN Middle SW Init */
	0x00622800,   0x00000000,

	0x00622820,   0x00000000, /* ?? */

	0x00646400,   0xffffffff, /* BVN Back SW Init */
	0x00646400,   0x00000000,

	0x00646418,   0x00000000, /* BVN Back Control */

	0x00404318,   0x00000200, /* SUN_TOP_CTRL_SW_RESET */

	0x0040431c,   0x00000200, /* SUN_TOP_CTRL_SW_INIT_0_CLEAR */

	0x00685070,   0x00000001, /* VEC */
	0x00685070,   0x00000000,
	0x00680004,   0x00000007,
	0x00680008,   0x00000007,
	0x0068000c,   0x00000007,
	0x00680010,   0x00000007,
	0x00680048,   0x0bff42d8,
	0x00680058,   0x228708aa,

	0x00404318,   0x00000400, /* SUN_TOP_CTRL_SW_RESET */

	0x0040431c,   0x00000400, /* SUN_TOP_CTRL_SW_INIT_0_CLEAR */

	0x006a083c,   0x40000000, /* HDMI TX Control */
	0x006a083c,   0x00000000,
	0x006a0870,   0x00002002,
	0x006a0a84,   0x0000000f,
	0x006a0a80,   0x0000003f,
	0x006a0abc,   0x00002301,
	0x006a0898,   0x76235410,

	0x00682058,   0x00003003, /* VEC */
	0x0068205c,   0x0000310a,
	0x00681058,   0x00003003,
	0x0068105c,   0x00002000,
	0x00681058,   0x00003003,
	0x0068105c,   0x00002000,

	0x006a0a88,   0x00a0280a, /* HDMI TX Control */
	0x006a0a8c,   0x0009180a,
	0x006a0a90,   0x21b00000,
	0x006a0984,   0x00000003,
	0x006a0a84,   0x00000000,
	0x006a0a80,   0x00000030,
	0x006a08e0,   0x001002d0,
	0x006a08e4,   0x003c003e,
	0x006a08e8,   0x060901e0,
	0x006a08ec,   0x0000001e,
	0x006a08f0,   0x060901e0,
	0x006a08f4,   0x0000001e,
	0x006a08d8,   0x00000083,
	0x006a087c,   0x00000000,
	0x006a0870,   0x00002002,
	0x006a0870,   0x00002042,

	0x0040864c,   0x80086400, /* ?? */
	0x0040864c,   0x80006400,

	0x006a0870,   0x00006002, /* HDMI TX Control */
	0x006a0870,   0x00006042,
	0x006a0870,   0x00006042,
	0x006a0944,   0xffffffff,
	0x006a0944,   0x00000000,
	0x006a0944,   0xffffffff,
	0x006a0944,   0x00000000,
};

