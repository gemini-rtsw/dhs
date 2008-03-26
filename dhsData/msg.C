static char rcsid[] = "$Id: msg.C,v 1.2 2002-11-27 17:15:08 brighton Exp $";
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
// dhsData/src/msg.C
//
// PURPOSE:
// To handle the formation and printing of messages
//
// METHOD NAME(S)
// None.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:27:18  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.17  2000/08/10 22:10:51  dunn
// Added S_BAD_TYPE message.
//
// Revision 1.16  2000/07/27 20:10:19  dunn
// Added standalone features.
//
// Revision 1.15  1999/06/16 22:27:22  nhill
// Changed some logged messages to debug messages.
//
// Revision 1.14  1998/08/04 22:03:54  dunn
// Fixed it so that Version works
//
// Revision 1.13  1998/06/22 20:47:31  dunn
// Changed S_ARC_REQ to be debug FULL only, fixed it to have updper
// case Data Server and added 2 messages.
//
// Revision 1.12  1998/06/11 16:39:56  dunn
// Added E_NOT_WRITTEN meessage;
//
// Revision 1.11  1998/05/07 23:35:26  dunn
// Fixed message to change health
//
// Revision 1.10  1998/05/04 23:01:10  dunn
// messages changes.
//
// Revision 1.9  1998/03/06 18:19:37  jaeger
// Stopped msgDisplay from infinitely calling itself, if logEvent
// cases an error then it will keep causing an error.
//
// Revision 1.8  1997/10/03 23:04:02  dunn
// Added a message.
//
// Revision 1.7  1997/09/17 22:13:28  dunn
// Added messages.
//
// Revision 1.6  1997/09/05 19:52:55  dunn
// New messages.
//
// Revision 1.5  1997/08/29 23:03:13  dunn
// Added messages.
//
// Revision 1.4  1997/08/19 22:30:54  dunn
// More msg's.
//
// Revision 1.3  1997/07/29 22:02:26  dunn
// Added messages.
//
// Revision 1.2  1997/07/23 19:41:35  dunn
// Added messages.
//
// Revision 1.1  1997/07/10 18:27:55  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>

#include "dhs++.H"

#include "globals.H"
	msgText( cDtsStatus,  S_ARC_HANDLER, P_DEBUG_FULL | P_LOG,
	    "Received archive notify command request : `%s'." );
	msgText( cDtsStatus,  S_ARC_REQ, P_DEBUG_FULL | P_LOG,
	    "File %s archived." );
	msgText( cDtsStatus,  S_ARC_REQUEST, P_LOG,
	    "Received archive notify command from `%s` on `%s', segDir: %s, completed with status `%s'." );
	msgText( cDtsStatus,  S_BAD_CONNECTION, P_LOG,
	    "Unauthorized connection request, disconnecting `%s' on `%s'." );
	msgText( cDtsStatus,  S_BAD_TYPE, P_LOG,
	    "Some/all data received for dataset:%s with diff types: old %c:%c." );
	msgText( cDtsStatus,  S_CMD_RECEIVED, P_DEBUG_MIN | P_LOG, 
	    "Received %s command request from `%s' on `%s'." );
	msgText( cDtsStatus,  S_COMP_TYPE, P_DEBUG_FULL | P_LOG,
	    "Decompression type set is `%s'." );
	msgText( cDtsStatus,  S_CONFIG_OPEN, P_DEBUG_FULL | P_LOG,
	    "Configuration file `%s' has been opened." );
	msgText( cDtsStatus,  S_CONNECT_REQ, P_LOG,
	    "Connection request received from `%s' on `%s'." );
	msgText( cDtsStatus,  S_CONNECTING, P_DEBUG_MIN | P_LOG,
	    "Connecting to server `%s' on `%s'." );
	msgText( cDtsStatus,  S_CONV_WARNINGS, P_DEBUG_MIN | P_LOG,
	    "Warnings for conversion to Fits for %s are:\n%s" );
	msgText( cDtsStatus,  S_CTL_HANDLER, P_DEBUG_FULL | P_LOG,
	    "Received control request : `%s'." );
	msgText( cDtsStatus,  S_CTL_REQUEST, P_DEBUG_MIN | P_LOG,
	    "Received %s request from `%s` on `%s' for dataset `%s' completed with status `%s'." );
	msgText( cDtsStatus,  S_CHUNK, P_DEBUG_FULL | P_LOG, 
	    "Debug: Dataset %s just received chunk number %d." );
	msgText( cDtsStatus,  S_CHUNK2, P_DEBUG_FULL | P_LOG,
	    "Debug: Dataset %s received, written, updated, chunk number is %d." );
	msgText( cDtsStatus,  S_DATA_RECEIVED, P_DEBUG_MIN | P_LOG,
	    "Data already received for dataset `%s' from source `%s'." );
	msgText( cDtsStatus,  S_DB_CONN, P_DEBUG_FULL,
	    "Locking Db connection of type `%d'." );
	msgText( cDtsStatus,  S_DEBUG_LEVEL, P_DEBUG_MIN | P_LOG,
	    "Debug level set to `%s'." );
	msgText( cDtsStatus,  S_DEBUG_MSG, P_DEBUG_FULL,
	    "Debug message: `%s'." );
	msgText( cDtsStatus,  S_DEL_HANDLER, P_DEBUG_FULL,
	    "Debug message: Delete request for dataset `%s'." );
	msgText( cDtsStatus,  S_DEL_NODATA, P_DEBUG_FULL | P_LOG,
	    "Debug message: Delete request for dataset `%s', no data received." );
	msgText( cDtsStatus,  S_DEL_REQ, P_LOG,
	    "Delete dataset request from `%s' on `%s' for dataset `%s' completed with status `%s'." );
	msgText( cDtsStatus,  S_DEL_TEMP, P_LOG,
	    "Warning, cleaning out temporary storage, deleting %s." );
	msgText( cDtsStatus,  S_DF_CMD, P_DEBUG_MIN | P_LOG, 
	    "Warning, Error in the df cmd : `%s'." );
	msgText( cDtsStatus,  S_DISCONNECT_REQ, P_LOG,
	    "Disconnect request received from `%s' on `%s'." );
	msgText( cDtsStatus,  S_DS_LIST_ADD, P_DEBUG_FULL,
	    "Adding dataset `%s' to list of current datasets." );
	msgText( cDtsStatus,  S_DS_LIST_NOADD, P_LOG,
	    "Warning, Duplicate datasetname `%s' NOT added to list of current datasets." );
	msgText( cDtsStatus,  S_GET_HANDLER, P_DEBUG_FULL | P_LOG, 
	    "Received get request for datatype `%d' for dataset `%s'." );
	msgText( cDtsStatus,  S_GET_REQ, P_LOG,
	    "Data request from `%s' on `%s' for dataset `%s' completed with status `%s'." );
	msgText( cDtsStatus,  S_HEADER_ERROR, P_DEBUG_MIN | P_LOG,
	    "Error parsing header for `%s', health is bad : %s" );
	msgText( cDtsStatus,  S_HEADER_WARN, P_DEBUG_MIN | P_LOG,
	    "Warnings parsing header for %s : %s" );
	msgText( cDtsStatus,  S_LABEL_INVALID, P_LOG,
	    "Error: Get request contains empty label from `%s' on `%s'" );
	msgText( cDtsStatus,  S_LOST_CONN, P_LOG,
	    "Warning, Connection lost to `%s' on `%s' %s." );
	msgText( cDtsStatus,  S_NEW_DS_NAME, P_LOG,
	    "New dataset name generated, was '%s', new name '%s'." );
	msgText( cDtsStatus,  S_NO_ROWS, P_DEBUG_FULL,
	    "No rows returned for `%s'." );
	msgText( cDtsStatus,  S_NOT_CONTRIB, P_LOG,
	    "Data received from an invalid contributor : `%s' for dataset : `%s'." );
	msgText( cDtsStatus,  S_NOT_IN_AD, P_DEBUG_MIN | P_LOG,
	    "Warning, attempting to remove file %s from ad tables, does not exist." );
	msgText( cDtsStatus,  S_NOT_IN_DS_LIST, P_DEBUG_FULL | P_LOG,
	    "Dataset %s not in the dataset list." );
	msgText( cDtsStatus,  S_NOT_READY, P_NULL,
	    "Data Server not available, disconnecting `%s' on `%s'." );
	msgText( cDtsStatus,  S_OLDP_NOTIFY, P_DEBUG_MIN | P_LOG,
	    "File notify command to oldp server by `%s' on `%s' for dataset `%s' sent to OLDP with `%s'." );
	msgText( cDtsStatus,  S_PATHS, P_DEBUG_MIN | P_LOG, 
	    "Temporary storage spaceAvail: %d  path:\n`%s',\nPermanent storage spaceAvail: %d  path:\n`%s'." );
	msgText( cDtsStatus,  S_PUT_HANDLER, P_DEBUG_FULL | P_LOG,
	    "Received put request for datatype `%d' for dataset `%s'." );
	msgText( cDtsStatus,  S_PUT_QL, P_DEBUG_MIN | P_LOG,
	    "Dataset %s sent to quick look server with status `%d'." );
	msgText( cDtsStatus,  S_PUT_REQ, P_LOG,
	    "Data sent from `%s' on `%s' for dataset `%s' completed with status `%s'." );
	msgText( cDtsStatus,  S_QL_CONNECT, P_DEBUG_MIN | P_LOG,
	    "Error connecting to QL server for %s request by `%s' on `%s' for dataset `%s'." );
	msgText( cDtsStatus,  S_QL_STREAM, P_DEBUG_FULL | P_LOG,
	    "QL Stream control by `%s' on `%s' for dataset `%s' passed on to QL with `%s'." );
	msgText( cDtsStatus,  S_REQUEST_REMOTELY, P_DEBUG_MIN | P_LOG,
	    "Requesting dataset `%s' from remote dataServer `%s'." );
	msgText( cDtsStatus,  S_RUNNING, P_LOG, 
	    "Data Server ready to receive requests." );
	msgText( cDtsStatus,  S_SDS2TAR, P_DEBUG_MIN | P_LOG,
	    "Recovering dataset `%s', taring dataset." );
	msgText( cDtsStatus,  S_SERVER_UNAVAIL, P_LOG,
	    "Warning, server `%s' on `%s' is no longer connected." );
	msgText( cDtsStatus,  S_SIMULATE_LEVEL, P_DEBUG_MIN | P_LOG, 
	    "Simulation level set to `%s' by `%s' on `%s'." );
	msgText( cDtsStatus,  S_SS_ARCHIVE, P_DEBUG_MIN | P_LOG,
	    "Archive file command to storage server by `%s' on `%s' for dataset `%s' sent to SS with `%s'." );
	msgText( cDtsStatus,  S_SERV_CONNECT, P_DEBUG_FULL | P_LOG,
	    "Error connecting to %s server for %s request by `%s' on `%s' for dataset `%s'." );
	msgText( cDtsStatus,  S_STARTUP, P_DEBUG_MIN | P_LOG,
	    "Startup of `%s',\nVersion `%s'." );
	msgText( cDtsStatus,  S_TEST_FAIL, P_LOG | P_HEALTH_WARN,
	    "Test command %s failed." );
	msgText( cDtsStatus,  S_TEST_SUCCESS, P_DEBUG_MIN | P_LOG, 
	    "Test command %s succeeded." );
	msgText( cDtsStatus,  S_UQ_LIST_ADD, P_DEBUG_FULL,
	    "Adding unique name `%s' to list of current unique names." );
	msgText( cDtsStatus,  S_VERSION, P_NULL,
	    "%s" );
	msgText( cDtsStatus,  S_WARN_INV_DS, P_LOG,
	    "Warning, do not have info about dataset %s for %s cmd.\n" );
	msgText( cDtsStatus,  S_WARN_INV_STATE, P_LOG,
	    "Warning, invalid %s state for dataset %s, ignoring archive command.\n" );
	msgText( cDtsStatus,  S_WROTE_FILE, P_DEBUG_FULL, 
	    "Debug: Just wrote file %s\n" );
	msgText( cDtsStatus,  E_ABORTABLE, P_LOG,
	    "Error, command %s is not abortable, sent by `%s' on `%s'." );
	msgText( cDtsStatus,  E_ACCESS, P_DEBUG_MIN | P_LOG,
	    "Error accessing the dataset chunk `%s'" );
	msgText( cDtsStatus,  E_AD, P_LOG,
	    "Error in the ad library:\n%s." );
	msgText( cDtsStatus,  E_AD_ARCHIVE, P_LOG,
	    "Error, %s volume archive name of `%s' does not match archive name in the ad config file of `%s'" );
	msgText( cDtsStatus,  E_AD_IDENTITY, P_LOG,
	    "Error, ad configured with `%s' identity string and dhsData has identity `%s'." );
	msgText( cDtsStatus,  E_COMP_TYPE, P_LOG,
	    "Unrecognized compression type, `%s', exiting." );
	msgText( cDtsStatus,  E_CONN_LIST, P_LOG,
	    "Error in the connection library function:\n%s\n%s" );
	msgText( cDtsStatus,  E_CONTROL_C, P_LOG,
	    "Control C was pressed, exiting." );
	msgText( cDtsStatus,  E_DATASET_RECEIVED, P_LOG,
	    "Error, data already received for dataset `%s', cannot set %s." );
	msgText( cDtsStatus,  E_DB, P_LOG | P_HEALTH_BAD,
	    "Error in the db library: `%s'." );
	msgText( cDtsStatus,  E_DB2, P_LOG | P_HEALTH_BAD,
	    "Error in the db library: `%s' \nfor function `%s'." );
	msgText( cDtsStatus,  E_DEBUG_COMMAND, P_LOG,
	    "Invalid debugLevel command: `%s' requested by `%s' at `%s'" );
	msgText( cDtsStatus,  E_DEL_PERM, P_LOG,
	    "Error, trying to delete dataset %s which is permanent." );
	msgText( cDtsStatus,  E_DIR_GONE, P_LOG,
	    "Error, directory `%s' does not exist" );
	msgText( cDtsStatus,  E_DIR_CLOSE, P_LOG,
	    "Error, directory `%s' did not close successfully" );
	msgText( cDtsStatus,  E_DS_NOT_FOUND, P_DEBUG_MIN | P_LOG,
	    "Error, dataset %s not found in dataset list." );
	msgText( cDtsStatus,  E_DSINFO_NULL, P_LOG,
	    "Error, dataset %s pointer information is not found, cannot convert to fits." );
	msgText( cDtsStatus, E_EXEC_ABNORMAL_EXIT, P_LOG,
	    "Error, abornomal exit from tar command : `%s'" );
	msgText( cDtsStatus,  E_EXEC_NONZERO_EXIT, P_LOG,
	    "Error in tar command : `%s', exit status: %d." );
	msgText( cDtsStatus,  E_FILE_OPEN, P_LOG | P_HEALTH_BAD,
	    "Error opening file `%s' for writing :" );
	msgText( cDtsStatus,  E_FILE_UNAVAIL, P_LOG,
	    "Error: File `%s' requested by `%s' at `%s' not available %s." );
	msgText( cDtsStatus,  E_FILE_WRITE, P_LOG | P_HEALTH_BAD,
	    "Error writing file `%s' :" );
	msgText( cDtsStatus,  E_GET_FAILED, P_LOG,
	    "Error, get of file `%s' remotely from %s failed." );
	msgText( cDtsStatus,  E_HEADER_CONFIG, P_LOG,
	    "Error reading the fits header information from the configuration file : %s" );
	msgText( cDtsStatus,  E_HEADER_INIT, P_LOG,
	    "Error initializing the fits header parsing library :\n%s" );
	msgText( cDtsStatus,  E_INVALID_COMMAND, P_LOG,
	    "Error, Invalid command: `%s' received from `%s' on `%s'" );
	msgText( cDtsStatus,  E_INV_MED, P_DEBUG_MIN | P_LOG, 
	    "Error, invalid media type in lookup of: `%s'" );
	msgText( cDtsStatus,  E_INV_TYPE, P_LOG,
	    "Error, type requested `%d'  and data types stored not compatible, stored filename : `%s'." );
	msgText( cDtsStatus,  E_LABEL_INVALID, P_LOG,
	    "Error, Blank dataset name sent" );
	msgText( cDtsStatus,  E_MED_MD, P_LOG,
	    "Error, medium MD not defined in ad library configuration file." );
	msgText( cDtsStatus,  E_MK_DIR, P_LOG | P_HEALTH_BAD, 
	    "Error, making directory %s to store data chunk:" );
	msgText( cDtsStatus,  E_MSG, P_LOG,
	    "Error called, message: `%s'." );
	msgText( cDtsStatus,  E_NO_DATA, P_LOG,
	    "Error: No data received for dataset : `%s'." );
	msgText( cDtsStatus,  E_NO_FILE, P_LOG,
	    "Error: File does not exist, looking for: `%s'." );
	msgText( cDtsStatus,  E_NO_LIST, P_LOG,
	    "Error: Ctr request from `%s' on `%s' missing attrib %s for command %s for dataset %s." );
	msgText( cDtsStatus,  E_NO_HEADER, P_LOG,
	    "Error: Cannot find end of fits header for : `%s'." );
	msgText( cDtsStatus,  E_NO_SPACE, P_LOG | P_HEALTH_BAD,
	    "Error: Not enough space available %dkB to write file : `%s', need %dkB." );
	msgText( cDtsStatus,  E_NO_READ, P_LOG,
	    "Error: File not read due to type and compression, filename: `%s'." );
	msgText( cDtsStatus,  E_NOT_FITS, P_LOG,
	    "Error: File type of fits requested, file is not a fits file : `%s'." );
	msgText( cDtsStatus,  E_NOT_RECVD, P_LOG,
	    "Error: File not all received (%c), file not available: `%s'." );
	msgText( cDtsStatus,  E_NOT_WRITTEN, P_LOG | P_HEALTH_BAD,
	    "Error, not writing file sent : `%s'." );
	msgText( cDtsStatus,  E_ONLY_ONE, P_LOG,
	    "Warning: There is already a Data Server running, exiting." );
	msgText( cDtsStatus,  E_OPENING, P_DEBUG_FULL| P_LOG,
	    "Error in DHS library open function." );
	msgText( cDtsStatus,  E_PRESS_LIB, P_LOG,
	    "Error initiliazing the press library: %s." );
	msgText( cDtsStatus,  E_RMV_RECEIVED, P_LOG,
	    "Error, data already received for dataset `%s', cannot %s." );
	msgText( cDtsStatus,  E_SDS2FITS, P_LOG,
	    "Error converting to fits for dataset %s.\n%s" );
	msgText( cDtsStatus,  E_SIMULATE_COMMAND, P_LOG, 
	    "Invalid simulateLevel command: `%s' from `%s' on `%s'" );
	msgText( cDtsStatus,  E_SF, P_LOG,
	    "Error initiliazing the sf library: %s." );
	msgText( cDtsStatus,  E_SS_CONNECT_ERROR, P_LOG,
	    "Error, Could not connect to the status server, check that its running/user name/imp master..." );
	msgText( cDtsStatus,  E_SS_CONNECT_INFO, P_LOG,
	    "Error, identity string for status server not found in `authorized' info in configuration file." );
	msgText( cDtsStatus,  E_STAT_FILE, P_LOG,
	    "Error, status of file failed, path:%s.");
	msgText( cDtsStatus,  E_STORAGE_PATH, P_LOG,
	    "Error, %s storage path does not exist, exiting : `%s'" );
	msgText( cDtsStatus,  E_STR_LEN, P_DEBUG_MIN | P_LOG,
	    "String is length 0 of variable: `%s'" );
	msgText( cDtsStatus,  E_TAR_FILE, P_LOG,
	    "Error taring files for dataset `%s'" );
	msgText( cDtsStatus,  E_UNCOMP, P_LOG,
	    "Error in press library : `%s'." );
	msgText( cDtsStatus,  E_USAGE, P_NULL, 
	    "Usage:\n"
	    "    dhsData [-V] [-debug {NONE|MIN|FULL} {NO_DHS}] [-simulate] [-noDb [-temp <tempPath>] [-perm <permPath>] ] [-noServers] [-noParse] [-noCleanup]");
	msgText( cDtsStatus,  E_VOL, P_LOG,
	    "Error, %s, volume name `%s'" );

