/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
*   Include File Name:	gen/h/gen_config.h
*
*   Purpose:
*	Header file for gen_config.
*
*   Date		: Apr 15, 1994
*
*   SCCS data		: @(#)
*	Module Name	: gen_config.h
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 03/25/97
*
*   Programmer		: Norman Hill
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/


#ifndef _gen_config_h
#define _gen_config_h

/*
 *  Status symbol definitions.
 */

#define	GC_SUCCESS		0	/* Normal completion.		*/

#define	GC_E_MEMORY		-1	/* Memory allocation failure.	*/
#define	GC_E_FILE_NOT_FOUND	-2	/* Config file not found.	*/
#define GC_E_KEY_NOT_FOUND	-3	/* Specified key not found.	*/
#define	GC_E_PARSE		-4	/* Error parsing config file.	*/

typedef		boolean (*get_fn)( int, char **);
extern  void    config_close(void);
extern  int     config_get(char *key, get_fn fn);
extern  int     config_open(char *rootname, char **config_file);

extern char	gc_msg[];

#endif /* _gen_config_h */
