/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1996				(c) 1996.
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
*   Name:		ad/sql/dbfiles.sql
*
*   Purpose:
*	This file contains the table and index definitions for
*       the dbfiles table.
*
*   Database		: archive
*
*   Date		: Dec 3, 1996
*
*   Programmer		: Marc LeBlanc
*
*   Field SCCS data	: @(#)
*	Module Name	: dbfiles.sql
*	Version Number	: 1.8
*	Release Number	: 1
*	Last Updated	: 01/18/99
*
*
*   Modification History:
*	97/01/29 SEC :	Removed references to segments, added prints,
*			Added grant statements. Removed extra 'go'.
*	97/06/20 SEC  : Mod'ed common_fname to file_id, added compression,
*			format. Tidy up a bit. 
*	97/09/23 pdd	: Changed to archive database
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database archive"
use archive
go

print "Creating new dbfiles table"
go

Create table dbfiles
(
	file_id		varchar(45)	not null,
	volume		varchar(32)	not null,
	fname		varchar(223)	not null,
	fsize		int		not null,
	uncomp_fsize	int		not null, 
	compression	char(1)		not null,
	format		char(1)		not null
)
go


print "Creating indices"
go

create clustered index dbfiles_volume on dbfiles ( volume )
go

create index dbfiles_file_id on dbfiles ( file_id ) 
go

print "Granting"
go

grant insert, update, delete, select on dbfiles to operator
go

grant select on dbfiles to public
go

