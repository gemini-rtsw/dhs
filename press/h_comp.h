/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	press/h/h_comp.h
*
*   Purpose:
*	Header file for the h compress routines.
*
*   Date		: Mar 15, 1993
*
*   SCCS data		: @(#)
*	Module Name	: h_comp.h
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 07/04/97
*
*   Programmer		: Norman Hill
*
*   Modification History:
*	97/07/02 SEC  :	Mod'ed for clean compile. 
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

static	byte	code_magic[2] = { 0xDD, 0x99 };

#define	read_int( char_in, data )	char_in( (byte *) data, sizeof( int ) )
#define	write_int( char_out, data )	char_out( (byte *) data, sizeof( int ) )
