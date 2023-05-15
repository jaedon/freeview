/* Define 선언

	대문자열을 _ 로 이어 표기합니다.
*/
#define	THIS_IS_DEFINITION	0



/* 휴맥스 헝가리안 표기법

	좀 오버스럽긴 합니다만 여러 사람이 쓰는 코드에서 빠른 분석 등의 개발 효율을 위해 되도록 지켜주시길 바랍니다.
	VariableName 부분이 변수명이고 대문자로 시작하는 단어를 이어 붙입니다.
	그 앞에 변수 type에 따라 휴맥스에서 전통적으로 사용하는 형가리안 표기법을 소문자로 붙입니다.

HINT8		cVariableName;
HINT8		*pcVariableName;

HUINT8		ucVariableName;
HUINT8		*pucVariableName;

HINT16		sVariableName;
HINT16		*psVariableName;

HUINT16 	usVariableName;
HUINT16 	*pusVariableName;

HINT32		nVariableName;
HINT32		*pnVariableName;

HUINT32 	ulVariableName;
HUINT32 	*pulVariableName;

HINT64		llVariableName;
HINT64		*pllVariableName;

HUINT64 	ullVariableName;
HUINT64 	*pullVariableName;

void		*pvVariableName;

struct
{
	....

} stStructName;

struct
{
	....

} *pstStructName;

union
{
	....

} utUnionName;

union
{
	....

} *putUnionName;

*/




/* 변수 type 선언

	대문자로 시작하는 단어를 이어 붙인 뒤에 _t 를 붙입니다.
*/
typedef int	NewType_t;




/* struct type 선언

	대문자로 시작하는 단어를 이어 붙인 뒤에 _t 를 붙입니다.
*/
typedef struct
{
	/*
		Member 변수들은 local 변수 convention을 따릅니다.
	*/
	HINT8		cMemberVariable;
	HINT8		*pcMemberVariable;
	/* 이하 생략 */

	....

} StructType_t;




/* enum type 선언

	Type 명은 대문자로 시작하는 단어들을 이어 붙인 뒤에 _e 를 붙입니다.
*/
typedef enum
{
	/* 	Enumerate 들은 e로 시작하는 대문자열을 _ 로 이어 표기합니다. */
	eENUM_NAME_0,
	eENUM_NAME_1,
	eENUM_NAME_2,

		....

} EnumType_e;




/* Bitwise type 선언

	typedef 를 이용하기 위하여 enum type을 응용합니다.
	Type 명은 대문자로 시작하는 단어들을 이어 붙인 뒤에 _b 를 붙입니다.
*/
typedef enum
{
	/* 	Enumerate 들은 e로 시작하는 대문자열을 _ 로 이어 표기합니다. */
	eBIT_NAME_0 = 0x0,
	eBIT_NAME_1 = 0x1,
	eBIT_NAME_2 = 0x2,
	eBIT_NAME_3 = 0x4,
	eBIT_NAME_4 = 0x8,

		....

} BitwiseType_b;

/* BitwiseType_b 으로 선언한 변수는 HUINT32 이어야 합니다. */
BitwiseType_b	bwBitwiseVariable;




/* Global 변수 선언.

	Global 변수는 형가리안 표기법 앞에 g_ 를 붙입니다.
*/
HINT8		g_cGlobalVariable;
HINT8		*g_pcGlobalVariable;
/* 이하 생략 */




/* Static 변수 선언

	Global 변수에서 g_ 를 s_ 로 교체하여 표기합니다.
*/
static HINT8		s_cStaticVariable;
static HINT8		*s_pcStaticVariable;
/* 이하 생략 */




HERROR function( /* Parameter는 local 변수와 동일 convention */ )
{
	/* Local 변수 선언

		g_, s_ 없이 헝가리안 표기법만 사용합니다.
	*/
	HINT8		cLocalVariable;
	HINT8		*pcLocalVariable;
	/* 이하 생략 */

	......
}



