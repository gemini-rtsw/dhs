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
// dhsData/h/list.H
//
// PURPOSE:
// Contains the definitions for the list of datasets and unique names
// currently in use.
//
// CLASSES:
// cDtsLists - Parent to the next two.
// cDtsDatasetList - List of datasets.
// cDtsUniqueList - List of uniqueNames.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/24 20:26:31  brighton
// Imported sources
//
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.12  2000/07/27 20:11:02  dunn
// .Added standalone features.
//
// Revision 1.11  1998/06/16 16:07:27  dunn
// Changed declarations to have void in them.
//
// Revision 1.10  1998/05/04 22:54:19  dunn
// Added eventLoop and all variables associated with it.
// Added extra flag to chunkExists.
//
// Revision 1.9  1997/10/06 20:02:21  dunn
// Fixed header messages and unused methods.
//
// Revision 1.8  1997/10/03 22:59:38  dunn
// Got rid of lstQlStream and defaultQl, not needed.
//
// Revision 1.7  1997/09/17 22:25:47  dunn
// Code cleanup.
//
// Revision 1.6  1997/09/05 19:54:21  dunn
// Added the unique list deconstructor.
//
// Revision 1.5  1997/08/29 23:10:37  dunn
// Added datasteListReset and exit for both unique and dataset lists.
//
// Revision 1.4  1997/08/19 22:33:02  dunn
// Bug fixes.
//
// Revision 1.3  1997/07/29 22:05:20  dunn
// Added datasetNameFind, datasetListRemove, setChunkList, resetChunkList, ~cDtsDatasetLis.
//
// Revision 1.2  1997/07/24 16:27:21  dunn
// Fixed lstUkPrefix to do a string save.
//
// Revision 1.1  1997/07/10 18:28:16  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DTSDSLIST_H
#define	DTSDSLIST_H

#include <map>
#include <list>
#include <string>
#include "genMutex.H"
#include "dhs++.H"

#define 	DTS_PERM	'P'
#define 	DTS_TEMP	'T'
#define		DTS_NORTH	'N'
#define		DTS_SOUTH	'S'
#define		DTS_OCS		'O'
#define		DTS_SUMMIT	'S'
#define		DTS_BASE	'B'

#define		DTS_MAX_CONTRIBUTORS	20

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsLists
//
// PUBLIC METHODS: 
// (void ) config - Read from the configuration file.
// (char ) defaultLife - Return default lifetime.
// (char *) defaultSource - Return default source for uniquename.
// (char *) defaultLocation- Return default location for uniquename.
//
// PURPOSE:
// Methods and variables in common between the dataset and unique name lists.
//
//-
//***********************************************************************
//

class 	cDtsLists
{
    public:
	static inline char
			defaultLife( void ) 
			    { 
				return( lstLifetime ); 
			    };
	static inline char 
			defaultLocation( void ) 
			    { 
				return( lstLocation ); 
			    };
	static inline char 
			defaultSource( void ) 
			    { 
				return( lstSource ); 
			    };
	static inline char  *
			uniquePrefix( void ) 
			    { 
				return( lstUkPrefix ); 
			    };
	static inline int
			uniquePrefixNum( void ) 
			    { 
				return( lstUkPrefixNum ); 
			    };
	static inline void
			setUniquePrefix( char *x, int i ) 
			    { 
				if ( lstUkPrefix != NULL )
				    free( lstUkPrefix );
				lstUkPrefix = strsav( x );
				lstUkPrefixNum = i;

				fileWriteLabelPrefix(lstUkPrefix, lstUkPrefixNum);
			    };
	static inline int 
			nextPrefixNum ( void )
			    { 
				lstUkPrefixNum++;

				fileWriteLabelPrefix(lstUkPrefix, lstUkPrefixNum);

				return lstUkPrefixNum; 
			    };

	static void	config( cDtsStatus & );		// Reads config.
	static cMutex	lstMutexNumIncomp;		// Protect num incomp.
							// ds calls.

    protected:
        static bool     fileWriteLabelPrefix(std::string inPrefixStr, int inPrefixInt);
        static bool     fileReadLabelPrefix(std::string& outPrefixStr, int& outPrefixInt); 
	
	static const    std::string prefixFilename;

    private:
	static boolean	nameInfo( int, char ** );	// Using in config.
	static char	lstLifetime;			// Default Lifetime
	static char	lstLocation;			// North or South
	static char	lstSource;			// Source, O, S, B.

	static char	*lstUkPrefix;			// Composed name prefix.
	static int	lstUkPrefixNum;			// Composed prefix num.




};



//
//***********************************************************************
//+
// CLASS NAME:
// cDtsUniqueList
//
// PUBLIC METHODS: 
// cDtsUniqueList - Constructor.
// ~cDtsUniqueList - Deconstructor.
// (void) ukNameLock - Locks uniqueName list.
// (void) ukNameUnlock - Unlocks uniqueName list.
// (void) add - Add a uniqueName to the list if not already there.
// (void) composeUName - Determine unique name prefix and number for startup.
// (void) createUniqueName - Create a new unique name.
// (void) exit - Delete and erase from the list of uniqueNames.
// (void) *eventLoop - Event loop to purse the unique label list.
// (void) findNameMax - Finds the maximum number and returns it.
// (void) init - Initialize the purse of the label loop.
// (void ) uniqueNameVerify - Verify the datasetname.
//
// PURPOSE:
// Contains and maintains a list of uniqueNames in use.
//
//-
//***********************************************************************
//

class 	cDtsUniqueList : public cDtsLists
{
    public:
	inline static void
			ukNameLock( void )
			    {
				culMutexUk.lock();
			    };
	inline static void
			ukNameUnlock( void )
			    {
				culMutexUk.unlock();
			    };

	typedef 	std::map< std::string,		// Sort key of datatype string.
		     	cDtsUniqueList *,	// Into this containr this type
		     	std::less<std::string> >		// Sorting mechanizm.
			tUniqueList;		// Provides access to containr
	typedef 	tUniqueList::iterator	// Provides iterator to containr
			iterator;

			cDtsUniqueList( char *, long );
						// Constructor. Adds to list.
			~cDtsUniqueList( void );
						// Destructor.  

	void  		add( cDtsStatus & );
	static void	composeUName( cDtsStatus & );
	static void	createUniqueName( cDtsStatus &, char ** );
	static void	exit( cDtsStatus & );	// Cycles through all objects.
	static void	*eventLoop( void * );
	static void	findNameMax( cDtsStatus &, char *, int * );
	static void	init( cDtsStatus & );
	static void	uniqueNameVerify( cDtsStatus &, char *, char ** );

    protected:
    private:
	static tUniqueList
			culUniqueList;		// List of uniqueNames in use.
	static cMutex	culMutexUk;		// Protect the unique list.

	char		*culUniqueName;		// Unique name. 
	long		culDateNotified;	// Date info about dataset.
	static bool	culElExit;		// True when the purge event 
						// loop should exit.
	static pthread_t			// Thread id of the event
			culElThread;		// loop thread.
	long		culLastAccess;		// Last access to  uniqueName.
	cDtsUniqueTable *culUq;			// Unique information.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsChunkInfo
//
// PUBLIC METHODS: 
//
// PURPOSE:
// Contains and maintains a list of chunks/contributors associated with 
// a dataset.  This is better named a contributor list.  One is associated
// with each and every dataset in the dataset list.
//
//-
//***********************************************************************
//

class 	cDtsChunkInfo
{
    friend class cDtsDatasetList;

    public:
	inline char *	name ( void ) 
			    { 
				return ( chiContributor );
			    };

	typedef 	std::map< std::string,		// Sort key is of type string.
		     	cDtsChunkInfo *,	// Insert into containr type
		     	std::less<std::string> >		// Sorting mechanizm.
			tChunkList;		// Provides access to containr
	typedef 	tChunkList::iterator	// Provides iterator to containr
			chunkIterator;
    protected:
    private:
			cDtsChunkInfo( char * );// Constructor. Adds to list.
			~cDtsChunkInfo( void );	// Destructor. 
	char		*chiContributor;	// Contributor.
	bool		chiLast;		// True indic last flag recvd.
	int		chiNumChunks;		// Number of chunks received;

};


//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDatasetList
//
// PUBLIC METHODS: 
// cDtsDatasetList - Constructor.
// cDtsDatasetList - Constructor.
// cDtsDatasetList - Constructor.
// ~cDtsDatasetList - Deconstructor.
// getDs - Returns pointer to the cDtsDatasetTable member in the object.
// chunkNumber - Returns the current number of chunks/contributors.
// datasetCompression - For standalone mode returns the compression.
// datasetFormat - For standalone mode returns the format.
// datasetObjectLock - Locks the current dataset object in the list.
// datasetObjectUnlock - Unlocks the current dataset object in the list.
// datasetPath - For standalone mode returns the path.
// datasetLock - Locks the dataset list.
// datasetUnLock - Unlocks the dataset list.
// (void) add - Add a dataset to the list if not already there.
// (void) addDatabase - Adds a newly create dataset table object to the db.
// (void) addList - Creates and adds a cDtsDatasetList object to the list.
// (void) addList - Creates and adds a cDtsDatasetList object to the list.
// (bool) chunkExits - Searchs the chunk list for this object given the contrib.
// (void) chunkLastCheck - Checks to see if this is the last contrib. for ds.
// (cDtsDatasetList *) datasetNameVerify - Verifies the dataset name passed in.
// (cDtsDatasetList *) datasetNameVerify - Verifies the dataset name passed in.
// (cDtsDatasetList *) datasetNameFind - Finds the ds in the dataset list.
// (void) datasetListRemove - Removes data & from the list this dataset.
// (void) datasetListReset - Removes data & resets received flags for a ds.
// (void) exit - Each item in the dataset list is deleted and erased.
// (cDtsChunkInfo *) findChunk - Finds source in the chunk list.
// (void) findNameMax - Finds the maximum number and returns it.
// (void) newDatasetName - Generates a new dataset name.
// (void) resetChunkList - Resets the chunk list associated with a ds.
// (void) setChunkList - Sets the chunk list associated with a ds.
// (void) updateObject - Updates the path, format & compression.
// (void) updateStatus - Updates the database - process table.
//
// PURPOSE:
// Contains and maintains a list of datasets in use.
//
//-
//***********************************************************************
//

class 	cDtsDatasetList : public cDtsLists
{
    public:
			cDtsDatasetList( char *, cDtsDatasetTable *, 
				DHS_BD_PUT_TYPE);	 // Constructor.
			cDtsDatasetList( char *, cDtsDatasetTable *, 
				char, int );	 // Constructor.
			cDtsDatasetList( char *, char *, cDtsDatasetTable *, 
				char, char, int );	 // Constructor.
			~cDtsDatasetList();	 // Destructor.

	inline cDtsDatasetTable *
			getDs ( void )
			    {
				return ( cdlDs );
			    };
	inline int	chunkNumber ( void )
			    {
				return ( cdlNumChunks );
			    };
	inline char 	datasetCompression( void )
			    {
				return( cdlComp );
			    };
	inline char 	datasetFormat( void )
			    {
				return( cdlFormat );
			    };
	inline void	datasetObjectLock( void )
			    {
				cdlMutexThis.lock();
			    };
	inline void	datasetObjectUnlock( void )
			    {
				cdlMutexThis.unlock();
			    };
	inline char *	datasetPath( void )
			    {
				return( cdlPath );
			    };
	inline static void
			datasetLock( void )
			    {
				cdlMutexDs.lock();
			    };
	inline static void
			datasetUnlock( void )
			    {
				cdlMutexDs.unlock();
			    };

	typedef 	std::map< std::string,		// Sort key is of type string.
		     	cDtsDatasetList *,	// Insert this type into contr.
		     	std::less<std::string> >		// Is the sorting mechanizm.
			tDataList;		// Provides access to containr.
	typedef 	tDataList::iterator	// Provides iterator to containr
			iterator;

	void  		add( cDtsStatus & );
	static void	addDatabase( cDtsStatus &, char *, char *,
				cDtsDatasetTable **, char *, DHS_BD_PUT_TYPE );
	static void	addList( cDtsStatus &, char *, cDtsDatasetTable *,
				cDtsDatasetList **, DHS_BD_PUT_TYPE );
	static void	addList( cDtsStatus &, char *, cDtsDatasetTable *,
				cDtsDatasetList **, char  );

	bool		chunkExists( cDtsStatus &, char *, DHS_BD_PUT_TYPE, 
				bool );
	void		chunkLastCheck( cDtsStatus &, bool, 
				char *, bool, bool & );
	static cDtsDatasetList
				*datasetNameVerify( cDtsStatus &, char **, 
				char *, char *,
				DHS_BD_PUT_TYPE, cDtsDatasetTable **, int,
				bool &, bool &, bool &, bool & );
	static cDtsDatasetList
			*datasetNameVerify( cDtsStatus &, char **, 
				char *, char *,
				DHS_BD_PUT_TYPE, int, 
				bool &, bool &, bool &, bool &, bool &,  bool );
	static cDtsDatasetList * 
			datasetNameFind( cDtsStatus &, char * );
	void  		datasetListRemove( cDtsStatus &, char, char );
	void  		datasetListReset( cDtsStatus &, cDtsDatasetTable * );
	static void	exit( cDtsStatus & );	// Cycles through all objects.
	cDtsChunkInfo *	findChunk( cDtsStatus &, char * );
	static void	findNameMax( cDtsStatus &, char *, int * );
	static void	newDatasetName( cDtsStatus &, char ** );
	static void	resetChunkList( cDtsStatus &, char * );
	void		setChunkList( cDtsStatus & );
	void		updateObject( cDtsStatus &, char *, int );
	static void	updateStatus( cDtsStatus &, char *, 
				cDtsDatasetTable * );

	struct sf_ds_info
			*cdlDsInfo;		// Chunk Information.

    protected:
    private:
			cDtsDatasetList( const char * );
						// Constructor. Adds to list.
	static tDataList
			cdlDataList;		// List of all datasets in use.

	cDtsChunkInfo::tChunkList		// List of chunks in the ds.
			cdlChunkList;
	int		cdlNumChunks;		// Number of chunks.
	cDtsDatasetTable
			*cdlDs;			// Dataset information.
	char		*cdlDatasetName;	// Dataset name. 
	char		cdlLifetime;		// Lifetime.
	int		cdlFsize;		// Total File Size;
	int		cdlFormat;		// Data format(DHS_BD_PUT_TYPE).
	int		cdlComp;		// Data compression type.
	char		*cdlPath;		// Path to Dataset 
						//     (standalone only). 
	char		*cdlUniqueName;		// UniqueName.

	static cMutex	cdlMutexDs;		// Protect the dataset list.
	cMutex		cdlMutexThis;		// Protect the dataset object.
	
};
#endif // DTSDSLIST_H
