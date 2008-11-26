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
*   Name:		ad/sql/vds.sql
*
*   Purpose:
*	This file contains the table and index definitions for
*       the vds table.
*
*   Database		: archive
*
*   Date		: August 31, 1998
*
*   Programmer		: Jennifer Dunn
*
*   Field SCCS data	: @(#)
*	Module Name	: vds.sql
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 01/25/99
*
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database archive"
use archive
go

if exists ( select name from sysobjects 
		where name = 'vds' and type = 'U' )
		begin 
		    print "Dropping existing vds table"
		    drop table vds
		end
go

print "Creating new vds table"
go

Create table vds
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

create clustered index vds_diskname 
	on vds ( diskname ) 
go

print "Granting"
go

grant insert, update, delete, select on vds to operator
go

grant select on vds to public
go

