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
*   Name:		ad/sql/cds.sql
*
*   Purpose:
*	This file contains the table and index definitions for
*       the cds table.
*
*   Database		: archive
*
*   Date		: Dec 3, 1996
*
*   Programmer		: Marc LeBlanc
*
*   Field SCCS data	: @(#)
*	Module Name	: cds.sql
*	Version Number	: 1.7
*	Release Number	: 1
*	Last Updated	: 09/24/97
*
*
*   Modification History:
*	97/01/29 SEC  :	Added 'print' statements, changed 'grant' statements.
*	97/06/20 SEC  :	Added back in diskname_orig, Tidy up a bit.
*	97/09/23 pdd	: Changed to archive database
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database archive"
use archive
go

if exists ( select name from sysobjects 
		where name = 'cds' and type = 'U' )
		begin 
		    print "Dropping existing cds table"
		    drop table cds
		end
go

print "Creating new cds table"
go

Create table cds
(
	archive		char(6)		null,
	creation_date	datetime	not null,
	diskname	varchar(32)	not null,
	diskname_orig	varchar(32)	not null,
	ingest_date	datetime	not null,
	mounted		char(1)		not null,
	mount_point	varchar(255)	not null,
	nfiles		int		not null,
	nkbytes		int		not null,
	type		char(10)	not null,
	verified	char(1)		not null 
)
go

print "Creating indices"
go

create clustered index cds_diskname 
	on cds ( diskname ) 
go

print "Granting"
go

grant insert, update, delete, select on cds to operator
go

grant select on cds to public
go

