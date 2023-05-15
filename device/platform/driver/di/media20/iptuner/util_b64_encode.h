#ifndef __UTIL_B64_ENCODE__
#define __UTIL_B64_ENCODE__
char *UTIL_B64Encode( const char *src );
int base64_decode(char *enc,char *dec,int decsize);

#endif /* ! __UTIL_B64_ENCODE__ */
