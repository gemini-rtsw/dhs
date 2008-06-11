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
// dhsData/h/cmd.H
//
// PURPOSE:
// Contains the definitions for command and control structures.
//
// CLASSES:
// cDtsDhsArchive - Receives cmds from storage server.
// cDtsDhsCmdRequest - Generic
// cDtsDhsDsInfo - Dataset information commands.
// cDtsDhsControl - Handles controls reset, lifetime, contrib, ql streams.
// cDtsDhsDelete - Delete a dataset
//
//INDENT-OFF*
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
//
//
// Revision 1.8  1998/05/04 22:45:29  dunn
// Added datasetInfo command, changed storage server notifiy
// command to stoArchiveFileAvailable, changed vrariable name.
//
// Revision 1.7  1997/09/29 16:07:40  dunn
// Code cleanup.
//
// Revision 1.6  1997/09/17 22:21:07  dunn
// Code cleanup.
//
// Revision 1.5  1997/09/05 19:53:51  dunn
// Added the archive notify command from ss.
//
// Revision 1.4  1997/08/29 23:07:01  dunn
// Got rid of unused classes.
//
// Revision 1.3  1997/08/19 22:32:21  dunn
// Bug fixes.
//
// Revision 1.2  1997/07/29 22:03:55  dunn
// Various fixes.
// Added delete.
//
// Revision 1.1  1997/07/10 18:28:16  dunn
// Initial revision
//
//INDENT-ON*
//
//***  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
//***********************************************************************
//

#ifndef DTSCMD_H
#define	DTSCMD_H

#include "dhs++.H"
#include "dhsGenStatus.H"

//
//  Local Defines.
//

#define DTS_CTL_COMMAND		"ctl"
#define DTS_CTL_DATA_LABEL	"dataLabel"
#define DTS_CTL_DEFINITION	"bdCtl"
#define DTS_CTL_LABEL		"datasetName"
#define DTS_CTL_LIFETIME	"lifeTime"
#define DTS_CTL_NAMES		"nameArray"

#define DTS_CMD_ARC_NOTIFY	"archive_file_notify"
#define DTS_CMD_DELETE		"bdDelete"
#define DTS_CMD_EXIT		"cmdSystemExit"
#define DTS_CMD_DS_INFO		"datasetInfo"

#define DTS_OLDP_START_CMD	"oldpStart"
#define	DTS_OLDP_NOTIFY_CMD	"fileAvail"

#define	DTS_QL_DS_ATTRIB	"dataset"
#define	DTS_QL_NOTIFY_CMD	"qlsDsStream"
#define	DTS_QL_STREAM_ATTRIB	"streams"

#define	DTS_SS_FILE_ID		"fileId"
#define DTS_SS_FILE_SZ		"fileSize"
#define	DTS_SS_NOTIFY_CMD	"stoArchiveFileAvailable"
#define DTS_SS_SEG_DIR		"segregationDir"
#define DTS_SS_START_CMD	"storageStart"



//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsArchive
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Handles archive commands sent from the storage server.
//
//-
//***********************************************************************
//

class 	cDtsDhsArchive: public cDhsServerCommand, public cDtsDhsName
{
    public:
	class	aStatus: public cDhsStatus	// Return response
	{
	    public:
		msgPrefix( "dts" );
		msgId( S_WARN_INV_DS );
		msgId( S_WARN_NULL_SEG );
	};
	aStatus		arcStatus;		// Response string.

    protected:
    			~cDtsDhsArchive( void );
    private:
	void		getFileIds( cDtsStatus & );
	void		exec( void );
	char		*ddaDatasetName;
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsCmdRequest
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Handles invalid, oldp and ss startup commands.
//
//-
//***********************************************************************
//

class 	cDtsDhsCmdRequest : public cDhsServerCommand, public cDtsDhsName
{
    public:
    protected:
    private:
	void		exec( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsDsInfo
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Handles dataset information requests.
//
//-
//***********************************************************************
//

class 	cDtsDhsDsInfo: public cDhsServerCommand, public cDtsDhsName
{
    public:
    protected:
    private:
	void		exec( void );
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsControl
//
// PUBLIC METHODS: 
//        cDtsDhsControl - Constructor.
// (void) generateUniqueName - Generate a unique name.
// (void) rmDataset - Remove the dataset.
// (void) setQlStream - Send the set ql stream to ql server.
// (void) setLifetime - Set the lifetime of the dataset.
// (void) setContribList - Set the contributor list of the dataset.
//
// PURPOSE:
// Handles all control commands sent to the data server. 
//
//-
//***********************************************************************
//

class 	cDtsDhsControl : public cDhsServerCommand , public cDtsDhsName 
{
    public:
	class	rStatus: public cDhsStatus	// Command return response
	{
	    public:
			msgPrefix( "dts" );
			msgId( E_DATASET_NAME );
			msgId( E_DATASET_RECEIVED );
			msgId( E_INVALID_CTL );
			msgId( E_NO_LIST );
			msgId( E_NO_NAME );
			msgId( E_RMV_RECEIVED );
			msgId( S_WARN_INV_DS );
	    protected:
	    private:
	};
	rStatus		cmdStatus;		// Response string.

			cDtsDhsControl( void );

	void		generateUniqueName( cDtsStatus &, char ** );
	void		rmDataset( cDtsStatus &, char *, bool );
	void		setQlStream( cDtsStatus &, char **, char *, 
				unsigned long * );
	void		setLifetime( cDtsStatus &, DHS_BD_LIFETIME, char * );
	void		setContribList( cDtsStatus &, char **, int, char * );

    protected:

    private:

	inline char 	*returnLabel( void )
			    {
				return( dcrLabel );
			    };

	void		exec( void );
	void 		getLabel( cDtsStatus &, DHS_BD_CTL );

	char		*dcrLabel;
	
};

//
//***********************************************************************
//+
// CLASS NAME:
// cDtsDhsDelete
//
// PUBLIC METHODS: 
// None.
//
// PURPOSE:
// Handles the deletion of a dataset from the data server.
//
//-
//***********************************************************************
//

class 	cDtsDhsDelete : public cDhsServerCommand, public cDtsDhsName
{
    public:
	class	cStatus: public cDhsStatus	// Return response
	{
	    public:
		msgPrefix( "dts" );
		msgId( S_WARN_INV_DS );
		msgId( S_WARN_NULL_LABEL );
		msgId( S_WARN_PERM );
	};
	cStatus		cmdStatus;		// Response string.

    protected:
    private:
	void		exec( void );
	char		*dddDatasetName;	// Dataset to delete.
};
#endif // DTSCMD_H
