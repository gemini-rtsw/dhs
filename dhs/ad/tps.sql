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
*   Name:		ad/sql/tps.sql	
*
*   Purpose:
*       This file contains the table and index definitions for
*       the tps table.
*
*   Database		: archive 
*
*   Date		: Mar 5, 1998 
*
*   SCCS data		: @(#)
*	Module Name	: tps.sql
*	Version Number	: 1.2
*	Release Number	: 1
*	Last Updated	: 04/23/98
*
*   Programmer		: George Zhang 
*
*   Modification History:
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database archive"
use archive 
go

print "Creating tps table"
go

create table tps
(
	archive         char(6)         not null,
	tapename        varchar(32)     not null,
	ingest_date     datetime        not null,
	nfiles          int             not null,
	nkbytes         int             not null,
	type		char(10)	not null,
	script		char(10)	not null,
	verified        char(1)         not null
)
go

print "Creating indices"
go

create unique clustered index tps_tapename on tps ( tapename, archive )
go

print "Granting"
go

grant insert, update, delete, select on tps to operator
go

grant select on tps to public
go
