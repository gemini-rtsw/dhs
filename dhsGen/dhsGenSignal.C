static char rcsid[] = "$Id: dhsGenSignal.C,v 1.3 2003-01-15 18:23:35 brighton Exp $";
//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1998				(c) 1998
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
// dhsGen/src/dhsGenSignal.C
//
// PURPOSE:
// Contains routines to set up a signal handler for a DHS server
//
// METHOD NAME(S)
// dhsGenSignal - Set up signal handlers for specified signals.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/11/27 17:15:08  brighton
// ported to gcc-3.2.1/linux
//
// Revision 1.1.1.1  2002/11/24 20:27:50  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:00  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.1  1998/03/13 22:05:22  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#include <pthread.h>
#include <csignal>
#include <cstdarg>

#include "dhsGenSignal.H"
#include "dhsGenStatus.H"
#include "genListOf.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cSig
//
// PUBLIC METHODS: 
//	  cSig - Constructor for the class.
//	  handle - Execute the user specified handler function.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// An object of this class is created for each handled signal
//
// The cListOf class is used to keep a list of these objects, indexed
// by signal number.
//
//-
//***********************************************************************
//

class cSig: public cListOf< cSig, int, cDhsStatus >
{
    public:
	//
	// Set up a type for the signal handler function.
	//

	typedef void (*sigFn)( int );


			cSig( int sig, sigFn fn ): 
				cListOf< cSig, int, cDhsStatus > ( sig )
			    {
				sSignal = sig;
				sFn = fn;
			    };
			cSig( int sig ):
				cListOf< cSig, int, cDhsStatus > ( sig ) {};
	void		handle( void )
			    {
				if ( sFn != NULL )
				{
				    sFn( sSignal );
				}
			    };
    private:
	int		sSignal;
	sigFn		sFn;
};

static sigset_t	sigSet;			// The set of signals being handled.
static void	*sigHandler( void * );	// Pthread_create callable function
					// to wait for signals.

//
//***********************************************************************
//+
// FUNCTION NAME:
// dhsGenSignal
//
// INVOCATION:
// dhsGenSignal( status, signal1, fn1, signal2, fn2, NULL );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) status (cDhsStatus &) Function return status.
// (>) ... (vararg) A list of signal numbers and the functions to call
//	in response to the signal. The list of signals is terminated by a
//	signal of NULL. The function may be NULL to disable a signal.
//
// FUNCTION VALUE:
// None
//
// PURPOSE:
// Set up handler functions for a specified list of signals
//
// The varargs part of the argument list consists of pairs of signals
// and handler functions. The list is terminated by a NULL signal. If the
// handler function is NULL, no function is called.
//
// This function should be called once at the begining of the execution of
// a program, and before any threads are created.
//
// DESCRIPTION:
// - Create a cSig object for each signal being handled.
// - Create a mask with a bit for each signal being handled.
// - block all signals to be handled. This blockage will be inherited
//   by all threads created by the current thread.
// - Create a new thread executing the sigHandler function. This thread
//   will wait for and then handle signals.
//
// EXTERNAL VARIABLES:
// sigSet - The current set of handled signals.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - sigHandler
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void	dhsGenSignal
(
    cDhsStatus 	&status,	// (mod) Function return status.	*/
    ...
)
{
    cSig::sigFn	fn;
    int		sig;
    va_list	varg;
    pthread_t thread; // XXX allan: added


    checkStat( status, return );


    //
    // Empty the sigSet mask.
    //

    sigemptyset( &sigSet );
    va_start( varg, &status );  // XXX allan: added &


    //
    // Read the varargs.
    //

    for (;;)
    {
	//
	// Get The signal number from the varargs list.
	//

	sig = va_arg( varg, int );


	//
	// If the signal is 0, there are no more arguments.
	//

	if ( sig == 0 )
	{
	    break;
	}


	//
	// Add the appropriate bit to the signal mask.
	//

	sigaddset( &sigSet, sig );


	//
	// Get the function pointer from the varargs list.
	//

	fn = va_arg( varg, cSig::sigFn );


	//
	// Create a cSig object for the signal.
	//

	new cSig( sig, fn );
    }
    va_end( varg );


    //
    // Block the set of signals being handled.
    //

    pthread_sigmask( SIG_BLOCK, &sigSet, NULL );


    //
    // Execute the signal handler in a new thread.
    //

    pthread_create(&thread, NULL, sigHandler, NULL ); // XXX allan: added &thread
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// sigHandler
//
// INVOCATION:
// N.A. called by pthread_create
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) arg (void *) Require parameter for a thread function.
//
// FUNCTION VALUE:
// (void *) Required return value.
//
// PURPOSE:
// This function is executued by pthread_create, and waits for and then
// handles signals
//
// DESCRIPTION:
// - Wait for a signal from the list of handled signals.
// - Find the cSig object handling the signal that occured.
// - Execute the handler for the signal.
//
// EXTERNAL VARIABLES:
// None.
//
// PRIOR REQUIREMENTS:
// None.
//
// SEE ALSO:
// - dhsGenSignal
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

void		*sigHandler
(
    void	*		// (in)  The input argument.
)
{
    cSig	*pSigObj;
    int		sig;


    //
    // Unblock the handled signals, but only in this thread.
    //

    pthread_sigmask( SIG_UNBLOCK, &sigSet, NULL );


    for (;;)
    {
	//
	// Wait for a signal.
	//

	sig = sigwait( &sigSet, &sig );


	//
	// find the cSig object assigned to the signal.
	//

	pSigObj = cSig::find( sig );
	if ( pSigObj != NULL )
	{
	    //
	    // Execute the signal handler.
	    //

	    pSigObj->handle();
	}
    }

    return( NULL );
}
