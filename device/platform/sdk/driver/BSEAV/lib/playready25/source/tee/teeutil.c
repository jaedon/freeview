#include <ctype.h>
#include <memory.h>
#include <stdio.h>
#include <malloc.h>

#include <teecommon.h>
#include <teeder.h>
#include <teeimplinternal.h>

void * _tee_malloc(size_t size) {
    return malloc(size);
}

void _tee_free(void * ptr) {
    if (ptr != NULL) free(ptr);
}

void * _tee_memcpy (void * destination, const void * source, size_t num ) {
    return memcpy(destination, source, num);
}

void * _tee_memset ( void * ptr, int value, size_t num ) {
    return memset(ptr, value, num);
}

time_t _get_drm_time() {
    DRMSYSTEMTIME playreadyTime = { 0, 0, 0, 0, 0, 0, 0, 0 };
    Oem_Clock_GetSystemTime(NULL, &playreadyTime);
    struct tm timeinfo;
    timeinfo.tm_sec =   playreadyTime.wSecond;
    timeinfo.tm_min =   playreadyTime.wMinute;
    timeinfo.tm_hour =  playreadyTime.wHour;
    timeinfo.tm_mday =  playreadyTime.wDay;
    timeinfo.tm_mon =   playreadyTime.wMonth - 1;
    timeinfo.tm_year =  playreadyTime.wYear - 1900;
    return timegm(&timeinfo);
}

#define PRINT_HEADER char indent[80]; \
	snprintf(indent, 80, "****** %s: %s [%d]: ", func, header, inlen); \
	_print("%s", indent); \
	memset(indent, ' ', strlen(indent));

/*
 * print binary data as sequence of hex
 */
#ifdef TEE_DEBUG
int _print_hex_func(const char * const func, const char * const header, const unsigned char inbuf[], const int inlen) {
    PRINT_HEADER
    int i;
    for (i = 0; i < inlen; i++) {
        if ((i % 32) == 0 && i != 0) {
            _print("\n%s", indent);
        }
        _print("%02x ", inbuf[i]);
    }
    _print("\n");
    return TEE_SUCCESS;
}
#else
int _print_hex_func(const char * const func, const char * const header, const unsigned char inbuf[], const int inlen) {
	return TEE_SUCCESS;
}
#endif

int _get_hex(const unsigned char inbuf[], const int inlen, char outbuf[]) {
    int i;
    char * p = outbuf;
    for (i = 0; i < inlen; i++, p += 2) {
        sprintf(p, "%02x", inbuf[i]);
    }
    *p = '\0';
    return TEE_SUCCESS;
}

/*
 * print binary data as sequence of hex or printable characters
 */
#ifdef TEE_DEBUG
int _print_bin_func(const char * const func, const char * const header, const unsigned char * const inbuf, const int inlen) {
    PRINT_HEADER
    int i;
    for (i = 0; i < inlen; ++i) {
        if ((i % 32) == 0 && i != 0) {
            _print("\n%s", indent);
        }
        if (isprint(inbuf[i])) {
            _print(" %c ", inbuf[i]);
        } else {
            _print("%02x ", inbuf[i]);
        }
    }
    _print("\n");
    return TEE_SUCCESS;
}
#else
int _print_bin_func(const char * const func, const char * const header, const unsigned char * const inbuf, const int inlen) {
	return TEE_SUCCESS;
}
#endif

/*
 * print ascii data
 */
#ifdef TEE_DEBUG
int _print_txt_func(const char * const func, const char * const header, const unsigned char * const inbuf, const int inlen) {
    PRINT_HEADER
    int i;
    for (i = 0; i < inlen; ++i) {
        if ((i % 96) == 0 && i != 0) {
            _print("\n%s", indent);
        }
        _print("%c", inbuf[i]);
    }
    _print("\n");
    return TEE_SUCCESS;
}
#else
int _print_txt_func(const char * const func, const char * const header, const unsigned char * const inbuf, const int inlen) {
	return TEE_SUCCESS;
}
#endif
#undef PRINT_HEADER

/*
 * calculate the encoding length for a single DER field, including DER header
 */
int _der_get_field_enc_len(int const inlen)
{
    return DER_GetHeaderLen(inlen) + inlen;
}

/*
 * calculate the encoding length for a single integer secure stop field
 */
int _der_get_int_field_enc_len(long const value)
{
    return _der_get_field_enc_len(4); // assume for now that time stamp always takes 4 bytes
}

/*
 * write DER OCTET STRING
 */
unsigned char * _der_write_octets(unsigned char * p, unsigned char * inbuf, int inlen)
{
    p = DER_WriteHeader(p, DER_OCTETS, inlen);
    _tee_memcpy(p, inbuf, inlen);
    return p + inlen;
}

/*
 * write DER INTEGER. Remove the first bytes equal to 0x00 (at least 1 byte must be retained).
 */
unsigned char * _der_write_int(unsigned char * p, long val)
{
    // convert bytes to BIG ENDIAN, and point at most significant byte
    val = INT32(val);
    unsigned char *p1 = (unsigned char *)&val;

    // skip bytes == 0x00 and find length for serialization
    int i;
    for (i = 0; i < 3; i++) {
        if ( (p1[0] == 0x00) && ((p1[1] & 0x80) == 0x00) ) ++p1;
        else break;
    }

    int len = 4 - i;
    p = DER_WriteHeader(p, DER_INTEGER, len);
    _tee_memcpy(p, p1, len);
    return p + len;
}

