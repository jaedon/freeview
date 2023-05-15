/***************************************************************************
 File : splash_vdc_reg.h
 Date  : Oct 31 2012 13:34:08
 Description : Register list for splash BVN configuration
 ***************************************************************************/

#include "splash_magnum.h"

uint32_t aulReg[] = 
{

	0x00462810,   0x00000000, /* ?? */
	0x00462818,   0x00000000,

	0x007d0ac0,   0x00000010, /* HDMI... */
	0x007d0ac4,   0x8c060000,
	0x007d0ac8,   0x051cfe08,
	0x007d0acc,   0x00a22004,
	0x007d0ac0,   0x00000000,

	0x0040c1f8,   0x00000004, /* ?? */

	0x00404014,   0x00000dc0, /* SUN_TOP_CTRL_SW_RESET */
	0x00404014,   0x00000000,

	0x00040818,   0x00000008, /* ?? */
	0x00040980,   0x00000010,
	0x00407090,   0x00000800,
	0x00407090,   0x00001000,
	0x00407090,   0x00002000,
	0x00407090,   0x00004000,
	0x00407090,   0x00008000,
	0x00407090,   0x00010000,
	0x00407090,   0x00020000,

	0x00401810,   0x00000001, /* SUN_L2_... */
	0x00401810,   0x00000040,
	0x00401810,   0x00000080,
	0x00401810,   0x00000100,
	0x00401810,   0x00000200,
	0x00401810,   0x00000400,
	0x00401810,   0x00000800,
	0x00401810,   0x00001000,
	0x00401810,   0x00002000,
	0x00401810,   0x00004000,
	0x00401810,   0x00008000,
	0x00401810,   0x00010000,
	0x00401810,   0x00020000,
	0x00401810,   0x00040000,
	0x00401810,   0x00080000,

	0x003b3010,   0x00000001, /* ?? */
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

	0x00401808,   0x00000001, /* SUN_L2_... */

	0x004000c8,   0x00000001, /* SUN_GISB_ARB_... */

	0x00401814,   0x00000001, /* SUN_L2_... */

	0x00404100,   0x00000000, /* ?? */
	0x00404104,   0x00000000,
	0x00404108,   0x11110100,
	0x0040410c,   0x11001133,
	0x00404110,   0x11221200,
	0x00404114,   0x11111111,
	0x00404118,   0x01111111,
	0x0040411c,   0x11110000,
	0x00404120,   0x00000011,
	0x0040421c,   0x00000320,
	0x00404200,   0x00000008,
	0x00462900,   0x00001000,
	0x00462800,   0x00000022,
	0x00462904,   0x00000020,
	0x00462908,   0x001d0003,
	0x0046290c,   0x0007d634,
	0x00462910,   0x00000000,
	0x00462918,   0x02002710,
	0x00462914,   0x10000000,
	0x00462900,   0x000008a0,
	0x00462800,   0x00000020,
	0x00406228,   0x000000d0,
	0x00406250,   0x00000040,
	0x00406228,   0x000000d0,

	0x00784000,   0x01000000, /* BCHP_FMISC_SOFT_RESET */
	0x00784000,   0x00000000,

	0x00400008,   0x00000000, /* SUN_GISB_ARB_... */

	0x00784000,   0xfeffffff, /* BCHP_FMISC_SOFT_RESET */
	0x00784000,   0x00000000,

	0x00784018,   0x00000000, /* ?? */

	0x007e8000,   0xffffffff, /* BCHP_DMISC_SOFT_RESET */
	0x007e8000,   0x00000000,

	0x007e8010,   0x00000000, /* ?? */

	0x007a2800,   0xffffffff, /* BCHP_MMISC_SOFT_RESET */
	0x007a2800,   0x00000000,

	0x007a2820,   0x00000000, /* ?? */

	0x007c6400,   0xffffffff, /* BCHP_BMISC_SOFT_RESET */
	0x007c6400,   0x00000000,

	0x007c6418,   0x00000000, /* ?? */

	0x00404014,   0x00000800, /* SUN_TOP_CTRL_SW_RESET */
	0x00404014,   0x00000000,

	0x00462900,   0x000008a0, /* ?? */
	0x00462904,   0x00000020,
	0x00462908,   0x001d0003,
	0x0046290c,   0x0007d634,
	0x00462910,   0x00022f00,
	0x00462914,   0x10000000,
	0x00462918,   0x02002710,
	0x0046291c,   0x00000000,
	0x00462920,   0x00000000,
	0x00462924,   0x00000000,
	0x00462928,   0x003fffe6,
	0x0046292c,   0x00000000,
	0x007f4868,   0x00000001,
	0x007f4868,   0x00000000,
	0x007f0034,   0x00000635,
	0x007f0004,   0x00000007,
	0x007f0038,   0x00002000,
	0x007f0008,   0x00000007,
	0x007f003c,   0x00002000,
	0x007f000c,   0x00000007,
	0x007f0040,   0x00002000,
	0x007f0010,   0x00000007,
	0x007f0044,   0x00002000,

	0x00404014,   0x00000200, /* SUN_TOP_CTRL_SW_RESET */
	0x00404014,   0x00000000,

	0x007d0854,   0x40000000, /* HDMI... */
	0x007d0854,   0x00000000,
	0x007d0880,   0x00002002,
	0x007d0ac4,   0x8c060000,
	0x007d0ac0,   0x003f001f,
	0x007d0ac4,   0x8c064444,
	0x007d0ac4,   0x80064444,
	0x007d08a8,   0x76235410,

	0x007f105c,   0x00001803, /* ?? */
	0x007f1060,   0x00001000,
	0x007f205c,   0x00001803,
	0x007f2060,   0x0000190a,
	0x007f105c,   0x00001803,
	0x007f1060,   0x00001000,

	0x007d0ac4,   0x8c060000, /* HDMI... */
	0x007d0acc,   0x00a22004,
	0x007d09c4,   0x00000003,
	0x007d0ac0,   0x00000010,
	0x007d08f0,   0x000002d0,
	0x007d08f4,   0x0f03e010,
	0x007d08f8,   0x060901e0,
	0x007d08fc,   0x0000001e,
	0x007d0900,   0x060901e0,
	0x007d0904,   0x0000001e,
	0x007d08e8,   0x00000083,
	0x007d088c,   0x00000000,
	0x007d0880,   0x00002002,
	0x007d0880,   0x00002042,
	0x007d0808,   0x00000007,
	0x007d0808,   0x00000006,
	0x007d0880,   0x00006002,
	0x007d0880,   0x00006042,
	0x007d0880,   0x00006042,
	0x007d0988,   0xffffffff,
	0x007d0988,   0x00000000,
	0x007d0988,   0xffffffff,
	0x007d0988,   0x00000000,
};

