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
// dhsQlServer/h/qlsDataset.H
//
// PURPOSE:
// Contains the interface to the cQlsDataset class.
//
// CLASSES:
// cQlsDataset - Keeps track of all datasets currently known the the 
//		status server.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:28:51  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.1.1.1  1999/08/23 19:58:25  cvs-tuc
// Initial install into CVS of dhs-0.16
//
// Revision 1.11  1999/03/03 23:35:38  nhill
// Fixed a bug in the cQlsDataset::deRef function that allowed two
// threads to delete the same dataset.
//
// Revision 1.10  1999/01/20 23:20:38  nhill
// Added method cQlsDataset::delteWait.
//
// Revision 1.9  1998/06/16 22:14:16  nhill
// Added initialization of the frame counter. This just makes the frame
// file names have consistant names, which makes testing easier.
//
// Revision 1.8  1998/05/14 20:25:22  nhill
// Added an extension number to the frameFind prototype.
//
// Revision 1.7  1997/12/04 23:57:34  nhill
// Changed the last method to public.
//
// Revision 1.6  1997/11/30 21:18:36  nhill
// Changed the cQlsDataset class to use the cListOf class to keep the list
// of datasets.
//
// Revision 1.5  1997/10/30 18:07:11  nhill
// Removed references to the unused/unimplemented init method.
//
// Revision 1.4  1997/10/22 17:47:02  nhill
// Checkpoint check in.
//
// Revision 1.3  1997/10/01 17:26:15  nhill
// Second alpha release.
//
// Revision 1.2  1997/08/20 21:19:20  nhill
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

#ifndef QLSDATASET_H
#define	QLSDATASET_H

//
// If it hasn't been defined already, define a FITSFILE macro. This allows
// the header to be included without requiring the cfitsio headers to be
// included. In files where the functions using the FITSFILE are used or
// defined, a FITSFILE macro must be defined as "fitsfile" before this 
// header file is included.
//

#ifndef	FITSFILE
#define	FITSFILE void
#endif	// FITSFILE

#include <string>
#include <map>

#include "genCond.H"
#include "genListOf.H"

#include "qlsStream.H"
#include "qlsFrame.H"
class	cDhsPutRequest;

//
//***********************************************************************
//+
// CLASS NAME:
// cQlsDataset
//
// PUBLIC METHODS: 
//		cQlsDataset	- Constructor for the class.
//		~cQlsDataset	- Destructor for the class.
// (void)	chunkAdd	- Add a chunk to a frame and notify everything
//				  about the new data.
// (char *)	datasetName	- Accessor function to return the name of
//				  the dataset.
// (void)	deleteWait	- Wait until the reference count for a dataset
//				  drops to one, and then deref the dataset so
//				  it will be deleted.
// (void)	deRef		- Decrement the dataset reference count.
// (static void)exit		- Called at exit to delete all cQlsDataset's.
// (void)	forEachFrame	- Execute the specified function for each
//				  frame in the dataset.
// (bool)	hasStream	- Determin if the specified stream is 
//				  associated with the dataset.
// (void)	ref		- Increment the dataset reference count.
// (void)	refCount	- Return the dataset reference count.
// (void)	setQlStream	- Add a list of streams to a dataset.
// (void)	streamAdd	- Add all stream names to an av list.
//
// PUBLIC TYPES:
// tFrameList - STL map of cQlsFrame objects indexed by frame id and version.
// iFrameList - Iterator into the tFrameList type.
// tForEachFrameFn - Function to be passed to the forEachFrame method.
//
// PUBLIC DATA: (">" input, "!" modified, "<" output)
// None.
//
// PURPOSE:
// Contains information about a dataset known to the quick look server
//
//-
//***********************************************************************
//

class cQlsDataset: 
	public cRwLock,
	public cListOf< cQlsDataset, std::string, cQlsStatus >
{
    public:
	friend class cQlsFrame;

	typedef std::map < std::string, cQlsFrame *, std::less<std::string> >
			tFrameList;
	typedef tFrameList::iterator
			iFrameList;
	typedef void (*tForEachFrameFn)( cQlsFrame &, void *, cQlsStatus & );

			cQlsDataset( std::string, bool = true );
			~cQlsDataset( void );
	void		cardAdd( char *, cQlsStatus & );
	void		chunkAdd( cDhsPutRequest *, cQlsStatus & );
	void		chunkMerge( void *, size_t, cQlsStatus & );
	inline char	*datasetName( void )
			{
			    return( qdsName );
			}
	inline void	deleteWait()
			{
			    qdsDeleteCond.lock();
			    while ( qdsRefCount != 1 )
			    {
				qdsDeleteCond.wait();
			    }
			    qdsDeleteCond.unlock();
			    deRef();
			}
	inline void	deRef()
			{
			    wLock();
			    qdsRefCount--;
			    if ( qdsRefCount == 1 )
			    {
				qdsDeleteCond.broadcast();
			    }
			    if ( qdsRefCount == 0 )
			    {
				delete this;
			    }
			    else
			    {
			    	unlock();
			    }
			}
	inline static void
			exit( cQlsStatus & );
	void		forEachFrame( tForEachFrameFn, void *, cQlsStatus & );
	bool		hasStream( cQlsStream & );
	inline bool	last( void )
			{
			    return( qdsLast );
			}
	inline cQlsFrame
			&lastFrame( void )
			{
			    return( *qdsPLastFrame );
			}
	inline cQlsFrame
			&lastFrame( cQlsFrame &frame )
			{
			    qdsPLastFrame = &frame;
			    return( frame );
			}
	inline void	ref()
			{
			    qdsRefCount++;
			}
	inline int	refCount()
			{
			    return( qdsRefCount );
			}
	void		setQlStream( char **, uint, cQlsStatus & );
	void		streamAdd( cDhsAvList &, cQlsStatus & );
    protected:
    private:
	cQlsFrame	*frameFind( FITSFILE *, bool, int, cQlsStatus & );
	inline void	last( bool value )
			{
			    qdsLast = value;
			}
	static void	modifiedClear( cQlsFrame &, void *, cQlsStatus & );


	bool		qdsFirst;	// Waiting for the first chunk?
	tFrameList	qdsFrameList;	// List of frames for the dataset.
	bool		qdsInList;	// True if the dataset is in the list
					// of datasets.
	bool		qdsIsChunk;	// Is the dataset in chunks?
	bool		qdsLast;	// Has the last chunk been seen?
	char		*qdsName;	// name of the dataset.
	time_t		qdsNotifyTime;	// Last time QLT's were notified about
					// changes to the ds.
	int		qdsNumWaits;	// Number of threads waiting to modify
					// the dataset.
	static cMutex	qdsNumWaitsLock;// Lock to protect the qdsNumWaits 
					// member.
	cQlsFrame	*qdsPLastFrame;	// Pointer to the frame to display
					// as the last modified frame.
	cQlsFrame	*qdsPMainFrame;	// Main Frame.
	int		qdsRefCount;	// Number of references to the ds.
	cQlsStream::tStreamList		// list of associated streams.
			qdsStreamList;
	cCond		qdsDeleteCond;	// Condition variable used for 
					// deleteWait
};

//
//***********************************************************************
//+
// FUNCTION NAME:
// cQlsDataset::exit
//
// INVOCATION:
// cQlsDataset::exit( status );
//
// PARAMETERS: (">" input, "!" modified, "<" output)
// (!) status (cQlsStatus &) Function return status.
//
// FUNCTION VALUE:
// None.
//
// PURPOSE:
// Delete all cQlsDataset objects
//
// DESCRIPTION:
// Delete the object from the top of the map until the map is empty.
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

void		cQlsDataset::exit
(
    cQlsStatus	&status		// (mod) Function return status.
)
{
    checkStat( status, return );


    deleteAll();

    cQlsFrame::qfdCounter = 0;
}
#endif // QLSDATASET_H
