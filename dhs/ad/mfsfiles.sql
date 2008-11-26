/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 2000				(c) 2000
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
*   Name:		ad/sql/mfsfiles.sql
*
*   Purpose:
*	This file contains the table and index definitions for
*       the mfsfiles table.
*
*   Database		: archive
*
*   Date		: Mar 09, 2000
*
*   Programmer		: Shannon Jaeger
*
*   SCCS data		: @(#)
*	Module Name	: mfsfiles.sql
*	Version Number	: 1.6
*	Release Number	: 1
*	Last Updated	: 03/26/01
*
*   Modification History:
*    2000/03/09	SDJ	: Copied from cdfiles.sql
*    2000/04/05 SDJ	: Fixed typo, "archvie" instead of "archive".
*    2000/04/10 SDJ	: Switch crc from int to binary data type.
*    2001/02/15 SDJ	: Changed file_id and fname to varchar(255).
*			  Added file_crc field.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsDB_NB"
use dhsDB_NB
go

print "Creating new mfsfiles table"
go

Create table mfsfiles
(
	crc		binary(4)	not null,
	file_id		varchar(255)	not null,
	fname		varchar(255)	not null,
	fsize		int		not null,
	file_crc	binary(4)	null,
	format		char(1)		not null,
	compression	char(1)		not null,
	uncomp_fsize	int		not null, 
	status		char(1)		not null,
)
go

print "Creating indices"
go

create unique clustered index mfsfiles_crc 
	on mfsfiles ( crc, fname ) 
create index mfsfiles_file_id 
	on mfsfiles ( file_id, status ) 
go

print "Granting"
go

grant select on mfsfiles to public
go


/*+
 *
 *  enforces the invariant that <file_id, status=C> is unique
 *
-*/

print "Creating triggers"
go

create trigger unique_current_mfsfile
on mfsfiles
for insert, update as
    update mfsfiles
    set status='C'
    from mfsfiles f, inserted i
    where ( i.status = 'I' or i.status = 'E' )
    and f.status = 'I'
    and f.file_id = i.file_id

    update mfsfiles
    set status='I'
    from mfsfiles f1, mfsfiles f2, mfs s1, mfs s2, inserted i
    where f1.status='C'
    and f2.status = 'C'
    and f1.file_id = i.file_id
    and f1.file_id = f2.file_id
    and f1.crc = s1.crc
    and f2.crc = s2.crc
    and s1.archive = s2.archive
    and
    (
	(
	    s1.creation_date < s2.creation_date
	)
	or
	(
	    s1.creation_date = s2.creation_date 
		    and s1.volume < s2.volume
	)
	or
	(
	    s1.creation_date = s2.creation_date 
		    and s1.volume = s2.volume
		    and f1.fname < f2.fname 
	)
    )
go

/*+ 
 *
 *  deletion trigger - just set status to C for that file_id and
 *  let the update trigger figure the rest out
 *
-*/
 
create trigger mfsfile_delete
on mfsfiles
for delete as
	update mfsfiles
	set status='C'
	from mfsfiles f1, deleted d
	where d.status = 'C'
	and f1.file_id = d.file_id
	and f1.status = 'I'
go
