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
// dhsStorage/h/stoGet.H
//
// PURPOSE:
//
// CLASSES:
// cStoGet - Stores file information.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:31:28  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  1999/10/12 23:21:46  nhill
// Removed the unused createPath member function.
//
// Revision 1.7  1999/06/16 23:49:50  nhill
// Changed to only wait until gets for one action complete. Added a
// maximum number of gets per action.
//
// Revision 1.6  1998/06/24 16:06:22  cockayne
// Modified checkPress() macro.
//
// Revision 1.5  1998/05/15 17:16:32  cockayne
// Modifications to support compression/uncompression of files.
//
// Revision 1.4  1998/04/14 16:55:20  cockayne
// Cleaned up, added comments.
//
// Revision 1.3  1998/04/08 20:59:00  cockayne
// Modified declaration of done() method for change to dhs++ library.
//
// Revision 1.2  1998/03/31 16:58:04  cockayne
// Modified #include files for Rational Rose Analyzer.
//
// Revision 1.1  1998/03/10 18:28:08  cockayne
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef STOGET_H
#define	STOGET_H

#include "globals.H"
#include "stoDbm.H"
#include "stoMedia.H"
#include "stoNode.H"


#define	STO_GET_MAX		5	// Maximum total number of gets.
#define	STO_ACTION_GET_MAX	3	// Maximum number of gets for one 
					// action.

#undef checkPress
#define checkPress( fn, status, label, ret )				\
    {									\
	int		s;						\
	if ( ( s = fn ) != PR_SUCCESS )					\
	{								\
	    status.E_PRESS( status, label );				\
	    status += pr_msg;						\
	    sgPressLibMutex.unlock();					\
	    ret;							\
	}								\
    }



//
//***********************************************************************
//+
// CLASS NAME:
// cStoGet
//
// PUBLIC METHODS:
// (cStoGet)		cStoGet		- Constructor.
// ()			~cStoGet	- Deconstructor.
// (void)		waitUntilGetsComplete - Waits until all gets are done.
// (AM_FILE *)		setPAmFile	- Set ptr to am library file struct.
// (AM_FILE *)		getPAmFile	- Get ptr to am library file struct.
// (cStoMedia *)	getPMedia	- Get ptr to Media object. 
// (DHS_CMD_STATUS)	getGetStatus	- Get the returned 'get' status.
// (char *)		getGetString	- Get the returned 'get' string.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Facilitates retrieval of data files. 
//
// This class allows for multiple concurrent file retrievals to be 
// running at one time. The number of concurrent file retrievals is 
// set by the STO_GET_MAX symbol. The private done() method is executed
// to deal with the data as it arrives. 
//-
//***********************************************************************
//
 
class cStoGet : public cDhsGetDataset
{
    public:
	//
	// Constructors. Destructors. 
	//

			cStoGet( AM_FILE *, cStoMedia *, cStoRetrieve * );
			~cStoGet( void );


	//
	// Static methods. 
	//
	
	static void	waitUntilGetsComplete( cStoRetrieve * );

	
	//
	// Set and Get methods. 
	//
	
	AM_FILE		*setPAmFile( AM_FILE * );
	inline AM_FILE	*getPAmFile( void )
			{
			    return( sgPAmFile );
			};
	inline cStoMedia 
			*getPMedia( void )
			{
			    return( sgPMedia );
			};
	inline DHS_CMD_STATUS
			getStatus( void )
			{
			    return( sgStatus );
			};
	inline char	*getString( void )
			{
			    return( sgString );
			};
	inline cDhsAvList
			*getAvList( void )
			{
			    return( sgPAvList );
			};
	inline cStoAction	
			*getPAction( void )
			{
			    return( sgPAction );
			};


    protected:
    private:
	static void	*execThread( void * );
	static cCond	sgCond;
	static cMutex	sgPressLibMutex;
	static int	sgCount;


	void 		done( DHS_CMD_STATUS, char *, cDhsAvList & );

	AM_FILE		*sgPAmFile;
	cStoMedia	*sgPMedia;
	DHS_CMD_STATUS	sgStatus;
	char		*sgString;
	cDhsAvList	*sgPAvList;
	cStoRetrieve	*sgPAction;
};
#endif // STOGET_H
