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
// dhsHistory/src/hisEvent.C
//
// PURPOSE:
//
// CLASSES
// n/a.
// 
// METHOD NAME(S)
//
//INDENT-OFF*
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>


extern "C"
{
#include "gen_types.h"
#include "gen_config.h"
}

#include "dhs++.H"
#include "cadef.h"
#include "tsDefs.h"


#include "globals.H"
#include "hisEvent.H"
#include "hisDhs.H"
#include "hisCa.H"


//
// Define the static data in the cHisEvent class.
//

char *		cHisEvent::heFilepath;
int		cHisEvent::heFileDesc = -1;
cCond		cHisEvent::heCondQueue;
cHisEvent::tEventQueue	cHisEvent::heQueue;
bool		cHisEvent::heQueueLoopDone;



//
//***********************************************************************
//+
// FUNCTION NAME:
// fileRename
//
// INVOCATION:
// fileRename( newPath, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) newPath	(char *) 	New path to rename history log file. 
// (!) status	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Rename history log to the passed new path. 
//
// DESCRIPTION:
// Rename history log to the passed new path. While renaming is taking
// place, the event queue must be locked. Lock the queue, close the file, 
// rename it, reopen the  history log file, unlock the queue. 
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

void		cHisEvent::fileRename
(
    char	*newPath,	// (in)  The new path to rename to. 
    cHisStatus	&status		// (mod) Program status. 
)
{
    int		rename_status;

    
    checkStat( status, return );


    queueLock();
    check( fileClose( status ), status, return );
    rename_status = rename( heFilepath, newPath );
    if ( rename_status != 0 )
    {
	status.E_FRENAME( status, heFilepath, newPath );
	status.sysErrno();
    }
    check( fileOpen( status ), status, return );
    queueUnlock();


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// filepathCreate
//
// INVOCATION:
// filepathCreate( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &)	Program status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Creates the path to the history log file 
//
// DESCRIPTION:
// Creates the path to the history log file and saves it within the class. 
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

void		cHisEvent::filepathCreate
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    char        tempstr[PATH_MAX];


    checkStat( status, return );


    sprintf( tempstr, "%s/%s", cHisCa::tempDir(), FNAME_HISTORYLOG );
    checkNull( heFilepath = strsav( tempstr ), status, return );
    

    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// fileOpen
//
// INVOCATION:
// fileOpen( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &)	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Opens the history log file
//
// DESCRIPTION:
// If needed, create the path to the history log file. Open the file, 
// save the file descriptor within the class. 
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

void		cHisEvent::fileOpen
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    checkStat( status, return );


    if ( heFilepath == NULL )
    {
	check( filepathCreate( status ), status, return );
    }
    heFileDesc = open( heFilepath, O_WRONLY | O_APPEND | O_CREAT | O_SYNC,
	    S_IRUSR | S_IWUSR | S_IRGRP );
    if ( heFileDesc == -1 )
    {
	status.E_FOPEN( status, heFilepath );
	status.sysErrno();
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// closeFile
//
// INVOCATION:
// closeFile( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &) 	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Close the history log file. 
//
// DESCRIPTION:
// Close the history log file. 
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

void		cHisEvent::fileClose
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    int		close_status;	// Return status from fclose.


    checkStat( status, return );


    close_status = close( heFileDesc );
    if ( close_status == -1 )
    {
	status.E_FCLOSE( status, heFilepath );
	status.sysErrno();
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// startQueueLoop
//
// INVOCATION:
// startQueueLoop();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status	(cHisStatus &)	Program status.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Start the queue loop in a thread 
//
// DESCRIPTION:
// Set flag indicating that the event processing queue loop should be 
// running, create thread and execute execThread() which runs the queue
// processing loop. 
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

void		cHisEvent::startQueueLoop
(
    cHisStatus	&status		// (mod) Program status. 
)
{
    pthread_attr_t
		attr;		// POSIX thread attributes.
    int         pthread_status;	// pthread_create function return status.


    checkStat( status, return );


    //
    // Start a thread for the list processing.
    //

    heQueueLoopDone = false;
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
    pthread_status = pthread_create( NULL, &attr, execThread, NULL );
    pthread_attr_destroy( &attr );
    if ( pthread_status != 0 )
    {
	status.E_PTHREAD( status );
    }


    return;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// readRecord
//
// INVOCATION:
// readRecord( fileDescriptor, endOfFile );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) fileDescriptor	(int)	 Open file descriptor to read from.
// (!) endOfFile	(bool &) If end of file, this flag is set on exit.
//
// FUNCTION VALUE:
// None. 
//
// PURPOSE:
// Reads one history event record from the passed file descriptor. 
//
// DESCRIPTION:
// Reads one history event record from the passed file descriptor. The file
// format for each record is: 
//
// (4-byte-int)(4-byte-int)(4-byte-int)(4-byte-int)(char-string)(char-string)
//    DATE        NSEC       LEN STR1    LEN STR2      STR1         STR2
// 
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

void            cHisEvent::readRecord
(
    int		fileDescriptor,		// (in)  Open file descriptor.
    bool	&endOfFile		// (mod) Flag to set if EOF. 
)
{
    int		numRead;		// Actual number of bytes read. 
    int		length1;		// Length of SIR label.
    int		length2;		// Length of SIR value. 

    
    numRead = read( fileDescriptor, &heDate, sizeof( heDate ) );
    if ( numRead == 0 )
    {
	endOfFile = true;
	return;
    }

    (void) read( fileDescriptor, &heNsec, sizeof( heNsec ) );
    (void) read( fileDescriptor, &length1, sizeof( length1 ) );
    (void) read( fileDescriptor, &length2, sizeof( length2 ) );
    (void) read( fileDescriptor, &heSIR, length1 );
    heSIR[length1] = '\0';
    (void) read( fileDescriptor, &heValue, length2 );
    heValue[length2] = '\0';


    return;
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// writeRecord
//
// INVOCATION:
// writeRecord();
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Writes a record to the history log file. 
//
// DESCRIPTION:
// Copies all relevant data into the buffer, then writes to the file. 
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

void            cHisEvent::writeRecord
(
    void
)
{
    unsigned char buffer[100];		// Buffer to write to file. 
    unsigned char *bufptr;		// Pointer into buffer. 
    int		length;			// Length to write to file. 
    int		num_to_write;		// Number of bytes to write to file. 
    int		num_written;		// Number of bytes written to file. 
    cHisStatus	status;			// Program status.


    bufptr = &(buffer[0]);


    //
    // Create buffer to write to file. 
    //
    
    memcpy( bufptr, &heDate, sizeof( heDate ) );
    bufptr += sizeof( heDate );

    memcpy( bufptr, &heNsec, sizeof( heNsec ) );
    bufptr += sizeof( heNsec );

    length = strlen( heSIR );
    memcpy( bufptr, &length, sizeof( length ) );
    bufptr += sizeof( length );

    length = strlen( heValue );
    memcpy( bufptr, &length, sizeof( length ) );
    bufptr += sizeof( length );

    memcpy( bufptr, &heSIR, strlen( heSIR ) );
    bufptr += strlen( heSIR );

    memcpy( bufptr, &heValue, strlen( heValue ) );
    bufptr += strlen( heValue );


    //
    // Write buffer to file. 
    //

    num_to_write = (int) ( bufptr - &(buffer[0]) );
    num_written = write( heFileDesc, &(buffer[0]), num_to_write );
    if ( num_written != num_to_write )
    {
	status.E_FWRITE( status, heFilepath );
	status.sysErrno();
    }


    return;
}


//
//***********************************************************************
//+
// FUNCTION NAME:
// execThread
//
// INVOCATION:
// Invoked only by pthread_create().
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None. - The default parameter of (void *) type is not used. 
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Contains the queue processing loop. 
//
// DESCRIPTION:
// Contains the queue processing loop. Removes events from the queue and
// writes them to the history log file. 
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

void		*cHisEvent::execThread
(
    void	*
)
{
    cHisEvent	*event;		// Event to remove from queue & add to file.
    cHisStatus	status;		// Status returned from file routines. 


    check( fileOpen( status ), status, VOID );
    if ( !status.ok() )
    {
	cHisCa::channelLoopStop();
	return( NULL );
    }
   


    for ( ; ; )
    {
	//
	// If no events in queue, wait. Unless done, in which case return. 
	//
	
	queueLock();
	while ( queueEmpty() )
	{
	    if ( heQueueLoopDone )
	    {
		queueUnlock();
		check( fileClose( status ), status, VOID );
		return( NULL );
	    }
	    queueWait();
	}
	event = queueFront();
	queuePop();
	queueUnlock();


	//
	// Write event to file and delete it. 
	//
	
	event->writeRecord();
	delete event;
    }
}
