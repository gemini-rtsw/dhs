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
// fh/h/fh.H
//
// PURPOSE:
// Contains the external definitions for the header parse library.
//
// CLASSES:
// cHdrParser - Handle all parsing of a fits buffer.
// cHdrParseList - Keeps a list of 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:05  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.5  1998/06/17 20:23:30  dunn
// Changed remainingCol parameters
//
// Revision 1.4  1998/04/23 17:04:32  nhill
// Moved the checkConfig and checkNull macros to the dhsGenStatus header.
// Moved the E_MEMORY, E_CONFIG, and E_CONFIG_KEYWORD messages to the
// cDhsStatus class.
//
// Revision 1.3  1997/08/29 22:01:43  dunn
// DD init flag added and deconstructor.
//
// Revision 1.2  1997/08/19 20:31:48  dunn
// Cleaned up the file.
//
// Revision 1.1  1997/07/29 20:01:51  dunn
// Initial revision
//
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef HDRPRS_H
#define	HDRPRS_H

#include <map>
#include <list>
#include <string>

extern "C"
{
#include "db.h"
#include "fitsio.h"
#include "dd.h"
}


#define	checkDd( fn, dStatus, status, ret )				\
    {									\
	fn;								\
	if ( ( dStatus ) != DD_S_SUCCESS )				\
	{								\
	    status.E_DD( status, ddMsg );				\
	    ret;							\
	}								\
    }

#define	checkDbLibHdr( fn, status, ret )				\
    {									\
	int		s;						\
	if ( ( s = fn ) != DB_SUCCESS )					\
	{								\
	    status.E_DB( status, db_msg );				\
	    ret;							\
	}								\
    }


#define	checkDbx( fn, status, ret )					\
    {									\
	int		s;						\
	if ( ( s = fn ) == FAIL )					\
	{								\
	    status.E_DB2( status, db_msg, #fn );			\
	    ret;							\
	}								\
    }

#define	checkFits( fn, fstatus, status, ret )				\
    {									\
	fn;								\
	if ( fstatus > 0 )						\
	{								\
	    status.E_CFITS( status, #fn );				\
	    ret;							\
	}								\
    }


#define	checkReadConfig( status, ret )					\
    {									\
	if ( ! hdrReadConfig ) 						\
	{								\
	    status.E_READ_CONFIG( status );				\
	    ret;							\
	}								\
    }

//
//***********************************************************************
//+
// CLASS NAME:
// cHdrParseList
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Contains list of keywords found in the fits buffer.
//
//-
//***********************************************************************
//

class 	cHdrParseList
{
    friend class cHdrParser;

    public:
	typedef 	map< string,		// Sort key of datatype string.
		     	cHdrParseList *,	// Into this containr this type
		     	less<string> >		// Sorting mechanizm.
			tParseList;		// Provides access to containr
	typedef 	tParseList::iterator	// Provides iterator to containr
			iterator;
    protected:
    private:
			cHdrParseList( struct dd_row  *, char *, char * );
			~cHdrParseList( void );
	char		hdrKeyword[FLEN_KEYWORD];
	struct dd_row	*hdrRow;		// Row.
	char		*hdrValue;		// Value.
};


//
//***********************************************************************
//+
// CLASS NAME:
// cHdrParser
//
// PUBLIC METHODS: 
// (void) dbName - Returns database name.
// (void) fitsName - Returns Fits table name.
// (void) ~cHdrParser - deconstructor
// (void) checkFitsTable - Check columns in the fits table.
// (void) config - Read config file.
// (void) close - Close db connections and free variables.
// (void) createSql - Compose an sql statement.
// (void) executeSql - Execute a passed in sql statement.
// (void) init - Read the config file, init dd,  check the database.
// (void) parseHeader - Parse the fits header.
// (void) remainingCols - To check that all mandatory keywords are there.
//
// PURPOSE:
// Contains parsing of a fits buffer information.
//
//-
//***********************************************************************
//

class 	cHdrParser
{
    public:
	class	hStatus: public cDhsStatus	// Hdr return response
	{
	    public:
		msgPrefix( "hdr" );
		msgId( S_WARNING_COL );
		msgId( S_WARNING_MAN );
		msgId( E_CFITS );
		msgId( E_COL_TYPE );
		msgId( E_DATA_TYPE );
		msgId( E_DB );
		msgId( E_DB2 );
		msgId( E_DD );
		msgId( E_FITS_TABLE );
		msgId( E_INSERT );
		msgId( E_READ_CONFIG );
	    protected:
	    private:
	};

	hStatus		hdrStatus;		// Header status.
	inline char *	dbName( void )
			    {
				return( hdrDatabaseName );
			    };
	inline char *	fitsName( void )
			    {
				return( hdrFitsTblName );
			    };

	static void	checkFitsTable( hStatus & );
	static void	close( hStatus & );
	static void	config( hStatus & );
	static void	createSql( hStatus &, char **, char *, 
			    cHdrParseList::tParseList );

	static void	executeSql( hStatus &, char * );

	static void	init( hStatus &, char *, char * );
	void		parseHeader( hStatus &, void *, long, char * );
	void		remainingCols( hStatus &, char *, 
			    cHdrParseList::tParseList );

    protected:
    private:
	static boolean	tblParams( int, char ** );	
	void		stripSpaces( char * );	

	static struct dbprocess
			*hdrDbProc;	// Database connection.

	static bool	hdrReadConfig;	// Read config flag.

	static char	*hdrFitsTblName;
					// Header Table name.
	static char	*hdrDatabaseName;
					// Database name.
	static bool	hdrDDInit;	// DD library initilized.

	static cMutex	cdmMutexHdr;	// Mutex to protect the db writes.
};

#endif // HDRPRS_H
