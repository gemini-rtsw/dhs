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
// dhsCmd/h/cmdCommandInfo.H
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
// Revision 1.6  1998/06/26 19:27:05  nhill
// Added function csExit.
//
// Revision 1.5  1998/06/26 19:00:44  nhill
// Changed method appliesToCmd to be a non-static, const function.
// Changed the way arguments are passed to the cCmdAttributeInfo constructor.
//
// Revision 1.4  1998/03/13 22:14:16  jaeger
// Added appliesToCmd, reReSend, reset, resetHealth and test methods.  Added
// a lock so only one reset can be processed at a given time.
//
// Revision 1.3  1997/04/28 22:54:19  nhill
// Added shutdown of client systems on exit.
//
// Revision 1.2  1997/04/24 19:39:52  nhill
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

#ifndef CMDCOMMANDINFO_H
#define	CMDCOMMANDINFO_H

#include <map>
#include <list>
#include <string>

#include "dhsGenSubsystem.H"
#define	DHS_NO_BULK
#define	DHS_NO_CLIENT
#include "dhs++.H"
class	cCmdServerCmd;

//
//***********************************************************************
//+
// CLASS NAME:
// cCmdAttributeInfo
//
// PUBLIC METHODS: 
// (type) method - description
//
// PURPOSE:
// Contains information about an attribute.
//
//-
//***********************************************************************
//

class 	cCmdAttributeInfo
{
    friend class cCmdCommandInfo;

    public:
	typedef map< string, cCmdAttributeInfo*, less<string> > 
			tAttributeList;
	typedef tAttributeList::iterator
			iterator;
	typedef tAttributeList::const_iterator
			constIterator;
	
	const char	*name( void )
				{
				    return( caiName );
				};
	bool		optional( void )
				{
				    return( caiOptional );
				};
	DHS_DATA_TYPE	type( void )
				{
				    return( caiType );
				};
    protected:
    private:
			cCmdAttributeInfo( const char *, bool, DHS_DATA_TYPE );
			~cCmdAttributeInfo( void );

	char		*caiName;	// Attribute name.
	bool		caiOptional;	// Is the attribute optional?
	DHS_DATA_TYPE	caiType;	// Type of data for the attribute.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cCmdCommandInfo
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

class 	cCmdCommandInfo
{
    friend class	cCmdAttributeInfo;

    public:
	typedef map< string, cCmdCommandInfo *, less<string> > 
			tCommandMap;
	typedef tCommandMap::iterator	
			iterator;

	inline bool	abortable() const
				{
				    return( cciAbortable );
				}
	cCmdAttributeInfo
			*attribFind( const char *, cCmdStatus & ) const;
	static void	config( cCmdStatus & );
	void		exec( cCmdServerCmd &, cCmdStatus & ) const;
	static void	exit( cCmdStatus & );
	static cCmdCommandInfo
			*find( const char *, cCmdStatus & );
	inline const char	
			*name( void ) const
				{
				    return( cciName );
				};
	static inline void	
			simulate( bool value )
				{
				    cciSimulate = value;
				};
	static inline bool	
			simulate( void )
				{
				    return( cciSimulate );
				};
    protected:
    private:
			cCmdCommandInfo( int, const char ** );
			~cCmdCommandInfo( void );
        boolean		appliesToCmd( cDhsServerCommand &, const char *,
				cCmdStatus & ) const;
	static boolean	attribute( int, const char ** );
	void		attributeAdd( cCmdAttributeInfo *, cCmdStatus & );
	static boolean	command( int, const char ** );
	void		csExit( cDhsServerCommand &, cCmdStatus & ) const;
    	void		debugSet( cDhsServerCommand &, cCmdStatus & ) const;
	void		reSend( cCmdServerCmd &, cCmdStatus & ) const;
    	void		riReSend( cCmdServerCmd &, cCmdStatus & ) const;
	void		reset( cDhsServerCommand &, cCmdStatus & ) const;
	void		resetHealth( cDhsServerCommand &, cCmdStatus & ) const;
    	void		simulateSet( cDhsServerCommand &, cCmdStatus & ) const;
	void		subsystemAdd( cCmdSubsystem *, cCmdStatus & );
	void		systemExit( cCmdStatus & ) const ;
	void		test( cDhsServerCommand &, cCmdStatus & ) const;
	void		verify( const cDhsServerCommand &, cCmdStatus & ) const;

	bool		cciAbortable;	// Is the command abortable.
	cCmdAttributeInfo::tAttributeList// List of attributes for the command.
			cciAttributeList;
	static tCommandMap		// List of all commands.
			cciCommandMap;
	static cMutex	cciMutex;	
	char		*cciName;	// Name of the command.
	static bool	cciSimulate;	// True if simulation is on.
	bool		cciSubsystemAttrib;// Could a subsystem list come with
					// the command?
	cDhsSubsystem::tSysList		// List of subsystems to which the 
			cciSubsystemList;// command applies.
};
#endif // CMDCOMMANDINFO_H
