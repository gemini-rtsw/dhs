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
// dhsData/h/seq.H
//
// PURPOSE:
// Header file for the initialization class cDtsDhsSequence.
//
// CLASSES:
// cDtsDhsSequence - Sequence class.
//
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.4  1998/05/04 22:44:19  dunn
// Added resetHealth command.
//
// Revision 1.3  1997/09/17 22:26:41  dunn
// Added the testing routines.
//
// Revision 1.2  1997/08/29 23:11:43  dunn
// Got rid of source stuff and changed name of init routine to initReset.
//
// Revision 1.1  1997/08/19 22:33:26  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DTSSEQ_H
#define	DTSSEQ_H

#include "dhs++.H"

//
//  Defines
//

#define DTS_ABORT_TEST_CMD	"cmdAbort"
#define	DTS_INIT_CMD		"init"
#define	DTS_DEBUG_CMD		"debugLevel"
#define DTS_EXIT_CMD		"exit"
#define DTS_PING_CMD		"ping"
#define	DTS_RESET_CMD		"reset"
#define	DTS_RESET_HEALTH_CMD	"resetHealth"
#define	DTS_SIM_CMD		"simulateLevel"
#define DTS_TEST_CMD		"test"


//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsSequence
//
// PUBLIC METHODS: 
// (void) debugLevel - Set the debug level.
// (void) exec - Main sequence handler.
// (void) exit - Exit nicely.
// (void) initReset - Initialize or reset.
// (void) resetHealth - Reset health.
// (void) simulateLevel - Set the simulate level.
// (void) test - Start a test.
// (void) cmdAbort - Stop tests.
// (void) ping - Return a response.
//
// PURPOSE:
// Handles all sequence commands.
//
//-
//***********************************************************************
//

class 	cDtsDhsSequence: public cDhsServerCommand, public cDtsDhsName
{
    public:
	class	sStatus: public cDhsStatus	// Command return response
	{
	    public:
			msgPrefix( "dts" );
			msgId( E_SEQ_CMD );
			msgId( E_DEBUG_COMMAND );
			msgId( E_SIMULATE_COMMAND );
			msgId( S_TEST_SUCCESS );
			msgId( S_TEST_FAIL );
	    protected:
	    private:
	};
	sStatus		seqStatus;		// Response string.

	inline void	continueTesting ( bool x )
			    {
				cdsContTest = x;
			    };
	inline bool	continueTesting ( void )
			    {
				return( cdsContTest );
			    };
				
    protected:
    private:
	inline bool	cmdAbort( void )
			    {
				cDtsStatus 	status;

				status.S_CMD_RECEIVED( status, "abort test",
					"N/A", "N/A" );

				if ( streq( name(), DTS_TEST_CMD ) )
				{
				    cdsContTest = FALSE;
				    return( TRUE );
				}
				else
				{
				    status.E_ABORTABLE( status, name(),
					    sourceName(), sourceAddr() );
				    status.display();
				    return( FALSE );
				}
			    };

	static bool	cdsContTest;		// Indicates continue testing.

	void		debugLevel( cDtsStatus & );
	void		exec( void );
	void		exit( cDtsStatus & );
	void		initReset( cDtsStatus & );
	void		resetHealth( cDtsStatus & );
	void		simulateLevel( cDtsStatus & );
	void		test( cDtsStatus & );
};

#endif // DTSSEQ_H
