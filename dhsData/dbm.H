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
// dhsData/h/dbm.H
//
// PURPOSE:
// Contains the external definitions for the database connections used in 
// the DHS data server.
//
// CLASSES:
// cDtsDbManager - Handle all database connections, etc.
// cDtsChunkInfo - Handle chunk/contributor information.
// cDtsDatasetTable - Handle process table functions.
// cDtsUniqueTable - Handle uniqueName table functions.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.11  2000/07/27 20:10:47  dunn
// Added standalone features.
//
// Revision 1.10  1999/06/16 22:28:01  nhill
// Added function cDtsDatasetTable::getAdFileInfo.
//
// Revision 1.9  1998/05/04 22:50:50  dunn
// Added ad database, user, passwrd information ,
// and composing sql statement for ds information command.
//
// Revision 1.8  1997/10/17 20:44:56  dunn
// Code cleanup
//
// Revision 1.7  1997/09/17 22:23:46  dunn
// Code cleanup.
//
// Revision 1.6  1997/09/05 19:54:36  dunn
// Added filePutDs
//
// Revision 1.5  1997/08/29 23:07:24  dunn
// Added close of db connection method.
//
// Revision 1.4  1997/08/19 22:32:41  dunn
//  Bug fixes.
//
// Revision 1.3  1997/07/29 22:04:20  dunn
// Added makeUnretrievable, fileDel.
// ,
//
// Revision 1.2  1997/07/23 19:44:14  dunn
// Changed checkAd to check for medium enum error before success,
// and added inline function to return servername.
//
// Revision 1.1  1997/07/10 18:28:16  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DTSDBM_H
#define	DTSDBM_H

#include <map>
#include <list>
#include <string>
#include "dhs++.H"

#define	checkAd( fn, status, ret )					\
    {									\
	int		s;						\
	s = fn;								\
	if ( s == AD_E_MEDIUM_ENUM )					\
	{								\
	    status.E_MED_MD( status );					\
	    ret;							\
	}								\
	else if ( s != AD_SUCCESS )					\
	{								\
	    status.E_AD( status, ad_msg );				\
	    ret;							\
	}								\
    }

#define	checkDbLib( fn, status, ret )					\
    {									\
	int		s;						\
	if ( ( s = fn ) != DB_SUCCESS )					\
	{								\
	    status.E_DB( status, db_msg );				\
	    ret;							\
	}								\
    }

#define	checkDbCall( fn, conType, status, ret )				\
    {									\
	int		s;						\
	if ( ( s = fn ) != DB_SUCCESS )					\
	{								\
	    status.E_DB( status, db_msg );				\
	    releaseDbConn( conType );					\
	    ret;							\
	}								\
    }

#define	checkDb( fn, conType, status, ret )				\
    {									\
	int		s;						\
	if ( ( s = fn ) == FAIL )					\
	{								\
	    status.E_DB2( status, db_msg, #fn );			\
	    releaseDbConn( conType );					\
	    ret;							\
	}								\
    }

//
//  Dataset Health
//
#define DTS_HL_HEALTHY		'H'
#define DTS_HL_SICK		'S'

//
//  Dataset Status 
//
#define DTS_DS_PART_RECEIVED	'P'
#define DTS_DS_ALL_RECEIVED	'R'
#define DTS_DS_NOT_RECEIVED	' '

//
//  Oldp Server Notification
//
#define DTS_OL_NOTIFIED		'Y'
#define DTS_OL_NOT_NOTIFIED	' '

//
//  Storage Server Notification
//
#define DTS_SS_NOTIFIED		'Y'
#define DTS_SS_NOT_NOTIFIED	' '
#define DTS_SS_UNECESSARY	'U'

//
//  Data Format.
//
#define DTS_FM_FITS		'F'
#define DTS_FM_RAW		'R'
#define DTS_FM_UNKNOWN		' '
#define DTS_FM_TAR		'T'
#define DTS_FM_DS		'D'


//
// String Sizes.
//
#define DTS_UNIQUE_NAME_LEN	45
#define DTS_DATASET_NAME_LEN	45
#define DTS_CONTRIB_LEN		256

typedef	char	DTS_UNIQUE_NAME[DTS_UNIQUE_NAME_LEN + 1];
typedef	char	DTS_DATASET_NAME[DTS_DATASET_NAME_LEN + 1];
typedef char	DTS_CONTRIBUTORS[DTS_CONTRIB_LEN + 1];

//
// Return ds info attribute names.
//
#define DTS_AD_DB	"database"
#define DTS_AD_ISQL	"isqlString"
#define DTS_AD_PSSWD	"password"
#define DTS_AD_SERV	"server"
#define DTS_AD_USER	"userName"



//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDbManager
//
// PUBLIC METHODS: 
// (char *) datasetTbl - returns name of the dataset table.
// (char *) processTbl - returns name of the process table.
// (char *) serverName - returns name of the db server
// (char *) storeTbl - returns name of the storage table.
// (char *) uniqueTbl - returns name of the unique table.
//
// (void) close - Close the db connections.
// (void) config() - Read the configuration parameters.
// (void) init - Initialize db and ad libraries & open connections.
// (char) formatDetermine() - returns single character format type.
// (char) formatDetermine() - returns single format type, given a name.
// (void) getDbConn() - get the database connection
// (void) getFileList() - get a listing from the database
// (void) makeRetrievable() - make a file retrievable
// (void) makeUnretrievable() - make a file unretrievable
// (void) mountPoints - get temporary and permanent mount points.
// (void) mountPoints - save temporary and permanent mount points.
// (void) nameInfo - get name information from ad.
// (void) releaseDbConn - release a database connection.
// (void) testAll - test all database connections.
// (void) uniqueConnect - check to make sure only dataServer connected.
// 
//
// PURPOSE:
// Contains database management information.
//
//-
//***********************************************************************
//

class 	cDtsDbManager
{
    public:
	//
	//  The following are defines for a process connection.
	//

	typedef enum
        {
	    	DB_AD_LIB = 0,
		DB_DS_LIB = 1,
		DB_PR_LIB = 2,
		DB_UN_LIB = 3
	} tProcessCon;


	static inline char *
		    	datasetTbl( void )
				{
				    return( cdmDsTblName );
				};
	static inline char *
		    	processTbl( void )
				{
			    	    return( cdmProcTblName );
				};
	static inline char *
			serverName( void )
				{
				    return( cdmServer ); 
				};
	static inline char *
			storeDb( void )
				{
				    return( cdmDatabase ); 
				};
	static inline char *
		    	uniqueTbl( void )
				{
				    return( cdmUniqueTblName );
				};

	static void	close( void );
	static void	config( cDtsStatus & );
	static void	init( cDtsStatus & );
	static char	formatDetermine( DHS_BD_PUT_TYPE );
	static char	formatDetermine( char *);
	static struct dbprocess *		
			getDbConn( tProcessCon );
	static void	getFileList( cDtsStatus &, cDhsAvList * );
	static void	makeRetrievable( cDtsStatus &, char *, char *, char *,
				int );
	static void	makeUnretrievable( cDtsStatus &, char *, char, 
				bool );
	static void	mountPoints( cDtsStatus & );
	static void	mountPoints( cDtsStatus &, char *, char * );
	static void	nameInfo( cDtsStatus &, char *, char *, char *, char * );
	static void	releaseDbConn( tProcessCon );
	static bool	testAll( cDtsStatus & );
	static void	uniqueConnect( cDtsStatus &, struct dbprocess *,
			       	char * );
    protected:
    private:
	static boolean	dbParams( int, char ** );	
					// database parameters.
	static struct dbprocess
			*dbprocAd;	// dbproc to use for ad library.
	static struct dbprocess
			*dbprocDs;	// dbproc for dataset library calls.
	static struct dbprocess
			*dbprocPr;	// dbproc for process library calls.
	static struct dbprocess
			*dbprocUn;	// dbproc to use for unique name 
					// library calls.
	static char	*cdmAdDatabase;	// Ad database name.
	static char	*cdmAdServer;	// Ad database server name.
	static char	*cdmDatabase;	// Database name.
	static char	*cdmDsTblName;	// Database Table name.
	static char	*cdmFileTbl;	// Md file table name.
	static char	*cdmPasswd;	// Passwd.
	static char	*cdmProcTblName;// Process Table name.
	static char	*cdmServer;	// Database server name.
	static char	*cdmUniqueTblName;
					// Unique Table name.
	static char	*cdmUserName;	// User name.
	static char	*cdmVolTbl;	// Md volume table name.

	static cMutex	cdmMutexAd;	// Mutex to protect the dbprocAd
	static cMutex	cdmMutexDs;	// Mutex to protect the dbprocDs
	static cMutex	cdmMutexPr;	// Mutex to protect the dbprocPr
	static cMutex	cdmMutexUn;	// Mutex to protect the dbprocUn
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDatasetTable
//
// PUBLIC METHODS: 
// cDtsDatasetTable - constructor.
// ~cDtsDatasetTable - constructor.
// (char *) contributors - Returns contributor list.
// (char *) uniqueName - Returns unique name.
// (void) datasetNameMax - Get max. name.
// (void) dsPut - Put to dataset table.
// (void) dsGet - Get from dataset table.
// (void) fileDel - Delete from process table.
// (void) fileGet - Get from process table.
// (void) fileListGet - Get a list of dataset from the process table.
// (void) filePut - Put to the process table.
// (void) fileUpdate - Update the process table.
//
// PURPOSE:
// Contains a row from the process table.
//
//-
//***********************************************************************
//

class 	cDtsDatasetTable : public cDtsDbManager
{
    public:
			cDtsDatasetTable( char *, char, char *, char *, long,
				DHS_BD_PUT_TYPE );
			cDtsDatasetTable( char *, char *, long, char, char, 
			char, char, char, char, char * );
			~cDtsDatasetTable( void );

	inline char *	contributors ( void )
				{
			    	    return( dstContributors );
				};
	
	inline char *	uniqueName ( void )
				{
			    	    return( dstUniqueName );
				};

	static void	datasetNameMax (cDtsStatus &,
				char *, int * );
	static void	dsPut (cDtsStatus &, char *, cDtsDatasetTable * );
	static void	dsGet (cDtsStatus &, char *, cDtsDatasetTable ** );
	static void	fileDel (cDtsStatus &, char * );
	static void	fileGet (cDtsStatus &, char *, cDtsDatasetTable ** );
	static void	fileListGet (cDtsStatus &, char, 
				map< string,cDtsDatasetTable *,
				less<string> > & );
	static void	filePut (cDtsStatus &, char *, cDtsDatasetTable * );
	static void	fileUpdate (cDtsStatus &, char *, cDtsDatasetTable * );
	void		getAdFileInfo( cDtsStatus & );

	char		*dstDatasetName;		// Dataset name.
	char		*dstFileName;			// Filename, for stand-
							// alone mode only.
	char		*dstUniqueName;			// Unique name.
	long		dstDateReceived;		// Data entered into db.
	char		dstReceived;			// Data received.
	char		dstLifetime;			// Life of the dataset.
	char		dstHealth;			// Health of the dataset
	char		dstOldpNotify;			// Y- notified, A-n.a., 
							//   " " Not done
	char		dstSsNotify;			// Storage server not.
	char		dstFormat;			// Data format.
	char		*dstContributors;		// Who contributes.
	int		dstFileSize;			// File size.
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsUniqueTable
//
// PUBLIC METHODS: 
// (void) uniqueNameGet - Get from the unique table.
// (void) uniqueNamePut - Insert into unique table.
// (void) uniqueNameMax - Get max. name.
//
// PURPOSE:
// Contains a row from the uniqueName table.
//
//-
//***********************************************************************
//

class 	cDtsUniqueTable : public cDtsDbManager
{
    public:
			cDtsUniqueTable( char *, long );
			~cDtsUniqueTable( void );
			
	char		*uqnUniqueName;			// Unique name.
	long		uqnDateReceived;		// Data entered into db.

	static void	uniqueNameGet (cDtsStatus &,
				char *, long * );
	static void	uniqueNamePut (cDtsStatus &, char *, long );
	static void	uniqueNameMax (cDtsStatus &,
				char *, int * );

    protected:
    private:
};


#endif // DTSDBM_H
