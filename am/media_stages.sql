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
*   Name:		am/sql/media_stages.sql
*
*   Purpose:
*	The table definition, indexes, & permission granting for the
*	media_stages table. 
*
*   Database		: dhsTestDB
*
*   Date		: February 4, 1998
*
*   SCCS data		: @(#)
*	Module Name	: media_stages.sql
*	Version Number	: 1.3
*	Release Number	: 1
*	Last Updated	: 09/14/98
*
*   Programmer		: Steve Cockayne
*
*   Modification History:
*	98/02/04 SEC  :	Entire library copied from VLT development tree 
*			to CADC development tree. Initial check-in.
*	98/04/07 SEC  : Modifications for readability.
*
****  C A N A D I A N   A S T R O N O M Y   D A T A   C E N T R E  *****
************************************************************************
-*/

print "Using database dhsTestDB"
use dhsTestDB 
go

print "Creating new media_stages table"
go

if exists ( select name from sysobjects 
		where name = 'media_stages' and type = 'U' )
		begin 
		    print "Dropping existing media_stages table"
		    drop table media_stages
		end
go

create table media_stages
(
	directory 		varchar(32) 	not null,
	media_request_id 	int 		null,
	media_unit_name 	varchar(32) 	null
)
go

print "Creating indexes"
go

create unique index ms_dir_index on media_stages
(
	directory
)
go

create unique index ms_id_name_index on media_stages
(
	directory,
	media_request_id,
	media_unit_name
)
go

print "Granting"
go

#grant insert, update, delete, select on media_stages to operator
#go

grant select on media_stages to public
go

insert media_stages values ( "staging0", null, null )
insert media_stages values ( "staging1", null, null )
insert media_stages values ( "staging2", null, null )
go

