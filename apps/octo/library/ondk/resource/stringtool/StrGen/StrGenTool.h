#ifndef __STR_GEN_H__
#define __STR_GEN_H__



#define	StrGenAssert(expr)	\
	if (!(expr)) {\
		printf("\n\n\n--------------------------------------------\n");\
		printf("file : %s!!\n", __FILE__);\
		printf("[%s:%d]  Assertion \'%s\' failed!!\n", __FUNCTION__, __LINE__, #expr);\
		printf("You will die, i will call abort()!!!!!!!!!!\n");\
		printf("---------------------------------------------------\n\n\n");\
		abort();\
		exit(-1);\
	}



#endif /* __STR_GEN_H__ */
