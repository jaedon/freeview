/*
    COPYRIGHT VIACCESS S.A.
    date        : 06/02/2006
    file        : va_opentv_Otype1x.h
    Description : It is the Viaccess'API of CAS functionalities

    $Revision: 352 $
*/

#ifndef _VA_OPENTV_OTYPE1X_H_
#define _VA_OPENTV_OTYPE1X_H_


/*    !!!!!!!!! WARNING !!!!!!!!!!     */
/* include of opentv.h or opentvx.h must be done  before the include of va_otv_ui_type1x.h */
/*    !!!!!!!!! WARNING !!!!!!!!!!     */


#define VA_DEF_SURVEY_TIME_SPAN	                    7		/* default survey time span in days */

/******************************/
/*** Common data structures ***/
/******************************/
#define VA_FALSE                                    (0)		/* This constant must be '0' */
#define VA_TRUE                                     (1)		/* This constant must be '1' */
#define VA_FALSE_VALUE								0
#define VA_TRUE_VALUE								1

#define VA_POSITIVE_VALUE							0
#define VA_NEGATIVE_VALUE							1

#define VA_PIN_CODE_LENGTH							8		/* length of PIN for the user */

#define VA_SOID_LABEL_MAX_LENGTH                    16
#define VA_LOWER_DATE_YEAR_VALUE                    1980
#define VA_LOWER_DATE_MONTH_VALUE                   1
#define VA_LOWER_DATE_DAY_VALUE                     1
#define VA_HIGHER_DATE_YEAR_VALUE                   2107
#define VA_HIGHER_DATE_MONTH_VALUE                  12
#define VA_HIGHER_DATE_DAY_VALUE					31
#define VA_SOID_CUSTWD_MAX_LENGTH                   32
#define VA_UA_MAX_LENGTH							5
#define VA_LOCKED_VALUE								0
#define VA_UNLOCKED_VALUE							1
#define VA_NO_MODIFICATION_LOCK_VALUE               0xFF
#define VA_PRESEL_THLE_VALUE						1
#define VA_PRESEL_LINK_VALUE						2
#define VA_PRESEL_PNUMB_VALUE						3
#define VA_PRESEL_PPVP_VALUE						4
#define VA_PRESEL_PPVT_VALUE						5
#define VA_INFO_PRESEL_MAX_LENGTH                   9
#define VA_WRITE_IN_PRESELECTION_VALUE              0
#define VA_DELETE_IN_PRESELECTION_VALUE             1

#define VA_PPVP_VALUE								0
#define VA_PPVT_VALUE								1

#define VA_CURRENCY_MAX_LENGTH                      16		/* ###0.35a */
#define VA_IPPV_UNKNOWN_PAYMENT                     0
#define VA_IPPV_POST_PAYMENT						1
#define VA_IPPV_PRE_PAYMENT                         2

#define VA_TYPE_EMMGA_VALUE                         0		/* cannot be changed (see _VA_emm_process_confidential_info and _VA_emm_enter_info)	*/
#define VA_TYPE_EMMGH_VALUE                         1		/*        " 												"												 */
#define VA_TYPE_EMMU_VALUE							2		/*        " 												"												 */

#ifdef VA_TCPIP_MODEM										/* ###0.25b */
	#define VA_NP_MAX_LENGTH						4		/* IP address */
	#define VA_NC_MAX_LENGTH						2		/* port number */
#else
	#define VA_NP_MAX_LENGTH						15		/* ###0.25a (see spec. "Viaccess Online Transactions") */
	#define VA_NC_MAX_LENGTH						24
#endif

#define VA_CONNEXION_NAL_SURVEY_VALUE               0
#define VA_CONNEXION_DIRECT_PHONE_VALUE             1
#define VA_NOTHING_WITH_EVENT_VALUE                 0
#define VA_PURCHASE_EVENT_VALUE                     1
#define VA_CANCEL_EVENT_VALUE						2
#define VA_DATA_COMMAND_MAX_LENGTH                  6

#define VA_LIST_PE_MAX_VALUE						32

#define VA_QEV_ACTIVATED_OK_VALUE                   0x00F3
#define VA_QEV_ACTIVATED_NOT_OK_VALUE               0xFFF3
#define VA_QEV_ACTIVATED_NO_CARD_VALUE              0xFFFF
#define VA_QEV_DESACTIVATED_VALUE                   0x002E
#define VA_NO_QEV_VALUE                             0x0000
#define VA_QEV_VALUE_MAX_LENGTH                     16
#define VA_QEV_TERMINAL_ID_MAX_LENGTH               8

#define VA_MAX_PREVIEW_READ                         32		/* ###0.35d */

/* (###0.35f */
#define VA_WRITE_EVENT_NOTIFICATION_VALUE           0
#define VA_DELETE_EVENT_NOTIFICATION_VALUE          1
#define VA_MAX_EVENT_NOTIFICATION                   8

#define VA_EVENT_GENERIC                            0		/* List of possible event_type */
#define VA_EVENT_PREBOOKED                          1
#define VA_EVENT_CREDIT                             2
#define VA_GENERIC_EVENT_MAX_LENGTH                 255
/* ###0.35f) */

#ifdef VA_MODEM_API /* ###0.37a */
	#define VA_MODEM_TAG1                           0xFF
	#define VA_MODEM_TAG2                           0x00

	enum    /* possible values for tagged data in VA_MODEM_SEND_REQUEST */
	{
        VA_MODEM_TAG_UA,
        VA_MODEM_TAG_NEXT       /* dummy value, for internal tests */
	};
#endif

/*****************************************/
/*** Possible values for va_msg_type_t ***/
/*****************************************/
enum
{
	/*-------------------*/
	/* Viaccess messages */
	/*-------------------*/

	VA_NO_MESSAGE,								/*	dummy value, no message will be sent to application */
	VA_AGREEMENT_MESSAGE,
	VA_PURCHASE_OFFER_MODIFIED_MESSAGE,
	VA_LOCK_OFFER_MODIFIED_MESSAGE,
	VA_DESCRAMBLING_OK_MESSAGE,
	VA_PC2_CARD_NEEDED_MESSAGE,
	VA_PC2_CARD_FAILED_MESSAGE,
	VA_PC2_CARD_NOT_ACCEPTED_MESSAGE,
	VA_NEW_CARD_OK_MESSAGE,
	VA_NOT_ENOUGH_MEMORY_MESSAGE,
	VA_OVERDRAFT_REACHED_MESSAGE,
	VA_THRESHOLD_REACHED_MESSAGE,
	VA_NO_RIGHT_MESSAGE,
	VA_GEOGRAPHICAL_BLACKOUT_MESSAGE,
	VA_ACCESS_INTO_PREVIEW_MESSAGE,
	VA_PPVP_NOT_POSSIBLE_NOW_MESSAGE,
	VA_STATUS_QEV_MESSAGE,

	/*------------------------------------------------*/
	/* Viaccess requests (also used as message types) */
	/*------------------------------------------------*/

	VA_SOID_LIST_REQUEST,
	VA_SOID_THEME_SUBSCRIPTION_LIST_REQUEST,
	VA_SOID_CLASS_SUBSCRIPTION_LIST_REQUEST,
	VA_SOID_PRE_BOOKED_LIST_REQUEST,
	VA_SOID_IPPV_P_LIST_REQUEST,
	VA_SOID_IPPV_T_LIST_REQUEST,
	VA_SOID_FAC_DATA_REQUEST,
	VA_SOID_LABEL_REQUEST,
	VA_SOID_INFO_CREDIT_REQUEST,
	VA_UA_REQUEST,
	VA_MORALITY_LEVEL_REQUEST,
	VA_CEILING_REQUEST,
	VA_THRESHOLD_REQUEST,
	VA_LOCK_REQUEST,
	VA_PRESELECTION_REQUEST,
	VA_SOID_CONSULTATION_REQUEST,
	VA_ECM_INFORMATION_REQUEST,
	VA_ECM_THLE_LIST_REQUEST,
	VA_ECM_LINK_LIST_REQUEST,
	VA_ECM_PNUM_LIST_REQUEST,
	VA_PURCHASE_ACCEPT_REQUEST,
	VA_PURCHASE_STOP_REQUEST,

	VA_MODIFY_PIN_CODE_REQUEST,
	VA_MODIFY_MORALITY_LEVEL_REQUEST,
	VA_MODIFY_CEILING_REQUEST,
	VA_MODIFY_THRESHOLD_REQUEST,
	VA_MODIFY_LOCK_REQUEST,
	VA_MODIFY_SOID_PRESELECTION_REQUEST,

	VA_MANUAL_CONNEXION_MODEM_REQUEST,
	VA_PURCHASE_BY_MODEM_REQUEST,
	VA_EMM_REQUEST,
	VA_INFO_QEV_REQUEST,

	VA_STOP_REQUEST,

	/*-------------------------------------*/
	/* some more Viaccess messages:			*/
	/* (added to the end of this list to 	*/
	/*	be compatible with former releases)	*/
	/*-------------------------------------*/

	VA_FREE_TO_AIR_MESSAGE,                             /* ###0.22i */
	VA_NOT_ENOUGH_TOKEN_MESSAGE,                        /* ###0.23a */
	VA_ECM_WRONG_REF_MESSAGE,                           /* ###0.24b */
	VA_PC2_NOT_AVAILABLE_MESSAGE,                       /* ###0.24d */
	VA_SOID_IPPV_PARAMETER_REQUEST,                     /* ###0.35a */
	VA_ECM_PREVIEW_INFO_REQUEST,                        /* ###0.35d */
	VA_MODIFY_SOID_EVENT_NOTIFICATION_REQUEST,          /* ###0.35f */
	VA_EVENT_NOTIFICATION_LIST_REQUEST,                 /* ###0.35f */
	VA_EVENT_NOTIFICATION_MESSAGE,                      /* ###0.35f */

	VA_MODEM_CONNECT_REQUEST,                           /* ###0.37a */
	VA_MODEM_SEND_REQUEST,                              /* ###0.37a */
	VA_MODEM_RECEIVE_REQUEST,                           /* ###0.37a */
	VA_MODEM_DISCONNECT_REQUEST,                        /* ###0.37a */
	VA_MODEM_DATA_AVAILABLE_MESSAGE,                    /* ###0.37a */
	VA_MODEM_CONNEXION_LOST_MESSAGE,                    /* ###0.37a */
	VA_ILLEGAL_PC2_CARD__MESSAGE,                       /* ###0.40  */
	VA_VERSION_NUMBER_REQUEST,                          /* ###0.40  */
	VA_VERSION_NAME_REQUEST,                            /* get name of deliveried library */
    VA_NEXT_REQUEST                                     /* dummy value, for internal tests */
};

/********************************************/
/*** List of possibilities for the status ***/
/********************************************/
enum
{
	VA_OK_STATUS,
	VA_ARRAY_TOO_SMALL_STATUS,
	VA_BAD_PIN_CODE_STATUS,
	VA_NO_PIN_CODE_STATUS,
	VA_NOT_ENOUGH_MEMORY_STATUS,
	VA_BAD_SOID_STATUS,
	VA_OPERATION_NOT_ACCEPTED_STATUS,
	VA_PRESELECTION_AREA_FULL_STATUS,
	VA_PROG_ALREADY_STORED_IN_PRESELECTION_AREA_STATUS,
	VA_PROG_NOT_FOUND_IN_PRESELECTION_AREA_STATUS,
	VA_PRESELECTION_MODE_NOT_ACCEPTED_STATUS,
	VA_FUNCTION_LOCKED_STATUS,
	VA_PHONE_NUMBER_NOT_FOUND_STATUS,
	VA_MODEM_CONNEXION_FAILED_STATUS,
	VA_MODEM_DECONNEXION_STATUS,
	VA_STOPPED_REQUEST_STATUS,
	VA_NO_REQUEST_FOUND_STATUS,
	VA_BAD_HANDLE_STATUS,
	VA_NO_PURCHASE_PROPOSED_STATUS,
	VA_ERROR_STATUS,							/* other errors */

	VA_EVENT_NOTIFICATION_AREA_FULL_STATUS,     /* ###0.35f */
	VA_EVENT_ALREADY_STORED_STATUS,             /* ###0.35f */
	VA_EVENT_NOT_FOUND_STATUS,                  /* ###0.35f */

	/* status values for the VA modem API */
	VA_MODEM_TIMEOUT_STATUS,                    /* ###0.37a */
	VA_MODEM_NOT_CONNECTED_STATUS,              /* ###0.37a */
	VA_MODEM_DATA_NOT_ACCEPTED_STATUS           /* ###0.37a */
};

/*******************************************************/
/*** List of possibilities for the progression value ***/
/*******************************************************/
enum												
{
	VA_PHONE_NUMBER_FOUND_STATUS,
	VA_MODEM_CONNEXION_OK_STATUS,
	VA_DIALOGUE_MODEM_IN_PROGRESS_STATUS
};


/*======================================================================*/
/*	types																*/
/*======================================================================*/

typedef void            *va_proc_id_t;

typedef int	        va_bool;

typedef u_int32		va_stream_t;
typedef u_int16		va_msg_type_t;
typedef u_int16		va_msg_class_t;


typedef int8 va_flag_t;			/* type of flags used in Viaccess */

typedef int32 va_handle_t;		/* ###0.21a type of handles used in Viaccess */

typedef int32 va_msg_status_t;	/* see Viaccess API, 'VA_STATUS_QEV_MESSAGE' */


typedef struct /* Viaccess message */
{
  va_msg_class_t	class; /* class of the message */
  va_msg_type_t		type;  /* type of the message   */
  union
  {
    struct /* Info type for Viaccess messages */
    {
        /* initiated by Viaccess */
        va_handle_t handle;     /* event handle */
        va_msg_status_t status; /* message status, e.g.: */
        /* VA_QEV_ACTIVATED_OK_VALUE     */
        /* VA_QEV_ACTIVATED_NOT_OK_VALUE */
        /* VA_QEV_DESACTIVATED_VALUE     */
        /* VA_NO_QEV_VALUE               */
    } msg;

    struct /* Info type for Viaccess messages */
    { /* after a user request */
        int32 p_req;    /* pointer to request */
        int32 progress; /* progression value  */
    } req;

    char  c[8]; /* Info types for Viaccess messages */
    short s[4]; /* received from the environment */
    long  l[2];
  } info;

} va_msg_t; /* (same format as optv_message) */



/*----------------------------------------------------------------------*/
/***                    Common data structures                        ***/
/*----------------------------------------------------------------------*/

typedef u_int8 va_request_type;			/* Common part of a request #####0.35e */
typedef u_int8 va_request_status;		/* #####0.35e */

typedef struct
{
	va_request_type     type;
	va_request_status   status;
	short               tag;
} va_common_request;


typedef u_int32 va_program_number;		/* Program numbers */

typedef struct
{
	va_program_number		from;
	va_program_number		to;
} va_program_number_range;


typedef u_int32     va_soid;    /* Service Operator IDs */

typedef u_int32     va_token;   /* Money, tokens, credits */

typedef struct
{
	int32       sign;   /* VA_POSITIVE_VALUE or VA_NEGATIVE_VALUE */
	va_token    n;
} va_signed_credit;

/* PIN code */
typedef u_int8 va_pin_code[VA_PIN_CODE_LENGTH];


typedef struct          /* Dates */
{                       /* If a date is before 1/1/1980, */
	u_int16     year;   /* the via-access module transforms it in 1/1/1980. */
	u_int8      month;  /* If after 12/31/2107, */
	u_int8      day;    /* it transforms it in 12/31/2107... */
} va_date;

typedef struct
{
	va_date     from;   /* minimum is 1/1/1980 */
	va_date     to;     /* maximum is 12/31/2107 */
} va_date_range;

typedef struct          /* ###0.35f */
{
	u_int8      length; /* number of useful bytes in 'data' */
	u_int8      *data;
} va_generic_data;



/*----------------------------------------------------------------------*/
/***                Consultation requests structures                  ***/
/*----------------------------------------------------------------------*/

typedef struct
{
	u_int8      preselection;
	u_int8      consultation;
	u_int8      maturity_rating;
	u_int8      impulse_purchase;
} va_lock;

typedef struct
{
	va_date     date;
	u_int8      th;
	u_int8      le;
} va_thle;

typedef struct
{
	va_date     date;
	u_int8      link;
} va_link;

typedef struct
{
	va_program_number       numb;
	va_token                cost;
} va_ippv_p;

typedef struct {
	va_program_number       numb;
	va_token                cost;
} va_ippv_t;

typedef va_handle_t		va_offer;

typedef struct  /* VA_SOID_LIST_REQUEST */
{
	va_common_request       c;
	size_t                  size;
	size_t                  number;
	va_soid                 *array;
} va_soid_list_request;

typedef struct  /* VA_SOID_THEME_SUBSCRIPTION_LIST_REQUEST */
{
	va_date_range       date;
	u_int8              theme;
	u_int8              level;
} va_theme_subscription;

typedef struct
{
	va_common_request       c;
	va_soid                 soid;
	va_date_range           date;
	size_t                  size;
	size_t                  number;
	va_theme_subscription   *array;
} va_soid_theme_subscription_list_request;

typedef struct  /* VA_SOID_CLASS_SUBSCRIPTION_LIST_REQUEST */
{
	va_date_range           date;
	size_t                  class_length;
	u_int8                  custwd[VA_SOID_CUSTWD_MAX_LENGTH];
} va_class_subscription;

typedef struct
{
	va_common_request       c;
	va_soid                 soid;
	va_date_range           date;
	size_t                  size;
	size_t                  number;
	va_class_subscription   *array;
} va_soid_class_subscription_list_request;

typedef va_program_number_range va_pre_booked;	/* VA_SOID_PRE_BOOKED_LIST_REQUEST */

typedef struct
{
	va_common_request           c;
	va_soid                     soid;
	va_program_number_range     range;
	size_t                      size;
	size_t                      number;
	va_pre_booked               *array;
} va_soid_pre_booked_list_request;

typedef struct  /* VA_SOID_IPPV_P_LIST_REQUEST */
{
	va_common_request       c;
	va_soid                 soid;
	va_program_number_range range;
	size_t                  size;
	size_t                  number;
	va_ippv_p               *array;
} va_soid_ippv_p_list_request;

typedef struct  /* VA_SOID_IPPV_T_LIST_REQUEST */
{
	va_common_request       c;
	va_soid                 soid;
	va_program_number_range range;
	size_t                  size;
	size_t                  number;
	va_ippv_t               *array;
} va_soid_ippv_t_list_request;

typedef struct  /* VA_SOID_IPPV_PARAMETER_REQUEST ###0.35a */
{
	u_int8          conversion_rate_numerator;
	u_int8          conversion_rate_denominator;
	u_int16         ippvt_base_rate;
	size_t          currency_length;
	char            currency[VA_CURRENCY_MAX_LENGTH];
} va_cost_of_token;

typedef struct
{
	va_common_request       c;
	va_soid                 soid;
	va_cost_of_token        cost_of_token;
	va_bool                 ippv_without_modem;
	u_int8                  commercial_approach;
} va_soid_ippv_parameter_request;

typedef struct  /* VA_SOID_FAC_DATA_REQUEST */
{
	va_common_request		c;
	va_soid					soid;
	u_int8					NP1;
	u_int8					NP2;
	size_t					size;
	size_t					number;
	u_int8					*array;
} va_soid_fac_data_request;

typedef struct  /* VA_SOID_LABEL_REQUEST */
{
	va_common_request       c;
	va_soid                 soid;
	size_t                  label_length;
	char                    label[VA_SOID_LABEL_MAX_LENGTH];
} va_soid_label_request;

typedef struct  /* VA_SOID_INFO_CREDIT_REQUEST */
{
	va_date         date;
	va_token        money;
} va_info_credit;


typedef struct
{
	va_common_request       c;
	va_soid                 soid;
	va_info_credit          totam;
	va_info_credit          over;
	va_token                sum1;
	va_token                sum2;
	va_signed_credit        remaining_credit;
} va_soid_info_credit_request;

typedef struct  /* VA_UA_REQUEST */
{
	va_common_request		c;
	size_t					ua_length;
	u_int8					ua[VA_UA_MAX_LENGTH];
} va_ua_request;

typedef struct  /* VA_MORALITY_LEVEL_REQUEST */
{
	va_common_request		c;
	u_int8					morality_level;
} va_morality_level_request;

typedef struct  /* VA_CEILING_REQUEST */
{
	va_common_request       c;
	va_token                ceiling;
} va_ceiling_request;

typedef struct  /* VA_THRESHOLD_REQUEST */
{
	va_common_request   c;
	va_token            threshold;
} va_threshold_request;

typedef struct  /* VA_LOCK_REQUEST */
{
	va_common_request   c;
	va_lock             lock;
} va_lock_request;

typedef union   /* VA_PRESELECTION_REQUEST */
{
	va_thle                 thle;
	va_link                 link;
	va_program_number       pnumb;
	va_ippv_p               ppvp;
	va_ippv_t               ppvt;
} va_info_preselection;

typedef struct
{
	va_soid                 soid;
	u_int8                  type;
	va_info_preselection    info;
} va_preselection;

typedef struct
{
	va_common_request       c;
	size_t                  size;
	size_t                  number;
	va_preselection         *array;
} va_preselection_request;

typedef struct  /* VA_SOID_CONSULTATION_REQUEST */
{
	va_common_request		c;
	va_soid					soid;
	u_int8					pi;
	u_int8					length_data_command;
	u_int8					data_command[VA_DATA_COMMAND_MAX_LENGTH];
	size_t					size;
	u_int8					*array;
} va_soid_consultation_request;

typedef struct  /* VA_ECM_INFORMATION_REQUEST */
{
	va_soid                 soid;
	u_int8                  thle_included_ecm;
	u_int8                  link_included_ecm;
	u_int8                  pnum_included_ecm;
	u_int8                  ppvp_included_ecm;
	va_bool                 ppvp_requested;
	va_program_number       pnum_ppvp;
	va_token                cost_ppvp;
	u_int8                  ppvt_included_ecm;
	va_bool                 ppvt_requested;
	va_program_number       pnum_ppvt;
	va_token                cost_ppvt;
	va_bool                 impulse_purchase_locked;
	va_bool                 morality_level_exceeded;
	u_int8                  program_morality_level;
} va_ecm_information;

typedef struct
{
	va_stream_t         type;
	int32               track;
} va_pe;

typedef struct
{
	u_int8              nb_pe;
	va_pe               list_pe[VA_LIST_PE_MAX_VALUE];
} va_pe_information;

typedef struct
{
	va_common_request       c;
	va_offer                offer;
	va_pe_information       pe_info;
	va_ecm_information      ecm_info;
} va_ecm_information_request;

typedef struct  /* VA_ECM_THLE_LIST_REQUEST*/
{
	va_common_request       c;
	va_offer                offer;
	va_soid                 soid;
	size_t                  size;
	size_t                  number;
	va_thle                 *array;
} va_ecm_thle_list_request;

typedef struct  /* VA_ECM_LINK_LIST_REQUEST*/
{
	va_common_request       c;
	va_offer                offer;
	va_soid                 soid;
	size_t                  size;
	size_t                  number;
	va_link                 *array;
} va_ecm_link_list_request;

typedef struct  /* VA_ECM_PNUM_LIST_REQUEST*/
{
	va_common_request       c;
	va_offer                offer;
	va_soid                 soid;
	size_t                  size;
	size_t                  number;
	va_program_number       *array;
} va_ecm_pnum_list_request;

typedef struct  /* VA_ECM_PREVIEW_INFO_REQUEST ###0.35d */
{
	u_int8          preview_index;
	u_int16         preview_number;
	u_int8          cw_number;
} va_preview_info;

typedef struct
{
	va_common_request   c;
	va_offer            offer;
	va_preview_info     preview_info_ecm;
	va_preview_info     preview_info_card;
} va_ecm_preview_info_request;

typedef struct  /* VA_PURCHASE_ACCEPT_REQUEST */
{
	va_common_request       c;
	va_offer                offer;
	u_int8                  type_ppv;
	va_program_number       pnum_ppv;
	va_token                cost_ppv;
} va_purchase_accept_request;

typedef struct									/* VA_PURCHASE_STOP_REQUEST */
{
	va_common_request       c;
	va_offer                offer;
} va_purchase_stop_request;

typedef struct  /* VA_EVENT_NOTIFICATION_LIST_REQUEST ###0.35f */
{
	u_int8          id;
	va_soid         soid;
	u_int8          type;
	union
	{
		va_program_number       prebooked;
		va_token                credit;
		va_generic_data         generic;
	} info;
} va_event_t;

typedef struct
{
	va_common_request		c;
	size_t					size;
	size_t					number;
	va_event_t				*array;
} va_event_notification_list_request;


/********************************************************/
/***          Modification requests structures        ***/
/********************************************************/
typedef struct  /* VA_MODIFY_PIN_CODE_REQUEST */
{
	va_common_request		c;
	va_pin_code				pin;
	va_pin_code				new_pin;
} va_modify_pin_code_request;

typedef struct  /* VA_MODIFY_MORALITY_REQUEST */
{
	va_common_request		c;
	va_pin_code				pin;
	u_int8					new_morality_level;
} va_modify_morality_level_request;

typedef struct  /* VA_MODIFY_CEILING_REQUEST */
{
	va_common_request       c;
	va_pin_code             pin;
	va_token                new_ceiling;
} va_modify_ceiling_request;

typedef struct  /* VA_MODIFY_THRESHOLD_REQUEST */
{
	va_common_request       c;
	va_pin_code             pin;
	va_token                new_threshold;
} va_modify_threshold_request;

typedef struct  /* VA_MODIFY_LOCK_REQUEST */
{
	va_common_request       c;
	va_pin_code             pin;
	va_lock                 lock;
} va_modify_lock_request;

typedef struct  /* VA_MODIFY_SOID_PRESELECTION_REQUEST */
{
	va_common_request		c;
	u_int8					writing_deleting;
	va_soid					soid;
	u_int8					type;
	va_info_preselection	info;
} va_modify_soid_preselection_request;

typedef struct  /* VA_MODIFY_SOID_EVENT_NOTIFICATION_REQUEST ###0.35f */
{
	va_common_request		c;
	u_int8					writing_deleting;
	va_event_t				event;
} va_modify_soid_event_notification_request;


/***********************************************/
/***        Other requests structures        ***/
/***********************************************/
typedef struct  /* VA_MANUAL_CONNEXION_MODEM_REQUEST */
{
	va_common_request		c;
	va_soid					soid;
} va_manual_connexion_modem_request;

typedef struct  /* VA_PURCHASE_BY_MODEM_REQUEST */
{
	va_common_request       c;
	va_soid                 soid;
	u_int8                  event_purchase_cancelling;
	va_program_number       event;
	va_bool                 ask_for_units;
	va_token                number_of_units;
	/* (###0.23f */
	va_bool                 generic_demand;
	u_int8                  generic_demand_length;
	u_int8                  *generic_demand_data;
	/* ###0.23f) */
} va_purchase_by_modem_request;

typedef struct  /* VA_EMM_REQUEST */
{
	va_common_request		c;
	u_int8					type_emm;
	size_t					emm_length;
	u_int8					*emm_buffer;
	u_int8					me1, me2;
	u_int8					mec1, mec2;
} va_emm_request;

typedef struct  /* VA_INFO_QEV_REQUEST */
{
	va_common_request		c;
	u_int16					flag_qev;
	size_t					qev_value_length;
	u_int8					qev_value[VA_QEV_VALUE_MAX_LENGTH];
} va_info_qev_request;

typedef struct  /* VA_STOP_REQUEST */
{
	va_common_request		c;
} va_stop_request;


/**********************************************/
/***      VIACCESS MODEM API structures     ***/
/**********************************************/
#ifdef VA_MODEM_API     /* ###0.37a */

typedef struct  /* VA_MODEM_CONNECT_REQUEST */
{
	va_common_request		c;
	va_soid					soid;
	u_int8					NP1;
	u_int8					NP2;
	u_int8					nal;
} va_modem_connect_request;

typedef struct  /* VA_MODEM_SEND_REQUEST */
{
	va_common_request		c;
	size_t					buf_num;
	u_int8					*buf_out;
	u_int8					tag;
} va_modem_send_request;

typedef struct  /* VA_MODEM_RECEIVE_REQUEST */
{
	va_common_request		c;
	size_t					buf_size;
	size_t					buf_num;
	u_int8					*buf_in;
} va_modem_receive_request;

typedef struct  /* VA_MODEM_DISCONNECT_REQUEST */
{
	va_common_request		c;
} va_modem_disconnect_request;

#endif

#define VA_VN_MAX_LENGTH	2

typedef struct
{
	va_common_request   c;
	size_t              vn_length;
	u_int8              vn[VA_VN_MAX_LENGTH];
} va_vn_request;


#define MAX_VERSION_NAME_SIZE 256

typedef struct
{
	va_common_request   c;
	char                version_name[MAX_VERSION_NAME_SIZE];
} va_version_name_request;


/***********************************************/
/***     return value of VA_mmi_request()    ***/
/***********************************************/
typedef enum
{
	VA_REQ_GOOD = GOOD,     /* OK, request has been registered */
	VA_REQ_UNKNOWN,         /* unknown request */
	VA_REQ_INVALID,         /* invalid command structure */
	VA_REQ_BUSY,            /* Viaccess is busy at the moment */
	VA_REQ_SC_ERROR         /* Smart card error */
} va_mmi_request_result;


/***********************************/
/***     Event Message types     ***/
/***********************************/
typedef enum
{
	VA_MSG_ALL,
	VA_MSG_PPV,
	VA_MSG_CARD,
	VA_MSG_SYSTEM
} va_msg_set_t;


typedef enum
{
	VA_FOCUS_ADD,
	VA_FOCUS_REMOVE,
	VA_FOCUS_REFRESH    /* ###0.24d */
} va_focus_t;


typedef va_bool (*va_msg_fct_t) ( va_msg_t *msg );



/**************************/
/***     QEV request    ***/
/**************************/
typedef struct
{
   u_int8 type;								/* IL type for hardware type */
   u_int8 version;							/* IL type for hardware version */
} va_il_hard_t;

typedef enum
{
	QEV_DISABLED 		= 0x00,				/* QEV is completely disabled */
	QEV_ACTIVATED 		= 0xF3,				/* QEV verification is activated */
	QEV_DISACTIVATED 	= 0x2E				/* QEV verification is disactivated */
} va_qev_status_t;

typedef struct
{
	size_t		length;						/* QEV value of the terminal */
	u_int8		value[VA_QEV_VALUE_MAX_LENGTH];
} va_qev_value_t;

typedef struct
{
	size_t		length;						/* terminal ID */
	u_int8		id[VA_QEV_TERMINAL_ID_MAX_LENGTH];
} va_terminal_id_t;

typedef struct
{
	va_qev_status_t     init_status;		/* init status of QEV */
	va_qev_value_t		qev_value;      /* QEV value of the terminal */
	va_terminal_id_t	terminal_id;    /* terminal ID for QEV verification */
} va_qev_info_t;

													/* function to build the modem dial string */
typedef u_int8* (*format_dial_fct_t) (	u_int8 *np, u_int8 *nc );

													/* function to notify the control task about QEV status */
													/* current_status is the same as 'msg.info.msg.status'	*/
													/* in a VA_STATUS_QEV_MESSAGE	*/
typedef void (*write_qev_status_fct_t) ( va_msg_status_t current_status);

													/*	function to erase the list of erroneous phone */
													/*	numbers that is managed by the decoder software */
typedef void (*erase_blacklist_fct_t) ( void );		/* ###0.21f */




#endif /* _VA_OPENTV_OTYPE1X_H_ */
