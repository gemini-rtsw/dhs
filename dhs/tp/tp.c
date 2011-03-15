/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1995				(c) 1995.
* National Research Council		Conseil national de recherches
* Ottawa, Canada, K1A 0R6 		Ottawa, Canada, K1A 0R6
* All rights reserved			Tous droits reserves
* 					
* NRC disclaims any warranties,		Le CNRC denie toute garantie
* expressed, implied, or statu-		enoncee, implicite ou legale,
* tory, of any kind with respect	de quelque nature que se soit,
* to the software, including		concernant le logiciel, y com-
* without limitation any war-		pris sans restriction toute
* ranty of merchantability or		garantie de valeur marchande
* fitness for a particular pur-		ou de pertinence pour un usage
* pose.  NRC shall not be liable	particulier.  Le CNRC ne
* in any event for any damages,		pourra en aucun cas etre tenu
* whether direct or indirect,		responsable de tout dommage,
* special or general, consequen-	direct ou indirect, particul-
* tial or incidental, arising		ier ou general, accessoire ou
* from the use of the software.		fortuit, resultant de l'utili-
* 					sation du logiciel.
*
************************************************************************
*
*   Module Name:	tp/src/tp.c
*
*   Purpose:
*	Init the tplib, and get the struct of scripts.
*
*   Routines:
*	int	tp_device_type	: Check if the device is loaded and
*				: get the physical type of the tape.
*	int 	tp_init		: Init the tp lib.
*	int	tp_script_get	: Get the scripts.
*static int 	tp_type_compare_sort : Sort by type for eptr_search.
*
*   Date		: Feb 11, 1998
*
*   SCCS data		: @(#)
*	Module Name	: tp.c
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 04/12/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/03/23 SDJ	: Added hp support.
*	99/04/12 SDJ	: Removed extraneous debugging code.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sybfront.h>
#include <sybdb.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mtio.h>

#include "gen_types.h"
#include "gen_str.h"
#include "gen_msg.h"
#include "gen_eptr.h"

#include "tp.h"
#include "local_tp.h"

static boolean  tp_init_flag;
EPTR		*tp_scripts_list;


/*+
************************************************************************
*
*   Function:	tp_device_type	(solaris version)
*
*   Purpose:
*	Get the physical type of the device.
*
*   Values Returned:
*	int	TP_E_DEVICE	: Error in device.
*	int	TP_SUCCESS	: All went well.
*
************************************************************************
-*/

/* XXX allan: replaced solaris/hpux #ifdefs with HAVE_MTDRIVETYPE check, updated for linux */
#ifdef HAVE_MTDRIVETYPE

/* Solaris version */

int	tp_device_type	
( 
   char		*device,	/* (in)  The device to check.	*/
   char		*tp_type	/* (out) The type of the device.*/
)
{
    int		dev_len; 	/* Length of the device name.		*/
    int		fd;		/* File descriptor.			*/
    struct mtdrivetype
		mtdt;		/* Tape Drive config. info.		*/	
    struct mtdrivetype_request	
		mtreq;		/* Tape Drive config. info. and size	*/	
    int		status;		/* Function return status.		*/
    

    /*
     *  Check the device.
     */

    dev_len = strlen( device );
    if ( device[dev_len - 1] != 'n' ||
       ( strncmp( "/dev/", device, 5 ) != 0 ) )
    {
	tp_format_message( TP_E_DEVICE, device );
	return( TP_E_DEVICE );
    }
     
    mtreq.size = sizeof( struct mtdrivetype );
    mtreq.mtdtp = &mtdt;
    fd = 0;
    status = TP_SUCCESS;

    
    if ( ( fd = open( device, O_RDONLY ) ) == -1 ||
	ioctl( fd, MTIOCGETDRIVETYPE, (char *) &mtreq ) == -1 )
    {
        switch( errno )
	{
	    case EPERM:
		tp_format_message( TP_E_PERM, device );
		break;

	    case EIO:
		tp_format_message( TP_E_UNLOAD, device );
		break;

	    case EBUSY:
		tp_format_message( TP_E_BUSY, device );
		break;

	    default:
		tp_format_message( MSG_ERRNO, device );
		break;
	}
				
	status = TP_E_DEVICE;
    }

    if ( fd >= 0 )
    {
	(void) close( fd );
    }
    TP_CHECK( status );


    /*
     *  Parse the type.
     */

    if ( strpattern( mtreq.mtdtp->name, TP_TYPE_DDS_PATTERN ) )
    {
	strcpy( tp_type, TP_TYPE_DDS );
    }
    else if ( strpattern( mtreq.mtdtp->name, TP_TYPE_DLT_PATTERN ) )
    {
	(void) strcpy( tp_type, TP_TYPE_DLT );
    }
    else if ( strpattern( mtreq.mtdtp->name, TP_TYPE_EXA_PATTERN ) )
    {
	(void) strcpy( tp_type, TP_TYPE_EXA );
    }
    else
    {
	(void) strcpy( tp_type, TP_TYPE_UNKNOWN );
    }

    return( TP_SUCCESS );
}

#else /* HAVE_MTDRIVETYPE */

/* allan: XXX replaced hpux with linux version: added defines below */
#ifndef MT_ISDLT
#define MT_ISDLT 0x38
#endif

#ifndef MT_IS8MM
#define MT_IS8MM 0x35
#endif


/*+
************************************************************************
*
*   Function:	tp_device_type	(hpux version)
*
*   Purpose:
*	Get the physical type of the device.
*
*   Values Returned:
*	int	TP_E_DEVICE	: Error in device.
*	int	TP_SUCCESS	: All went well.
*
************************************************************************
-*/

int	tp_device_type	
( 
   char		*device,	/* (in)  The device to check.		*/
   char		*tp_type	/* (out) The type of the device.	*/
)
{
    int		    dev_len; 	/* Length of the device name.		*/
    int		    fd;		/* File descriptor.			*/
    struct mtget    mtgt;	/* Tape Drive property. info.		*/
    int		    status;	/* Function return status.		*/


    /*
     *  Check the device.
     */

    dev_len = strlen( device );
    if ( device[dev_len - 1] != 'n' ||
       ( strncmp( "/dev/", device, 5 ) != 0 ) )
    {
	tp_format_message( TP_E_DEVICE, device );
	return( TP_E_DEVICE );
    }

    fd = 0;
    status = TP_SUCCESS;

    
    if ( ( fd = open( device, O_RDONLY ) ) == -1 || 
	ioctl( fd, MTIOCGET, &mtgt ) == -1 )
    {
        switch( errno )
	{
	    case EPERM:
		tp_format_message( TP_E_PERM, device );
		break;

	    case EIO:
		tp_format_message( TP_E_UNLOAD, device );
		break;

	    case EBUSY:
		tp_format_message( TP_E_BUSY, device );
		break;

	    default:
		tp_format_message( MSG_ERRNO, device );
		break;
	}
	
	status = TP_E_DEVICE;
    }

    if ( fd >= 0 )
    {
	(void) close( fd );
    }
    TP_CHECK( status );
    

    /*
     *  Parse the type.
     */

    if ( mtgt.mt_type == MT_ISDDS1 || mtgt.mt_type == MT_ISDDS2 )
    {
	(void) strcpy( tp_type, TP_TYPE_DDS );
    }
    else if ( mtgt.mt_type == MT_ISDLT )
    {
	(void) strcpy( tp_type, TP_TYPE_DLT );
    }
    else if ( mtgt.mt_type == MT_IS8MM )
    {
	(void) strcpy( tp_type, TP_TYPE_EXA );
    }
    else
    {
	(void) strcpy( tp_type, TP_TYPE_UNKNOWN );
    }

    
    return( TP_SUCCESS );
}
#endif /* HAVE_MTDRIVETYPE */

/*+
************************************************************************
*
*   Function:   tp_init()
*
*   Purpose:
*	Initialize the tp library.
*
*   Values Returned:
*       int     TP_SUCCESS      : Normal completion.
*       int     TP_E_CONFIG     : Error in configuration.
*
************************************************************************
-*/

int 		tp_init
(
    void
)
{
    TP_CHECK( tp_config_init() );


    tp_init_flag = TRUE;

    return( TP_SUCCESS );
}

/*+
************************************************************************
*
*   Function:	tp_script_get 
*
*   Purpose:
*	Return a pointer to a structure in the list whose type 
*	matches the input type.
*	
*   Values Returned:
*	int	TP_SUCCESS	: Normal completion.
*	int	TP_E_INIT	: Do not init the tp lib.
*	int	TP_E_TYPE	: Type not in the list.
*
************************************************************************
-*/

int		tp_script_get
(
    char	*type,		/* (in)  The tar operation. 		*/
    TP_SCRIPTS 	**scripts	/* (out) A struct of TP_SCRIPTS.	*/
)
{
    int		index;          /* Index into eptr list.                */
    TP_SCRIPTS	*tmp;


    if ( ! tp_init_flag )
    {
	tp_format_message( TP_E_INIT );
	return( TP_E_INIT );
    }

    TP_CHECK_NULL_CONFIG( tmp = gen_alloc( sizeof( TP_SCRIPTS ) ) );
    tmp->tp_type = strsav( type );

    if ( eptr_search( tp_scripts_list, tmp, &index, 
    	    tp_type_compare_sort ) )
    {
        *scripts =  eptritem( tp_scripts_list, index );
    }
    else 
    {
	tp_format_message( TP_E_TYPE, type );
        return( TP_E_TYPE );
    }

    return( TP_SUCCESS );
}

/*+
************************************************************************
*
*   Function:   tp_type_compare_sort
*
*   Purpose:
*       eptr_search compare routine for type config ingo.
*
*   Values Returned:
*       int     >1      : String is greater than format config info.
*       int     0       : String matches the format config info.
*       int     <1      : String is less than format config info.
*
************************************************************************
-*/

int		tp_type_compare_sort
(
    TP_SCRIPTS	*script1,
    TP_SCRIPTS  *script2
)
{
    return( strcmp( script1->tp_type, script2->tp_type ) );
}
