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
*   Name:		ad/sql/mdfiles.sql
*
*   Purpose:
*	This file contains the table and index definitions for
*       the mdfiles table.
*
*   Database		: archive
*
*   Date		: Dec 3, 1996
*
*   Programmer		: Marc LeBlanc
*
*   Field SCCS data	: @(#)
*	Module Name	: mdfiles.sql
*	Version Number	: 1.9
*	Release Number	: 1
*	Last Updated	: 01/18/99
*
*
*   Modification History:
*	97/01/29 SEC :	Removed references to segments, added grant 
*			statements, added prints.
*	97/06/20 SEC  : Mod'ed common_fname to file_id, added compression,
*			format. Tidy up a bit. 
*	97/09/23 pdd	: Changed to archive database
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

print "Creating new mdfiles table"
go

Create table mdfiles
(
	file_id		varchar(45)	not null,
	diskname	varchar(32)	not null,
	fname		varchar(223)	not null,
	fsize		int		not null,
	status		char(1)		not null,
	uncomp_fsize	int		not null,
	compression	char(1)		not null,
	format		char(1)		not null
)
go


print "Creating indices"
go

create unique clustered index mdfiles_diskname on mdfiles ( diskname, fname ) 
go

create index mdfiles_file_id on mdfiles ( file_id, status ) on index_seg
go


print "Granting"
go

grant insert, update, delete, select on mdfiles to operator
go

grant select on mdfiles to public
go


/*+
 *
 * enforces the invariant that <file_id,status=C> is unique
 *
-*/

print "Creating triggers"
go

create trigger unique_current_mdfile
on mdfiles
for insert, update as
	update mdfiles
	set status='C'
	from mdfiles f, inserted i
	where ( i.status = 'I' or i.status = 'E' )
	and f.status = 'I'
	and f.file_id = i.file_id

	update mdfiles
	set status='I'
	from mdfiles f1, mdfiles f2, inserted i
	where f1.status='C'
	and f2.status = 'C'
	and f1.file_id = i.file_id
	and f1.file_id = f2.file_id
	and (
		( f1.diskname < f2.diskname )
		or
		( f1.diskname = f2.diskname and f1.fname < f2.fname )
	)
go

/*+
 *  deletion trigger - just set status to C for that file_id and
 *  let the update trigger fiugre the rest out
-*/

create trigger mdfile_delete
on mdfiles
for delete as
	update mdfiles
	set status='C'
	from mdfiles f1, deleted d
	where d.status = 'C'
	and f1.file_id = d.file_id
	and f1.status = 'I'
go
