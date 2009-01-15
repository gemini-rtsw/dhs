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
// dhsData/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS data server.
//
// CLASSES:
// cDtsStatus - Status class.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/25 10:32:32  brighton
// added genBool.H
//
// Revision 1.1.1.1  2002/11/24 20:26:29  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.13  2000/08/10 22:02:44  dunn
// Added S_BAD_TYPE message.
//
// Revision 1.12  2000/07/27 20:10:59  dunn
// Added standalone features.
//
// Revision 1.11  1998/06/22 20:50:33  dunn
// Added messages E_EXEC_ABNORMAL_EXIT, E_EXEC_NONZERO_EXIT
//
// Revision 1.10  1998/06/11 16:41:08  dunn
// Added E_NOT_WRITTEN.
//
// Revision 1.9  1998/05/04 22:53:32  dunn
// Added more messages and changed in status class.
//
// Revision 1.8  1997/10/03 22:58:14  dunn
// Added a message, S_DF_CMD.
//
// Revision 1.7  1997/09/17 22:25:06  dunn
// Added messages and cleanupOn for the cleanup flag.
//
// Revision 1.6  1997/09/05 19:53:44  dunn
// Added some messages.
//
// Revision 1.5  1997/08/29 23:10:28  dunn
// Added messages.
//
// Revision 1.4  1997/08/19 22:32:57  dunn
// Bug fixes.
//
// Revision 1.3  1997/07/29 22:05:02  dunn
// Added messages.
//
// Revision 1.2  1997/07/23 19:45:11  dunn
// Added message defines and fixed E_MEMORY to return function name.
//
// Revision 1.1  1997/07/10 18:28:16  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GLOBALS_H
#define	GLOBALS_H

#include <cstdarg>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
}

#include "genBool.H"

#include "dhsGenStatus.H"
#include "dhsSta.H"

#define	APPLICATION	"dhsData"
#define	PREFIX		"dts"

#define DTS_DEFAULT_LIFE	-1
#define DTS_DATASET_NAME_LEN	45
#define DTS_UNIQUE_NAME_LEN	45


//
//***********************************************************************
//+
// CLASS NAME:
// cDtsStatus
//
// PUBLIC METHODS: 
// (void) msgId - Declaration of messages.
// (void) info - Print information message.
// (void) defDhsDebug - set default debug mode for the DHS.
//
// PURPOSE:
// Contains a status value.
//
//-
//***********************************************************************
//

class 	cDtsStatus: public cStaStatus
{
    public:
	//
	//  The following are the defines for the message in the msg.C file.
	//
	msgPrefix( PREFIX );

	msgId( S_ARC_HANDLER	);	// Archive command handler.
	msgId( S_ARC_REQUEST	);	// Archive command request.
	msgId( S_ARC_REQ	);	// Archive command request.
	msgId( S_BAD_CONNECTION	);	// Unauthorized connector.
	msgId( S_BAD_TYPE	);	// Bad data type received.
	msgId( S_CHUNK		);	// Chunk received.
	msgId( S_CHUNK2		);	// Chunk received.
	msgId( S_CMD_RECEIVED	);	// Command request received.
	msgId( S_COMP_TYPE	);	// Decompression type.
	msgId( S_CONFIG_OPEN	);	// Config. file opened.
	msgId( S_CONNECT_REQ	);	// Connection was request.
	msgId( S_CONNECTING	);	// Connecting to a server.
	msgId( S_CONV_WARNINGS	);	// Conversion warnings.
	msgId( S_CTL_HANDLER	);	// Control command handler.
	msgId( S_CTL_REQUEST	);	// Control command request.
	msgId( S_DATA_RECEIVED	);	// Data already received for ds.
	msgId( S_DB_CONN	);	// In get DB connection.
	msgId( S_DEBUG_LEVEL	);	// Debug level set by command.
	msgId( S_DEBUG_MSG	);	// Debug level set by command.
	msgId( S_DEL_HANDLER	);	// Debug msg, delete cmd.
	msgId( S_DEL_NODATA	);	// Deleting no data.
	msgId( S_DEL_REQ	);	// Delete request response.
	msgId( S_DEL_TEMP	);	// Delete from temp.
	msgId( S_DF_CMD		);	// Error in df cmd.
	msgId( S_DISCONNECT_REQ	);	// Disconnect request.
	msgId( S_DS_LIST_ADD	);	// Added to dataset list.
	msgId( S_DS_LIST_NOADD	);	// Not adding a dataset to the list.
	msgId( S_GET_HANDLER	);	// Get handler.
	msgId( S_GET_REQ	);	// Get request.
	msgId( S_HEADER_ERROR	);	// Error parsing fits header.
	msgId( S_HEADER_WARN	);	// Warnings parsing fits header.
	msgId( S_LABEL_INVALID	);	// Empty label in get request.
	msgId( S_LOST_CONN	);	// Lost connection.
	msgId( S_NEW_DS_NAME	);	// New name generated.
	msgId( S_NO_ROWS	);	// No rows returned.
	msgId( S_NOT_CONTRIB	);	// Not a contributor.
	msgId( S_NOT_IN_AD	);	// Not in the ad.
	msgId( S_NOT_IN_DS_LIST	);	// Not in the ds list.
	msgId( S_NOT_READY	);	// Data server not ready.
	msgId( S_OLDP_NOTIFY	);	// OLDP notified.
	msgId( S_PATHS		);	// Storage Paths.
	msgId( S_PUT_HANDLER	);	// Put handler.
	msgId( S_PUT_QL		);	// Put to quick look.
	msgId( S_PUT_REQ	);	// Put request.
	msgId( S_QL_CONNECT	);	// Error connecting to ql.
	msgId( S_QL_STREAM	);	// QL Stream request.
	msgId( S_REQUEST_REMOTELY);	// Request remotely.
	msgId( S_RUNNING	);	// Running event loop.
	msgId( S_SDS2TAR	);	// Info message to tar sds files.
	msgId( S_SERVER_UNAVAIL	);	// Server unavailable.
	msgId( S_SIMULATE_LEVEL	);	// Simulate level set by 							// command.
	msgId( S_SS_ARCHIVE	);	// Storage Server Archive cmd.
	msgId( S_SERV_CONNECT	);	// Error looking up ss in connect info.
	msgId( S_STARTUP	);	// Application startup.
	msgId( S_TEST_FAIL	);	// Test command resulted in 
					   // failure.
	msgId( S_TEST_SUCCESS	);	// Test command resulted in 
					   // success.
	msgId( S_UQ_LIST_ADD	);	// Added to unique list.
	msgId( S_VERSION	);	// Print version number.
	msgId( S_WARN_INV_DS	);	// Warn, invalid ds.
	msgId( S_WARN_INV_STATE	);	// Warn, invalid ds state for arch cmd.
	msgId( S_WROTE_FILE	);	// Wrote a file.

	msgId( E_ABORTABLE	);	// Command not abortable.
	msgId( E_ACCESS		);	// Dataset access error.
	msgId( E_AD		);	// Error in Ad library.
	msgId( E_AD_ARCHIVE	);	// Error in Ad config file.
	msgId( E_AD_IDENTITY	);	// Invalid ad ds identity string.
	msgId( E_COMP_TYPE	);	// Unrecognized comp type.
	// Don't think I need this one???msgId( E_CONFIG_KEYWORD	);	// Error reading identity 
					   // config line.
	msgId( E_CONN_LIST	);	// Error in connection lib.
	msgId( E_CONTROL_C	);	// Control C was pressed.
	msgId( E_DATASET_RECEIVED);	// Dataset data already received.
	msgId( E_DB		);	// Error in Db library.
	msgId( E_DB2		);	// Error in Db library.
	msgId( E_DEBUG_COMMAND	);	// Invalid debug command.
	msgId( E_DEL_PERM	);	// Trying to del a perm file.
	msgId( E_DIR_GONE	);	// Trying to open invalid dir.
	msgId( E_DIR_CLOSE	);	// Trying to close a dir.
	msgId( E_DS_NOT_FOUND	);	// Dataset not found in list.
	msgId( E_DSINFO_NULL	);	// Dataset info ptr is null.
	msgId( E_EXEC_ABNORMAL_EXIT);	// Abnormal exit from tar.
	msgId( E_EXEC_NONZERO_EXIT);	// Non-zero exit from tar.
	msgId( E_FILE_UNAVAIL	);	// Error file unavailable.
	msgId( E_FILE_OPEN	);	// Error opening for to write.
	msgId( E_FILE_WRITE	);	// Error writing file.
	msgId( E_GET_FAILED	);	// Error get remote failed.
	msgId( E_HEADER_CONFIG	);	// Error reading parse config file.
	msgId( E_HEADER_INIT	);	// Error initing parse lib.
	msgId( E_INVALID_COMMAND);	// Invalid command received.
	msgId( E_INV_MED	);	// Invalid media in ad.
	msgId( E_INV_TYPE	);	// Invalid media in ad.
	msgId( E_LABEL_INVALID	);	// Blank label sent.
	msgId( E_MED_MD		);	// MD missing.
	msgId( E_MK_DIR		);	// Error making a directory.
	msgId( E_MSG		);	// Error callback msg.
	msgId( E_NO_DATA	);	// No data for the ds.
	msgId( E_NO_FILE	);	// No file found.
	msgId( E_NO_LIST	);	// Control list not complete.
	msgId( E_NO_HEADER	);	// No file header found.
	msgId( E_NO_SPACE	);	// No space available.
	msgId( E_NO_READ	);	// No file read.
	msgId( E_NOT_FITS	);	// Not a fits file.
	msgId( E_NOT_RECVD	);	// Not all of the file received.
	msgId( E_NOT_WRITTEN	);	// File not written.
	msgId( E_ONLY_ONE	);	// Already data server running.
	msgId( E_OPENING	);	// Error opening connection.
	msgId( E_PRESS_LIB	);	// Error in press lib.
	msgId( E_RMV_RECEIVED	);	// Error reseting and removeing ds.
	msgId( E_SDS2FITS	);	// Converting to fits failed.
	msgId( E_SIMULATE_COMMAND);	// Invalid simulate command.
	msgId( E_SF		);	// Sf library error.
	msgId( E_SS_CONNECT_ERROR);	// Error connection to the ss.
	msgId( E_SS_CONNECT_INFO);	// Error looking up ss in connect info.
	msgId( E_STAT_FILE	);	// Status of file failed.
	msgId( E_STORAGE_PATH	);	// Storage path does not exist.
	msgId( E_STR_LEN	);	// String not long enough.
	msgId( E_TAR_FILE	);	// Tar file error.
	msgId( E_UNCOMP		 );	// Error in press library.
	msgId( E_USAGE		 );	// Command line usage error.
	msgId( E_VOL		);	// Error in volume name.

	void			info( tStatus, ... );
	inline static bool	cleanupOn( void )
				    { 
					return( dtsCleanup ); 
				    };
	inline void		cleanupOn( bool x )
				    {
					dtsCleanup = x; 
				    };
	inline static tDebugLevel
				defDhsDebug( void )
				    { 
					return( dhsLevel ); 
				    };
	inline void		defDhsDebug( tDebugLevel x )
				    {
					dhsLevel = x; 
				    };
	inline static bool	parseOn( void )
				    { 
					return( dtsHeader ); 
				    };
	inline void		parseOn( bool x )
				    {
					dtsHeader = x; 
				    };
	inline static bool	serversOn( void )
				    { 
					return( dtsServer ); 
				    };
	inline void		serversOn( bool x )
				    {
					dtsServer = x; 
				    };
	inline static bool	standAlone( void )
				    { 
					return( dtsStandalone ); 
				    };
	inline void		standAlone( bool x )
				    {
					dtsStandalone = x; 
				    };
    protected:
    private:
	static tDebugLevel	dhsLevel;
	static bool		dtsServer; 
	static bool		dtsStandalone; 
	static bool		dtsCleanup; 
	static bool		dtsHeader; 
};


extern void	configInit( cDtsStatus & );
extern void	argInit( int, char *[], cDtsStatus & );
#endif // GLOBALS_H
