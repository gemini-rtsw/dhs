/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
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
*   Include File Name:	am/h/localAm.h
*
*   Purpose:
*       Local symbol definitions used by the am library.
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: localAm.h
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 02/03/00
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/09/04 JSD  :	Added device table names to local structure.
*	98/10/20 SDJ  : Added eptr compare functions.
*	98/01/20 SDJ  : Added laDefaultPrec.
*	99/04/28 NRH  : Added the action table to the global structure.
*     2000/02/03 SDJ  : Added the options table to the global structure.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


#define	AM_LIBRARY	"libAm"


/*
 *	Error Checking Macros.
 */

#define AM_CHECK(s)							\
	{   int	estatus;						\
	    if ( ( estatus = (s) ) != AM_SUCCESS)			\
		return(estatus); }

#define AM_CHECK_CONFIG(s)						\
	{   if ( ( s ) != GC_SUCCESS ) {				\
		amFormatMessage( AM_E_CONFIG );				\
		msg_append( amMsg, gc_msg );				\
		msg_clear( gc_msg );					\
		return( AM_E_CONFIG ); } }

#define	AM_CHECK_DB(s)	 						\
	{   if ( (s) == FAIL )						\
	    {	amFormatMessage( AM_E_DB );				\
		msg_append( amMsg, db_msg );				\
		msg_clear( db_msg );					\
		return( AM_E_DB ); } }

#define	AM_CHECK_DB_LIB(s)						\
	{   if ( (s) != DB_SUCCESS ) 					\
	    {	amFormatMessage( AM_E_DB_LIB );				\
		msg_append( amMsg, db_msg );				\
		msg_clear( db_msg );					\
		return( AM_E_DB_LIB ); } }

#define	AM_CHECK_NULL(s) 						\
	{   if ( (s) == NULL ) 						\
	    {	amFormatMessage( AM_E_MEMORY );				\
		return( AM_E_MEMORY ); } }


#define	AM_CHECK_NULL_CONFIG(s)						\
	{   if ( (s) == NULL ) 						\
	    {	amFormatMessage( AM_E_MEMORY );				\
		return( FALSE ); } }

#define	AM_CHECK_INIT							\
	if ( ! amInitFlag )						\
	{   amFormatMessage( AM_E_INIT );				\
	    return( AM_E_INIT ); }

#define	AM_CHECK_SYSTEM( s, file )					\
	{   if ( (s) < 0 )						\
	    {	amFormatMessage( MSG_ERRNO, file );			\
		return( AM_E_SYSTEM ); } }


typedef struct localAm
{
    char	*laArchDataStage;	/* Name of Archive Data Stage.	*/
    char	*laBookingTable;	/* Device booking table name.	*/
    char	*laDatabase;		/* Media database name.		*/
    int		laDefaultPrec;		/* Unit name default Precision.	*/
    char	*laDeviceTable;		/* Device table name.		*/
    char	*laIngestScript;	/* mediaIngest script name.	*/
    char	*laLogFileDir;		/* Name of the logfile path.	*/
    char	*laLogicalTable;	/* Logical device table name.	*/
    char	*laMedActionsTable;	/* Media actions table.		*/
    char	*laMedFilesTable;	/* Name of the mediaFiles tbl.	*/
    char	*laMedOptionsTable;	/* Media options table.		*/
    char	*laMedStagesTable;	/* Name of the mediaStages tbl.	*/
    char	*laMedTypesTable;	/* MediaTypes table name.	*/
    char	*laMedUnitsTable;	/* Name of the mediaUnits tbl.	*/
    char	*laMediaDataStage;	/* Name of Media Data Stage.	*/
    char	*laMediaTable;		/* Name of the media table.	*/
    struct eptr
    		*laMediaScripts;	/* Eptr list of media scripts.	*/
    char	*laServer;		/* Name of the database server.	*/
    char	*laStatusTable;		/* Name of the status tbl.	*/
    char	*laUserDataStage;	/* Name of User Data Stage.	*/
} LOCAL_AM;


/*
 *	Prototype declarations.
 */

int	amConfigInit( void );
void	amFormatMessage( int, ... );
int	devCmp( AM_DEVICE *, AM_DEVICE * );
int	scriptCmp( char *, AM_MED_SCRIPT * );


/*
 *  Define the globals local to the library.
 */

extern	LOCAL_AM	localAm;
extern	boolean		amInitFlag;



