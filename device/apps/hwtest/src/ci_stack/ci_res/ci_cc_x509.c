/***************************************************************
* $Workfile:   ci_cc_x509.c  $
* $Modtime:
* Auther : hmkim / dslee
*
* Desc : Content Control (CI+) Addon for X.509 parsing
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
****************************************************************/


/***************************************************************
* #ifdef statements and include ////////////////////////////////
***************************************************************/

#include "ci_cc_x509.h"
#include "ci_cc_cipher.h"	// for V_RSA()


/***************************************************************
* debugging ////////////////////////////////////////////////////
***************************************************************/

//#define CHECK_X509_ParseRawCert

#ifndef CHECK_X509_ParseRawCert

#define PrintString(x)
#define PrintValue(x,y,z)
#define PrintHex(x,y,z)
#define PrintArcs(x,y,z)

#else

#define PrintString(x) VK_Print x

void PrintValue(char *valueTag, unsigned char *value, int size)
{
	int i;

	VK_Print("%s", valueTag);
	for (i = 0; i < size; i++)
	{
		VK_Print(" %02x", value[i]);
	}
	VK_Print("\n");
}
void PrintHex(char *valueTag, unsigned char *value, int size)
{
	int i;

	VK_Print("%s", valueTag);
	for (i = 0; i < size; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n       ");
		VK_Print(" %02x", value[i]);
	}
	VK_Print("\n");
}
void PrintArcs(char *valueTag, unsigned long *value, int size)
{
	int i;

	VK_Print("%s", valueTag);
	for (i = 0; i < size-1; i++)
	{
		VK_Print("%ld.", value[i]);
	}
	VK_Print("%ld\n", value[i]);
}
void X509_ParseRawCert_Test(void)
{
	// sample-Certificate-1.der
	unsigned char ucRawCert1[1128] = {
		0x30, 0x82, 0x04, 0x64, 0x30, 0x82, 0x03, 0x4C, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x10, 0x44,
		0xBE, 0x0C, 0x8B, 0x50, 0x00, 0x24, 0xB4, 0x11, 0xD3, 0x36, 0x30, 0x4B, 0xC0, 0x33, 0x77, 0x30,
		0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 0x81,
		0xA3, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x0B,
		0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x13, 0x02, 0x55, 0x54, 0x31, 0x17, 0x30, 0x15, 0x06,
		0x03, 0x55, 0x04, 0x07, 0x13, 0x0E, 0x53, 0x61, 0x6C, 0x74, 0x20, 0x4C, 0x61, 0x6B, 0x65, 0x20,
		0x43, 0x69, 0x74, 0x79, 0x31, 0x1E, 0x30, 0x1C, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x13, 0x15, 0x54,
		0x68, 0x65, 0x20, 0x55, 0x53, 0x45, 0x52, 0x54, 0x52, 0x55, 0x53, 0x54, 0x20, 0x4E, 0x65, 0x74,
		0x77, 0x6F, 0x72, 0x6B, 0x31, 0x21, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x13, 0x18, 0x68,
		0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x75, 0x73, 0x65, 0x72, 0x74, 0x72,
		0x75, 0x73, 0x74, 0x2E, 0x63, 0x6F, 0x6D, 0x31, 0x2B, 0x30, 0x29, 0x06, 0x03, 0x55, 0x04, 0x03,
		0x13, 0x22, 0x55, 0x54, 0x4E, 0x2D, 0x55, 0x53, 0x45, 0x52, 0x46, 0x69, 0x72, 0x73, 0x74, 0x2D,
		0x4E, 0x65, 0x74, 0x77, 0x6F, 0x72, 0x6B, 0x20, 0x41, 0x70, 0x70, 0x6C, 0x69, 0x63, 0x61, 0x74,
		0x69, 0x6F, 0x6E, 0x73, 0x30, 0x1E, 0x17, 0x0D, 0x39, 0x39, 0x30, 0x37, 0x30, 0x39, 0x31, 0x38,
		0x34, 0x38, 0x33, 0x39, 0x5A, 0x17, 0x0D, 0x31, 0x39, 0x30, 0x37, 0x30, 0x39, 0x31, 0x38, 0x35,
		0x37, 0x34, 0x39, 0x5A, 0x30, 0x81, 0xA3, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06,
		0x13, 0x02, 0x55, 0x53, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x08, 0x13, 0x02, 0x55,
		0x54, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x07, 0x13, 0x0E, 0x53, 0x61, 0x6C, 0x74,
		0x20, 0x4C, 0x61, 0x6B, 0x65, 0x20, 0x43, 0x69, 0x74, 0x79, 0x31, 0x1E, 0x30, 0x1C, 0x06, 0x03,
		0x55, 0x04, 0x0A, 0x13, 0x15, 0x54, 0x68, 0x65, 0x20, 0x55, 0x53, 0x45, 0x52, 0x54, 0x52, 0x55,
		0x53, 0x54, 0x20, 0x4E, 0x65, 0x74, 0x77, 0x6F, 0x72, 0x6B, 0x31, 0x21, 0x30, 0x1F, 0x06, 0x03,
		0x55, 0x04, 0x0B, 0x13, 0x18, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E,
		0x75, 0x73, 0x65, 0x72, 0x74, 0x72, 0x75, 0x73, 0x74, 0x2E, 0x63, 0x6F, 0x6D, 0x31, 0x2B, 0x30,
		0x29, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x22, 0x55, 0x54, 0x4E, 0x2D, 0x55, 0x53, 0x45, 0x52,
		0x46, 0x69, 0x72, 0x73, 0x74, 0x2D, 0x4E, 0x65, 0x74, 0x77, 0x6F, 0x72, 0x6B, 0x20, 0x41, 0x70,
		0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x73, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0D,
		0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01,
		0x0F, 0x00, 0x30, 0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 0x00, 0xB3, 0xFB, 0x91, 0xA1, 0xE4,
		0x36, 0x55, 0x85, 0xAC, 0x06, 0x34, 0x5B, 0xA0, 0x9A, 0x58, 0xB2, 0xF8, 0xB5, 0x0F, 0x05, 0x77,
		0x83, 0xAE, 0x32, 0xB1, 0x76, 0x92, 0x68, 0xEC, 0x23, 0x4A, 0xC9, 0x76, 0x3F, 0xE3, 0x9C, 0xB6,
		0x37, 0x79, 0x03, 0xB9, 0xAB, 0x69, 0x8D, 0x07, 0x25, 0xB6, 0x19, 0x67, 0xE4, 0xB0, 0x1B, 0x18,
		0x73, 0x61, 0x4A, 0xE8, 0x7E, 0xCD, 0xD3, 0x2F, 0x64, 0xE3, 0xA6, 0x7C, 0x0C, 0xFA, 0x17, 0x80,
		0xA3, 0x0D, 0x47, 0x89, 0x4F, 0x51, 0x71, 0x2F, 0xEE, 0xFC, 0x3F, 0xF9, 0xB8, 0x16, 0x80, 0x87,
		0x89, 0x93, 0x25, 0x20, 0x9A, 0x43, 0x82, 0x69, 0x24, 0x76, 0x28, 0x59, 0x35, 0xA1, 0x1D, 0xC0,
		0x7F, 0x83, 0x06, 0x64, 0x16, 0x20, 0x2C, 0xD3, 0x49, 0xA4, 0x85, 0xB4, 0xC0, 0x61, 0x7F, 0x51,
		0x08, 0xF8, 0x68, 0x15, 0x91, 0x80, 0xCB, 0xA5, 0xD5, 0xEE, 0x3B, 0x3A, 0xF4, 0x84, 0x04, 0x5E,
		0x60, 0x59, 0xA7, 0x8C, 0x34, 0x72, 0xEE, 0xB8, 0x78, 0xC5, 0xD1, 0x3B, 0x12, 0x4A, 0x6F, 0x7E,
		0x65, 0x27, 0xB9, 0xA4, 0x55, 0xC5, 0xB9, 0x6F, 0x43, 0xA4, 0xC5, 0x1D, 0x2C, 0x99, 0xC0, 0x52,
		0xA4, 0x78, 0x4C, 0x15, 0xB3, 0x40, 0x98, 0x08, 0x6B, 0x43, 0xC6, 0x01, 0xB0, 0x7A, 0x7B, 0xF5,
		0x6B, 0x1C, 0x22, 0x3F, 0xCB, 0xEF, 0xFF, 0xA8, 0xD0, 0x3A, 0x4B, 0x76, 0x15, 0x9E, 0xD2, 0xD1,
		0xC6, 0x2E, 0xE3, 0xDB, 0x57, 0x1B, 0x32, 0xA2, 0xB8, 0x6F, 0xE8, 0x86, 0xA6, 0x3F, 0x70, 0xAB,
		0xE5, 0x70, 0x92, 0xAB, 0x44, 0x1E, 0x40, 0x50, 0xFB, 0x9C, 0xA3, 0x62, 0xE4, 0x6C, 0x6E, 0xA0,
		0xC8, 0xDE, 0xE2, 0x80, 0x42, 0xFA, 0xE9, 0x2F, 0xE8, 0xCE, 0x32, 0x04, 0x8F, 0x7C, 0x8D, 0xB7,
		0x1C, 0xA3, 0x35, 0x3C, 0x15, 0xDD, 0x9E, 0xC3, 0xAE, 0x97, 0xA5, 0x02, 0x03, 0x01, 0x00, 0x01,
		0xA3, 0x81, 0x91, 0x30, 0x81, 0x8E, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x1D, 0x0F, 0x04, 0x04, 0x03,
		0x02, 0x01, 0xC6, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x1D, 0x13, 0x01, 0x01, 0xFF, 0x04, 0x05, 0x30,
		0x03, 0x01, 0x01, 0xFF, 0x30, 0x1D, 0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0xFA,
		0x86, 0xC9, 0xDB, 0xE0, 0xBA, 0xE9, 0x78, 0xF5, 0x4B, 0xA8, 0xD6, 0x15, 0xDF, 0xF0, 0xD3, 0xE1,
		0x6A, 0x14, 0x3C, 0x30, 0x4F, 0x06, 0x03, 0x55, 0x1D, 0x1F, 0x04, 0x48, 0x30, 0x46, 0x30, 0x44,
		0xA0, 0x42, 0xA0, 0x40, 0x86, 0x3E, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x63, 0x72, 0x6C,
		0x2E, 0x75, 0x73, 0x65, 0x72, 0x74, 0x72, 0x75, 0x73, 0x74, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x55,
		0x54, 0x4E, 0x2D, 0x55, 0x53, 0x45, 0x52, 0x46, 0x69, 0x72, 0x73, 0x74, 0x2D, 0x4E, 0x65, 0x74,
		0x77, 0x6F, 0x72, 0x6B, 0x41, 0x70, 0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x73,
		0x2E, 0x63, 0x72, 0x6C, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01,
		0x05, 0x05, 0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0xA4, 0xF3, 0x25, 0xCC, 0xD1, 0xD4, 0x91, 0x83,
		0x22, 0xD0, 0xCC, 0x32, 0xAB, 0x9B, 0x96, 0x4E, 0x34, 0x91, 0x54, 0x20, 0x25, 0x34, 0x61, 0x5F,
		0x2A, 0x02, 0x15, 0xE1, 0x8B, 0xAA, 0xFF, 0x7D, 0x64, 0x51, 0xCF, 0x0A, 0xFF, 0xBC, 0x7D, 0xD8,
		0x21, 0x6A, 0x78, 0xCB, 0x2F, 0x51, 0x6F, 0xF8, 0x42, 0x1D, 0x33, 0xBD, 0xEB, 0xB5, 0x7B, 0x94,
		0xC3, 0xC3, 0xA9, 0xA0, 0x2D, 0xDF, 0xD1, 0x29, 0x1F, 0x1D, 0xFE, 0x8F, 0x3F, 0xBB, 0xA8, 0x45,
		0x2A, 0x7F, 0xD1, 0x6E, 0x55, 0x24, 0xE2, 0xBB, 0x02, 0xFB, 0x31, 0x3F, 0xBE, 0xE8, 0xBC, 0xEC,
		0x40, 0x2B, 0xF8, 0x01, 0xD4, 0x56, 0x38, 0xE4, 0xCA, 0x44, 0x82, 0xB5, 0x61, 0x20, 0x21, 0x67,
		0x65, 0xF6, 0xF0, 0x0B, 0xE7, 0x34, 0xF8, 0xA5, 0xC2, 0x9C, 0xA3, 0x5C, 0x40, 0x1F, 0x85, 0x93,
		0x95, 0x06, 0xDE, 0x4F, 0xD4, 0x27, 0xA9, 0xB6, 0xA5, 0xFC, 0x16, 0xCD, 0x73, 0x31, 0x3F, 0xB8,
		0x65, 0x27, 0xCF, 0xD4, 0x53, 0x1A, 0xF0, 0xAC, 0x6E, 0x9F, 0x4F, 0x05, 0x0C, 0x03, 0x81, 0xA7,
		0x84, 0x29, 0xC4, 0x5A, 0xBD, 0x64, 0x57, 0x72, 0xAD, 0x3B, 0xCF, 0x37, 0x18, 0xA6, 0x98, 0xC6,
		0xAD, 0x06, 0xB4, 0xDC, 0x08, 0xA3, 0x04, 0xD5, 0x29, 0xA4, 0x96, 0x9A, 0x12, 0x67, 0x4A, 0x8C,
		0x60, 0x45, 0x9D, 0xF1, 0x23, 0x9A, 0xB0, 0x00, 0x9C, 0x68, 0xB5, 0x98, 0x50, 0xD3, 0xEF, 0x8E,
		0x2E, 0x92, 0x65, 0xB1, 0x48, 0x3E, 0x21, 0xBE, 0x15, 0x30, 0x2A, 0x0D, 0xB5, 0x0C, 0xA3, 0x6B,
		0x3F, 0xAE, 0x7F, 0x57, 0xF5, 0x1F, 0x96, 0x7C, 0xDF, 0x6F, 0xDD, 0x82, 0x30, 0x2C, 0x65, 0x1B,
		0x40, 0x4A, 0xCD, 0x68, 0xB9, 0x72, 0xEC, 0x71, 0x76, 0xEC, 0x54, 0x8E, 0x1F, 0x85, 0x0C, 0x01,
		0x6A, 0xFA, 0xA6, 0x38, 0xAC, 0x1F, 0xC4, 0x84
	};
	// ciplusBrandBCert (from SmarDTV test kit)
	unsigned char ucRawCert2[1089] = {
		0x30, 0x82, 0x04, 0x3D, 0x30, 0x82, 0x02, 0xFC, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x02, 0x10,
		0x05, 0x30, 0x36, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0A, 0x30, 0x29,
		0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x30, 0x16, 0x06, 0x09, 0x2A,
		0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x08, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02,
		0x1A, 0x05, 0x00, 0x02, 0x01, 0x14, 0x02, 0x01, 0x01, 0x30, 0x81, 0x9A, 0x31, 0x0B, 0x30, 0x09,
		0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x46, 0x52, 0x31, 0x0C, 0x30, 0x0A, 0x06, 0x03, 0x55,
		0x04, 0x08, 0x0C, 0x03, 0x44, 0x31, 0x33, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07,
		0x0C, 0x09, 0x4C, 0x61, 0x20, 0x43, 0x69, 0x6F, 0x74, 0x61, 0x74, 0x31, 0x25, 0x30, 0x23, 0x06,
		0x03, 0x55, 0x04, 0x0A, 0x0C, 0x1C, 0x2D, 0x2D, 0x20, 0x53, 0x6D, 0x61, 0x72, 0x44, 0x54, 0x56,
		0x20, 0x53, 0x41, 0x20, 0x2D, 0x2D, 0x2D, 0x20, 0x43, 0x49, 0x2B, 0x20, 0x52, 0x4F, 0x54, 0x20,
		0x2D, 0x2D, 0x31, 0x11, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x08, 0x43, 0x49, 0x2B,
		0x20, 0x74, 0x65, 0x61, 0x6D, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x04,
		0x74, 0x65, 0x73, 0x74, 0x31, 0x20, 0x30, 0x1E, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x17, 0x43,
		0x49, 0x2B, 0x20, 0x72, 0x6F, 0x6F, 0x74, 0x20, 0x43, 0x41, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69,
		0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 0x30, 0x1E, 0x17, 0x0D, 0x30, 0x38, 0x30, 0x34, 0x31, 0x36,
		0x31, 0x34, 0x33, 0x33, 0x34, 0x31, 0x5A, 0x17, 0x0D, 0x30, 0x39, 0x30, 0x38, 0x32, 0x39, 0x31,
		0x34, 0x33, 0x33, 0x34, 0x31, 0x5A, 0x30, 0x6E, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04,
		0x06, 0x13, 0x02, 0x46, 0x52, 0x31, 0x0C, 0x30, 0x0A, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0C, 0x03,
		0x44, 0x31, 0x33, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0C, 0x09, 0x4C, 0x61,
		0x20, 0x43, 0x69, 0x6F, 0x74, 0x61, 0x74, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x0A,
		0x0C, 0x07, 0x42, 0x72, 0x61, 0x6E, 0x64, 0x20, 0x42, 0x31, 0x0D, 0x30, 0x0B, 0x06, 0x03, 0x55,
		0x04, 0x0B, 0x0C, 0x04, 0x74, 0x65, 0x73, 0x74, 0x31, 0x1C, 0x30, 0x1A, 0x06, 0x03, 0x55, 0x04,
		0x03, 0x0C, 0x13, 0x43, 0x49, 0x2B, 0x20, 0x52, 0x4F, 0x54, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x62,
		0x72, 0x61, 0x6E, 0x64, 0x20, 0x42, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86,
		0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0F, 0x00, 0x30, 0x82,
		0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 0x00, 0xAF, 0x4F, 0x8A, 0x97, 0xDE, 0x96, 0x20, 0x6E, 0x10,
		0x0E, 0xEE, 0xDA, 0xB7, 0x0E, 0xC0, 0x26, 0x34, 0xAA, 0xAA, 0xE7, 0xBF, 0xFF, 0x38, 0xF4, 0xBA,
		0xC4, 0xDF, 0xD6, 0x69, 0xDB, 0x93, 0x63, 0x7D, 0x0E, 0x72, 0x17, 0x43, 0xBD, 0x0B, 0x41, 0x4C,
		0x3F, 0xEC, 0x84, 0x41, 0x8A, 0x18, 0xFD, 0x15, 0xBB, 0x09, 0xE5, 0x1F, 0xFD, 0xEF, 0x36, 0x84,
		0x6E, 0xF5, 0x6F, 0x20, 0x9D, 0x9A, 0x74, 0x8C, 0x5B, 0x7C, 0x35, 0x3B, 0x62, 0x1F, 0x95, 0xB6,
		0x8B, 0x98, 0xDA, 0x4E, 0x31, 0x35, 0x6F, 0xCD, 0x5D, 0x7B, 0xC8, 0x39, 0x1C, 0x6A, 0x62, 0x29,
		0x58, 0xAE, 0x19, 0xC7, 0xC3, 0xF9, 0xF8, 0xDF, 0x67, 0x0C, 0x8D, 0xD6, 0x16, 0x11, 0x7F, 0x9D,
		0xB5, 0x30, 0x2F, 0xEB, 0xE4, 0xC2, 0x42, 0xBD, 0x95, 0x09, 0xC2, 0x56, 0x54, 0x82, 0x93, 0xED,
		0xD5, 0x81, 0xD9, 0x82, 0x41, 0xD0, 0x60, 0x21, 0xD7, 0x95, 0xA0, 0x64, 0x7C, 0xCC, 0x70, 0x79,
		0xDD, 0xA5, 0xFF, 0xA0, 0x96, 0xEE, 0xAC, 0x0A, 0xED, 0x0D, 0xBB, 0x48, 0x69, 0x50, 0x14, 0xF2,
		0x40, 0x9C, 0x18, 0x7B, 0xE6, 0xE2, 0x02, 0x7B, 0x02, 0xF4, 0x57, 0x94, 0x9B, 0xF0, 0xF9, 0x47,
		0x75, 0xA8, 0x34, 0xBE, 0x3D, 0xA6, 0xC8, 0x3F, 0xA0, 0xC8, 0xAC, 0xE3, 0x8A, 0x39, 0xC4, 0xBC,
		0xC7, 0x6B, 0x73, 0xCF, 0x6E, 0x45, 0xA4, 0x36, 0x63, 0x2F, 0x7F, 0xDB, 0x1A, 0x8F, 0xB9, 0x29,
		0xAC, 0x74, 0x93, 0xAF, 0xA1, 0x30, 0x9A, 0x8A, 0x3B, 0x39, 0xD7, 0x5F, 0x76, 0x15, 0x49, 0x52,
		0x99, 0x79, 0xFB, 0xBA, 0x44, 0x83, 0x52, 0xC1, 0x79, 0x2A, 0xAF, 0x30, 0xC9, 0x9D, 0x87, 0xF0,
		0xFF, 0xD4, 0xB4, 0xF3, 0xCE, 0x7D, 0xCC, 0x95, 0x87, 0x4C, 0xB6, 0x00, 0x86, 0x0B, 0x19, 0xD3,
		0x1F, 0xCC, 0xDC, 0x22, 0x7E, 0x14, 0xF7, 0x02, 0x03, 0x01, 0x00, 0x01, 0xA3, 0x66, 0x30, 0x64,
		0x30, 0x12, 0x06, 0x03, 0x55, 0x1D, 0x13, 0x01, 0x01, 0xFF, 0x04, 0x08, 0x30, 0x06, 0x01, 0x01,
		0xFF, 0x02, 0x01, 0x00, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x1D, 0x0F, 0x01, 0x01, 0xFF, 0x04, 0x04,
		0x03, 0x02, 0x02, 0x04, 0x30, 0x1D, 0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0x33,
		0x30, 0xF0, 0x89, 0x99, 0x49, 0x0E, 0xA1, 0x47, 0xD9, 0x6F, 0x5D, 0xB2, 0x00, 0x86, 0x61, 0x56,
		0x01, 0x0F, 0xEA, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14,
		0x55, 0xC9, 0x18, 0xF8, 0x8F, 0x06, 0xD1, 0x8A, 0xB5, 0xE0, 0xC1, 0xFC, 0x3B, 0x55, 0x39, 0xE9,
		0xDE, 0xA4, 0x3C, 0x88, 0x30, 0x36, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01,
		0x0A, 0x30, 0x29, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x30, 0x16,
		0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x08, 0x30, 0x09, 0x06, 0x05, 0x2B,
		0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x02, 0x01, 0x14, 0x02, 0x01, 0x01, 0x03, 0x82, 0x01, 0x01,
		0x00, 0x25, 0x7F, 0x53, 0x11, 0x52, 0x9D, 0x4D, 0x71, 0x08, 0x2A, 0x39, 0xEA, 0x1E, 0x3B, 0x86,
		0x3F, 0x09, 0x55, 0xD8, 0x97, 0xED, 0x08, 0x48, 0xF9, 0x21, 0x2E, 0x0B, 0x98, 0x83, 0x18, 0x22,
		0xD4, 0xE8, 0x4D, 0xC4, 0x9A, 0xF0, 0x4C, 0x70, 0x9F, 0xAA, 0x29, 0xD4, 0x9D, 0x23, 0xBB, 0xDE,
		0x21, 0xFE, 0x8C, 0xA7, 0xE8, 0x83, 0x09, 0x5B, 0x08, 0xD9, 0xDA, 0x10, 0xD1, 0xEE, 0xB8, 0x23,
		0xA9, 0x59, 0x97, 0x76, 0xCB, 0x9B, 0xF0, 0x3C, 0xE9, 0xAD, 0x38, 0xED, 0x7D, 0x19, 0xCA, 0xEB,
		0x60, 0x9C, 0x39, 0x68, 0xE0, 0x2B, 0xED, 0x8E, 0x1C, 0x9E, 0x60, 0xC6, 0xA7, 0x84, 0x23, 0xF7,
		0x39, 0x53, 0xAB, 0xAB, 0x14, 0x08, 0xE5, 0x57, 0x43, 0xDF, 0x7A, 0xEE, 0xE1, 0x21, 0x4C, 0xF0,
		0xD7, 0xFB, 0x9A, 0xDC, 0xE4, 0x3C, 0x0B, 0x91, 0x92, 0x5E, 0x3D, 0xEA, 0x05, 0x5F, 0xCE, 0xF0,
		0xD3, 0xF2, 0x7A, 0x15, 0x05, 0x69, 0x46, 0xEA, 0xD8, 0x6B, 0xBA, 0xBA, 0xEA, 0x8C, 0xB5, 0x01,
		0x94, 0x05, 0xFC, 0x82, 0x32, 0xA9, 0x24, 0xB7, 0x68, 0x18, 0x95, 0x66, 0x04, 0x17, 0x56, 0x4D,
		0x58, 0x28, 0xDB, 0x88, 0x51, 0xC8, 0x2B, 0xAA, 0xB9, 0x7C, 0xBB, 0xB8, 0x9C, 0x3D, 0xAF, 0xB8,
		0x54, 0xA1, 0xB9, 0x17, 0x34, 0xEE, 0x63, 0x7D, 0xA8, 0x02, 0x60, 0x0B, 0x88, 0x0D, 0xCF, 0xFF,
		0xAB, 0xCC, 0xB7, 0x7D, 0x0E, 0xA4, 0x27, 0x00, 0xE8, 0xE7, 0x37, 0xD8, 0x1F, 0x88, 0x67, 0x18,
		0xF4, 0xD0, 0x22, 0x2E, 0x22, 0x58, 0xF5, 0x2E, 0x8C, 0x29, 0x0D, 0x56, 0xC6, 0x2F, 0x14, 0x00,
		0xB8, 0x44, 0xB3, 0x10, 0x12, 0x5A, 0xFA, 0x56, 0x13, 0x3D, 0xE6, 0xEA, 0xC0, 0x6A, 0xE7, 0x87,
		0x2E, 0x93, 0xC3, 0xB4, 0x87, 0x11, 0x9A, 0x99, 0x28, 0xCD, 0x34, 0xD0, 0x34, 0x7A, 0x52, 0xEB,
		0xA7
	};
	// ciplusDevice3Cert (from SmarDTV test kit)
	unsigned char ucRawCert3[1033] = {
		0x30, 0x82, 0x04, 0x05, 0x30, 0x82, 0x02, 0xC4, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x02, 0x30,
		0x0F, 0x30, 0x36, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0A, 0x30, 0x29,
		0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x30, 0x16, 0x06, 0x09, 0x2A,
		0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x08, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02,
		0x1A, 0x05, 0x00, 0x02, 0x01, 0x14, 0x02, 0x01, 0x01, 0x30, 0x6E, 0x31, 0x0B, 0x30, 0x09, 0x06,
		0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x46, 0x52, 0x31, 0x0C, 0x30, 0x0A, 0x06, 0x03, 0x55, 0x04,
		0x08, 0x0C, 0x03, 0x44, 0x31, 0x33, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0C,
		0x09, 0x4C, 0x61, 0x20, 0x43, 0x69, 0x6F, 0x74, 0x61, 0x74, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03,
		0x55, 0x04, 0x0A, 0x0C, 0x07, 0x42, 0x72, 0x61, 0x6E, 0x64, 0x20, 0x42, 0x31, 0x0D, 0x30, 0x0B,
		0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x04, 0x74, 0x65, 0x73, 0x74, 0x31, 0x1C, 0x30, 0x1A, 0x06,
		0x03, 0x55, 0x04, 0x03, 0x0C, 0x13, 0x43, 0x49, 0x2B, 0x20, 0x52, 0x4F, 0x54, 0x20, 0x66, 0x6F,
		0x72, 0x20, 0x62, 0x72, 0x61, 0x6E, 0x64, 0x20, 0x42, 0x30, 0x1E, 0x17, 0x0D, 0x30, 0x38, 0x30,
		0x34, 0x31, 0x36, 0x31, 0x34, 0x35, 0x38, 0x30, 0x30, 0x5A, 0x17, 0x0D, 0x30, 0x39, 0x30, 0x38,
		0x32, 0x39, 0x31, 0x34, 0x35, 0x38, 0x30, 0x30, 0x5A, 0x30, 0x6F, 0x31, 0x0B, 0x30, 0x09, 0x06,
		0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x46, 0x52, 0x31, 0x0C, 0x30, 0x0A, 0x06, 0x03, 0x55, 0x04,
		0x08, 0x0C, 0x03, 0x44, 0x31, 0x33, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x07, 0x0C,
		0x09, 0x4C, 0x61, 0x20, 0x43, 0x69, 0x6F, 0x74, 0x61, 0x74, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03,
		0x55, 0x04, 0x0A, 0x0C, 0x07, 0x42, 0x72, 0x61, 0x6E, 0x64, 0x20, 0x42, 0x31, 0x11, 0x30, 0x0F,
		0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x08, 0x43, 0x49, 0x43, 0x41, 0x4D, 0x20, 0x23, 0x33, 0x31,
		0x19, 0x30, 0x17, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x10, 0x41, 0x31, 0x32, 0x37, 0x38, 0x44,
		0x32, 0x33, 0x30, 0x32, 0x36, 0x46, 0x38, 0x42, 0x45, 0x37, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0D,
		0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01,
		0x0F, 0x00, 0x30, 0x82, 0x01, 0x0A, 0x02, 0x82, 0x01, 0x01, 0x00, 0xEA, 0x11, 0xAC, 0x70, 0xDD,
		0x31, 0xEB, 0xED, 0x27, 0x58, 0x08, 0xF0, 0x9B, 0xB5, 0x3F, 0xDD, 0x09, 0x09, 0x4E, 0x8C, 0x9D,
		0x52, 0xE1, 0xA8, 0x87, 0xB4, 0xB7, 0xD3, 0x9B, 0xCC, 0x39, 0xA9, 0xDA, 0xA2, 0x31, 0xA1, 0x45,
		0xF6, 0xC2, 0x4F, 0xAA, 0xDC, 0x8B, 0xF9, 0xC6, 0x74, 0xB7, 0x3B, 0x06, 0xA2, 0x61, 0xE4, 0x3F,
		0x4B, 0xE3, 0xE5, 0x1C, 0xD2, 0x00, 0x66, 0xC4, 0x89, 0x31, 0x70, 0x1F, 0xE3, 0x84, 0x9A, 0xF8,
		0x4D, 0x33, 0xFD, 0xA5, 0x67, 0x96, 0x44, 0xBB, 0xA9, 0xB6, 0x49, 0x99, 0x22, 0xC2, 0xC4, 0x14,
		0x4F, 0x43, 0x16, 0xBC, 0x81, 0x10, 0xD6, 0xE4, 0xB4, 0xFC, 0x33, 0x00, 0xF5, 0x55, 0xCF, 0x83,
		0x61, 0xBD, 0xF6, 0xC1, 0xA0, 0xA9, 0x56, 0xAF, 0xB3, 0x4F, 0x7C, 0xD2, 0x27, 0x68, 0x16, 0x83,
		0xEB, 0x83, 0xCD, 0x9D, 0x5D, 0x07, 0xB6, 0xF2, 0xCE, 0xB8, 0x8E, 0x17, 0xFC, 0x7D, 0xA4, 0x5C,
		0x46, 0xE5, 0x4B, 0xB4, 0x71, 0x90, 0xAF, 0x4F, 0x5E, 0xB3, 0xDD, 0x81, 0x12, 0x90, 0xE4, 0x75,
		0x72, 0x67, 0x9D, 0x0C, 0x2C, 0xC8, 0x03, 0x4F, 0xA0, 0x7B, 0xB5, 0xC9, 0xDD, 0x52, 0xFF, 0xEE,
		0xE4, 0xDD, 0xC3, 0xAD, 0x26, 0x23, 0x6B, 0xDA, 0x02, 0x23, 0x7C, 0x6F, 0x69, 0xA3, 0x66, 0xFF,
		0x93, 0xDD, 0xE8, 0xED, 0x15, 0xC2, 0x01, 0x53, 0xB1, 0x35, 0xE9, 0x49, 0xAB, 0x22, 0x04, 0xFC,
		0x37, 0xFD, 0x99, 0xF8, 0xCE, 0x7C, 0xD9, 0xFE, 0x6C, 0x83, 0x28, 0x04, 0xF1, 0x08, 0xF8, 0x5A,
		0x9C, 0xBA, 0xE0, 0xE5, 0x33, 0x4C, 0xC6, 0x56, 0x42, 0x44, 0x77, 0x4E, 0x14, 0x38, 0x86, 0xF2,
		0x0B, 0x95, 0xC2, 0xD1, 0xE0, 0xD8, 0x76, 0xAE, 0x27, 0x31, 0xAF, 0x77, 0xEE, 0x8C, 0xA7, 0x18,
		0xDD, 0x6C, 0xA3, 0x0E, 0xB7, 0x24, 0x6C, 0xC1, 0x3E, 0x03, 0x27, 0x02, 0x03, 0x01, 0x00, 0x01,
		0xA3, 0x5A, 0x30, 0x58, 0x30, 0x0C, 0x06, 0x03, 0x55, 0x1D, 0x13, 0x01, 0x01, 0xFF, 0x04, 0x02,
		0x30, 0x00, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x1D, 0x0F, 0x01, 0x01, 0xFF, 0x04, 0x04, 0x03, 0x02,
		0x07, 0x80, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x33,
		0x30, 0xF0, 0x89, 0x99, 0x49, 0x0E, 0xA1, 0x47, 0xD9, 0x6F, 0x5D, 0xB2, 0x00, 0x86, 0x61, 0x56,
		0x01, 0x0F, 0xEA, 0x30, 0x17, 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01, 0x19, 0x01,
		0x01, 0xFF, 0x04, 0x08, 0x30, 0x06, 0x02, 0x01, 0x01, 0x02, 0x01, 0x01, 0x30, 0x36, 0x06, 0x09,
		0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0A, 0x30, 0x29, 0x30, 0x09, 0x06, 0x05, 0x2B,
		0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x30, 0x16, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D,
		0x01, 0x01, 0x08, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x02, 0x01,
		0x14, 0x02, 0x01, 0x01, 0x03, 0x82, 0x01, 0x01, 0x00, 0x02, 0x8B, 0x87, 0xA6, 0x08, 0x4F, 0x98,
		0x57, 0x02, 0x37, 0xD2, 0xAD, 0x25, 0x3C, 0xB4, 0xB5, 0xB1, 0xB1, 0xBB, 0x6D, 0x94, 0x14, 0x88,
		0xC1, 0x89, 0x83, 0x9F, 0xF6, 0xC5, 0x9D, 0x42, 0x32, 0xA6, 0xBB, 0xCC, 0x4D, 0x8F, 0x6A, 0xF3,
		0x6D, 0x0E, 0xAE, 0x6C, 0x35, 0xC7, 0x6A, 0x4A, 0x7D, 0x4B, 0xDA, 0x38, 0x52, 0x99, 0x11, 0x30,
		0xE7, 0xCE, 0x53, 0x11, 0x23, 0xCA, 0xEB, 0xE9, 0x6B, 0xB8, 0x44, 0xDA, 0x33, 0x77, 0x64, 0x58,
		0xDD, 0xB2, 0xFF, 0xFA, 0xB6, 0xB6, 0x4A, 0xBF, 0xFD, 0x19, 0x09, 0xD7, 0xF0, 0x91, 0x34, 0x08,
		0x4C, 0x0A, 0xF6, 0x1D, 0xF5, 0x40, 0x3D, 0xAC, 0xD5, 0x52, 0xC9, 0xBA, 0xBF, 0xC2, 0xC1, 0x6C,
		0x2E, 0x51, 0xF1, 0x96, 0x19, 0x40, 0x76, 0x44, 0x68, 0xDE, 0x05, 0x45, 0x67, 0x8D, 0x74, 0xCC,
		0x7B, 0x2B, 0xB6, 0x2D, 0x45, 0x77, 0xFA, 0x03, 0xE9, 0xB2, 0x0D, 0xDF, 0x2C, 0xF2, 0x1F, 0x37,
		0x92, 0x9E, 0x07, 0x1B, 0xA7, 0x4A, 0x37, 0x30, 0x59, 0x8C, 0x6D, 0x5C, 0xFA, 0xCA, 0xD0, 0x54,
		0x23, 0xAC, 0xDE, 0x0B, 0xB0, 0xED, 0x16, 0xCA, 0x06, 0x41, 0xD3, 0x26, 0x5F, 0xF1, 0x4B, 0xCB,
		0x71, 0xC9, 0x71, 0xC6, 0xA3, 0x42, 0xCA, 0xBF, 0x03, 0x1C, 0x77, 0x94, 0xFA, 0xDD, 0x9C, 0xFA,
		0x77, 0x5D, 0x49, 0x1F, 0x76, 0x1F, 0xDD, 0xDB, 0x94, 0x8C, 0x40, 0x34, 0x87, 0x1A, 0x79, 0xCF,
		0x9E, 0xA8, 0x2A, 0x8A, 0x01, 0x27, 0x43, 0x05, 0x6B, 0xF7, 0x67, 0x21, 0x3D, 0x46, 0xF2, 0x01,
		0x99, 0x92, 0xC1, 0x78, 0xD2, 0xC6, 0xCC, 0x91, 0x92, 0x17, 0x76, 0xD7, 0xA0, 0xD9, 0xE3, 0xB2,
		0xCF, 0xA4, 0xD6, 0x95, 0xDF, 0x9B, 0x6B, 0xBA, 0x5E, 0xEA, 0x24, 0xDD, 0x4E, 0x45, 0x69, 0xA8,
		0x20, 0xC9, 0xB8, 0xFC, 0x55, 0xA5, 0xB7, 0xDE, 0x34
	};

	Certificate_t *pstCert = NULL;

	VK_Print("\n X509_ParseRawCert_Test (sample-Certificate-1.der) Start... \n");
	pstCert = X509_ParseRawCert(ucRawCert1, 1128);
	if (pstCert == NULL)
	{
		VK_Print(" ERROR !!!\n");
		return;
	}
	CA_MEM_Free(pstCert);
	VK_Print(" X509_ParseRawCert_Test (sample-Certificate-1.der) End. \n");

	VK_Print("\n X509_ParseRawCert_Test (ciplusBrandBCert) Start... \n");
	pstCert = X509_ParseRawCert(ucRawCert2, 1089);
	if (pstCert == NULL)
	{
		VK_Print(" ERROR !!!\n");
		return;
	}
	CA_MEM_Free(pstCert);
	VK_Print(" X509_ParseRawCert_Test (ciplusBrandBCert) End. \n");

	VK_Print("\n X509_ParseRawCert_Test (ciplusDevice3Cert) Start... \n");
	pstCert = X509_ParseRawCert(ucRawCert3, 1033);
	if (pstCert == NULL)
	{
		VK_Print(" ERROR !!!\n");
		return;
	}
	CA_MEM_Free(pstCert);
	VK_Print(" X509_ParseRawCert_Test (ciplusDevice3Cert) End. \n");
}

#endif // CHECK_X509_ParseRawCert


/***************************************************************
* porting //////////////////////////////////////////////////////
***************************************************************/

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "AlgorithmIdentifier.h"

static asn_TYPE_member_t asn_MBR_AlgorithmIdentifier_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct AlgorithmIdentifier, algorithm),
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_OBJECT_IDENTIFIER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"algorithm"
		},
	{ ATF_OPEN_TYPE | ATF_POINTER, 1, offsetof(struct AlgorithmIdentifier, parameters),
		-1 /* Ambiguous tag (ANY?) */,
		0,
		&asn_DEF_ANY,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"parameters"
		},
};
static ber_tlv_tag_t asn_DEF_AlgorithmIdentifier_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_AlgorithmIdentifier_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (6 << 2)), 0, 0, 0 } /* algorithm at 385 */
};
static asn_SEQUENCE_specifics_t asn_SPC_AlgorithmIdentifier_specs_1 = {
	sizeof(struct AlgorithmIdentifier),
	offsetof(struct AlgorithmIdentifier, _asn_ctx),
	asn_MAP_AlgorithmIdentifier_tag2el_1,
	1,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_AlgorithmIdentifier = {
	"AlgorithmIdentifier",
	"AlgorithmIdentifier",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_AlgorithmIdentifier_tags_1,
	sizeof(asn_DEF_AlgorithmIdentifier_tags_1)
		/sizeof(asn_DEF_AlgorithmIdentifier_tags_1[0]), /* 1 */
	asn_DEF_AlgorithmIdentifier_tags_1,	/* Same as above */
	sizeof(asn_DEF_AlgorithmIdentifier_tags_1)
		/sizeof(asn_DEF_AlgorithmIdentifier_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_AlgorithmIdentifier_1,
	2,	/* Elements count */
	&asn_SPC_AlgorithmIdentifier_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "CertificateSerialNumber.h"

int
CertificateSerialNumber_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_INTEGER.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using INTEGER,
 * so here we adjust the DEF accordingly.
 */
static void
CertificateSerialNumber_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_INTEGER.free_struct;
	td->print_struct   = asn_DEF_INTEGER.print_struct;
	td->ber_decoder    = asn_DEF_INTEGER.ber_decoder;
	td->der_encoder    = asn_DEF_INTEGER.der_encoder;
	td->xer_decoder    = asn_DEF_INTEGER.xer_decoder;
	td->xer_encoder    = asn_DEF_INTEGER.xer_encoder;
	td->uper_decoder   = asn_DEF_INTEGER.uper_decoder;
	td->uper_encoder   = asn_DEF_INTEGER.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_INTEGER.per_constraints;
	td->elements       = asn_DEF_INTEGER.elements;
	td->elements_count = asn_DEF_INTEGER.elements_count;
	td->specifics      = asn_DEF_INTEGER.specifics;
}

void
CertificateSerialNumber_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	CertificateSerialNumber_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
CertificateSerialNumber_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	CertificateSerialNumber_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
CertificateSerialNumber_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	CertificateSerialNumber_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
CertificateSerialNumber_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	CertificateSerialNumber_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
CertificateSerialNumber_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	CertificateSerialNumber_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
CertificateSerialNumber_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	CertificateSerialNumber_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_CertificateSerialNumber_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
asn_TYPE_descriptor_t asn_DEF_CertificateSerialNumber = {
	"CertificateSerialNumber",
	"CertificateSerialNumber",
	CertificateSerialNumber_free,
	CertificateSerialNumber_print,
	CertificateSerialNumber_constraint,
	CertificateSerialNumber_decode_ber,
	CertificateSerialNumber_encode_der,
	CertificateSerialNumber_decode_xer,
	CertificateSerialNumber_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_CertificateSerialNumber_tags_1,
	sizeof(asn_DEF_CertificateSerialNumber_tags_1)
		/sizeof(asn_DEF_CertificateSerialNumber_tags_1[0]), /* 1 */
	asn_DEF_CertificateSerialNumber_tags_1,	/* Same as above */
	sizeof(asn_DEF_CertificateSerialNumber_tags_1)
		/sizeof(asn_DEF_CertificateSerialNumber_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "Version.h"

int
Version_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_INTEGER.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using INTEGER,
 * so here we adjust the DEF accordingly.
 */
static void
Version_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_INTEGER.free_struct;
	td->print_struct   = asn_DEF_INTEGER.print_struct;
	td->ber_decoder    = asn_DEF_INTEGER.ber_decoder;
	td->der_encoder    = asn_DEF_INTEGER.der_encoder;
	td->xer_decoder    = asn_DEF_INTEGER.xer_decoder;
	td->xer_encoder    = asn_DEF_INTEGER.xer_encoder;
	td->uper_decoder   = asn_DEF_INTEGER.uper_decoder;
	td->uper_encoder   = asn_DEF_INTEGER.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_INTEGER.per_constraints;
	td->elements       = asn_DEF_INTEGER.elements;
	td->elements_count = asn_DEF_INTEGER.elements_count;
	td->specifics      = asn_DEF_INTEGER.specifics;
}

void
Version_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	Version_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
Version_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	Version_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
Version_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	Version_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
Version_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	Version_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
Version_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	Version_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
Version_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	Version_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_Version_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
asn_TYPE_descriptor_t asn_DEF_Version = {
	"Version",
	"Version",
	Version_free,
	Version_print,
	Version_constraint,
	Version_decode_ber,
	Version_encode_der,
	Version_decode_xer,
	Version_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Version_tags_1,
	sizeof(asn_DEF_Version_tags_1)
		/sizeof(asn_DEF_Version_tags_1[0]), /* 1 */
	asn_DEF_Version_tags_1,	/* Same as above */
	sizeof(asn_DEF_Version_tags_1)
		/sizeof(asn_DEF_Version_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* Defined elsewhere */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "UniqueIdentifier.h"

int
UniqueIdentifier_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_BIT_STRING.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using BIT_STRING,
 * so here we adjust the DEF accordingly.
 */
static void
UniqueIdentifier_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_BIT_STRING.free_struct;
	td->print_struct   = asn_DEF_BIT_STRING.print_struct;
	td->ber_decoder    = asn_DEF_BIT_STRING.ber_decoder;
	td->der_encoder    = asn_DEF_BIT_STRING.der_encoder;
	td->xer_decoder    = asn_DEF_BIT_STRING.xer_decoder;
	td->xer_encoder    = asn_DEF_BIT_STRING.xer_encoder;
	td->uper_decoder   = asn_DEF_BIT_STRING.uper_decoder;
	td->uper_encoder   = asn_DEF_BIT_STRING.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_BIT_STRING.per_constraints;
	td->elements       = asn_DEF_BIT_STRING.elements;
	td->elements_count = asn_DEF_BIT_STRING.elements_count;
	td->specifics      = asn_DEF_BIT_STRING.specifics;
}

void
UniqueIdentifier_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	UniqueIdentifier_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
UniqueIdentifier_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	UniqueIdentifier_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
UniqueIdentifier_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	UniqueIdentifier_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
UniqueIdentifier_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	UniqueIdentifier_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
UniqueIdentifier_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	UniqueIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
UniqueIdentifier_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	UniqueIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_UniqueIdentifier_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (3 << 2))
};
asn_TYPE_descriptor_t asn_DEF_UniqueIdentifier = {
	"UniqueIdentifier",
	"UniqueIdentifier",
	UniqueIdentifier_free,
	UniqueIdentifier_print,
	UniqueIdentifier_constraint,
	UniqueIdentifier_decode_ber,
	UniqueIdentifier_encode_der,
	UniqueIdentifier_decode_xer,
	UniqueIdentifier_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_UniqueIdentifier_tags_1,
	sizeof(asn_DEF_UniqueIdentifier_tags_1)
		/sizeof(asn_DEF_UniqueIdentifier_tags_1[0]), /* 1 */
	asn_DEF_UniqueIdentifier_tags_1,	/* Same as above */
	sizeof(asn_DEF_UniqueIdentifier_tags_1)
		/sizeof(asn_DEF_UniqueIdentifier_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "SubjectPublicKeyInfo.h"

static asn_TYPE_member_t asn_MBR_SubjectPublicKeyInfo_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SubjectPublicKeyInfo, algorithm),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_AlgorithmIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"algorithm"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SubjectPublicKeyInfo, subjectPublicKey),
		(ASN_TAG_CLASS_UNIVERSAL | (3 << 2)),
		0,
		&asn_DEF_BIT_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"subjectPublicKey"
		},
};
static ber_tlv_tag_t asn_DEF_SubjectPublicKeyInfo_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_SubjectPublicKeyInfo_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (3 << 2)), 1, 0, 0 }, /* subjectPublicKey at 314 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 0 } /* algorithm at 313 */
};
static asn_SEQUENCE_specifics_t asn_SPC_SubjectPublicKeyInfo_specs_1 = {
	sizeof(struct SubjectPublicKeyInfo),
	offsetof(struct SubjectPublicKeyInfo, _asn_ctx),
	asn_MAP_SubjectPublicKeyInfo_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_SubjectPublicKeyInfo = {
	"SubjectPublicKeyInfo",
	"SubjectPublicKeyInfo",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SubjectPublicKeyInfo_tags_1,
	sizeof(asn_DEF_SubjectPublicKeyInfo_tags_1)
		/sizeof(asn_DEF_SubjectPublicKeyInfo_tags_1[0]), /* 1 */
	asn_DEF_SubjectPublicKeyInfo_tags_1,	/* Same as above */
	sizeof(asn_DEF_SubjectPublicKeyInfo_tags_1)
		/sizeof(asn_DEF_SubjectPublicKeyInfo_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_SubjectPublicKeyInfo_1,
	2,	/* Elements count */
	&asn_SPC_SubjectPublicKeyInfo_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "Time.h"

static asn_TYPE_member_t asn_MBR_Time_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Time, choice.utcTime),
		(ASN_TAG_CLASS_UNIVERSAL | (23 << 2)),
		0,
		&asn_DEF_UTCTime,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"utcTime"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Time, choice.generalTime),
		(ASN_TAG_CLASS_UNIVERSAL | (24 << 2)),
		0,
		&asn_DEF_GeneralizedTime,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"generalTime"
		},
};
static asn_TYPE_tag2member_t asn_MAP_Time_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (23 << 2)), 0, 0, 0 }, /* utcTime at 302 */
    { (ASN_TAG_CLASS_UNIVERSAL | (24 << 2)), 1, 0, 0 } /* generalTime at 303 */
};
static asn_CHOICE_specifics_t asn_SPC_Time_specs_1 = {
	sizeof(struct Time),
	offsetof(struct Time, _asn_ctx),
	offsetof(struct Time, present),
	sizeof(((struct Time *)0)->present),
	asn_MAP_Time_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_Time = {
	"Time",
	"Time",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_Time_1,
	2,	/* Elements count */
	&asn_SPC_Time_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "Validity.h"

static asn_TYPE_member_t asn_MBR_Validity_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Validity, notBefore),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_Time,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"notBefore"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Validity, notAfter),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_Time,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"notAfter"
		},
};
static ber_tlv_tag_t asn_DEF_Validity_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Validity_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (23 << 2)), 0, 0, 1 }, /* utcTime at 302 */
    { (ASN_TAG_CLASS_UNIVERSAL | (23 << 2)), 1, -1, 0 }, /* utcTime at 302 */
    { (ASN_TAG_CLASS_UNIVERSAL | (24 << 2)), 0, 0, 1 }, /* generalTime at 303 */
    { (ASN_TAG_CLASS_UNIVERSAL | (24 << 2)), 1, -1, 0 } /* generalTime at 303 */
};
static asn_SEQUENCE_specifics_t asn_SPC_Validity_specs_1 = {
	sizeof(struct Validity),
	offsetof(struct Validity, _asn_ctx),
	asn_MAP_Validity_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Validity = {
	"Validity",
	"Validity",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Validity_tags_1,
	sizeof(asn_DEF_Validity_tags_1)
		/sizeof(asn_DEF_Validity_tags_1[0]), /* 1 */
	asn_DEF_Validity_tags_1,	/* Same as above */
	sizeof(asn_DEF_Validity_tags_1)
		/sizeof(asn_DEF_Validity_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Validity_1,
	2,	/* Elements count */
	&asn_SPC_Validity_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "AttributeType.h"

int
AttributeType_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_OBJECT_IDENTIFIER.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using OBJECT_IDENTIFIER,
 * so here we adjust the DEF accordingly.
 */
static void
AttributeType_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_OBJECT_IDENTIFIER.free_struct;
	td->print_struct   = asn_DEF_OBJECT_IDENTIFIER.print_struct;
	td->ber_decoder    = asn_DEF_OBJECT_IDENTIFIER.ber_decoder;
	td->der_encoder    = asn_DEF_OBJECT_IDENTIFIER.der_encoder;
	td->xer_decoder    = asn_DEF_OBJECT_IDENTIFIER.xer_decoder;
	td->xer_encoder    = asn_DEF_OBJECT_IDENTIFIER.xer_encoder;
	td->uper_decoder   = asn_DEF_OBJECT_IDENTIFIER.uper_decoder;
	td->uper_encoder   = asn_DEF_OBJECT_IDENTIFIER.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_OBJECT_IDENTIFIER.per_constraints;
	td->elements       = asn_DEF_OBJECT_IDENTIFIER.elements;
	td->elements_count = asn_DEF_OBJECT_IDENTIFIER.elements_count;
	td->specifics      = asn_DEF_OBJECT_IDENTIFIER.specifics;
}

void
AttributeType_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	AttributeType_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
AttributeType_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeType_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
AttributeType_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	AttributeType_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
AttributeType_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeType_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
AttributeType_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	AttributeType_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
AttributeType_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeType_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_AttributeType_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (6 << 2))
};
asn_TYPE_descriptor_t asn_DEF_AttributeType = {
	"AttributeType",
	"AttributeType",
	AttributeType_free,
	AttributeType_print,
	AttributeType_constraint,
	AttributeType_decode_ber,
	AttributeType_encode_der,
	AttributeType_decode_xer,
	AttributeType_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_AttributeType_tags_1,
	sizeof(asn_DEF_AttributeType_tags_1)
		/sizeof(asn_DEF_AttributeType_tags_1[0]), /* 1 */
	asn_DEF_AttributeType_tags_1,	/* Same as above */
	sizeof(asn_DEF_AttributeType_tags_1)
		/sizeof(asn_DEF_AttributeType_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "AttributeValue.h"

int
AttributeValue_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_ANY.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using ANY,
 * so here we adjust the DEF accordingly.
 */
static void
AttributeValue_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_ANY.free_struct;
	td->print_struct   = asn_DEF_ANY.print_struct;
	td->ber_decoder    = asn_DEF_ANY.ber_decoder;
	td->der_encoder    = asn_DEF_ANY.der_encoder;
	td->xer_decoder    = asn_DEF_ANY.xer_decoder;
	td->xer_encoder    = asn_DEF_ANY.xer_encoder;
	td->uper_decoder   = asn_DEF_ANY.uper_decoder;
	td->uper_encoder   = asn_DEF_ANY.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_ANY.per_constraints;
	td->elements       = asn_DEF_ANY.elements;
	td->elements_count = asn_DEF_ANY.elements_count;
	td->specifics      = asn_DEF_ANY.specifics;
}

void
AttributeValue_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
AttributeValue_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
AttributeValue_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
AttributeValue_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
AttributeValue_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
AttributeValue_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	AttributeValue_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

asn_TYPE_descriptor_t asn_DEF_AttributeValue = {
	"AttributeValue",
	"AttributeValue",
	AttributeValue_free,
	AttributeValue_print,
	AttributeValue_constraint,
	AttributeValue_decode_ber,
	AttributeValue_encode_der,
	AttributeValue_decode_xer,
	AttributeValue_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "AttributeTypeAndValue.h"

static asn_TYPE_member_t asn_MBR_AttributeTypeAndValue_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct AttributeTypeAndValue, type),
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_AttributeType,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"type"
		},
	{ ATF_OPEN_TYPE | ATF_NOFLAGS, 0, offsetof(struct AttributeTypeAndValue, value),
		-1 /* Ambiguous tag (ANY?) */,
		0,
		&asn_DEF_AttributeValue,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"value"
		},
};
static ber_tlv_tag_t asn_DEF_AttributeTypeAndValue_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_AttributeTypeAndValue_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (6 << 2)), 0, 0, 0 } /* type at 84 */
};
static asn_SEQUENCE_specifics_t asn_SPC_AttributeTypeAndValue_specs_1 = {
	sizeof(struct AttributeTypeAndValue),
	offsetof(struct AttributeTypeAndValue, _asn_ctx),
	asn_MAP_AttributeTypeAndValue_tag2el_1,
	1,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_AttributeTypeAndValue = {
	"AttributeTypeAndValue",
	"AttributeTypeAndValue",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_AttributeTypeAndValue_tags_1,
	sizeof(asn_DEF_AttributeTypeAndValue_tags_1)
		/sizeof(asn_DEF_AttributeTypeAndValue_tags_1[0]), /* 1 */
	asn_DEF_AttributeTypeAndValue_tags_1,	/* Same as above */
	sizeof(asn_DEF_AttributeTypeAndValue_tags_1)
		/sizeof(asn_DEF_AttributeTypeAndValue_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_AttributeTypeAndValue_1,
	2,	/* Elements count */
	&asn_SPC_AttributeTypeAndValue_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "RelativeDistinguishedName.h"

static asn_TYPE_member_t asn_MBR_RelativeDistinguishedName_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_AttributeTypeAndValue,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_RelativeDistinguishedName_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (17 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_RelativeDistinguishedName_specs_1 = {
	sizeof(struct RelativeDistinguishedName),
	offsetof(struct RelativeDistinguishedName, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_RelativeDistinguishedName = {
	"RelativeDistinguishedName",
	"RelativeDistinguishedName",
	SET_OF_free,
	SET_OF_print,
	SET_OF_constraint,
	SET_OF_decode_ber,
	SET_OF_encode_der,
	SET_OF_decode_xer,
	SET_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_RelativeDistinguishedName_tags_1,
	sizeof(asn_DEF_RelativeDistinguishedName_tags_1)
		/sizeof(asn_DEF_RelativeDistinguishedName_tags_1[0]), /* 1 */
	asn_DEF_RelativeDistinguishedName_tags_1,	/* Same as above */
	sizeof(asn_DEF_RelativeDistinguishedName_tags_1)
		/sizeof(asn_DEF_RelativeDistinguishedName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_RelativeDistinguishedName_1,
	1,	/* Single element */
	&asn_SPC_RelativeDistinguishedName_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "RDNSequence.h"

static asn_TYPE_member_t asn_MBR_RDNSequence_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (17 << 2)),
		0,
		&asn_DEF_RelativeDistinguishedName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_RDNSequence_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_RDNSequence_specs_1 = {
	sizeof(struct RDNSequence),
	offsetof(struct RDNSequence, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_RDNSequence = {
	"RDNSequence",
	"RDNSequence",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_RDNSequence_tags_1,
	sizeof(asn_DEF_RDNSequence_tags_1)
		/sizeof(asn_DEF_RDNSequence_tags_1[0]), /* 1 */
	asn_DEF_RDNSequence_tags_1,	/* Same as above */
	sizeof(asn_DEF_RDNSequence_tags_1)
		/sizeof(asn_DEF_RDNSequence_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_RDNSequence_1,
	1,	/* Single element */
	&asn_SPC_RDNSequence_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "Name.h"

static asn_TYPE_member_t asn_MBR_Name_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Name, choice.rdnSequence),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_RDNSequence,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"rdnSequence"
		},
};
static asn_TYPE_tag2member_t asn_MAP_Name_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 0 } /* rdnSequence at 248 */
};
static asn_CHOICE_specifics_t asn_SPC_Name_specs_1 = {
	sizeof(struct Name),
	offsetof(struct Name, _asn_ctx),
	offsetof(struct Name, present),
	sizeof(((struct Name *)0)->present),
	asn_MAP_Name_tag2el_1,
	1,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_Name = {
	"Name",
	"Name",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_Name_1,
	1,	/* Elements count */
	&asn_SPC_Name_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "Extension.h"

static asn_TYPE_member_t asn_MBR_Extension_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Extension, extnID),
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_OBJECT_IDENTIFIER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extnID"
		},
	{ ATF_POINTER, 1, offsetof(struct Extension, critical),
		(ASN_TAG_CLASS_UNIVERSAL | (1 << 2)),
		0,
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"critical"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Extension, extnValue),
		(ASN_TAG_CLASS_UNIVERSAL | (4 << 2)),
		0,
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extnValue"
		},
};
static ber_tlv_tag_t asn_DEF_Extension_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Extension_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (1 << 2)), 1, 0, 0 }, /* critical at 320 */
    { (ASN_TAG_CLASS_UNIVERSAL | (4 << 2)), 2, 0, 0 }, /* extnValue at 321 */
    { (ASN_TAG_CLASS_UNIVERSAL | (6 << 2)), 0, 0, 0 } /* extnID at 319 */
};
static asn_SEQUENCE_specifics_t asn_SPC_Extension_specs_1 = {
	sizeof(struct Extension),
	offsetof(struct Extension, _asn_ctx),
	asn_MAP_Extension_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Extension = {
	"Extension",
	"Extension",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Extension_tags_1,
	sizeof(asn_DEF_Extension_tags_1)
		/sizeof(asn_DEF_Extension_tags_1[0]), /* 1 */
	asn_DEF_Extension_tags_1,	/* Same as above */
	sizeof(asn_DEF_Extension_tags_1)
		/sizeof(asn_DEF_Extension_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Extension_1,
	3,	/* Elements count */
	&asn_SPC_Extension_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "Extensions.h"

static asn_TYPE_member_t asn_MBR_Extensions_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_Extension,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_Extensions_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_Extensions_specs_1 = {
	sizeof(struct Extensions),
	offsetof(struct Extensions, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_Extensions = {
	"Extensions",
	"Extensions",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Extensions_tags_1,
	sizeof(asn_DEF_Extensions_tags_1)
		/sizeof(asn_DEF_Extensions_tags_1[0]), /* 1 */
	asn_DEF_Extensions_tags_1,	/* Same as above */
	sizeof(asn_DEF_Extensions_tags_1)
		/sizeof(asn_DEF_Extensions_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Extensions_1,
	1,	/* Single element */
	&asn_SPC_Extensions_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "TBSCertificate.h"

static int asn_DFL_2_set_0(int set_value, void **sptr) {
	Version_t *st = *sptr;

	if(!st) {
		if(!set_value) return -1;	/* Not a default value */
		st = (*sptr = CALLOC(1, sizeof(*st)));
		if(!st) return -1;
	}

	if(set_value) {
		/* Install default value 0 */
		return asn_long2INTEGER(st, 0);
	} else {
		/* Test default value 0 */
		long value;
		if(asn_INTEGER2long(st, &value))
			return -1;
		return (value == 0);
	}
}
static asn_TYPE_member_t asn_MBR_TBSCertificate_1[] = {
	{ ATF_POINTER, 1, offsetof(struct TBSCertificate, version),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_Version,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		asn_DFL_2_set_0,	/* DEFAULT 0 */
		"version"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TBSCertificate, serialNumber),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_CertificateSerialNumber,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"serialNumber"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TBSCertificate, signature),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_AlgorithmIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signature"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TBSCertificate, issuer),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_Name,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"issuer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TBSCertificate, validity),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_Validity,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"validity"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TBSCertificate, subject),
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_Name,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"subject"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct TBSCertificate, subjectPublicKeyInfo),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_SubjectPublicKeyInfo,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"subjectPublicKeyInfo"
		},
	{ ATF_POINTER, 3, offsetof(struct TBSCertificate, issuerUniqueID),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_UniqueIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"issuerUniqueID"
		},
	{ ATF_POINTER, 2, offsetof(struct TBSCertificate, subjectUniqueID),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_UniqueIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"subjectUniqueID"
		},
	{ ATF_POINTER, 1, offsetof(struct TBSCertificate, extensions),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_Extensions,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extensions"
		},
};
static ber_tlv_tag_t asn_DEF_TBSCertificate_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_TBSCertificate_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, 0, 0 }, /* serialNumber at 280 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 2, 0, 4 }, /* signature at 281 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 3, -1, 3 }, /* rdnSequence at 248 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 4, -2, 2 }, /* validity at 283 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 5, -3, 1 }, /* rdnSequence at 248 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 6, -4, 0 }, /* subjectPublicKeyInfo at 285 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* version at 279 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 7, 0, 0 }, /* issuerUniqueID at 286 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 8, 0, 0 }, /* subjectUniqueID at 288 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 9, 0, 0 } /* extensions at 290 */
};
static asn_SEQUENCE_specifics_t asn_SPC_TBSCertificate_specs_1 = {
	sizeof(struct TBSCertificate),
	offsetof(struct TBSCertificate, _asn_ctx),
	asn_MAP_TBSCertificate_tag2el_1,
	10,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_TBSCertificate = {
	"TBSCertificate",
	"TBSCertificate",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_TBSCertificate_tags_1,
	sizeof(asn_DEF_TBSCertificate_tags_1)
		/sizeof(asn_DEF_TBSCertificate_tags_1[0]), /* 1 */
	asn_DEF_TBSCertificate_tags_1,	/* Same as above */
	sizeof(asn_DEF_TBSCertificate_tags_1)
		/sizeof(asn_DEF_TBSCertificate_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_TBSCertificate_1,
	10,	/* Elements count */
	&asn_SPC_TBSCertificate_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "Certificate.h"

static asn_TYPE_member_t asn_MBR_Certificate_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Certificate, tbsCertificate),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_TBSCertificate,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"tbsCertificate"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Certificate, signatureAlgorithm),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_AlgorithmIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signatureAlgorithm"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Certificate, signature),
		(ASN_TAG_CLASS_UNIVERSAL | (3 << 2)),
		0,
		&asn_DEF_BIT_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"signature"
		},
};
static ber_tlv_tag_t asn_DEF_Certificate_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Certificate_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (3 << 2)), 2, 0, 0 }, /* signature at 276 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 1 }, /* tbsCertificate at 274 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, -1, 0 } /* signatureAlgorithm at 275 */
};
static asn_SEQUENCE_specifics_t asn_SPC_Certificate_specs_1 = {
	sizeof(struct Certificate),
	offsetof(struct Certificate, _asn_ctx),
	asn_MAP_Certificate_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Certificate = {
	"Certificate",
	"Certificate",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_Certificate_tags_1,
	sizeof(asn_DEF_Certificate_tags_1)
		/sizeof(asn_DEF_Certificate_tags_1[0]), /* 1 */
	asn_DEF_Certificate_tags_1,	/* Same as above */
	sizeof(asn_DEF_Certificate_tags_1)
		/sizeof(asn_DEF_Certificate_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Certificate_1,
	3,	/* Elements count */
	&asn_SPC_Certificate_specs_1	/* Additional specs */
};

// start CI+ extensions

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "KeyIdentifier.h"

int
KeyIdentifier_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_OCTET_STRING.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using OCTET_STRING,
 * so here we adjust the DEF accordingly.
 */
static void
KeyIdentifier_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_OCTET_STRING.free_struct;
	td->print_struct   = asn_DEF_OCTET_STRING.print_struct;
	td->ber_decoder    = asn_DEF_OCTET_STRING.ber_decoder;
	td->der_encoder    = asn_DEF_OCTET_STRING.der_encoder;
	td->xer_decoder    = asn_DEF_OCTET_STRING.xer_decoder;
	td->xer_encoder    = asn_DEF_OCTET_STRING.xer_encoder;
	td->uper_decoder   = asn_DEF_OCTET_STRING.uper_decoder;
	td->uper_encoder   = asn_DEF_OCTET_STRING.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_OCTET_STRING.per_constraints;
	td->elements       = asn_DEF_OCTET_STRING.elements;
	td->elements_count = asn_DEF_OCTET_STRING.elements_count;
	td->specifics      = asn_DEF_OCTET_STRING.specifics;
}

void
KeyIdentifier_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	KeyIdentifier_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
KeyIdentifier_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	KeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
KeyIdentifier_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	KeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
KeyIdentifier_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	KeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
KeyIdentifier_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	KeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
KeyIdentifier_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	KeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_KeyIdentifier_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))
};
asn_TYPE_descriptor_t asn_DEF_KeyIdentifier = {
	"KeyIdentifier",
	"KeyIdentifier",
	KeyIdentifier_free,
	KeyIdentifier_print,
	KeyIdentifier_constraint,
	KeyIdentifier_decode_ber,
	KeyIdentifier_encode_der,
	KeyIdentifier_decode_xer,
	KeyIdentifier_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_KeyIdentifier_tags_1,
	sizeof(asn_DEF_KeyIdentifier_tags_1)
		/sizeof(asn_DEF_KeyIdentifier_tags_1[0]), /* 1 */
	asn_DEF_KeyIdentifier_tags_1,	/* Same as above */
	sizeof(asn_DEF_KeyIdentifier_tags_1)
		/sizeof(asn_DEF_KeyIdentifier_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "SubjectKeyIdentifier.h"

int
SubjectKeyIdentifier_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_KeyIdentifier.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using KeyIdentifier,
 * so here we adjust the DEF accordingly.
 */
static void
SubjectKeyIdentifier_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_KeyIdentifier.free_struct;
	td->print_struct   = asn_DEF_KeyIdentifier.print_struct;
	td->ber_decoder    = asn_DEF_KeyIdentifier.ber_decoder;
	td->der_encoder    = asn_DEF_KeyIdentifier.der_encoder;
	td->xer_decoder    = asn_DEF_KeyIdentifier.xer_decoder;
	td->xer_encoder    = asn_DEF_KeyIdentifier.xer_encoder;
	td->uper_decoder   = asn_DEF_KeyIdentifier.uper_decoder;
	td->uper_encoder   = asn_DEF_KeyIdentifier.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_KeyIdentifier.per_constraints;
	td->elements       = asn_DEF_KeyIdentifier.elements;
	td->elements_count = asn_DEF_KeyIdentifier.elements_count;
	td->specifics      = asn_DEF_KeyIdentifier.specifics;
}

void
SubjectKeyIdentifier_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	SubjectKeyIdentifier_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
SubjectKeyIdentifier_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	SubjectKeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
SubjectKeyIdentifier_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	SubjectKeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
SubjectKeyIdentifier_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	SubjectKeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
SubjectKeyIdentifier_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	SubjectKeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
SubjectKeyIdentifier_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	SubjectKeyIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_SubjectKeyIdentifier_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (4 << 2))
};
asn_TYPE_descriptor_t asn_DEF_SubjectKeyIdentifier = {
	"SubjectKeyIdentifier",
	"SubjectKeyIdentifier",
	SubjectKeyIdentifier_free,
	SubjectKeyIdentifier_print,
	SubjectKeyIdentifier_constraint,
	SubjectKeyIdentifier_decode_ber,
	SubjectKeyIdentifier_encode_der,
	SubjectKeyIdentifier_decode_xer,
	SubjectKeyIdentifier_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_SubjectKeyIdentifier_tags_1,
	sizeof(asn_DEF_SubjectKeyIdentifier_tags_1)
		/sizeof(asn_DEF_SubjectKeyIdentifier_tags_1[0]), /* 1 */
	asn_DEF_SubjectKeyIdentifier_tags_1,	/* Same as above */
	sizeof(asn_DEF_SubjectKeyIdentifier_tags_1)
		/sizeof(asn_DEF_SubjectKeyIdentifier_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "ExtensionAttribute.h"

static int
memb_extension_attribute_type_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	value = *(const long *)sptr;

	if((value >= 0 && value <= 256)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_ExtensionAttribute_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ExtensionAttribute, extension_attribute_type),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_extension_attribute_type_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extension-attribute-type"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ExtensionAttribute, extension_attribute_value),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ANY,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extension-attribute-value"
		},
};
static ber_tlv_tag_t asn_DEF_ExtensionAttribute_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ExtensionAttribute_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* extension-attribute-type at 523 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* extension-attribute-value at 525 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ExtensionAttribute_specs_1 = {
	sizeof(struct ExtensionAttribute),
	offsetof(struct ExtensionAttribute, _asn_ctx),
	asn_MAP_ExtensionAttribute_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ExtensionAttribute = {
	"ExtensionAttribute",
	"ExtensionAttribute",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ExtensionAttribute_tags_1,
	sizeof(asn_DEF_ExtensionAttribute_tags_1)
		/sizeof(asn_DEF_ExtensionAttribute_tags_1[0]), /* 1 */
	asn_DEF_ExtensionAttribute_tags_1,	/* Same as above */
	sizeof(asn_DEF_ExtensionAttribute_tags_1)
		/sizeof(asn_DEF_ExtensionAttribute_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ExtensionAttribute_1,
	2,	/* Elements count */
	&asn_SPC_ExtensionAttribute_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "ExtensionAttributes.h"

static asn_TYPE_member_t asn_MBR_ExtensionAttributes_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_ExtensionAttribute,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_ExtensionAttributes_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (17 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_ExtensionAttributes_specs_1 = {
	sizeof(struct ExtensionAttributes),
	offsetof(struct ExtensionAttributes, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_ExtensionAttributes = {
	"ExtensionAttributes",
	"ExtensionAttributes",
	SET_OF_free,
	SET_OF_print,
	SET_OF_constraint,
	SET_OF_decode_ber,
	SET_OF_encode_der,
	SET_OF_decode_xer,
	SET_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ExtensionAttributes_tags_1,
	sizeof(asn_DEF_ExtensionAttributes_tags_1)
		/sizeof(asn_DEF_ExtensionAttributes_tags_1[0]), /* 1 */
	asn_DEF_ExtensionAttributes_tags_1,	/* Same as above */
	sizeof(asn_DEF_ExtensionAttributes_tags_1)
		/sizeof(asn_DEF_ExtensionAttributes_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ExtensionAttributes_1,
	1,	/* Single element */
	&asn_SPC_ExtensionAttributes_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "BuiltInDomainDefinedAttribute.h"

static int permitted_alphabet_table_2[256] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
};

static int check_permitted_alphabet_2(const void *sptr) {
	int *table = permitted_alphabet_table_2;
	/* The underlying type is PrintableString */
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	const uint8_t *ch = st->buf;
	const uint8_t *end = ch + st->size;

	for(; ch < end; ch++) {
		uint8_t cv = *ch;
		if(!table[cv]) return -1;
	}
	return 0;
}

static int permitted_alphabet_table_3[256] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
};

static int check_permitted_alphabet_3(const void *sptr) {
	int *table = permitted_alphabet_table_3;
	/* The underlying type is PrintableString */
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	const uint8_t *ch = st->buf;
	const uint8_t *end = ch + st->size;

	for(; ch < end; ch++) {
		uint8_t cv = *ch;
		if(!table[cv]) return -1;
	}
	return 0;
}

static int
memb_type_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 8)
		 && !check_permitted_alphabet_2(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_value_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 128)
		 && !check_permitted_alphabet_3(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_BuiltInDomainDefinedAttribute_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct BuiltInDomainDefinedAttribute, type),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		memb_type_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"type"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BuiltInDomainDefinedAttribute, value),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		memb_value_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"value"
		},
};
static ber_tlv_tag_t asn_DEF_BuiltInDomainDefinedAttribute_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_BuiltInDomainDefinedAttribute_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 0, 0, 1 }, /* type at 511 */
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 1, -1, 0 } /* value at 513 */
};
static asn_SEQUENCE_specifics_t asn_SPC_BuiltInDomainDefinedAttribute_specs_1 = {
	sizeof(struct BuiltInDomainDefinedAttribute),
	offsetof(struct BuiltInDomainDefinedAttribute, _asn_ctx),
	asn_MAP_BuiltInDomainDefinedAttribute_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_BuiltInDomainDefinedAttribute = {
	"BuiltInDomainDefinedAttribute",
	"BuiltInDomainDefinedAttribute",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_BuiltInDomainDefinedAttribute_tags_1,
	sizeof(asn_DEF_BuiltInDomainDefinedAttribute_tags_1)
		/sizeof(asn_DEF_BuiltInDomainDefinedAttribute_tags_1[0]), /* 1 */
	asn_DEF_BuiltInDomainDefinedAttribute_tags_1,	/* Same as above */
	sizeof(asn_DEF_BuiltInDomainDefinedAttribute_tags_1)
		/sizeof(asn_DEF_BuiltInDomainDefinedAttribute_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BuiltInDomainDefinedAttribute_1,
	2,	/* Elements count */
	&asn_SPC_BuiltInDomainDefinedAttribute_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "BuiltInDomainDefinedAttributes.h"

static asn_TYPE_member_t asn_MBR_BuiltInDomainDefinedAttributes_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_BuiltInDomainDefinedAttribute,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_BuiltInDomainDefinedAttributes_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_BuiltInDomainDefinedAttributes_specs_1 = {
	sizeof(struct BuiltInDomainDefinedAttributes),
	offsetof(struct BuiltInDomainDefinedAttributes, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_BuiltInDomainDefinedAttributes = {
	"BuiltInDomainDefinedAttributes",
	"BuiltInDomainDefinedAttributes",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_BuiltInDomainDefinedAttributes_tags_1,
	sizeof(asn_DEF_BuiltInDomainDefinedAttributes_tags_1)
		/sizeof(asn_DEF_BuiltInDomainDefinedAttributes_tags_1[0]), /* 1 */
	asn_DEF_BuiltInDomainDefinedAttributes_tags_1,	/* Same as above */
	sizeof(asn_DEF_BuiltInDomainDefinedAttributes_tags_1)
		/sizeof(asn_DEF_BuiltInDomainDefinedAttributes_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BuiltInDomainDefinedAttributes_1,
	1,	/* Single element */
	&asn_SPC_BuiltInDomainDefinedAttributes_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "OrganizationalUnitName.h"

static int permitted_alphabet_table_1[256] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
};

static int check_permitted_alphabet_1(const void *sptr) {
	int *table = permitted_alphabet_table_1;
	/* The underlying type is PrintableString */
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	const uint8_t *ch = st->buf;
	const uint8_t *end = ch + st->size;

	for(; ch < end; ch++) {
		uint8_t cv = *ch;
		if(!table[cv]) return -1;
	}
	return 0;
}

int
OrganizationalUnitName_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 32)
		 && !check_permitted_alphabet_1(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using PrintableString,
 * so here we adjust the DEF accordingly.
 */
static void
OrganizationalUnitName_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_PrintableString.free_struct;
	td->print_struct   = asn_DEF_PrintableString.print_struct;
	td->ber_decoder    = asn_DEF_PrintableString.ber_decoder;
	td->der_encoder    = asn_DEF_PrintableString.der_encoder;
	td->xer_decoder    = asn_DEF_PrintableString.xer_decoder;
	td->xer_encoder    = asn_DEF_PrintableString.xer_encoder;
	td->uper_decoder   = asn_DEF_PrintableString.uper_decoder;
	td->uper_encoder   = asn_DEF_PrintableString.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_PrintableString.per_constraints;
	td->elements       = asn_DEF_PrintableString.elements;
	td->elements_count = asn_DEF_PrintableString.elements_count;
	td->specifics      = asn_DEF_PrintableString.specifics;
}

void
OrganizationalUnitName_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	OrganizationalUnitName_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
OrganizationalUnitName_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	OrganizationalUnitName_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
OrganizationalUnitName_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	OrganizationalUnitName_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
OrganizationalUnitName_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	OrganizationalUnitName_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
OrganizationalUnitName_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	OrganizationalUnitName_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
OrganizationalUnitName_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	OrganizationalUnitName_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_OrganizationalUnitName_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (19 << 2))
};
asn_TYPE_descriptor_t asn_DEF_OrganizationalUnitName = {
	"OrganizationalUnitName",
	"OrganizationalUnitName",
	OrganizationalUnitName_free,
	OrganizationalUnitName_print,
	OrganizationalUnitName_constraint,
	OrganizationalUnitName_decode_ber,
	OrganizationalUnitName_encode_der,
	OrganizationalUnitName_decode_xer,
	OrganizationalUnitName_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_OrganizationalUnitName_tags_1,
	sizeof(asn_DEF_OrganizationalUnitName_tags_1)
		/sizeof(asn_DEF_OrganizationalUnitName_tags_1[0]), /* 1 */
	asn_DEF_OrganizationalUnitName_tags_1,	/* Same as above */
	sizeof(asn_DEF_OrganizationalUnitName_tags_1)
		/sizeof(asn_DEF_OrganizationalUnitName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "OrganizationalUnitNames.h"

static asn_TYPE_member_t asn_MBR_OrganizationalUnitNames_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_OrganizationalUnitName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_OrganizationalUnitNames_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_OrganizationalUnitNames_specs_1 = {
	sizeof(struct OrganizationalUnitNames),
	offsetof(struct OrganizationalUnitNames, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_OrganizationalUnitNames = {
	"OrganizationalUnitNames",
	"OrganizationalUnitNames",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_OrganizationalUnitNames_tags_1,
	sizeof(asn_DEF_OrganizationalUnitNames_tags_1)
		/sizeof(asn_DEF_OrganizationalUnitNames_tags_1[0]), /* 1 */
	asn_DEF_OrganizationalUnitNames_tags_1,	/* Same as above */
	sizeof(asn_DEF_OrganizationalUnitNames_tags_1)
		/sizeof(asn_DEF_OrganizationalUnitNames_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_OrganizationalUnitNames_1,
	1,	/* Single element */
	&asn_SPC_OrganizationalUnitNames_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "PersonalName.h"

//static int permitted_alphabet_table_2[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
//10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
// 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
//38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
// 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
//64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
//};

//static int check_permitted_alphabet_2(const void *sptr) {
//	int *table = permitted_alphabet_table_2;
//	/* The underlying type is PrintableString */
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

//static int permitted_alphabet_table_3[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
//10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
// 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
//38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
// 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
//64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
//};

//static int check_permitted_alphabet_3(const void *sptr) {
//	int *table = permitted_alphabet_table_3;
//	/* The underlying type is PrintableString */
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

static int permitted_alphabet_table_4[256] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
};

static int check_permitted_alphabet_4(const void *sptr) {
	int *table = permitted_alphabet_table_4;
	/* The underlying type is PrintableString */
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	const uint8_t *ch = st->buf;
	const uint8_t *end = ch + st->size;

	for(; ch < end; ch++) {
		uint8_t cv = *ch;
		if(!table[cv]) return -1;
	}
	return 0;
}

static int permitted_alphabet_table_5[256] = {
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
};

static int check_permitted_alphabet_5(const void *sptr) {
	int *table = permitted_alphabet_table_5;
	/* The underlying type is PrintableString */
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	const uint8_t *ch = st->buf;
	const uint8_t *end = ch + st->size;

	for(; ch < end; ch++) {
		uint8_t cv = *ch;
		if(!table[cv]) return -1;
	}
	return 0;
}

static int
memb_surname_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 40)
		 && !check_permitted_alphabet_2(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_given_name_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 16)
		 && !check_permitted_alphabet_3(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_initials_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 5)
		 && !check_permitted_alphabet_4(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_generation_qualifier_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 3)
		 && !check_permitted_alphabet_5(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_PersonalName_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PersonalName, surname),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		memb_surname_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"surname"
		},
	{ ATF_POINTER, 3, offsetof(struct PersonalName, given_name),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		memb_given_name_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"given-name"
		},
	{ ATF_POINTER, 2, offsetof(struct PersonalName, initials),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		memb_initials_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"initials"
		},
	{ ATF_POINTER, 1, offsetof(struct PersonalName, generation_qualifier),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		memb_generation_qualifier_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"generation-qualifier"
		},
};
static ber_tlv_tag_t asn_DEF_PersonalName_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (17 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_PersonalName_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* surname at 486 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* given-name at 488 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* initials at 490 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* generation-qualifier at 492 */
};
static uint8_t asn_MAP_PersonalName_mmap_1[(4 + (8 * sizeof(unsigned int)) - 1) / 8] = {
	(1 << 7) | (0 << 6) | (0 << 5) | (0 << 4)
};
static asn_SET_specifics_t asn_SPC_PersonalName_specs_1 = {
	sizeof(struct PersonalName),
	offsetof(struct PersonalName, _asn_ctx),
	offsetof(struct PersonalName, _presence_map),
	asn_MAP_PersonalName_tag2el_1,
	4,	/* Count of tags in the map */
	asn_MAP_PersonalName_tag2el_1,	/* Same as above */
	4,	/* Count of tags in the CXER map */
	0,	/* Whether extensible */
	(unsigned int *)asn_MAP_PersonalName_mmap_1	/* Mandatory elements map */
};
asn_TYPE_descriptor_t asn_DEF_PersonalName = {
	"PersonalName",
	"PersonalName",
	SET_free,
	SET_print,
	SET_constraint,
	SET_decode_ber,
	SET_encode_der,
	SET_decode_xer,
	SET_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_PersonalName_tags_1,
	sizeof(asn_DEF_PersonalName_tags_1)
		/sizeof(asn_DEF_PersonalName_tags_1[0]), /* 1 */
	asn_DEF_PersonalName_tags_1,	/* Same as above */
	sizeof(asn_DEF_PersonalName_tags_1)
		/sizeof(asn_DEF_PersonalName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_PersonalName_1,
	4,	/* Elements count */
	&asn_SPC_PersonalName_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "PrivateDomainName.h"

//static int permitted_alphabet_table_2[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* .                */
// 2, 3, 4, 5, 6, 7, 8, 9,10,11, 0, 0, 0, 0, 0, 0,	/* 0123456789       */
//};

//static int check_permitted_alphabet_2(const void *sptr) {
//	int *table = permitted_alphabet_table_2;
//	/* The underlying type is NumericString */
//	const NumericString_t *st = (const NumericString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

//static int permitted_alphabet_table_3[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
//10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
// 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
//38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
// 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
//64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
//};

//static int check_permitted_alphabet_3(const void *sptr) {
//	int *table = permitted_alphabet_table_3;
//	/* The underlying type is PrintableString */
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

static int
memb_numeric_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const NumericString_t *st = (const NumericString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 16)
		 && !check_permitted_alphabet_2(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_printable_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 16)
		 && !check_permitted_alphabet_3(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_PrivateDomainName_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct PrivateDomainName, choice.numeric),
		(ASN_TAG_CLASS_UNIVERSAL | (18 << 2)),
		0,
		&asn_DEF_NumericString,
		memb_numeric_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numeric"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct PrivateDomainName, choice.printable),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		memb_printable_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"printable"
		},
};
static asn_TYPE_tag2member_t asn_MAP_PrivateDomainName_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (18 << 2)), 0, 0, 0 }, /* numeric at 469 */
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 1, 0, 0 } /* printable at 470 */
};
static asn_CHOICE_specifics_t asn_SPC_PrivateDomainName_specs_1 = {
	sizeof(struct PrivateDomainName),
	offsetof(struct PrivateDomainName, _asn_ctx),
	offsetof(struct PrivateDomainName, present),
	sizeof(((struct PrivateDomainName *)0)->present),
	asn_MAP_PrivateDomainName_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_PrivateDomainName = {
	"PrivateDomainName",
	"PrivateDomainName",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_PrivateDomainName_1,
	2,	/* Elements count */
	&asn_SPC_PrivateDomainName_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "AdministrationDomainName.h"

//static int permitted_alphabet_table_2[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* .                */
// 2, 3, 4, 5, 6, 7, 8, 9,10,11, 0, 0, 0, 0, 0, 0,	/* 0123456789       */
//};

//static int check_permitted_alphabet_2(const void *sptr) {
//	int *table = permitted_alphabet_table_2;
//	/* The underlying type is NumericString */
//	const NumericString_t *st = (const NumericString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

//static int permitted_alphabet_table_3[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
//10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
// 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
//38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
// 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
//64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
//};
//
//static int check_permitted_alphabet_3(const void *sptr) {
//	int *table = permitted_alphabet_table_3;
//	/* The underlying type is PrintableString */
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

//static int
//memb_numeric_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
//			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
//	const NumericString_t *st = (const NumericString_t *)sptr;
//	size_t size;
//
//	if(!sptr) {
//		_ASN_CTFAIL(app_key, td, sptr,
//			"%s: value not given (%s:%d)",
//			td->name, __FILE__, __LINE__);
//		return -1;
//	}
//
//	size = st->size;
//
//	if((size <= 16)
//		 && !check_permitted_alphabet_2(st)) {
//		/* Constraint check succeeded */
//		return 0;
//	} else {
//		_ASN_CTFAIL(app_key, td, sptr,
//			"%s: constraint failed (%s:%d)",
//			td->name, __FILE__, __LINE__);
//		return -1;
//	}
//}

//static int
//memb_printable_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
//			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	size_t size;
//
//	if(!sptr) {
//		_ASN_CTFAIL(app_key, td, sptr,
//			"%s: value not given (%s:%d)",
//			td->name, __FILE__, __LINE__);
//		return -1;
//	}
//
//	size = st->size;
//
//	if((size <= 16)
//		 && !check_permitted_alphabet_3(st)) {
//		/* Constraint check succeeded */
//		return 0;
//	} else {
//		_ASN_CTFAIL(app_key, td, sptr,
//			"%s: constraint failed (%s:%d)",
//			td->name, __FILE__, __LINE__);
//		return -1;
//	}
//}

static asn_TYPE_member_t asn_MBR_AdministrationDomainName_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct AdministrationDomainName, choice.numeric),
		(ASN_TAG_CLASS_UNIVERSAL | (18 << 2)),
		0,
		&asn_DEF_NumericString,
		memb_numeric_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numeric"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct AdministrationDomainName, choice.printable),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		memb_printable_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"printable"
		},
};
static ber_tlv_tag_t asn_DEF_AdministrationDomainName_tags_1[] = {
	(ASN_TAG_CLASS_APPLICATION | (2 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_AdministrationDomainName_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (18 << 2)), 0, 0, 0 }, /* numeric at 458 */
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 1, 0, 0 } /* printable at 459 */
};
static asn_CHOICE_specifics_t asn_SPC_AdministrationDomainName_specs_1 = {
	sizeof(struct AdministrationDomainName),
	offsetof(struct AdministrationDomainName, _asn_ctx),
	offsetof(struct AdministrationDomainName, present),
	sizeof(((struct AdministrationDomainName *)0)->present),
	asn_MAP_AdministrationDomainName_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_AdministrationDomainName = {
	"AdministrationDomainName",
	"AdministrationDomainName",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	asn_DEF_AdministrationDomainName_tags_1,
	sizeof(asn_DEF_AdministrationDomainName_tags_1)
		/sizeof(asn_DEF_AdministrationDomainName_tags_1[0]), /* 1 */
	asn_DEF_AdministrationDomainName_tags_1,	/* Same as above */
	sizeof(asn_DEF_AdministrationDomainName_tags_1)
		/sizeof(asn_DEF_AdministrationDomainName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_AdministrationDomainName_1,
	2,	/* Elements count */
	&asn_SPC_AdministrationDomainName_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "CountryName.h"

//static int permitted_alphabet_table_2[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* .                */
// 2, 3, 4, 5, 6, 7, 8, 9,10,11, 0, 0, 0, 0, 0, 0,	/* 0123456789       */
//};

//static int check_permitted_alphabet_2(const void *sptr) {
//	int *table = permitted_alphabet_table_2;
//	/* The underlying type is NumericString */
//	const NumericString_t *st = (const NumericString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

//static int permitted_alphabet_table_3[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
//10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
// 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
//38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
// 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
//64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
//};

//static int check_permitted_alphabet_3(const void *sptr) {
//	int *table = permitted_alphabet_table_3;
//	/* The underlying type is PrintableString */
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

static int
memb_x121_dcc_code_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const NumericString_t *st = (const NumericString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size == 3)
		 && !check_permitted_alphabet_2(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_iso_3166_alpha2_code_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size == 2)
		 && !check_permitted_alphabet_3(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_CountryName_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CountryName, choice.x121_dcc_code),
		(ASN_TAG_CLASS_UNIVERSAL | (18 << 2)),
		0,
		&asn_DEF_NumericString,
		memb_x121_dcc_code_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"x121-dcc-code"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CountryName, choice.iso_3166_alpha2_code),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		memb_iso_3166_alpha2_code_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"iso-3166-alpha2-code"
		},
};
static ber_tlv_tag_t asn_DEF_CountryName_tags_1[] = {
	(ASN_TAG_CLASS_APPLICATION | (1 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_CountryName_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (18 << 2)), 0, 0, 0 }, /* x121-dcc-code at 452 */
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 1, 0, 0 } /* iso-3166-alpha2-code at 454 */
};
static asn_CHOICE_specifics_t asn_SPC_CountryName_specs_1 = {
	sizeof(struct CountryName),
	offsetof(struct CountryName, _asn_ctx),
	offsetof(struct CountryName, present),
	sizeof(((struct CountryName *)0)->present),
	asn_MAP_CountryName_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_CountryName = {
	"CountryName",
	"CountryName",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	asn_DEF_CountryName_tags_1,
	sizeof(asn_DEF_CountryName_tags_1)
		/sizeof(asn_DEF_CountryName_tags_1[0]), /* 1 */
	asn_DEF_CountryName_tags_1,	/* Same as above */
	sizeof(asn_DEF_CountryName_tags_1)
		/sizeof(asn_DEF_CountryName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_CountryName_1,
	2,	/* Elements count */
	&asn_SPC_CountryName_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "NumericUserIdentifier.h"

//static int permitted_alphabet_table_1_NUI[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* .                */
// 2, 3, 4, 5, 6, 7, 8, 9,10,11, 0, 0, 0, 0, 0, 0,	/* 0123456789       */
//};

//static int check_permitted_alphabet_1(const void *sptr) {
//	int *table = permitted_alphabet_table_1_NUI;
//	/* The underlying type is NumericString */
//	const NumericString_t *st = (const NumericString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

int
NumericUserIdentifier_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const NumericString_t *st = (const NumericString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 32)
		 && !check_permitted_alphabet_1(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using NumericString,
 * so here we adjust the DEF accordingly.
 */
static void
NumericUserIdentifier_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NumericString.free_struct;
	td->print_struct   = asn_DEF_NumericString.print_struct;
	td->ber_decoder    = asn_DEF_NumericString.ber_decoder;
	td->der_encoder    = asn_DEF_NumericString.der_encoder;
	td->xer_decoder    = asn_DEF_NumericString.xer_decoder;
	td->xer_encoder    = asn_DEF_NumericString.xer_encoder;
	td->uper_decoder   = asn_DEF_NumericString.uper_decoder;
	td->uper_encoder   = asn_DEF_NumericString.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NumericString.per_constraints;
	td->elements       = asn_DEF_NumericString.elements;
	td->elements_count = asn_DEF_NumericString.elements_count;
	td->specifics      = asn_DEF_NumericString.specifics;
}

void
NumericUserIdentifier_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	NumericUserIdentifier_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
NumericUserIdentifier_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	NumericUserIdentifier_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
NumericUserIdentifier_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	NumericUserIdentifier_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
NumericUserIdentifier_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	NumericUserIdentifier_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
NumericUserIdentifier_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	NumericUserIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
NumericUserIdentifier_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	NumericUserIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_NumericUserIdentifier_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (18 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NumericUserIdentifier = {
	"NumericUserIdentifier",
	"NumericUserIdentifier",
	NumericUserIdentifier_free,
	NumericUserIdentifier_print,
	NumericUserIdentifier_constraint,
	NumericUserIdentifier_decode_ber,
	NumericUserIdentifier_encode_der,
	NumericUserIdentifier_decode_xer,
	NumericUserIdentifier_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_NumericUserIdentifier_tags_1,
	sizeof(asn_DEF_NumericUserIdentifier_tags_1)
		/sizeof(asn_DEF_NumericUserIdentifier_tags_1[0]), /* 1 */
	asn_DEF_NumericUserIdentifier_tags_1,	/* Same as above */
	sizeof(asn_DEF_NumericUserIdentifier_tags_1)
		/sizeof(asn_DEF_NumericUserIdentifier_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "OrganizationName.h"

//static int permitted_alphabet_table_1_ON[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
//10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
// 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
//38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
// 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
//64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
//};

//static int check_permitted_alphabet_1(const void *sptr) {
//	int *table = permitted_alphabet_table_1_ON;
//	/* The underlying type is PrintableString */
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

int
OrganizationName_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 64)
		 && !check_permitted_alphabet_1(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using PrintableString,
 * so here we adjust the DEF accordingly.
 */
static void
OrganizationName_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_PrintableString.free_struct;
	td->print_struct   = asn_DEF_PrintableString.print_struct;
	td->ber_decoder    = asn_DEF_PrintableString.ber_decoder;
	td->der_encoder    = asn_DEF_PrintableString.der_encoder;
	td->xer_decoder    = asn_DEF_PrintableString.xer_decoder;
	td->xer_encoder    = asn_DEF_PrintableString.xer_encoder;
	td->uper_decoder   = asn_DEF_PrintableString.uper_decoder;
	td->uper_encoder   = asn_DEF_PrintableString.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_PrintableString.per_constraints;
	td->elements       = asn_DEF_PrintableString.elements;
	td->elements_count = asn_DEF_PrintableString.elements_count;
	td->specifics      = asn_DEF_PrintableString.specifics;
}

void
OrganizationName_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	OrganizationName_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
OrganizationName_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	OrganizationName_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
OrganizationName_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	OrganizationName_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
OrganizationName_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	OrganizationName_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
OrganizationName_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	OrganizationName_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
OrganizationName_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	OrganizationName_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_OrganizationName_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (19 << 2))
};
asn_TYPE_descriptor_t asn_DEF_OrganizationName = {
	"OrganizationName",
	"OrganizationName",
	OrganizationName_free,
	OrganizationName_print,
	OrganizationName_constraint,
	OrganizationName_decode_ber,
	OrganizationName_encode_der,
	OrganizationName_decode_xer,
	OrganizationName_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_OrganizationName_tags_1,
	sizeof(asn_DEF_OrganizationName_tags_1)
		/sizeof(asn_DEF_OrganizationName_tags_1[0]), /* 1 */
	asn_DEF_OrganizationName_tags_1,	/* Same as above */
	sizeof(asn_DEF_OrganizationName_tags_1)
		/sizeof(asn_DEF_OrganizationName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "TerminalIdentifier.h"

//static int permitted_alphabet_table_1_TI[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
//10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
// 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
//38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
// 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
//64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
//};

//static int check_permitted_alphabet_1(const void *sptr) {
//	int *table = permitted_alphabet_table_1_TI;
//	/* The underlying type is PrintableString */
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

int
TerminalIdentifier_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 24)
		 && !check_permitted_alphabet_1(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using PrintableString,
 * so here we adjust the DEF accordingly.
 */
static void
TerminalIdentifier_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_PrintableString.free_struct;
	td->print_struct   = asn_DEF_PrintableString.print_struct;
	td->ber_decoder    = asn_DEF_PrintableString.ber_decoder;
	td->der_encoder    = asn_DEF_PrintableString.der_encoder;
	td->xer_decoder    = asn_DEF_PrintableString.xer_decoder;
	td->xer_encoder    = asn_DEF_PrintableString.xer_encoder;
	td->uper_decoder   = asn_DEF_PrintableString.uper_decoder;
	td->uper_encoder   = asn_DEF_PrintableString.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_PrintableString.per_constraints;
	td->elements       = asn_DEF_PrintableString.elements;
	td->elements_count = asn_DEF_PrintableString.elements_count;
	td->specifics      = asn_DEF_PrintableString.specifics;
}

void
TerminalIdentifier_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	TerminalIdentifier_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
TerminalIdentifier_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	TerminalIdentifier_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
TerminalIdentifier_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	TerminalIdentifier_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
TerminalIdentifier_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	TerminalIdentifier_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
TerminalIdentifier_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	TerminalIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
TerminalIdentifier_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	TerminalIdentifier_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_TerminalIdentifier_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (19 << 2))
};
asn_TYPE_descriptor_t asn_DEF_TerminalIdentifier = {
	"TerminalIdentifier",
	"TerminalIdentifier",
	TerminalIdentifier_free,
	TerminalIdentifier_print,
	TerminalIdentifier_constraint,
	TerminalIdentifier_decode_ber,
	TerminalIdentifier_encode_der,
	TerminalIdentifier_decode_xer,
	TerminalIdentifier_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_TerminalIdentifier_tags_1,
	sizeof(asn_DEF_TerminalIdentifier_tags_1)
		/sizeof(asn_DEF_TerminalIdentifier_tags_1[0]), /* 1 */
	asn_DEF_TerminalIdentifier_tags_1,	/* Same as above */
	sizeof(asn_DEF_TerminalIdentifier_tags_1)
		/sizeof(asn_DEF_TerminalIdentifier_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "X121Address.h"

//static int permitted_alphabet_table_1_XA[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* .                */
// 2, 3, 4, 5, 6, 7, 8, 9,10,11, 0, 0, 0, 0, 0, 0,	/* 0123456789       */
//};

//static int check_permitted_alphabet_1(const void *sptr) {
//	int *table = permitted_alphabet_table_1_XA;
//	/* The underlying type is NumericString */
//	const NumericString_t *st = (const NumericString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

int
X121Address_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const NumericString_t *st = (const NumericString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 16)
		 && !check_permitted_alphabet_1(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using NumericString,
 * so here we adjust the DEF accordingly.
 */
static void
X121Address_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NumericString.free_struct;
	td->print_struct   = asn_DEF_NumericString.print_struct;
	td->ber_decoder    = asn_DEF_NumericString.ber_decoder;
	td->der_encoder    = asn_DEF_NumericString.der_encoder;
	td->xer_decoder    = asn_DEF_NumericString.xer_decoder;
	td->xer_encoder    = asn_DEF_NumericString.xer_encoder;
	td->uper_decoder   = asn_DEF_NumericString.uper_decoder;
	td->uper_encoder   = asn_DEF_NumericString.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NumericString.per_constraints;
	td->elements       = asn_DEF_NumericString.elements;
	td->elements_count = asn_DEF_NumericString.elements_count;
	td->specifics      = asn_DEF_NumericString.specifics;
}

void
X121Address_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	X121Address_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
X121Address_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	X121Address_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
X121Address_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	X121Address_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
X121Address_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	X121Address_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
X121Address_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	X121Address_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
X121Address_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	X121Address_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_X121Address_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (18 << 2))
};
asn_TYPE_descriptor_t asn_DEF_X121Address = {
	"X121Address",
	"X121Address",
	X121Address_free,
	X121Address_print,
	X121Address_constraint,
	X121Address_decode_ber,
	X121Address_encode_der,
	X121Address_decode_xer,
	X121Address_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_X121Address_tags_1,
	sizeof(asn_DEF_X121Address_tags_1)
		/sizeof(asn_DEF_X121Address_tags_1[0]), /* 1 */
	asn_DEF_X121Address_tags_1,	/* Same as above */
	sizeof(asn_DEF_X121Address_tags_1)
		/sizeof(asn_DEF_X121Address_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "NetworkAddress.h"

//static int permitted_alphabet_table_1_NA[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* .                */
// 2, 3, 4, 5, 6, 7, 8, 9,10,11, 0, 0, 0, 0, 0, 0,	/* 0123456789       */
//};

//static int check_permitted_alphabet_1(const void *sptr) {
//	int *table = permitted_alphabet_table_1_NA;
//	/* The underlying type is NumericString */
//	const NumericString_t *st = (const NumericString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

int
NetworkAddress_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const X121Address_t *st = (const X121Address_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1 && size <= 16)
		 && !check_permitted_alphabet_1(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using X121Address,
 * so here we adjust the DEF accordingly.
 */
static void
NetworkAddress_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_X121Address.free_struct;
	td->print_struct   = asn_DEF_X121Address.print_struct;
	td->ber_decoder    = asn_DEF_X121Address.ber_decoder;
	td->der_encoder    = asn_DEF_X121Address.der_encoder;
	td->xer_decoder    = asn_DEF_X121Address.xer_decoder;
	td->xer_encoder    = asn_DEF_X121Address.xer_encoder;
	td->uper_decoder   = asn_DEF_X121Address.uper_decoder;
	td->uper_encoder   = asn_DEF_X121Address.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_X121Address.per_constraints;
	td->elements       = asn_DEF_X121Address.elements;
	td->elements_count = asn_DEF_X121Address.elements_count;
	td->specifics      = asn_DEF_X121Address.specifics;
}

void
NetworkAddress_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	NetworkAddress_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
NetworkAddress_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	NetworkAddress_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
NetworkAddress_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	NetworkAddress_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
NetworkAddress_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	NetworkAddress_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
NetworkAddress_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	NetworkAddress_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
NetworkAddress_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	NetworkAddress_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_NetworkAddress_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (18 << 2))
};
asn_TYPE_descriptor_t asn_DEF_NetworkAddress = {
	"NetworkAddress",
	"NetworkAddress",
	NetworkAddress_free,
	NetworkAddress_print,
	NetworkAddress_constraint,
	NetworkAddress_decode_ber,
	NetworkAddress_encode_der,
	NetworkAddress_decode_xer,
	NetworkAddress_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_NetworkAddress_tags_1,
	sizeof(asn_DEF_NetworkAddress_tags_1)
		/sizeof(asn_DEF_NetworkAddress_tags_1[0]), /* 1 */
	asn_DEF_NetworkAddress_tags_1,	/* Same as above */
	sizeof(asn_DEF_NetworkAddress_tags_1)
		/sizeof(asn_DEF_NetworkAddress_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "BuiltInStandardAttributes.h"

static asn_TYPE_member_t asn_MBR_BuiltInStandardAttributes_1[] = {
	{ ATF_POINTER, 9, offsetof(struct BuiltInStandardAttributes, country_name),
		(ASN_TAG_CLASS_APPLICATION | (1 << 2)),
		0,
		&asn_DEF_CountryName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"country-name"
		},
	{ ATF_POINTER, 8, offsetof(struct BuiltInStandardAttributes, administration_domain_name),
		(ASN_TAG_CLASS_APPLICATION | (2 << 2)),
		0,
		&asn_DEF_AdministrationDomainName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"administration-domain-name"
		},
	{ ATF_POINTER, 7, offsetof(struct BuiltInStandardAttributes, network_address),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NetworkAddress,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"network-address"
		},
	{ ATF_POINTER, 6, offsetof(struct BuiltInStandardAttributes, terminal_identifier),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_TerminalIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"terminal-identifier"
		},
	{ ATF_POINTER, 5, offsetof(struct BuiltInStandardAttributes, private_domain_name),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_PrivateDomainName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"private-domain-name"
		},
	{ ATF_POINTER, 4, offsetof(struct BuiltInStandardAttributes, organization_name),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OrganizationName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"organization-name"
		},
	{ ATF_POINTER, 3, offsetof(struct BuiltInStandardAttributes, numeric_user_identifier),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NumericUserIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"numeric-user-identifier"
		},
	{ ATF_POINTER, 2, offsetof(struct BuiltInStandardAttributes, personal_name),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PersonalName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"personal-name"
		},
	{ ATF_POINTER, 1, offsetof(struct BuiltInStandardAttributes, organizational_unit_names),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OrganizationalUnitNames,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"organizational-unit-names"
		},
};
static ber_tlv_tag_t asn_DEF_BuiltInStandardAttributes_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_BuiltInStandardAttributes_tag2el_1[] = {
    { (ASN_TAG_CLASS_APPLICATION | (1 << 2)), 0, 0, 0 }, /* country-name at 435 */
    { (ASN_TAG_CLASS_APPLICATION | (2 << 2)), 1, 0, 0 }, /* administration-domain-name at 436 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 2, 0, 0 }, /* network-address at 437 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 3, 0, 0 }, /* terminal-identifier at 439 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 4, 0, 0 }, /* private-domain-name at 440 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 5, 0, 0 }, /* organization-name at 441 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 6, 0, 0 }, /* numeric-user-identifier at 444 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 7, 0, 0 }, /* personal-name at 445 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 8, 0, 0 } /* organizational-unit-names at 448 */
};
static asn_SEQUENCE_specifics_t asn_SPC_BuiltInStandardAttributes_specs_1 = {
	sizeof(struct BuiltInStandardAttributes),
	offsetof(struct BuiltInStandardAttributes, _asn_ctx),
	asn_MAP_BuiltInStandardAttributes_tag2el_1,
	9,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_BuiltInStandardAttributes = {
	"BuiltInStandardAttributes",
	"BuiltInStandardAttributes",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_BuiltInStandardAttributes_tags_1,
	sizeof(asn_DEF_BuiltInStandardAttributes_tags_1)
		/sizeof(asn_DEF_BuiltInStandardAttributes_tags_1[0]), /* 1 */
	asn_DEF_BuiltInStandardAttributes_tags_1,	/* Same as above */
	sizeof(asn_DEF_BuiltInStandardAttributes_tags_1)
		/sizeof(asn_DEF_BuiltInStandardAttributes_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BuiltInStandardAttributes_1,
	9,	/* Elements count */
	&asn_SPC_BuiltInStandardAttributes_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "ORAddress.h"

static asn_TYPE_member_t asn_MBR_ORAddress_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ORAddress, built_in_standard_attributes),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_BuiltInStandardAttributes,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"built-in-standard-attributes"
		},
	{ ATF_POINTER, 2, offsetof(struct ORAddress, built_in_domain_defined_attributes),
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_BuiltInDomainDefinedAttributes,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"built-in-domain-defined-attributes"
		},
	{ ATF_POINTER, 1, offsetof(struct ORAddress, extension_attributes),
		(ASN_TAG_CLASS_UNIVERSAL | (17 << 2)),
		0,
		&asn_DEF_ExtensionAttributes,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"extension-attributes"
		},
};
static ber_tlv_tag_t asn_DEF_ORAddress_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ORAddress_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 0, 0, 1 }, /* built-in-standard-attributes at 426 */
    { (ASN_TAG_CLASS_UNIVERSAL | (16 << 2)), 1, -1, 0 }, /* built-in-domain-defined-attributes at 428 */
    { (ASN_TAG_CLASS_UNIVERSAL | (17 << 2)), 2, 0, 0 } /* extension-attributes at 430 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ORAddress_specs_1 = {
	sizeof(struct ORAddress),
	offsetof(struct ORAddress, _asn_ctx),
	asn_MAP_ORAddress_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ORAddress = {
	"ORAddress",
	"ORAddress",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ORAddress_tags_1,
	sizeof(asn_DEF_ORAddress_tags_1)
		/sizeof(asn_DEF_ORAddress_tags_1[0]), /* 1 */
	asn_DEF_ORAddress_tags_1,	/* Same as above */
	sizeof(asn_DEF_ORAddress_tags_1)
		/sizeof(asn_DEF_ORAddress_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ORAddress_1,
	3,	/* Elements count */
	&asn_SPC_ORAddress_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "DirectoryString.h"

//static int permitted_alphabet_table_3[256] = {
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/*                  */
// 1, 0, 0, 0, 0, 0, 0, 2, 3, 4, 0, 5, 6, 7, 8, 9,	/* .      '() +,-./ */
//10,11,12,13,14,15,16,17,18,19,20, 0, 0,21, 0,22,	/* 0123456789:  = ? */
// 0,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,	/*  ABCDEFGHIJKLMNO */
//38,39,40,41,42,43,44,45,46,47,48, 0, 0, 0, 0, 0,	/* PQRSTUVWXYZ      */
// 0,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,	/*  abcdefghijklmno */
//64,65,66,67,68,69,70,71,72,73,74, 0, 0, 0, 0, 0,	/* pqrstuvwxyz      */
//};

//static int check_permitted_alphabet_3(const void *sptr) {
//	int *table = permitted_alphabet_table_3;
//	/* The underlying type is PrintableString */
//	const PrintableString_t *st = (const PrintableString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	for(; ch < end; ch++) {
//		uint8_t cv = *ch;
//		if(!table[cv]) return -1;
//	}
//	return 0;
//}

//static int check_permitted_alphabet_4(const void *sptr) {
//	/* The underlying type is UniversalString */
//	const UniversalString_t *st = (const UniversalString_t *)sptr;
//	const uint8_t *ch = st->buf;
//	const uint8_t *end = ch + st->size;
//
//	if(st->size % 4) return -1; /* (size%4)! */
//	for(; ch < end; ch += 4) {
//		uint32_t cv = (ch[0] << 24)
//				| (ch[1] << 16)
//				| (ch[2] << 8)
//				|  ch[3];
//		if(!(cv <= 2147483647)) return -1;
//	}
//	return 0;
//}

static int check_permitted_alphabet_6(const void *sptr) {
	/* The underlying type is BMPString */
	const BMPString_t *st = (const BMPString_t *)sptr;
	const uint8_t *ch = st->buf;
	const uint8_t *end = ch + st->size;

	if(st->size % 2) return -1; /* (size%2)! */
	for(; ch < end; ch += 2) {
		uint16_t cv = (ch[0] << 8)
				| ch[1];
		if(!(cv <= 65533)) return -1;
	}
	return 0;
}

static int
memb_teletexString_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const TeletexString_t *st = (const TeletexString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_printableString_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const PrintableString_t *st = (const PrintableString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size;

	if((size >= 1)
		 && !check_permitted_alphabet_3(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_universalString_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const UniversalString_t *st = (const UniversalString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size >> 2;	/* 4 byte per character */

	if((size >= 1)
		 && !check_permitted_alphabet_4(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_utf8String_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const UTF8String_t *st = (const UTF8String_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = UTF8String_length(st);
	if((ssize_t)size < 0) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: UTF-8: broken encoding (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	if((size >= 1)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_bmpString_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const BMPString_t *st = (const BMPString_t *)sptr;
	size_t size;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	size = st->size >> 1;	/* 2 byte per character */

	if((size >= 1)
		 && !check_permitted_alphabet_6(st)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_DirectoryString_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DirectoryString, choice.teletexString),
		(ASN_TAG_CLASS_UNIVERSAL | (20 << 2)),
		0,
		&asn_DEF_TeletexString,
		memb_teletexString_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"teletexString"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DirectoryString, choice.printableString),
		(ASN_TAG_CLASS_UNIVERSAL | (19 << 2)),
		0,
		&asn_DEF_PrintableString,
		memb_printableString_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"printableString"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DirectoryString, choice.universalString),
		(ASN_TAG_CLASS_UNIVERSAL | (28 << 2)),
		0,
		&asn_DEF_UniversalString,
		memb_universalString_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"universalString"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DirectoryString, choice.utf8String),
		(ASN_TAG_CLASS_UNIVERSAL | (12 << 2)),
		0,
		&asn_DEF_UTF8String,
		memb_utf8String_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"utf8String"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DirectoryString, choice.bmpString),
		(ASN_TAG_CLASS_UNIVERSAL | (30 << 2)),
		0,
		&asn_DEF_BMPString,
		memb_bmpString_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"bmpString"
		},
};
static asn_TYPE_tag2member_t asn_MAP_DirectoryString_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (12 << 2)), 3, 0, 0 }, /* utf8String at 268 */
    { (ASN_TAG_CLASS_UNIVERSAL | (19 << 2)), 1, 0, 0 }, /* printableString at 266 */
    { (ASN_TAG_CLASS_UNIVERSAL | (20 << 2)), 0, 0, 0 }, /* teletexString at 265 */
    { (ASN_TAG_CLASS_UNIVERSAL | (28 << 2)), 2, 0, 0 }, /* universalString at 267 */
    { (ASN_TAG_CLASS_UNIVERSAL | (30 << 2)), 4, 0, 0 } /* bmpString at 269 */
};
static asn_CHOICE_specifics_t asn_SPC_DirectoryString_specs_1 = {
	sizeof(struct DirectoryString),
	offsetof(struct DirectoryString, _asn_ctx),
	offsetof(struct DirectoryString, present),
	sizeof(((struct DirectoryString *)0)->present),
	asn_MAP_DirectoryString_tag2el_1,
	5,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_DirectoryString = {
	"DirectoryString",
	"DirectoryString",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_DirectoryString_1,
	5,	/* Elements count */
	&asn_SPC_DirectoryString_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "EDIPartyName.h"

static asn_TYPE_member_t asn_MBR_EDIPartyName_1[] = {
	{ ATF_POINTER, 1, offsetof(struct EDIPartyName, nameAssigner),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_DirectoryString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"nameAssigner"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct EDIPartyName, partyName),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_DirectoryString,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"partyName"
		},
};
static ber_tlv_tag_t asn_DEF_EDIPartyName_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_EDIPartyName_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* nameAssigner at 367 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* partyName at 368 */
};
static asn_SEQUENCE_specifics_t asn_SPC_EDIPartyName_specs_1 = {
	sizeof(struct EDIPartyName),
	offsetof(struct EDIPartyName, _asn_ctx),
	asn_MAP_EDIPartyName_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_EDIPartyName = {
	"EDIPartyName",
	"EDIPartyName",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_EDIPartyName_tags_1,
	sizeof(asn_DEF_EDIPartyName_tags_1)
		/sizeof(asn_DEF_EDIPartyName_tags_1[0]), /* 1 */
	asn_DEF_EDIPartyName_tags_1,	/* Same as above */
	sizeof(asn_DEF_EDIPartyName_tags_1)
		/sizeof(asn_DEF_EDIPartyName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_EDIPartyName_1,
	2,	/* Elements count */
	&asn_SPC_EDIPartyName_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "AnotherName.h"

static asn_TYPE_member_t asn_MBR_AnotherName_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct AnotherName, type_id),
		(ASN_TAG_CLASS_UNIVERSAL | (6 << 2)),
		0,
		&asn_DEF_OBJECT_IDENTIFIER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"type-id"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct AnotherName, value),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ANY,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"value"
		},
};
static ber_tlv_tag_t asn_DEF_AnotherName_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_AnotherName_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (6 << 2)), 0, 0, 0 }, /* type-id at 363 */
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 1, 0, 0 } /* value at 364 */
};
static asn_SEQUENCE_specifics_t asn_SPC_AnotherName_specs_1 = {
	sizeof(struct AnotherName),
	offsetof(struct AnotherName, _asn_ctx),
	asn_MAP_AnotherName_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_AnotherName = {
	"AnotherName",
	"AnotherName",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_AnotherName_tags_1,
	sizeof(asn_DEF_AnotherName_tags_1)
		/sizeof(asn_DEF_AnotherName_tags_1[0]), /* 1 */
	asn_DEF_AnotherName_tags_1,	/* Same as above */
	sizeof(asn_DEF_AnotherName_tags_1)
		/sizeof(asn_DEF_AnotherName_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_AnotherName_1,
	2,	/* Elements count */
	&asn_SPC_AnotherName_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "GeneralName.h"

static asn_TYPE_member_t asn_MBR_GeneralName_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.otherName),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_AnotherName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"otherName"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.rfc822Name),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_IA5String,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"rfc822Name"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.dNSName),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_IA5String,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"dNSName"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.x400Address),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_ORAddress,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"x400Address"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.directoryName),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_Name,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"directoryName"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.ediPartyName),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_EDIPartyName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"ediPartyName"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.uniformResourceIdentifier),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_IA5String,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"uniformResourceIdentifier"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.iPAddress),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"iPAddress"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct GeneralName, choice.registeredID),
		(ASN_TAG_CLASS_CONTEXT | (8 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_OBJECT_IDENTIFIER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"registeredID"
		},
};
static asn_TYPE_tag2member_t asn_MAP_GeneralName_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* otherName at 352 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* rfc822Name at 353 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* dNSName at 354 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* x400Address at 355 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* directoryName at 356 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* ediPartyName at 357 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* uniformResourceIdentifier at 358 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 }, /* iPAddress at 359 */
    { (ASN_TAG_CLASS_CONTEXT | (8 << 2)), 8, 0, 0 } /* registeredID at 360 */
};
static asn_CHOICE_specifics_t asn_SPC_GeneralName_specs_1 = {
	sizeof(struct GeneralName),
	offsetof(struct GeneralName, _asn_ctx),
	offsetof(struct GeneralName, present),
	sizeof(((struct GeneralName *)0)->present),
	asn_MAP_GeneralName_tag2el_1,
	9,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_GeneralName = {
	"GeneralName",
	"GeneralName",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	0,	/* No PER visible constraints */
	asn_MBR_GeneralName_1,
	9,	/* Elements count */
	&asn_SPC_GeneralName_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "GeneralNames.h"

static asn_TYPE_member_t asn_MBR_GeneralNames_1[] = {
	{ ATF_POINTER, 0, 0,
		-1 /* Ambiguous tag (CHOICE?) */,
		0,
		&asn_DEF_GeneralName,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_GeneralNames_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_GeneralNames_specs_1 = {
	sizeof(struct GeneralNames),
	offsetof(struct GeneralNames, _asn_ctx),
	2,	/* XER encoding is XMLValueList */
};
asn_TYPE_descriptor_t asn_DEF_GeneralNames = {
	"GeneralNames",
	"GeneralNames",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_GeneralNames_tags_1,
	sizeof(asn_DEF_GeneralNames_tags_1)
		/sizeof(asn_DEF_GeneralNames_tags_1[0]), /* 1 */
	asn_DEF_GeneralNames_tags_1,	/* Same as above */
	sizeof(asn_DEF_GeneralNames_tags_1)
		/sizeof(asn_DEF_GeneralNames_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_GeneralNames_1,
	1,	/* Single element */
	&asn_SPC_GeneralNames_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "AuthorityKeyIdentifier.h"

static asn_TYPE_member_t asn_MBR_AuthorityKeyIdentifier_1[] = {
	{ ATF_POINTER, 3, offsetof(struct AuthorityKeyIdentifier, keyIdentifier),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_KeyIdentifier,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"keyIdentifier"
		},
	{ ATF_POINTER, 2, offsetof(struct AuthorityKeyIdentifier, authorityCertIssuer),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_GeneralNames,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"authorityCertIssuer"
		},
	{ ATF_POINTER, 1, offsetof(struct AuthorityKeyIdentifier, authorityCertSerialNumber),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		+1,	/* EXPLICIT tag at current level */
		&asn_DEF_CertificateSerialNumber,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"authorityCertSerialNumber"
		},
};
static ber_tlv_tag_t asn_DEF_AuthorityKeyIdentifier_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_AuthorityKeyIdentifier_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* keyIdentifier at 344 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* authorityCertIssuer at 345 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* authorityCertSerialNumber at 346 */
};
static asn_SEQUENCE_specifics_t asn_SPC_AuthorityKeyIdentifier_specs_1 = {
	sizeof(struct AuthorityKeyIdentifier),
	offsetof(struct AuthorityKeyIdentifier, _asn_ctx),
	asn_MAP_AuthorityKeyIdentifier_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_AuthorityKeyIdentifier = {
	"AuthorityKeyIdentifier",
	"AuthorityKeyIdentifier",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_AuthorityKeyIdentifier_tags_1,
	sizeof(asn_DEF_AuthorityKeyIdentifier_tags_1)
		/sizeof(asn_DEF_AuthorityKeyIdentifier_tags_1[0]), /* 1 */
	asn_DEF_AuthorityKeyIdentifier_tags_1,	/* Same as above */
	sizeof(asn_DEF_AuthorityKeyIdentifier_tags_1)
		/sizeof(asn_DEF_AuthorityKeyIdentifier_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_AuthorityKeyIdentifier_1,
	3,	/* Elements count */
	&asn_SPC_AuthorityKeyIdentifier_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "KeyUsage.h"

int
KeyUsage_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_BIT_STRING.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using BIT_STRING,
 * so here we adjust the DEF accordingly.
 */
static void
KeyUsage_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_BIT_STRING.free_struct;
	td->print_struct   = asn_DEF_BIT_STRING.print_struct;
	td->ber_decoder    = asn_DEF_BIT_STRING.ber_decoder;
	td->der_encoder    = asn_DEF_BIT_STRING.der_encoder;
	td->xer_decoder    = asn_DEF_BIT_STRING.xer_decoder;
	td->xer_encoder    = asn_DEF_BIT_STRING.xer_encoder;
	td->uper_decoder   = asn_DEF_BIT_STRING.uper_decoder;
	td->uper_encoder   = asn_DEF_BIT_STRING.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_BIT_STRING.per_constraints;
	td->elements       = asn_DEF_BIT_STRING.elements;
	td->elements_count = asn_DEF_BIT_STRING.elements_count;
	td->specifics      = asn_DEF_BIT_STRING.specifics;
}

void
KeyUsage_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	KeyUsage_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
KeyUsage_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	KeyUsage_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
KeyUsage_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	KeyUsage_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
KeyUsage_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	KeyUsage_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
KeyUsage_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	KeyUsage_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
KeyUsage_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	KeyUsage_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_KeyUsage_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (3 << 2))
};
asn_TYPE_descriptor_t asn_DEF_KeyUsage = {
	"KeyUsage",
	"KeyUsage",
	KeyUsage_free,
	KeyUsage_print,
	KeyUsage_constraint,
	KeyUsage_decode_ber,
	KeyUsage_encode_der,
	KeyUsage_decode_xer,
	KeyUsage_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_KeyUsage_tags_1,
	sizeof(asn_DEF_KeyUsage_tags_1)
		/sizeof(asn_DEF_KeyUsage_tags_1[0]), /* 1 */
	asn_DEF_KeyUsage_tags_1,	/* Same as above */
	sizeof(asn_DEF_KeyUsage_tags_1)
		/sizeof(asn_DEF_KeyUsage_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* Defined elsewhere */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "BasicConstraints.h"

static int
memb_pathLenConstraint_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const INTEGER_t *st = (const INTEGER_t *)sptr;
	long value;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	/* Check if the sign bit is present */
	value = st->buf ? ((st->buf[0] & 0x80) ? -1 : 1) : 0;

	if((value >= 0)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_BasicConstraints_1[] = {
	{ ATF_POINTER, 2, offsetof(struct BasicConstraints, cA),
		(ASN_TAG_CLASS_UNIVERSAL | (1 << 2)),
		0,
		&asn_DEF_BOOLEAN,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"cA"
		},
	{ ATF_POINTER, 1, offsetof(struct BasicConstraints, pathLenConstraint),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		memb_pathLenConstraint_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"pathLenConstraint"
		},
};
static ber_tlv_tag_t asn_DEF_BasicConstraints_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_BasicConstraints_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (1 << 2)), 0, 0, 0 }, /* cA at 339 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, 0, 0 } /* pathLenConstraint at 340 */
};
static asn_SEQUENCE_specifics_t asn_SPC_BasicConstraints_specs_1 = {
	sizeof(struct BasicConstraints),
	offsetof(struct BasicConstraints, _asn_ctx),
	asn_MAP_BasicConstraints_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_BasicConstraints = {
	"BasicConstraints",
	"BasicConstraints",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_BasicConstraints_tags_1,
	sizeof(asn_DEF_BasicConstraints_tags_1)
		/sizeof(asn_DEF_BasicConstraints_tags_1[0]), /* 1 */
	asn_DEF_BasicConstraints_tags_1,	/* Same as above */
	sizeof(asn_DEF_BasicConstraints_tags_1)
		/sizeof(asn_DEF_BasicConstraints_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BasicConstraints_1,
	2,	/* Elements count */
	&asn_SPC_BasicConstraints_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "ScramblerCapabilities.h"

static int
memb_capability_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const INTEGER_t *st = (const INTEGER_t *)sptr;
	long value;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	/* Check if the sign bit is present */
	value = st->buf ? ((st->buf[0] & 0x80) ? -1 : 1) : 0;

	if((value >= 0)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_version_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	const INTEGER_t *st = (const INTEGER_t *)sptr;
	long value;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	/* Check if the sign bit is present */
	value = st->buf ? ((st->buf[0] & 0x80) ? -1 : 1) : 0;

	if((value >= 0)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_TYPE_member_t asn_MBR_ScramblerCapabilities_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ScramblerCapabilities, capability),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		memb_capability_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"capability"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ScramblerCapabilities, version),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		memb_version_constraint_1,
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"version"
		},
};
static ber_tlv_tag_t asn_DEF_ScramblerCapabilities_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ScramblerCapabilities_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 1 }, /* capability at 414 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, -1, 0 } /* version at 415 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ScramblerCapabilities_specs_1 = {
	sizeof(struct ScramblerCapabilities),
	offsetof(struct ScramblerCapabilities, _asn_ctx),
	asn_MAP_ScramblerCapabilities_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ScramblerCapabilities = {
	"ScramblerCapabilities",
	"ScramblerCapabilities",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ScramblerCapabilities_tags_1,
	sizeof(asn_DEF_ScramblerCapabilities_tags_1)
		/sizeof(asn_DEF_ScramblerCapabilities_tags_1[0]), /* 1 */
	asn_DEF_ScramblerCapabilities_tags_1,	/* Same as above */
	sizeof(asn_DEF_ScramblerCapabilities_tags_1)
		/sizeof(asn_DEF_ScramblerCapabilities_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ScramblerCapabilities_1,
	2,	/* Elements count */
	&asn_SPC_ScramblerCapabilities_specs_1	/* Additional specs */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_extensionsAdded.asn1"
 */

//#include <asn_internal.h>
//
//#include "CiplusInfo.h"

int
CiplusInfo_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_BIT_STRING.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using BIT_STRING,
 * so here we adjust the DEF accordingly.
 */
static void
CiplusInfo_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_BIT_STRING.free_struct;
	td->print_struct   = asn_DEF_BIT_STRING.print_struct;
	td->ber_decoder    = asn_DEF_BIT_STRING.ber_decoder;
	td->der_encoder    = asn_DEF_BIT_STRING.der_encoder;
	td->xer_decoder    = asn_DEF_BIT_STRING.xer_decoder;
	td->xer_encoder    = asn_DEF_BIT_STRING.xer_encoder;
	td->uper_decoder   = asn_DEF_BIT_STRING.uper_decoder;
	td->uper_encoder   = asn_DEF_BIT_STRING.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_BIT_STRING.per_constraints;
	td->elements       = asn_DEF_BIT_STRING.elements;
	td->elements_count = asn_DEF_BIT_STRING.elements_count;
	td->specifics      = asn_DEF_BIT_STRING.specifics;
}

void
CiplusInfo_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	CiplusInfo_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
CiplusInfo_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	CiplusInfo_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
CiplusInfo_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	CiplusInfo_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
CiplusInfo_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	CiplusInfo_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
CiplusInfo_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	CiplusInfo_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
CiplusInfo_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	CiplusInfo_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_CiplusInfo_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (3 << 2))
};
asn_TYPE_descriptor_t asn_DEF_CiplusInfo = {
	"CiplusInfo",
	"CiplusInfo",
	CiplusInfo_free,
	CiplusInfo_print,
	CiplusInfo_constraint,
	CiplusInfo_decode_ber,
	CiplusInfo_encode_der,
	CiplusInfo_decode_xer,
	CiplusInfo_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_CiplusInfo_tags_1,
	sizeof(asn_DEF_CiplusInfo_tags_1)
		/sizeof(asn_DEF_CiplusInfo_tags_1[0]), /* 1 */
	asn_DEF_CiplusInfo_tags_1,	/* Same as above */
	sizeof(asn_DEF_CiplusInfo_tags_1)
		/sizeof(asn_DEF_CiplusInfo_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_defs_CI+1.1Added.asn1"
 */

//#include <asn_internal.h>
//
//#include "CicamBrandId.h"

int
CicamBrandId_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;

	if(!sptr) {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}

	value = *(const long *)sptr;

	if((value >= 1 && value <= 65535)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		_ASN_CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

/*
 * This type is implemented using NativeInteger,
 * so here we adjust the DEF accordingly.
 */
static void
CicamBrandId_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NativeInteger.free_struct;
	td->print_struct   = asn_DEF_NativeInteger.print_struct;
	td->ber_decoder    = asn_DEF_NativeInteger.ber_decoder;
	td->der_encoder    = asn_DEF_NativeInteger.der_encoder;
	td->xer_decoder    = asn_DEF_NativeInteger.xer_decoder;
	td->xer_encoder    = asn_DEF_NativeInteger.xer_encoder;
	td->uper_decoder   = asn_DEF_NativeInteger.uper_decoder;
	td->uper_encoder   = asn_DEF_NativeInteger.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NativeInteger.per_constraints;
	td->elements       = asn_DEF_NativeInteger.elements;
	td->elements_count = asn_DEF_NativeInteger.elements_count;
	td->specifics      = asn_DEF_NativeInteger.specifics;
}

void
CicamBrandId_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	CicamBrandId_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
CicamBrandId_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	CicamBrandId_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
CicamBrandId_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	CicamBrandId_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
CicamBrandId_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	CicamBrandId_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
CicamBrandId_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	CicamBrandId_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
CicamBrandId_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	CicamBrandId_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

static ber_tlv_tag_t asn_DEF_CicamBrandId_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (2 << 2))
};
asn_TYPE_descriptor_t asn_DEF_CicamBrandId = {
	"CicamBrandId",
	"CicamBrandId",
	CicamBrandId_free,
	CicamBrandId_print,
	CicamBrandId_constraint,
	CicamBrandId_decode_ber,
	CicamBrandId_encode_der,
	CicamBrandId_decode_xer,
	CicamBrandId_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_CicamBrandId_tags_1,
	sizeof(asn_DEF_CicamBrandId_tags_1)
		/sizeof(asn_DEF_CicamBrandId_tags_1[0]), /* 1 */
	asn_DEF_CicamBrandId_tags_1,	/* Same as above */
	sizeof(asn_DEF_CicamBrandId_tags_1)
		/sizeof(asn_DEF_CicamBrandId_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	0, 0,	/* No members */
	0	/* No specifics */
};

/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "x509_defs_CI+1.1Added.asn1"
 */

//#include <asn_internal.h>
//
//#include "RSAPublicKey.h"

static asn_TYPE_member_t asn_MBR_RSAPublicKey_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct RSAPublicKey, modulus),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"modulus"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct RSAPublicKey, publicExponent),
		(ASN_TAG_CLASS_UNIVERSAL | (2 << 2)),
		0,
		&asn_DEF_INTEGER,
		0,	/* Defer constraints checking to the member type */
		0,	/* PER is not compiled, use -gen-PER */
		0,
		"publicExponent"
		},
};
static ber_tlv_tag_t asn_DEF_RSAPublicKey_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_RSAPublicKey_tag2el_1[] = {
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 0, 0, 1 }, /* modulus at 424 */
    { (ASN_TAG_CLASS_UNIVERSAL | (2 << 2)), 1, -1, 0 } /* publicExponent at 425 */
};
static asn_SEQUENCE_specifics_t asn_SPC_RSAPublicKey_specs_1 = {
	sizeof(struct RSAPublicKey),
	offsetof(struct RSAPublicKey, _asn_ctx),
	asn_MAP_RSAPublicKey_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_RSAPublicKey = {
	"RSAPublicKey",
	"RSAPublicKey",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	0, 0,	/* No PER support, use "-gen-PER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_RSAPublicKey_tags_1,
	sizeof(asn_DEF_RSAPublicKey_tags_1)
		/sizeof(asn_DEF_RSAPublicKey_tags_1[0]), /* 1 */
	asn_DEF_RSAPublicKey_tags_1,	/* Same as above */
	sizeof(asn_DEF_RSAPublicKey_tags_1)
		/sizeof(asn_DEF_RSAPublicKey_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_RSAPublicKey_1,
	2,	/* Elements count */
	&asn_SPC_RSAPublicKey_specs_1	/* Additional specs */
};

// end CI+ extensions

// start field parsing

int GetArcsFromOID(OBJECT_IDENTIFIER_t *oid, unsigned long *arcs, int arcSize)
{
	int arc_type_size = sizeof(long);	// sizeof(long)
	int arc_slots = arcSize;
	int count;	// Real number of arcs.

	count = OBJECT_IDENTIFIER_get_arcs(oid, arcs,
					arc_type_size, arc_slots);

	return count;
}

BOOL IsSameOID(unsigned long* targetOID, int targetOIDLen, unsigned long* sourceOID, int sourceOIDLen)
{
	int i;

	if (targetOIDLen != sourceOIDLen)
	{
		return FALSE;
	}

	for (i = 0; i < sourceOIDLen; i++)
	{
		if (targetOID[i] != sourceOID[i])
		{
			return FALSE;
		}
	}

	return TRUE;
}

// get X501 Name attribute
int GetX501DNFromName_t(AttributeValue_t **pAttrList, Name_t* name, unsigned long* targetOID, int targetLen)
{
	int i;
	int listCount = name->choice.rdnSequence.list.count;
	RDNSequence_t rdnSeq = name->choice.rdnSequence;
	unsigned long arcs[10];
	int arcsLen;
	BOOL bIsSameOID = FALSE;
	int iMatchedNameListCount = 0;

	// compare OIDs and return matched value
	for (i = 0; i < listCount; i++)
	{
		AttributeType_t  *pAttrType = &(rdnSeq.list.array[i][0].list.array[0][0].type);
		AttributeValue_t *pAttrValue;

		arcsLen = GetArcsFromOID(pAttrType, arcs, targetLen);
		bIsSameOID = IsSameOID(arcs, arcsLen, targetOID, targetLen);

		if (TRUE == bIsSameOID)
		{
			pAttrValue = &(rdnSeq.list.array[i][0].list.array[0][0].value);
			pAttrList[iMatchedNameListCount] = pAttrValue;
			iMatchedNameListCount++;
		}
	}

	return iMatchedNameListCount;
}

unsigned long CountryOID[] = {2, 5, 4, 6};
unsigned long StateOID[] = {2, 5, 4, 8};
unsigned long LocationOID[] = {2, 5, 4, 7};
unsigned long OrganizationNameOID[] = {2, 5, 4, 10};
unsigned long OrganizationalUnitNameOID[] = {2, 5, 4, 11};
unsigned long CommonNameOID[] = {2, 5, 4, 3};

int GetX501CountryFromName_t(AttributeValue_t **pAttrList, Name_t* name)
{
	return GetX501DNFromName_t(pAttrList, name, CountryOID, sizeof(CountryOID) / sizeof(CountryOID[0]));
}

int GetX501StateFromName_t(AttributeValue_t **pAttrList, Name_t* name)
{
	return GetX501DNFromName_t(pAttrList, name, StateOID, sizeof(StateOID) / sizeof(StateOID[0]));
}

int GetX501LocationName_t(AttributeValue_t **pAttrList, Name_t* name)
{
	return GetX501DNFromName_t(pAttrList, name, LocationOID, sizeof(LocationOID) / sizeof(LocationOID[0]));
}

int GetX501OrganizationNameFromName_t(AttributeValue_t **pAttrList, Name_t* name)
{
	return GetX501DNFromName_t(pAttrList, name, OrganizationNameOID, sizeof(OrganizationNameOID) / sizeof(OrganizationNameOID[0]));
}

int GetX501OrganizationalUnitNameFromName_t(AttributeValue_t **pAttrList, Name_t* name)
{
	return GetX501DNFromName_t(pAttrList, name, OrganizationalUnitNameOID, sizeof(OrganizationalUnitNameOID) / sizeof(OrganizationalUnitNameOID[0]));
}

int GetX501CommonNameFromName_t(AttributeValue_t **pAttrList, Name_t* name)
{
	return GetX501DNFromName_t(pAttrList, name, CommonNameOID, sizeof(CommonNameOID) / sizeof(CommonNameOID[0]));
}

// get specified extensions
Extension_t* GetExtension(Certificate_t *cert, unsigned long* targetOID, int targetLen)
{
	int i;
	int listCount = cert->tbsCertificate.extensions->list.count;
	Extensions_t* exts = cert->tbsCertificate.extensions;

	unsigned long arcs[10];
	int arcsLen;
	BOOL bIsSameOID = FALSE;

	// compare OIDs and return matched value
	for (i = 0; i < listCount; i++)
	{
		arcsLen = GetArcsFromOID(&(exts->list.array[i][0].extnID), arcs, targetLen);
		bIsSameOID = IsSameOID(arcs, arcsLen, targetOID, targetLen);

		if (TRUE == bIsSameOID)
		{
			return &(exts->list.array[i][0]);
		}
	}

	return NULL;
}

// id-ce - {2, 5, 29}
// id-pe - {1, 3, 6, 1, 5, 5, 7, 1}
unsigned long SubjectKeyIdentifierOID[] = {2, 5, 29, 14};		// id-ce 14
unsigned long AuthorityKeyIdentifierOID[] = {2, 5, 29, 35};		// id-ce 35
unsigned long KeyUsageOID[] = {2, 5, 29, 15};					// id-ce 15
unsigned long BasicConstraintsOID[] = {2, 5, 29, 19};			// id-ce 19
unsigned long ScramblerCapabilitiesOID[] = {1, 3, 6, 1, 5, 5, 7, 1, 25};	// id-pe 25
unsigned long CIplusInfoOID[] = {1, 3, 6, 1, 5, 5, 7, 1, 26};				// id-pe 26
unsigned long CicamBrandIdOID[] = {1, 3, 6, 1, 5, 5, 7, 1, 27};				// id-pe 27

// brand / device certificate의 mandatory extension들이 아닌 extension들은 critical일 수 없다는 것을 확인하기 위한 변수
#define BrandCertificateMandatoryExtensionsCnt		4
unsigned long *BrandCertificateMandatoryExtensionsOIDs[BrandCertificateMandatoryExtensionsCnt]
	= {KeyUsageOID, SubjectKeyIdentifierOID, AuthorityKeyIdentifierOID, BasicConstraintsOID};
unsigned long BrandCertificateMandatoryExtensionsOIDsLengths[BrandCertificateMandatoryExtensionsCnt] = {4, 4, 4, 4};
#define DeviceCertificateMandatoryExtensionsCnt	6
unsigned long *DeviceCertificateMandatoryExtensionsOIDs[DeviceCertificateMandatoryExtensionsCnt]
	= {KeyUsageOID, AuthorityKeyIdentifierOID, BasicConstraintsOID, ScramblerCapabilitiesOID, CIplusInfoOID, CicamBrandIdOID};
unsigned long DeviceCertificateMandatoryExtensionOIDsLengths[DeviceCertificateMandatoryExtensionsCnt] = {4, 4, 4, 9, 9, 9};

BOOL IsNoOtherExtensionMarkedAsCritical(Certificate_t *cert, unsigned long* mandatoryExtensionOIDs[], int iExtCnt
										, unsigned long mandatoryExtensionOIDsLengths[])
{
	// BrandCertificateMandatoryExtensionsOIDs가 아닌 extension 들에서 critical이 있으면 안된다
	int i, k;
	int listCount = cert->tbsCertificate.extensions->list.count;
	Extensions_t* exts = cert->tbsCertificate.extensions;

	unsigned long arcs[10];
	int arcsLen;
	BOOL bIsSameOID;

	// compare OIDs
	for (i = 0; i < listCount; i++)
	{
		arcsLen = GetArcsFromOID(&(exts->list.array[i][0].extnID), arcs, 10);

		// Madatory Extension이면 볼 필요 없음 -> break
		bIsSameOID = FALSE;
		for (k = 0; k < iExtCnt; ++k)
		{
			bIsSameOID = IsSameOID(arcs, arcsLen
				, mandatoryExtensionOIDs[k], mandatoryExtensionOIDsLengths[k]);
			if (bIsSameOID)
			{
				break;
			}
		}

		// Mandatory Extension이 아닐시에는 critical이 아니어야 한다.
		if (FALSE == bIsSameOID)
		{
			#if 0
			{
				int z;
				VK_Print("OID = [ ");
				for (z = 0; z < arcsLen; z++)
					VK_Print("%ld ", arcs[z]);
				VK_Print("] -> ");
				VK_Print("%d\n", exts->list.array[i][0].critical != NULL ? *(exts->list.array[i][0].critical) : -1);
			}
			#endif
			if (exts->list.array[i][0].critical)
			{
				if (0 != *(exts->list.array[i][0].critical))
				{
					return FALSE;
				}
			}
		}
	}	// end for i

	return TRUE;
}

BOOL IsNoOtherExtensionMarkedAsCriticalOfBrandCertificate(Certificate_t *brandCert)
{
	return IsNoOtherExtensionMarkedAsCritical(brandCert, BrandCertificateMandatoryExtensionsOIDs
		, BrandCertificateMandatoryExtensionsCnt, BrandCertificateMandatoryExtensionsOIDsLengths);
}

BOOL IsNoOtherExtensionMarkedAsCriticalOfDeviceCertificate(Certificate_t *deviceCert)
{
	return IsNoOtherExtensionMarkedAsCritical(deviceCert, DeviceCertificateMandatoryExtensionsOIDs
		, DeviceCertificateMandatoryExtensionsCnt, DeviceCertificateMandatoryExtensionOIDsLengths);
}

// get specified extensions
SubjectKeyIdentifier_t*	GetSubjectKeyIdentifierFromExtension(Certificate_t *cert, BOOLEAN_t *critical)
{
	SubjectKeyIdentifier_t *subKeyIdentifier =0;
	Extension_t* ext = 0;

	ext = GetExtension(cert, SubjectKeyIdentifierOID, sizeof(SubjectKeyIdentifierOID) / sizeof(SubjectKeyIdentifierOID[0]));
	if (ext)
	{
		asn_dec_rval_t rval; // Decoder return value //

		rval = ber_decode(0, &asn_DEF_SubjectKeyIdentifier,
			(void **)&subKeyIdentifier, ext->extnValue.buf, ext->extnValue.size);

		if(rval.code != RC_OK || rval.consumed != ext->extnValue.size)
		{
			return NULL;
		}

		if (ext->critical)
		{
			*critical = *(ext->critical);
		}
		else
		{
			*critical = 0;
		}
		return subKeyIdentifier;
	}
	else
	{
		return NULL;
	}
}

#ifdef UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
BOOL	bForceDecoding = FALSE;
#endif // UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER

AuthorityKeyIdentifier_t*	GetAuthorityKeyIdentifierFromExtension(Certificate_t *cert, BOOLEAN_t *critical)
{
	AuthorityKeyIdentifier_t *autKeyIdentifier =0;
	Extension_t* ext = 0;

	ext = GetExtension(cert, AuthorityKeyIdentifierOID, sizeof(AuthorityKeyIdentifierOID) / sizeof(AuthorityKeyIdentifierOID[0]));
	if (ext)
	{
		asn_dec_rval_t rval; // Decoder return value //

		rval = ber_decode(0, &asn_DEF_AuthorityKeyIdentifier,
			(void **)&autKeyIdentifier, ext->extnValue.buf, ext->extnValue.size);

		if(rval.code != RC_OK || rval.consumed != ext->extnValue.size)
		{
#ifdef UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
			// ASN_LIB에서 Key Identifier Octet String tag를 제대로 처리하지 못하고 있다.
			// AuthorityKeyIdentifier의 decoding에서는 임시 방편으로
			// T/L 2 byte 이후의 내용을 가지고 AuthorityKeyIdentifier의
			// KeyIdentifier의 내용으로 하도록 한다.

			int iAlreadyConsumedByte = rval.consumed;
			unsigned char ucCheckTag;
			BOOL bIsKeyIdentifierTag = FALSE;

			// 남아 있는 buffer 내용이 최소 3 byte 이상은 되어야 한다 (TLV)
			if (ext->extnValue.size - iAlreadyConsumedByte < 3)
			{
				return NULL;
			}

			ucCheckTag = ext->extnValue.buf[iAlreadyConsumedByte];

			// iCheckTag의 하위 4bit의 값이 0 이어야 한다. ( [0] )
			if (0 == (ucCheckTag & 0x0F))
			{
				bIsKeyIdentifierTag = TRUE;
			}

			if (bIsKeyIdentifierTag)
			{
				autKeyIdentifier = (AuthorityKeyIdentifier_t*)MALLOC(sizeof(AuthorityKeyIdentifier_t));
				autKeyIdentifier->keyIdentifier = (KeyIdentifier_t*)MALLOC(sizeof(KeyIdentifier_t));
				autKeyIdentifier->keyIdentifier->size = ext->extnValue.buf[iAlreadyConsumedByte+1];
				autKeyIdentifier->keyIdentifier->buf = (uint8_t*)MALLOC(sizeof(uint8_t) * autKeyIdentifier->keyIdentifier->size);
				memcpy(autKeyIdentifier->keyIdentifier->buf, &ext->extnValue.buf[iAlreadyConsumedByte+2], autKeyIdentifier->keyIdentifier->size);
				bForceDecoding = TRUE;
			}
			else
			{
				return NULL;
			}
#else
			return NULL;
#endif // UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
		}

		if (ext->critical)
		{
			*critical = *(ext->critical);
		}
		else
		{
			*critical = 0;
		}
		return autKeyIdentifier;
	}
	else
	{
		return NULL;
	}
}

KeyUsage_t*	GetKeyUsageFromExtension(Certificate_t *cert, BOOLEAN_t *critical)
{
	KeyUsage_t *keyUsage =0;
	Extension_t* ext = 0;

	ext = GetExtension(cert, KeyUsageOID, sizeof(KeyUsageOID) / sizeof(KeyUsageOID[0]));
	if (ext)
	{
		asn_dec_rval_t rval; // Decoder return value //

		rval = ber_decode(0, &asn_DEF_KeyUsage,
			(void **)&keyUsage, ext->extnValue.buf, ext->extnValue.size);
		if(rval.code != RC_OK || rval.consumed != ext->extnValue.size)
		{
			return NULL;
		}

		if (ext->critical)
		{
			*critical = *(ext->critical);
		}
		else
		{
			*critical = 0;
		}
		return keyUsage;
	}
	else
	{
		return NULL;
	}
}

BasicConstraints_t*	GetBasicConstraintsFromExtension(Certificate_t *cert, BOOLEAN_t *critical)
{
	BasicConstraints_t *basicConstraints =0;
	Extension_t* ext = 0;

	ext = GetExtension(cert, BasicConstraintsOID, sizeof(BasicConstraintsOID) / sizeof(BasicConstraintsOID[0]));
	if (ext)
	{
		asn_dec_rval_t rval; // Decoder return value //

		rval = ber_decode(0, &asn_DEF_BasicConstraints,
			(void **)&basicConstraints, ext->extnValue.buf, ext->extnValue.size);
		if(rval.code != RC_OK || rval.consumed != ext->extnValue.size)
		{
			return NULL;
		}

		if (ext->critical)
		{
			*critical = *(ext->critical);
		}
		else
		{
			*critical = 0;
		}
		return basicConstraints;
	}
	else
	{
		return NULL;
	}
}

ScramblerCapabilities_t*	GetScramblerCapabilitiesFromExtension(Certificate_t *cert, BOOLEAN_t *critical)
{
	ScramblerCapabilities_t *scramblerCapabilities =0;
	Extension_t* ext = 0;

	ext = GetExtension(cert, ScramblerCapabilitiesOID, sizeof(ScramblerCapabilitiesOID) / sizeof(ScramblerCapabilitiesOID[0]));
	if (ext)
	{
		asn_dec_rval_t rval; // Decoder return value //

		rval = ber_decode(0, &asn_DEF_ScramblerCapabilities,
			(void **)&scramblerCapabilities, ext->extnValue.buf, ext->extnValue.size);
		if(rval.code != RC_OK || rval.consumed != ext->extnValue.size)
		{
			return NULL;
		}

		if (ext->critical)
		{
			*critical = *(ext->critical);
		}
		else
		{
			*critical = 0;
		}
		return scramblerCapabilities;
	}
	else
	{
		return NULL;
	}
}

CiplusInfo_t*	GetCIplusInfoFromExtension(Certificate_t *cert, BOOLEAN_t *critical)
{
	CiplusInfo_t *ciplusInfo =0;
	Extension_t* ext = 0;

	ext = GetExtension(cert, CIplusInfoOID, sizeof(CIplusInfoOID) / sizeof(CIplusInfoOID[0]));
	if (ext)
	{
		asn_dec_rval_t rval; // Decoder return value //

		rval = ber_decode(0, &asn_DEF_CiplusInfo,
			(void **)&ciplusInfo, ext->extnValue.buf, ext->extnValue.size);
		if(rval.code != RC_OK || rval.consumed != ext->extnValue.size)
		{
			return NULL;
		}

		if (ext->critical)
		{
			*critical = *(ext->critical);
		}
		else
		{
			*critical = 0;
		}
		return ciplusInfo;
	}
	else
	{
		return NULL;
	}
}

CicamBrandId_t*	GetCicamBrandIdFromExtension(Certificate_t *cert, BOOLEAN_t *critical)
{
	CicamBrandId_t *cicamBrandId =0;
	Extension_t* ext = 0;

	ext = GetExtension(cert, CicamBrandIdOID, sizeof(CicamBrandIdOID) / sizeof(CicamBrandIdOID[0]));
	if (ext)
	{
		asn_dec_rval_t rval; // Decoder return value //

		rval = ber_decode(0, &asn_DEF_CicamBrandId,
			(void **)&cicamBrandId, ext->extnValue.buf, ext->extnValue.size);
		if(rval.code != RC_OK || rval.consumed != ext->extnValue.size)
		{
			return NULL;
		}

		if (ext->critical)
		{
			*critical = *(ext->critical);
		}
		else
		{
			*critical = 0;
		}
		return cicamBrandId;
	}
	else
	{
		return NULL;
	}
}

// end field parsing

///////// start <-- verification codes ////////////////

// Verification of the brand certificate (ROOT <--> Brand certificate compare)
/* 1) Check version (CI+ implementations shall use X.509 version 3 (02)).
   2) Check that the Issuer of the brand certificate is identical to the Subject of the root certificate.
   3) Check that the validity period of the brand certificate includes the current date and time.
   4) Check that each mandatory extension listed in section 9.3.9 exists and the values are valid.
	  Check that no other extension is marked as critical.
   5) Verify that the KeyIdentifier in the brand certificate's authority key identifier extension
	  is identical to the KeyIdentifier in the root certificate's subject key identifier extension.
   6) Verify the certificate's signature by using the RSASSA-PSS verification. */

// Verification of the device certificate (Brand <--> Device certificate compare)
/* 1) Check version (CI+ implementations shall use X.509 version 3 (02)).
   2) Check that the Issuer of the device certificate is identical to the Subject of the brand certificate.
   3) Check that the validity period of the device certificate includes the current date and time.
   4) Check that each mandatory extension listed in CI+ spec section 9.3.9 exists and the values are valid.
	  Check that no other extension is marked as critical.
   5) Verify that the KeyIdentifier in the device certificate's authority key identifier extension
	  is identical to the KeyIdentifier in the brand certificate's subject key identifier extension.
   6) Verify the certificate's signature by using the RSASSA-PSS verification.
   7) Verify that the device ID (which is part of the Subject field) contains a valid value. See CI+ spec Annex B and C.
   8) ucBindedCamId, usBindedCamBrandId, nNegotiatedCckCipher (negotiate with CAM's and s_stCiHostLicenseConst.Host_CckCipher) 셋팅 */

// 1) Check version
BOOL IsVersion3_X509Certificate(Certificate_t *cert)
{
	// 길이 1이면서 값이 2(version 3) 이어야 한다.
	if (1 != cert->tbsCertificate.version->size)
	{
		return FALSE;
	}

	if (2 != cert->tbsCertificate.version->buf[0])
	{
		return FALSE;
	}

	return TRUE;
}

BOOL IsSameAttrValue(AttributeValue_t* pSour, AttributeValue_t* pTarget)
{
	int i;

	// compare size
	if (pSour->size != pTarget->size)
	{
		return FALSE;
	}

	// compare content (buf)
	for (i = 0; i < pSour->size; ++i)
	{
		if (pSour->buf[i] != pTarget->buf[i])
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL IsNameMatch(AttributeValue_t **pSourAttrList, int iSourAttrListCnt, AttributeValue_t **pDestAttrList, int iDestAttrListCnt)
{
	BOOL bResult = TRUE;
	int i;

	if (iSourAttrListCnt != iDestAttrListCnt)
	{
		bResult = FALSE;
	}

	if (TRUE == bResult)
	{
		for (i = 0; i < iSourAttrListCnt; i++)
		{
			if (FALSE == IsSameAttrValue(pSourAttrList[i], pDestAttrList[i]))
			{
				bResult = FALSE;
				break;
			}
		}
	}

	return bResult;
}

// 2) Check Name
BOOL IsNameMatchBetweenIssuerAndSubjectCertificates(Certificate_t *subjectCert, Certificate_t *issuerCert)
{
	AttributeValue_t *pIssuerAttrValueList[10], *pSubjectAttrValueList[10];
	int iIssuerAttrValueListCount, iSubjectAttrValueListCount;
	BOOL bResult = TRUE;

	Name_t subject, issuer;
	subject = issuerCert->tbsCertificate.subject;
	issuer  = subjectCert->tbsCertificate.issuer;

	// compare country (C)
	iIssuerAttrValueListCount  = GetX501CountryFromName_t(pIssuerAttrValueList, &issuer);
	iSubjectAttrValueListCount = GetX501CountryFromName_t(pSubjectAttrValueList, &subject);
	bResult = IsNameMatch(pIssuerAttrValueList, iIssuerAttrValueListCount, pSubjectAttrValueList, iSubjectAttrValueListCount);
	if (FALSE == bResult)
	{
		return FALSE;
	}

	// compare state (ST)
	iIssuerAttrValueListCount  = GetX501StateFromName_t(pIssuerAttrValueList, &issuer);
	iSubjectAttrValueListCount = GetX501StateFromName_t(pSubjectAttrValueList, &subject);
	bResult = IsNameMatch(pIssuerAttrValueList, iIssuerAttrValueListCount, pSubjectAttrValueList, iSubjectAttrValueListCount);
	if (FALSE == bResult)
	{
		return FALSE;
	}

	// compare Location (L)
	iIssuerAttrValueListCount  = GetX501LocationName_t(pIssuerAttrValueList, &issuer);
	iSubjectAttrValueListCount = GetX501LocationName_t(pSubjectAttrValueList, &subject);
	bResult = IsNameMatch(pIssuerAttrValueList, iIssuerAttrValueListCount, pSubjectAttrValueList, iSubjectAttrValueListCount);
	if (FALSE == bResult)
	{
		return FALSE;
	}

	// compare Organization (O)
	iIssuerAttrValueListCount  = GetX501OrganizationNameFromName_t(pIssuerAttrValueList, &issuer);
	iSubjectAttrValueListCount = GetX501OrganizationNameFromName_t(pSubjectAttrValueList, &subject);
	bResult = IsNameMatch(pIssuerAttrValueList, iIssuerAttrValueListCount, pSubjectAttrValueList, iSubjectAttrValueListCount);
	if (FALSE == bResult)
	{
		return FALSE;
	}

	// compare Organization Unit (OU)
	iIssuerAttrValueListCount  = GetX501OrganizationalUnitNameFromName_t(pIssuerAttrValueList, &issuer);
	iSubjectAttrValueListCount = GetX501OrganizationalUnitNameFromName_t(pSubjectAttrValueList, &subject);
	bResult = IsNameMatch(pIssuerAttrValueList, iIssuerAttrValueListCount, pSubjectAttrValueList, iSubjectAttrValueListCount);
	if (FALSE == bResult)
	{
		return FALSE;
	}

	// compare Common Name (CN)
	iIssuerAttrValueListCount  = GetX501CommonNameFromName_t(pIssuerAttrValueList, &issuer);
	iSubjectAttrValueListCount = GetX501CommonNameFromName_t(pSubjectAttrValueList, &subject);
	bResult = IsNameMatch(pIssuerAttrValueList, iIssuerAttrValueListCount, pSubjectAttrValueList, iSubjectAttrValueListCount);
	if (FALSE == bResult)
	{
		return FALSE;
	}

	return TRUE;
}

static BOOL IsNumber(unsigned char ch)
{
	// 상수 범위 ->  - 48, 문자 범위 (A~F) -> - 55
	// '0' ~ '9'
	if ('0' <= ch && ch <= '9')
	{
		return TRUE;
	}
	return FALSE;
}

// 3) Check that the validity period
BOOL IsCertificatePeriodValid(Certificate_t *cert)
{
	unsigned char szCurTime[13], szBeforeTime[13], szAfterTime[13];
	int iTimeCompareResult, i;

	// YYMMDDHHMMSSZ format (length 13)
	if (13 != cert->tbsCertificate.validity.notBefore.choice.utcTime.size
		|| 13 != cert->tbsCertificate.validity.notAfter.choice.utcTime.size)
	{
		PrintErr(("[CICC]---> IsCertificatePeriodValid()---> Error : Invalid length !!!\n"));
		return FALSE;
	}

	// and should be numbers
	for (i = 0; i < 12; ++i)
	{
		if (FALSE == IsNumber(cert->tbsCertificate.validity.notBefore.choice.utcTime.buf[i]))
		{
			PrintErr(("[CICC]---> IsCertificatePeriodValid()---> Error : Invalid number !!!\n"));
			return FALSE;
		}
		if (FALSE == IsNumber(cert->tbsCertificate.validity.notAfter.choice.utcTime.buf[i]))
		{
			PrintErr(("[CICC]---> IsCertificatePeriodValid()---> Error : Invalid number !!!\n"));
			return FALSE;
		}
	}
	if ('Z' != cert->tbsCertificate.validity.notBefore.choice.utcTime.buf[12]
		|| 'Z' != cert->tbsCertificate.validity.notAfter.choice.utcTime.buf[12])
	{
		PrintErr(("[CICC]---> IsCertificatePeriodValid()---> Error : Invalid format !!!\n"));
		return FALSE;
	}

	// get current time as UTC format
	if (CI_GetCurrentUtcTime(szCurTime))
	{
		PrintErr(("[CICC]---> IsCertificatePeriodValid()---> Error : Can't get current time !!!\n"));
		return FALSE;
	}

	// get certificate time value
	memcpy(szBeforeTime, cert->tbsCertificate.validity.notBefore.choice.utcTime.buf, sizeof(unsigned char) * 13);
	memcpy(szAfterTime, cert->tbsCertificate.validity.notAfter.choice.utcTime.buf, sizeof(unsigned char) * 13);

	// If afterTime < beforeTime -> we consider afterTime is 21YY... (Not specificated, our opinion)
	// Instead of using 21YY, just change YY value as 99 -> 2099MMDD ...)
	iTimeCompareResult = CI_CompareUtcTimes(szBeforeTime, szAfterTime);
	if (iTimeCompareResult == 1)
	{
		szAfterTime[0] = '9';
		szAfterTime[1] = '9';
	}

	PrintNotify(("[CICC]---> IsCertificatePeriodValid()---> BeforeTime (%s), CurTime (%s), AfterTime (%s)\n", szBeforeTime, szCurTime, szAfterTime)); // 중요 로그이므로 항시 찍어 주자.

	// compare with time of certification (szBeforeTime <= szCurTime <= szAfterTime)
	iTimeCompareResult = CI_CompareUtcTimes(szBeforeTime, szCurTime);
	if (iTimeCompareResult == 1)
	{
		PrintErr(("[CICC]---> IsCertificatePeriodValid()---> Error : BeforeTime (%s) > CurTime (%s) !!!\n", szBeforeTime, szCurTime));
		return FALSE;
	}

	iTimeCompareResult = CI_CompareUtcTimes(szCurTime, szAfterTime);
	if (iTimeCompareResult == 1)
	{
		PrintErr(("[CICC]---> IsCertificatePeriodValid()---> Error : CurTime (%s) > AfterTime (%s) !!!\n", szCurTime, szAfterTime));
		return FALSE;
	}

	return TRUE;
}

BOOL IsValidKeyUsageExtensionOfBrandCertificate(Certificate_t *brandCert)
{
	// always be present and marked as critical
	// brand -> KeyCertSign : BIT STRING (5)
	BOOLEAN_t critical = 0;
	KeyUsage_t *keyUsage = GetKeyUsageFromExtension(brandCert, &critical);
	BOOL bResult = FALSE;

	if (keyUsage)
	{
		BOOL bIsKeyCertSignUsageInclude = keyUsage->buf[0] & 0x04;
		if (bIsKeyCertSignUsageInclude && critical)
		{
			bResult = TRUE;
		}
		ASN_STRUCT_FREE(asn_DEF_KeyUsage, keyUsage);
	}

	return bResult;
}

BOOL IsValidKeyUsageExtensionOfDeviceCertificate(Certificate_t *deviceCert)
{
	// always be present and marked as critical
	// device -> digitalSignature : BIT STRING (0)
	// BIT STRING 0
	BOOLEAN_t critical = 0;
	KeyUsage_t *keyUsage = GetKeyUsageFromExtension(deviceCert, &critical);
	BOOL bResult = FALSE;

	if (keyUsage)
	{
		BOOL bIsDigitalSignatureInclude = keyUsage->buf[0] & 0x80;
		if (bIsDigitalSignatureInclude && critical)
		{
			bResult = TRUE;
		}
		ASN_STRUCT_FREE(asn_DEF_KeyUsage, keyUsage);
	}

	return bResult;
}

BOOL IsValidSubjectKeyIdentifierExtension(Certificate_t *cert)
{
	// MUST NOT be marked critical
	BOOLEAN_t critical = 0;
	SubjectKeyIdentifier_t *subKeyIdentifier = GetSubjectKeyIdentifierFromExtension(cert, &critical);
	BOOL bResult = FALSE;

	if (subKeyIdentifier)
	{
		if (FALSE == critical)
		{
			bResult = TRUE;
		}
		ASN_STRUCT_FREE(asn_DEF_SubjectKeyIdentifier, subKeyIdentifier);
	}

	return bResult;
}

BOOL IsValidAuthorityKeyIdentifierExtension(Certificate_t *cert)
{
	// MUST NOT be marked critical
	BOOLEAN_t critical = 0;
	AuthorityKeyIdentifier_t *authorityKeyIdentifier = GetAuthorityKeyIdentifierFromExtension(cert, &critical);
	BOOL bResult = FALSE;

	if (authorityKeyIdentifier)
	{
		if (FALSE == critical)
		{
			bResult = TRUE;
		}

#ifdef UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
		if (bForceDecoding)
		{
			FREEMEM(authorityKeyIdentifier->keyIdentifier->buf);
			FREEMEM(authorityKeyIdentifier->keyIdentifier);
			FREEMEM(authorityKeyIdentifier);
			bForceDecoding = FALSE;
		}
		else
		{
			ASN_STRUCT_FREE(asn_DEF_AuthorityKeyIdentifier, authorityKeyIdentifier);
		}
#else
		ASN_STRUCT_FREE(asn_DEF_AuthorityKeyIdentifier, authorityKeyIdentifier);
#endif // UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
	}

	return bResult;
}

BOOL IsValidBasicConstraintsExtensionOfBrandCertificate(Certificate_t *brandCert)
{
	// brand -> cA (TRUE), pathLenConstraint (0)
	BOOLEAN_t critical = 0;
	BasicConstraints_t *basicConstraints = GetBasicConstraintsFromExtension(brandCert, &critical);
	BOOL bResult = FALSE;

	if (basicConstraints)
	{
		if (basicConstraints->cA && basicConstraints->pathLenConstraint)
		{
			if (*(basicConstraints->cA) && 0 == basicConstraints->pathLenConstraint->buf[0])
			{
				bResult = TRUE;
			}
		}
		ASN_STRUCT_FREE(asn_DEF_BasicConstraints, basicConstraints);
	}

	return bResult;
}

BOOL IsValidBasicConstraintsExtensionOfDeviceCertificate(Certificate_t *deviceCert)
{
	// device -> cA (FALSE), pathLenConstraint (-)
	BOOLEAN_t critical = 0;
	BasicConstraints_t *basicConstraints = GetBasicConstraintsFromExtension(deviceCert, &critical);
	BOOL bResult = FALSE;

	if (basicConstraints)
	{
		if (basicConstraints->cA)
		{
			if (0 == *(basicConstraints->cA) && NULL == basicConstraints->pathLenConstraint)
			{
				bResult = TRUE;
			}
		}
		else if (NULL == basicConstraints->pathLenConstraint)
		{
			bResult = TRUE;
		}
		ASN_STRUCT_FREE(asn_DEF_BasicConstraints, basicConstraints);
	}

	return bResult;
}

BOOL IsValidScramblerCapabilitiesExtension(Certificate_t *cert)
{
	// it shall be present in each device certificate and marked as critical
	BOOLEAN_t critical = 0;
	ScramblerCapabilities_t *scramblerCapabilities = GetScramblerCapabilitiesFromExtension(cert, &critical);
	BOOL bResult = FALSE;

	if (scramblerCapabilities)
	{
		if (critical)
		{
			bResult = TRUE;
		}
		ASN_STRUCT_FREE(asn_DEF_ScramblerCapabilities, scramblerCapabilities);
	}

	return bResult;
}

BOOL IsValidCiPlusInfoExtension(Certificate_t *cert)
{
	// Optional
	// shall not be declared as critical
	BOOLEAN_t critical = 0;
	CiplusInfo_t *ciplusInfo = GetCIplusInfoFromExtension(cert, &critical);
	BOOL bResult = TRUE;

	if (ciplusInfo)
	{
		if (critical)
		{
			bResult = FALSE;
		}
		ASN_STRUCT_FREE(asn_DEF_CiplusInfo, ciplusInfo);
	}

	return TRUE;
}

BOOL IsValidCICAMBrandIdentifierExtension(Certificate_t *cert)
{
	// The extension shall be optionally present in a CICAM device certificate only and shall not be declared as critical.
	BOOLEAN_t critical = 0;
	CicamBrandId_t *cicamBrandId = GetCicamBrandIdFromExtension(cert, &critical);
	BOOL bResult = TRUE;

	if (cicamBrandId)
	{
		if (critical)
		{
			bResult = FALSE;
		}

		// CicamBrandId ::= INTEGER (1..65535)
		if (*cicamBrandId < 1 || 65535 < *cicamBrandId)
		{
			bResult = FALSE;
		}

		ASN_STRUCT_FREE(asn_DEF_CicamBrandId, cicamBrandId);
	}

	return bResult;
}

// 4) Check that each mandatory extension listed in section 9.3.9 exists and the values are valid. Check that no other extension is marked as critical.
// The mandatory extension list is listed in Table 9.4 and we consider Brand and Device certificate
BOOL IsMandatoryExtensionCorrectlyListedOfBrandCertificate(Certificate_t *brandCert)
{
	// key usage
	if (FALSE == IsValidKeyUsageExtensionOfBrandCertificate(brandCert))
	{
		return FALSE;
	}

	// subject key identifier
	if (FALSE == IsValidSubjectKeyIdentifierExtension(brandCert))
	{
		return FALSE;
	}

	// authority key identifier
	if (FALSE == IsValidAuthorityKeyIdentifierExtension(brandCert))
	{
		return FALSE;
	}

	// basic constraints
	if (FALSE == IsValidBasicConstraintsExtensionOfBrandCertificate(brandCert))
	{
		return FALSE;
	}

	// Check that no other extension is marked as critical.
	if (FALSE == IsNoOtherExtensionMarkedAsCriticalOfBrandCertificate(brandCert))
	{
		return FALSE;
	}

	return TRUE;
}

/* mandatory extension list of Device certificate */
BOOL IsMandatoryExtensionCorrectlyListedOfDeviceCertificate(Certificate_t *deviceCert)
{
	// key usage
	if (FALSE == IsValidKeyUsageExtensionOfDeviceCertificate(deviceCert))
	{
		return FALSE;
	}

	// authority key identifier
	if (FALSE == IsValidAuthorityKeyIdentifierExtension(deviceCert))
	{
		return FALSE;
	}

	// basic constraints
	if (FALSE == IsValidBasicConstraintsExtensionOfDeviceCertificate(deviceCert))
	{
		return FALSE;
	}

	// scrambler capabilities
	if (FALSE == IsValidScramblerCapabilitiesExtension(deviceCert))
	{
		return FALSE;
	}

	// CI Plus info (optional)
	if (FALSE == IsValidCiPlusInfoExtension(deviceCert))
	{
		return FALSE;
	}

	// CICAM brand identifier (optional, CICAM only)
	if (FALSE == IsValidCICAMBrandIdentifierExtension(deviceCert))
	{
		return FALSE;
	}

	// Check that no other extension is marked as critical.
	if (FALSE == IsNoOtherExtensionMarkedAsCriticalOfDeviceCertificate(deviceCert))
	{
		return FALSE;
	}

	return TRUE;
}

// 5) Verify that the KeyIdentifier in the brand certificate's authority key identifier extension is identical to the KeyIdentifier in the root certificate's subject key identifier extension.
// 5) Verify that the KeyIdentifier in the device certificate's authority key identifier extension is identical to the KeyIdentifier in the brand certificate's subject key identifier extension.
BOOL IsKeyIdentifierMatchBetweenIssuerAndSubjectCertificates(Certificate_t *subjectCert, Certificate_t *issuerCert)
{
	int i;
	BOOLEAN_t critical = 0;
	SubjectKeyIdentifier_t *subKeyIdentifier = GetSubjectKeyIdentifierFromExtension(issuerCert, &critical);
	AuthorityKeyIdentifier_t *authorityKeyIdentifier = GetAuthorityKeyIdentifierFromExtension(subjectCert, &critical);
	BOOL bResult = TRUE;

	if (subKeyIdentifier)
	{
		if (authorityKeyIdentifier)
		{
			if (subKeyIdentifier->size != authorityKeyIdentifier->keyIdentifier->size)
			{
				bResult = FALSE;
			}

			for (i = 0; i < authorityKeyIdentifier->keyIdentifier->size; ++i)
			{
				if (authorityKeyIdentifier->keyIdentifier->buf[i] != subKeyIdentifier->buf[i])
				{
					bResult = FALSE;
				}
			}

#ifdef UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
			if (bForceDecoding)
			{
				FREEMEM(authorityKeyIdentifier->keyIdentifier->buf);
				FREEMEM(authorityKeyIdentifier->keyIdentifier);
				FREEMEM(authorityKeyIdentifier);
				bForceDecoding = FALSE;
			}
			else
			{
				ASN_STRUCT_FREE(asn_DEF_AuthorityKeyIdentifier, authorityKeyIdentifier);
			}
#else
			ASN_STRUCT_FREE(asn_DEF_AuthorityKeyIdentifier, authorityKeyIdentifier);
#endif // UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
		}	// end if authorityKeyIdentifier
		ASN_STRUCT_FREE(asn_DEF_SubjectKeyIdentifier, subKeyIdentifier);
	}	// end if subKeyIdentifier

	return bResult;
}

unsigned long RSAPublicKeyOID[] = {1, 2, 840, 113549, 1, 1, 1};
BOOL GetRSAPublicKey_N(Certificate_t *cert, unsigned char *pCharKey_N, int *pCharKey_N_Size)
{
	unsigned long arcs[10];
	int	 arcsLen;
	BOOL bIsRSAPublicKey;
	BOOL bResult = FALSE;

	arcsLen = GetArcsFromOID(&(cert->tbsCertificate.subjectPublicKeyInfo.algorithm.algorithm), arcs, 10);
	bIsRSAPublicKey = IsSameOID(RSAPublicKeyOID, sizeof(RSAPublicKeyOID)/sizeof(RSAPublicKeyOID[0]), arcs, arcsLen);
	if (bIsRSAPublicKey)
	{
		RSAPublicKey_t *rsaPublicKey = 0;
		asn_dec_rval_t rval; // Decoder return value //

		rval = ber_decode(0, &asn_DEF_RSAPublicKey,
			(void **)&rsaPublicKey
			, cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey.buf
			, cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey.size);
		if(rval.code != RC_OK || rval.consumed != cert->tbsCertificate.subjectPublicKeyInfo.subjectPublicKey.size)
		{
			rsaPublicKey = 0;
		}
		else
		{
			int i, iZeroLen = 0;

			// 앞 쪽의 00 으로 채워지는 값은 제거
			for (i = 0; i < rsaPublicKey->modulus.size; ++i)
			{
				if (rsaPublicKey->modulus.buf[i] != 0)
				{
					iZeroLen = i;
					break;
				}
			}

			// RSA key's public exponent shall be 65537 == 0x10001
			if (rsaPublicKey->publicExponent.size == 3)
			{
				if (rsaPublicKey->publicExponent.buf[0] == 1
					&& rsaPublicKey->publicExponent.buf[1] == 0
					&& rsaPublicKey->publicExponent.buf[2] == 1
					)
				{
					bResult = TRUE;
				}
			}

			*pCharKey_N_Size = rsaPublicKey->modulus.size - iZeroLen;
			memcpy(pCharKey_N, &rsaPublicKey->modulus.buf[iZeroLen], rsaPublicKey->modulus.size - iZeroLen);

			ASN_STRUCT_FREE(asn_DEF_RSAPublicKey, rsaPublicKey);
		}

		return bResult;
	}
	else
	{
		return FALSE;
	}
}

BOOL GetSignature(Certificate_t *cert, unsigned char *pSignBuf, int *pSignBufSize)
{
	if (cert)
	{
		*pSignBufSize = cert->signature.size;
		memcpy(pSignBuf, cert->signature.buf, cert->signature.size);
		return TRUE;
	}

	return FALSE;
}

BOOL GetCertMessage(unsigned char *pucRawCert, unsigned short usRawCertLen, unsigned char *pMessageBuf, int *pMessageBufSize)
{
	ber_tlv_len_t length;
	int iRet;

	// get tbs certificate length
	iRet = ber_fetch_length(0, &pucRawCert[5], 3, &length);
	if(iRet < 1)
	{
		return FALSE;
	}

	// tbsCert Tag 부터 value 까지 전체
	*pMessageBufSize = length + 4;
	memcpy(pMessageBuf, &pucRawCert[4], length + 4);

	return TRUE;
}

// 6) Verify the certificate's signature by using the RSASSA-PSS verification.
BOOL IsSignatureValid(Certificate_t *upperCert, Certificate_t *cert, unsigned char *pucRawCert, unsigned short usRawCertLen)
{
	RSA_public_key MDP;
	bignum n, M, S;

	// get n (signer's RSA public key)
	if (FALSE == GetRSAPublicKey_N(upperCert, n.num, &n.len))
	{
		return FALSE;
	}

	// get M (message to be verified)
	if (FALSE == GetCertMessage(pucRawCert, usRawCertLen, M.num, &M.len))
	{
		return FALSE;
	}

	// get S (signature to be verified)
	if (FALSE == GetSignature(cert, S.num, &S.len))
	{
		return FALSE;
	}

	MDP.n = &n;
	MDP.e = &bn_e;

	if (!V_RSA(&MDP, &M, &S))
	{
		return FALSE;
	}

	return TRUE;
}

static BOOL IsDigit(unsigned char ch)
{
	// 상수 범위 ->  - 48, 문자 범위 (A~F) -> - 55
	// '0' ~ '9'
	if ('0' <= ch && ch <= '9')
	{
		return TRUE;
	}
	// 'A' ~ 'F' => 10 ~ 15
	if ('A' <= ch && ch <= 'F')
	{
		return TRUE;
	}
	return FALSE;
}

// 7) Verify that the device ID (which is part of the Subject field) contains a valid value. See CI+ spec Annex B and C.
// -> check only length (is it 16 bytes?)
// subject의 CN에 담김 (16 digits)
BOOL IsDeviceIdValueValid(Certificate_t *cert)
{
	AttributeValue_t *pAttrValueList[10];
	int i, iAttrValueListCount, k;

	iAttrValueListCount = GetX501CommonNameFromName_t(pAttrValueList, &cert->tbsCertificate.subject);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		// index 0 = T / 1 = L / 2~17 Value (16 digits)
		if (pAttrValueList[i]->size != 18)
		{
			return FALSE;
		}

		// values are digit ? ('0' ~ 'F')
		for (k = 2; k < pAttrValueList[i]->size; ++k)
		{
			BOOL bDigit = IsDigit(pAttrValueList[i]->buf[k]);
			if (FALSE == bDigit)
			{
				return FALSE;
			}
		}	// end for k
	}	// end for i

	return TRUE;
}

static int	ctoi(unsigned char ch)
{
	// 상수 범위 ->  - 48, 문자 범위 (A~F) -> - 55
	// '0' ~ '9'
	if ('0' <= ch && ch <= '9')
	{
		return (ch - '0');
	}
	// 'A' ~ 'F' => 10 ~ 15
	if ('A' <= ch && ch <= 'F')
	{
		return (ch - 55);
	}
	return -1;
}

BOOL GetDeviceID(Certificate_t *cert, unsigned char* charBuf)
{
	AttributeValue_t *pAttrValueList[10];
	int i, iAttrValueListCount;

	iAttrValueListCount = GetX501CommonNameFromName_t(pAttrValueList, &cert->tbsCertificate.subject);

	if (0 == iAttrValueListCount)
	{
		return FALSE;
	}

	for (i = 0; i < iAttrValueListCount; i++)
	{
		// index 0 = T / 1 = L / 2~17 Value (16 digits)
		if (pAttrValueList[i]->size != 18)
		{
			return FALSE;
		}
	}

	// spec 상의 device Id 담는 변수가 8byte(64bit) 임
	// 따라서 이에 맞게 담아주어야 함
	// ex) A1 27 8D 23 02 6F 8B E7 이 대로 charBuf에 담아라
	// '1' -> 49 / 'A' -> 65

	// upperValue -> << 4 | downValue
	for (i = 0; i < 16; i+=2)
	{
		int iUpperValue, iDownValue;

		iUpperValue = ctoi(pAttrValueList[0]->buf[2+i]);
		iDownValue	= ctoi(pAttrValueList[0]->buf[3+i]);
		charBuf[i/2] = iUpperValue << 4 | iDownValue;
	}
	return TRUE;
}

BOOL GetDeviceIDAsAsciiCode(Certificate_t *cert, unsigned char* charBuf)
{
	AttributeValue_t *pAttrValueList[10];
	int i, iAttrValueListCount;

	iAttrValueListCount = GetX501CommonNameFromName_t(pAttrValueList, &cert->tbsCertificate.subject);

	if (0 == iAttrValueListCount)
	{
		return FALSE;
	}
	for (i = 0; i < iAttrValueListCount; i++)
	{
		// index 0 = T / 1 = L / 2~17 Value (16 digits)
		if (pAttrValueList[i]->size != 18)
		{
			return FALSE;
		}
	}

	memcpy(charBuf, &pAttrValueList[0]->buf[2], 16);
	charBuf[16] = '\0';

	return TRUE;
}

BOOL GetBrandID(Certificate_t *cert, unsigned short* pusCamBrandId)
{
	// The extension shall be optionally present in a CICAM device certificate only and shall not be declared as critical.
	BOOLEAN_t critical = 0;
	CicamBrandId_t *cicamBrandId = GetCicamBrandIdFromExtension(cert, &critical);
	BOOL bResult = TRUE;

	*pusCamBrandId = 0;

	if (cicamBrandId)
	{
		if (critical)
		{
			bResult = FALSE;
		}

		// CicamBrandId ::= INTEGER (1..65535)
		if (*cicamBrandId < 1 || 65535 < *cicamBrandId)
		{
			bResult = FALSE;
		}
		else
		{
			*pusCamBrandId = (unsigned short)*cicamBrandId;
		}

		ASN_STRUCT_FREE(asn_DEF_CicamBrandId, cicamBrandId);
	}

	return bResult;
}

BOOL GetScramblerCapabilities(Certificate_t *cert, int *pCapability)
{
	// it shall be present in each device certificate and marked as critical
	BOOLEAN_t critical = 0;
	ScramblerCapabilities_t *scramblerCapabilities = GetScramblerCapabilitiesFromExtension(cert, &critical);
	BOOL bResult = FALSE;

	if (scramblerCapabilities)
	{
		*pCapability = scramblerCapabilities->capability.buf[0];
		bResult = TRUE;
		ASN_STRUCT_FREE(asn_DEF_ScramblerCapabilities, scramblerCapabilities);
	}

	return bResult;
}

// 아래 두 함수가 위 검증 함수들을 차례로 호출하는 Wrapping 함수

int VerificateBrandCertificate(Certificate_t *brandCert, Certificate_t *rootCert)
{
	// 1) Check version (CI+ implementations shall use X.509 version 3 (02)).
	if (FALSE == IsVersion3_X509Certificate(brandCert))
	{
		return -1;
	}

    // 2) Check that the Issuer of the brand certificate is identical to the Subject of the root certificate.
	if (FALSE == IsNameMatchBetweenIssuerAndSubjectCertificates(brandCert, rootCert))
	{
		return -2;
	}

	// 3) Check that the validity period of the brand certificate includes the current date and time.
	if (FALSE == IsCertificatePeriodValid(brandCert))
	{
		return -3; // -3 : 이 값 절대 바꾸지 말것 !!!
	}

	// 4) Check that each mandatory extension listed in section 9.3.9 exists and the values are valid.
	//       Check that no other extension is marked as critical.
	if (FALSE == IsMandatoryExtensionCorrectlyListedOfBrandCertificate(brandCert))
	{
		return -4;
	}

	// 5) Verify that the KeyIdentifier in the brand certificate's authority key identifier extension
	//   is identical to the KeyIdentifier in the root certificate's subject key identifier extension.
	if (FALSE == IsKeyIdentifierMatchBetweenIssuerAndSubjectCertificates(brandCert, rootCert))
	{
		return -5;
	}

	// 6) Verify the certificate's signature by using the RSASSA-PSS verification - see IsSignatureValid()

	return 0;
}

int VerificateDeviceCertificate(Certificate_t *deviceCert, Certificate_t *brandCert)
{
	// 1) Check version (CI+ implementations shall use X.509 version 3 (02)).
	if (FALSE == IsVersion3_X509Certificate(deviceCert))
	{
		return -1;
	}

    // 2) Check that the Issuer of the device certificate is identical to the Subject of the brand certificate.
	if (FALSE == IsNameMatchBetweenIssuerAndSubjectCertificates(deviceCert, brandCert))
	{
		return -2;
	}

	// 3) Check that the validity period of the device certificate includes the current date and time.
	if (FALSE == IsCertificatePeriodValid(deviceCert))
	{
		return -3; // -3 : 이 값 절대 바꾸지 말것 !!!
	}

	// 4) Check that each mandatory extension listed in CI+ spec section 9.3.9 exists and the values are valid.
	//    Check that no other extension is marked as critical.
	if (FALSE == IsMandatoryExtensionCorrectlyListedOfDeviceCertificate(deviceCert))
	{
		return -4;
	}

	// 5) Verify that the KeyIdentifier in the device certificate's authority key identifier extension
	//  is identical to the KeyIdentifier in the brand certificate's subject key identifier extension.
	if (FALSE == IsKeyIdentifierMatchBetweenIssuerAndSubjectCertificates(deviceCert, brandCert))
	{
		return -5;
	}

	// 6) Verify the certificate's signature by using the RSASSA-PSS verification - see IsSignatureValid()

	// 7) Verify that the device ID (which is part of the Subject field) contains a valid value. See CI+ spec Annex B and C.
	if (FALSE == IsDeviceIdValueValid(deviceCert))
	{
		return -7;
	}

	return 0;
}

///////// end --> verification codes ////////////////


/***************************************************************
* public function bodies ///////////////////////////////////////
***************************************************************/

Certificate_t* X509_ParseRawCert(unsigned char *pucRawCert, unsigned short usRawCertLen) // cert는 내부적으로 memory alloc 되므로 caller는 사용 후 반드시 이를 X509_FreeCert() 해주어야 한다!
{
	Certificate_t *cert = 0;	// Type to decode
	asn_dec_rval_t rval;		// Decoder return value

	if (pucRawCert == NULL || usRawCertLen == 0)
	{
		PrintErr(("[CICC]---> X509_ParseRawCert()---> Error : Invalid params !!!\n"));
		return NULL;
	}

	// Decode the input buffer as Rectangle type
	rval = ber_decode(0, &asn_DEF_Certificate, (void **)&cert, pucRawCert, (size_t)usRawCertLen);
	if(rval.code != RC_OK || rval.consumed != usRawCertLen)
	{
		PrintErr(("[CICC]---> X509_ParseRawCert()---> Error (%d) : Broken encoding at byte %ld !!!\n", rval.code, (long)rval.consumed));
		return NULL;
	}

#ifdef CHECK_X509_ParseRawCert
{
	TBSCertificate_t	tCert = cert->tbsCertificate;
	AttributeValue_t	*pAttrValueList[10];
	int					iAttrValueListCount;
	BOOLEAN_t			critical = 0;
	unsigned long		arcs[10];
	int					arcsLen;
	int					i;

	// tbsCertificate
	PrintString(("\n< TBSCertificate >\n\n"));

	PrintValue("  Version :", tCert.version->buf, tCert.version->size);
	PrintValue("  Serial Number :", tCert.serialNumber.buf, tCert.serialNumber.size);

	PrintString(("\n  Signature \n\n"));
	if (tCert.signature.algorithm.buf)
	{
		arcsLen = GetArcsFromOID(&(tCert.signature.algorithm), arcs, 10);
		PrintArcs("    algorithm : ", arcs, arcsLen);
	}
	if (tCert.signature.parameters)
	{
		PrintHex("    parameters :", tCert.signature.parameters->buf, tCert.signature.parameters->size);
	}

	PrintString(("\n  Issuer \n\n"));
	iAttrValueListCount = GetX501CountryFromName_t(pAttrValueList, &tCert.issuer);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    C : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501StateFromName_t(pAttrValueList, &tCert.issuer);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    ST : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501LocationName_t(pAttrValueList, &tCert.issuer);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    L : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501OrganizationNameFromName_t(pAttrValueList, &tCert.issuer);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    O : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501OrganizationalUnitNameFromName_t(pAttrValueList, &tCert.issuer);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    OU : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501CommonNameFromName_t(pAttrValueList, &tCert.issuer);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    CN : (%s)\n", &pAttrValueList[i]->buf[2]));
	}

	PrintString(("\n  Validity \n\n"));
	PrintString(("    not before : %s (size = %d)\n", tCert.validity.notBefore.choice.utcTime.buf, tCert.validity.notBefore.choice.utcTime.size));
	PrintString(("    not after  : %s (size = %d)\n", tCert.validity.notAfter.choice.utcTime.buf, tCert.validity.notAfter.choice.utcTime.size));

	PrintString(("\n  Subject \n\n"));
	iAttrValueListCount = GetX501CountryFromName_t(pAttrValueList, &tCert.subject);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    C : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501StateFromName_t(pAttrValueList, &tCert.subject);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    ST : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501LocationName_t(pAttrValueList, &tCert.subject);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    L : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501OrganizationNameFromName_t(pAttrValueList, &tCert.subject);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    O : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501OrganizationalUnitNameFromName_t(pAttrValueList, &tCert.subject);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    OU : (%s)\n", &pAttrValueList[i]->buf[2]));
	}
	iAttrValueListCount = GetX501CommonNameFromName_t(pAttrValueList, &tCert.subject);
	for (i = 0; i < iAttrValueListCount; i++)
	{
		PrintString(("    CN : (%s)\n", &pAttrValueList[i]->buf[2]));
	}

	PrintString(("\n  SubjectPublicKeyInfo \n\n"));
	arcsLen = GetArcsFromOID(&(tCert.subjectPublicKeyInfo.algorithm.algorithm), arcs, 10);
	PrintArcs("    algorithm : ", arcs, arcsLen);
	PrintHex("    parameters :", tCert.subjectPublicKeyInfo.algorithm.parameters->buf, tCert.subjectPublicKeyInfo.algorithm.parameters->size);
	PrintHex("    public key :", tCert.subjectPublicKeyInfo.subjectPublicKey.buf, tCert.subjectPublicKeyInfo.subjectPublicKey.size);
	PrintString(("    unused bit : %d\n", tCert.subjectPublicKeyInfo.subjectPublicKey.bits_unused));

	// decode the RSAPublicKey
	{
		unsigned long RSAPublicKeyOID[] = {1, 2, 840, 113549, 1, 1, 1};
		BOOL bIsRSAPublicKey = IsSameOID(RSAPublicKeyOID, sizeof(RSAPublicKeyOID)/sizeof(RSAPublicKeyOID[0]), arcs, arcsLen);

		if (bIsRSAPublicKey)
		{
			RSAPublicKey_t *rsaPublicKey = 0;
			asn_dec_rval_t rval; // Decoder return value //

			rval = ber_decode(0, &asn_DEF_RSAPublicKey,
				(void **)&rsaPublicKey
				, tCert.subjectPublicKeyInfo.subjectPublicKey.buf
				, tCert.subjectPublicKeyInfo.subjectPublicKey.size);
			if(rval.code != RC_OK || rval.consumed != tCert.subjectPublicKeyInfo.subjectPublicKey.size)
			{
				rsaPublicKey = 0;
			}
			else
			{
				PrintHex("    RSA modulus (n) :", rsaPublicKey->modulus.buf, rsaPublicKey->modulus.size);
				PrintHex("    RSA exponent (e) :", rsaPublicKey->publicExponent.buf, rsaPublicKey->publicExponent.size);
				ASN_STRUCT_FREE(asn_DEF_RSAPublicKey, rsaPublicKey);
			}
		}
	}

	// CI+ extensions
	PrintString(("\n  extensions \n\n"));
	{
		SubjectKeyIdentifier_t *subKeyIdentifier = GetSubjectKeyIdentifierFromExtension(cert, &critical);
		if (subKeyIdentifier)
		{
			PrintValue("    Subject Key Identifier :", subKeyIdentifier->buf, subKeyIdentifier->size);
			if (critical)
			{
				PrintString(("    Critical : %d\n", critical));
			}
			ASN_STRUCT_FREE(asn_DEF_SubjectKeyIdentifier, subKeyIdentifier);
		}
	}
	{
		AuthorityKeyIdentifier_t *authorityKeyIdentifier = GetAuthorityKeyIdentifierFromExtension(cert, &critical);
		if (authorityKeyIdentifier)
		{
			PrintValue("    Authority Key Identifier :", authorityKeyIdentifier->keyIdentifier->buf, authorityKeyIdentifier->keyIdentifier->size);
			if (critical)
			{
				PrintString(("    Critical : %d\n", critical));
			}
#ifdef UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
			FREEMEM(authorityKeyIdentifier->keyIdentifier->buf);
			FREEMEM(authorityKeyIdentifier->keyIdentifier);
			FREEMEM(authorityKeyIdentifier);
#else
			ASN_STRUCT_FREE(asn_DEF_AuthorityKeyIdentifier, authorityKeyIdentifier);
#endif // UNEXPECTED_TAG_ACCEPT_FOR_AUTHORITY_KEY_IDENTIFIER
		}
	}
	{
		KeyUsage_t *keyUsage = GetKeyUsageFromExtension(cert, &critical);
		if (keyUsage)
		{
			PrintValue("    Key usage :", keyUsage->buf, keyUsage->size);
			PrintString(("    unused bit : %d\n", keyUsage->bits_unused));
			if (critical)
			{
				PrintString(("    Critical : %d\n", critical));
			}
			ASN_STRUCT_FREE(asn_DEF_KeyUsage, keyUsage);
		}
	}
	{
		BasicConstraints_t *basicConstraints = GetBasicConstraintsFromExtension(cert, &critical);
		if (basicConstraints)
		{
			PrintString(("    Basic constraints \n"));
			if (basicConstraints->cA)
			{
				PrintString(("        cA : %d\n", *(basicConstraints->cA)));
			}
			if (basicConstraints->pathLenConstraint)
			{
				PrintString(("        pathLenConstraint : %d\n", basicConstraints->pathLenConstraint->buf[0]));
			}
			if (critical)
			{
				PrintString(("        Critical : %d\n", critical));
			}
			ASN_STRUCT_FREE(asn_DEF_BasicConstraints, basicConstraints);
		}
	}
	{
		ScramblerCapabilities_t *scramblerCapabilities = GetScramblerCapabilitiesFromExtension(cert, &critical);
		if (scramblerCapabilities)
		{
			PrintString(("    Scrambler capabilities : (%d) capability, (%d) version\n", scramblerCapabilities->capability.buf[0], scramblerCapabilities->version.buf[0]));
			if (critical)
			{
				PrintString(("    Critical : %d\n", critical));
			}
			ASN_STRUCT_FREE(asn_DEF_ScramblerCapabilities, scramblerCapabilities);
		}
	}
	{
		CiplusInfo_t *ciplusInfo = GetCIplusInfoFromExtension(cert, &critical);
		if (ciplusInfo)
		{
			PrintValue("    CI+ Info :\n", ciplusInfo->buf, ciplusInfo->size);
			PrintString(("    unused bit : %d\n", ciplusInfo->bits_unused));
			if (critical)
			{
				PrintString(("    Critical : %d\n", critical));
			}
			ASN_STRUCT_FREE(asn_DEF_CiplusInfo, ciplusInfo);
		}
	}

	{
		CicamBrandId_t *cicamBrandId = GetCicamBrandIdFromExtension(cert, &critical);
		if (cicamBrandId)
		{
			PrintString(("    CI Cam Brand ID : (%ld) \n", *cicamBrandId));
			if (critical)
			{
				PrintString(("    Critical : %d\n", critical));
			}
			ASN_STRUCT_FREE(asn_DEF_CicamBrandId, cicamBrandId);
		}
	}

	// signature
	PrintString(("\n< Signature >\n\n"));
	arcsLen = GetArcsFromOID(&(cert->signatureAlgorithm.algorithm), arcs, 10);
	PrintArcs("    algorithm : ", arcs, arcsLen);
	if (cert->signatureAlgorithm.parameters)
	{
		PrintHex("    parameters :", cert->signatureAlgorithm.parameters->buf, cert->signatureAlgorithm.parameters->size);
	}
	PrintHex("    signature :", cert->signature.buf, cert->signature.size);
	PrintString(("    unused bit : %d\n", cert->signature.bits_unused));

	PrintString(("\n< End >\n\n"));
}
#endif

	return cert;
}

