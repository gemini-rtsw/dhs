/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
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
*   Include File Name:	crc/h/crc.h
*
*   Purpose:
*	Contains the declaration of function crc_buffer: the C wrapper
*	to the C++ 32 bit crc class.
*
*   Date		: Nov 1, 1999
*
*   SCCS data		: %Z%
*	Module Name	: %M%
*	Version Number	: %I%
*	Release Number	: %R%
*	Last Updated	: %G%
*
*   Programmer		: Norm Hill
*
*   Modification History:
*	99/12/02 nrh	: Added function crc_buffer64.
*	00/04/06 nrh	: Added functions crc322str and crc642str.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#ifdef __cplusplus
extern "C"
{
#endif
    typedef	unsigned long		crc32_t;
    typedef	unsigned long long	crc64_t;

    crc32_t crc_buffer( unsigned int, const char * );
    crc64_t crc_buffer64( unsigned int, const char * );
    char 		*crc322str( crc32_t, char * );
    char 		*crc642str( crc64_t, char * );
#ifdef __cplusplus
}
#endif
