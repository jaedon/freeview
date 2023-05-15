
/* SVC 공통 헤더 include */
#include <svc_common.h>


/*
	모듈 plug-in 헤더만 include 함.
	모듈 plug-in 헤더는 makefile에 path 설정이 안되어 있으므로 상대 path로 include 해야 함.
*/
#include "psvc_xxx.h"


/*
	파생 모듈 plug-in body 헤더 include 함.(파생 모듈 내부 공유 헤더)
	파생 모듈 plug-in 헤더는 makefile에 path 설정이 안되어 있으므로 상대 path로 include 해야 함.
*/
#if defined(CONFIG_OP_111)
	#include "./xxx/include/_xsvc_xxx_111.h"
#elif defined(CONFIG_OP_2222)
	#include "./xxx/include/_xsvc_xxx_222.h"
#elif defined(CONFIG_PROD_3)
	#include "./xxx/include/_xsvc_xxx_333.h"
#else
	#error "CONFIG_OP_XXX or CONFIG_PROD_XXX should be defined."
#endif


/* 모듈 typedef

	Svc : 대문자로 시작하는 layer 명	(Psvc : X, Svc : O)
	Xxx : 대문자로 시작하는 Module 명
	Type : 대문자로 시작하는 type 명. Common convention 을 따른다.
	_t/e/b : type 종류. Common convention을 따른다.
*/
typedef int SvcXxx_Type_t; /* SvcXxx_Type_e, SvcXxx_Type_b */
typedef (void *)SvcXxx_DoSomethingCb_t(void);


/* 모듈 plug-in API

	psvc_ : Plug-in 표시 p + 소문자로 layer 명 표시
	xxx_ : 소문자로 module 명 표시
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임
	
	설명 :
		SVC 모듈 내부에서 파생에 따른 option이 존재할 경우 호출하는 API.
		Proto type은 base의 모듈 internal include에 선언되고 body는 파생 모듈에서 구현됨.

	주의사항 :
		반드시 base 모듈에서만 호출되어야 함.
*/
HERROR psvc_xxx_DoSomething( ... )
{
#if defined(CONFIG_OP_111)
	xsvc_xxx_DoSomething_111();
#elif defined(CONFIG_OP_222)
	xsvc_xxx_DoSomething_222();
#elif defined(CONFIG_OP_333)
	xsvc_xxx_DoSomething_333();
#else
	#error "CONFIG_OP_XXX or CONFIG_PROD_XXX should be defined."
#endif
}





