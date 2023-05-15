/* SVC 공통 헤더 include */
#include <svc_common.h>



/* 사용하는 SVC 모듈의 헤더 include */
#include <svc_111.h>
#include <svc_222.h>


/* MGR 공통 헤더 include */
#include <mgr_common.h>



/* 모듈 헤더

	xmgr_ : Extension을 타나내는 x + 소문자 layer명 표시
	xxx.h : 소문자 module명 표시. 1 depth만 허용.

	설명 :
		MGR 모듈당 1개의 헤더를 제공한다.

	주의 사항 :
		모듈 헤더에는 모듈 API proto type만 있어야 함.
		모듈 헤더는 makefile에서 path 설정이 되어 있으므로 아래와 같이 include 가능함.
*/
#include <xmgr_xxx.h>



/* 모듈 내부 공유 헤더

	_mgr_ : Internal 표시 _ + extension x 표시 + 소문자 layer명 표시
	xxx_ : 소문자 module명 표시
	yyy.h : 소문자 sub module 표시

	설명 :
		MGR 모듈 내부에서 공유할 type 와 function 선언.

	주의 사항 :
		모듈 내부 공유 헤더는 makefile에서 path 설정이 되어 있지 않음.
		모듈 내부에 include directory를 만들어 위치하고 모듈 위치로부터의 상태 path로 include 함.
*/
#include "./include/_xmgr_xxx_yyy.h"


/* 모듈 typedef

	Xmgr : 대문자로 시작하는 layer 명
	Xxx : 대문자로 시작하는 Module 명
	Type : 대문자로 시작하는 type 명. Common convention 을 따른다.
	_t/e/b : type 종류. Common convention을 따른다.
*/
typedef int XmgrXxx_Type_t; /* XmgrXxx_Type_e, XmgrXxx_Type_b */
typedef (void *)XmgrXxx_DoSomethingCb_t(void);



/* 모듈 파일의 local typedef

	xmgr : 소문자로 시작하는 layer 명
	Xxx : 대문자로 시작하는 Module 명
	Type : 대문자로 시작하는 type 명. Common convention 을 따른다.
	_t/e/b : type 종류. Common convention을 따른다.
*/
typedef int xmgrXxx_Type_t; /* xmgrXxx_Type_e, xmgrXxx_Type_b */
typedef (void *)xmgrXxx_DoSomethingCb_t(void);


/* 모듈 파일의 local function

	xmgr_ : Extension을 표시하는 x + 소문자로 layer 명 표시
	xxx_ : 소문자로 module 명 표시
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임
	ZZZ_ : 대문자로 파생명 표시
	
	설명 :
		MGR 모듈 안에서도 한 파일 안에서만 사용하기 위한 function으로 파일 내부 호출 용으로 사용.

	주의사항 :
		앞에 반드시 static 또는 STATIC을 붙여야 함.
		모듈 세마포를 local function 에서 사용하면 안됨.
		local function은 recursive 호출이 될 가능성이 있으므로 세마포를 사용하면 dead lock 됨.
*/
STATIC HERROR xmgr_xxx_DoSomething_ZZZ( ... )
{

	....

}



/* 모듈 내부의 global function

	xmgr_ : Extension을 표시하는 x + 소문자로 layer 명 표시
	xxx_ : 소문자로 module 명 표시(모듈 local function과 같음)
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임(모듈 local function과 같음)
	ZZZ_ : 대문자로 파생명 표시
	
	설명 :
		MGR 모듈이 여러 file로 구성될 경우 파일간에 공유하기 위한 global function으로 모듈 내부 파일간 호출 용으로 사용.
		Coding convention은 모듈 파일의 local function과 같고 앞에 static 선언만 없다.

	주의사항 :
		모듈 세마포를 내부 global function 에서 사용하면 안됨.
		내부 global function은 recursive 호출이 될 가능성이 있으므로 세마포를 사용하면 dead lock 됨.
*/
HERROR xmgr_xxx_DoSomething_ZZZ( ... )
{

	....

}




/* 모듈 API

	XMGR_ : Extension을 표시하는 X + 대문자로 layer 명 표시
	XXX_ : 대문자로 module 명 표시
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임
	ZZZ_ : 대문자로 파생명 표시
	
	주의사항 :
*/
HERROR XMGR_XXX_DoSomething_ZZZ( ... )
{
	xmgr_xxx_zzz_DoSomething();

	....

}



/* 모듈 Procedure

	xproc_ : Extension을 표시하는 x + Manager bus 에 plug-in 되는 procedure 임을 나타냄
	xxx_ : 소문자로 module 명 표시
	Function Name : 대문자로 시작하는 단어를 이어 붙임
	ZZZ_ : 대문자로 파생명 표시

	주의 사항 :
		자기 모듈의 base와 파생 모듈 API를 호출 할 수 있다.
		다른 MGR 모듈의 base와 파생 모듈 API를 호출 할 수 있다.
		SVC 모듈 API를 호출 할 수 있다.

		!!! plug-in API 호출 불가 !!!
		!!! PAL 모듈 API는 호출 불가. !!!
*/
BUS_Result_t xproc_xxx_ProcedureName_ZZZ(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	mgr_xxx_DoSomething();
	xmgr_xxx_DoSomething();

	MGR_XXX_DoSomething();
	MGR_YYY_DoSomething();

	XMGR_XXX_DoSomething();
	XMGR_YYY_DoSomething();

	SVC_111_DoSomething();
	SVC_222_DoSomething();

	XSVC_111_XXX_DoSomething();
	XSVC_222_XXX_DoSomething();

	....

}


