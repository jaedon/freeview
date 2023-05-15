
#define	____OxEPGQUERY_README___________________

/******************************************************************************
 [Example Code]

int main (int argc, char *argv[])
{

	return 0;
}


 [Expected Result]


 ************************************************************************************/

#ifndef __OxEPGQUERY_H__
#define __OxEPGQUERY_H__


void					svc_epgquery_Traverse (const SvcEpg_Query_t *q, void (* each) (const SvcEpg_Query_t *q, void *), void *userdata);
SvcEpg_Query_t *		svc_epgquery_New (SvcEpg_Field_e field, SvcEpg_Comparison_e comparison, const HCHAR *value);
SvcEpg_Query_t *		svc_epgquery_NewWithInteger (SvcEpg_Field_e field, SvcEpg_Comparison_e comparison, HUINT32 value);
void					svc_epgquery_Delete (SvcEpg_Query_t *q);


#endif /* __OxEPGQUERY_H__ */


