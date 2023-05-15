#include "bchp.h"
const uint32_t BDSP_IMG_scm1_digest_array1[] = {
	0xdc69bebc,
	0x43f7ecbb,
	0x77c76c03,
	0x8b814120,
	0xc1380393,
};
const uint32_t BDSP_IMG_scm1_digest_header [2] = {sizeof(BDSP_IMG_scm1_digest_array1), 1};
const void * const BDSP_IMG_scm1_digest [2] = {BDSP_IMG_scm1_digest_header, BDSP_IMG_scm1_digest_array1};
