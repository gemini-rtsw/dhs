/*+
************************************************************************
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
*
* (c) 1998.				(c) 1998.
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
*   Name:		sizes_db.sql
*
*   Purpose:
*	To create a table for tracking the growth of a list of databases 
*
*   Database		: cadctemp
*
*   Date		: Jan, 08, 1999
*
*   SCCS data		: @(#)
*	Module Name	: sizes_db.sql
*	Version Number	: 1.1
*	Release Number	: 1
*	Last Updated	: 01/25/99
*
*   Programmer		: David Delisle
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database cadctemp"
use cadctemp
go

print "Creating sizes_db table"
create table sizes_db
(
    name        varchar(30)  not null,
    date        datetime     not null,
    database_sz int          not null,
    reserved_sz int          not null,
    data_sz     int          not null,
    index_sz    int          not null,
)
go

print "Granting permissions on table sizes_db"
grant select
on sizes_db
to public
go

print "Creating indices on table sizes_db"
create unique clustered index sizes_db_index
on sizes_db(name, date)
go
