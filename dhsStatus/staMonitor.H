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
// fitness for a particular pur-	ou de pertinence pour un usage
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
// dhsStatus/h/staMonitor.H
//
// PURPOSE:
// Contains the external definitions for the disk space and database
// resource monitors and for the resource monitor..
//
// CLASSES:
// cStaMonDb    - Handles the monitoring of databases resources.
// cStaMonMd	- Handles the monitoring of disk space resources.
// cStaMon  	- Handles the monitoring of all resources.
//		      
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:29  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.7  1998/06/24 23:03:26  jaeger
// Added simStop variable to differentiate the stopping of monitoring do
// to "reset", "init", or "exit" commands and changes in simulation levels.
//
// Revision 1.6  1998/06/19 18:10:24  nhill
// Changed the cStaStatus class name to cStaStat.
//
// Revision 1.5  1998/06/01 22:41:19  jaeger
// Added "full name" field to resource information.
//
// Revision 1.4  1998/05/20 04:58:38  jaeger
// Added simulate methods.
//
// Revision 1.3  1998/04/15 22:24:56  nhill
// Updated to the latest version of the cDhsStatus class.
//
// Revision 1.2  1998/03/06 19:21:29  jaeger
// Moved dbConnGet out of class definition.
//
// Revision 1.1  1998/02/05 21:17:35  jaeger
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STAMONITOR_H
#define	STAMONITOR_H


//***********************************************************************
//
// Include files.
//
//***********************************************************************
    
#include 	<list>
#include	 <pthread.h>

extern "C"
{
#include	<sybfront.h>
#include 	<sybdb.h>
}
    
#include 	"genCond.H"

#include 	"dhs.h"    
    
#include 	"globals.H"

#define NULL_RES_NUM			-1    	// Null Resource number.
#define MAX_RESOURCES			12	// Maximum resources allowed.
    

//***********************************************************************
//
// Macro Definitions.
//
//***********************************************************************


//
//***********************************************************************
//+
// MACRO NAME:
// checkCadcDb
//
// INVOCATION:
// checkCadcDb( fn, status, ret );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>)	fn	(function)     		CADC Db library fucntion.
// (!) 	status	(cStaStat&)		Function return value.
// (>) 	ret	(return statement)	return statement  on error.
//
// MACRO VALUE:
// None.
//    
// PURPOSE:
// To execute a CADC Db library function and check its return
// value.
//
// DESCRIPTION:
// Execute the function.
// If an error occured then
//     change the status to E_CDB, release te database connection
//     and execute the return statement.
// Else continue.
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
#define checkCadcDb( fn, status, ret )                                 	\
    {                                                                   \
        int             s;                                              \
        if ( ( s = fn ) != DB_SUCCESS )                                 \
        {                                                               \
            status.E_CDB( status, db_msg, #fn );                        \
	    msg_clear( db_msg );					\
            dbConnRelease();  						\
	    ret;                                                        \
        }                                                               \
    }

//
//***********************************************************************
//+
// MACRO NAME:
// checkSybDb
//
// INVOCATION:
// checkSybDb( fn, status, ret );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>)	fn	(function)     		SYBASE Db library fucntion.
// (!) 	status	(cStaStat&)		Function return value.
// (>) 	ret	(return statement)	return statement to execute on error.
//
// MACRO VALUE:
// None.
//
// PURPOSE:
// To execute a SYBASE Db library function and check its return
// value.
//
// DESCRIPTION:
// Execute the function.
// If an error occured then
//     change the status to E_SDB, release te database connection
//     and execute the return statement.
// Else continue.
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

#define checkSybDb( fn, status, ret )                          		\
    {                                                                   \
        int             s;                                              \
	if ( !status.ok() || isStopping() )				\
        {								\
            dbConnRelease();  						\
	    ret;							\
        }								\
        else if ( ( s = fn ) == FAIL )                                  \
        {                                                               \
            status.E_SDB( status, db_msg, #fn );                       	\
	    msg_clear( db_msg );					\
            dbConnRelease();  						\
	    ret;                                                        \
        }                                                               \
    }

//
//***********************************************************************
//+
// MACRO NAME:
// convertBlocks
//
// INVOCATION:
// convertBlocks( b, size )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>)	b	( int )	      	Block size value.	
// (>) 	size	( string )	Size to be converted to.
//
// MACRO VALUE:
// (int) 	The converted block value.
//
// PURPOSE:
// To convert a value from a block value to either blocks,
// bytes, kilobytes or megabytes where one block contains
// 512 bytes.
//
// DESCRIPTION:
// If size is "kilobytes" then divide block value by 2
// If size is "megabytes" then divide block value by 2048
// If size is "bytes" then multiply block value by 512
// otherwise do not alter the block value.   
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
    
#define convertBlocks( b, size )                                        \
    ( strcmp( "kilobytes", size  ) == 0  ?  ( b ) / 2 :                 \
      strcmp( "megabytes", size ) == 0 ?  ( b ) / 2048 :                \
      strcmp( "bytes", size ) == 0 ? ( b ) * 512 : ( b ) )

//
//***********************************************************************
//+
// MACRO NAME:
// convertBytes
//
// INVOCATION:
// convertBytes( b, size )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>)	b	( int )	      	Byte size value 
// (>) 	size	( string )	Size value is in.
//
// MACRO VALUE:
// (int) 	The byte value converted to kilobytes.
//
// PURPOSE:
// To convert a value from either kilobytes, megabytes or gigabytes
// to kilobytes.
// 512 bytes.
//
// DESCRIPTION:
// If size is "GB" then multiply byte value by 2048
// If size is "MB" then multiply byte value by 1024
// If size is "KB" then return byte value
// otherwise do not alter the byte value.   
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
	
#define convertBytes( b, size )                                        	\
    ( strcmp( "GB", size  ) == 0  ?  ( b ) * 2048 :                 	\
      strcmp( "MB", size ) == 0 ?  ( b ) * 1024 :                	\
      strcmp( "KB", size ) == 0 ? ( b ) : ( b ) )	

//***********************************************************************
//
// Structure types
//    
//***********************************************************************

//
// Channel aliases
//
    
typedef struct
{
    char	inUse[20];		// Channel Access prefix
    char	max[20];		// Channel Access prefix
    char	fullName[50];		// Channel Access prefix	
    char	name[20];		// Channel Access prefix
    char	type[20];		// Channel Access prefix
    char	units[20];		// Channel Access prefix
    char	used[20];		// Channel Access prefix
} tStaCa;
    

//
// Database information.
//

typedef struct
{
    char*	dbName;		// Database name;
    char	*name;		// String describing the database.
    int		max;		// Total database space..
    int		used;		// Database space used.
    tStaCa	*aliases;	// Channel Aliases associated with it.
} tStaDb;


//
//  Magnetic Disk information.
//
    
typedef struct
{
    char	*path;		// Disks directory path.
    char	*name;		// String describing the magnetic disk.
    int	 	max;		// Total disk space.
    int		used;		// Disk space used
    tStaCa	*aliases;	// Channel Aliases associated with it.
} tStaMd; 

//
//***********************************************************************
//+
// CLASS NAME:
// cStaMon
//
// PUBLIC METHODS: 
// ???
// (bool) simulate	Get or set the simulate mode for the cStaMon class.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// ???
//
// PURPOSE:
// Monitors the resources used by DHS.
//
//-
//***********************************************************************
//

class cStaMon
{
    public:
	//
	// Data - None.
	//

	static char	*staResInUse;
	static char	*staResMax;
	static char	*staResFullName;
	static char	*staResName;
	static char	*staResNum;
	static char	*staResType;
	static char	*staResUnits;
	static char	*staResUsed;


	
    	//
        // Methods
	//
	
				cStaMon( void );
				~cStaMon( void ) { };

	static inline void	checkAliases( tStaCa &, cStaStat & );
	static void		configAll( cStaStat & );
	static inline void	delResource( void )
				{
				    if ( numResources > NULL_RES_NUM )
				    {
					numResources--;
				    }
				}
	static inline int	newResource( void )
				{
				    if ( numResources < MAX_RESOURCES )
				    {
					return( ++numResources );
				    }
				    else
				    {
					return( NULL_RES_NUM );
				    }
				}
        static inline bool
	                        simulate( void ) { return( cmSimulate ); };
	static inline bool	simulate( bool );
	static void		start( cStaStat & );
	static void		stop();
	inline bool		isStopping( void )
				{
				    return( stopping );
				}
	static void		testAll( cStaStat & );
	static void		updateEpics( const void*, DHS_DATA_TYPE,
					const char *, cStaStat & );
	static void		updateAll ( cStaStat & );

    protected:
	//
	// Data.
	//

	static bool		configFound;	// TRUE if Monitoring info.
						// found in the config.file.

	//
	// Methods.
	//

	virtual void		cleanup( cStaStat & );
	virtual void		config( cStaStat & );
	static tStaCa		*getAliases( int, cStaStat & );
	virtual void		init( cStaStat & ) { };
	inline void		startThread( cStaStat &status )
				{
				    int		s; // thread func. result.

				    s = pthread_create( &staMonThread,
			    		    NULL, &monitor, this );
				    if ( s != 0 )
				    {
					status.E_MON_THREAD( status );
				    }
				}
	
	virtual void		test( cStaStat & ) {};
	virtual void		updateRes( cStaStat & ) {};

    private:
    	//
	// Data
    	//

	static timespec		absTime;	// Current time.
	static bool		checking;	// Check resources?
	static cCond		checkRes;	// Cond. variable for above.
	static list<cStaMon*>	classList;	// List of class instances.
	static long		frequency;	// Freq. of checking.
	static int		numResources;	// Number of resources monitored.
	static bool		cmSimulate;	// True if in simulate mode.
	static bool		simStop;	// True if simulate level cmd
						// caused monitoring to stop.
	static pthread_t	staMonThread;	// Monitoring thread.
	static bool		stopping;	// Stop monitoring?

	
    	//
        // Methods
	//

	static boolean	aliasConfig( int, char** );
	static inline void
			broadcastCheckRes( void )
			{
			    checkRes.broadcast();
			}
	static boolean	freqConfig( int, char** );
	static inline void 
			lockCheckRes( void )
			{
				checkRes.lock();
			};
	static void	*monitor( void* );
	static boolean 	str2long( char*, long* );
	static inline void 
			timedWaitCheckRes( void )
			{
				checkRes.timedWait( absTime );
			};
	static inline void 
			unlockCheckRes( void )
			{
				checkRes.unlock();
			};
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMon::simulate
//
// INVOCATION:
// cStaMon::simulate( true );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) value (bool) New value for the simulate member.
//
// FUNCTION VALUE:
// (bool) The old value of the simulate member.
//
// PURPOSE:
// Set the new value of the simulate data member and return the old value
//
// DESCRIPTION:
// Trivial.
//
// EXTERNAL VARIABLES:
// None.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool            cStaMon::simulate
(
    bool        value           // (in)  New value for the simulate member.
)
{
    bool        oldSimulate;
    cStaStat	status;

    
    oldSimulate = cmSimulate;
    cmSimulate = value;

    //
    // If old value is true and new value is false, then the
    // monitoring of spaces needs to stop.
    //

    if ( oldSimulate == true && cmSimulate == false && staMonThread == 0 ) {
	start( status );
    }
        
    
    //
    // If old value is false and new value is true, then the
    // monitoring of spaces needs to start.
    //

    if ( oldSimulate == false && cmSimulate == true && staMonThread != 0 ) {
	simStop = TRUE;
	stop();
    }
	
    return( oldSimulate );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cStaMonDb
//
// PUBLIC METHODS: 
// (type) method - description
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// (type) data - description
//
// PURPOSE:
// Responsible for updating the space values for each database used
// by DHS.
//
//-
//***********************************************************************
//

class	cStaMonDb : cStaMon
{
    public:
	//
	// Data - None.
	//

	
	//
	// Methods
	//

	void inline	config( cStaStat & );
	void		cleanup( cStaStat & );
        static inline struct dbprocess *
			dbConnGet( cStaStat & );
        static inline void
                        dbConnRelease( void )
                        {
                            dbProcMutex.unlock();
                        };
	void	init( cStaStat& );
	void	test( cStaStat & );
	void	updateRes( cStaStat & );
	
    protected:
	//
	// No protected methods or data.
	//
	
    private:
	//
	// Data.
	//

	static struct dbprocess
			*dbConnection;	// The database connection.
	static cMutex	dbProcMutex;	// Mutex prevents miuse of db lib.
	static char 	*dbServerName;	// Database server name.
	static list<tStaDb*>
			dbList;		// List of magnetic disks.

	//
	// Methods
	//

	static void	checkDatabase( char*, cStaStat & );
	static boolean	db( int, char** );
	static boolean	serverName( int, char** );
	void	update( tStaDb &, cStaStat & );
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cStaMonDb::dbConnGet
//
// INVOCATION:
// cStaMonDb::dbConnGet( status )
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cStaStat&) 	Function return status.
//
// FUNCTION VALUE:
// NULL 			: If the database connection is not found
// dbConnection (dbprocess*)	: If the database connection is found.
//
// PURPOSE:
// To find  the database connection, if there is one.
//
// DESCRIPTION:
// Place a lock on getting the connection, ensures proper usage of the
// database.  Then if the connection is NULL register an error otherwise
// cancel any pending queries and return the database connection.
//    
//
// EXTERNAL VARIABLES:
// cStaMonDb::dbConnection
// cStaMonDb::dbProcMutex
//
// PRIOR REQUIREMENTS:
// db_open and db_init have been called, db_open has been called with
// dbConnection.
//
// SEE ALSO:
// cStaMonDb::dbConnRelease.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//
    
inline struct dbprocess *	cStaMonDb::dbConnGet
(
    cStaStat	&status		// (mod) Function return status.
)
{
    dbProcMutex.lock();
    if ( dbConnection != NULL )
    {
	if( dbcanquery( dbConnection ) == FAIL )
	{
	    status.E_SDB( status, " ", "dbcanquery( dbConnection )" );
	    return( dbConnection );
	}
    
    }
    else 
    {
	status.E_SDB( status, "no connection to SYBASE server",
		"dbcanquery(dbConnection)"  );
    }
    return( dbConnection );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cStaMonMd
//
// PUBLIC METHODS: 
// (type) method - description
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// (type) data - description
//
// PURPOSE:
// Determines the disk space usage for the various disks used by
// DHS.
//
//-
//***********************************************************************
//

class 	cStaMonMd : cStaMon
{
    public:
	//
	// Data - None.
	//
	
	//
	// Methods.
	//

	void inline	config( cStaStat & );
	void		cleanup( cStaStat & );
	void		test( cStaStat & );
	void		updateRes( cStaStat & );


    protected:
	//
	// No protected methods or data.
	//
	
    private:
	//
	// Data.
	//

	static list<tStaMd*>
			mdList;			// List of magnetic disks.

	//
	// Methods
	//

	static void	checkPath( const char*, cStaStat & );
	static char	*findMountPoint( const char*, cStaStat & );
	static boolean	md( int, char** );
	void		update( tStaMd &, cStaStat & );
};

   

#endif // STAMONITOR_H
