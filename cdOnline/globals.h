/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Include File Name:	cdOnline/h/globals.h
*
*   Purpose:
*	Global definitions for program cdOnline.
*
*   Date		: Dec 13, 1996
*
*   SCCS data		: @(#)
*	Module Name	: globals.h
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 06/25/97
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/02/18 JSD  :	Added CO_E_AL and macro CO_CHECK_AL.
*	97/06/20 SEC  : Added CO_QUIT status. 
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


/*
 *  Define the message and return codes.
 */

#define	CO_QUIT			4	/* Quit at user's request.	*/
#define	CO_VERSION		3	/* Version message.		*/
#define	CO_VOLUME_MOUNTED	2	/* The volume has been mounted.	*/
#define	CO_VOLUME_UNMOUNTED	1	/* The voluem has been unmounted*/

#define	CO_SUCCESS		0	/* Normal completion.		*/

#define	CO_E_AD			(-1)	/* Error in the ad library.	*/
#define	CO_E_AL			(-2)	/* Error in the al library.	*/
#define	CO_E_CD			(-3)	/* Error in the cd library.	*/
#define	CO_E_DB			(-4)	/* Error in the sybase db lib.	*/
#define	CO_E_DB_LIB		(-5)	/* Error in the CADC db lib.	*/
#define	CO_E_MEMORY		(-6)	/* Memory allocation failure.	*/
#define	CO_E_NO_VOLNAME		(-7)	/* Voluem name not in cds table.*/
#define	CO_E_NO_VOLUME		(-8)	/* Volume not in cds table.	*/
#define	CO_E_SYSTEM		(-9)	/* Error in a system call.	*/
#define	CO_E_USAGE		(-10)	/* Command line usage error.	*/


#define	APPLICATION		"cdOnline"


/*
 *  Define the message severity symbols.
 */

#define	MESS_ERROR		0
#define	MESS_INFO		1


/*
 *  Return status handling macros.
 */

#define	CO_CHECK(s)							\
	{   int		estatus;					\
	    if ( ( estatus = (s) ) != CO_SUCCESS )			\
	 	return( estatus ); }

#define	CO_CHECK_AD( s )						\
	{   if ( (s) < 0 )						\
	    {	formatMessage( CO_E_AD );				\
		msg_append( msg, ad_msg );				\
		msg_clear( ad_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( CO_E_AD ); } }

#define CO_CHECK_AL(s)							\
	{								\
	    if ( (s)!= AL_SUCCESS )					\
	    {								\
		formatMessage( CO_E_AL );				\
		msg_append( msg, alMsg );				\
		msg_clear( alMsg );					\
		printMessage( msg, MESS_ERROR );			\
		return( CO_E_AL );					\
	    }								\
	}

#define	CO_CHECK_CD( s )						\
	{   if ( (s) != CD_SUCCESS )						\
	    {	formatMessage( CO_E_CD );				\
		msg_append( msg, cd_msg );				\
		msg_clear( cd_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( CO_E_CD ); } }

#define	CO_CHECK_DB( s )						\
	{   if ( ( s ) == FAIL )					\
	    {	formatMessage( CO_E_DB );				\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( CO_E_DB ); } }

#define	CO_CHECK_DB_LIB( s )						\
	{   if( (s) != DB_SUCCESS )					\
	    {	formatMessage( CO_E_DB_LIB );			\
		msg_append( msg, db_msg );				\
		msg_clear( db_msg );					\
		printMessage( msg, MESS_ERROR );			\
		return( CO_E_DB_LIB ); } }

#define	CO_CHECK_NULL(s)						\
	{   if ( (s) == NULL )						\
	    {	formatMessage( CO_E_MEMORY );			\
		printMessage( msg, MESS_ERROR );			\
		return( CO_E_MEMORY ); } }

#define	CO_CHECK_SYSTEM( s, file )					\
	{   if ( (s) < 0 )						\
	    {	formatMessage( MSG_ERRNO, file );			\
		printMessage( msg, MESS_ERROR );			\
		return( CO_E_SYSTEM ); } }


/*
 *  Define the globals strucutre.
 */

typedef struct globals
{
    struct dbprocess 
		*gDbproc;	/* Dbprocess to use.			*/
    char	*gTarget;	/* target disk or path.			*/
    boolean	gUnmount;	/* Are we unmounting a disk?		*/
    boolean	gVersion;	/* Print version and exit?		*/
} GLOBALS;


/*
 *  Declare the global symbols.
 */

extern	GLOBALS	globals;
extern	char	msg[];
extern	char	*version_long;
extern	char	*version_short;


/*
 * Declare the global functions.
 */

int	cdMount( char * );
int	cdUnmount( char * );
void	formatMessage( int, ... );
int	globalsInit( void );
int	logDb( void );
void	printMessage( char *, int );
