#ifndef __STR_GEN_H__
#define __STR_GEN_H__



#define	ImgGenAssert(expr)	\
	if (!(expr)) {\
		printf("\n\n\n-----------------------------------\n");\
		printf("%s:%d:%s: Assertion \'%s\' failed!!\n", __FILE__, __LINE__, __FUNCTION__, #expr);\
		printf("You will die, i will call abort()!!!!!!!!!!\n");\
		printf("-----------------------------------\n\n\n");\
		abort();\
		exit(-1);\
	}



#endif /* __STR_GEN_H__ */