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
// dhsCmd/h/cmdDhs.H
//
// PURPOSE:
// Contains the interface to the cCmdDhs class.
//
// CLASSES:
// cCmdDhs - The interface to the dhs++ library.
// cCmdServerCmd - A command received from a client system.
//
// METHODS
// cCmdDhs::cCmdDhs - Constructor for the cCmdDhs class.
// cCmdDhs::simulate - Set or get the simulation level.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:08  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/04/16 22:46:19  nhill
// Updated the documentation.
// Moved the configuration methods to the dhs++ library.
//
// Revision 1.3  1998/03/13 22:12:09  jaeger
// Added disable and enable methods for disabling and enabling commands.
// Added riForward method for handling reset and init commands.
//
// Revision 1.2  1997/05/01 23:37:30  nhill
// *** empty log message ***
//
// Revision 1.1  1997/04/16 21:51:10  nhill
// Initial revision
//
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef CMDDHS_H
#define	CMDDHS_H

#include "dhs++.H"

//
//***********************************************************************
//+
// CLASS NAME:
// cCmdDhs
//
// PUBLIC METHODS: 
// cCmdDhs - The constructor for the class.
// ~cCmdDhs - The destructor for the class.
// disable - Disable the reception of commands.
// enable - Enable the reception of commands.
// exit - Clean up the DHS library for exit.
// init - Initialize the DHS library.
// retry - Set or get the retry flag.
// simulate - Set or get the simulate flag.
// start - Start the DHS Event loop.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of the DHS++ library.
//
//-
//***********************************************************************
//

class	cCmdDhs
{
    public:
	inline		cCmdDhs( void );
			~cCmdDhs( void );
	static inline void
			disable( void ) { cmdDhsHandler->disable(); };
	
	static inline void
			enable( void ) { cmdDhsHandler->enable(); };
	void		exit( cCmdStatus & );
	void		init( cCmdStatus & );
	static inline bool	
			retry( void ) { return( sdRetry ); };
	static inline bool	
			retry( bool v ) { return( sdRetry = v ); };
	static inline bool
			simulate( void ) { return( sdSimulate ); };
	static inline bool
			simulate( bool );
	void		start( cCmdStatus & );
    protected:
    private:
	static cDhsHandler		// command handler for CMD.
			*cmdDhsHandler;
	cDhs		*sdPDhs;	// The dhs library object.
	static bool	sdRetry;	// True if rerun.
	static bool	sdSimulate;	// True if in simulate mode.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cCmdClientCmd
//
// PUBLIC METHODS: 
// cCmdClientCmd - Constructor for the class.
//
// PURPOSE:
// An object of this class is created for each command forwarded to another
// subsystem
//
//-
//***********************************************************************
//

class	cCmdClientCmd: public cDhsClientCommand
{
    public:
			cCmdClientCmd( cDhsServerCommand & );

    protected:
    private:
	void		done( DHS_CMD_STATUS, char *, const cDhsAvList & );
	void		response( DHS_CMD_STATUS, char *, const cDhsAvList & );

	cDhsServerCommand		// The associated server command.
			*cccServerCommand;
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdDhs::cCmdDhs
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
//  None.
//
// PURPOSE:
// Constructor for the cCmdDhs class
//
// DESCRIPTION:
// Initialize the data members.
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

		cCmdDhs::cCmdDhs
(
    void
)
{
    sdPDhs = NULL;
}

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdDhs::simulate
//
// INVOCATION:
// cCmdDhs::simulate( true );
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
// PRIOR REQUIREMENTS:
// None.
//
// DEFICIENCIES:
// None.
//-
//***********************************************************************
//

bool		cCmdDhs::simulate
(
    bool	value		// (in)  New value for the simulate member.
)
{
    bool	oldSimulate;

    oldSimulate = sdSimulate;
    sdSimulate = value;
    return( oldSimulate );
}

//
//***********************************************************************
//+
// CLASS NAME:
// cCmdServerCmd
//
// PUBLIC METHODS: 
// cCmdServerCmd - Constructor for the class.
// (bool) forward - Forward the command to a subsytem.
// (bool) riForward - Forward a reset or init command to a subsystem.
//
// PURPOSE:
// An object of this type is created for each command received by the
// command server.
//
//-
//***********************************************************************
//

class 	cCmdServerCmd: public cDhsServerCommand
{
    friend class cCmdCommandInfo;

    public:
	inline		cCmdServerCmd( void )
				{
				    cscAborted = false;
				};
	bool		forward( cDhsConnection &, cCmdStatus & );
    	bool		riForward( cDhsConnection &, cCmdStatus & );
 
    protected:
    private:
	bool		cmdAbort( void );
	void		exec( void );

	bool		cscAborted;	// True if the command is aborted.
	static cMutex	cscMutex;
	cCmdClientCmd	*cscPClientCmd;	// Client command pointer.
	const cCmdCommandInfo		// Command info for the command.
			*cscPCmdInfo;
};

#endif // CMDDHS_H
