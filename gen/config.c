/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Module Name:	gen/src/config.c
*
*   Purpose:
*	Configuration file processing routines.
*
*   Routines:
*	void		config_close	: Closes the configuration file.
*	int		config_get	: Gets config information for a keyword.
*	static void	config_line_free: Frees a config line structure.
*	int		config_open	: Opens the configuration file.
*	static int	file_open	: Finds and opens config file.
*	static int	gc_format_message
*					: Format a message.
*	static int	parse_file	: Parses the config file.
*
*   Date		: Apr 15, 1994
*
*   SCCS data		: @(#)
*	Module Name	: config.c
*	Version Number	: 1.12
*	Release Number	: 1
*	Last Updated	: 12/14/99
*
*   Programmer		: Norman Hill
*
*   Modification History:
*	99/12/06 nrh	: Changed to allow arbitrarily long lines in config
*			  files.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef vxWorks
#include <string.h>
#else
#include <memory.h>
#endif

#include "gen_types.h"
#include "gen_msg.h"
#include "gen_str.h"
#include "gen_eptr.h"
#include "gen_config.h"


typedef struct config_line
{
    char	*cl_line;	/* The line from the config file.	*/
    int		cl_line_num;	/* The line the data occurs on.		*/
    int		cl_num_tokens;	/* The number of tokens.		*/
    char	**cl_tokens;	/* The list of tokens.			*/
} CONFIG_LINE;
#define	CONFIG_LINE_ALLOC(n)	((CONFIG_LINE *)gen_alloc((n)*sizeof(CONFIG_LINE)))


#define	DEFAULT_DIR		"DEFAULT_CONFIG_DIR"
#define	CONFIG_MAX_TOKENS	64
#define	GC_PREFIX		"gc"


#define GC_CHECK_NULL(s)        { if ((s)==NULL) {                      \
                                    gc_format_message( GC_E_MEMORY );   \
                                    return( GC_E_MEMORY ); } }

#define GC_CHECK(s)             { int estatus;                          \
                                  if((estatus=(s))!=GC_SUCCESS)         \
                                        return(estatus); }


static EPTR	*config_line_list;	
				/* The list of lines in the file.	*/
static char	*file_name;	/* The name of the config file.		*/
char		gc_msg[MSG_MAX_LEN];
static MSG	gc_msgs[] = {
    { GC_E_MEMORY,
	"Memory allocation failure." },
    { GC_E_FILE_NOT_FOUND,
	"Could not find file %s.config in directories:\n%s" },
    { GC_E_KEY_NOT_FOUND,
	"Key %s is not in the configuration file." },
    { GC_E_PARSE,
	"Error parsing configuration file at line %d" },
};
static int	gc_num_msgs = sizeof( gc_msgs ) / sizeof( MSG );

static void	config_line_free( CONFIG_LINE *config_line );
static int	file_open( char *rootname, FILE **config_fp );
static void	gc_format_message( int, ... );
static int	parse_file( FILE *config_fp );



/*+
************************************************************************
*
*   Synopsis:
*	void	config_close()
*
*   Purpose:
*	Closes the configuration library.
*
*   Parameters:
*	none
*
*   Values Returned:
*	int	GC_SUCCESS	: Normal completion.
*
************************************************************************
-*/

void	config_close(void)
{
    if ( file_name != NULL )
    {
	gen_free( file_name );
    }

    if ( config_line_list != NULL )
    {
	eptr_free( config_line_list, config_line_free );
    }
}

/*+
************************************************************************
*
*   Synopsis:
*	int	config_get( key, fn )
*
*   Purpose:
*	Applys the config function fn to every line in the config file
*	with keyword = key.
*
*   Parameters:
*	char	*keyword	: (in)	keyword to look for.
*	pfb	fn		: (in)	Function to apply.
*
*   Values Returned:
*	int	GC_SUCCESS		: Normal completion.
*	int	GC_E_KEY_NOT_FOUND	: Could not find key.
*
************************************************************************
-*/

int	config_get( char *key, pfb fn )
{
    CONFIG_LINE	*config_line;
    int		i;
    boolean	found;


    for ( found = FALSE, i = 0; i < eptrlen( config_line_list ); i++ )
    {
	config_line = eptritem( config_line_list, i );

	if ( streq( key, config_line->cl_tokens[0] ) )
	{
	    if ( ! fn( config_line->cl_num_tokens, config_line->cl_tokens ) )
	    {
		gc_format_message( GC_E_PARSE, config_line->cl_line_num );
		return( GC_E_PARSE );
	    }
	    found = TRUE;
	}
    }

    if ( ! found )
    {
	gc_format_message( GC_E_KEY_NOT_FOUND, key );
	return( GC_E_KEY_NOT_FOUND );
    }
    else
    {
	return( GC_SUCCESS );
    }
}

/*+
************************************************************************
*
*   Synopsis:
*	static int	config_line_free( config_line )
*
*   Purpose:
*	Frees the memory allocated to a config line structure.
*
*   Parameters:
*	CONFIG_LINE	*config_line	: (in)	The config line to free.
*
*   Values Returned:
*	int	GC_SUCCESS	: Normal completion.
*
************************************************************************
-*/

static void	config_line_free( CONFIG_LINE *config_line )
{
    gen_free( config_line->cl_line );
    gen_free( config_line->cl_tokens );
    gen_free( config_line );
}

/*+
************************************************************************
*
*   Synopsis:
*	int	config_open( rootname, config_file )
*
*   Purpose:
*	Initializes the configuration file.
*
*   Parameters:
*	char	*rootname	: (in)	Program rootname used to calculate 
*					the config file name.
*	char	**config_file	: (out)	The name and full path of the config
*					file opened.
*
*   Values Returned:
*	int	GC_SUCCESS		: Normal completion.
*	int	GC_E_MEMORY		: Memory allocation failure.
*	int	GC_E_FILE_NOT_FOUND	: Could not find the file.
*
************************************************************************
-*/

int	config_open( char *rootname, char **config_file )
{
    FILE	*config_fp;

    file_name = NULL;
    config_line_list = NULL;


    GC_CHECK( file_open( rootname, &config_fp ) );

    if ( config_file != NULL )
    {
	*config_file = file_name;
    }


    /*
     *  Parse the configuration file.
     */

    GC_CHECK( parse_file( config_fp ) );

    (void) fclose( config_fp );

    return( GC_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*	static int	file_open( rootname, config_fp )
*
*   Purpose:
*	Opens the configuration file.
*
*   Parameters:
*	char	*rootname	: (in)	The rootname of the config file.
*	FILE	*config_fp	: (out)	Pointer to config file.
*
*   Values Returned:
*	int	GC_SUCCESS		: Normal completion.
*	int	GC_E_MEMORY		: Memory allocation failure.
*	int	GC_E_FILE_NOT_FOUND	: Could not find file.
*
************************************************************************
-*/

static int	file_open( char *rootname, FILE **config_fp )
{
    char	*tried_path;	/* List of directories checked.		*/
    struct stat	buf;		/* The stat buffer.			*/
    char	file[256];	/* base files name			*/
    char	symbol[256];	/* The default environment variable	*/
				/*  For the config file directory.	*/


    /*
     *  Figure out the config dir symbol name.
     */

    (void) sprintf( symbol, "%s_CONFIG_DIR", rootname );
    str2upper( symbol, symbol );


    (void) sprintf( file, "/%s.config", rootname );

    /*
     *  Try to find the configuration file.
     */

    
    *config_fp = NULL;

    /*
     *  Try to find the configuration file in a directory specifed for
     *  the application.
     */

    if ( getenv( symbol ) != NULL )
    {
	GC_CHECK_NULL( tried_path = strsav( getenv( symbol ) ) );
	GC_CHECK_NULL( tried_path = strapp( tried_path, " " ) );
	GC_CHECK_NULL( file_name = strsav( getenv( symbol ) ) );
	GC_CHECK_NULL( file_name = strapp( file_name, file ) );

	if ( stat( file_name, &buf ) == 0 && S_ISREG( buf.st_mode ) )
	{
	    *config_fp = fopen( file_name, "r" );
	}
    }
    else
    {
	GC_CHECK_NULL( tried_path = strsav( "" ) );
    }


    /*
     *  Try the default configuration directory.
     */

    if ( *config_fp == NULL && getenv( DEFAULT_DIR ) != NULL )
    {
	GC_CHECK_NULL( tried_path = strapp( tried_path, 
		getenv( DEFAULT_DIR ) ) );
	GC_CHECK_NULL( tried_path = strapp( tried_path, " " ) );
	GC_CHECK_NULL( file_name = strsav( getenv( DEFAULT_DIR ) ) );
	GC_CHECK_NULL( file_name = strapp( file_name, file ) );

	if ( stat( file_name, &buf ) == 0 && S_ISREG( buf.st_mode ) )
	{
	    *config_fp = fopen( file_name, "r" );
	}
    }


    /*
     *  Try the current directory.
     */

    if ( *config_fp == NULL )
    {
	GC_CHECK_NULL( tried_path = strapp( tried_path, "." ) );
	GC_CHECK_NULL( tried_path = strapp( tried_path, " " ) );
	GC_CHECK_NULL( file_name = strsav( "." ) );
	GC_CHECK_NULL( file_name = strapp( file_name, file ) );

	if ( stat( file_name, &buf ) == 0 && S_ISREG( buf.st_mode ) )
	{
	    *config_fp = fopen( file_name, "r" );
	}
    }


    if ( *config_fp == NULL )
    {
	gc_format_message( GC_E_FILE_NOT_FOUND, rootname, tried_path );
	gen_free( tried_path );
	return( GC_E_FILE_NOT_FOUND );
    }

    gen_free( tried_path );

    return( GC_SUCCESS );
}

/*+
************************************************************************
*
*   Synopsis:
*	static void	gc_format_message( status, ... )
*
*   Purpose:
*	Formats a message from the contents of the va_alist.
*
*   Parameters:
*	int	status		: (in)	The message status number.
*	...			: (in)  The message parameters.
*
*   Values Returned:
*	None
*
************************************************************************
-*/

/* VARARGS 1 */
static void	gc_format_message( 
    int status, 		/* (in)  The message status number.	*/
    ... 			/* (in)  The message parameters.	*/
)
{
    va_list	args;


    va_start( args, status );
    msg_format( gc_msg, GC_PREFIX, gc_num_msgs, gc_msgs,status, args );
    va_end( args );
}

/*+
************************************************************************
*
*   Synopsis:
*	static int	parse_file( config_fp )
*
*   Purpose:
*	Parses the contents of a configuration file.
*
*   Parameters:
*	FILE	*config_fp	: (in)	File pointer to the file.
*
*   Values Returned:
*	int	GC_SUCCESS	: Normal completion.
*	int	GC_E_MEMORY	: Memory allocation failure.
*
************************************************************************
-*/

static int	parse_file( FILE *config_fp )
{
    char	*buffer;
    char	inputline[STRSIZ];
    int		line;
    int		num_tokens;
    char	*tokens[CONFIG_MAX_TOKENS];
    CONFIG_LINE	*config_line;


    GC_CHECK_NULL( config_line_list = eptr_alloc( 10 ) );

    for ( line = 1; fgets( inputline, STRSIZ, config_fp ) != NULL; line++ )
    {
	buffer = strsav( inputline );
	while ( buffer[ strlen( buffer ) - 1 ] != '\n' )
	{
	    if ( fgets( inputline, STRSIZ, config_fp ) == NULL )
	    {
		break;
	    }
	    buffer = strapp( buffer, inputline );
	}
	    
	strtokens( buffer, CONFIG_MAX_TOKENS, tokens, &num_tokens );

	if ( num_tokens > 0 && tokens[0][0] != '#' )
	{
	    GC_CHECK_NULL( config_line = CONFIG_LINE_ALLOC(1) );
	    config_line->cl_line = buffer;
	    GC_CHECK_NULL( config_line->cl_tokens = 
		    (char **) ptr_alloc( num_tokens ) )
	    (void) memcpy( config_line->cl_tokens, tokens, 
		    num_tokens * sizeof( pointer ) );
	    config_line->cl_num_tokens = num_tokens;
	    config_line->cl_line_num = line;
	    GC_CHECK_NULL( eptr_insert( config_line_list, 
		    eptrlen( config_line_list ), config_line ) );
	}
	else
	{
	    gen_free( buffer );
	}
    }

    return( GC_SUCCESS );
}
