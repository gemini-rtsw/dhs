static char rcsid[] = "$Id: frameInfo.c,v 1.1.1.1 2002-11-24 20:35:31 brighton Exp $";
/*
 ************************************************************************
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 *
 * (c) <year>				(c) <year>
 * National Research Council		Conseil national de recherches
 * Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
 * All rights reserved			Tous droits reserves
 * 					
 * NRC disclaims any warranties,	Le CNRC denie toute garantie
 * expressed, implied, or statu-	enoncee, implicite ou legale,
 * tory, of any kind with respect	de quelque nature que se soit,
 * to the software, including		concernant le logiciel, y com-
 * without limitation any war-		pris sans restriction toute
 * ranty of merchantability or		garantie de valeur marchande
 * fitness for a particular pur-	ou de pertinence pour un usage
 * pose.  NRC shall not be liable	particulier.  Le CNRC ne
 * in any event for any damages,	pourra en aucun cas etre tenu
 * whether direct or indirect,		responsable de tout dommage,
 * special or general, consequen-	direct ou indirect, particul-
 * tial or incidental, arising		ier ou general, accessoire ou
 * from the use of the software.	fortuit, resultant de l'utili-
 * 					sation du logiciel.
 *
 ************************************************************************
 *
 * FILENAME
 * sf/src/frameInfo.c
 *
 * PURPOSE:
 * Manages the frame information sturcture used in the sds2fits converions.
 *
 * FUNCTION NAME(S)
 * ????
 *
 *INDENT-OFF*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.1.1  2002/02/21 20:23:34  tpaz
 *
 *
 * Revision 1.1.1.1  1999/08/23 19:58:04  cvs-tuc
 * Initial install into CVS of dhs-0.16
 *
 * Revision 1.2  1998/05/14 18:45:58  jaeger
 * Added functions for sorting and finding frames on a frameList.
 *
 * Revision 1.1  1998/04/30 06:12:27  jaeger
 * Initial revision
 *
 *INDENT-ON*
 *
 ****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
 ************************************************************************
*/
/*
 ************************************************************************
 *  CADC library include files.
 ************************************************************************
 */

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"


/*
 ************************************************************************
 *  sf library include files.
 ************************************************************************
 */

#include "sf.h"
#include "localSf.h"
#include "frameInfo.h"

void        sfFormatMessage();

/*
************************************************************************
 *+
 * FUNCTION NAME:
 * sfFrameCompare
 *
 * INVOCATION:
 * difference = sfFrameCompare( frame1, frame2 )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output) *
 * (>)  frame1    (SF_FRAME_INFO*)          Frame to find.      
 * (>)  frame2    (SF_FRAME_INFO*)          The strucutre to compare to.    
 *
 * FUNCTION VALUE:
 * (int) < 0  : if frame1 < frame2
 * (int) 0    : if frame1 == frame2
 * (int) > 0  : if frame1 > frame2
 *
 * PURPOSE:
 * Compare two frame identifiers for eptr_search, find one that
 * matches this particular frame identifier.
 *
 * DESCRIPTION:
 * Compare the frameId's of the two frames.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */
 
int     sfFrameCompare
(
    SF_FRAME_INFO *frame1,  /* (in) the strucutre to find.          */
    SF_FRAME_INFO *frame2   /* (in) the strucutre to compare to.    */
)
{
    int         ret;       /* Return value.                     */
    
    ret = strncmp( frame1->frmId, frame2->frmId, FLEN_VALUE );
 
        
    fnReturn( ret );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFrameFind
 *
 * INVOCATION:
 * frameInfo = sfFrameFind( frameList, frameId, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>)  frameList   (EPTR*)             Frame list.
 * (>)  frmid       (char[FLEN_VALUE])  Frame identifier.
 * (!)  sfStatus    (SF_STATUS*)        SF library status.
 *
 * FUNCTION VALUE:
 * (SF_FRAME_INFO*) : If a frame is found.
 * NULL		    : If no frame is found.
 *
 * PURPOSE:
 * To find the frame with the given identifier in a frame list.
 *
 * DESCRIPTION:
 * Search the frame list for the desired frame.  If it is found
 * return it, otherwise return NULL.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */
 
SF_FRAME_INFO *sfFrameFind
(
    EPTR        *frameList,    	    /* (in)  The extension list.        */
    char        frmId[FLEN_VALUE],  /* (in)  Extension identifier.      */
    SF_STATUS   *sfStatus           /* (mod) SF library status.         */
)
{
    int         index;          /* index into frame list            */
    SF_FRAME_INFO *tmpFrame;
        
    fnEntry( "sfFrameFind", *sfStatus );
    statCheck( *sfStatus, NULL );
    tmpFrame = NULL;
 
 
    nullCheck( tmpFrame = (SF_FRAME_INFO*)malloc( sizeof( SF_FRAME_INFO ) ),
            *sfStatus, NULL );
    
    strnCpy( tmpFrame->frmId, frmId, FLEN_VALUE, *sfStatus, NULL );
    if ( frameList != NULL && eptr_search( frameList, (pointer)tmpFrame,
	    &index, sfFrameCompare ) )
    {
        /*
         *   Found it, so return the frame.
         */
 
        gen_free( tmpFrame );
	nullCheck( tmpFrame = eptritem( frameList, index ), *sfStatus, NULL );
    }
    else
    {
        gen_free( tmpFrame );
        tmpFrame = NULL;
        *sfStatus = SF_E_FRAME_NOT_FOUND;
	sfFormatMessage(*sfStatus, frmId );
    }
    
    fnReturn( tmpFrame );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFrameListCreate
 *
 * INVOCATION:
 * sfFrameListCreate( num, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) num	(int)		Number of frames to preallocate.
 * (!) sfStatus	(SF_STATUS*)	Function status.
 *
 * FUNCTION VALUE:
 * NULL  : If an error occurs.
 * EPTR* : If no errors occur.
 *
 * PURPOSE:
 * To initialize an eptr list that will hold frame information.
 *
 * DESCRIPTION:
 * Trivial.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * None.
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

EPTR	*sfFrameListCreate
(
    int		num,		/* (in)  Number to preallocate.		*/
    SF_STATUS	*sfStatus	/* (mod) function status.		*/
)
{
    EPTR	*frameList;

    fnEntry( "sfFrameListInit", *sfStatus );
    statCheck( *sfStatus, NULL );
    
    nullCheck( frameList = eptr_alloc( num ), *sfStatus, NULL );

    fnReturn( frameList );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFrameListDelete
 *
 * INVOCATION:
 * sfFrameListDelete frameList sfStatus
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) frameList (EPTR*)	List of frame information.
 * (!) sfStatus	 (SF_STATUS*)	Function return status.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * Free the memory associated with a frame list.
 *
 * DESCRIPTION:
 * Trivial.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * sfFrameListCreate
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfFrameListDelete
(
    EPTR	*frameList,	/* (in)  List of frames.		*/
    SF_STATUS	*sfStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "sfFrameListDelete", *sfStatus );
    statCheck( *sfStatus, VOID );
    
    eptr_free( frameList, free );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfFrameListInsert
 *
 * INVOCATION:
 * sfFrameListInsert( frameList, newFrame, sfStatus );
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) frameList    (EPTR**)		List of frames.
 * (>) newFrame	    (SF_FRAME_INFO*)	A frame information struct.
 * (!) sfStatus	    (SF_STATUS*) 	Function return value.
 *
 * FUNCTION VALUE:
 * None.
 *
 * PURPOSE:
 * To insert a new item into a frame list.
 *
 * DESCRIPTION:
 * Trivial.
 *
 * EXTERNAL VARIABLES:
 * None.
 *
 * PRIOR REQUIREMENTS:
 * None.
 *
 * SEE ALSO:
 * ???
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

void	sfFrameListInsert
(
    EPTR	    **frameList, /* (in)  FrameList to add to.		*/
    SF_FRAME_INFO   *newFrame,	/* (in)  Frame to be added.		*/
    SF_STATUS	    *sfStatus	/* (mod) Function return status.	*/
)
{
    fnEntry( "sfFrameListInsert", *sfStatus );
    statCheck( *sfStatus, VOID );
    
    nullCheck( *frameList = eptr_insert( *frameList, eptrlen( *frameList ),
	    newFrame ), *sfStatus, VOID );

    fnReturn( VOID );
}

/*
 ************************************************************************
 *+
 * FUNCTION NAME:
 * sfNewFrameInfo
 *
 * INVOCATION:
 * sfNewFrameInfo( parentId, attribName, frame, sfStatus )
 *
 * PARAMETERS: (">" input, "!" modified, "<" output)
 * (>) parentId	    (char[FLEN_VALUE)	Parent's frame Id.
 * (>) attribName   (const char*)	Name given to the frame.
 * (>) frame	    (DHS_BD_FRAME)	sds id of the frame
 * (!) sfStatus	    (SF_STATUS*)	SF function return status.
 *
 * FUNCTION VALUE:
 * (NULL) 		: If an error occurs.
 * (SF_FRAME_INFO*)	: If no errors occur.
 *
 * PURPOSE:
 * Create a new frame information structure.
 *
 * DESCRIPTION:
 * ???
 *
 * EXTERNAL VARIABLES:
 * ???
 *
 * PRIOR REQUIREMENTS:
 * ???
 *
 * SEE ALSO:
 * ???
 *
 * DEFICIENCIES:
 * None.
 *-
 ************************************************************************
 */

SF_FRAME_INFO*	sfNewFrameInfo
(
    char	    parentId[FLEN_VALUE],   /* (in) Parent frame Identifier.*/
    const char	    *attribName,	    /* (in) Frame attribute name.   */
    DHS_BD_FRAME    frame,		    /* (in) Frame.		    */
    SF_STATUS	    *sfStatus		    /* (mod) Function Status.	    */
)
{
    SF_FRAME_INFO   *newFrameInfo;
    

    fnEntry( "sfNewFrameInfo", *sfStatus );
    statCheck( *sfStatus, NULL );

    
    /*
     * NOTE: TEMPORARY FIX FOR FRAMES.  THE DATA IS
     * NOT A POINTER BUT THE VALUE ITSELF.
     */
	    
    nullCheck( newFrameInfo = (SF_FRAME_INFO*) malloc(
	sizeof( SF_FRAME_INFO ) ), *sfStatus, NULL );

    newFrameInfo->sdsId = frame;


    if ( streq( attribName, "" ) )
    {
	strnCpy( newFrameInfo->frmId, parentId, FLEN_VALUE,
		*sfStatus, NULL );
    }
     else
    {
	attribName = strrchr( attribName, '_' );
	if ( strint( attribName + 1 )  )
	{
	    if ( strlen( parentId ) > 0 )
	    {
		sprintf( newFrameInfo->frmId, "%s.%s", parentId,
			attribName + 1 );
	    }	
	    else
	    {
		sprintf( newFrameInfo->frmId, "%s", attribName + 1 );
	    }
	}
	else
	{
	    *sfStatus = SF_E_FRAMEID;
	    sfFormatMessage( *sfStatus, attribName );
	    fnReturn( NULL );
	}
    }
    

    strnCpy( newFrameInfo->frmName, "", FLEN_VALUE, *sfStatus, NULL );
    strnCpy( newFrameInfo->extName, "", FLEN_VALUE, *sfStatus, NULL ) ;
    newFrameInfo->extVer = SF_NULL_EXTVER;
    newFrameInfo->hasData = FALSE;

    fnReturn( newFrameInfo );
}
