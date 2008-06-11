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
*   Name:		ad/sql/odfiles.sql
*
*   Purpose:
*	This file contains the table and index definitions for
*       the odfiles table.
*
*   Database		: archive
*
*   Date		: Dec 3, 1996
*
*
*   Field SCCS data	: @(#)
*	Module Name	: odfiles.sql
*	Version Number	: 1.10
*	Release Number	: 1
*	Last Updated	: 01/18/99
*
*   Programmer		: Marc LeBlanc
*
*   Modification History:
*	97/01/29 SEC :	Removed references to segments, added prints, 
*			added grant statements. 
*	97/06/20 SEC  : Mod'ed fname to file_id, added compression,
*			format. Tidy up a bit. Added fname.
*	97/09/23 pdd	: Changed to archive database
*	97/11/24 pdd	: changed file_id index to clustered
*	97/11/24 pdd	: changed order of fname and status in table
*			  to be the same as current tables
*	98/11/12 pdd	: Added trigger to enforce uniqueness of
*			  <file_id,status=C> pair by setting non-current
*			  to status == I
*	98/12/08 pdd	: extended triggers to handle update, delete,
*			  and mutliple rows
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database archive"
use archive
go

print "Creating new odfiles table"
go

Create table odfiles
(
	file_id		varchar(45)	not null,
	diskname	varchar(32)	not null,
	fsize		int		not null,
	dirsectoff	int		not null,
	status		char(1)		not null,
	fname		varchar(223)	not null,
	compression	char(1)		not null,
	format		char(1)		not null
)
go


print "Creating indices"
go

create unique clustered index odfiles_diskname on odfiles ( diskname, dirsectoff ) 
go

create index odfiles_file_id on odfiles ( file_id, status ) on index_seg
go


print "Granting"
go

grant insert, update, delete, select on odfiles to operator
go

grant select on odfiles to public
go

/*+
 *
 * enforces the invariant that <file_id,status=C> is unique
 *
-*/

print "Creating triggers"
go

create trigger unique_current_odfile
on odfiles
for insert, update as
	update odfiles
	set status='C'
	from odfiles f, inserted i
	where ( i.status = 'I' or i.status = 'E' )
	and f.status = 'I'
	and f.file_id = i.file_id

	update odfiles
	set status='I'
	from odfiles f1, odfiles f2, ods s1, ods s2, inserted i
	where f1.status='C'
	and f2.status = 'C'
	and f1.file_id = i.file_id
	and f1.file_id = f2.file_id
	and f1.diskname = s1.diskname
	and f2.diskname = s2.diskname
	and (
		( s1.creation_date < s2.creation_date )
		or
		( s1.creation_date = s2.creation_date and f1.fname < f2.fname )
	)
go

/*+
 *  deletion trigger - just set status to C for that file_id and
 *  let the update trigger fiugre the rest out
-*/

create trigger odfile_delete
on odfiles
for delete as
	update odfiles
	set status='C'
	from odfiles f1, deleted d
	where d.status = 'C'
	and f1.file_id = d.file_id
	and f1.status = 'I'
go
