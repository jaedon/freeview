
/* SVC 공통 헤더

	svc_ : 소문자 layer명 표시
	common.h : common 파일 표시.

	설명 :
		OBAMA 내의 모든 layer 모듈이 공통으로 사용하는 TV service layer 헤더.
		OCTO 1.0 시절의 stb_param.h 와 같은 역할.
		주로 OAPI에 정의되지 않은 obama 내부 type이나 struct를 정의
		tvservice/include 에 위치하고 makefile path 설정이 되어 있음.

	주의 사항 :
		TV manager 의 헤더는 include 할 수 없음.
*/
#include <svc_common.h>



/* 사용하는 PAL 모듈의 헤더 include */
#include <pal_111.h>
#include <pal_222.h>



/* 모듈 헤더

	svc_ : 소문자 layer명 표시
	xxx.h : 소문자 module명 표시. 1 depth만 허용.

	설명 :
		SVC 모듈당 1개의 헤더를 제공한다.

	주의 사항 :
		모듈 헤더에는 모듈 API proto type만 있어야 함.
		모듈 헤더는 makefile에서 path 설정이 되어 있으므로 아래와 같이 include 가능함.
*/
#include <svc_xxx.h>




/* 모듈 내부 공유 헤더

	_svc_ : Internal 표시 _ + 소문자 layer명 표시
	xxx_ : 소문자 module명 표시
	yyy.h : 소문자 sub module 표시

	설명 :
		SVC 모듈 내부에서 공유할 type 와 function 선언.

	주의 사항 :
		모듈 내부 공유 헤더는 makefile에서 path 설정이 되어 있지 않음.
		모듈 내부에 include directory를 만들어 위치하고 모듈 위치로부터의 상태 path로 include 함.
*/
#include "_svc_xxx_yyy.h"




/* 모듈 plug-in 헤더

	psvc_ : Plug-in 표시 p + 소문자 layer명 표시
	xxx.h : 소문자 module명 표시

	설명 :
		TV service 모듈의 파생 API 헤더.

	주의 사항 :
		모듈 plug-in 헤더는 makefile에서 path 설정이 되어 있지 않음.
		모듈 내부에 include directory를 만들어 모듈 위치로부터의 상태 path로 include 함.
*/
#include "./include/psvc_xxx.h"



/* 모듈 typedef

	Svc : 대문자로 시작하는 layer 명
	Xxx : 대문자로 시작하는 Module 명
	Type : 대문자로 시작하는 type 명. Common convention 을 따른다.
	_t/e/b : type 종류. Common convention을 따른다.
*/
typedef int SvcXxx_Type_t; /* SvcXxx_Type_e, SvcXxx_Type_b */
typedef (void *)SvcXxx_DoSomethingCb_t(void);




/* API 또는 context에 접근하는 세마포가 필요할 경우 사용 */
STATIC HULONG	s_ulModuleSema;



/* 모듈 파일의 local function

	svc_ : 소문자로 layer 명 표시
	xxx_ : 소문자로 module 명 표시
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임
	
	설명 :
		SVC 모듈 안에서도 한 파일 안에서만 사용하기 위한 function으로 파일 내부 호출 용으로 사용.

	주의사항 :
		앞에 반드시 static 또는 STATIC을 붙여야 함.
		모듈 세마포를 local function 에서 사용하면 안됨.
		local function은 recursive 호출이 될 가능성이 있으므로 세마포를 사용하면 dead lock 됨.
*/
STATIC HERROR svc_xxx_DoSomething( ... )
{

	....

}


/* 모듈 파일의 local typedef

	svc : 소문자로 시작하는 layer 명
	Xxx : 대문자로 시작하는 Module 명
	Type : 대문자로 시작하는 type 명. Common convention 을 따른다.
	_t/e/b : type 종류. Common convention을 따른다.
*/
typedef int svcXxx_Type_t; /* svcXxx_Type_e, svcXxx_Type_b */
typedef (void *)svcXxx_DoSomethingCb_t(void);


/* 모듈 내부의 global function

	svc_ : 소문자로 layer 명 표시(모듈 local function과 같음)
	xxx_ : 소문자로 module 명 표시(모듈 local function과 같음)
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임(모듈 local function과 같음)
	
	설명 :
		SVC 모듈이 여러 file로 구성될 경우 파일간에 공유하기 위한 global function으로 모듈 내부 파일간 호출 용으로 사용.
		Coding convention은 모듈 파일의 local function과 같고 앞에 static 선언만 없다.

	주의사항 :
		모듈 세마포를 내부 global function 에서 사용하면 안됨.
		내부 global function은 recursive 호출이 될 가능성이 있으므로 세마포를 사용하면 dead lock 됨.
*/
HERROR svc_xxx_DoSomething( ... )
{

	....

}




/* 모듈 API

	SVC_ : 대문자로 layer 명 표시
	XXX_ : 대문자로 module 명 표시
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임
	
	주의사항 :
		* 모듈 세마포는 모듈 API에서만 사용해야 함.
		즉, 모듈 외부에서 호출하는 API만 세마포 사용 가능함.
		만약 local function 이 그대로 모듈 API로 쓰일 수 있는 경우라고 해도 아래와 같이
		세마포 호출과 local function 호출을 조합하여 모듈 API를 구성함.

		* 모듈 API는 모듈 내부에서 호출 되어서는 안된다.
		세마포를 품고 있는 모둘 API가 내부 호출이 되어 recursive 호출이 걸리면 dead lock 됨.
*/
HERROR SVC_XXX_DoSomething( ... )
{
	VK_SEM_Get(s_ulModuleSema); /* 모듈 세마포는 모듈 API 에서만 사용해야 한다. */

	svc_xxx_DoSomething();
	psvc_xxx_DoSomething();

	PAL_111_DoSomething();
	PAL_222_DoSomething();

	....

	VK_SEM_Release(s_ulModuleSema);
}



