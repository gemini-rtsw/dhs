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
// dhsHistory/h/hisDhs.H
//
// PURPOSE:
// Contains the interface to the cHisDhs class
//
// CLASSES:
// cHisDhs - Takes care of the dhs++ library setup.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:22  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.4  1998/06/23 19:24:59  cockayne
// Added 'ready-to-receive-commands' stuff, and reset-health class.
//
// Revision 1.3  1998/04/17 22:52:15  nhill
// Moved the configuration data and methods to the cDhs class.
//
// Revision 1.2  1997/10/20 21:48:03  cockayne
// Check-in for install.
//
// Revision 1.1  1997/10/06 20:16:32  cockayne
// Initial revision
//
// Revision 1.2  1997/08/20 21:19:48  nhill
// First alpha release.
//
// Revision 1.1  1997/08/11 22:13:18  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef HISDHS_H
#define	HISDHS_H

//
//***********************************************************************
//+
// CLASS NAME:
// cHisDhs
//
// PUBLIC METHODS: 
// 		cHisDhs		- Constructor for the class.
//		~cHisDhs	- Destructor for the class.
// (void)	exit		- Stop the event loop.
// (void)	init		- Initailize the dhs library.
// (char *)	identity	- Return this applications identity.
// (bool)	retry		- Set and/or return the hdRetry member.
// (bool)	simulate	- Set and/or return the hdSimulate member.
// (void)	start		- Start the dhs event loop.
// (DHS_CONNECT)SSConnect	- Returns statusServer connection. 
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of the DHS++ library setup.
//
//-
//***********************************************************************
//

class	cHisDhs
{
    public:
	//
	// Constructors, destructors.
	//
	
	inline		cHisDhs( void )
			{
			    hdPDhs = NULL;
			};
	inline		~cHisDhs( void )
			{
			    if ( hdPDhs != NULL )
			    {
				delete (cDhs *) hdPDhs;
			    }
			}

	
	//
	// Static member functions. 
	//
	
	inline static const char *
			identity( void )
			{
			    return( cDhs::identity() );
			}
	inline static bool	
			ready( void ) 
			{ 
			    return( hdReady ); 
			};
	inline static bool	
			ready( bool v ) 
			{ 
			    return( hdReady = v ); 
			};
	inline static bool	
			retry( void ) 
			{ 
			    return( hdRetry ); 
			};
	inline static bool	
			retry( bool v ) 
			{ 
			    return( hdRetry = v ); 
			};
	inline static bool
			simulate( void ) 
			{ 
			    return( hdSimulate ); 
			};
	inline static bool
			simulate( bool value )
			{
			    bool	oldSimulate;

			    oldSimulate = hdSimulate;
			    hdSimulate = value;
			    return( oldSimulate );
			};

	
	//
	// Non-static member functions. 
	//
	
	void		exit( cHisStatus & );
	void		init( cHisStatus & );
	void		start( cHisStatus & );


    protected:
    private:
	static boolean	dataServer( int, char ** );


	static bool	hdRetry;	// True if rerun.
	static bool	hdReady;	// True if ready to recieve commands.
	static bool	hdSimulate;	// True if in simulate mode.

	cDhs		*hdPDhs;	// The dhs library object.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisDhsError
//
// PUBLIC METHODS:
// (void) error - Called to start the processing of an error message.
//
// PURPOSE:
// An object of this type is created and executed each time an error
// occurs in the DHS library. This class defines the error methods
// originally declared as pure virtual in the cDhsError base class.
//
//-
//***********************************************************************
//
 
class		cHisDhsError : public cDhsError
{
     private:
	 void            error( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisConnect
//
// PUBLIC METHODS:
// (void) error - Called to start the processing of a connection message.
//
// PURPOSE:
// Object to deal with DHS library connection requests.
//
//-
//***********************************************************************
//
 
class   cHisConnect: public cDhsError
{
     private:
	 void            error( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisDisconnect
//
// PUBLIC METHODS:
// (void) error - Called to start processing an disconnect message.
//
// PURPOSE:
// Object to deal with DHS library disconnect requests.
//
//-
//***********************************************************************
//
 
class   cHisDisconnect: public cDhsError
{
    private:
        void            error( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisDefaultCmd
//
// PUBLIC METHODS:
// (void) exec - called to start the processing of an unknown command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handles unknown command received by dhsHistory
//
//-
//***********************************************************************
//
 
class   cHisDefaultCmd: public cDhsServerCommand
{
    public:
        void            exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisDebugCmd
//
// PUBLIC METHODS:
// (void) exec - Called to start the processing of the debug command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the debug command
//
//-
//***********************************************************************
//
 
class cHisDebugCmd: public cDhsServerCommand
{
    public:
        void            exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisExitCmd
//
// PUBLIC METHODS:
// (void) exec - Begin execution of the exit command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Handle the exit command.
//
//-
//***********************************************************************
//
 
class cHisExitCmd: public cDhsServerCommand
{
    public:
        void            exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisInitCmd
//
// PUBLIC METHODS:
// (void) exec - Called to start the processing of the init command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the init command
//
//-
//***********************************************************************
//
 
class cHisInitCmd: public cDhsServerCommand
{
    public:
        void            exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisPingCmd
//
// PUBLIC METHODS:
// (void) exec - Called to start the processing of the simulate command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the ping command
//
//-
//***********************************************************************
//
 
class cHisPingCmd: public cDhsServerCommand
{
    public:
        void            exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisResetHealthCmd
//
// PUBLIC METHODS:
// (void) exec - Called to start the processing of the resetHealth command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the resetHealth command
//
//-
//***********************************************************************
//
 
class cHisResetHealthCmd: public cDhsServerCommand
{
    public:
        void            exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisSimulateCmd
//
// PUBLIC METHODS:
// (void) exec - Called to start the processing of the simulate command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the simulate command
//
//-
//***********************************************************************
//
 
class cHisSimulateCmd: public cDhsServerCommand
{
    public:
        void            exec( void );
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cHisTestCmd
//
// PUBLIC METHODS:
// (void) exec - Called to start the processing of the test command.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Takes care of processing the test command
//
//-
//***********************************************************************
//
 
class cHisTestCmd: public cDhsServerCommand
{
    public:
        bool            cmdAbort( void );
        void            exec( void );
    protected:
    private:
};
#endif // HISDHS_H
