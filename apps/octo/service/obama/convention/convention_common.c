/* Define ����

	�빮�ڿ��� _ �� �̾� ǥ���մϴ�.
*/
#define	THIS_IS_DEFINITION	0



/* �޸ƽ� �밡���� ǥ���

	�� ���������� �մϴٸ� ���� ����� ���� �ڵ忡�� ���� �м� ���� ���� ȿ���� ���� �ǵ��� �����ֽñ� �ٶ��ϴ�.
	VariableName �κ��� �������̰� �빮�ڷ� �����ϴ� �ܾ �̾� ���Դϴ�.
	�� �տ� ���� type�� ���� �޸ƽ����� ���������� ����ϴ� �������� ǥ����� �ҹ��ڷ� ���Դϴ�.

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




/* ���� type ����

	�빮�ڷ� �����ϴ� �ܾ �̾� ���� �ڿ� _t �� ���Դϴ�.
*/
typedef int	NewType_t;




/* struct type ����

	�빮�ڷ� �����ϴ� �ܾ �̾� ���� �ڿ� _t �� ���Դϴ�.
*/
typedef struct
{
	/*
		Member �������� local ���� convention�� �����ϴ�.
	*/
	HINT8		cMemberVariable;
	HINT8		*pcMemberVariable;
	/* ���� ���� */

	....

} StructType_t;




/* enum type ����

	Type ���� �빮�ڷ� �����ϴ� �ܾ���� �̾� ���� �ڿ� _e �� ���Դϴ�.
*/
typedef enum
{
	/* 	Enumerate ���� e�� �����ϴ� �빮�ڿ��� _ �� �̾� ǥ���մϴ�. */
	eENUM_NAME_0,
	eENUM_NAME_1,
	eENUM_NAME_2,

		....

} EnumType_e;




/* Bitwise type ����

	typedef �� �̿��ϱ� ���Ͽ� enum type�� �����մϴ�.
	Type ���� �빮�ڷ� �����ϴ� �ܾ���� �̾� ���� �ڿ� _b �� ���Դϴ�.
*/
typedef enum
{
	/* 	Enumerate ���� e�� �����ϴ� �빮�ڿ��� _ �� �̾� ǥ���մϴ�. */
	eBIT_NAME_0 = 0x0,
	eBIT_NAME_1 = 0x1,
	eBIT_NAME_2 = 0x2,
	eBIT_NAME_3 = 0x4,
	eBIT_NAME_4 = 0x8,

		....

} BitwiseType_b;

/* BitwiseType_b ���� ������ ������ HUINT32 �̾�� �մϴ�. */
BitwiseType_b	bwBitwiseVariable;




/* Global ���� ����.

	Global ������ �������� ǥ��� �տ� g_ �� ���Դϴ�.
*/
HINT8		g_cGlobalVariable;
HINT8		*g_pcGlobalVariable;
/* ���� ���� */




/* Static ���� ����

	Global �������� g_ �� s_ �� ��ü�Ͽ� ǥ���մϴ�.
*/
static HINT8		s_cStaticVariable;
static HINT8		*s_pcStaticVariable;
/* ���� ���� */




HERROR function( /* Parameter�� local ������ ���� convention */ )
{
	/* Local ���� ����

		g_, s_ ���� �밡���� ǥ����� ����մϴ�.
	*/
	HINT8		cLocalVariable;
	HINT8		*pcLocalVariable;
	/* ���� ���� */

	......
}



