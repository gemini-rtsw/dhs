
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
// dhsData/h/put.H
//
// PURPOSE:
// Contains the definitions for bulk data puts to the DHS data server.
//
// CLASSES:
// cDtsDhsPut - Get request responder.
// cDtsPutFits - Handles fits put requests.
// cDtsPutRaw - Handles raw put requests.
// cDtsPutDs - Handles ds put requests.
// cDtsPutDsQl - Handles ds quick look put requests.
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.15  2000/07/27 20:11:13  dunn
// .Added standalone features.
//
// Revision 1.14  1999/02/16 00:24:59  jaeger
// *** empty log message ***
//
// Revision 1.13  1999/02/16 00:24:17  jaeger
// Added msg_clear( sfMsg ) to checkSfLib macro.
//
// Revision 1.12  1998/06/22 20:50:48  dunn
// Removed cdlMutexPopen, and added 2 user messages: S_E_CONV_WARNING, S_E_TAR_FILE.
//
// Revision 1.11  1998/05/04 22:54:51  dunn
// Added more messages and status.display was removed from check msacro.
//
// Revision 1.10  1997/11/25 00:11:07  jaeger
// Fixed buffer size (length) of passed into parseHeader - now is the actual
// size of the composed FITS file.
//  .
//
// Revision 1.9  1997/10/03 22:58:56  dunn
// Added mutex around popen call.
//
// Revision 1.8  1997/10/01 16:23:41  dunn
// Code cleanup.
//
// Revision 1.7  1997/09/29 21:25:04  dunn
// Code cleanup
//
// Revision 1.6  1997/09/17 22:26:24  dunn
// Added cDtsPutDsQl.
//
// Revision 1.5  1997/08/29 23:11:11  dunn
// Removed source stuff and added names class.  Added finalDesting.
//
// Revision 1.4  1997/08/19 22:33:10  dunn
// Bug fixes.
//
// Revision 1.3  1997/07/29 22:05:55  dunn
// Changed lastFlagCheck to pass in datasetName also.
//
// Revision 1.2  1997/07/23 19:45:36  dunn
// Added messages.
//
// Revision 1.1  1997/07/10 18:28:16  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DTSPUT_H
#define	DTSPUT_H

#include <map>
#include <list>
#include <string>
#include "genMutex.H"
#include "dhs++.H"
#include "dhsGenStatus.H"


//
//  Defines
//

#define	checkSfLib( fn, sfStatus, datasetName, dsInfo, status, ret )	\
    {									\
	fn;								\
	if ( sfStatus <  SF_S_SUCCESS )					\
	{								\
            sfDsInfoFree( dsInfo );                                     \
            dsInfo = NULL;                                              \
	    status.E_SDS2FITS( status, datasetName, sfMsg );		\
	    ret;							\
	}								\
	msg_clear( sfMsg );						\
    }



//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsPut
//
// PUBLIC METHODS: 
// (void) initReceived() - Set the initial Retrieval state.
// (char) initReceived() - Return the initial Retrieval state.
// (void) segDir() - Set the segregation directory.
// (char *) segDir() - Return the segregation directory.
//        ~cDtsDhsPut() - Deconstructor.
// (void) destinationExtract() - Determines where to write the file.
// (void) fileProcess() - Write the file.
// (void) lastFlagCheck() - Check the state of the last flag.
// (void) oldpServerNotify() - Notify the oldp server.
// (void) qlSend() - Send to quick look server.
// (void) serverNotify() - Notify the oldp and ss, if necessary.
// (void) setSeg() - Parse the datasetName for segration directory.
// (void) storageServerNotify() - Notify the storage server.
//
// PURPOSE:
// To provide functions for all types of put requests.
//
//-
//***********************************************************************
//

class 	cDtsDhsPut : public cDhsPutRequest, public cDtsDhsName
{
    public:
	inline		cDtsDhsPut( void )
			{
			    cdpSegDir = NULL;	
			};
			~cDtsDhsPut( void );
	inline void	initReceived( char x )
			{
			    cdpInitReceivedState = x;
			}
	inline char	initReceived( void )
			{
			    return ( cdpInitReceivedState );
			}
	inline void
			segDir( char * x ) 
			    { 
				cdpSegDir = strsav( x ); 
			    };
	inline char *
			segDir( void ) 
			    { 
				return( cdpSegDir ); 
			    };



	virtual void	destinationExtract( cDtsStatus &, char **, char **,
				char **, int, cDtsDatasetTable * );
	void		fileProcess(  cDtsStatus &, char *, int );
	void		lastFlagCheck( cDtsStatus &, char * );
	static void	oldpServerNotify( cDtsStatus &, cDtsDatasetTable *,
				char *, char * );
	void		qlSend( cDtsStatus &, char *, bool, cDtsDatasetList * );
	void		serverNotify( cDtsStatus &, cDtsDatasetTable * );
	void		setSeg( cDtsStatus &, char ** );
	static void	storageServerNotify( cDtsStatus &, cDtsDatasetTable *,
				char *, char *, char * );


	class	pStatus: public cDhsStatus	// Put return response
	{
	    public:
		msgPrefix( "dts" );
		msgId( S_BAD_TYPE );
		msgId( S_CONV_WARNING );
		msgId( S_DATA_RECEIVED );
		msgId( S_DATA_RECOVERY );
		msgId( S_E_CONV_WARNING );
		msgId( S_E_TAR_FILE );
		msgId( S_FILE_OPEN );
		msgId( S_FILE_WRITE );
		msgId( S_HEADER_ERROR );
		msgId( S_HEADER_WARN );
		msgId( S_NEW_UNIQUE_NAME );
		msgId( S_NEW_DS_NAME );
		msgId( S_NO_DATA );
		msgId( S_NO_SPACE );
		msgId( S_NOT_CONTRIB );
		msgId( S_TMP_FILE );
		msgId( S_WARN_INV_LABEL );
		msgId( S_WARN_LAST_FLAG );
		msgId( S_WARN_NULL_LABEL );
	    protected:
	    private:
	};
	pStatus		putStatus;		// Response string.

    protected:
    private:
	char		cdpInitReceivedState;	// InitReceived State.
	char		*cdpSegDir;		// Segregation dir.
};


//
//***********************************************************************
//+
// CLASS NAME:
// cDtsPutFits
//
// PUBLIC METHODS: 
// (void) exec() - Execute for fits request.
//
// PURPOSE:
// Handle put fits requests.
//
//-
//***********************************************************************
//

class 	cDtsPutFits: public cDtsDhsPut
{
    public:
	void		exec();
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsPutRaw
//
// PUBLIC METHODS: 
// (void) exec() - Execute for raw request.
//
// PURPOSE:
// Handles a put raw request.
//
//-
//***********************************************************************
//

class 	cDtsPutRaw : public cDtsDhsPut
{
    public:
	void		exec();
    protected:
    private:
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsPutDs
//
// PUBLIC METHODS: 
// (void) lockSf() - Lock out cfitsio routines.
// (void) unlockSf() - Unlock cfitsio routines.
// (void) chunkConvert() - Convert each chunk into fits.
// (void) datasetCompose() - Compose chunks into final fits.
// (void) datasetRecovery() - Recover a sick dataset.
// (void) datasetRecovery() - Recover a sick dataset.
// (void) destinationExtract() - Determine where to write a chunk.
// (void) destinationExtract() - Determine where to write the final fits.
// (void) exec() - Execute for ds request.
// (void) fileProcess() - Write the ds file.
// (void) fileProcess() - Write the fits file.
// (void) finalDestin() - Determine final destination of a ds/fits file.
//
// PURPOSE:
// Handles a put ds request.
//
//-
//***********************************************************************
//

class 	cDtsPutDs: public cDtsDhsPut
{
    public:
	static inline void
			lockSf ( void )
			    {
				cdlMutexSf.lock();
			    };
	static inline void
			unlockSf ( void )
			    {
				cdlMutexSf.unlock();
			    };

	void		chunkConvert( cDtsStatus &, int, 
				cDtsDatasetList *, cDtsDatasetTable *, char *,
				fitsfile *, int & );
	void		datasetCompose( cDtsStatus &, bool, int, 
				cDtsDatasetList *, void **, int*  );
	static void	datasetRecovery( cDtsStatus &, char *, 
				cDtsDatasetList * );
	static void	datasetRecovery( cDtsStatus &, char *, 
				cDtsDatasetTable * );
	inline void	destinationExtract( cDtsStatus &status, 
			    char **fname, char **filePath,
			    char **vol, int size, cDtsDatasetTable *dsPtr )
			    {
				cDtsDhsPut::destinationExtract( status,
				       fname, filePath, vol, size, dsPtr );
			    };

	void		destinationExtract( cDtsStatus &, char **, char **,
				char **, int, cDtsDatasetTable *, int );
	void		exec();
	void		fileProcess(  cDtsStatus &, char *, int );
	void		fileProcess(  cDtsStatus &, char *, int, void *, bool );

	static void	finalDestin( cDtsStatus &, char **, char **,
				char **, int &, cDtsDatasetTable * );
    protected:
    private:
	static cMutex	cdlMutexSf;		// Protect cfitsio calls.
	bool		cdlRecovery;		// Recovery was done.
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsPutDsQl
//
// PUBLIC METHODS: 
// (void) exec() - Execute for ds ql only request.
//
// PURPOSE:
// Handles a put ds ql only request.
//
//-
//***********************************************************************
//

class 	cDtsPutDsQl: public cDtsDhsPut
{
    public:
	void		exec();
    private:
};
#endif // DTSPUT_H
