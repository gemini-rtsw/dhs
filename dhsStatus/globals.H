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
// dhsStatus/h/globals.H
//
// PURPOSE:
// Contains the external definitions for the DHS status server.
//
// CLASSES:
// cStaStat - The status class for this server.
//
// MACROS:
// checkCa - Check the return code from a channel access function.
// checkGen - Check the return code from the gen library.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:28  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.15  1998/06/24 23:05:44  nhill
// Added a new message.
//
// Revision 1.14  1998/06/19 21:18:40  nhill
// Fixed the caCheck macro to prevent deadlocks.
//
// Revision 1.13  1998/06/19 18:11:29  nhill
// Changed the cStaStatus class name to cStaStat.
// Changed cStaStat to inherit from the cStaStatus class instead of cDhsStatus,
// and overloaded the routines necessary to log updates directly instead
// of using ICD1c commands.
//
// Revision 1.12  1998/06/02 19:17:44  nhill
// Added message E_CHANNEL_NAME.
//
// Revision 1.11  1998/05/12 20:18:03  nhill
// Shortened some message ids.
//
// Revision 1.10  1998/04/15 21:16:24  nhill
// Updated the documentation.
// Moved the check and checkConfig macros to the dhsGen library.
// Updated to the newest version of the cDhsStatus class.
//
// Revision 1.9  1998/03/06 18:51:10  jaeger
// Addec NO_MONITORING message
//
// Revision 1.8  1998/02/17 00:24:57  jaeger
// Changed checkCa so that it frees the mutex lock
//
// Revision 1.7  1998/02/05 21:15:51  jaeger
// Added messages for the monitoring of resources.
//
// Revision 1.6  1998/01/08 21:01:47  jaeger
// Add a message id for the reset health command.
//
// Revision 1.5  1997/12/23 23:19:31  nhill
// Added a comment about a compiler bug.
//
// Revision 1.4  1997/10/01 17:13:27  nhill
// Added record name to the channel access errors.
//
// Revision 1.3  1997/06/13 20:41:49  nhill
// Changed to use the new version of the cDhsStatus class.
//
// Revision 1.2  1997/04/16 22:31:59  nhill
// Added some information to the channel access error message.
//
// Revision 1.1  1997/03/27 22:16:14  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GLOBALS_H
#define	GLOBALS_H

#include <stdarg.h>

extern "C"
{
#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
}

#include "dhsSta.H"
#include "dhsGenSubsystem.H"

#ifdef HAVE_BOOL_H
#include <bool.h>
#endif

#define	DHS_NO_BULK
#define	APPLICATION	"dhsStatus"
#define	PREFIX		"sta"

//
// There is a bug in the sun workshop C++ compiler (v4.2) which makes
// the expression in the EPICS channel access status macros(e.g. 
// ECA_NORMAL) evaluate incorrectly when used in an inlined function
// (note functions are only inlined with optimized code). Because of this,
// this macro should not be used in an inline function.

#define	checkCa( fn, s, ret, record )					\
    {									\
	int		caS;						\
	if ( (caS = fn) != ECA_NORMAL )					\
	{								\
	    scMutex.unlock();						\
	    s.E_CA( s, caS, record, #fn, ca_message( caS ) );		\
	    scMutex.lock();						\
	    ret;							\
	}								\
    }

#define checkGen( fn, status, ret )					\
    {									\
        if ( ( fn ) == FALSE )						\
        {								\
	    status.E_GEN( status, #fn );				\
	    ret;							\
	}								\
    }

//
//***********************************************************************
//+
// CLASS NAME:
// cStaStat
//
// PUBLIC METHODS: 
// (static void) flush - Flush all state updates to the status recievers.
// (static void) init - Initialize this class.
// (static bool) logStatus - Indicate if status messages should be logged or
//		 not. Mesasges should not be logged before the config file
//		 has been read, or for messages that occur as a result of
//		 attempting to log another messages.
// (static void) resetHealth - Reset health status to good.
// (static void) update - Update a status record.
//
// PURPOSE:
// Contains a status value.
//
//-
//***********************************************************************
//

class 	cStaStat: public cStaStatus
{
    public:
	msgPrefix( PREFIX );
	msgId( S_CA_CONNECT );		// Making ca connection.
	msgId( S_CMD_RECEIVED );	// A command was received.
	msgId( S_CONNECT );		// Connection request received.
	msgId( S_DEBUG_LEVEL );		// Debug level set by command.
	msgId( S_DISCONNECT );		// Disconnect request received.
	msgId( S_EXITING );		// Program is exiting.
	msgId( S_INITIALIZING );	// Initializing the status server.
	msgId( S_MONITORING );		// Monitoring of spaces started.
	msgId( S_NO_MONITORING );	// No Monitoring specifeid in config.
	msgId( S_SET_HEALTH );		// Health level reset to GOOD.
	msgId( S_SIMULATE_LEVEL );	// Simulate level set by command.
	msgId( S_STOP_MONITOR );	// Monitoring of spaces stopped.
	msgId( S_TEST_FAILURE );	// Test command resulted in failure.
	msgId( S_TEST_SUCCESS );	// Test command resulted in success.
	msgId( S_VERSION );		// Print version number.

	msgId( E_BAD_CHALIAS );		// No channel alias found for monitor.
	msgId( E_BAD_EPICS_VAL );	// Bad epics value.
	msgId( E_CA );			// Error from channel access.
	msgId( E_CA_LOG );		// Error from channel access log record.
	msgId( E_CDB );			// Error in CADC Db Library.
	msgId( E_CHANNEL_MISSING );	// Could not find channel.
	msgId( E_CHANNEL_NAME );	// Invalid channel name.
	msgId( E_CONFIG_TYPE );		// doesn't conform to expected type.
	msgId( E_CWD );			// Problem getting current working dir.
	msgId( E_DB_TEST_FAIL );	// Magnetic disk test failed.
	msgId( E_DEBUG_CMD );		// Invalid debug command.
	msgId( E_DF_CMD );		// Error in df command.
	msgId( E_EVENTLOG_CMD );	// Invalid eventLog command.
	msgId( E_GEN );			// Error in a CADC gen func.
	msgId( E_INVALID_CMD );		// Invalid command received.
	msgId( E_MD_TEST_FAIL );	// Magnetic disk test failed.
	msgId( E_MON_CONFIG );		// Error getting monitoring info from
					// the configuration file.
	msgId( E_MON_TEST_FAIL );	// Monitoring test failed.
	msgId( E_MON_THREAD );		// Error in the monitoring thread.
	msgId( E_NO_ALIASES );		// No Aliases found for a resource.
	msgId( E_PATH );		// Invalid disk path in config file.
        msgId( E_SDB );      		// Error in SYBASE Db library.
	msgId( E_SIMULATE_CMD );	// Invalid simulate command.
	msgId( E_STAUPDATE_CMD );	// Invalid status update command.
	msgId( E_SUBSCRIBE_CMD );	// Invalid subscribe command.
	msgId( E_SUBSYSTEM );		// Error in the subsytem lib.
	msgId( E_UNSUBSCRIBE_CMD );	// Invalid unsubscribe command.
	msgId( E_USAGE );// Command line usage error.

	static void	flush( cStaStat &status );
	static void	init( cStaStat & );
	static bool	logStatus( bool flag )
			{
			    bool	oldFlag( ssLogStatus );

			    ssLogStatus = flag;

			    return( oldFlag );
			}
	static void	resetHealth( cStaStat & status )
			{
			    update( "health", "GOOD", status );
			    flush( status );
			}
	static void	update( const char *, const char *, cStaStat & );
    protected:
    private:
	static bool	messageLog( cDhsStatus &, 
				const cDhsStatus::cStatusProperty &, 
				const char * );
	static bool	healthSet( cDhsStatus &, 
				const cDhsStatus::cStatusProperty &, 
				const char * );

	static		bool	ssLogStatus;


	//
	// The following functions aren't real functions. They are declared
	// to prevent the compiler warning indicating these functions are
	// hidden by the real functions above.
	//

	static void	init( cDhsConnection &, const char *, cStaStatus & );
	static void	init( const char *, cStaStatus & );
	static void	flush( cDhsStatus & );
	static void	update( const char *, const char *, cDhsStatus & );
	static void	resetHealth( cDhsStatus & );
};


extern void	configInit( cStaStat & );
extern void	argInit( int, char *[], cStaStat & );
#endif // GLOBALS_H
