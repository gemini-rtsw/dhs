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
// dhsData/h/get.H
//
// PURPOSE:
// Contains the definitions for bulk data gets to the DHS data server.
//
// CLASSES:
// cDtsDhsGet - Get request responder.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.7  2000/07/27 20:10:55  dunn
// Added standalone features.
//
// Revision 1.6  1998/05/04 22:44:37  dunn
// Added cdmCompression flag.
//
// Revision 1.5  1998/03/06 19:09:29  cockayne
// Modified to return (in addition to the file itself) the name of the file,
// on response to a get.
//
// Revision 1.4  1997/09/29 16:18:47  dunn
// Code cleanup.
//
// Revision 1.3  1997/08/29 23:09:53  dunn
// Reordered methods, added messages, added deconstructor, got rid of unused class.
//
// Revision 1.2  1997/07/29 22:04:43  dunn
// Added cdmMutexPr for press lib.
//
// Revision 1.1  1997/07/10 18:28:16  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef GET_H
#define	GET_H

#include "dhs++.H"
#include "dhsGenStatus.H"


//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsGet
//
// PUBLIC METHODS: 
// (void) asisRead - read in a file as is.
// (void) decompressRead - read in a file and decompress it.
// (void) exec - Called by get handler.
// (void) fileRead - Reads in the requested file.
// (void) fileReadAlone - Reads in the requested file, searching the list.
// (void) fitsCheck - check that the file is a fits file.
// (void) headerRead - read in a fits file header.
// (void) requestRemotely - requests a file from a remote dataServer.
//
// PURPOSE:
// Handles a get request.
//
//-
//***********************************************************************
//

class 	cDtsDhsGet : public cDhsGetRequest, public cDtsDhsName
{
    public:
	void		asisRead(  cDtsStatus &, void **, char *, int & );
	void		decompressRead(  cDtsStatus &, void **, char *, int &,
				void **, bool );
	void		exec();
	void		fileRead( cDtsStatus &, char **, void **, int & );
	void		fileReadAlone( cDtsStatus &, char **, void **, int & );
	void		fitsCheck(  cDtsStatus &, void **, int, char );
	void		headerRead(  cDtsStatus &, void **, FILE *, int & );
	void		requestRemotely( cDtsStatus &, char **, void **, 
				struct dbprocess *, struct ad_file *, int &,
				int & );

	class	gStatus: public cDhsStatus	// Get return response
	{
	    public:
		msgPrefix( "Get Request" );
		msgId( S_FILENAME );
		msgId( E_INV_COMP );
		msgId( E_INV_MED );
		msgId( E_INV_TYPE );
		msgId( E_NO_HEADER );
		msgId( E_NO_FILE );
		msgId( E_NOT_FITS );
		msgId( E_FILE_UNAVAIL );
		msgId( E_UNCOMP_FAIL );
	    protected:
	    private:
	};
	gStatus		getStatus;		// Response string.
	char		cdmCompression;		// Compression flag.

    protected:
	inline		~cDtsDhsGet( void )
			    {
				VOID;
			    };
    private:
	static cMutex	cdmMutexPr;	// Mutex to protect the press library.
	cDhsGetDataset	*pGetRequest;	// Get remote type pointer.
	bool		remoteRead;	// Decompression buffer.
	

};
#endif // GET_H
