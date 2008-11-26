/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1997.				(c) 1997.
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
*   Name:		remote_files.sql
*
*   Purpose:
*	Creation of the ad library's master files table and creation
*	of its indices and granting of its privileges. 
*
*   Database		: archive
*
*   Date		: Jun 11, 1997
*
*   SCCS data		: @(#)
*	Module Name	: remote_files.sql
*	Version Number	: 1.4
*	Release Number	: 1
*	Last Updated	: 09/23/97
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	97/08/25 SEC  :	Mod'ed server_id to site_id, added unique index.
*			Changed name of table to remote_files.sql
*	97/09/23 pdd	: Changed to archive database
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database archive"
use archive
go

if exists ( select name from sysobjects
		where name = 'remote_files' and type = 'U' )
		begin
		    print "Dropping existing remote_files table"
		    drop table remote_files
		end
go

print "Creating new remote_files table"
go

create table remote_files
(
	file_id		varchar(45)	not null,
	media_id	varchar(32)	not null,
	site_id		varchar(3)	not null,
	status		char(1)		not null,
	compression	varchar(32)	not null,
	format		varchar(32)	not null
)
go

print "Creating indices on remote_files table"
go

create unique index remote_files_unique on remote_files ( 
	file_id, media_id, site_id, status )
go

create index remote_files_file_id on remote_files ( file_id, status )
go

print "Granting privileges on remote_files table"
go

grant insert, update, delete, select on remote_files to operator
go

grant select on remote_files to public
go

