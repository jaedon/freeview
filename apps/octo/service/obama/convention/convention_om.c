#include <oapi.h>


/* 모듈 typedef

	Type : 대문자로 시작하는 type 명. Common convention 을 따른다.
	_t/e/b : type 종류. Common convention을 따른다.
*/

/*	Message
	MSG_OM_XXX_YYY
*/
enum
{
	MSG_OM_XXX_YYY_DOSOMETHING,
	MSG_OM_NOTIFY_XXX_YYY_DOSOMETHING,
}


/* 모듈 typedef
	N/A
*/


/* 모듈 파일의 local typedef

	om : 소문자로 시작하는 layer 명
	Xxx : 대문자로 시작하는 Module 명
	Type : 대문자로 시작하는 type 명. Common convention 을 따른다.
	_t/e/b : type 종류. Common convention을 따른다.
*/
typedef int omXxx_Type_t; /* omXxx_Type_e, omXxx_Type_b */
typedef (void *)omXxx_DoSomethingCb_t(void);


/* 모듈 파일의 local function

	om_ : 소문자로 oapi manager layer 명 표시
	xxx_ : 소문자로 category 명 표시
	yyy_ : 소문자로 module 명 표시
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임
	
	설명 :
		OAPI manager 모듈 안에서도 한 파일 안에서만 사용하기 위한 function으로 파일 내부 호출 용으로 사용.

	주의사항 :
		앞에 반드시 static 또는 STATIC을 붙여야 함.
		모듈 세마포를 local function 에서 사용하면 안됨.
		local function은 recursive 호출이 될 가능성이 있으므로 세마포를 사용하면 dead lock 됨.
*/
STATIC HERROR om_xxx_yyy_DoSomething( ... )
{

	....

}



/* 모듈 내부의 global function

	om_ : 소문자로 OAPI manager layer 명 표시(모듈 local function과 같음)
	xxx_ : 소문자로 category 명 표시(모듈 local function과 같음)
	yyy_ : 소문자로 module 명 표시(모듈 local function과 같음)
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임(모듈 local function과 같음)
	
	설명 :
		OAPI manager 모듈이 여러 file로 구성될 경우 파일간에 공유하기 위한 global function으로 모듈 내부 파일간 호출 용으로 사용.
		Coding convention은 모듈 파일의 local function과 같고 앞에 static 선언만 없다.

	주의사항 :
		모듈 세마포를 내부 global function 에서 사용하면 안됨.
		내부 global function은 recursive 호출이 될 가능성이 있으므로 세마포를 사용하면 dead lock 됨.
*/
HERROR om_xxx_yyy_DoSomething( ... )
{

	....

}




/* 모듈 API

	OM_ : 대문자로 OAPI manager layer 명 표시
	XXX_ : 대문자로 category 명 표시
	YYY_ : 대문자로 module 명 표시
	Function Name : 영문법을 지켜 동사 + 목적어 형식으로 대문자로 시작하는 단어를 이어 붙임
	
	주의사항 :
*/
HERROR OM_XXX_YYY_DoSomething( ... )
{
	om_xxx_yyy_DoSomething();

	....

}

/* RPC API string define
	RPC_ : RPC 용 string define 표시
	OAPI_XXX_YYY_DoSomething : 모듈 API와 동일
	"xxx.yyy.DoSomething" : JAVA style로 xxx.yyy 에 function 명 동일하게 붙임.
*/
#define RPC_OAPI_XXX_YYY_DoSomething	"xxx.yyy.DoSomething"

/* RPC notify string define
	RPC_ : RPC 용 string define 표시
	OBAMA_XXX_YYY_onSomeEvent : Event handler functin와 동일
	"xxx.yyy.onSomeEvent" : JAVA style로 xxx.yyy 에 event notify function 명 동일하게 붙임.
*/
#define RPC_OM_OUTPUT_VIDEO_onSomeEvent		"output.video.onSomeEvent"


