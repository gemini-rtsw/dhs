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
// dhsCommand/h/cmdSubsystem.H
//
// PURPOSE:
// Whatever
//
// CLASSES:
// <class> - Description
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:33  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:08  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.5  1998/03/13 22:13:01  jaeger
// Added exitAll and pingAll methods.  Removed cssConnected, and connected
// method.  Whether or not a connection is open can be determined from the
// through the cDhsSubsystem class.
//
// Revision 1.4  1997/06/13 20:46:51  nhill
// Changed to use a new version of the cDhsStatus class.
//
// Revision 1.3  1997/05/01 17:49:49  nhill
// Added a subsystemExit routine to the cCmdSubsystem class.
//
// Revision 1.2  1997/04/24 19:40:54  nhill
// Added some "const" keywords.
//
// Revision 1.1  1997/04/16 21:51:10  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef CMDSUBSYSTEM_H
#define	CMDSUBSYSTEM_H

#include "dhsGenStatus.H"
#include "dhsGenSubsystem.H"
#include "cmdDhs.H"

//
//***********************************************************************
//+
// CLASS NAME:
// name
//
// PUBLIC METHODS: 
// (type) method - description
//
// PURPOSE:
// Statement of purpose  
// First line should be a summary not terminated with a period.
//
//-
//***********************************************************************
//

class cCmdSubsystem: public cDhsSubsystem
{
    public:
	static void	config( cStatus & );
	void		connect( cCmdStatus & );
	cDhsConnection	&connect( void )
				{
				    return( cssConnect );
				};
	inline bool	connected( void )
				{ 
				    return( cssConnect.isOpen() ); 
				};
	static void	connectAll( cCmdStatus & );
	static void	exitAll( cCmdStatus & );
    	static void	pingAll( boolean * );
	static inline void	
			subsystemExit( bool value )
				{
				    cssSubsystemExit = value;
				};
	static inline bool
			subsystemExit( void )
				{
				    return( cssSubsystemExit );
				};
	inline static cDhsSubsystem
			*subsystemFind( const char * name, 
				cDhsSubsystem::cStatus & status)
				{
				    return( cDhsSubsystem::subsystemFind( 
					    name, status ) );
				};
	inline static cCmdSubsystem
			*subsystemFind( const char *, cCmdStatus & );
    protected:
    private:
			cCmdSubsystem( const char *, const char *, 
				const char *, const char * );
			~cCmdSubsystem( void );
	static boolean	subsystem( int, char ** );

	char		*cssAddress;	// Subsystem address.
	char		*cssCommandLine;// Subsystem command line.
	cDhsConnection	cssConnect;	// Connection to the server.
	char		*cssServerName;	// IMP name of the server.
	static bool	cssSubsystemExit;// Exit subsystems on exit?
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cCmdSubsystem::subsystemFind
//
// INVOCATION:
// pCmdSubsystem = cCmdSubsystem::subsystemFind( name, status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (>) name (char *) Name of the subystem to find.
// (!) status (cCmdStatus &) Function return status.
//
// FUNCTION VALUE:
// (pCmdSubsystem) Subsystem located.
//
// PURPOSE:
// Interface to the cDhsSubystem::subystemFind function
//
// DESCRIPTION:
// Description of the algorithm.
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

cCmdSubsystem	*cCmdSubsystem::subsystemFind
(
    const char	*name,		// (in)  Name of the subystem to find.
    cCmdStatus	&status		// (mod) Function return status.
)
{
    cDhsSubsystem::cStatus	ssStatus;
    cCmdSubsystem		*pSubsystem;


    pSubsystem = (cCmdSubsystem *) cDhsSubsystem::subsystemFind( name, 
	    ssStatus );
    if ( ssStatus == ssStatus.S_SUCCESS )
    {
	return( pSubsystem );
    }
    else
    {
	status.E_SUBSYSTEM( status, "pSubsystem = (cCmdSubsystem *) "
		"cDhsSubsystem::subsystemFind( name, ssStatus )" , 
		ssStatus.message() );
	status.display();
	return( NULL );
    }
}
#endif // CMDSUBSYSTEM_H
