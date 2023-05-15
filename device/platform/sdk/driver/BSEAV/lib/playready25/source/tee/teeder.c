#include <stdlib.h>
#include <teeder.h>

/*
 * calculate the encoding length of DER header for a single field of a given length
 */
int DER_GetHeaderLen(int inlen)
{
    int len = 1; // DER data type byte
    if (inlen <= 0x7f) {
        len++;
    } else if (inlen <= 0xff) {
        len += 2;
    } else if (inlen <= 0x7fff) {
        len += 3;
    } else { // bigger size not supported
        return -1;
    }
    return len;

}

unsigned char * DER_WriteHeader(unsigned char * p, int type, int inlen) {
    *p = (unsigned char)type;
    ++p;

    if (inlen <= 0x7f) {
        *p = (unsigned char)inlen; ++p;
    } else if (inlen <= 0xff) {
        *p = 0x81; ++p;
        *p = (unsigned char)(inlen & 0xff); ++p;
    } else if (inlen <= 0x7fff) {
        *p = 0x82; ++p;
        *p = (unsigned char)((inlen >> 8) & 0xff); ++p;
        *p = (unsigned char)(inlen & 0xff); ++p;
    } else { // bigger size not supported
        return NULL;
    }

    return p;
}

