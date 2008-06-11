//
//***********************************************************************
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//
// (c) 1997.				(c) 1997
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
// dhsQlServer/h/qlsFrame.H
//
// PURPOSE:
// Contains the interface to the cQlsFrame class.
//
// CLASSES:
// cQlsFrame - Contains information about a frame in a dataset.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:28:54  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.11  1999/02/01 21:11:14  nhill
// Removed the unused qdfModified flag.
//
// Revision 1.10  1999/01/20 23:21:15  nhill
// Added methods modified, modfiedClear, and regionsAdd to cQlsFrame.
// Added a list of modified regions to cQlsFrame.
//
// Revision 1.9  1998/05/27 16:35:12  nhill
// Added a list of keyword positions, indexed by keyword to the cQlsFrame
// class.
//
// Revision 1.8  1998/04/27 20:51:26  nhill
// Changed to use separate header and data memory mapped files.
//
// Revision 1.7  1997/12/04 23:59:37  nhill
// Added attribute qfdCounter to the cQlsFrame class.
//
// Revision 1.6  1997/11/30 21:20:20  nhill
// Removed the cQlsFrameKey class.
//
// Revision 1.5  1997/10/30 18:07:40  nhill
// Removed the file descriptor from the class, since it is no longer left
// open while not in use.
//
// Revision 1.4  1997/10/22 17:47:44  nhill
// Checkpoint check in.
//
// Revision 1.3  1997/10/01 17:26:34  nhill
// Second alpha release.
//
// Revision 1.2  1997/08/26 21:53:50  nhill
// Made class cQlsDataset a friend of class cQlsFrame.
//
// Revision 1.1  1997/08/20 21:21:25  nhill
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef QLSFAME_H
#define	QLSFAME_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <vector>
#include "genRwLock.H"

class	cQlsDataset;

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsFrame
//
// PUBLIC METHODS: 
//		cQlsFrame	- Constructor for the class.
//		~cQlsFrame	- Destructor for the class.
// (cQlsFrame &)operator=	- Assignment operator.
// (void)	cardAdd		- Add a new card to the frame.
// (char *)	dataFName	- Accessor function to return the file the
//				  frame is using for output of data.
// (void)	dataMerge	- Add the data part of a FITS HDU into
//				  a frame.
// (cQlsDataset &)		- Accessor function to return the parent
//		dataset		  dataset.
// (char *)	frameId		- Accessor function to return the file the
//				  frame is using for output data.
// (void)	headerCopy	- Copy the header data from one cQlsFrame 
//				  structure to another.
// (char *)	headerFName	- Accessor function to return the file the
//				  frame is using for output of header data.
// (void)	headerMerge	- Add the header part of a FITS HDU into a 
//				  frame.
// (bool)	modified	- Return a flag indicating if the image was
//				  modified, or indicate that a region of the 
//				  image was modified.
// (void)	modifiedClear	- Clears the list of modified regions.
// (void)	regionsAdd	- Add the list of modified regions to an av 
//				  list.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Contains information about a frame within a dataset.
//
//-
//***********************************************************************
//

class 	cQlsFrame : public cRwLock
{
    friend class cQlsBufIndex;
    friend class cQlsDataset;
    public:
			cQlsFrame( cQlsDataset *, char *, char * );
	inline		~cQlsFrame( void );
	bool		operator<( const cQlsFrame &) const ;
	void		cardAdd( char *, cQlsStatus & );
	void		cardAdd( char *, int, cQlsStatus & );
	inline const char	
			*dataFName( void ) const
			{
			    return( qfdDataFName );
			}
	void		dataMerge( FITSFILE *, bool, void *, cQlsStatus & );
	inline cQlsDataset	
			&dataset( void ) const
			{
			    return( *qfdDataset );
			}
	inline const char
			*frameId( void ) const
			{
			    return( qfdFrameId );
			}
	void		headerCopy( cQlsFrame &, cQlsStatus & );
	inline const char	
			*headerFName( void ) const
			{
			    return( qfdHeaderFName );
			}
	void		headerMerge( FITSFILE *, bool, cQlsStatus & );
	inline bool	modified( void ) const
			{
			    return( qfdModifiedRegions.size() != 0 );
			}
	bool		modified( long[], long[], cQlsStatus & );
	void		modifiedClear( cQlsStatus & );
	void		regionsAdd( cDhsAvList &, cQlsStatus & );
    protected:
    private:
	typedef std::map< std::string, int, std::less<std::string> >
			tKeywordList;
	typedef tKeywordList::iterator
			iKeywordList;

	void		allocData( cQlsStatus & );
	void		dataCopy( FITSFILE *, long [], long[], void *, 
				cQlsStatus & );
	void		resizeHeader( int, cQlsStatus & );


	int		qfdBytePix;	// Number of bytes per pixel.
	static int	qfdCounter;	// counter of frames created.
	char		*qfdData;	// Pointer to the data buffer.
	char		qfdDataFName[PATH_MAX];	
					// Name of the memory map file.
	cQlsDataset	*qfdDataset;	// The parent dataset.
	long		qfdDataSize;	// Size of the data part of the file.
	char		*qfdFrameId;	// Frame id.
	char		*qfdHeader;	// Buffer containing the header.
	char		qfdHeaderFName[PATH_MAX];	
					// Name of the memory map file.
	ulong		qfdHeaderSize;	// Size of the header.
	bool		qfdLastIsEnd;	// Is the last card an end card?
	std::vector<long *>	qfdModifiedRegions;
					// This is a list of regions that
					// have been modified since the last
					// time the QLTs were notified.
					// The list contains a pointer to a
					// list of longs, containing X origin,
					// X region size, Y origin, Y region
					// size, Z origin, ...
	long		qfdNaxes;	// Number of axes in the data array.
	long		qfdNaxis[7];	// Size of each axis.
	ulong		qfdNextCard;	// Location to put the next card.
	int		qfdNumCards;	// Number of cards in the header.
	tKeywordList 	qfKeywordList;	// The list of keywords in the header.
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsFrame::~cQlsFrame
//
// INVOCATION:
// n.a.
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// None.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Destructor for the class
//
// Frees all resources allocated to the object.
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

		cQlsFrame::~cQlsFrame
(
    void
)
{
    if ( qfdHeader != NULL )
    {
	munmap( qfdHeader, qfdHeaderSize );
    }

    if ( qfdData != NULL )
    {
	munmap( qfdData, qfdDataSize );
    }

    unlink( qfdDataFName );	
    unlink( qfdHeaderFName );	

    if ( qfdFrameId != NULL )
    {
	free( qfdFrameId );
    }
}
#endif // QLSFAME_H
