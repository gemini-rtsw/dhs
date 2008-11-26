/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	db/h/db.h
*
*   Purpose:
*	Contains the globals definitions for the cadc db library.
*
*   Date		: Sept 10, 1993
*
*   SCCS data		: @(#)
*	Module Name	: db.h
*	Version Number	: 1.29
*	Release Number	: 1
*	Last Updated	: 04/06/00
*
*   Programmer		: Norman Hill
*
*   Modification History:
*	25/05/94 NRH  :	Added dbrc status codes.
*	16/08/95 NRH  :	Added fn db_strcpy.
*       96/02/07 SEC  :	Added function db_datetimedata().
*	97/07/02 SEC  :	Added C++ compatibility #ifdef. 
*	97/11/02 pdd  : Added db_doubledata().
*	97/12/17 pdd  : added db_smallintdata() and db_tinyintdata().
*	00/04/06 nrh  : Added db_crc32data() and db_crc64data().
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#define	DB_NO_INDEX		3	/* No index was found.		*/
#define DB_NULL_VALUE		2	/* Retrieved a NULL value from db. */
#define DB_TRUNCATED		1	/* String truncated (dbstringdata). */

#define	DB_SUCCESS		0	/* Normal completion.		*/

#define DB_E_BAD_COLUMN		-1	/* Invalid column specified.	*/
#define	DB_E_DATE_FORMAT	-2	/* Invalid date format.		*/
#define	DB_E_DB			-3	/* Error in the db library.	*/
#define	DB_E_DB_LIBRARY		-4	/* DBerrhandler message.	*/
#define	DB_E_IOCTL		-5	/* Error calling oictl.		*/
#define	DB_E_MEMORY		-6	/* Memory allocation failure.	*/
#define	DB_E_NO_USER_INF	-7	/* No sybase user info available*/
#define	DB_E_NOT_EXISTS		-8	/* The specified process does  	*/
					/* not exist.			*/
#define	DB_E_PERMIT		-9	/* Permitions on the dbrc file	*/
					/* are incorrect.		*/
#define	DB_E_SQL_SERVER		-10	/* Error from the sql server.	*/
#define	DB_E_SYSTEM		-11	/* Unix system error.		*/


#define	DB_UTC			1	/* Convert assuming utc time zone.*/
#define	DB_LOCAL		2	/* Convert assuming local time zone.*/

#define DB_DAYS1900TO1970	25567	/* Number of days between 
					    Jan 1 1900 and Jan 1 1970.	*/
#define DB_SECSPERDAY		3600*24	/* Number of secs in a day.	*/
#define	DB_SYSNAME_LEN		30	/* Max length of system names.	*/


/*
 *  The following #ifdef makes the interface to C++ cleaner.
 */

#ifdef  __cplusplus
extern "C" {
#endif


/*
 *  Start of function prototypes and external declarations. 
 */

int	db_binarydata( struct dbprocess *, int, byte *, int );
int	db_chardata( struct dbprocess*, int, char * );
int	db_close( struct dbprocess * );
int	db_count_app( struct dbprocess *, char *, char *, int * );
int	db_crc32data( struct dbprocess *, int, unsigned long * );
int	db_crc64data( struct dbprocess *, int, unsigned long long * );

/*
 *  The definition of db_datetime2time is sybase version dependant, 
 *  and changed for the system 10 libraries.  The #ifdef below
 *  allows for this.
 */

#ifdef __CSTYPES_H__
time_t	db_datetime2time( struct _cs_datetime * );
int	db_datetimedata( struct dbprocess *, int, struct _cs_datetime * );
#else
time_t	db_datetime2time( struct com_datetime * );
int	db_datetimedata( struct dbprocess *, int, struct com_datetime * );
#endif

int	dbrc_get( char *, char *, boolean, char **, char ** );
int	db_doubledata( struct dbprocess *, int, double * );
int	db_exit( void );
int	db_floatdata( struct dbprocess *, int, float * );
int	db_index_id( struct dbprocess *, char *, char *, char *, int * );
int	db_init( char * );
int	db_intdata( struct dbprocess *, int, int * );
int	db_smallintdata( struct dbprocess *, int, int * );
int	db_tinyintdata( struct dbprocess *, int, int * );
int	db_open( char *, char *, char *, char *, boolean, struct dbprocess ** );
int	db_simple_query( struct dbprocess *, char *, ... );
long	db_str2time( char *, int );
void	db_strcpy( char *, char * );
int	db_stringdata( struct dbprocess *, int, char *, int );
int	db_table_exists( struct dbprocess *, char *, char *, boolean * );
char	*db_time2str( long, int );
int	db_timedata( struct dbprocess *, int, time_t * );

extern char	db_msg[];
extern int	db_errno;


/*
 *   Ending the C++ compatibility from above. 
 */

#ifdef    __cplusplus
}
#endif

