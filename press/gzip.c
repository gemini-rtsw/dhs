/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	press/src/gzip.c
*
*   Purpose:
* 	decompress files in gzip format.
*	Copyright (C) 1992-1993 Jean-loup Gailly
*	This is free software; you can redistribute it and/or modify it under 
*	the terms of the GNU General Public License, see the file COPYING.
*	This version can extract files in gzip format.
*
*	The code in this file is derived from the file funzip.c written
*	and put in the public domain by Mark Adler.
*
*   Routines:
*	int	gzip_comp	: Uncompresses gzipped data.
*	int	unzip		: Compress data into gzip format. 
*
*   Date		: June 21, 1993
*
*   SCCS data		: @(#)
*	Module Name	: gzip.c
*	Version Number	: 1.14
*	Release Number	: 1
*	Last Updated	: 10/18/99
*
*   Programmer		: Norman Hill
*
*   Modification History:
*	97/07/02 SEC  :	Bring up to ANSI C. 
*	98/05/05 SEC  :	Mod'ed vfork() call to use fork() instead. 
*			vfork's man-page says it is not thread-safe.
*	98/05/05 SEC  : And furthermore it appears fork1() is even
*			safer than fork().
*	99/04/20 NRH  : Replace the sleep in the gzip_comp routine with
*			a select, and simplified the algorithm to take
* 			advantage of select.
*	99/06/16 NRH  : Added a waitpid to get rid of zombie processes.
*			Added flockfile and funlock file around the file
*			descriptor manipulation hoping to avoid interaction
*			with stdin and stdout from other threads.
*	99/10/14 SDJ  : Changed back go fork(), fork1() is not defined
*			on HP systems.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/wait.h>

#if ! defined( _SOL ) && ! defined( lint )
#include <unistd.h>
#endif

#include "gen_types.h"
#include "gen_msg.h"

#include "press.h"
#include "local_press.h"
#include "gzip.h"

/* XXX allan: no fork1 on linux */
#ifdef linux
#define fork1 fork
#endif

unsigned	inptr;		/* Index of next byte to be processed 	*/
				/* in inbuf.				*/
unsigned	insize;		/* valid bytes in inbuf 		*/
pfi		char_in;	/* Function to read bytes.		*/
pfi		char_out;	/* Function to write bytes.		*/

long	bytes_out;		/* The number of output bytes.		*/

#define	EXTHDR	16		/* Size of extend local header.		*/

/*+
************************************************************************
*
*   Synopsis:
*	int	gzip_comp( char_in, char_out )
*
*   Purpose:
*	Statement of purpose.
*
*   Parameters:
*	int	(*char_in)()	: (in)	Function to get data from input.
*	int	(*char_out)()	: (in)	Function to write data to output.
*
*   Values Returned:
*	int	PR_SUCCESS	: Normal completion.
*	int	PR_E_IO		: Error during io.
*
************************************************************************
-*/

int	gzip_comp
( 
    pfi		char_in,	/* (in)  Func. to get data from input.	*/
    pfi		char_out	/* (in)  Func. to put data to output.	*/
)
{
    /* void	pr_format_message(); */

    byte	out_buffer[4096];
    byte	in_buffer[4096];
    byte	*buf_ptr;
    int		in_pipe[2];
    int		bytes_left;
    int		bytes_read;
    int		bytes_written;
    int		out_pipe[2];
    int		tmp_stdin;
    int		tmp_stdout;
    int		pid;
    fd_set	readFds;
    fd_set	writeFds;
    int		execStatus;


    flockfile( stdin );
    flockfile( stdout );
    tmp_stdin =  dup( 0 );
    tmp_stdout =  dup( 1 );
    PR_CHECK_IO( pipe( in_pipe ), "in pipe" );
    PR_CHECK_IO( pipe( out_pipe ), "out pipe" );
    (void) dup2( in_pipe[1], 1 );
    (void) dup2( out_pipe[0], 0 );

    if ( ( pid = fork1() ) == 0 )
    {
	(void) close( in_pipe[0] );
	(void) close( out_pipe[1] );
	(void) execlp( "gzip", "gzip", "-c", NULL );
	pr_format_message( MSG_ERRNO, "gzip" );
	_exit( 999 );
    }
    PR_CHECK_IO( pid, "fork" );

    (void) close( in_pipe[1] );
    (void) close( out_pipe[0] );

    (void) dup2( tmp_stdin, 0 );
    (void) dup2( tmp_stdout, 1 );
    (void) close( tmp_stdin );
    (void) close( tmp_stdout );
    funlockfile( stdin );
    funlockfile( stdout );
    PR_CHECK_IO( fcntl( out_pipe[1], F_SETFL, O_WRONLY ), "fcntl" );

    while ( ( bytes_left = char_in( out_buffer, 4096 ) ) != PR_E_EOI )
    {
	buf_ptr = out_buffer;
	do  
	{
	    FD_ZERO( &readFds );
	    FD_ZERO( &writeFds );
	    FD_SET( in_pipe[0], &readFds );
	    FD_SET( out_pipe[1], &writeFds );
	    PR_CHECK_IO( select( MAX( out_pipe[1], in_pipe[0] ) + 1, 
		    &readFds,  &writeFds, NULL, NULL ), "select" );

	    if ( FD_ISSET( out_pipe[1], &writeFds ) )
	    {
		PR_CHECK_IO( ( bytes_written = 
			write( out_pipe[1], buf_ptr, bytes_left ) ), 
			"write gzip" );
		buf_ptr += bytes_written;
		bytes_left -= bytes_written;
	    }


	    if ( FD_ISSET( in_pipe[0], &readFds ) )
	    {
		bytes_read = read( in_pipe[0], in_buffer, 4096 );
		if ( bytes_read > 0 )
		{
		    PR_CHECK( char_out( in_buffer, bytes_read ) );
		}
	    }
	} while ( bytes_left > 0 );
    }
 
    close( out_pipe[1] );
    do
    {
        bytes_read = read( in_pipe[0], in_buffer, 4096 );
        if ( bytes_read > 0 )
        {
            PR_CHECK( char_out( in_buffer, bytes_read ) );
        }
    } while ( bytes_read > 0 );
    close( in_pipe[0] );
    waitpid( pid, &execStatus, 0 );

    return( PR_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*	int	unzip( p_char_in, p_char_out )
*
*   Purpose:
*	Function to uncompress gzip files.
*
*   Parameters:
*	int	(*p_char_in)()	: (in)	Function to get the next input 
*					character.
*	int	(*p_char_out)()	: (in)	Function to send data to the output.
*
*   Values Returned:
*	int	PR_SUCCESS	: Normal completion.
*	int	PR_E_EOI	: End of input detected.
*	int	PR_E_MAGIC	: Invalid magic number.
*	int	PR_E_METHOD	: Unknown compression method.
*
************************************************************************
-*/

int gzip_uncomp
( 
    pfi		p_char_in,	/* (in)  Func. to get char from input.	*/
    pfi		p_char_out	/* (in)  Func. to put char to output.	*/
)
{
    /* void	pr_format_message(); */

    unsigned char
    		buff[EXTHDR];	/* extended local header 	*/
    char	c;
    byte	dummy[6];
    byte	flags;		/* Compression flags.		*/
    byte	magic[2];	/* The magic number.		*/
    byte	method;		/* The compress			*/
    unsigned long
    		orig_crc;	/* original crc			*/
    unsigned long
    		orig_len;   	/* original uncompressed length */


    char_in = p_char_in;
    char_out = p_char_out;


    orig_crc = 0;
    orig_len = 0;


    /*
     *  Check the magic number and compression type.
     */

    PR_CHECK( char_in( magic, 2 ) );
    if ( memcmp( magic, GZIP_MAGIC, 2 ) != 0 )
    {
	pr_format_message( PR_E_MAGIC );
	return( PR_E_MAGIC );
    }


    PR_CHECK( char_in( &method, 1 ) );
    if ( method != DEFLATED )
    {
	pr_format_message( PR_E_METHOD, method );
	return( PR_E_METHOD );
    }

    PR_CHECK( char_in( &flags, 1 ) );
    if ( flags & ENCRYPTED || flags & CONTINUATION || flags & RESERVED )
    {
	pr_format_message( PR_E_UNSUPPORT, "" );
	return( PR_E_UNSUPPORT );
    }


    /*
     *  Skip over time stamp, extra flags, and os.
     */

    PR_CHECK( char_in( dummy, 6 ) );


    if ( ( flags & EXTRA_FIELD ) != 0 )
    {
	/*
	 *  Skip the file length.
	 */

	PR_CHECK( char_in( dummy, 2 ) );
    }


    /*
     *  Skip over the original file name.
     */

    if ( ( flags & ORIG_NAME ) != 0 )
    {
	do
	{
	    PR_CHECK( char_in( &c, 1 ) );
	} while ( c != '\0' );
    }


    /*
     *  skip over the comment.
     */

    if ( ( flags & COMMENT ) != 0 )
    {
	do
	{
	    PR_CHECK( char_in( &c, 1 ) );
	} while ( c != '\0' );
    }


    /* 
     * initialize crc 
     */

    (void) updcrc(NULL, 0);

    /* 
     * Decompress 
     */

    PR_CHECK( inflate() );


    /* 
     *  Get the crc and original length 
     */

    /* 
     * crc32  (see algorithm.doc)
     * uncompressed input size modulo 2^32
     */

    PR_CHECK( char_in( buff, 8 ) );
    orig_crc = LG(buff);
    orig_len = LG(buff+4);


    /* 
     * Validate decompression 
     */

#ifdef OMIT
    if ( orig_crc != updcrc(buff, 0)) 
    {
	pr_format_message( PR_E_CRC );
    }
#endif
    if (orig_len != bytes_out) 
    {
	pr_format_message( PR_E_SIZE );
    }

    return( PR_SUCCESS );
}
