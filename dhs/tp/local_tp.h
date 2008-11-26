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
*   Include File Name:	tp/h/local_tp.h
*
*   Purpose:
*	Local definitions for the tp library.
*
*   Date		: Feb 11, 1998
*
*   SCCS data		: @(#)
*	Module Name	: local_tp.h
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 03/23/99
*
*   Programmer		: George Zhang
*
*   Modification History:
*	99/03/23 SDJ	: Changed the TP_E_CONFIG msg to print out the
*			  name of the configuration file.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#define TP_LIBRARY              "libTp"


/*
 *  Define the physical type of the tape.
 */

#define TP_TYPE_DDS		"DDS"		/* 4mm type tape.	*/
#define TP_TYPE_DLT		"DLT"		/* DLT7000 type tape.	*/
#define	TP_TYPE_EXA		"EXA"		/* 8mm type tape.	*/
#define TP_TYPE_UNKNOWN		" "		/* Unknown type.		*/

#define TP_TYPE_DDS_PATTERN	"*4mm*"		/* 4mm type pattern.	*/
#define	TP_TYPE_DLT_PATTERN	"*DLT*"		/* DLT type pattern.	*/
#define	TP_TYPE_EXA_PATTERN	"*8mm*"		/* 8mm type pattern.	*/


#define TP_CHECK( s )                                                   \
        {   int estatus;                                                \
            if ( ( estatus = ( s ) ) != TP_SUCCESS )                    \
	    return( estatus ); }

#define TP_CHECK_CONFIG(s, filename)                                    \
        {   if ( ( s ) != GC_SUCCESS ) {                                \
            tp_format_message( TP_E_CONFIG, filename );                 \
            msg_append( tp_msg, gc_msg );                               \
            msg_clear( gc_msg );                                        \
            return( TP_E_CONFIG ); } }

#define TP_CHECK_NULL( s )						\
        {   if ( ( s ) == NULL ) {                                      \
            tp_format_message( TP_E_MEMORY );                   	\
	    return( TP_E_MEMORY ); } }


#define TP_CHECK_NULL_CONFIG(s)                                         \
        {   if ( (s) == NULL )                                          \
            {   tp_format_message( TP_E_MEMORY );                       \
                return( FALSE ); } }

					 
extern EPTR       *tp_scripts_list;
								  
extern int      tp_config_init( void );
extern void     tp_format_message( int, ... );
extern int	tp_type_compare_sort( TP_SCRIPTS *, TP_SCRIPTS *);
