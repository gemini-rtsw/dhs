static char rcsid[] = "$Id: staMonitor.C,v 1.1.1.1 2002-11-24 20:30:52 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997				(c) 1997
// National Research Council		Conseil national de recherches
// Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
// All rights reserved			Tous droits reserves
// 					
// NRC disclaims any warranties,	Le CNRC denie toute garantie
// expressed, implied, or statu-	enoncee, implicite ou legale,
// tory, of any kind with respect	de quelque nature que se soit,
// to the software, including		concernant le logiciel, y com-
// without limitation any war-		pris sans restriction toute
// ranty of merchantability or		garantie de valeur marchande
// fitness for a particular pur-	ou de pertinence pour un fs_usage
// pose.  NRC shall not be liable	particulier.  Le CNRC ne
// in any event for any damages,	pourra en aucun cas etre tenu
// whether direct or indirect,		responsable de tout dommage,
// special or general, consequen-	direct ou indirect, particul-
// tial or incidental, arising		ier ou general, accessoire ou
// from the use of the software.	fortuit, resultant de l'utili-
// 					sation du logiciel.
//
//***********************************************************************
//
// FILENAME
// dhsStatus/src/staMonitor.C
//
// PURPOSE:
// Handles the monitoring of all resources.
//
// METHOD NAME(S)
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.15  1999/06/16 22:29:43  nhill
// Added code to initialize the maximum number of resources logged.
//
// Revision 1.14  1998/06/25 19:59:42  jaeger
// Fixed bug in cStaMonDb::update.  In the check macros the return code was
// doing a break before releasing the database connection lock, NOT GOOD.
// There order was reversed.
//
// Revision 1.13  1998/06/24 23:01:48  jaeger
// Fixed database monitoring.  Was using the length of the data in the first
// column but getting the value in the second column, we wanted the second.
// Also made a check for MORE_ROWS in case an error occurs.
//
// Revision 1.12  1998/06/23 20:21:34  jaeger
// Fixed fullname value for database monitoring.  It was SERVER:alias instead
// of SERVER:DATABASE.  Made sure configuration information is remembered
// when simulate level command is executed.
//
// Revision 1.11  1998/06/19 18:04:58  nhill
// Changed the cStaStatus class name to cStaStat.
// Fixed bugs in disk and database monitoring.
// Improved some error messages.
//
// Revision 1.10  1998/06/16 22:23:37  jaeger
// fixed resource fullname.
//
// Revision 1.9  1998/06/02 18:03:34  jaeger
// Made sure if any of the resource items were not found in the
// config file an error was reported and the status server exited.
//
// Revision 1.8  1998/06/01 22:40:59  jaeger
// Added "full name" field to resource information.
//
// Revision 1.7  1998/05/29 21:13:53  jaeger
// Added an extra check on the times wait so that monitoring should
// be exited quicker when the server is asked to stop.
//
// Revision 1.6  1998/05/20 04:58:16  jaeger
// Added simulate methods.
//
// Revision 1.5  1998/05/12 20:25:51  nhill
// Minor fixes.
//
// Revision 1.4  1998/04/15 21:12:17  nhill
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.3  1998/03/06 19:28:36  jaeger
// Changed configuration reading so it doesn't error if there is
// nothing to monitor.  Made database query non-blocking. Added
// new parameter to staChannel::put method.
//
// Revision 1.2  1998/02/17 00:22:35  jaeger
// Added documentation.
//
// Revision 1.1  1998/02/05 21:24:33  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#define STDC	1    

//***********************************************************************
// 
//  General Unix include files.
// 
//***********************************************************************
    
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <errno.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>    
#include <signal.h>
    
extern "C"
{
//***********************************************************************
// 
//  SYBASE include files.
// 
//***********************************************************************

#include	<sybfront.h>
#include	<sybdb.h>


//***********************************************************************
// 
//  File until include files.
// 
//***********************************************************************

// XXX allan: replaced with statvfs
//#include "fsusage.h"
#include <sys/statvfs.h>
int statvfs(const char *, struct statvfs *);


//***********************************************************************
// 
//  CADC library include files.
// 
//***********************************************************************

#include "gen_types.h"	
#include "gen_config.h"
#include "gen_str.h"	

#include "db.h"
}
 
    

    
//***********************************************************************
// 
//  DHS include files.
// 
//***********************************************************************
    
#include "dhsGenStatus.H"


//***********************************************************************
// 
//  Dhs Status include files.
// 
//***********************************************************************    
    
#include "globals.H"
#include "staDhs.H"
#include "staMonitor.H"
#include "staChannel.H"    



    
//***********************************************************************
//
// Local defines
//
//***********************************************************************
    
#define CHANNEL_KEYWORD			"channel"
#define	FREQ_KEYWORD			"timer" // Keyword for freq.
#define DB_SERVER_KEYWORD		"dbServer"    
#define DB_MON_KEYWORD			"db"	// config Keyword for DB.
#define MD_MON_KEYWORD			"md"	// config Keyword for disks.

#define MAX_LS				10	// Max. chars in a string
    						// representing a long.
#define MAX_RES				12	// Maximum allowed res.    

#define NULL_SPACE			0	// Null space value
#define STA_RES_INUSE			":inUse"
#define STA_RES_LEN			8	    
#define STA_RES_MAX			":max"
#define STA_RES_FULLNAME		":fullname"
#define STA_RES_NAME			":name"
#define STA_RES_PREFIX			"resource"
#define STA_RES_SUB			"-"
#define STA_RES_SIZE			"resourceArraySize"    
#define STA_RES_TYPE			":type"
#define STA_RES_UNITS			":units"
#define STA_RES_USED			":used"

#define PATH_INCR			32    


//***********************************************************************
//
// Definitions of static variables.
//
//***********************************************************************
	
struct dbprocess    *cStaMonDb::dbConnection = NULL;
list<tStaDb*>  	    cStaMonDb::dbList;
cMutex		    cStaMonDb::dbProcMutex;
char		    *cStaMonDb::dbServerName;
    
list<tStaMd*>  	    cStaMonMd::mdList;
    
timespec	    cStaMon::absTime;
bool		    cStaMon::checking=TRUE;
cCond		    cStaMon::checkRes;
list<cStaMon*>	    cStaMon::classList;
bool		    cStaMon::configFound=FALSE;
long		    cStaMon::frequency;
int		    cStaMon::numResources=NULL_RES_NUM;
bool       	    cStaMon::cmSimulate = FALSE;
bool       	    cStaMon::simStop = FALSE;
pthread_t	    cStaMon::staMonThread=0;
char		    *cStaMon::staResInUse=NULL;
char		    *cStaMon::staResFullName=NULL;
char		    *cStaMon::staResName=NULL;
char		    *cStaMon::staResNum=NULL;
char		    *cStaMon::staResMax=NULL;
char		    *cStaMon::staResType=NULL;
char		    *cStaMon::staResUnits=NULL;
char		    *cStaMon::staResUsed=NULL;
bool		    cStaMon::stopping=FALSE;
 

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::cStaMon
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Constructor for the database and disk space monitor.
//
// DESCRIPTION:
// Trivial
//
// EXTERNAL VARIABLES:
// (boolean)	cStaMon::stopping
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

   	cStaMon::cStaMon
(
)
{
    frequency = 120;
    classList.push_back( this );
    
    lockCheckRes();
    stopping = FALSE;
    unlockCheckRes();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::aliasConfig
//
// INVOCATION:
// cStaMon::aliasConfig( numTokens, tokens );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens    (int) 	Number of tokens on the config. line.
// (>) tokens	    (char**)	List of tokens from the config. line.
//
// FUNCTION VALUE:
// (bool) : False if an error occurs
// (bool) : True if no errors occur.
//
// PURPOSE:
// Find the channel sliases given for the EPICS resource data records.
//
// DESCRIPTION:
// If the channel name (token[2]) matches one of:
//	resource%d:inUse
//	resource%d:fullName
//	resource%d:name    
//	resource%d:max
//	resource%d:type
//	resource%d:units
//	resource%d:used
// and the subsystem identifier (token[1]) is "-" then store its
// alias (token[3]).
//
// EXTERNAL VARIABLES:
// cStaMon::staResNum	    (int)	// The current resource number
// cStaMon::staResfullName   (char*)	// resource%d:fullName alias
// cStaMon::staResInUse	    (char*)	// resource%d:inUse alias
// cStaMon::staResName	    (char*)	// resource%d:name alias
// cStaMon::staResMax	    (char*)	// resource%d:max alias
// cStaMon::staResType	    (char*)	// resource%d:type alias
// cStaMon::staResUnits	    (char*)	// resource%d:units alias
// cStaMon::staResUsed	    (char*)	// resource%d:used alias
//
// PRIOR REQUIREMENTS:
// The configuration files must be opened with config_open
//
// SEE ALSO:
// cStaMon::config
//
// DEFICIENCIES:
// Assumes that the resources are specified as indexed channels,
// "resource%d:???" when they could all be specified, "resource0:???"
//
// Only handles the "DHSS" records and should handle both "DHSS" and
// "DHSB" record.
//-
//***********************************************************************
//

boolean		cStaMon::aliasConfig
(
    int		numTokens, 		// (in) Number of tokens.
    char**	tokens			// (in) list of tokens.
)
{
    boolean	ret;			// Return status.
    cStaStat	status;			// Status code.
    
    ret = TRUE;
    
    if ( numTokens != 5 )
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }
    else
    {
	if ( ( strncmp( tokens[2], STA_RES_PREFIX, STA_RES_LEN ) == 0 ) 
		&& ( strcmp( tokens[1], STA_RES_SUB ) == 0 ) )
	{
	    if( strcmp( tokens[2], STA_RES_SIZE ) == 0 )
	    {
		//
		// Channel alias for resourceArraySize
		//
		
		if ( staResNum  != NULL )
		{
		    delete staResNum;
		    staResNum = NULL;
		}
		checkNull( staResNum = strsav( tokens[3] ), status,
			return( FALSE ) );
	    }
	    
	    if ( strcmp( tokens[2]+STA_RES_LEN+2, STA_RES_INUSE ) == 0 )
	    {
		//
		// Channel alias for resource%d:inUse
		//

		if ( staResInUse != NULL )
		{
		    delete staResInUse;
		    staResInUse = NULL;
		}
		checkNull( staResInUse = strsav( tokens[3] ), status,
			return( FALSE ) );
	    }
	    if ( strcmp( tokens[2]+STA_RES_LEN+2, STA_RES_MAX ) == 0 )
	    {
		//
		// Channel alias for resource%d:max
		//

		if ( staResMax != NULL )
		{
		    delete staResMax;
		    staResMax = NULL;
		}
		checkNull( staResMax = strsav( tokens[3] ), status,
			return( FALSE ) );
	    }
	    if ( strcmp( tokens[2]+STA_RES_LEN+2, STA_RES_FULLNAME ) == 0 )
	    {
		//
		// Channel alias for resource%d:name
		//

		if ( staResFullName != NULL )
		{
		    delete staResFullName;
		    staResFullName = NULL;
		}
		checkNull( staResFullName = strsav( tokens[3] ), status,
			return( FALSE ) );
	    }
	    if ( strcmp( tokens[2]+STA_RES_LEN+2, STA_RES_NAME ) == 0 )
	    {
		//
		// Channel alias for resource%d:name
		//

		if ( staResName != NULL )
		{
		    delete staResName;
		    staResName = NULL;
		}
		checkNull( staResName = strsav( tokens[3] ), status,
			return( FALSE ) );
	    }
	    if ( strcmp( tokens[2]+STA_RES_LEN+2, STA_RES_TYPE ) == 0 )
	    {
		//
		// Channel alias for resource%d:type
		//

		if ( staResType != NULL )
		{
		    delete staResType;
		    staResType = NULL;
		}
		checkNull( staResType = strsav( tokens[3] ), status,
			return( FALSE ) );
	    }
	    if ( strcmp( tokens[2]+STA_RES_LEN+2, STA_RES_UNITS ) == 0 )
	    {
		//
		// Channel alias for resource%d:units
		//

		if ( staResUnits != NULL )
		{
		    delete staResUnits;
		    staResUnits = NULL;
		}
		checkNull( staResUnits = strsav( tokens[3] ), status,
			return( FALSE ) );
	    }
	    if ( strcmp( tokens[2]+STA_RES_LEN+2, STA_RES_USED ) == 0 )
	    {
		//
		// Channel alias for resource%d:used
		//

		if ( staResUsed != NULL )
		{
		    delete staResUsed;
		    staResUsed = NULL;
		}
		checkNull( staResUsed = strsav( tokens[3] ), status,
			return( FALSE ) );
	    }
	}
	
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::checkAliases
//
// INVOCATION:
// cStaMon::checkChAlias( alias, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) alias	(tStaCa &)	EPICS channel alias structure.
// (!) status	(cStaStat &)	Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To verify the correctness of a channel alias.
//
// DESCRIPTION:
// Try and find the channel for each alias in the channel alias
// structure.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMon::checkAliases
(
    tStaCa	&aliases,	// (in)  EPICS Channel alias to be checked.
    cStaStat	&status		// (mod) Function return value.
)
{
    cStaChannel	*channel;	// EPICS channel.
    

    if ( (  channel = cStaChannel::find( aliases.name, status ) ) == NULL )
    {
	status.E_BAD_CHALIAS( status, aliases.name );
    }

    if ( (  channel = cStaChannel::find( aliases.fullName, status ) ) == NULL )
    {
	status.E_BAD_CHALIAS( status, aliases.fullName );
    }

    if ( (  channel = cStaChannel::find( aliases.type, status ) ) == NULL )
    {
	status.E_BAD_CHALIAS( status, aliases.type );
    }

    if ( (  channel = cStaChannel::find( aliases.units, status ) ) == NULL )
    {
	status.E_BAD_CHALIAS( status, aliases.units );
    }

    if ( (  channel = cStaChannel::find( aliases.max, status ) ) == NULL )
    {
	status.E_BAD_CHALIAS( status, aliases.max );
    }

    if ( (  channel = cStaChannel::find( aliases.used, status ) ) == NULL )
    {
	status.E_BAD_CHALIAS( status, aliases.used );
    }

    if ( (  channel = cStaChannel::find( aliases.inUse, status ) ) == NULL )
    {
	status.E_BAD_CHALIAS( status, aliases.inUse );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::cleanup
//
// INVOCATION:
// cStaMon::cleanup( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!)	status	(cStaStat)	Function return Status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To cleanup all used memory and wait for the monitor thread to
// join this thread.
//
// DESCRIPTION:
// Wait for the monitor thread to join this thread.
// Cleanup, free, all memory used.
//
// EXTERNAL VARIABLES:
// cStaMon:staMonThread (pthread_t)	// Monitoring thread.
// cStaMon:staResNum	(int)		// The current resource number
// cStaMon:staResInUse	(char*)		// resource%d:inUse alias
// cStaMon:staResName	(char*)		// resource%d:name alias
// cStaMon:staResMax	(char*)		// resource%d:max alias
// cStaMon:staResType	(char*)		// resource%d:type alias
// cStaMon:staResUnits	(char*)		// resource%d:units alias
// cStaMon:staResUsed	(char*)		// resource%d:used alias
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMon::cleanup
(
    cStaStat	&status		// (mod) Function return status.
)
{

    if( staMonThread != 0 )
    {
	if ( pthread_join( staMonThread, NULL ) != 0  )
	{
	    status.E_MON_THREAD( status );
	}
    
    }
    staMonThread = 0;
    
    
    if ( staResNum != NULL )
    {
	delete staResNum;
	staResNum = NULL;
    }

    if ( staResInUse != NULL )
    {
	delete staResInUse;
	staResInUse = NULL;
    }
    
    if ( staResName != NULL )
    {
	delete staResName;
	staResName = NULL;
    }
    if ( staResMax != NULL )
    {
	delete staResMax;
	staResMax = NULL;
    }
    if ( staResType != NULL )
    {
	delete staResType;
	staResType = NULL;
    }
    if ( staResUnits != NULL )
    {
	delete staResUnits;
	staResUnits = NULL;
    }
    if ( staResUsed != NULL )
    {
	delete staResUsed;
	staResUsed = NULL;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::config
//
// INVOCATION:
// cStaMon::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the monitoring space parameters from the config file.
//
// DESCRIPTION:
// Read the frequency the resources are to be monitored at.
//
// Note that it is not an error if the frequency is not found in
// configuration file.  A default value of "120" seconds will be
// used if it has not been found.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open
//
// SEE ALSO:
// cStaMon::freqConfig
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMon::config
(
    cStaStat	&status 	// (mod) Function return value.
)
{
    checkStat( status, return );

    if ( ( config_get( FREQ_KEYWORD, freqConfig ) ) != GC_SUCCESS )
    {
	frequency = 120;
    }


    if ( ( config_get( CHANNEL_KEYWORD, aliasConfig ) ) != GC_SUCCESS )
    {
	status.E_CONFIG( status );
    }
    checkStat( status, return );
    
    if ( staResFullName == NULL || staResInUse == NULL ||
	staResName == NULL || staResNum == NULL ||
	staResMax == NULL || staResType == NULL ||
	staResUnits == NULL || staResUsed == NULL )
    {
	status.E_MON_CONFIG( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::configAll
//
// INVOCATION:
// cStaMon::configAll( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat)		Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To read the configuration parameters for all resources.
//
// DESCRIPTION:
// For each instance of the cStaMon class call the config routine,
// this includes all subclasses.
//    
// EXTERNAL VARIABLES:
// cStaMon::
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open
//
// SEE ALSO:
// virtual config method for this class and all its subclasses.
//
// DEFICIENCIES:
// Should make sure that it calls the config method for cStaMon first.
//-
//***********************************************************************
//

void	cStaMon::configAll
(
    cStaStat	&status	      	// (mod) Function return status.
)
{
    checkStat( status, return );


    //
    //	Call the config method for each subclass.
    //
    
    list<cStaMon*>::iterator i;

    for( i = classList.begin(); i != classList.end(); i++ )
    {
	( (cStaMon*) (*i) )->config( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// freqConfig
//
// INVOCATION:
// freqConfig( numTokens, tokens );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens (int) 	    Number of tokens in config file.
// (>) tokens	 (char**)   List of tokens from config file.
//
// FUNCTION VALUE:
// ( bool ) false : if an error occurs.
// ( bool ) true  : if no errors occur.
//
// PURPOSE:
// Determine the frequency at which the DHS resources are to be
// monitored, default value is 120.    
// 
//
// DESCRIPTION:
// Read and store the frequency at which the DHS resources are to
// be monitored.
//
// This function intended caller is config_get.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open
//
// SEE ALSO:
// cStaMon::config
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean	cStaMon::freqConfig
(
    int		numTokens,	// (in)  Number of tokens.
    char**	tokens		// (in)  List of tokens.
)
{
    boolean	ret;		// Function return value.
    cStaStat	status;		// Status code.

    
    if ( numTokens != 2 ) 
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }
    else
    {
	if ( !str2long( tokens[1], &frequency ) )
	{
	    status.E_CONFIG_TYPE( status, tokens[0], "integer" );
	    ret = FALSE;
	}
	else
	{
	    ret = TRUE;
	}
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::getAliases
//
// INVOCATION:
// chAliases = cStaMon::getAliases( resNum, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) resNum	(int)		Resource number for the aliases.
// (!) status	(cStaStat)	Function return value.
//
// FUNCTION VALUE:
// (tSaCa*) : If all channell aliases are successfully created.
// NULL	    : If an error has occured. 
//
// PURPOSE:
// To find all the channel names for a given resource number.
//
// DESCRIPTION:
// For each channel, inUse, name, max, type, units, and used
// create the particular string expresion and store it.
//
// EXTERNAL VARIABLES:
// cStaMon:staResInUse	(char*)		// resource%d:inUse alias
// cStaMon:staResName	(char*)		// resource%d:name alias
// cStaMon:staResMax	(char*)		// resource%d:max alias
// cStaMon:staResType	(char*)		// resource%d:type alias
// cStaMon:staResUnits	(char*)		// resource%d:units alias
// cStaMon:staResUsed	(char*)		// resource%d:used alias
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

tStaCa	*cStaMon::getAliases
(
    int		resNum,		// (in)  Resource number.
    cStaStat	&status		// (mod) function return status.
)
{
    tStaCa	*newCA;		// Set of aliases for the res. num.
    
    checkStat( status, return( NULL ) );

    checkNull( newCA = new tStaCa, status, return ( NULL ) );

    if ( staResInUse != NULL ) {
	sprintf( newCA->inUse, staResInUse, resNum );
    }

    if ( staResMax != NULL )
    {
	sprintf( newCA->max, staResMax, resNum );
    }

    if ( staResName != NULL )
    {
	sprintf( newCA->name, staResName, resNum );
    }

    if ( staResName != NULL )
    {
	sprintf( newCA->fullName, staResFullName, resNum );
    }

    if ( staResType != NULL )
    {
	sprintf( newCA->type, staResType, resNum );
    }
    
    if ( staResUnits != NULL )
    {
	sprintf( newCA->units, staResUnits, resNum );
    }
    
    if ( staResUsed != NULL )
    {
	sprintf( newCA->used, staResUsed, resNum );
    }
    return( newCA );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::monitor
//
// INVOCATION:
// cStaMon::monitor( cStaMon )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) arg	(void*)	instance of a cStaMon class.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To control the monitoring of DHS resources.
//
// DESCRIPTION:
// A condition variable ( cCond ), and a boolean are used to control
// control he frequency the resources are monitored.  The conditional
// variables timed wait method is executed.  This is either interupted
// when the timer goes of or when "stopping" is set to TRUE.  
// "stopping" is set to TRUE only when the "stop" method is called.
// If monitoring is not stopping then after the timed wait the resource
// are queried for their new max and used values.
//    
// This method should be run in a separate thread, by calling
// "pthead_create".
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// Configuration file should be read, but not necessary
//
// SEE ALSO:
// cStaMon::start
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//
    
void 	*cStaMon::monitor
(
    void	*arg		// (in)  Pointer to a cStaMon object.
)
{
    cStaMon 	*pStaMon;	// cStaMon object.
    cStaStat	status;		// function return status.

    pStaMon = ( cStaMon *)arg;

    while( ! pStaMon->stopping && ! simulate() )
    {
	//
	// Continue monitoring, since we aren't stopping 
	//
	
	pStaMon->lockCheckRes();
	while( ! pStaMon->checking && ! pStaMon->stopping && ! simulate() )
	{
	    //
	    // Wait until checking turns TRUE to check the resources
	    // or until the time is current time + frequency of
	    // monitoring
	    //
	    
	    clock_gettime( CLOCK_REALTIME, &(pStaMon->absTime) );
	    pStaMon->absTime.tv_sec += pStaMon->frequency;
	    if ( pStaMon->stopping || simulate() ) {
		//
		// Since stopping could have changed value since
		// the beginning of the loop verify that we aren't
		// stopping, if we are break out of the inner
		// while loop and don't monitor the resources.
		//
		
		break;
	    }
	    pStaMon->timedWaitCheckRes();
	    
		
	    //
	    // Set checking to TRUE so we go past the end of
	    // the inner while loop and check the resources.
	    //
	    
	    checking = TRUE;
	}
	pStaMon->unlockCheckRes();
	    
	if ( !pStaMon->stopping && checking && !simulate() )
	{
	    //
	    // Update the resource values if we aren't stopping
	    // monitoring and not in simulation mode.
	    //
	    
	    pStaMon->updateAll( status );
	}
	pStaMon->lockCheckRes();
	checking = FALSE;
	pStaMon->unlockCheckRes();
    }

    pthread_exit( NULL );
    return( NULL );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::start
//
// INVOCATION:
// cStaMon::start();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat&)	Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To start the monitoring of the various spaces.
//
// DESCRIPTION:
// 1. If currently monitoring then stop it.   
// 2. Display a message, indicating that the monitoring is starting.
// 3. Call the initialization routine for each cStaMon object.
// 4. Start the monitoring in a separate thread.
//
// EXTERNAL VARIABLES:
// cStaMon::monThread
//
// PRIOR REQUIREMENTS:
// The config file should be read before starting.
//
// SEE ALSO:
// cStaMon::stop()
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMon::start
(
    cStaStat 	&status		// Function return status.
)
{
    char	chAlias[40];	// EPICS channel alias.
    cStaStat	chStatus;	// Status used for checking EPICS.
    cStaMon	*pMonitor;	// Monitoring object.       
    char	strResNum[4];	// Number of resources in string format.
    

    checkStat( status, return );
    

    //
    // Call each objects init method.
    //
    
    list<cStaMon*>::iterator i;
    for( i = pMonitor->classList.begin(); i != pMonitor->classList.end();
	 i++ )
    {
	( (cStaMon*) (*i) )->init( status );
    }
    
    
    //
    // Update the resource array number.
    //

    sprintf( strResNum, "%d", MAX_RESOURCES );
    cStaStat::update( staResNum, strResNum, chStatus );

    
    //
    // Initialize the InUse values of unused resources.
    //

    for( int j = 0; j < MAX_RESOURCES; j++ )
    {
	sprintf( chAlias, staResInUse, j );
	if ( j <= numResources )
	{
	    cStaStat::update( chAlias, "1", chStatus );
	}
	else
	{
	    cStaStat::update( chAlias, "0", chStatus );
	}
    }
    cStaStat::flush( status );


    lockCheckRes();
    stopping = FALSE;
    checking = TRUE;
    unlockCheckRes();    


    //
    // Display a message and start the monitoring..
    //

    if ( configFound && ! simulate() )
    {
	status.S_MONITORING( status );
	i = pMonitor->classList.begin();
	( (cStaMon*) (*i) )->startThread( status );
    }
    else
    {
	status.S_NO_MONITORING( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::stop
//
// INVOCATION:
// cStaMon::stop();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To stop the monitoring of the various spaces.
//
// DESCRIPTION:
// 1. Change the stopping variable to TRUE and the checking value
//    to FALSE and broadcast the change.
// 2. Display a "stopping" message.
// 3. Call the cleanup routine for each object, after the monitoring
//    thread has joined this one..
//
// EXTERNAL VARIABLES:
// cStaMon::checking
// cStaMon::stopping
//
// PRIOR REQUIREMENTS:
// The monitoring of space should have been initiated with
// cStaMon::start()
//
// SEE ALSO:
// cStaMon::start()
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMon::stop
(
)
{
    cStaMon	*pMonitor;
    cStaStat	status;		//  Function return value.


    //
    // Change the stopping value to TRUE to indicate that we are
    // stopping and changing check to FALSE so we don't check the
    // resources and broadcast these changes.
    //

    lockCheckRes();
    stopping = TRUE;
    checking = FALSE;
    unlockCheckRes();    
    broadcastCheckRes();


    //
    // Call the cleanup method for each object after the monitoring
    // thread has joined this one.
    //


    if ( simStop ) {
	simStop = FALSE;
    }
    else
    {
	list<cStaMon*>::iterator i;
	for( i = pMonitor->classList.begin(); i != pMonitor->classList.end();
	     i++ )
	{
	    ( (cStaMon*) (*i) )->cleanup( status );
	}
    }


    //
    // Display a message stating that space monitoring is stopping.
    //

    status.S_STOP_MONITOR( status );
}    

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::str2long
//
// INVOCATION:
// cStaMon::str2long( string, longInt)
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) string	(char*)		// Character string to be converted.
// (<> longInt	(long*)		// The string as a long int.
//
// FUNCTION VALUE:
// (boolean) : True if no errors occure
// (boolean) : False if an error occurs.
//
// PURPOSE:
// To convert a string to a long integer.
//
// DESCRIPTION:
// Verify that the string is a long value and do the conversion.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// This method should be part of the gen library.
//-
//***********************************************************************
//

boolean	cStaMon::str2long
(
    char	*string,	// (in)  String to be converted.
    long	*longInt	// (out) Converted string.
)
{
    char	*end;		// Where the conversion stopped.
    boolean	ret;		// Function return value.
    
    *longInt = 0;
    ret = TRUE;

    
    //
    // Convert the string to a long int.
    //

    *longInt = strtol( string, &end, 0 );

    
    //
    // Check for errors.
    //

    if ( *longInt == LONG_MAX && errno == ERANGE )
    {
	ret = FALSE;
    }

    if ( *longInt == LONG_MIN && errno == ERANGE )
    {
	ret = FALSE;
    }

    if ( *end != '\0' && isspace( *end ) )
    {
	ret = FALSE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::testAll
//
// INVOCATION:
// cStaMon::testAll( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat)	Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To test the monitoring resources.
//
// DESCRIPTION:
// Call the test method for each cStaMon object.
//
// EXTERNAL VARIABLES:
// cStaMon::classList
//
// PRIOR REQUIREMENTS:
// None
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMon::testAll
(
    cStaStat	&status	      	// (mod) Function return status.
)
{
    cStaMon	*monitor;	// Monitoring object.
    cStaStat	s;		// Internal status.
    boolean		ok;	// test ok?
    
    checkStat( status, return );

    ok = TRUE;

    //
    //  Call the test method for each object.
    //
    
    list<cStaMon*>::iterator i;
    for( i = monitor->classList.begin(); i != monitor->classList.end(); i++ )
    {
	( (cStaMon*) (*i) )->test( s );
	if ( !s.ok() )
	{
	    s.S_SUCCESS( s );
	    status+=s;
	    ok = FALSE;
	}
    }
    
    if ( ! ok )
    {	
	//
	// If at least one test failed set the status to
	// indicate errors in the resource monitoring test.
	//
	
	status.E_MON_TEST_FAIL( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::updateAll
//
// INVOCATION:
// cStaMon::updateAll( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To update the EPICS values for all the items being monitored by the
// status server.
//
// DESCRIPTION:
// 1. Verify that the monitoring should be done, monitoring is not
//    stopping and we aren't in simulate mode.
// 2. Call the updateRes method for each object.
//
// EXTERNAL VARIABLES:
// cStaMon::stopping
// cSta::checking
//
// PRIOR REQUIREMENTS:
// None
//
// SEE ALSO:
// updateRes for each object.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMon::updateAll
(
    cStaStat	&status	      	// Function return status.
)
{
    checkStat( status, return );

    if ( stopping || ! checking || simulate() )
    {
	//
	// If monitoring is stopping, or cheking of resouces should
	// not be occureing, or if in simulation mode then return.
	//

	return;
    }


    //
    // Call updateRes for each object.
    //
    
    list<cStaMon*>::iterator i;
    for( i = classList.begin(); i != classList.end(); i++ )
    {
	( (cStaMon*) (*i) )->updateRes( status );
    }
    cStaStat::flush( status );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::updateEpics
//
// INVOCATION:
// cStaMon::updateEpics( data, dataType, chAlias, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) data	(void*)		The new value.
// (>) dataType (DHS_DT_TYPE)	The data type.
// (>) chAlias  (const char*)	Channel alias to be updated.
// (!) status   (cStaStat&)	Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Update an DHS SAD EPICS data record with the new value given.
//
// DESCRIPTION:
// 1. Convert the value from the given data type to a string.
// 2. Find the EPICS channel and send the new value.
//
// EXTERNAL VARIABLES:
// None. 
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None
//-
//***********************************************************************
//

void	cStaMon::updateEpics
(
    const void	    *data,	// (in)  The new value.
    DHS_DATA_TYPE   dataType,	// (in)  The data type.
    const char	    *chAlias,	// (in)  Channel alias to be updated.
    cStaStat	    &status	// (mod) Function return status.
)
{
    cStaStat		chStatus;	// Channel Access return value.
    char		*value;
    

    checkStat( status, return );

    if ( data == NULL || chAlias == NULL )
    {
	status.E_BAD_EPICS_VAL( status );
	return;
    }


    //
    // Convert the value to a string.
    //

    switch( dataType )
    {
	case DHS_DT_BOOLEAN:
	    checkNull( value = (char*) char_alloc( 2 ), status, return )
	    if ( *((boolean*)data) == TRUE )
	    {
		strcpy( value, "T\0");
	    }
	    else
	    {
		strcpy( value, "F\0");
	    }
	    break;
	case DHS_DT_CHAR:
	    checkNull( value = (char*) char_alloc( 2 ), status, return )
	    value[0] = *((char*) data );
	    value[1] = '\0';
	    break;
	case DHS_DT_STRING:
	    checkNull( value = strsav( (char*)data ), status, return )
	    break;
	case DHS_DT_INT8:
	    checkNull( value = (char*) char_alloc( 50 ), status, return );
	    sprintf( value, "%-hd", *((short*)data) );
	    break;
	case DHS_DT_UINT8:
	    checkNull( value = (char*) char_alloc( 50 ), status, return );
	    sprintf( value, "%-hu", *((unsigned short*)data) );
	    break;
	case DHS_DT_INT16:
	    checkNull( value = (char*) char_alloc( 50 ), status, return );
	    sprintf( value, "%-d", *((int*)data) );
	    break;
	case DHS_DT_UINT16:
	    checkNull( value = (char*) char_alloc( 50 ), status, return );
	    sprintf( value, "%-u", *((unsigned int*)data) );
	    break;
	case DHS_DT_INT32:
	    checkNull( value = (char*) char_alloc( 50 ), status, return );
	    sprintf( value, "%-ld", *((long*)data) );
	    break;
	case DHS_DT_UINT32:
	    checkNull( value = (char*) char_alloc( 50 ), status, return );
	    sprintf( value, "%-lu", *( (unsigned long*)data) );
	    break;
	case DHS_DT_FLOAT:
	case DHS_DT_DOUBLE:
	    checkNull( value = (char*) char_alloc( 50 ), status, return );
	    sprintf( value, "%-f", *((long*)data) );
	    break;
	default:
	    status.E_BAD_EPICS_VAL( status );
	    break;
    }


    
    //
    // Update the channel with the new value.
    //
    

    if ( value != NULL )
    {
	cStaStat::update( chAlias, value, chStatus );
	delete value;
	value = NULL;
    }
   

   
   //
   // If the status is bad change the status to success, The other
   // channels may be fine.
   //
   
   if ( !status.ok() )
   {
       status.S_SUCCESS( status );
   }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::checkDatabase
//
// INVOCATION:
// cStaMonDb::checkDatabase( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output);
// (>) dbName	(char*)			Name of the database.
// (!) status	(cStaStat&)		Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To verify the existence and access to a database.
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// Connection to the database has been open.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonDb::checkDatabase
(
    char	*dbName,	// (in)  Name of the database.
    cStaStat	&status		// (mod) Function return value.
)
{
    DBPROCESS	*dbProc;	// The DB process..
    cStaStat	tStatus;	// Temporary status.
    

    checkStat( status, return );

    //
    //  Get the connection and see if we can use the given database.
    //
    
    if ( ( dbProc = dbConnGet( tStatus ) ) != NULL )
    {
	if ( dbuse( dbProc, dbName ) == FAIL )
	{
	    tStatus.E_SDB( tStatus, "dbuse( dbProc, dbName )" );
	    dbConnRelease();
	    return;
	}
    }
    else
    {
	tStatus.E_SDB( status, "dbuse( dbProc, dbName )" );
    }
    dbConnRelease();
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::config
//
// INVOCATION:
// cStaMonDb::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat)	// Function return value.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the configuration parameters for monitoring databases.
//
// DESCRIPTION:
// Read and store the name of each database and their respective
// channel aliases.  Also find which server the databases are on.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open
//
// SEE ALSO:
// cStaMonDb::db
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonDb::config
(
    cStaStat	&status		// (mod) Function return status.
)
{
    int		configStatus;	// Return value from config_get


    checkStat( status, return );

    cStaMon::config( status );
    
    //
    // Load information from config file, if there isn't any 
    // monitoring information it is not an error.
    //

    if ( ( configStatus = config_get( DB_SERVER_KEYWORD, serverName ) )
	!= GC_SUCCESS )
    {
	    status.E_CONFIG( status );
    }
    checkStat( status, return );    

    
    if ( ( configStatus =  config_get( DB_MON_KEYWORD, db ) )
	!= GC_SUCCESS )
    {
	if ( configStatus != GC_E_KEY_NOT_FOUND )
	{
	    status.E_CONFIG( status );
	}
	checkStat( status, return );
    }
    else
    {
	//
	// Something was found.
	//

	configFound = true;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::cleanup
//
// INVOCATION:
// cStaMonDb::cleanup( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!)	status	(cStaStat)	Function return Status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To cleanup all used memory and close the database connection.
//
// DESCRIPTION:
// Close the database connection.  Free the memory used in the
// list of databases.
//
// EXTERNAL VARIABLES:
// cStaMonDb::dbList
// cStaMonDb::dbConnection
//
// PRIOR REQUIREMENTS:
// Connection to the database is open.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonDb::cleanup
(
    cStaStat	&status
)
{
    cStaMon::cleanup( status );
        
    //
    // Close the connection to the database.
    //

    if ( dbConnection != NULL )
    {
	
	dbcanquery( dbConnection );
	db_close( dbConnGet( status ) );
	dbConnection = NULL;
	db_exit();
	dbConnRelease();
    }
    
    
    //
    //  Erase all information stored from reading config file previously.
    //

    list<tStaDb*>::iterator	i;
    for( i = dbList.begin(); i != dbList.end(); i++ )
    {
	if ( ((tStaDb*)(*i))->aliases != NULL )
	{
	    delete ((tStaDb*)(*i))->aliases;
	    ((tStaDb*)(*i))->aliases = NULL;
	}
	if ( ((tStaDb*)(*i))->name != NULL )
	{
	    delete ((tStaDb*)(*i))->name;
	    ((tStaDb*)(*i))->name = NULL;   
 	}
	if ( ((tStaDb*)(*i))->dbName != NULL )
	{
	    delete ((tStaDb*)(*i))->dbName;
	    ((tStaDb*)(*i))->dbName = NULL;
	}

	delete *i;
	*i = NULL;
	delResource();
    }
    dbList.erase( dbList.begin(), dbList.end() );

    if ( dbServerName != NULL )
    {
	delete dbServerName;
	dbServerName = NULL;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::db
//
// INVOCATION:
// cStaMonDb::db( numTokens, tokens );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens    (int) 	Number of tokens on the config. line.
// (>) tokens	    (char**)	List of tokens from the config. line.
//
// FUNCTION VALUE:
// (bool) : False if an error occurs
// (bool) : True if no errors occur.
//
// PURPOSE:
// Read the dbName and a descriptor for each database, a
// given line in the config file.
//
// DESCRIPTION:
// 1. Check to make sure there are exactly three tokens.
// 2. Create the new database information structure with the database name
//      and description from the config file.
// 3. Initialize the max and used values to 0.
// 4. Create the channel aliases for this resource, if possible.
// 5. Add the new database structure to the list of databases.
//
// EXTERNAL VARIABLES:
// cStaMonDb::dbList
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open
//
// SEE ALSO:
// cStaMon::config
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean		cStaMonDb::db
(
    int		numTokens, 		// (in) Number of tokens.
    char**	tokens			// (in) list of tokens.
)
{
    tStaDb	*db;			// Magnetic disk info.
    int		resNum;			// Resource Number.
    boolean	ret;			// Return value.
    cStaStat	status;			// Function return value.
    

    if ( numTokens != 3 )
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }
    else
    {
	//
	// Create new database information and save the name and
	// the database name.
	//
	
	checkNull( db = new tStaDb, status, return( FALSE ) );

	checkNull( db->name = (char *) strsav( tokens[1] ), status,
		return( FALSE ) );

	checkNull( db->dbName = (char *) strsav( tokens[2] ), status,
		return( FALSE ) );
	//
	// Use the maximum space and used space to zero.
	//
	
	db->max = NULL_SPACE;
	db->used = NULL_SPACE;


	//
	// If not all resource records are used then set up the
	// channel alias for this record.
	//
	   
	if ( ( resNum = newResource() ) != NULL_RES_NUM )
	{
	    checkNull( db->aliases = getAliases( resNum, status ),
    		    status, VOID );
	    if ( !status.ok() )
	    {
		delResource();
		return( FALSE );
	    }
	}
	else
	{
	    db->aliases = NULL;
	}
	
	dbList.push_back( db );
	ret = TRUE;
	}
	
    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::init
//
// INVOCATION:
// cStaMonDb::init( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!)	status	(cStaStat)	Function return Status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To open a connection to the database.
//
// DESCRIPTION:
// Trivial
//
// EXTERNAL VARIABLES:
// cStaMonDb::dbServerName
// cStaMonDb::dbConnection
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonDb::init
(
    cStaStat	&status
)
{
    cStaStat	tStatus;		// Temporary Status.
    
    checkStat( status, return );

    cStaMon::init( status );
    

    //
    // Initialize thd DB library and open a connection to the database.
    //
    
    checkCadcDb( db_init( "Status Server" ), tStatus, dbConnection = NULL );
    if ( tStatus.ok() )
    {
	checkCadcDb( db_open( dbServerName, NULL, NULL, "master", FALSE,
		&dbConnection ), tStatus, dbConnection = NULL );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::serverName
//
// INVOCATION:
// cStaMonDb::serverName( numTokens, tokens );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens    (int) 	Number of tokens on the config. line.
// (>) tokens	    (char**)	List of tokens from the config. line.
//
// FUNCTION VALUE:
// (bool) : False if an error occurs
// (bool) : True if no errors occur.
//
// PURPOSE:
// Read the serverName from the configuration file.
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// cStaMonDb::dbServerName
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open
//
// SEE ALSO:
// cStaMonDb::config
//
// DEFICIENCIES:
// Does not ensure that the server name has occured once in the
// configuration file.
//-
//***********************************************************************
//

boolean		cStaMonDb::serverName
(
    int		numTokens, 		// (in) Number of tokens.
    char**	tokens			// (in) list of tokens.
)
{
    boolean	ret;			// Return value.
    cStaStat	status;			// Function status code.

    if ( numTokens != 2 )
    {
	//
	// Error if there is anything but two tokens.
	//
	
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }
    else
    {
	//
	// Record the server name found in the configuration file.
	//
	
	checkNull( dbServerName = (char *) strsav( tokens[1] ),
		status, return( FALSE ) );
	ret = TRUE;
    }

    return( ret );
}
    
//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::test
//
// INVOCATION:
// cStaMonDb::test( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat&)		// Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To test the monitoring of the database resources.
//
// DESCRIPTION:
// For each database verify the database and check the EPICS channels
// associated with the database.
//
// An internal status is used.  This is done so that all databases
// can be verified.  The "check" routines do not perform the checks
// if the status is not ok.
//    
// EXTERNAL VARIABLES:
// cStaMonDb::dbList
//
// PRIOR REQUIREMENTS:
// The connection to the database is open.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonDb::test
(
    cStaStat	&status		// (mod) function return value.
)
{
    cStaStat 	s;		// Internal status indicator.
    boolean	ok;		// Test was ok?
    

    checkStat( status, return );
    
    cStaMon::test( status );

    ok  = TRUE;

    list<tStaDb*>::iterator	i;
    for( i = dbList.begin(); i != dbList.end(); i++ )
    {
	checkDatabase( ((tStaDb*) (*i))->dbName, s );
	if ( !s.ok() )
	{
	    ok = FALSE;
	    status+=s;
	    s.S_SUCCESS( s );
	}
	checkAliases( *(((tStaDb*) (*i))->aliases), s );
	if ( !s.ok() )
	{
	    ok = FALSE;
	    status+=s;
	    s.S_SUCCESS( s );
	}
    }

    if ( !ok )
    {
	status.E_DB_TEST_FAIL( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::update
//
// INVOCATION:
// cStaMonDb( db, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) db	    (database&)	       	Database information.
// (!) status	    (cStaStat) 		Function return value.	
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To examine that database resources and update the EPICS SAD
// with the values found.
//
// DESCRIPTION:
// 1. Get the connection and find the database.
// 2. Add the "sp_spaceused" command to the databases command buffer.
// 3. Execute the "sp_spaceused" command.
// 4. Process the results.  Note that the values needed are stored in
//    the second table returned.  In order to see the values in the
//    second table all rows in the first table must be passed.
//
// There are two tables returned by SYBASE stored procedure
// "sp_space used" when no parameter is given to it.  The
// tables are as follows:
//
//  Table 1:
//
//   column 1			  column 2
//  -------------	---------------------------
//  database name	database size in mega bytes.
//   
//  Table 2:
//
//  column 1	  column 2	column 3	column 4
//  ---------	-----------	--------	--------
//  reserved	space used 	 index		unused 
//   space 	  by data	  size		space
//
//  Both tables contain one row only.  Table 2's values are in
//  kilobyates.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// Connection to the database is open.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// sp_helpdb <databaseName> may be a better stored procedure to use.
// It returns results more quickly, and dbuse databaName is not needed.
// But the results would require more processing.
//-
//***********************************************************************
//

void	cStaMonDb::update    
(
    tStaDb	&db,		// (in)  Magnetic disk structure.
    cStaStat	&status		// (mod) Function return value.	
)
{
    char		*dbInfo;	// Server Name:Database Name
    DBPROCESS		*dbProc;	// The DB process..
    int			dbRet;		// Db library return value.
    static cCond	dbTimedWait;	// Condition varible for noResult.
    int			length;		// String length.
    float		max;		// Maximum database space.
    char		*maxStr = NULL;	// String representing the max space.
    bool		noResults;	// TRUE if SYBASE has the results
    					// from the query.
    int			table;		// Counts the number of tables.
    static timespec	time;		// Current time.
    char	*	units = NULL;	// Unit value (KB, MB or GB ).
    float		used;		// Disk space available.
    char		*usedStr = NULL;// String value


    checkStat( status, return );

    
    if ( isStopping() )
    {
	return;
    }
    
    
    //
    // Issue the "sp_spaceused" command on the database.
    //

    if ( ( dbProc = dbConnGet( status ) ) != NULL )
    {
	checkSybDb( dbuse( dbProc, db.dbName ), status, max = 0  );
	checkSybDb( dbcmd( dbProc, "exec sp_spaceused" ), status, max = 0 );
	checkSybDb( dbsqlsend( dbProc ), status, max = 0 );

	//
	// Poll sybase for the results of the query.  This is a better
	// alternative than using dbsqlexec since it is non-blocking.
	// 

	noResults = true;
	while( noResults && !isStopping() )
	{
	    checkSybDb( dbpoll( dbConnection, 100, NULL, &dbRet ), status,
		    max = 0 );
	    if ( dbRet == DBRESULT )
	    {
		noResults = false;
	    }
	    else if ( isStopping() )
	    {
		dbcanquery( dbConnection );
		dbConnRelease();
		return;
	    }
	    else
	    {
		dbTimedWait.lock();
		clock_gettime( CLOCK_REALTIME, &time );
		time.tv_sec += 8;
		dbTimedWait.timedWait( time );
		dbTimedWait.unlock();
	    }
	}

	if ( isStopping() )
	{
	    dbcanquery( dbConnection );
	    dbConnRelease();
	    return;
	}
	checkSybDb( dbsqlok( dbProc ), status, max = 0 );
	    

	
	//
	// Process the results of the "sp_spaceused" command.  The values
	// needed are in the second table of results.
	//
	
	table = 0;
	while( status.ok() && ( dbRet = dbresults( dbProc ) ) == SUCCEED )
	{
	    if ( status.ok() && DBROWS( dbProc ) == SUCCEED )
	    {
		while ( status.ok() &&
		    ( dbRet = dbnextrow( dbProc ) ) != NO_MORE_ROWS &&
	    		dbRet == MORE_ROWS )
		{
		    if ( table == 0 )
		    {
			checkNull( maxStr = strnsav( (char*)dbdata( dbProc, 2 ),
				dbdatlen( dbProc, 2 ) ), status,
				dbConnRelease(); break );
			
			checkSybDb( length = dbdatlen( dbProc, 2 ), status,
			    break );
			checkNull( units = strsav( maxStr + ( length - 2 ) ),
			    status, dbConnRelease(); break );
			
			maxStr[ length - 3 ] = '\0';
			checkGen( str2float( maxStr, &max ), status, 
			    dbConnRelease(); break );
			max = convertBytes( max, units );
		    }
		    if ( table ==  1 ) 	   	   
		    {
			checkNull( usedStr = strnsav( (char*)dbdata(dbProc, 1 ),
				dbdatlen( dbProc, 1 ) ), status, 
				dbConnRelease(); break );
			checkSybDb( length = dbdatlen( dbProc, 1 ), status,
				dbConnRelease(); break  );
			
			checkNull( units = strsav( usedStr + ( length - 2 ) ),
			    status, dbConnRelease(); break );
			usedStr[ length - 3 ] = '\0';
			checkGen( str2float( usedStr, &used ), status,
				    dbConnRelease(); break );
			used = convertBytes( used, units );
		    }
		}
	    }

	    //
	    // Free memory used and increment table counter.
	    //
	    
	    if ( maxStr != NULL )
	    {
		delete maxStr;
		maxStr = NULL;
	    }
	    if ( usedStr != NULL )
	    {
		delete usedStr;
		usedStr = NULL;
	    }
	    if ( units != NULL )
	    {
		delete units;
		units = NULL;
	    }
	    table++;	
	}
    }


    
    //
    // Send values to EPICS
    //

    if ( status.ok() && db.aliases != NULL && dbProc != NULL )
    {

	//
	// Set the max and availble values.  Also release the databse
	// connection lock.
	//
    
	dbConnRelease();
	if ( max > 0 )
	{
	    db.max = (int) max;
	    db.used = (int) used;
	}
	

	updateEpics( "1", DHS_DT_STRING, db.aliases->inUse, status );
	checkNull( dbInfo = (char*)char_alloc( strlen( dbServerName ) +
		strlen( db.dbName ) + 2 ), status, VOID );
	sprintf( dbInfo, "%s:%s", dbServerName, db.dbName );
	updateEpics( (void*)dbInfo, DHS_DT_STRING, db.aliases->fullName,
		status );
	updateEpics( (void*) &db.max, DHS_DT_INT16, db.aliases->max, status );
	updateEpics( (void*)db.name, DHS_DT_STRING, db.aliases->name,
		status );
	updateEpics( "Database", DHS_DT_STRING, db.aliases->type, status );
	updateEpics( "Kilobytes", DHS_DT_STRING, db.aliases->units,
		status );
	updateEpics( (void*) &db.used, DHS_DT_INT16, db.aliases->used,
		status );
    }
    else if ( db.aliases != NULL )
    {
	status.S_SUCCESS( status );
	updateEpics( "1", DHS_DT_STRING, db.aliases->inUse, status );
	checkNull( dbInfo = (char*)char_alloc( strlen( dbServerName ) +
		strlen( db.dbName ) + 1 ), status, VOID );
	sprintf( dbInfo, "%s:%s", dbServerName, db.dbName );
	updateEpics( (void*)dbInfo, DHS_DT_STRING, db.aliases->fullName,
		status );
	updateEpics( "Unknown", DHS_DT_STRING, db.aliases->max,
		status );
	updateEpics( (void*)db.name, DHS_DT_STRING, db.aliases->name,
		status );
	updateEpics( "Database", DHS_DT_STRING, db.aliases->type, status );
	updateEpics( "Unknown", DHS_DT_STRING, db.aliases->units,
		status );
	updateEpics( "Unknown", DHS_DT_STRING, db.aliases->used,
		status );
	
    }
    else
    {
	status.E_NO_ALIASES( status, db.name );
	status.S_SUCCESS( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::updateRes
//
// INVOCATION:
// cStaMonDb::updateRes( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat)	// Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To update database resource monitoring values in the DHS SAD.
//
// DESCRIPTION:
// For each database structure find the current space usage for it.
// After updating the values flush EPICS so all values are updated.
//
// EXTERNAL VARIABLES:
// cStaMonDb::dbList
//
// PRIOR REQUIREMENTS:
// The conection to the database and EPICS are open.
//
// SEE ALSO:
// cStaMonDb::update
//
// DEFICIENCIES:
// ???
//-
//***********************************************************************
//

   void	cStaMonDb::updateRes
(
    cStaStat	&status		// (mod) Function return value.
)
{
    list<tStaDb*>::iterator	i;

    cStaMon::updateRes( status );
    
    for( i = dbList.begin(); i != dbList.end(); i++ )
    {
	update( *((tStaDb*) (*i)), status );
    }
}    

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonMd::checkPath
//
// INVOCATION:
// cStaMonMd::checkPath( path, status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) path	(const char*)	The path to be verified.
// (!) status	(cStaStat&)	The function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Determine if the given path exists.
//
// DESCRIPTION:
// Attempt to "open" the path.  If it fails then the
// path doesn't exist.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonMd::checkPath
(
    const char	*path,		// (in)  Path to be verified.
    cStaStat	&status		// (mod) function return value.
)
{
    struct stat	fileStat;	// struture with file information.
    

    checkStat( status, return );

    if ( stat( path, &fileStat ) != 0 )
    {
	//
	// Not sble to find the file so path is not good.
	//

	status.E_PATH( status, path );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonMd::config
//
// INVOCATION:
// cStaMonMd::config( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat)		// Function return value.
// 
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Read the configuration parameters for monitoring magnetic disks.
//
// DESCRIPTION:
// Read and store the paths and channel aliases for all magnetic disks
// being monitored.
//
// EXTERNAL VARIABLES:
// Trivial.
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open
//
// SEE ALSO:
// cStaMonMd::md
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonMd::config
(
    cStaStat	&status		// (mod) Function return status.
)
{
    int		configStatus;	// Return value from config_get

    
    checkStat( status, return );

	
    cStaMon::config( status );

    //
    // Load information from config file.
    //
	
    if ( ( configStatus = config_get( MD_MON_KEYWORD, md ) ) != GC_SUCCESS )
    {
	if ( configStatus != GC_E_KEY_NOT_FOUND )
	{
	    status.E_CONFIG( status );
	}
	checkStat( status, return );    
    }
    else
    {
	//
	// Something was found.
	//

	configFound = true;
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonMd::cleanup
//
// INVOCATION:
// cStaMonMd::cleanup( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!)	status	(cStaStat)	Function return Status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To cleanup all used memory.
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// cStaMonMd::dbList
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonMd::cleanup
(
    cStaStat	&status
)
{
    cStaMon::cleanup( status );

    //
    //  Erase all magnetic disk information, free memory.
    //

    list<tStaMd*>::iterator	i;
    for( i = mdList.begin(); i != mdList.end(); i++ )
    {
	if ( ((tStaMd*)(*i))->aliases != NULL )
	{
	    delete ((tStaMd*)(*i))->aliases;
	    ((tStaMd*)(*i))->aliases = NULL;
	}
	if ( ((tStaMd*)(*i))->name != NULL )
	{
	    delete ((tStaMd*)(*i))->name;
	    ((tStaMd*)(*i))->name = NULL;
	}
	if ( ((tStaMd*)(*i))->path != NULL )
	{
	    delete ((tStaMd*)(*i))->path;
	    ((tStaMd*)(*i))->path = NULL;
	}

	
	delete *i;
	*i = NULL;
	delResource();
    }
    mdList.erase( mdList.begin(), mdList.end() );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonMd::findMountPoint
//
// INVOCATION:
// char *cStaMonMd::findMountPoint( path, fileStat, status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) path	(const char*)	Directory path whose mount point is desired.
// (!) status	(cStaStat&)	Function return value.
//
// FUNCTION VALUE:
// NULL		(char*) : if a severe error occured.   
// path         (char*) : if an error occurs the path input is returned.
// mountPoint	(char*)	: if the mount point is found.
//
// PURPOSE:
// To find the mount of a given file or directory.
//
// DESCRIPTION:
// Find the status information for the file and find the current working
// directory.
// If the file is a directory move to the directory otherwise find the
// directory ( remove everything after the last slash of the path) and
// go to it.
// Traverse up the dirctory path until the mount point is found or until
// "/" is reached.
//
// This is based on the find_mount_point function in df.c of the
// fileutils-3.16 library.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// A better method could be used see CADC/cd application.
//-
//***********************************************************************
//

char	*cStaMonMd::findMountPoint
(
    const char	*path,		// (in)  Path whose mount point is desired.
    cStaStat	&status		// (mod) Function return status.
)
{
    
    int			cwdDesc = -1;	// Current working dir. file desc.
    char		*dir=NULL;	// Directory.
    struct stat		fileStat;	// File status information.
    boolean		found;		// Mount point found?
    int			last;		// String index.
    struct stat		lastStat;	// File status information for last dir.
    char		*mp=NULL;	// The mount point found.
    unsigned		pathMax;	//
    char		*ret=NULL;	// Return value from getcwd.
    struct stat		tempStat;	// Temporary file status.
    

    checkStat( status, return( NULL ) );

    if ( *path == '\0' )
    {
	status.E_PATH( status, path );
	return NULL;
    }

    checkNull( mp = (char* )char_alloc( strlen( path ) + 1 ),
	    status, return( NULL ) );
    
    
    found = FALSE;


    //
    // Try and find the file status information, if not found its an error.
    //
    
    if ( mp != NULL && stat( path, &fileStat ) != 0 )
    {
	status.E_PATH( status, path );
	delete mp;
	mp = NULL;
    }


    //
    // Find and record the current working directory.
    //

    if ( ( cwdDesc = open( ".", O_RDONLY ) ) < 0 )
    {
	status.E_CWD( status );
	mp =  NULL;
    }


    if ( mp != NULL && S_ISDIR ( fileStat.st_mode ) )
    {
	//
	// File is a directory so chdir to it.
	//

	lastStat = fileStat;
	if ( chdir ( path ) < 0 )
	{
	    status.E_PATH( status, path );
	    delete mp;
	    mp = NULL;
	}
    }
    else if ( mp != NULL )
    {
	//
	// Path is not a directory so we need to use its directory.
	//

	checkNull( dir = strsav ( (char*)path ), status, return( NULL ) );
	

	//
	// Strip trailing slashes
	//
	
	last = strlen( dir ) - 1;
	while( last > 0 && dir[last] == '/' )
	{
	    dir[last--] = '\0';
	}


	//
	// Get the directory and try to go to it.
	//
	
        strhead( dir );
	if( chdir( dir ) < 0 )
	{
	    status.E_PATH( status, dir );
	    delete mp;
	    mp = NULL;
	}

	if ( mp != NULL && stat ( ".", &lastStat ) < 0 )
	{
	    //
	    // We have reached the mount point.
	    //

	    found = TRUE;
	}
    }
    
	
    //
    // Walk up the FILE's parents until we find another file system or
    // "/", do a chdir as the path is traversed.  lastStat will hold
    // the status information for the last place visited.
    //

    for( ; mp != NULL && found == FALSE ; )
    {
	if( stat ( "..", &tempStat ) < 0 )
	{
	    found = TRUE;
	}

	if ( !found && ( tempStat.st_dev != lastStat.st_dev
		|| ( tempStat.st_ino == lastStat.st_ino ) ) )
	{
	    //
	    // Mount point has been found in cwd, so find it's name
	    //

	    pathMax = (unsigned)PATH_MAX;
	    pathMax += 2;
	    delete mp;
	    mp = NULL;
	    mp = (char*)char_alloc( pathMax );
	    while ( ( ret = getcwd( mp, pathMax ) ) == NULL
    		&& errno == ERANGE )
	    {
		pathMax += PATH_INCR;
		delete mp;
		mp = NULL;
		mp = (char*)char_alloc( pathMax );
		delete ret;
		ret = NULL;
		errno = 0;
	    }

	    if ( ret != NULL )
	    {
		found = TRUE;

	    }
	}
	else if ( !found && chdir ( ".." ) < 0 )
	{
	    found = TRUE;
	}
	else
	{
	    lastStat = tempStat;
	}
    }
    
    //
    // Restore the current working directory.
    //

    if ( fchdir( cwdDesc ) )
    {
	status.E_CWD ( status );
	delete mp;
	mp = NULL;
    }
    close( cwdDesc );
    
    if ( dir != NULL )
    {
	delete dir;
	dir = NULL;
    }

    if ( ret != NULL && ret != mp )
    {
	delete ret;
	ret = NULL;
    }
    
    
    if ( !status.ok() )
    {
	mp = NULL;
    }

    if ( mp ==  NULL )
    {
	checkNull( mp = strsav( path ), status, return( NULL ) );
    }
    
    return( mp );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonMd::md
//
// INVOCATION:
// cStaMonMd::md( numTokens, tokens );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) numTokens    (int) 	Number of tokens on the config. line.
// (>) tokens	    (char**)	List of tokens from the config. line.
//
// FUNCTION VALUE:
// (bool) : False if an error occurs
// (bool) : True if no errors occur.
//
// PURPOSE:
// Read the path for the magnetic disk and its descriptive name from a line
// in the config file.
//
// DESCRIPTION:
// 1. Check to make sure there are exactly three tokens.
// 2. Create the new magnetic disk information structure with the path
//      and description given the config file.
// 3. Initialize the max and used values to 0.
// 4. Create the channel aliases for this resource, if possible.
// 5. Add the new database structure to the list of databases.
//
// EXTERNAL VARIABLES:
// cStaMonMd::mdList
//
// PRIOR REQUIREMENTS:
// The configuration file must be opened with config_open
//
// SEE ALSO:
// cStaMonMd::config
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

boolean		cStaMonMd::md
(
    int		numTokens, 		// (in) Number of tokens.
    char**	tokens			// (in) list of tokens.
)
{
    tStaMd	*md;			// Magnetic disk info.
    int		resNum;			// Resource Number.
    boolean	ret;			// Return value.
    cStaStat	status;			// Function status code.

    if ( numTokens != 3 )
    {
	status.E_CONFIG_KWD( status, tokens[0] );
	ret = FALSE;
    }
    else
    {
	//
	// Create new magnetic disk info and it to the magnetic
	// disk list.
	//

	checkNull( md = new tStaMd, status, return( FALSE) );

	checkNull( md->name = strsav( tokens[1] ), status,
		return( FALSE ) );

	if ( ( md->path = findMountPoint( tokens[2], status ) ) == NULL )
	{
	    return( FALSE );
	}
	
	md->max = NULL_SPACE;
	md->used = NULL_SPACE;
	

	//
	// Set the status to ok, findMountPoint may have an
	// error status and returned the path given in the
	// config file.
	//

	if ( !status.ok() )
	{
	    status.S_SUCCESS( status );
	}
    

	
	//
	// If not all resource records are used then set up the
	// channel alias for this record.
	//
	   
	if ( ( resNum = newResource() ) != NULL_RES_NUM )
	{
	    checkNull( md->aliases = getAliases( resNum, status ),
    		    status, delResource() );
	}
	else
	{
	    md->aliases = NULL;
	}

	mdList.push_back( md );
	ret = TRUE;
    }

    return( ret );
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonMd::test
//
// INVOCATION:
// cStaMonMd::test( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat&)		// Function return value.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To test the monitoring of magnetic disk resources.
//
// DESCRIPTION:
// For each magnetic disk verify the path and check the EPICS channels that
// are associated with the disk.
//    
// An internal status is used.  This is done so that all databases
// can be verified.  The "check" routines do not perform the checks
// if the status is not ok.
//
// EXTERNAL VARIABLES:
// cStaMonMd::mdList
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonMd::test
(
    cStaStat	&status		// (mod) function return value.
)
{
    cStaStat 	s;		// Internal status indicator.
    boolean	ok;		// Test was ok?

    cStaMon::test( status );
    
    ok = TRUE;
    
    list<tStaMd*>::iterator	i;
    for( i = mdList.begin(); i != mdList.end(); i++ )
    {
	checkPath( ((tStaMd*) (*i))->path, s );
	if ( !s.ok() )
	{
	    ok = FALSE;
	    status+=s;
	    s.S_SUCCESS( s );
	}
	checkAliases( *(((tStaMd*) (*i))->aliases), s );
	if ( !s.ok() )
	{
	    ok = FALSE;
	    status+=s;
	    s.S_SUCCESS( s);
	}
    }

    if ( !ok )
    {
	status.E_MD_TEST_FAIL( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonMd::update
//
// INVOCATION:
// cStaMonMd( magnetDisk, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) magnetDisk   (tStaMd&)	       	Magnetic disk information.
// (!) status	    (cStaStat) 		Function return value.	
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To find the disk space and usage of a particular disk and have this
// information updated in EPICS.
//
// DESCRIPTION:
// 1. Use get_fs_usage to find the maximum space and the space used on the
//    magnetic disk.
// 2. Send these new values to EPICS.    
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None. 
//
// SEE ALSO:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	cStaMonMd::update    
(
    tStaMd	&md,			// (in)  Magnetic disk structure.
    cStaStat	&status			// (mod) Function return value.	
)
{
    int			available;	// Disk space available.
    //struct fs_usage	fsus;		// File system usage.
    //struct fs_usage	*f_usagex;	// File system usage.
    int			max;		// Total disk space.

    checkStat( status, return );

    if ( isStopping() )
    {
	return;
    }
    
    
    //f_usagex = & fsus; // XXX allan: replaced with statvfs
    struct statvfs vfs;
    if (statvfs(md.path, &vfs) != 0)
    {
	//
	// Call failed, print warning and use stored values.
	//
	    
	status.E_DF_CMD( status, "statvfs failed for ", md.path );
    }
    else
    {
	//
	// Update the values in the structure
	//

	md.max = convertBlocks( vfs.f_blocks, "kilobytes" );
	max = md.max;
	if ( max == 0 )
	{
	    md.used  = 0;
	}
	else
	{
	    available =  convertBlocks( vfs.f_bavail, "kilobytes" );
	    md.used  = max - available;
	}
    }

    

    //
    // Send values to EPICS
    //

    if ( status.ok() && md.aliases != NULL )
    {

	updateEpics( (void*)md.name, DHS_DT_STRING, md.aliases->name,
		status );
	updateEpics( (void*)md.path, DHS_DT_STRING, md.aliases->fullName,
		status );
	updateEpics( "Magnetic Disk", DHS_DT_STRING, md.aliases->type,
		status );
	updateEpics( "Kilobytes", DHS_DT_STRING, md.aliases->units,
		status );
	updateEpics( (void*) &md.max, DHS_DT_INT16, md.aliases->max, status );
	updateEpics( (void*) &md.used, DHS_DT_INT16, md.aliases->used,
		status );
	updateEpics( "1", DHS_DT_STRING, md.aliases->inUse, status );
    }
    else if ( md.aliases != NULL )
    {
	status.S_SUCCESS( status );
	
	updateEpics( "1", DHS_DT_STRING, md.aliases->inUse, status );
	updateEpics( (void*)md.path, DHS_DT_STRING, md.aliases->fullName,
		status );
	updateEpics( "Unknown", DHS_DT_STRING, md.aliases->max, status );
	updateEpics( (void*)md.name, DHS_DT_STRING, md.aliases->name,
		status );
	updateEpics( "Magnetic Disk", DHS_DT_STRING, md.aliases->type,
		status );
	updateEpics( "Unknown", DHS_DT_STRING, md.aliases->units,
		status );
	updateEpics( "Unknown", DHS_DT_STRING, md.aliases->used, status );
    }
    else
    {
	status.E_NO_ALIASES( status, md.name );
	status.S_SUCCESS( status );
    }
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonMd::updateRes
//
// INVOCATION:
// cStaMonMd::updateRes( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cStaStat)	// Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// To find the current space used for all of the magnetic disk spaces
// being monitored. 
//
// DESCRIPTION:
// For each magnetic disk structure find the current space usage.  After
// all values have been determined and sent to EPICS flush EPICS to make
// sure all values are updated.
//
// EXTERNAL VARIABLES:
// cStaMonMd::mdList.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// cStaMonMd::update
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

   void	cStaMonMd::updateRes
(
    cStaStat	&status		// (mod) Function return value.
)
{
    list<tStaMd*>::iterator	i;
    cStaStat			tStatus;

    cStaMon::updateRes( status );

    for( i = mdList.begin(); i != mdList.end(); i++ )
    {
	update (*((tStaMd*) (*i)), tStatus );
    }
}
