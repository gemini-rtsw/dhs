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
// dhsStorage/h/stoStage.H
//
// PURPOSE:
// Media requests are the reason Storage Server exists. This file
// contains code to provide a list of media staging directories.
//
// CLASSES:
// cStoStage - Store staging directory information in a list. 
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.7  2000/12/19 19:37:34  nhill
// Updated for workshop 5.
//
// Revision 1.6  1999/06/16 22:54:06  nhill
// Bug fixes.
//
// Revision 1.5  1999/02/24 23:56:59  jaeger
// Added findByDirectory() and find().
//
// Revision 1.4  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.2  1998/03/26 21:23:04  cockayne
// Checkpointing.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOSTAGE_H
#define	STOSTAGE_H

#define NO_FIND_CREATE
#include "genListOf.H"

#include "globals.H"
#include "stoDbm.H"
#include "stoEpicsRecord.H"


//
// Define EPICS record array size.  Note that this value must match the
// the EPICS array size if the EPICS database is being used, and must
// match the size specified in the console configuration file.
//

#define STO_MAX_STAGE_EPICS_ARRAY	10


//
// Define EPICS record names - NOTE: some have embedded integer formats.
//

#define STO_ERS_ARRAY_SIZE		"stageArraySize"
#define STO_ERS_IN_USE			"stage%d:inUse"
#define	STO_ERS_REQUEST_ID		"stage%d:reqId"
#define	STO_ERS_UNIT_NAME		"stage%d:unitName"
#define	STO_ERS_DIR_NAME		"stage%d:dirName"



//
//***********************************************************************
//+
// CLASS NAME:
// cStoStage
//
// PUBLIC METHODS:
// (cStoStage)	cStoStage	- Constructor (three flavours).
// ()		~cStoStage	- Deconstructor.
// (int)	selectEpicsRow	- Pick a row for an object in the Epics array.
// (void)	init		- Initializes the list of media stage objects.
// (void)	exit		- Nuke list, free resources held by the class. 
// (void)	clear		- 'Clear' a stage object, freeing the direc-
//				  tory it represents for use by another unit.
// (AM_MSTAGE *) setPAmStage	- Set ptr to am library media stage struct.
// (AM_MSTAGE *) getPAmStage    - Get ptr to am library media stage struct.
// (int)	getMediaRequestId - Get object's media request identifier.
// (bool)	getInUse	- Get value of object's in-use flag.
// (char *)	getMediaUnitName - Get value of object's media unit name.
// (char *)	getDirectory	- Get name of directory object represents.
// (void)	setInUse	- Set value of in-use flag.
// (void)	setMediaRequestId - Set value of object's media request id.
// (void)	setMediaUnitName - Set value of object's media unit name.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Stores information about media staging directories in a list.
//
// Objects of this type represent subdirectories of the media staging
// area. Methods are available to set and get information about the current
// use of the directory.  
//-
//***********************************************************************
//
 
class cStoStage: 
	public cListOf< cStoStage, int, cStoStatus >
{
    public:
			cStoStage( void );
			~cStoStage( void );

	static int	selectEpicsRow( void );
	static void	init( cStoStatus & );
	static void	exit( cStoStatus & );
	void		clear( void );
	

	static cStoStage
	    		*find( int, char * );
	static cStoStage
	    		*findByDirectory( char * );
	
	static AM_MSTAGE	
			*setPAmStage( AM_MSTAGE * );
	AM_MSTAGE	*getPAmStage( void )
			{
			    return( ssPAmStage );
			};
	

	//
	// Set and get methods for media object data.
	//
	
	inline int	getMediaRequestId( void )
			{
			    return( ssPAmStage->amsMediaRequestId );
			};
	inline bool	getInUse( void )
			{
			    return( ssInUse );
			};
	inline char	*getMediaUnitName( void )
			{
			    return( ssMediaUnitName.value() );
			};
	inline char	*getDirectory( void )
			{
			    return( ssDirectory.value() );
			};
	inline void	setInUse( bool b )
			{
			    ssInUse = b;
			};
	inline void	setMediaRequestId( int mediaRequestId )
			{
			    ssMediaRequestId = mediaRequestId;
			};
	inline void	setMediaUnitName( char * mediaUnitName )
			{
			    if ( ssMediaUnitName.value() != NULL )
			    {
				gen_free( ssMediaUnitName.value() );
			    }
			    ssMediaUnitName = strsav( mediaUnitName );
			};
	inline void	setDirectory( char * mediaUnitName )
			{
			    if ( ssDirectory.value() != NULL )
			    {
				gen_free( ssDirectory.value() );
			    }
			    ssDirectory = strsav( mediaUnitName );
			};


    protected:
    private:
	static cStoEpicsRecord<int, cStoStage, cStoStatus>
			ssArraySize;

	//
	// EPICS free array row list. 
	//

	static bool	ssEpicsFreeList[STO_MAX_STAGE_EPICS_ARRAY];
					// The list of free/non-free EPICS 
					// stage array rows. 


	//
	// A flag that is set during initialization.  If it is set
	// then the staging directory is being used.
	//

	boolean		ssInUse;
	
	
	//
	// Pointer to am library media stage structure. 
	// Index into EPICS Media Request Array.
	//
	
	AM_MSTAGE	*ssPAmStage;	// am library stage struct. 
	int		ssEpicsArrayRow;// Row in EPICS array. 


	//
	// Some am library information which is kept in EPICS. 
	//
	
	cStoEpicsRecord<bool, cStoStage, cStoStatus>
			ssEpicsRecInUse;// Is the epics record in use?
	cStoEpicsRecord<int, cStoStage, cStoStatus>
			ssMediaRequestId;
					// Media Request Identifier.
	cStoEpicsRecord<char *, cStoStage, cStoStatus>
			ssMediaUnitName;// Media Unit Name.
	cStoEpicsRecord<char *, cStoStage, cStoStatus>
			ssDirectory;	// Directory name.
};
#endif // STOSTAGE_H
