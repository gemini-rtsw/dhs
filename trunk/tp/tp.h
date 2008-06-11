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
*   Include File Name:	tp/h/tp.h
*
*   Purpose:
*	Global definitions for the tp library.
*
*   Date		: Feb 11, 1998
*
*   SCCS data		: @(#)
*	Module Name	: tp.h
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 03/23/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/03/23 SDJ	: Added TP_E_SYSTEM and removed tp_type_get().
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

/*
 *  Status and message codes.
 */

#define TP_CFILE_OPEN           1       /* Opening configuration file.  */

#define	TP_SUCCESS		0	/* Normal completion.		*/

#define TP_E_BUSY		(-1)	/* The device busy.		*/
#define TP_E_CONFIG             (-2)    /* Error reing config file.     */
#define TP_E_DEVICE		(-3)	/* Inproper device.		*/
#define TP_E_INIT               (-4)    /* Library not initialized.     */
#define TP_E_MEMORY             (-5)    /* Error out of memory.         */
#define TP_E_PERM		(-6)	/* Permition denied.		*/
#define TP_E_TYPE               (-7)    /* Select type invalid.         */
#define TP_E_UNLOAD		(-8)	/* The device unloaded.		*/


typedef struct tp_scripts
{
    char 	*tp_type;		/* The data type of the tape.	*/
    char 	*tp_ingest;
    char 	*tp_retrieve;
    char 	*tp_name;
} TP_SCRIPTS;


/*
 * Function prototypes and external declarations.
 */

extern	char	tp_msg[];

extern	int	tp_device_type( char *, char * );
					/* Check if the device is loaded.*/
extern	int 	tp_init( void );	/* Init tp librrary.		*/
extern	int	tp_script_get( char *, TP_SCRIPTS ** );
					/* Get the script from tp lib.	*/

